#include "sharedMemory.h"

int shmCreate(key_t key, int size){
    return shmget(key,size,0644|IPC_CREAT);
}

void* shmGet(int shmid){
    return shmat(shmid,0,0);
}

int shmDetach(void * sharedMemory){
    return shmdt(sharedMemory);
}

int shmDelete(int shmid){
    return shmctl(shmid,0,IPC_RMID);
}
