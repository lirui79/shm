#include "ishm.h"
#include "ipcshm.h"

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


void shm_read() {
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
    while(1) {
    	sign = atomic_load(&sign_exit);
    	if (sign > 0) {
    		break;
    	}
	    ishm_ipc_sem_p(semid, 1);
	    printf("shmaddr:%s\n", shmaddr);
	    ishm_ipc_sem_v(semid, 0);
	}

    ishm_ipc_shmunmap(shmaddr);
    printf("quit\n");
}

int main(int argc, char *argv[]) {
//	shm_read();
	void* ihandle = NULL;
    int shmsz = 0x100000, flags = 0, recv = 0, bufsize = 256;
	unsigned int sign = 0;
	char buffer[256] = {0};
    atomic_init(&sign_exit, 0);
    // ctrl+c 捕获
    signal(SIGINT, HandleSignal);
    signal(SIGTERM, HandleSignal);
	ihandle = ipcshm_alloc("/tmp/vpuipc", 0, shmsz, flags);
	if (ihandle == NULL) {
		printf("ipcshm_alloc wrong\n");
		return -1;
	}

    while(1) {
    	sign = atomic_load(&sign_exit);
    	if (sign > 0) {
    		break;
    	}
    	memset(buffer, 0, 256);
    	bufsize = ipcshm_read(ihandle, buffer, bufsize);
    	printf("%s - %d recv:%d times\n", buffer, bufsize, recv++);
    	sleep(1);
	}

	ipcshm_free(ihandle);
	printf("quit\n");
	return 0;
}