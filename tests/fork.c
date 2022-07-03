/* Test tworzy za pomocą wywołania `fork` trzy łańcuchy procesów.
 * Każdy proces w łańcuchu (oprócz ostatniego) najpierw wywołuje `fork`,
 * a następnie wywołuje przypadającą na niego część pracy. W ten sposób
 * wszystkie procesy będą działać jednocześnie.
 * 
 * Po zakończeniu pracy każdy proces czeka na swojego potomka.
 * Gdy wszystkie procesy w łańcuchu zakończą działanie, wypisywany jest
 * odpowiedni identyfikator. Kolejność wypisania identyfikatorów
 * decyduje, czy test jest zaliczany.
 * 
 * Jeżeli procesy dziedziczą priorytety po swoim przodku
 * i zaimplementowany jest poprawny algorytm szeregowania,
 * to w obu podzadaniach jako pierwszy zakończy się łańcuch C.
 */

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>

#define SMALL_NUM_ITERS ((uint64_t)(50) * 1000)
#define BASE_NUM_ITERS ((uint64_t)(1500) * 1000 * 1000)
#define CHAINS 3

/* funkcje pomocnicza (skopiowana z testu przykładowego) */
volatile uint64_t work_result;
void work(uint64_t num_iters) {
	uint64_t a = 1;
	while (num_iters--) {
		a *= 3;
	}
	work_result = a;
}

void chain_job(int jobs_count, uint64_t iters) {
	pid_t child_pid = 0;
	if (jobs_count > 0) {
		child_pid = fork();
		if (child_pid < 0) {
			perror("fork");
			exit(1);
		} else if (child_pid == 0) {
			chain_job(jobs_count - 1, iters);
			__builtin_unreachable();
		}
	}
	
	work(iters);
	
	if (child_pid > 0) {
		int wstatus;
		pid_t pid = wait(&wstatus);
		assert(pid == child_pid);
	}
	
	_exit(0);
}

void start_chain(int pos, int jobs_count, pid_t *pids, uint64_t iters, int bucket_nr) {
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		assert(set_bucket(bucket_nr) == 0);
		work(SMALL_NUM_ITERS);
		sleep(1);
		chain_job(jobs_count - 1, iters);
	} else {
		pids[pos] = pid;
	}
}

void subtask(int subtask_nr, pid_t *pids) {
	if (subtask_nr == 1) {
		start_chain(0, 3, pids, 2 * BASE_NUM_ITERS, 4);
		start_chain(1, 3, pids, 2 * BASE_NUM_ITERS, 4);
		start_chain(2, 8, pids, BASE_NUM_ITERS, 7);
	} else {
		start_chain(0, 7, pids, BASE_NUM_ITERS, 3);
		start_chain(1, 7, pids, BASE_NUM_ITERS, 3);
		start_chain(2, 3, pids, 3 * BASE_NUM_ITERS, 8);
	}
}

void do_subtask(int nr) {
	int order[CHAINS];
	pid_t pids[CHAINS];
	
	subtask(nr, pids);
	
	for (int i = 0; i < CHAINS; i++) {
		pid_t pid = wait(NULL);
		order[i] = -1;
		for (int j = 0; j < CHAINS; j++) {
			if (pid == pids[j]) {
				order[i] = j;
				printf("%c ", (char)('A' + j));
				fflush(stdout);
			}
		}
		
		assert(order[i] != -1);
	}
	
	printf("\nSubtask %d: ", nr);
	
	if (order[0] == 2) printf("passed\n");
	else printf("failed\n");
}

int main(void) {
	do_subtask(1);
	do_subtask(2);
}
