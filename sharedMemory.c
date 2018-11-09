#include "sharedMemory.h"

key_t getSharedKey(){
    key_t key;
    if ((key = ftok("main.c", 'R')) == -1){
        perror("ftok");
        exit(1);
    }
}

int memCreate(key_t key, int size){
    if((size <= 0) || (key < 0))
        return -1;
    return shmget(key,size,0644|IPC_CREAT);
}

int memID(key_t key, int size){
    if((size <= 0) || (key < 0))
        return -1;
    return shmget(key,size,0644);
}

void * memGet(int shmid){
    if(shmid < 0)
        return NULL;
    return shmat(shmid,0,0);
}

int memDetach(void *ptr){
    return shmdt(ptr);
}

int memDelete(int shmid){
    if(shmid < 0)
        return -1;
    return shmctl(shmid,0,IPC_RMID);
}
