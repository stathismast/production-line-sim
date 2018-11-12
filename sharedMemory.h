#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int shmCreate(key_t key, int size);
void* shmGet(int shmid);
int shmDetach(void * sharedMemory);
int shmDelete(int shmid);
