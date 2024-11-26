/** @file   test_7_3/main.c
 *
 *  @brief  user-space project "test_7_3", demonstrates nested mutex implementation
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @threads 	T0(20, 104), S0(0-10)
 *				T1(30, 200), S0(0-10), S1(0-10)
 *
 *  @output
 *				t=0	    Thread 0 locked 0
 *				t=8	    Thread 0 unlocked 0
 *				t=8	    Thread 1 locked 0
 *				t=8	    Thread 1 locked 1
 *				t=16	Thread 1 unlocked 0
 *				t=16	Thread 1 unlocked 1
 *				t=104	Thread 0 locked 0
 *				t=112	Thread 0 unlocked 0
 *				t=200	Thread 1 locked 0
 *				t=200	Thread 1 locked 1
 *				t=208	Thread 1 unlocked 0
 *				t=208	Thread 0 locked 0
 *				t=216	Thread 0 unlocked 0
 *				t=216	Thread 1 unlocked 1
 *				t=312	Thread 0 locked 0
 *				t=320	Thread 0 unlocked 0
 *				t=400	Thread 1 locked 0
 *				t=400	Thread 1 locked 1
 *				t=408	Thread 1 unlocked 0
 *				t=408	Thread 1 unlocked 1
 *				t=416	Thread 0 locked 0
 *				t=424	Thread 0 unlocked 0
 *				t=520	Thread 0 locked 0
 *				t=528	Thread 0 unlocked 0
 *				t=600	Thread 1 locked 0
 *				t=600	Thread 1 locked 1
 *				t=608	Thread 1 unlocked 0
 *				t=608	Thread 1 unlocked 1
 *				t=624	Thread 0 locked 0
 *				t=632	Thread 0 unlocked 0
 *
 */

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 1KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 2
#define NUM_MUTEXES 2
#define CLOCK_FREQUENCY 100

/** @brief How much to reduce spin wait */
#define REDUCE_SPIN 2

typedef struct {
	mutex_t* mutex_0;
	mutex_t* mutex_1;
} arg_t;

/** @brief Basic thread which locks and unlocks a mutex
 *	T0(20, 104)
 */
void thread_0(void* vargp) {
	arg_t* arg =(arg_t*)vargp;
	int cnt = 0;

	while(1) {
		cnt++;
		mutex_lock(arg->mutex_0);
		print_status("0 locked 0");

		spin_wait(10 - REDUCE_SPIN);

		mutex_unlock(arg->mutex_0);
		print_status("0 unlocked 0");

		if(cnt > 6) {
			break;
		}

		wait_until_next_period();
	}
}

/** @brief Basic thread which locks and unlocks a mutex
 *				 T1(30, 200)
 */
void thread_1(void* vargp) {
	arg_t* arg =(arg_t*)vargp;
	int cnt = 0;

	while(1) {
		cnt++;
		mutex_lock(arg->mutex_0);
		print_status("1 locked 0");
		mutex_lock(arg->mutex_1);
		print_status("1 locked 1");

		spin_wait(10 - REDUCE_SPIN);

		print_status("1 unlocked 0");
		mutex_unlock(arg->mutex_0);
		print_status("1 unlocked 1");
		mutex_unlock(arg->mutex_1);

		if(cnt > 3) {
			break;
		}

		wait_until_next_period();
	}
}

int main(UNUSED int argc, UNUSED char* const argv[]) {

	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, PER_THREAD, NUM_MUTEXES));

	mutex_t* s0 = mutex_init(0);
	if(s0 == NULL) {
		printf("Failed to create mutex 0\n");
		return -1;
	}

	mutex_t* s1 = mutex_init(1);
	if(s1 == NULL) {
		printf("Failed to create mutex 0\n");
		return -1;
	}

	arg_t* arg = malloc(sizeof(arg_t));
	if(arg == NULL) {
		printf("Malloc failed\n");
		return -1;
	}
	arg->mutex_0 = s0;
	arg->mutex_1 = s1;

	ABORT_ON_ERROR(thread_create(&thread_0, 0, 20, 104,(void*)arg));
	ABORT_ON_ERROR(thread_create(&thread_1, 1, 30, 200,(void*)arg));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
