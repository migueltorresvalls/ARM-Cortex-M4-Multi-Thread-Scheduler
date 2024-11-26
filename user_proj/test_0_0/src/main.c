/** @file   test_0_0/main.c
 *
 *  @brief  user-space project "test_0_0", demonstrates context saving with a single thread (and idle thread)
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output repeatedly outputs thread status
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

void thread_0(UNUSED void* vargp) {
	int cnt = 0;
	while(1) print_status_cnt("0", cnt++);
}

void idle_thread() {
	while(1);
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, &idle_thread, KERNEL_ONLY, NUM_MUTEXES));

	printf("Successfully initialized threads...\n");

	ABORT_ON_ERROR(thread_create(&thread_0, 0, 100, 500, NULL));

	printf("Successfully created threads! Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY), "Threads are unschedulable!\n");

	while(1);

	return 0;
}
