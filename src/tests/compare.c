/* Test zawiera kilka scenariuszy, w których porównywane są czasy działania programów
 * w zależności od ilości pracy i rozłożenia jej między kubełkami.
 * 
 * Każdy podtest jest warty 0,3 punktu.
 */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define BASE_NUM_ITERS ((uint64_t)(1500) * 1000 * 1000)
#define SMALL_NUM_ITERS ((uint64_t)(50) * 1000)
#define MAX_JOBS 20

volatile uint64_t work_result;
void work(volatile uint64_t num_iters) {
	uint64_t a = 1;
	while (num_iters--) {
		a *= 3;
	}
	work_result = a;
}

void runner(uint64_t num_iters, int niceval) {
	nice(niceval);
	work(SMALL_NUM_ITERS);
	sleep(1);

	work(num_iters * BASE_NUM_ITERS);
	_exit(0);
}

void run_test(int num_jobs, uint64_t *work_sizes, int *bucket_nrs, int *final_order) {
	int pids[num_jobs];

	for (int i = 0; i < num_jobs; i++) {
		pids[i] = fork();
		if (pids[i] < 0) {
			perror("fork");
			exit(1);
		} else if (pids[i] == 0) {
			set_bucket(bucket_nrs[i]);
			runner(work_sizes[i], i * 8 - 20);
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
	uint64_t work_sizes[MAX_JOBS];
	int bucket_nrs[MAX_JOBS];
	int final_order[MAX_JOBS];

	// Podtest 1: każdy proces do innego kubełka, różna ilość wykonywanej pracy.
	// Sprawdzamy końcową kolejność.
	work_sizes[0] = 4;
	work_sizes[1] = 1;
	work_sizes[2] = 3;
	work_sizes[3] = 6;
	
	bucket_nrs[0] = 7;
	bucket_nrs[1] = 3;
	bucket_nrs[2] = 4;
	bucket_nrs[3] = 8;

	printf("Podtest 1: ");
	fflush(stdout);
	run_test(4, work_sizes, bucket_nrs, final_order);
	
	int ok = 0, tests_passed = 0;
	if (final_order[0] == 1 && final_order[1] == 2 && final_order[2] == 0 && final_order[3] == 3) ok = 1;
	printf(" => %s\n", ok ? "OK" : "WRONG");
	tests_passed += ok;

	// Podtest 2: w jednym z kubełków umieszczamy dwa procesy.
	// Pierwszy powinien skończyć się po 4 jednostkach czasu, drugi po ośmiu.
	work_sizes[0] = 2;
	work_sizes[1] = 6;
	
	bucket_nrs[0] = bucket_nrs[1] = 4;
	
	work_sizes[2] = 3;
	work_sizes[3] = 6;
	
	bucket_nrs[2] = 1;
	bucket_nrs[3] = 2;
	
	printf("Podtest 2: ");
	fflush(stdout);
	
	run_test(4, work_sizes, bucket_nrs, final_order);
	
	ok = 0;
	if (final_order[0] == 2 && final_order[1] == 0 && final_order[2] == 3 && final_order[3] == 1) ok = 1;
	
	printf(" => %s\n", ok ? "OK" : "WRONG");
	tests_passed += ok;

	// Podtest 3: umieszczamy dwa procesy w jednym kubełku, a trzy procesy w drugim.
	bucket_nrs[0] = bucket_nrs[1] = bucket_nrs[2] = 7;
	bucket_nrs[3] = bucket_nrs[4] = 9;
	
	// Momenty ukończenia procesów: 12, 16, 20, 18, 14.
	work_sizes[0] = 4;
	work_sizes[1] = 6;
	work_sizes[2] = 10;
	
	work_sizes[3] = 11;
	work_sizes[4] = 7;
	
	printf("Podtest 3: ");
	fflush(stdout);
	
	run_test(5, work_sizes, bucket_nrs, final_order);
	
	ok = 0;
	if (final_order[0] == 0 && final_order[1] == 4 && final_order[2] == 1
	    && final_order[3] == 3 && final_order[4] == 2) ok = 1;
	
	printf(" => %s\n", ok ? "OK" : "WRONG");
	tests_passed += ok;

	// Podtest 4: jeden działający długo proces i dużo działających krótko, ale wszystkie razem w jednym kubełku.
	// Długo działający proces powinien zakończyć się pierwszy.
	
	bucket_nrs[0] = bucket_nrs[1] = bucket_nrs[2] = bucket_nrs[3] = bucket_nrs[4]
	= bucket_nrs[5] = bucket_nrs[6] = bucket_nrs[7] = bucket_nrs[8] = 1;
	
	work_sizes[0] = work_sizes[1] = work_sizes[2] = work_sizes[3] = work_sizes[4]
	= work_sizes[5] = work_sizes[6] = work_sizes[7] = work_sizes[8] = 1;
	
	bucket_nrs[9] = 3;
	work_sizes[9] = 6;
	
	printf("Podtest 4: ");
	fflush(stdout);
	
	run_test(10, work_sizes, bucket_nrs, final_order);
	if (final_order[0] == 9) ok = 1;
	else ok = 0;
	
	printf(" => %s\n", ok ? "OK" : "WRONG");
	tests_passed += ok;
	
	// Podtest 5: mamy po trzy procesy o czasach działania 2 i 3.
	// Umieszczamy dwa w jednym kubełku, dwa w innym, a pozostałe dwa w osobnych.
	// Procesy umieszczone w osobnych kubełkach powinny działać szybciej.
	
	work_sizes[0] = work_sizes[1] = work_sizes[2] = 4;
	work_sizes[3] = work_sizes[4] = work_sizes[5] = 6;
	
	bucket_nrs[0] = bucket_nrs[3] = 9;
	bucket_nrs[1] = bucket_nrs[4] = 2;
	bucket_nrs[2] = 3;
	bucket_nrs[5] = 1;
	
	printf("Podtest 5: ");
	fflush(stdout);
	
	run_test(6, work_sizes, bucket_nrs, final_order);
	
	// Jedna z oczekiwanych kolejności to CFABDE
	// Można zamienić A z B lub D z E.
	
	ok = 1;
	if (final_order[0] != 2 || final_order[1] != 5) ok = 0;
	if (final_order[2] != 0 && final_order[2] != 1) ok = 0;
	if (final_order[3] != 0 && final_order[3] != 1) ok = 0;
	if (final_order[4] != 3 && final_order[4] != 4) ok = 0;
	if (final_order[5] != 3 && final_order[5] != 4) ok = 0;
	
	printf(" => %s\n", ok ? "OK" : "WRONG");
	tests_passed += ok;

	printf("\n\n===============\n%d / 5 tests passed, %.2lf / 1.5 points\n", tests_passed, tests_passed * 0.3);

	return 0;
}


