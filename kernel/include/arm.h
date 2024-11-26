/** @file   arm.h
 *
 *  @brief  helpful ARM utilities and assembly wrappers
 *  @note   not for public release, do not share
 *
 *  @date   last modified 31 Oct 2023
 *  @author CMU 14-642
**/

#ifndef _ARM_H_
#define _ARM_H_

#include <unistd.h>

/** MIN macro for general kernel use */
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
 /** MAX macro for general kernel use */
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#ifndef NULL
/** define NULL as 0 if not done elsewhere */
    #define NULL    0
#endif

/** local macro for static inline function signatures */
#define intrinsic       __attribute__((always_inline)) static inline
/** helper macro for polling loop */
#define BUSY_LOOP(COND) while (COND) __asm("")
/** helper macro for forced delay w/o clocks */
#define LAZY_DELAY(N)   for(int i=N-1; i>=0; i--)

/** @brief initialize stack alignment and SVC priority */
void init_align_prio();

/** @brief enable instruction caching */
void enable_prefetch_i_cache();

/** @brief enable floating-point unit */
void enable_fpu();

/** @brief set PendSV interrupt as pending */
void pend_pendsv();

/** @brief clear PendSV interrupt pending */
void clear_pendsv();

/** @brief get status indicator for SVC in progress */
int get_svc_status();

/** @brief set status indicator for SVC in progress */
void set_svc_status(int status);

/** @brief set breakpoint to current instruction */
intrinsic void breakpoint() {
    __asm volatile("bkpt");
}

/** @brief wait for interrupt to occur */
intrinsic void wait_for_interrupt() {
    __asm volatile("wfi");
}

/** @brief enable interrupts */
intrinsic void enable_interrupts() {
    __asm volatile("cpsie f");
}

/** @brief disable interrupts */
intrinsic void disable_interrupts() {
    __asm volatile("cpsid f");
}

/** @brief data memory barrier */
intrinsic void data_mem_barrier() {
    __asm volatile("dmb");
}

/** @brief data synchronization memory barrier */
intrinsic void data_sync_barrier() {
    __asm volatile("dsb");
}

/** @brief instruction syncronization barrier */
intrinsic void inst_sync_barrier() {
    __asm volatile("isb");
}

/** @brief wrapper for asm strex (store exclusive) */
intrinsic uint32_t store_exclusive(uint32_t *addr, uint32_t val) {
    uint32_t status;
    __asm volatile("strex %0, %1, [%2]" : "=r" (status) : "r" (val), "r" (addr));
    return status;
}

/** @brief wrapper for asm ldrex (load exclusive) */
intrinsic uint32_t load_exclusive(uint32_t *addr) {
    uint32_t value;
    __asm volatile("ldrex %0, [%1]" : "=r" (value) : "r" (addr));
    return value;
}

/** @brief saves interrupt enabled state and disables interrupts */
intrinsic int save_interrupt_state_and_disable() {
    int result;
    int disable_constant = 1;
    __asm volatile("mrs %0, PRIMASK" : "=r" (result));
    __asm volatile("msr PRIMASK, %0" :: "r" (disable_constant));
    return result;
}

/** @brief restores saved interrupt enable state */
intrinsic void restore_interrupt_state(int state) {
    __asm volatile("msr PRIMASK, %0" :: "r" (state));
}

#undef intrinsic
#endif /* _ARM_H_ */
