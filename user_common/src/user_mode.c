#include <usr_arg.h>
#include <stdlib.h>
#include <stdint.h>

extern int main(int argc, const char* argv[]);

/** NOTE: this is really not a good way to switch to userspace code. An RTOS with
 *  memory protection would launch a userspace program using an exception return, 
 *  which provides an atomic mode and control switch, which prevents an MPU fault.
 *  For simplicity, we're switching to user mode and then launching the program. 
 *
 *  Production code should never ever ever do this...
 */

/* force noinline because it would return to wrong stack */
void __attribute__((noinline)) launch_main() {
    int ret_val = main(user_argc, user_argv);
    exit(ret_val);
}

void _usr_mod(uint32_t control) {
    uint32_t value = 0x3; 
    __asm volatile("msr control, %0" :: "r" (value|control));
    __asm volatile("isb");

    launch_main();
}