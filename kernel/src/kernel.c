/** @file   kernel.c
 *  @note   put a better file header here
**/

#include <arm.h>
#include <timer.h>
#include <rtt.h>
#include <printk.h>
#include <reset.h>
#include <rtt.h>
#include <gpio.h>
#include <mpu.h>

/** @brief enter in user mode, change privilege and stack pointer*/
extern void enter_user_mode();

/** @brief Thread user stack limit */
extern char __thread_u_stacks_limit;
/** @brief Thread user stack base address */
extern char __thread_u_stacks_base; 
/** @brief User code base address */
extern char __svc_stub_start;
/** @brief User read-only data base address */
extern char  __user_rodata_start; 
/** @brief User data base address */
extern char __user_data_start; 
/** @brief User BSS base address */
extern char __user_bss_start; 
/** @brief Heap base base address */
extern char __heap_base; 
/** @brief Main thread's user stack base address */
extern char __psp_stack_base;

/** @brief main function of our kernel that initializes peripherials and changes to user mode */
int kernel_main() {
    init_align_prio();   /* do not remove this line */
    enable_reset();
    rtt_init();
    gpio_init(RED_LED_PORT, RED_LED_PIN, MODE_OUTPUT, DRIVE_S0H1, PUPD_NONE);
    gpio_init(BLUE_LED_PORT, BLUE_LED_PIN, MODE_OUTPUT, DRIVE_S0H1, PUPD_NONE);

    // User code
    mm_region_enable(0, &__svc_stub_start, 14, 1, 0);
    // User read-only data
    mm_region_enable(1, &__user_rodata_start, 11, 0, 0);
    // User data
    mm_region_enable(2, &__user_data_start, 10, 0, 1);
    // User BSS
    mm_region_enable(3, &__user_bss_start, 10, 0, 1);
    // User heap
    mm_region_enable(4, &__heap_base, 13, 0, 1);
    // Main thread user stack
    mm_region_enable(5, &__psp_stack_base-2*1024, 11, 0, 1);
    enter_user_mode();

    BUSY_LOOP(1);
    return 0;
}
