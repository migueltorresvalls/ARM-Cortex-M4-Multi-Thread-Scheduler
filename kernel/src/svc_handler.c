/** @file   svc_handler.c
 *  @note   put a better file header here
**/

#include <arm.h>
#include <printk.h>
#include <svc_num.h>
#include <svc_handler.h>
#include <unistd.h>
#include <gpio.h>
#include <rtt.h>
#include <syscall.h>
#include <syscall_thread.h>
#include <syscall_mutex.h>
#include <stdio.h>
#include <stdlib.h>

/** @brief c part of the svc handler interruption
 * @param psp pointer to process stack frame*/
void svc_c_handler(void* psp) {
    stack_frame_t* s = (stack_frame_t *)psp;

    uint32_t* pc = (uint32_t *)(s->pc-2); 
    uint8_t svc_num = *pc;

    int return_value = 0;

    switch (svc_num) {
        case SVC_EXIT:
            sys_exit(s->r0);
            break;
        case SVC_WRITE:
            return_value = sys_write(s->r0, (char *)s->r1, s->r2);
            break;
        case SVC_READ:
            return_value = sys_read(s->r0, (char *)s->r1, s->r2);
            break;
        case SVC_SBRK:
            return_value = (int)sys_sbrk(s->r0);
            break;
        case SVC_THR_INIT:
            return_value = sys_thread_init(s->r0, s->r1, (void*)s->r2, s->r3, s->arg5);
            break;
        case SVC_THR_CREATE:
            return_value = sys_thread_create((void *)s->r0, s->r1, s->r2, s->r3, (void*)s->arg5);
            break;
        case SVC_THR_KILL:
            sys_thread_kill();
            break;
        case SVC_SCHD_START:
            return_value = sys_scheduler_start(s->r0);
            break;
        case SVC_TIME:
            return_value = sys_get_time(s->r0);
            break;
        case SVC_THR_TIME:
            return_value = sys_thread_time();
            break;
        case SVC_WAIT:
            sys_wait_until_next_period();
            break;
        case SVC_PRIORITY:
            return_value = sys_get_priority();
            break;
        case SVC_MUT_INIT:
            return_value = (uint32_t)sys_mutex_init(s->r0);
            break;
        case SVC_MUT_LOK:
            sys_mutex_lock((kmutex_t*)s->r0);
            break;
        case SVC_MUT_ULK:
            sys_mutex_unlock((kmutex_t*)s->r0);
            break;
        default:
            printk("svc #%d not implemented yet\n", svc_num);
    }

    s->r0 = return_value;
    return;
}

