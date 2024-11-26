/** @file   mpu.h
 *
 *  @brief  prototypes for memory protection
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 3 November 2023
 *  @author CMU 14-642
**/

#ifndef _MPU_H_
#define _MPU_H_

#include <unistd.h>

/** @brief mpu control register */
#define MPU_CTRL *((uint32_t*)0xE000ED94)
/** @brief mpu region number register */
#define MPU_RNR *((uint32_t*)0xE000ED98)
/** @brief mpu region base address register */
#define MPU_RBAR *((uint32_t*)0xE000ED9C)
/** @brief mpu region attribute and size register */
#define MPU_RASR *((uint32_t*)0xE000EDA0)

/** @brief MPU CTRL register flags */
//@{
#define MPU_CTRL_ENABLE_BG  (1 << 2)
#define MPU_CTRL_ENABLE     (1 << 0)
//@}

/** @brief MPU RNR register flags */
//@{
#define MPU_RNR_REGION      (0xFF)
#define MPU_RNR_REGION_MAX  (7)
//@}

/** @brief MPU RASR register masks */
//@{
#define MPU_RASR_XN         (1 << 28)
#define MPU_RASR_SIZE       (0x3E)
#define MPU_RASR_ENABLE     (1 << 0)
#define MPU_RASR_USER_RO    (2 << 24)
#define MPU_RASR_USER_RW    (3 << 24)
//@}

/** @brief system handler control and state register */
#define SCB_SHCRS *((uint32_t*)0xE000ED24)
/** @brief configurable fault status register */
#define SCB_CFSR *((uint32_t*)0xE000ED28)
/** @brief mem mgmt fault address register */
#define SCB_MMFAR *((uint32_t*)0xE000ED34)

/** @brief SCB_SHCRS mem mgmt fault enable bit */
#define SHCRS_MEMFAULTENA   (1 << 16)

/** @brief stacking error */
#define SCB_CFSR_STKERR     (1 << 4)
/** @brief unstacking error */
#define SCB_CFSR_UNSTKERR   (1 << 3)
/** @brief data access error */
#define SCB_CFSR_DACCVIOL   (1 << 1)
/** @brief instruction access error */
#define SCB_CFSR_IACCVIOL   (1 << 0)
/** @brief indicates the MMFAR is valid */
#define SCB_CFSR_MMFARVALID (1 << 7)

/** @enum   mpu_mode
 *  @brief  memory protection mode is PER_THREAD or KERNEL_ONLY
 */
typedef enum { PER_THREAD = 1, KERNEL_ONLY = 0 } mpu_mode;

uint32_t mm_log2ceil_size(uint32_t n);
void mm_region_disable(uint32_t region_number);
int mm_region_enable(uint32_t region_number, void *base_address, uint8_t size_log2, int execute, int user_write_access);

#endif /* _MPU_H_ */