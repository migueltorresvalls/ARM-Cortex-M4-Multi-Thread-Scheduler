/** @file   test_6_2/main.c
 *
 *  @brief  user-space project "test_6_2", demonstrates mutex exclusivity
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
#define NUM_THREADS 2
#define NUM_MUTEXES 1
#define CLOCK_FREQUENCY 1000

/** @brief success flag */
volatile int success = 1;
/** @brief Shared variable protected by mutex. */
volatile int shared = 0;

/** @brief Thread 0(50, 100)
 */
void thread_0(void* vargp) {

	mutex_t* s1 = (mutex_t*)vargp;
	shared = 0;
	int pid = getpid();

	while(1) {
		if(shared > 8) {
			break;
		}

		mutex_lock(s1);
		print_num_status_cnt(pid, shared++);
		mutex_unlock(s1);
		wait_until_next_period();
	}
}

/** @brief Thread 1(1000, 2500)
 */
void thread_1(void* vargp) {
	int i;
	mutex_t* s1 = (mutex_t*)vargp;
	mutex_lock(s1);
	printf("Nothing should print between now and UNLOCK...\n");
	i = shared;
	spin_wait(950);
	if(i != shared) success = 0;
	printf("UNLOCK.\n");
	mutex_unlock(s1);
	wait_until_next_period();
	mutex_lock(s1);
	printf("Nothing should print between now and UNLOCK...\n");
	i = shared;
	spin_wait(950);
	if(i != shared) success = 0;
	printf("UNLOCK.\n");
	mutex_unlock(s1);
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, PER_THREAD, NUM_MUTEXES));

	mutex_t* s1 = mutex_init(0);

	ABORT_ON_ERROR(thread_create(&thread_0, 0, 50, 500, (void*)s1));

	ABORT_ON_ERROR(thread_create(&thread_1, 1, 1000, 2500, (void*)s1));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	if(success) return RET_GOOD;
	else return RET_FAIL;
}
