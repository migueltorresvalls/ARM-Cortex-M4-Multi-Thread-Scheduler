/** @file   mpu.c
 *
 *  @brief  implementation of memory protection for lab 4
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
**/

#include <arm.h>
#include <mpu.h>
#include <printk.h>
#include <syscall.h>
#include <syscall_thread.h>
#include <unistd.h>

void mm_c_handler(void *psp) {
    int status = SCB_CFSR & 0xFF;

    // Attempt to print cause of fault
    printk("%s: ", __func__);
    printk("Memory Protection Fault\n");
    if(status & SCB_CFSR_STKERR) printk("Stacking Error\n");
    if(status & SCB_CFSR_UNSTKERR) printk("Unstacking Error\n");
    if(status & SCB_CFSR_DACCVIOL) printk("Data access violation\n");
    if(status & SCB_CFSR_IACCVIOL) printk("Instruction access violation\n");
    if(status & SCB_CFSR_MMFARVALID) printk("Faulting Address = %x\n", SCB_MMFAR);

    // unrecoverable stack overflow
    uint32_t sp = (uint32_t)psp; 
    if(sp > u_stacks[thread_i] || sp < u_stacks[thread_i]-stacks) {
        printk("stack overflow --> aborting\n");
        sys_exit(-1);
    }

    // Other errors can be recovered from by killing the offending thread.
    // Standard thread killing rules apply. You should halt if the thread
    // is the main or idle thread, but otherwise up to you. Manually set
    // the pc? Call a function? Context swap? TODO
    sys_thread_kill();
}



/** @brief  enables an aligned memory protection region
 *
 *  @param  region_number       region number to enable
 *  @param  base_address        region's base address
 *  @param  size_log2           log[2] of the region's size
 *  @param  execute             indicator if region is user-executable
 *  @param  user_write_access   indicator if region is user-writable
 *
 *  @return 0 if successful, -1 on failure
 */
int mm_region_enable(uint32_t region_number, void* base_address, uint8_t size_log2, int execute, int user_write_access) {
    if(region_number > MPU_RNR_REGION_MAX) {
        printk("Invalid region number\n");
        return -1;
    }

    if((uint32_t)base_address & ((1 << size_log2) - 1)) {
        printk("Misaligned region\n");
        breakpoint();
        return -1;
    }

    if(size_log2 < 5) {
        printk("Region too small\n");
        return -1;
    }

    MPU_RNR = region_number & MPU_RNR_REGION;
    MPU_RBAR = (uint32_t)base_address;

    uint32_t size = ((size_log2 - 1) << 1) & MPU_RASR_SIZE;
    uint32_t ap = user_write_access ? MPU_RASR_USER_RW : MPU_RASR_USER_RO;
    uint32_t xn = execute ? 0 : MPU_RASR_XN;

    MPU_RASR = size | ap | xn | MPU_RASR_ENABLE;
    return 0;
}

/**
 * @brief  Disables a memory protection region.
 *
 * @param  region_number      The region number to disable.
 */
void mm_region_disable(uint32_t region_number) {
    MPU_RNR = region_number & MPU_RNR_REGION;
    MPU_RASR &= ~MPU_RASR_ENABLE;
}

/**
 * @brief  Returns ceiling (log_2 n).
 */
uint32_t mm_log2ceil_size(uint32_t n) {
    uint32_t ret = 0;
    while(n > (1U << ret)) {
        ret++;
    }
    return ret;
}
