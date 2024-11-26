/** @file   svc_num.h
 *
 *  @brief  constant defines for svc calls for lab 4
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
**/

#ifndef _SVC_NUM_H_
#define _SVC_NUM_H_

/** base syscalls for lab 3 user-space apps */
/** @brief SVC number for _sbrk() */
#define SVC_SBRK        0
/** @brief SVC number for _write() */
#define SVC_WRITE       1
/** @brief SVC number for _read() */
#define SVC_READ        2
/** @brief SVC number for _exit() */
#define SVC_EXIT        3

/** extended syscalls for lab 4 multitasking */
/** @brief SVC number for thread_init() */
#define SVC_THR_INIT    4
/** @brief SVC number for thread_create() */
#define SVC_THR_CREATE  5
/** @brief SVC number for thread_kill() */
#define SVC_THR_KILL    6
/** @brief SVC number for mutex_init() */
#define SVC_MUT_INIT    7
/** @brief SVC number for mutex_lock() */
#define SVC_MUT_LOK     8
/** @brief SVC number for mutex_unlock() */
#define SVC_MUT_ULK     9
/** @brief SVC number for wait_until_next_period() */
#define SVC_WAIT        10
/** @brief SVC number for get_time() */
#define SVC_TIME        11
/** @brief SVC number for scheduler_start() */
#define SVC_SCHD_START  12
/** @brief SVC number for get_priority() */
#define SVC_PRIORITY    13
/** @brief SVC number for thread_get_time() */
#define SVC_THR_TIME    14

/** custom syscalls for lab 3 user-space apps */
/** @brief SVC number for delay_ms() */
#define SVC_DELAY_MS        22
/** @brief SVC number for lux_read() */
#define SVC_LUX             23
/** @brief SVC number for pix_set() */
#define SVC_PIX             24

#endif /* _SVC_NUM_H_ */

