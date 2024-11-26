/** @file   syscall_thread.h
 *
 *  @brief  system calls to support thread library for lab 4
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
**/

#ifndef _SYSCALL_THREAD_H_
#define _SYSCALL_THREAD_H_

#include <unistd.h>
#include <mpu.h>
#include <svc_handler.h>
#include <syscall_mutex.h>

/** @brief Maximum number of mutexes */
#define MAXIMUM_MUTEXES 32

/** @brief Array index of current running thread*/
extern uint32_t thread_i; 
/** @brief Array that contains base address of user space staks for all threads*/
extern uint32_t u_stacks[16];
/** @brief Size of each thread's user space and kernel space stack*/
extern uint32_t stacks; 

/** @brief initialize thread switching
 *  @note  must be called before creating threads or scheduling
 *
 *  @param max_threads      max number of threads to be created
 *  @param stack_size       stack size in words of all created stacks
 *  @param idle_fn          function pointer for idle thread, NULL for default
 *  @param mem_protect      mpu mode, either PER_THREAD or KERNEL_ONLY
 *  @param max_mutexes      max number of mutexes supported
 *
 *  @return     0 for success, -1 for failure
 */
int sys_thread_init(uint32_t max_threads, uint32_t stack_sizes, void* idle_fn, mpu_mode mem_protect, uint32_t max_mutexes);

/** @brief create a new thread as specified, if UB allows
 *
 *  @param fn       thread function pointer
 *  @param prio     thread priority, with 0 being highest
 *  @param C        execution time (scheduler ticks)
 *  @param T        task period (scheduler ticks)
 *  @param vargp    thread function argument
 *
 *  @return     0 for success, -1 for failure
 */
int sys_thread_create(void* fn, uint32_t prio, uint32_t C, uint32_t T, void* vargp);

/** @brief tell the kernel to start running threads using Systick
 *  @note  returns only after all threads complete or are killed
 *  @param frequency frequency of context switches in Hz (sched ticks/second)
 *  @return     0 for success, -1 for failure
 */
int sys_scheduler_start(uint32_t frequency);

/** @brief get the current time in ticks */
uint32_t sys_get_time();

/** @brief get the dynamic priority of the running thread */
uint32_t sys_get_priority();

/** @brief get the total elapsed time for the running thread */
uint32_t sys_thread_time();

/** @brief deschedule thread and wait until next turn */
void sys_wait_until_next_period();

/** @brief kill the running thread
 *
 *  @note  locks if main or idle thread is running or thread holds a mutex
 *
 *  @return does not return
 */
void sys_thread_kill();

/** @enum   thread_status
 *  @brief  thread's status is RUNNABLE or WAITING
 */
typedef enum { DONE = 2, RUNNABLE = 1, WAITING = 0 } thread_status;

/** @brief struct for easy access to stack frame after calling pendsv_c_handler*/

typedef struct {
    /** Register r0*/
    uint32_t r0;
    /** Register r1*/
    uint32_t r1;
    /** Register r2*/
    uint32_t r2;
    /** Register r3*/
    uint32_t r3;
    /** Register r12*/
    uint32_t r12;
    /** Link Register */
    uint32_t lr;
    /** Program Counter */
    uint32_t pc;
    /** Special Program Status Register */
    uint32_t xPSR;
    /** Fifth argument stored on stack */
} stack_frame_p;

/** @brief struct used to handle kernel stack. Needs to be correctly aligned so stack values match struct fields*/
typedef struct {
    /** pointer to user stack struct*/
    stack_frame_p* psp;
    /** register r4*/
    uint32_t r4; 
    /** register r5*/
    uint32_t r5; 
    /** register r6*/
    uint32_t r6;
    /** register r7*/
    uint32_t r7; 
    /** register r8*/
    uint32_t r8; 
    /** register r9*/
    uint32_t r9; 
    /** register r10*/
    uint32_t r10; 
    /** register r11*/
    uint32_t r11; 
    /** register lr*/
    uint32_t lr; 
} stack_frame_m; 

/** @brief struct used to represent Thread Control Block (TCB) */
typedef struct {
    /** id of blocking thread*/
    int blocked_thread; 
    /** id of blocking mutex*/
    int blocked_mutex;
    /** Kernel's stack frame */
    stack_frame_m* msp;
    /** Thread's execution time (C) */
    uint32_t C;
    /** Thread's time period (T) */
    uint32_t T;
    /** Thread's static priority */
    uint32_t priority;
    /** Thread's dynamic priority*/
    uint32_t dynamic_priority;
    /** Thread's status */
    thread_status status;
    /** SVC status */
    uint32_t SVC_status; 
    /** Thread's time */
    uint32_t exec_time;
    /** total time that a thread has been running*/
    uint32_t total_ticks;
} tcb_t;

/** @brief Array to store the created threads (and idle and main thread)*/
extern tcb_t threads_tcb[16];

#endif /* _SYSCALL_THREAD_H_ */
