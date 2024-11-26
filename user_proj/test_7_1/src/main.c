/** @file   test_7_1/main.c
 *
 *  @brief  user-space project "test_7_1", demonstrates mutex priority deyielding
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @threads 	T1(20, 100), S1(0-20)
 *				T2(150, 1000), S1(0-110)
 *
 *  @output     thread 1 should be preempted immediately after unlocking and should
 *              print "non-critical" section only after thread 0 runs
 */

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 1KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 2
#define NUM_MUTEXES 1
#define CLOCK_FREQUENCY 100

/** @brief How much to reduce spin wait */
#define REDUCE_SPIN 2

/** @brief Basic thread which locks and unlocks a mutex
 *				 T1(20, 100), S1(0-20)
 */
void thread_0(void* vargp) {
	int cnt = 0;
	mutex_t* mutex = (mutex_t*)vargp;

	while(1) {
		cnt++;
		mutex_lock(mutex);
		print_status("0 locked");

		spin_wait(20 - REDUCE_SPIN);

		print_status("0 unlocked");
		mutex_unlock(mutex);

		if(cnt > 12) {
			break;
		}

		wait_until_next_period();
	}
}

/** @brief Basic thread which locks and unlocks a mutex
 *				 T1(150, 1000), S1(0-110)
 */
void thread_1(void* vargp) {
	mutex_t* mutex = (mutex_t*)vargp;
	int cnt = 0;

	while(1) {
		cnt++;
		mutex_lock(mutex);
		print_status("1 locked");

		spin_wait(110 - REDUCE_SPIN);

		print_status("1 unlocked");
		printf("Task 0 should happen now!\n");
		mutex_unlock(mutex);
		print_status("1 non-critical section");
		spin_wait(30 - REDUCE_SPIN);

		if(cnt >= 2) {
			break;
		}

		wait_until_next_period();
	}
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, PER_THREAD, NUM_MUTEXES));

	mutex_t* s1 = mutex_init(0);
	if(s1 == NULL) {
		printf("Failed to create mutex 0\n");
		return -1;
	}

	ABORT_ON_ERROR(thread_create(&thread_0, 0, 20, 100,(void*)s1));
	ABORT_ON_ERROR(thread_create(&thread_1, 1, 150, 1000,(void*)s1));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
