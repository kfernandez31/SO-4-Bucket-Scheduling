/* Procesy są wrzucane do jednego kubełka i wykonują tę samą ilość pracy.
 * Jeden z procesów szybko zmienia kubełek na inny. Powinien zakończyć działanie jako pierwszy.
 */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUCKET_NR 2
#define ALT_BUCKET_NR 3

#define BASE_NUM_ITERS ((uint64_t)(1500) * 1000 * 1000)
#define SMALL_NUM_ITERS ((uint64_t)(50) * 1000)
#define NR_JOBS 5

volatile uint64_t work_result;
void work(volatile uint64_t num_iters) {
	uint64_t a = 1;
	while (num_iters--) {
		a *= 3;
	}
	work_result = a;
}

void runner(uint64_t num_iters) {
	work(SMALL_NUM_ITERS);
	sleep(1);

	work(num_iters);
	_exit(0);
}

void runner_alt(uint64_t num_iters) {
	work(SMALL_NUM_ITERS);
	sleep(1);
	
	if (set_bucket(ALT_BUCKET_NR) < 0) {
		perror("set_bucket");
		exit(1);
	}
	
	work(num_iters);
	_exit(0);
}

void run_test(int num_jobs, uint64_t work_size, int *final_order) {
	int pids[num_jobs];

	for (int i = 0; i < num_jobs; i++) {
		pids[i] = fork();
		if (pids[i] < 0) {
			perror("fork");
			exit(1);
		} else if (pids[i] == 0) {
			if (set_bucket(BUCKET_NR) < 0) {
				perror("set_bucket");
				exit(1);
			}
			
			if (i + 1 == num_jobs) runner_alt(work_size);
			else runner(work_size);
			__builtin_unreachable();
		}
	}

	for (int i = 0; i < num_jobs; i++) {
		int wstatus;
		int child_pid = wait(&wstatus);
		if (child_pid == -1) {
			perror("wait");
			exit(1);
		}

		final_order[i] = -1;
		for (int j = 0; j < num_jobs; j++) {
			if (child_pid == pids[j]) {
				final_order[i] = j;
				printf("%c ", (char)('A' + j));
				fflush(stdout);
				break;
			}
		}
		assert(final_order[i] != -1);
	}
}


int main(void) {
	int final_order[NR_JOBS];

	run_test(NR_JOBS, BASE_NUM_ITERS, final_order);
	
	int ok = 0;
	if (final_order[0] + 1 == NR_JOBS) ok = 1;
	
	printf(ok ? "Test passed 0.5/0.5 pkt\n" : "Test failed 0/0.5 pkt\n");
	
	return 0;
}


