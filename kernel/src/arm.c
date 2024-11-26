/** @file   arm.c
 *
 *  @brief  helpful ARM utilities for system configuration
 *  @note   not for public release, do not share
 *
 *  @date   Last modified 31 Oct 2023
 *  @author CMU 14-642
**/

#include <arm.h>
#include <unistd.h>

/** @brief Interrupt Control and State Register */
#define ICSR ((volatile uint32_t *) 0xE000ED04)

/** @brief Configuration and Control Register */
#define CCR ((volatile uint32_t *) 0xE000ED14)

/** @brief System handler priority register 1 */
#define SHPR1 ((volatile uint32_t *) 0xE000ED18)

/** @brief System handler priority register 2 */
#define SHPR2 ((volatile uint32_t *) 0xE000ED1C)

/** @brief System handler priority register 3 */
#define SHPR3 ((volatile uint32_t *) 0xE000ED20)

/** @brief System handler control and state register */
#define SHCSR ((volatile uint32_t *) 0xE000ED24)

/** @brief Register to enable/disable fpu */
#define CPACR ((volatile uint32_t *) 0xE000ED88)

/** @brief FPU control data */
#define FPCCR ((volatile uint32_t *) 0xE000EF34)

/** @brief The I-code cache configuration register is used to configure
  * instruction caching, the only available caching on the nrf52840     */
#define ICACHECNF ((volatile uint32_t *) 0x4001E540)

/** @brief in initialize stack alignment and set handler priorities */
void init_align_prio() {
    // stack alignment
    *CCR &= ~(0x1 << 9);

    // MemoryMgmt priority ==> 1
    *SHPR1 |= 0x00000020;
    *SHPR1 &= 0xFFFFFF20;

    // SVC handler priority ==> 2
    *SHPR2 |= 0x40000000;
    *SHPR2 &= 0x40FFFFFF;

    // Systick & PendSV priority ==> 1
    *SHPR3 |= 0x20200000;
    *SHPR3 &= 0x2020FFFF;

    data_sync_barrier();
    inst_sync_barrier();
}

/** @brief set the enable bit for instruction caching */
void enable_prefetch_i_cache() {
    *ICACHECNF |= (0x1 << 0);
}

/** @brief set the FPU enable bits in necessary registers */
void enable_fpu() {
    *CPACR |= (0xF << 20);
    *FPCCR |= (0x1 << 31);
    data_sync_barrier();
    inst_sync_barrier();
}

/** @brief pend a pendsv */
void pend_pendsv(){
    *ICSR |= (1 << 28);
}

/** @brief clears pendsv */
void clear_pendsv(){
    *ICSR |= (1 << 27);
}

/** @brief indicator for SVC active/inactive */
int get_svc_status() {
    return (*SHCSR & (1 << 7));
}

/** @brief set SVC active/inactive */
void set_svc_status(int status) {
    if(status) *SHCSR |= (1 << 7);
    else *SHCSR &= ~(1 << 7);
}
