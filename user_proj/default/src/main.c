/** @file   default/main.c
 *
 *  @brief  user-space project "default", demonstrates system call use
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output repeatedly prints provided argument to console
**/

#include <stdio.h>

#define UNUSED __attribute__((unused))

int main(UNUSED int argc, UNUSED char* const argv[]) {
    printf("\nHello world!");
    return 0;
}
