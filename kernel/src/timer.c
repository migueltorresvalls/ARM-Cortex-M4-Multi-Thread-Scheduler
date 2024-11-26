/** @file   timer.c
 *  @note   put a better file header here
**/

#include <timer.h>
#include <unistd.h>

/** Base address of SysTick timer */
static const uint32_t systick_base_addr = (uint32_t)0xE000E010;

/** @brief Function to start the SysTick timer 
 *  @param frequency frequency of context switches in Hz (sched ticks/second)
*/
void systick_start(int frequency) {
    (void)frequency;
    systick_t *systick_addr = (systick_t *)systick_base_addr;
    systick_addr->stcsr = 0;
    systick_addr->strvr = 64000; // 1ms
    systick_addr->stcsr |= (1<<2); // configure clock source as processor clock
    systick_addr->stcsr |= (1<<1); // configure tickint 
    systick_addr->stcsr |= 1;
    return;
}

/** @brief Function to stop the SysTick timer */
void systick_stop() {
    systick_t *systick_addr = (systick_t *)systick_base_addr;
    systick_addr->stcsr &= ~1;
}

