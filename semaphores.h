#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

int semCreate(key_t,int);
int semDown(int);
int semUp(int);
int semDelete(int);
