/** @file   test_2_2/main.c
 *
 *  @brief  user-space project "test_2_2", demonstrates implicit idle and thread limit
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
#define NUM_THREADS 4
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1000

/** @brief Computation time of the task */
#define THREAD_C 100
/** @brief Period of the thread */
#define THREAD_T 1000

void thread_fn(void* vargp) {
	int num = (int)vargp;
	print_num_status(num);
	while(1) wait_until_next_period();
}

void thread_fn_last(void* vargp) {
	int num = (int)vargp;
	print_num_status(num);
	printf("A memory error after this is probably an implicit idle thread error.\n");
	wait_until_next_period();
	printf("Test passed!\n");
	while(1) wait_until_next_period();
}

int main(UNUSED int argc, UNUSED char* const argv[]) {

	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, KERNEL_ONLY, NUM_MUTEXES), "Test failed: implicit idle thread.\n");

	for(int i = 0; i < NUM_THREADS - 1; i++) {
		ABORT_ON_ERROR(thread_create(&thread_fn, i, THREAD_C, THREAD_T,(void*)i), "Failed to create thread %d\n", i);
	}
	ABORT_ON_ERROR(thread_create(&thread_fn_last, NUM_THREADS - 1, THREAD_C, THREAD_T,(void *)(NUM_THREADS - 1)),
		"Failed to create thread %d\n", NUM_THREADS - 1);

	int stat = thread_create(&thread_fn_last, 2, THREAD_C, THREAD_T, NULL);
	if(stat == 0) {
		printf("Test failed: max_threads exceeded.\n");
	}

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
