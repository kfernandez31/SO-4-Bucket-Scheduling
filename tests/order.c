#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int semid;

union semun {
	int              val;
	struct semid_ds *buf;
	unsigned short  *array;
	struct seminfo  *__buf;
};

void create_semaphore() {
	semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
	if (semid == -1) {
		perror("semget");
		exit(1);
	}

	union semun arg;
	arg.val = 0;
	int ret = semctl(semid, 0, SETVAL, arg);
	if (ret == -1) {
		perror("semctl (create_semaphore)");
		exit(1);
	}
}

void destroy_semaphore() {
	int ret;
	ret = semctl(semid, 0, IPC_RMID);
	if (ret == -1) {
		perror("semctl (destroy_semaphore)");
		exit(1);
	}
}

void wait_semaphore(int op) {
	struct sembuf sop = {
		.sem_num = 0,
		.sem_op = -1,
		.sem_flg = 0
	};
	int ret = semop(semid, &sop, 1);
	if (ret == -1) {
		if (errno != EIDRM) {
			perror("semop");
			exit(1);
		}
	}
}

void child_job(int bucket_id) {
	printf("%d", bucket_id);
	fflush(stdout);
	set_bucket(bucket_id);
	wait_semaphore(-1);
	printf("%d", bucket_id);
	fflush(stdout);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		exit(1);
	}

	create_semaphore();
	printf("Semaphore id: %d\n", semid);

	int num_children = 0;
	for (char *ch = argv[1]; *ch; ++ch) {
		if (!isdigit(*ch))
			continue;
		pid_t fork_ret = fork();
		if (fork_ret < 0) {
			perror("fork");
			exit(1);
		} else if (fork_ret == 0) {
			child_job(*ch - '0');
			exit(0);
		} else {
			++num_children;
		}
	}

	sleep(1);
	printf("|"); fflush(stdout);
	destroy_semaphore();

	while (num_children > 0) {
		int status;
		wait(&status);
		--num_children;
	}

	printf("\n");
	return 0;
}
