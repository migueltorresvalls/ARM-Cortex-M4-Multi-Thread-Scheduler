/** @file   test_7_2/main.c
 *
 *  @brief  user-space project "test_7_2", demonstrates nested mutex implementation
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @threads 	T0(100, 500), S0(0-90)
 *				T1(10000, 1000000), S0(0-600), S1(0-)
 *
 *  @output     thread 1 should be preempted while mutex 1 is still locked
 
 *				t=0	    Thread 0 locked 0
 *				t=88	Thread 0 unlocked 0
 *				t=88	Thread 1 locked 1
 *				t=88	Thread 1 locked 0
 *				t=88	Thread 1 unlocked 0
 *				t=500	Thread 0 locked 0
 *				t=588	Thread 0 unlocked 0
 *				t=774	Thread 1 unlocked 1
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
	int done;
} arg_t;

/** @brief Basic thread which locks and unlocks a mutex
 *	T0(100, 500)
 */
void thread_0(void* vargp) {
	arg_t* arg =(arg_t*)vargp;

	while(1) {
		if(arg->done < 2) {
			mutex_lock(arg->mutex_0);
			print_status("0 locked 0");

			spin_wait(90 - REDUCE_SPIN);

			print_status("0 unlocked 0");
			mutex_unlock(arg->mutex_0);
			arg->done++;

			wait_until_next_period();
		}
		else {
			exit(-1);
		}
	}
}

/** @brief Basic thread which locks and unlocks a mutex
 *				 T1(10000, 1000000)
 */
void thread_1(void* vargp) {
	arg_t* arg =(arg_t*)vargp;

	while(1) {
		mutex_lock(arg->mutex_1);
		print_status("1 locked 1");

		mutex_lock(arg->mutex_0);
		print_status("1 locked 0");

		print_status("1 unlocked 0");
		mutex_unlock(arg->mutex_0);

		// we should be preempted here by task 1...
		spin_wait(600 - REDUCE_SPIN);

		print_status("1 unlocked 1");
		mutex_unlock(arg->mutex_1);
		exit(0);
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
		printf("Failed to create mutex 1\n");
		return -1;
	}

	arg_t* arg = malloc(sizeof(arg_t));
	if(arg == NULL) {
		printf("Malloc failed\n");
		return -1;
	}
	arg->mutex_0 = s0;
	arg->mutex_1 = s1;
	arg->done = 0;

	ABORT_ON_ERROR(thread_create(&thread_0, 0, 100, 500,(void*)arg));
	ABORT_ON_ERROR(thread_create(&thread_1, 1, 10000, 1000000,(void*)arg));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return -1;
}
