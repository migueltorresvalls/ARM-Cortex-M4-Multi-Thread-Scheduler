/** @file   reset.h
 *
 *  @brief  prototypes for push-button "soft reset"
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 5 October 2023
 *  @author CMU 14-642
**/

#ifndef _RESET_H_
#define _RESET_H_

/** @brief  enable the use of the on-board RESET button to trigger a system reset */
void enable_reset();

#endif /* _RESET_H_ */
