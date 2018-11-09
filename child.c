#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#include "sharedMemory.h"

int main(void){
    key_t key = getSharedKey();
    
    int shmid = memID(key,1024);
    void * shptr = memGet(shmid);
    int val = *((int *)shptr);
    printf("%d: val: %d\n", (int)getpid(),val);
}
