#include "ishm.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>


/*
错误原因存于errno中
EINVAL：参数size小于SHMMIN或大于SHMMAX
EEXIST：预建立key所指的共享内存，但已经存在
EIDRM ：参数key所指的共享内存已经删除
ENOSPC：超过了系统允许建立的共享内存的最大值(SHMALL)
ENOENT：参数key所指的共享内存不存在，而参数shmflg未设IPC_CREAT位
EACCES：没有权限
ENOMEM：核心内存不足
*/
int     ishm_ipc_shmget(key_t key, int shmsz) {
	int shmid = shmget(key, shmsz, IPC_CREAT | IPC_EXCL | 0666);//
	if (shmid == -1) {
		switch(errno) {
		  case EINVAL:
		  	return -1;
		  case EEXIST:
		  	shmid = shmget(key, shmsz, IPC_CREAT | 0666);
		  	return shmid;
		  case EIDRM:
		  	return -2;
		  case ENOSPC:
		  	return -3;
		  case ENOENT:
		  	return -4;
		  case EACCES:
		  	return -5;
		  case ENOMEM:
		  	return -6;
		  default:
		  	break;
		}
	}

	return shmid;
}

void*   ish_ipc_shmmap(int shmid) {
	void* shmaddr = NULL;
	if (shmid < 0) {
		return shmaddr;
	}
	shmaddr = shmat(shmid, NULL, 0);
	if (shmaddr == (void*) -1) {
		return NULL;
	}
	return shmaddr;
}

int     ishm_ipc_shmunmap(const void* shmaddr) {
	if (shmaddr == NULL) {
		return -2;
	}
	return shmdt(shmaddr);
}

int     ishm_ipc_shmdel(int shmid) {
	if (shmid < 0) {
		return -2;
	}
	return shmctl(shmid, IPC_RMID, NULL);
}


union semun {
    int val;                //使用的值
    struct semid_ds *buf;   //IPC_STAT、IPC_SET 使用的缓存区
    unsigned short *arry;   //GETALL,、SETALL 使用的数组
    struct seminfo *__buf;  // IPC_INFO(Linux特有) 使用的缓存区
};

/*
错误原因存于errno中
EACCES
密钥有一个信号集，但是调用进程没有访问该集的权限，并且在管理其IPC名称空间的用户名称空间中不具有CAP_IPC_OWNER功能。
EEXIST
在semflg中指定了IPC_CREAT和IPC_EXCL，但是密钥的信号量集已经存在。
EINVAL
nsems小于0或大于每个信号量集(SEMMSL)的信号量限制。
EINVAL
对应于key的信号量集已经存在，但是nsems大于该键集中的信号量。
ENOENT
没有为键设置信号量，并且semflg未指定IPC_CREAT。
ENOMEM
必须创建一个信号集，但是系统没有足够的内存来存储新的数据结构。
ENOSPC
必须创建一个信号集，但是将超出最大信号集集(SEMMNI)的系统限制或系统范围内最大信号量(SEMMNS)。
*/
int     ishm_ipc_semget(key_t key, int nsems) {
	int semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);//
	if (semid == -1) {
		switch(errno) {
		  case EINVAL:
		  	return -1;
		  case EEXIST:
		  	semid = semget(key, nsems, IPC_CREAT | 0666);
		  	return semid;
		  case EIDRM:
		  	return -2;
		  case ENOSPC:
		  	return -3;
		  case ENOENT:
		  	return -4;
		  case EACCES:
		  	return -5;
		  case ENOMEM:
		  	return -6;
		  default:
		  	break;
		}
	}

	return semid;
}


int     ishm_ipc_semset(int semid, int semnum, int semunval) {
	union semun semarg;
	if (semid < 0) {
		return -2;
	}
	semarg.val = semunval;
	return semctl(semid, semnum, SETVAL, semarg);
}

int     ishm_ipc_sem_p(int semid, int semnum) {
	struct sembuf sbuf;
	if (semid < 0) {
		return -2;
	}
	sbuf.sem_num  = semnum;
	sbuf.sem_op   = -1;
	sbuf.sem_flg  = SEM_UNDO;
	return semop(semid, &sbuf, 1);
}

int     ishm_ipc_sem_v(int semid, int semnum) {
	struct sembuf sbuf;
	if (semid < 0) {
		return -2;
	}
	sbuf.sem_num  = semnum;
	sbuf.sem_op   = 1;
	sbuf.sem_flg  = SEM_UNDO;
	return semop(semid, &sbuf, 1);
}

int     ishm_ipc_semdel(int semid) {
	union semun semarg;
	if (semid < 0) {
		return -2;
	}
    return semctl(semid, 0, IPC_RMID, semarg);
}