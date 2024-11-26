/** @file   grade_recursion/main.c
 *
 *  @brief  user-space project "grade_recursion", demonstrates stack overflow detection
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @usage  include USER_ARG with recursion depth n and stack size s parameters, e.g.:
 *              make run USER_PROJ=grade_recursion USER_ARG="-n 200 -s 512"
 *
 *  @output prints multiples of 7, aborting if recursion depth is too large for stack and overflow occurs
**/

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 1
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 200

uint32_t multiply7(uint32_t n) {
	if(n < 1)
		return 0;
	else
		return multiply7(n - 1) + 7;
}

// Attempt stack overflow via recursion
void overflow_function(void* vargp) {
	uint32_t end = *(uint32_t*)vargp;
	for(uint32_t i = 0; i < end; i++) {
		printf("7 * %lu = %lu\n", i, multiply7(i));
	}
}

int main(int argc, char* const argv[]) {
	int n = 0;
	int s = 256;
	int opt;

	while((opt = getopt(argc, argv, "n:s:")) != -1) {
		switch(opt) {
		case 'n':
			n = atoi(optarg);
			break;

		case 's':
			s = atoi(optarg);
			break;

		default:
			abort();
		}
	}

	ABORT_ON_ERROR(thread_init(NUM_THREADS, s, NULL, PER_THREAD, NUM_MUTEXES));

	int vargp = n;
	ABORT_ON_ERROR(thread_create(&overflow_function, 0, 400, 500, &vargp));

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
