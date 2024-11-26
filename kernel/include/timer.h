/** @file   timer.h
 *
 *  @brief  function prototypes for systick timer
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
**/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <arm.h>

/** @brief Maximum RVR load value */
#define SYS_TICK_FREQUENCY 64000000

/** @brief struct used to map to memory all diferent SysTick registers */
typedef struct systick {
    /** Systick control and status register */
    volatile uint32_t stcsr;
    /** Systick reload value register */
    volatile uint32_t strvr;
    /** Systick current value register */
    volatile uint32_t stcvr;
    /** Systick calibration value register */
    volatile uint32_t stcr;
} systick_t;

/** @brief Function to start the SysTick timer 
 *  @param frequency frequency of context switches in Hz (sched ticks/second)
*/
void systick_start(int frequency);

/** @brief Function to stop the SysTick timer */
void systick_stop();

#endif /* _TIMER_H_ */
