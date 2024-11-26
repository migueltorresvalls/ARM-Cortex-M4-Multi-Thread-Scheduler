/** @file   test_1_0/main.c
 *
 *  @brief  user-space project "test_1_0", demonstrates vargp usage for argument passage
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output phrase will be output correctly if everything works
**/

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 4KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS     14
#define NUM_MUTEXES     0
#define CLOCK_FREQUENCY 1000

/** @brief Computation time of each task (they are all the same) */
#define THREAD_TIME  100
/** @brief Period of each thread (they are all the same) */
#define THREAD_PERIOD   (NUM_THREADS * THREAD_TIME * 5)

char* names[] = {
	"This ", "is ", "a ", "test ", "that ",
	"ensures ", "args ", "given ", "to ", "the ",
	"thread ", "fns ", "are ", "working.\n"
};

void thread_fn(UNUSED void* vargp) {
	int cnt = 0;

	while(1) {
		printf("%s", (char*)vargp);   // use "print_status_cnt((char*)vargp , cnt++);" to help with debugging
		spin_until(++cnt * THREAD_PERIOD - 1);
	}
}

void idle_thread() {
	while(1);
}

int main(UNUSED int argc, UNUSED char* const argv[]) {
	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, &idle_thread, KERNEL_ONLY, NUM_MUTEXES));

	int i;
	for(i = 0; i < NUM_THREADS; ++i) {
		ABORT_ON_ERROR(thread_create(&thread_fn, i, THREAD_TIME, THREAD_PERIOD,(void *)names[i]), "Failed to create thread %d\n", i);
	}

	printf("Successfully created threads! Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;

}
