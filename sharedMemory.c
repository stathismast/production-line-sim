#include "sharedMemory.h"

int memCreate(key_t key,int nbytes){
    return shmget(key,nbytes,0644|IPC_CREAT);
}

void* memGet(int shmid){
    return shmat(shmid,0,0);
}

int memDetach(void *ptr){
    return shmdt(ptr);
}

int memDelete(int shmid){
    return shmctl(shmid,0,IPC_RMID);
}
