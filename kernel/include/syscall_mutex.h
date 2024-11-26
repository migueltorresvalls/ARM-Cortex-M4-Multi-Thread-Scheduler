/** @file   syscall_mutex.h
 *
 *  @brief  prototypes for system calls to support mutexes for lab 4
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
**/

#ifndef _SYSCALL_MUTEX_H_
#define _SYSCALL_MUTEX_H_

#include <unistd.h>

/** @brief Lock variable value when unlocked */
#define UNLOCKED ((uint32_t)-1)

/**
 * @brief The struct for a mutex.
 */
typedef struct {
  /** index of the mutex in the array */
  volatile uint32_t idx_mutex;
  /** Index of thread that locked the mutex */
  volatile uint32_t locked_by;
  /** Priority ceiling of the mutex */
  volatile uint32_t prio_ceil;
  /** Lock variable that will be set to 0 or 1 */
  volatile uint32_t lock_var;
} kmutex_t;

/**
 * @brief      Used to create a mutex object. The mutex resides in kernel
 *             space. The user receives a handle to it. With memory
 *             protection, the user cannot modify it. However, it can still
 *             be passed around and used with lock and unlock.
 *
 * @param      max_prio  The maximum priority of a thread which could use this mutex.
 *
 * @return     A pointer to the mutex. NULL if max_mutexes would be exceeded.
 */
kmutex_t* sys_mutex_init(uint32_t max_prio);

/**
 * @brief      Lock a mutex
 *
 *             This function will not return until the requesting thread has
 *             obtained the mutex.
 *
 * @param[in]  mutex  The mutex to act on.
 */
void sys_mutex_lock(kmutex_t* mutex);

/**
 * @brief      Unlock a mutex
 *
 * @param[in]  mutex  The mutex to act on.
 */
void sys_mutex_unlock(kmutex_t* mutex);

#endif /* _SYSCALL_MUTEX_H_ */
