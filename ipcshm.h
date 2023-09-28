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
#ifndef INTERFACE_IPC_SHARE_MEMORY_H
#define INTERFACE_IPC_SHARE_MEMORY_H

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void*    ipcshm_alloc(const char *ifile, int index, int shmsz, int flags);

int      ipcshm_write(void* ihandle, const unsigned char *buffer, int bufsize);

int      ipcshm_read(void* ihandle, unsigned char *buffer, int bufsize);

int      ipcshm_free(void* ihandle);


#ifdef __cplusplus
}
#endif

#endif