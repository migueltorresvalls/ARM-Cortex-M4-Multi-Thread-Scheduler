/** @file   syscall_thread.c
 *  @note   put a better file header here
**/

#include <mpu.h>
#include <syscall_mutex.h>
#include <syscall_thread.h>
#include <svc_handler.h>
#include <unistd.h>
#include <timer.h>
#include <arm.h>
#include <stdio.h>
#include <stdint.h>
#include <printk.h>
#include <syscall.h>

/** @brief      Initial XPSR value, all 0s except thumb bit. */
#define XPSR_INIT 0x1000000

/** @brief Return code to return to user mode with user stack. */
#define LR_RETURN_TO_USER_PSP 0xFFFFFFFD

/** @brief Maximum number of threads */
#define MAXIMUM_THREADS 16

/** @brief Position of idle thread in threads list */
#define IDLE_THREAD_INDEX 14

/** @brief Position of main thread in threads list */
#define MAIN_THREAD_INDEX 15

/** @brief Maximum number of mutexes */
#define MAXIMUM_MUTEXES 32

/** @brief thread user stack region limit */
extern char __thread_u_stacks_limit;
/** @brief thread kernl stack region limit */
extern char __thread_k_stacks_limit;

/** @brief thread user stack region base */
extern char __thread_u_stacks_base;
/** @brief thread kernel stack region base  */
extern char __thread_k_stacks_base;

/** @brief main thread stack region base */
extern char __psp_stack_base;
/** @brief main thread stack region limit */
extern char __psp_stack_limit;
/** @brief kernel stack region base */
extern char __msp_stack_base;
/** @brief kernel stack region limit */
extern char __msp_stack_limit;

/** @brief Function executed forcibly to kill the thread or executed when thread is done executing */
extern void thread_kill(void);

/** @brief Default idle thread that runs in absence of idle thread given by user */
extern void default_idle(void);


/** @brief precomputed values for UB test */
float ub_table[] = {
    0.000, 1.000,  .8284, .7798, .7568,
     .7435, .7348, .7286, .7241, .7205,
     .7177, .7155, .7136, .7119, .7106,
     .7094, .7083, .7075, .7066, .7059,
     .7052, .7047, .7042, .7037, .7033,
     .7028, .7025, .7021, .7018, .7015,
     .7012, .7009
};

/** @brief memory addresses of user stacks*/
uint32_t u_stacks[MAXIMUM_THREADS];
/** @brief memory addresses of kernel stacks*/
uint32_t k_stacks[MAXIMUM_THREADS];

/** @brief array containing all threads*/
tcb_t threads_tcb[MAXIMUM_THREADS];

/** @brief index of current running thread*/
uint32_t thread_i = 0;
/** @brief maximum number of threads*/
uint32_t maximum_threads = 0;
/** @brief number of active threads*/
uint32_t active_threads = 0;
/** @brief variable that indicates if main thread is running*/
uint32_t main_thread_running = 1; 
/** @brief total number of threads*/
uint32_t thread_count = 0;

/** @brief counts miliseconds using systick timer*/
uint32_t sys_tick_period_ms = 1;
/** @brief counts systick ticks*/
uint32_t sys_tick_counter = 0;

/** @brief maximum number of mutexes*/
uint32_t maximum_mutexes = 0;
/** @brief total number of mutexes*/
uint32_t mutex_count = 0;
/** @brief array containing all mutexes*/
kmutex_t mutexes[MAXIMUM_MUTEXES];

/** @brief mpu mode: either KERNEL_ONLY or PER_THREAD */
mpu_mode mpu_type;  
/** @brief size of user and kernel thread's stack */
uint32_t stacks; 

/** @brief function to find maximum value between two inputs
 *  @param a input number 1
 *  @param b input number 2
 *  @return maximum value between a and b
 */
int max(int a, int b){
    return a > b ? a : b; 
}

/** @brief iterates through mutexes array and returns maximum priority ceiling value
 *  @return maximum priority ceiling
 */
uint32_t get_max_ceiling() {
    uint32_t max_prio = UINT32_MAX;
    for (int i=0; i < MAXIMUM_MUTEXES; i++) {
        if (mutexes[i].prio_ceil < max_prio && mutexes[i].lock_var == 1) {
            max_prio = mutexes[i].prio_ceil;
        }
    }
    return max_prio; 
}

/** @brief iterates through mutexes array and returns index of maximum priority ceiling
 *  @return index of max priority ceiling
 */
int get_max_ceiling_idx() {
    uint32_t max_prio = UINT32_MAX;
    int max_prio_idx = -1;
    for (int i=0; i < MAXIMUM_MUTEXES; i++) {
        if (mutexes[i].prio_ceil < max_prio && mutexes[i].lock_var == 1) {
            max_prio = mutexes[i].prio_ceil;
            max_prio_idx = i;
        }
    }
    return max_prio_idx;
}

/** @brief returns index of next runnable thread
 *  @return index for thread_tcb array
 */
uint32_t get_max_priority_thread_index() {
    uint32_t max_prio = UINT32_MAX;
    uint32_t max_prio_idx = UINT32_MAX;

    for (int i=0; i < IDLE_THREAD_INDEX; i++) {
        if (threads_tcb[i].dynamic_priority < max_prio && threads_tcb[i].status == RUNNABLE && threads_tcb[i].blocked_thread == -1) {
            max_prio_idx = i;
            max_prio = threads_tcb[i].priority;
        }
    }

    if (max_prio_idx == UINT32_MAX) {
        for (int i=0; i < IDLE_THREAD_INDEX; i++) {
            if (threads_tcb[i].status == WAITING && threads_tcb[i].msp != NULL) {
                return IDLE_THREAD_INDEX;
            }
        }
        return MAIN_THREAD_INDEX;
    }
    return max_prio_idx;
}

/** @brief wakes up threads that have been waiting. Makes them runnable */
void wake_up_threads() {
    uint32_t ticks = (sys_tick_counter/sys_tick_period_ms);
    for (int i=0; i < IDLE_THREAD_INDEX; i++) {
        if (ticks % threads_tcb[i].T == 0 && threads_tcb[i].status != DONE) {
            threads_tcb[i].status = RUNNABLE;
        }
    }
}

/** @brief systick timer interrupt c handler */
void systick_c_handler() {
    sys_tick_counter++;
    
    if (sys_tick_counter % sys_tick_period_ms == 0) {
        threads_tcb[thread_i].exec_time++;
        threads_tcb[thread_i].total_ticks++;

        if (threads_tcb[thread_i].exec_time == threads_tcb[thread_i].C && thread_i != IDLE_THREAD_INDEX) {            
            threads_tcb[thread_i].status = WAITING;
            threads_tcb[thread_i].exec_time = 0;
        }

        wake_up_threads();
        pend_pendsv();
    }
    return;
}

/** @brief switches context from one thread to another
 *  @param kernel_stack actual kernel stack 
 *  @param new_thread_i index of new runnable thread
 */
void switch_context(stack_frame_m* kernel_stack, uint32_t new_thread_i){
    if (main_thread_running) {
        main_thread_running = 0;
        thread_i = MAIN_THREAD_INDEX;
    }
    // Save context of currently running thread
    threads_tcb[thread_i].msp = kernel_stack; 

    // Save SVC status of currently running thread
    threads_tcb[thread_i].SVC_status = get_svc_status();

    // Restore SVC status of new thread
    set_svc_status(threads_tcb[new_thread_i].SVC_status);
    
    // Restore context of new thread
    kernel_stack = threads_tcb[new_thread_i].msp; 

    // Update currently running thread index
    thread_i = new_thread_i; 

    // Update kernel mpu
    mm_region_disable(7); 
    mm_region_enable(7, (void *)k_stacks[thread_i]-stacks, mm_log2ceil_size(stacks), 0, 1);
    // Upadte user mpu
    if (mpu_type == PER_THREAD){
        mm_region_disable(6);
        mm_region_enable(6, (void *)u_stacks[thread_i]-stacks, mm_log2ceil_size(stacks), 0, 1);
    }
}   

/** @brief pensv c handler to manage context switching
 *  @param msp pointer to kenel's stack
 */
void *pendsv_c_handler(void *msp) {
    stack_frame_m* kernel_stack = (stack_frame_m *)msp;

    int priority_thread_i = get_max_priority_thread_index();

    if (priority_thread_i == MAIN_THREAD_INDEX) {
        systick_stop();
    }

    switch_context(kernel_stack, priority_thread_i);

    return (void *)threads_tcb[priority_thread_i].msp; 
}

/** @brief constructs thread's control block
 *  @param fn pointer to thread's code
 *  @param prio thread's priority
 *  @param C thread's worst case execution time
 *  @param T thread's period
 *  @param vargp thread's arguments
 *  @param thread_index thread index
 *  @return thread control block struct containing the necessary information for thread's initialization and context switching
 */
tcb_t construct_tcb(void *fn, uint32_t prio, uint32_t C, uint32_t T, void *vargp, uint32_t thread_index) {
    stack_frame_m *kernel_stack;
    stack_frame_p *user_stack;

    if (thread_index != MAIN_THREAD_INDEX) {
        user_stack = (stack_frame_p *)(u_stacks[thread_index]-32);
        user_stack->r0 = (uint32_t)vargp;
        user_stack->lr = (uint32_t)&thread_kill;
        user_stack->xPSR = XPSR_INIT;
        user_stack->pc = (uint32_t)fn;

        kernel_stack = (stack_frame_m *)(k_stacks[thread_index]-40);
        kernel_stack->psp = user_stack; 
        kernel_stack->lr = LR_RETURN_TO_USER_PSP; 
    } else {
        kernel_stack = (stack_frame_m *)((uint32_t)&__msp_stack_base-40);
        kernel_stack->psp = (stack_frame_p *)&__psp_stack_base-32;
    }
    
    tcb_t thread = {
        .blocked_thread = -1,
        .blocked_mutex = -1,
        .status = RUNNABLE,
        .C = C,
        .T = T,
        .priority = prio,
        .dynamic_priority = prio,
        .msp = kernel_stack,
        .SVC_status = 0,
        .exec_time = 0,
        .total_ticks = 0,
    };
    return thread;
}

/** @brief allocates space for user, kernel stacks, mutexes and initializes memory protection unit for a given number of threads and mutexes
 *  @param max_threads maximum number of threads
 *  @param stack_size size of each user and kernel stacks
 *  @param idle_fn pointer to idle thread's function
 *  @param mem_protect memory protection type. Either Kernel_Only or Per_Thread
 *  @param max_mutexes maximum number of mutexes
 *  @return -1 if there is not enough space to allocate stacks and 0 otherwise
*/
int sys_thread_init(uint32_t max_threads, uint32_t stack_size, void *idle_fn, mpu_mode mem_protect, uint32_t max_mutexes) {
    mpu_type = mem_protect; 

    maximum_mutexes = max_mutexes;
    maximum_threads = max_threads;

    uint32_t bits = mm_log2ceil_size(stack_size*4);
    stacks = max(2*1024, 1<<bits);

    uint32_t u_thread_pos = (uint32_t)&__thread_u_stacks_base;
    uint32_t k_thread_pos = (uint32_t)&__thread_k_stacks_base;

    uint32_t u_thread_limit = (uint32_t)&__thread_u_stacks_limit;
    uint32_t k_thread_limit = (uint32_t)&__thread_k_stacks_limit;

    if ((u_thread_pos - stacks*max_threads >= u_thread_limit) && (k_thread_pos - stacks*max_threads >= k_thread_limit)){
        for (uint32_t i=0; i < max_threads; i++) {
            u_stacks[i] = u_thread_pos; 
            k_stacks[i] = k_thread_pos;

            u_thread_pos -= stacks; 
            k_thread_pos -= stacks; 
        }

        u_stacks[IDLE_THREAD_INDEX] = u_thread_pos; 
        k_stacks[IDLE_THREAD_INDEX] = k_thread_pos;

        if (idle_fn == NULL) {
            idle_fn = &default_idle;
        }

        threads_tcb[IDLE_THREAD_INDEX] = construct_tcb(idle_fn, 100, 1000, 100000, NULL, IDLE_THREAD_INDEX);
        threads_tcb[MAIN_THREAD_INDEX] = construct_tcb(NULL, 1000, 100000, 10000, NULL, MAIN_THREAD_INDEX);

        // Default values for mpu 
        if (mpu_type == KERNEL_ONLY){
            mm_region_enable(6, (void *)(u_stacks[0]-16*stacks), mm_log2ceil_size(stacks*16), 0, 1);
        }
        mm_region_enable(7, &__msp_stack_base-2*1024, 11, 0, 1);
        
        // Enable memory management faults
        SCB_SHCRS |= SHCRS_MEMFAULTENA;

        // Enable MPU
        MPU_CTRL |= MPU_CTRL_ENABLE_BG;
        MPU_CTRL |= MPU_CTRL_ENABLE;
        
        return 0; 
    }
    return -1;
}

/** @brief creates thread
 * @param fn thread's function pointer
 * @param prio thread's priority
 * @param C thread's worst case execution time
 * @param T thread's period
 * @param vargp thread's arguments
 * @return -1 in case of error, 0 otherwise*/
int sys_thread_create(void *fn, uint32_t prio, uint32_t C, uint32_t T, void *vargp) {
    if (active_threads == maximum_threads || thread_count == IDLE_THREAD_INDEX) {
        return -1;
    }

    enable_fpu();
    float util = 0.0f;
    for (uint32_t i=0; i < thread_count; i++) {
        if (threads_tcb[i].status != DONE) {
            float c = threads_tcb[i].C;
            float t = threads_tcb[i].T;
            util += (c/t);
        }
    }

    util += (float)C/(float)T;

    if (util > ub_table[active_threads + 1]) {
        return -1;
    }

    uint32_t index = thread_count;

    for (uint32_t i=0; i < IDLE_THREAD_INDEX; i++) { // for thread revival
        if (threads_tcb[i].status == DONE) {
            index = i;
            break;
        }
    }

    threads_tcb[index] = construct_tcb(fn, prio, C, T, vargp, index);
    if (index == thread_count) {
        thread_count++;
    }
    active_threads++;
    return 0; 
}

/** @brief starts scheduler
 * @param frequency systick clock frequency used by the scheduler
 * @return -1 if error, 0 otherwise
 */
int sys_scheduler_start(uint32_t frequency) {
    sys_tick_period_ms = 1000/frequency;
    systick_start(frequency);
    pend_pendsv();
    return 0;
}

/** @brief returns priority of current running thread
 *  @return priority of current running thread
 */
uint32_t sys_get_priority() {
    return threads_tcb[thread_i].dynamic_priority;
}

/**  @brief Function to reutn the total time since scheduler started
 *   @return total time since scheduler started
*/
uint32_t sys_get_time() {
    return sys_tick_counter/sys_tick_period_ms;
}

/**  @return total time a thread has been running
*/
uint32_t sys_thread_time() {
    return threads_tcb[thread_i].total_ticks;
}

/** @brief kills the current running thread*/
void sys_thread_kill() {
    if (thread_i == IDLE_THREAD_INDEX) {
        threads_tcb[thread_i] = construct_tcb(&default_idle, 100, 1000, 100000, NULL, thread_i);
    } else if (thread_i == MAIN_THREAD_INDEX) {
        sys_exit(0);
        return;
    } else {
        active_threads--;
        threads_tcb[thread_i].status = DONE;
    }
    pend_pendsv();
}

/** @brief used in case a thread is done before its worst case execution time. Tells the scheduler to turn the current thread into sleep mode until its next period*/
void sys_wait_until_next_period() {
    threads_tcb[thread_i].status = WAITING;
    threads_tcb[thread_i].exec_time = 0;
    pend_pendsv();
}

/** @brief Function to initialize a mutex 
 *  @return address of newly created mutex of type `kmutex_t`
*/
kmutex_t *sys_mutex_init(uint32_t max_prio) {
    kmutex_t mutex = {
        .locked_by = -1,
        .prio_ceil = max_prio,
        .lock_var = 0,
        .idx_mutex = ++mutex_count,
    };
    
    mutexes[mutex_count-1] = mutex;
    return &mutexes[mutex_count-1];
}

/** @brief Function to lock a previously initialized mutex
 *  @param mutex Mutex to be locked by calling thread
*/
void sys_mutex_lock(kmutex_t *mutex) {
    if (mutex->prio_ceil > threads_tcb[thread_i].priority) {
        printk("Insufficient priority of thread %d to lock mutex!\n", thread_i);
        threads_tcb[thread_i].status = DONE;
        pend_pendsv();
    }

    if (mutex->lock_var == 1 && mutex->locked_by == thread_i) {
        printk("Cannot lock a mutex that is already locked by this thread!\n");
    } else {        
        int max_prio_mutex_idx = get_max_ceiling_idx();

        // No lock is in progress
        if (max_prio_mutex_idx == -1 || threads_tcb[thread_i].dynamic_priority < get_max_ceiling()) {
            uint32_t status = 0;
            do {
                while (load_exclusive((uint32_t*)&mutex->lock_var) != 0);
                status = store_exclusive((uint32_t*)&mutex->lock_var, 1);
            } while (status != 0);
            mutex->locked_by = thread_i;
        }
        
        // One lock is in progress
        else if (max_prio_mutex_idx != -1) {
            
            // Can lock
            if (threads_tcb[thread_i].dynamic_priority < get_max_ceiling() || thread_i == mutexes[max_prio_mutex_idx].locked_by) {
                // Update priority
                if (threads_tcb[thread_i].priority < threads_tcb[mutexes[max_prio_mutex_idx].locked_by].dynamic_priority){
                    threads_tcb[mutexes[max_prio_mutex_idx].locked_by].dynamic_priority = threads_tcb[thread_i].priority;
                }

                // Lock
                uint32_t status = 0;
                do {
                    while (load_exclusive((uint32_t*)&mutex->lock_var) != 0);
                    status = store_exclusive((uint32_t*)&mutex->lock_var, 1);
                } while (status != 0);
                mutex->locked_by = thread_i;
            } 

            // Cannot lock
            else {
                // Update priority
                if (threads_tcb[thread_i].priority < threads_tcb[mutexes[max_prio_mutex_idx].locked_by].dynamic_priority){
                    threads_tcb[mutexes[max_prio_mutex_idx].locked_by].dynamic_priority = threads_tcb[thread_i].priority;
                }

                // Update block status
                threads_tcb[thread_i].blocked_thread = mutexes[max_prio_mutex_idx].locked_by;
                if (threads_tcb[thread_i].dynamic_priority == get_max_ceiling()) threads_tcb[thread_i].blocked_mutex = mutexes[max_prio_mutex_idx].idx_mutex;
                else threads_tcb[thread_i].blocked_mutex = mutex->idx_mutex;
                
                pend_pendsv();

                // Lock
                uint32_t status = 0;
                do {
                    while (load_exclusive((uint32_t*)&mutex->lock_var) != 0);
                    status = store_exclusive((uint32_t*)&mutex->lock_var, 1);
                } while (status != 0);
                mutex->locked_by = thread_i;
            }
        }
    }
}

/** @brief Function to unlock a previously locked mutex
 *  @param mutex Mutex to be unlocked by calling thread
*/
void sys_mutex_unlock(kmutex_t *mutex) {
    if (mutex->lock_var == 0) {
        printk("Cannot unlock a mutex that is already unlocked!\n");
        return;
    }
    disable_interrupts();

    // Unlock
    mutex->lock_var = 0;
    mutex->locked_by = -1;
    threads_tcb[thread_i].dynamic_priority = threads_tcb[thread_i].priority; 
    
    for (int i=0; i<IDLE_THREAD_INDEX; i++){
        if (threads_tcb[i].blocked_thread == (int)thread_i && threads_tcb[i].blocked_mutex == (int)mutex->idx_mutex){
            threads_tcb[i].blocked_mutex = -1;
            threads_tcb[i].blocked_thread = -1;
        } 
    }

    enable_interrupts();
    pend_pendsv();
}