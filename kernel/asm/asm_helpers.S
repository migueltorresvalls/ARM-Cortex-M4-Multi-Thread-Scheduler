.cpu cortex-m4
.syntax unified
.thumb

.section .text

.thumb_func
.global enter_user_mode
enter_user_mode:
    /* Load psp register */
    ldr r1, =__psp_stack_base
    msr psp, r1

    mrs r0, control
    b _usr_mod

    @ to get this to launch user mode:
    @ - set up the process stack pointer
    @ - tail-call to the user-mode function that does the rest
    @ - remove the breakpoint
    @ then, replace these comments with something of your own
    @
    @ user mode is done in a way that control will never come back
    @ to kernel mode, otherwise the wrong stack will be used...



