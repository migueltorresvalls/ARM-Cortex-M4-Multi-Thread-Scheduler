/** @file   test_5_0/main.c
 *
 *  @brief  user-space project "test_5_0", demonstrates erroneous read/write detection
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output stack exhaustion triggers MPU fault and kills thread
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

// Attempt a buffer overflow attack
void thread_function(UNUSED void* vargp) {
	int buffer[USR_STACK_WORDS / 2];
	int i = 0;

	while(1) {
		printf("i=%d\tAddr=%p\tVal=%d\n", i, &buffer[i], buffer[i]);
		i++;
	}
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, PER_THREAD, NUM_MUTEXES));

	ABORT_ON_ERROR(thread_create(&thread_function, 0, 400, 500, NULL));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
