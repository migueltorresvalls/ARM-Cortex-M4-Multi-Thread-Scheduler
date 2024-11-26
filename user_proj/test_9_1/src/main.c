/** @file   test_9_1/main.c
 *
 *  @brief  user-space project "test_9_1", written question 2
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @threads 	T0(300, 700), S0(100-300)
 *				T1(100, 900), S1(0-100)
 *              T2(600, 2600), S0(0-400), S1(200-600)
 *
 *  @output self-explanatory success/failure messages
 */

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define USR_STACK_WORDS 256
#define NUM_THREADS 3
#define NUM_MUTEXES 2
#define CLOCK_FREQUENCY 200

/** @brief How much to reduce spin wait */
#define REDUCE_SPIN 2

#define TEST_DEBUG

typedef struct {
	mutex_t* mutex_0;
	mutex_t* mutex_1;
} arg_t;

/** @brief print a single char
 *
 *	@param c	 the character
 */
void print_char(int c) {
	putchar(c);
#ifdef TEST_DEBUG
	printf("--Prio %d, t=%d\n", (int)get_priority(), (int)get_time());
#endif
}

/** @brief T0:(300, 700), S0(100-300)
 */
void thread_0(void* vargp) {
	arg_t* arg =(arg_t*)vargp;
	int cnt = 0;

	while(cnt < 6) {
		print_char('S');
		spin_wait(95);
		mutex_lock(arg->mutex_0);
		print_char('C');
		spin_wait(190);
		mutex_unlock(arg->mutex_0);
		cnt++;
		wait_until_next_period();
	}
}


/** @brief T1:(100, 900)
 */
void thread_1(void* vargp) {
	arg_t* arg =(arg_t*)vargp;
	int cnt = 0;

	while(cnt < 4) {
		mutex_lock(arg->mutex_1);
		spin_wait(45);
		print_char('H');
		spin_wait(45);
		mutex_unlock(arg->mutex_1);
		cnt++;
		wait_until_next_period();
	}

	printf(" - Obtained\n");
	printf("SCHEDSUCLHSCHSCESDUCLSCH - Expected\n");
}

/** @brief T2:(600, 2600), S0(0-400), S1(200-600)
 */
void thread_2(void* vargp) {
	arg_t* arg =(arg_t*)vargp;
	int cnt = 0;

	while(cnt < 2) {
		mutex_lock(arg->mutex_0);
		print_char('E');
		spin_wait(200);
		mutex_lock(arg->mutex_1);
		print_char('D');
		spin_wait(150);
		print_char('U');
		spin_wait(45);
		mutex_unlock(arg->mutex_0);
		spin_wait(145);
		print_char('L');
		spin_wait(45);
		mutex_unlock(arg->mutex_1);
		cnt++;
		wait_until_next_period();
	}
}

int main() {

	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, PER_THREAD, NUM_MUTEXES));

	mutex_t* mutex_0 = mutex_init(0);
	if(mutex_0 == NULL) {
		printf("Failed to create mutex 0\n");
		return -1;
	}

	mutex_t* mutex_1 = mutex_init(1);
	if(mutex_1 == NULL) {
		printf("Failed to create mutex 1\n");
		return -1;
	}

	arg_t* arg = malloc(sizeof(arg_t));
	if(arg == NULL) {
		printf("Malloc on arg failed\n");
		return -1;
	}
	arg->mutex_0 = mutex_0;
	arg->mutex_1 = mutex_1;

	ABORT_ON_ERROR(thread_create(&thread_0, 0, 300, 700,(void*)arg));
	ABORT_ON_ERROR(thread_create(&thread_1, 1, 100, 900,(void*)arg));
	ABORT_ON_ERROR(thread_create(&thread_2, 2, 600, 2600,(void*)arg));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return RET_0642;
}
