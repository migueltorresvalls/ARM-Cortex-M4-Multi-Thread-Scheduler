/** @file   grade_revive/main.c
 *
 *  @brief  user-space project "grade_revive", demonstrates RMS with thread revival
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 31 October 2023
 *  @author CMU 14-642
 *
 *  @output outputs thread status with thread-kill and thread-revive messages
**/

#include <lib642.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** @brief thread user space stack size - 1KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 5
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1000

typedef struct {
	int index;
} thread_var;

void thread_function(void* vargp) {
	int cnt = 0;
	thread_var* var = (thread_var*)vargp;

	while(1) {
		print_num_status_cnt(var->index, cnt++);

		if(get_priority() == 4) {
			// Thread 4 kills itself
			printf("Thread 4 killed itself\n");
			return;
		}

		wait_until_next_period();
		print_num_status_cnt(var->index, cnt++);
		wait_until_next_period();

		if(get_priority() == 3) {
			// thread 3 revives thread 4
			printf("Thread 3 revived thread 4\n");
			int status = thread_create(&thread_function, 4, 100, 500, &cnt);

			if(status) {
				printf("Failed to revive thread\n");
				exit(-1);
			}
		}
	}
}

int main(int argc, char* const argv[]) {
	int protection = 0;
	int opt;

	while((opt = getopt(argc, argv, "p:")) != -1) {
		switch(opt) {
		case 'p':
			protection = atoi(optarg);
			break;

		default:
			abort();
		}
	}

	ABORT_ON_ERROR(thread_init(NUM_THREADS, USR_STACK_WORDS, NULL, protection, NUM_MUTEXES));

	for(int i = 0; i < NUM_THREADS; ++i) {
		thread_var* thread_variable = malloc(sizeof(thread_var));

		if(thread_variable == NULL) {
			printf("Malloc failed for thread %d\n", i);
			return -1;
		}
		thread_variable->index = i;
		ABORT_ON_ERROR(thread_create(&thread_function, i, 50, 500, (void*)thread_variable));
	}

	printf("Starting scheduler...\n");

	ABORT_ON_ERROR(scheduler_start(CLOCK_FREQUENCY));

	return 0;
}
