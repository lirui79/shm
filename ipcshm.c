#include "ipcshm.h"
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

typedef struct ipcshm_t {
	int            shmid;
    int            shmsz;
	int            semid;
	int            semnum;
	int            index;
    char           name[256];
    int            flags;
    unsigned char *shmaddr;
} ipcshm_t;



void*  ipcshm_alloc(const char *ifile, int index, int shmsz, int flags) {
	struct ipcshm_t *handle = (struct ipcshm_t*) malloc(sizeof(struct ipcshm_t));
	key_t shmkey, semkey;
	char shmfile[256] = {0}, semfile[256] = {0};
	if (handle == NULL) {
		return handle;
	}
	sprintf(shmfile, "%s_shm", ifile);
	sprintf(semfile, "%s_sem", ifile);
	memset(handle, 0, sizeof(struct ipcshm_t));
    shmkey = ftok(shmfile, index);
    semkey = ftok(semfile, index);
    handle->index = index;
    sprintf(handle->name, "%s", ifile);
    handle->flags = flags;
    handle->shmid = ishm_ipc_shmget(shmkey, shmsz);
    if (handle->shmid < 0) {
    	free(handle);
    	handle = NULL;
    	return handle;
    }
    handle->shmsz = shmsz;
    handle->semid = ishm_ipc_semget(semkey, 1);
    if (handle->semid < 0) {
    	if (flags > 0)
    	  	ishm_ipc_shmdel(handle->shmid);
    	free(handle);
    	handle = NULL;
    	return handle;
    }
    handle->semnum = 1;
    handle->shmaddr = (unsigned char*) ish_ipc_shmmap(handle->shmid);
    ishm_ipc_semset(handle->semid, 0, 1);
    return handle;
}

int      ipcshm_write(void* ihandle, const unsigned char *buffer, int bufsize) {
	struct ipcshm_t *handle = NULL;
	if ((ihandle == NULL) || (buffer == NULL) || (bufsize <= 0)) {
		return -1;
	}
	handle = (struct ipcshm_t *) ihandle;
	if (bufsize > handle->shmsz) 
		bufsize = handle->shmsz;
	ishm_ipc_sem_p(handle->semid, 0);
    memcpy(handle->shmaddr, buffer, bufsize);
	ishm_ipc_sem_v(handle->semid, 0);
	return bufsize;
}

int      ipcshm_read(void* ihandle, unsigned char *buffer, int bufsize) {
	struct ipcshm_t *handle = NULL;
	if ((ihandle == NULL) || (buffer == NULL) || (bufsize <= 0)) {
		return -1;
	}
	handle = (struct ipcshm_t *) ihandle;
	if (bufsize > handle->shmsz) 
		bufsize = handle->shmsz;
	ishm_ipc_sem_p(handle->semid, 0);
    memcpy(buffer, handle->shmaddr, bufsize);
	ishm_ipc_sem_v(handle->semid, 0);
	return bufsize;
}

int      ipcshm_free(void* ihandle) {
	struct ipcshm_t *handle = NULL;
	if (ihandle == NULL) {
		return -1;
	}
	handle = (struct ipcshm_t *) ihandle;
    ishm_ipc_shmunmap(handle->shmaddr);
    if (handle->flags > 0) {
	    ishm_ipc_shmdel(handle->shmid);
	    ishm_ipc_semdel(handle->semid);
    }
    free(handle);
    return 0;
}
