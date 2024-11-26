/** @file   test_8_0/main.c
 *
 *  @brief  user-space project "test_8_0", demonstrates PCP priority inheritance
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @threads 	T0(100, 500), S0(0-75)
 *				T1(100, 500), S1(0-75)
 *              T2(750, 2000), S0(0-350), S1(0-545)
 *
 *  @output
 *              t=0	    Thread 0	Prio: 0	Cnt: 0
 *              t=0	    Thread 0	Prio: 0	Cnt: 1
 *              t=75	Thread 1	Prio: 1	Cnt: 0
 *              t=75	Thread 1	Prio: 1	Cnt: 1
 *              t=150	Thread 2	Prio: 2	Cnt: 0
 *              t=150	Thread 2	Prio: 2	Cnt: 1
 *              t=500	Thread 0	Prio: 0	Cnt: 2
 *              t=500	Thread 2	Prio: 0	Cnt: 2
 *              t=500	Thread 0	Prio: 0	Cnt: 3
 *              t=575	Thread 1	Prio: 1	Cnt: 2
 *              t=580	Thread 2	Prio: 1	Cnt: 3
 *              t=770	Thread 2	Prio: 1	Cnt: 4
 *              t=770	Thread 1	Prio: 1	Cnt: 3
 *              t=846	Thread 2	Prio: 2	Cnt: 5
 */

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 4KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 3
#define NUM_MUTEXES 2
#define CLOCK_FREQUENCY 100

typedef struct {
	mutex_t* mutex_0;
	mutex_t* mutex_1;
} arg_t;

void thread_0(void* vargp) {
	int cnt = 0;
	arg_t* arg =(arg_t*)vargp;

	while(1) {
		print_status_prio_cnt("0", cnt++);
		mutex_lock(arg->mutex_0);
		print_status_prio_cnt("0", cnt++);
		spin_wait(75);
		mutex_unlock(arg->mutex_0);

		if(cnt >= 3) {
			break;
		}

		wait_until_next_period();
	}
}

void thread_1(void* vargp) {
	int cnt = 0;
	arg_t* arg =(arg_t*)vargp;

	while(1) {
		print_status_prio_cnt("1", cnt++);
		mutex_lock(arg->mutex_1);
		print_status_prio_cnt("1", cnt++);
		spin_wait(75);
		mutex_unlock(arg->mutex_1);
		wait_until_next_period();
	}
}

void thread_2(void* vargp) {
	int cnt = 0;
	arg_t* arg =(arg_t*)vargp;

	print_status_prio_cnt("2", cnt++);
	mutex_lock(arg->mutex_1);
	mutex_lock(arg->mutex_0);
	print_status_prio_cnt("2", cnt++);
	spin_wait(350);         // Interrupted by T0
	print_status_prio_cnt("2", cnt++);
	mutex_unlock(arg->mutex_0);
	spin_wait(5);
	print_status_prio_cnt("2", cnt++);;
	spin_wait(190);         // interrupted by T1
	print_status_prio_cnt("2", cnt++);
	mutex_unlock(arg->mutex_1);
	spin_wait(1);
	print_status_prio_cnt("2", cnt++);
	exit(0);
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

	ABORT_ON_ERROR(thread_create(&thread_0, 0, 100, 500,(void*)arg));
	ABORT_ON_ERROR(thread_create(&thread_1, 1, 100, 500,(void*)arg));
	ABORT_ON_ERROR(thread_create(&thread_2, 2, 750, 2000,(void*)arg));


	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
