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

void shm_write() {
    key_t shmkey, semkey;
    int shmid = -1, semid = -1;
    char *shmaddr = NULL;
    int shmsz = 0x100000, send = 0;
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
	printf("shmaddr:%lx\n", shmaddr);
    ishm_ipc_semset(semid, 0, 1);
    ishm_ipc_semset(semid, 1, 0);
    while(1) {
    	sign = atomic_load(&sign_exit);
    	if (sign > 0) {
    		break;
    	}

	    memset(shmaddr, 0, shmsz);
	    ishm_ipc_sem_p(semid, 0);
	    sprintf(shmaddr, "hello world, I am back! times:%d", send++);
	    ishm_ipc_sem_v(semid, 1);
	}
	ishm_ipc_sem_p(semid, 0);
    ishm_ipc_shmunmap(shmaddr);
    ishm_ipc_shmdel(shmid);
    ishm_ipc_semdel(semid);
    printf("quit\n");	
}

int main(int argc, char *argv[]) {
//  shm_write();
	void* ihandle = NULL;
    int shmsz = 0x100000, flags = 1, send = 0, bufsize = 256;
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
    	bufsize = sprintf(buffer, "hello world! i come here in %d times", send++);
    	ipcshm_write(ihandle, buffer, bufsize);
    	sleep(1);
	}

	ipcshm_free(ihandle);
	printf("quit\n");
	return 0;
}