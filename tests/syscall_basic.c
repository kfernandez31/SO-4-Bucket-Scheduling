#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>

#define NR_BUCKETS 10

void test_call(int bucket_nr) {
	errno = 0;
	int x = set_bucket(bucket_nr);
	int y = errno;
	int r = 1;
	if (bucket_nr >= 0 && bucket_nr < NR_BUCKETS) {
		if (x == 0 && y == 0) r = 0;
	} else {
		if (x == -1 && y == EINVAL) r = 0;
	}
	
	if (r == 1) {
		printf("WRONG\n");
		_exit(0);
	}
}

void test_all_nrs(void) {
	test_call(INT_MIN);
	test_call(SHRT_MIN);
	for (int i = 0; i < NR_BUCKETS / 2; i++) test_call(i);
	for (int i = -NR_BUCKETS / 2; i <= 1; i++) test_call(i);
	test_call(SHRT_MAX);
	test_call(USHRT_MAX);
	for (int i = NR_BUCKETS / 2; i <= NR_BUCKETS + 4; i++) test_call(i);
	test_call(INT_MAX);
}
