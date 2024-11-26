/** @file   test_6_1/main.c
 *
 *  @brief  user-space project "test_6_1", demonstrates mutex priority
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

/** @brief thread user space stack size - 4KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 1
#define NUM_MUTEXES 1
#define CLOCK_FREQUENCY 1000

/** @brief success flag */
volatile int success = 1;

/** @brief Thread 1(800, 1024)
 */
void thread(void* vargp) {
	mutex_t* s1 =(mutex_t*)vargp;

	if(s1 == NULL) {
		printf("Invalid mutex passed\n");
		return;
	}

	printf("Trying to lock mutex...\n");

	mutex_lock(s1);

	printf("Test failed! I shouldn't be able to lock it because my priority is too low.\n");
	success = 0;
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, PER_THREAD, NUM_MUTEXES));

	mutex_t* s1 = mutex_init(1);
	if(s1 == NULL) {
		printf("mutex_init failed.\n");
		return -1;
	}

	ABORT_ON_ERROR(thread_create(&thread, 0, 500, 500,(void*)s1));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	if(success) return RET_GOOD;
	else return RET_FAIL;
}
