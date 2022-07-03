#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

extern void test_call(int bucket_nr);
extern void test_all_nrs(void);

void sighandler(int signum __attribute__((unused))) {
	test_call(-1);
	test_call(0);
	test_call(9);
	test_call(10);
}

void child_process(void) {
	struct sigaction act;
	sigset_t block_mask;
	if (sigemptyset(&block_mask) < 0) {
		perror("sigemptyset");
		exit(1);
	}
	
	memset(&act, 0, sizeof(act));
	act.sa_handler = sighandler;
	act.sa_mask = block_mask;
	act.sa_flags = 0;
	
	if (sigaction(SIGTERM, &act, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}
	
	int x;
	ssize_t r = read(0, &x, sizeof(int));
	if (r >= 0) {
		printf("error: read should fail\n");
		exit(1);
	}
	
	if (errno != EINTR) {
		perror("read");
		exit(1);
	}

	_exit(0);
}

int main(void) {
	test_all_nrs();
	int pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		child_process();
		__builtin_unreachable();
	}
	
	sleep(1);
	if (kill(pid, SIGTERM) < 0) {
		perror("kill");
		exit(1);
	}
	
	int wstatus;
	int cpid = wait(&wstatus);
	if (cpid != pid) {
		perror("wait");
		exit(1);
	}
	
	if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus) != 0) {
		printf("strange exit of child process\n");
		exit(1);
	}
	
	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		printf("OK from child\n");
		exit(0);
	} else {
		printf("OK from parent\n");
	}
}
