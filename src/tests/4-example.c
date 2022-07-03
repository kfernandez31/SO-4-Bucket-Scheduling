/* Przykladowy test szeregowania.
 * Test jest parametryzowany liczbami calkowitymi `x` oraz `y`.
 * W tescie menedzer tworzy piec pomocniczych procesow A, B, C, D, E, gdzie:
 *   -- proces A nalezy do kubelka nr 1 i wykonuje jako swoja prace
 *         `x` iteracji petli,
 *   -- procesy B, C, D, E naleza do kubelka nr 2 i kazdy z nich wykonuje
 *          jako swoja prace po `y` iteracji petli.
 *
 * Po zakonczeniu pracy pomocniczy proces sie konczy, a menedzer odnotowuje
 *   kolejnosc, w ktorej prace konczyly procesy.
 *
 * W takim modelu proces A powinien wykonywac sie czterokrotnie szybciej niz
 *   procesy B, C, D, E.
 * Wobec tego test oczekuje, ze:
 *   -- dla parametrow `x = BASE_NUM_ITERS * 15` oraz `y = BASE_NUM_ITERS / 2`,
 *          procesy B, C, D, E zakoncza sie przed procesem A,
 *   -- dla parametrow `x = BASE_NUM_ITERS * 3` oraz `y = BASE_NUM_ITERS * 2`,
 *          procesy B, C, D, E zakoncza sie po procesie A.
 */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define BASE_NUM_ITERS ((uint64_t)(1500) * 1000 * 1000)
#define SMALL_NUM_ITERS ((uint64_t)(50) * 1000)
#define NUM_JOBS 5

/* Funkcja wykonujaca prace w procesach pomocniczych.
 * Praca jest modelowana poprzez wykonanie `num_iters` iteracji prostej petli.
 */
volatile uint64_t work_result;
void work(uint64_t num_iters) {
	uint64_t a = 1;
	while (num_iters--) {
		a *= 3;
	}
	work_result = a;
}


/* Glowna funkcja wykonywana przez proces pomocniczy. */
void runner(uint64_t num_iters) {
	/* Maly hack zwiekszajacy deterministycznosc rezultatow testu:
	 *   uruchamiamy petle wykonujaca bardzo mala ilosc pracy
	 *   i czekamy sekunde za pomoca funkcji `sleep`.
	 */
	work(SMALL_NUM_ITERS);
	sleep(1);

	/* Teraz wykonujemy glowna prace i konczymy proces. */
	work(num_iters);
	/* `_exit` zamiast `exit` wydaje sie byc bardziej deterministyczne:
	 *   druga funkcja czasami musi przywolac do pamieci fizycznej
	 *   czesc kodu biblioteki standardowej, co zuzywa kilka kwantow czasu.
	 */
	_exit(0);
}


/* Funkcja przeprowadzajaca test. Przyjmuje parametry `x` oraz `y`
 *   zgodnie z opisem w naglowku pliku.
 * Funkcja zapisuje (numerowana od 0) kolejnosc konczenia sie procesow
 *   w tablicy `final_order`.
 */
void run_test(uint64_t x, uint64_t y, int *final_order) {
	uint64_t work_sizes[NUM_JOBS] = {x, y, y, y, y};
	int bucket_nrs[NUM_JOBS] = {1, 2, 2, 2, 2};
	int pids[NUM_JOBS];

	/* Tworzymy NUM_JOBS pomocniczych procesow, ustawiamy im numery kubelkow
	 * i zlecamy im prace.
	 */
	for (int i = 0; i < NUM_JOBS; i++) {
		pids[i] = fork();
		if (pids[i] < 0) {
			perror("fork");
			exit(1);
		} else if (pids[i] == 0) {
			/* Proces-dziecko. */
			set_bucket(bucket_nrs[i]);
			runner(work_sizes[i]);
			__builtin_unreachable();
		}
		/* Proces-ojciec kontynuuje iterowanie petli. */
	}

	/* Czekamy na zakonczenie procesow i zapisujemy porzadek, w ktorym
	 * procesy sie zakonczyly.
	 */
	for (int i = 0; i < NUM_JOBS; i++) {
		int wstatus;
		int child_pid = wait(&wstatus);
		if (child_pid == -1) {
			perror("wait");
			exit(1);
		}

		final_order[i] = -1;
		for (int j = 0; j < NUM_JOBS; j++) {
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
	int final_order[NUM_JOBS];

	/* Podtest 1: A wykonuje wielokrotnie wiecej pracy niz B, C, D, E.
	 * Wymagamy, by A zakonczyl sie jako ostatni.
	 */
	printf("Podtest 1: ");
	fflush(stdout);
	run_test(BASE_NUM_ITERS * 15, BASE_NUM_ITERS / 2, final_order);
	printf(" => %s\n", (final_order[NUM_JOBS - 1] == 0) ? "OK" : "WRONG");

	/* Podtest 2: A wykonuje 1.5x wiecej pracy niz B, C, D, E.
	 * Wymagamy, by A zakonczyl sie jako pierwszy.
	 */
	printf("Podtest 2: ");
	fflush(stdout);
	run_test(BASE_NUM_ITERS * 3, BASE_NUM_ITERS * 2, final_order);
	printf(" => %s\n", (final_order[0] == 0) ? "OK" : "WRONG");

	return 0;
}

