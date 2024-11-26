/** @file   test_2_1/main.c
 *
 *  @brief  user-space project "test_2_1", demonstrates stack allocation checks
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output self-explanatory success/failure messages
**/

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 1KB */
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1000

void idle_thread() {
	while(1);
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	int status;
	status = thread_init(4, 16384, &idle_thread, KERNEL_ONLY, NUM_MUTEXES);
	if(!status) {
		printf("Test failed, stack should be too large (%d threads).\n", 4);
		return -1;
	}

	status = thread_init(1, 16384, &idle_thread, KERNEL_ONLY, NUM_MUTEXES);
	if(!status) {
		printf("Test failed, stack should be too large (%d threads).\n", 1);
		return -1;
	}

	status = thread_init(12, 1024, &idle_thread, KERNEL_ONLY, NUM_MUTEXES);
	if(!status) {
		printf("Test failed, stack should be too large (%d threads).\n", 12);
		return -1;
	}

	printf("Test passed!\n");

	return 0;
}
