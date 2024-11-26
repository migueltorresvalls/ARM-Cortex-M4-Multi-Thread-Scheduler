/** @file   test_1_4/main.c
 *
 *  @brief  user-space project "test_1_4", demonstrates WUNP with two threads
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
#define NUM_THREADS 2
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1000

/** @brief Computation time of the task */
#define THREAD0_C 321
#define THREAD1_C 123
/** @brief Period of the thread */
#define THREAD_T 1024

/** @brief Give some leeway when testing */
#define SLACK 5

void thread_0(void* vargp) {
	uint32_t cnt = 0;
	uint32_t* ptr_time =(uint32_t*)vargp;
	while(1) {
		uint32_t t = get_time();

		if(cnt > 20) {
			// printf("Test passed!\n");
			while(1);
		}

		if(t < THREAD_T * cnt || t > THREAD_T * cnt + SLACK) {
			printf("Failed. Should have woken up at or after t = %lu\n", THREAD_T * cnt);
			printf("Woke up at t = %lu\n", t);
			while(1);
		}

		print_num_status_cnt(0, cnt++);
		*ptr_time = get_time();
		wait_until_next_period();
	}
}

void thread_1(void* vargp) {
	uint32_t cnt = 0;
	uint32_t* ptr_time =(uint32_t*)vargp;
	while(1) {
		uint32_t t = get_time();
		
		if(t > *ptr_time + SLACK) {
			printf("Failed. Should have woken up close to t = %lu\n", *ptr_time);
			printf("Woke up at t = %lu\n", t);
			while(1);
		}

		print_num_status_cnt(1, cnt++);

		if(cnt > 20) {
			printf("Test passed!\n");
			while(1);
		}

		wait_until_next_period();
	}
}

void idle_thread() {
	while(1);
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, &idle_thread, KERNEL_ONLY, NUM_MUTEXES));

	printf("Successfully initialized threads...\n");

	uint32_t* ptr_time = malloc(sizeof(uint32_t));

	ABORT_ON_ERROR(thread_create(&thread_0, 0, THREAD0_C, THREAD_T, ptr_time));

	ABORT_ON_ERROR(thread_create(&thread_1, 1, THREAD1_C, THREAD_T, ptr_time));

	printf("Successfully created threads! Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY), "Threads are unschedulable!\n");

	while(1);

	return 0;
}
