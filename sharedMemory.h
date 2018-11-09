#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int memCreate(key_t,int);
void *memGet(int);
int memDetach(void *);
int memDelete(int);
