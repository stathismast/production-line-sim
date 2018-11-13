#include <sys/sem.h>

int semCreate(key_t key, int val);
int semDown(int semid);
int semUp(int semid);
int semValue(int semid);
int semDelete(int semid);
