/*****************************************************/
/*
**   Author: lirui
**   Date: 2023/09/20
**   File: IList.h
**   Function:  Interface of shm for user
**   History:
**       2023/09/20 create by lirui
**
**   Copy Right: lirui
**
*/
/*****************************************************/
#ifndef INTERFACE_SHARE_MEMORY_H
#define INTERFACE_SHARE_MEMORY_H

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int     ishm_ipc_shmget(key_t key, int shmsz);

void*   ish_ipc_shmmap(int shmid);

int     ishm_ipc_shmunmap(const void* shmaddr);

int     ishm_ipc_shmdel(int shmid);



int     ishm_ipc_semget(key_t key, int nsems);

int     ishm_ipc_semset(int semid, int semnum, int semunval);

int     ishm_ipc_sem_p(int semid, int semnum);

int     ishm_ipc_sem_v(int semid, int semnum);

int     ishm_ipc_semdel(int semid);


#ifdef __cplusplus
}
#endif

#endif