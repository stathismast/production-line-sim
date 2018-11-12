#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

int semCreate(key_t key, int val);
int semDown(int semid);
int semUp(int semid);
int semDelete(int semid);
