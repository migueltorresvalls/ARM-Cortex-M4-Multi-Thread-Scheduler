/** @file   svc_stubs.S
 *
 *  @brief  Stub functions for 14642 syscalls
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 3 November 2023
 *  @author CMU 14-642
**/

.cpu cortex-m4
.syntax unified
.section .svc_stub
.thumb

#include "../../kernel/include/svc_num.h"

/** the first four functions are base system call stubs that are
  * exposed to user-space programs and used primarily by libraries.
  * these should be copied from your lab 3 implementation.
 **/
.global _sbrk
_sbrk:
    svc #0x0
    bx r14


.global _write
_write:
    svc #0x1
    bx r14


.global _read
_read:
    svc #0x2
    bx r14


.global _exit
_exit:
    svc #0x3
    bx r14


/** the next several functions are custom system call stubs that are
  * exposed to support multitasking / context switching and user projects.
 **/

.global thread_init
thread_init:
    svc     #0x4
    bx      r14

.global thread_create
thread_create:
    svc     #0x5
    bx      r14

.global thread_kill
thread_kill:
    svc     #0x6
    bx      r14

.global scheduler_start
scheduler_start:
    svc     #0xC
    bx      r14

.global mutex_init
mutex_init:
    svc     #0x7
    bx      r14

.global mutex_lock
mutex_lock:
    svc     #0x8
    bx      r14

.global mutex_unlock
mutex_unlock:
    svc     #0x9
    bx      r14

.global wait_until_next_period
wait_until_next_period:
    svc     #0xA
    bx      r14

.global get_time
get_time: 
    svc     #0xB
    bx      r14

.global get_priority
get_priority:
    svc     #0xD
    bx      r14

.global thread_time
thread_time:
    svc     #0xE
    bx      r14

.global default_idle
default_idle:
    end:
    b end

/** the last few functions are simple system call stubs that
  * just return a constant (without a complete system call) or
  * halt the program using a breakpoint.  any library functions
  * that lead to one of the breakpoints are not supported by
  * our system call implementation.
  *
  * everything below this comment can stay exactly how it is
 **/
.global _getpid
_getpid:
    mov r0, #1
    bx lr

.global _fstat
_fstat:
    mov r0, #-1
    bx lr

.global _isatty
_isatty:
    mov r0, #1
    bx lr

.global _lseek
_lseek:
    mov r0, #-1
    bx lr

.global _start
_start:
    bkpt

.global _close
_close:
    bkpt

.type thread_kill, %function
.global _kill
_kill:
    bkpt

.global _gettimeofday
_gettimeofday:
    bkpt

.global _times
_times:
    bkpt

