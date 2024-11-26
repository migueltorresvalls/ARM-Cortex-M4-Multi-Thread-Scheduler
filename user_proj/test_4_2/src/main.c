/** @file   test_4_2/main.c
 *
 *  @brief  user-space project "test_4_2", demonstrates UB test with thread spawning
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output self-explanatory success/failure messages
 *  @note   thread 2 in set C should not be created due to UB test violation
**/

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 1KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 3
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1000

/** @brief Computation time of the threads */
#define THREAD_C_A { 100, 440, 180 }
#define THREAD_C_B { 100, 100, 380 }
#define THREAD_C_C { 100, 150, 450 }
/** @brief Period of the threads */
#define THREAD_T_A { 500, 1100, 1200 }
#define THREAD_T_B { 500, 700, 900 }
#define THREAD_T_C { 500, 750, 950 }

static const int THREAD_C_SETS[3][3] = {THREAD_C_A, THREAD_C_B, THREAD_C_C};
static const int THREAD_T_SETS[3][3] = {THREAD_T_A, THREAD_T_B, THREAD_T_C};

/** @brief thread return counters */
volatile int counters[ NUM_THREADS ] = {0, 0, 0};

void thread_fn(void* vargp) {
	int cnt = 0;
	int num = (int)vargp;
	while(cnt < 2) {
		print_num_status_cnt(num, cnt++);
		wait_until_next_period();
	}
	counters[num]++;
	return;
}

void thread_spawner(UNUSED void* vargp) {
	int cnt = 0;
	int set = 0;
	int status = 0;
	while(set < 3) {
		print_num_status_cnt(0, cnt++);
		if(counters[1] == set || counters[2] == set) {
			wait_until_next_period();
			continue;
		}

		set++;
		for(int i = 1; i < NUM_THREADS; i++) {
			status = thread_create(&thread_fn, i, THREAD_C_SETS[set][i], THREAD_T_SETS[set][i],(void*)i);

			if(set == 2 && i == 2) {
				if(!status) counters[0]++;
				return;
			}
			else if(status) printf("Failed on thread_create %d, set %d.\n", i, set);
		}

		wait_until_next_period();
	}

	counters[0]++;
	return;
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, KERNEL_ONLY, NUM_MUTEXES));

	ABORT_ON_ERROR(thread_create(&thread_spawner, 0, THREAD_C_SETS[0][0], THREAD_T_SETS[0][0], NULL));
	for(int i = 1; i < NUM_THREADS; ++i) {
		ABORT_ON_ERROR(thread_create(&thread_fn, i, THREAD_C_SETS[0][i], THREAD_T_SETS[0][i], (void*)i), "Thread %d\n", i);
	}

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	if(counters[0] == 0 && counters[1] == 3 && counters[2] == 2) {
		printf("Test passed!\n");
	} else {
		printf("Test failed.\n");
		return 1;
	}

	return 0;
}
