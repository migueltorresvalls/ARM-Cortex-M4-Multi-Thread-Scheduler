/** @file   svc_handler.h
 *
 *  @brief  prototypes for push-button "soft reset"
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 5 October 2023
 *  @author CMU 14-642
**/

#ifndef _SVC_HANDLER_H_
#define _SVC_HANDLER_H_

/** @brief struct used to map to memory all diferent components to be pushed on a stack frame */
typedef struct {
    /** Register r0*/
    uint32_t r0;
    /** Register r1*/
    uint32_t r1;
    /** Register r2*/
    uint32_t r2;
    /** Register r3*/
    uint32_t r3;
    /** Register r12*/
    uint32_t r12;
    /** Link Register */
    uint32_t lr;
    /** Program Counter */
    uint32_t pc;
    /** Special Program Status Register */
    uint32_t xPSR;
    /** Fifth argument stored on stack */
    uint32_t arg5;
} stack_frame_t;

#endif /* _SVC_HANDLER_H_ */
