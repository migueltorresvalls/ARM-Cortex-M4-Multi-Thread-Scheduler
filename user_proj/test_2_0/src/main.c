/** @file   test_2_0/main.c
 *
 *  @brief  user-space project "test_2_0", demonstrates idle thread state
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
#define USR_STACK_WORDS 256
#define NUM_THREADS 1
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1000

/** @brief Computation time of the task */
#define THREAD_C 10
/** @brief Period of the thread */
#define THREAD_T 10

/** @brief Idle thread that does something! */
void idle_thread() {
	if(print_fibs(10000000, 100000, 1000000000) == 380546875) {
		printf("Test passed!\n");
	} else {
		printf("Test failed...\n");
	}

	while(1);
}

void thread_0(UNUSED void* vargp) {
	while(1) {
		wait_until_next_period();
	}
}

int main(UNUSED int argc, UNUSED char* const argv[] ) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, &idle_thread, KERNEL_ONLY, NUM_MUTEXES));

	ABORT_ON_ERROR(thread_create(&thread_0, 0, THREAD_C, THREAD_T, NULL));

	printf("Successfully created threads! Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
