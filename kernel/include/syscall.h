/** @file   syscall.h
 *
 *  @brief  declarations of base system calls for lab 4
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 3 November 2023
 *  @author CMU 14-642
**/

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/** @brief  kernel-side function to set program break, called via SVC from _sbrk */
void* sys_sbrk(int incr);

/** @brief  kernel-side function to write via RTT, called via SVC from _write */
int sys_write(int file, char* ptr, int len);

/** @brief  kernel-side function to read via RTT, called via SVC from _read */
int sys_read(int file, char* ptr, int len);

/** @brief  kernel-side function to exit user program, called via SVC from _exit */
void sys_exit(int status);

#endif /* _SYSCALL_H_ */
