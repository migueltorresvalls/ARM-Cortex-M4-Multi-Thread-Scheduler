/** @file   test_5_1/main.c
 *
 *  @brief  user-space project "test_5_1", demonstrates stack isolation and heap sharing
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output access to wrong stack triggers MPU fault and kills offending thread
**/

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 1KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 2
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1000

/** @brief Computation time of the threads */
#define THREAD_C 100
/** @brief Period of the threads */
#define THREAD_T 500

void thread_good(void* vargp) {
	int cnt = 0;
	volatile int** data_ptr = (volatile int**)vargp;
	volatile int data = 42;
	*data_ptr = &data;
	while(data) {
		print_status_cnt("good", cnt++);
		wait_until_next_period();
		if(cnt > 5) {
			printf("Test passed.\n");
			return;
		}
	}
	printf("Test failed.\n");
	return;
}

void thread_bad(void* vargp) {
	int cnt = 0;
	volatile int** data_ptr =(volatile int**)vargp;
	print_status_cnt("bad", cnt++);
	wait_until_next_period();
	**data_ptr = 0;
	return;
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, PER_THREAD, NUM_MUTEXES));

	int** data_ptr = malloc(sizeof(int*));

	ABORT_ON_ERROR(thread_create(&thread_good, 0, THREAD_C, THREAD_T, data_ptr));
	ABORT_ON_ERROR(thread_create(&thread_bad, 1, THREAD_C, THREAD_T, data_ptr));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
