#include "ishm.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdatomic.h>

_Atomic unsigned int   sign_exit;

void HandleSignal(int signo) {
	unsigned int sign = 0;
	sign = atomic_load(&sign_exit);
    atomic_store(&sign_exit, sign + 1);
    printf("ctr + c times:%d\n", sign);
}

int main(int argc, char *argv[]) {
    key_t shmkey, semkey;
    int shmid = -1, semid = -1;
    char *shmaddr = NULL;
    int shmsz = 0x100000;
	unsigned int sign = 0;
    atomic_init(&sign_exit, 0);
    // ctrl+c 捕获
    signal(SIGINT, HandleSignal);
    signal(SIGTERM, HandleSignal);
    shmkey = ftok("/tmp/vpushm", 1);
    semkey = ftok("/tmp/vpusem", 1);

    shmid = ishm_ipc_shmget(shmkey, shmsz);
    if (shmid < 0) {
    	printf("shmid:%d\n", shmid);
    	return -1;
    }
    semid = ishm_ipc_semget(semkey, 2);
    if (semid < 0) {
    	printf("semid:%d\n", semid);
    	ishm_ipc_shmdel(shmid);
    	return -1;
    }

    shmaddr = (char*) ish_ipc_shmmap(shmid);
    ishm_ipc_semset(semid, 0, 1);
    ishm_ipc_semset(semid, 1, 0);

    ishm_ipc_sem_p(semid, 0);

    memset(shmaddr, 0, shmsz);
    printf("shmaddr:%lx\n", shmaddr);
    sprintf(shmaddr, "hello world, I am back!");
    ishm_ipc_sem_v(semid, 1);
    ishm_ipc_sem_p(semid, 0);

    ishm_ipc_shmunmap(shmaddr);
    ishm_ipc_shmdel(shmid);
    ishm_ipc_semdel(semid);
    printf("quit\n");

	return 0;
}