#include <sys/shm.h>

int shmCreate(key_t key, int size);
void* shmGet(int shmid);
int shmDetach(void * sharedMemory);
int shmDelete(int shmid);
