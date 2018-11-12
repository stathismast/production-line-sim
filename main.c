#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#include "semaphores.h"
#include "sharedMemory.h"

int child_work(int * sharedMemory, int semFull, int semEmpty){
    printf("child is ready to enter CS\n");
    semDown(semFull);
        printf("child in CS\n");
        printf("%d: %d\n", (int)getpid(), *sharedMemory);
    semUp(semEmpty);
}

int main(int argc, char * argv[]){
    srand(time(NULL));

    // Set up shared memory segment
    key_t shmkey = 1634;
    int shmid;
    int * sharedMemory;
    shmid = shmCreate(shmkey,sizeof(int));
    sharedMemory = shmGet(shmid);

    // Set up empty-ness semaphore
    int semEmpty;
    key_t semEmptyKey = 3869;
    semEmpty = semCreate(semEmptyKey,1);

    // Set up full-ness semaphore
    int semFull;
    key_t semFullKey = 5316;
    semFull = semCreate(semFullKey,0);

    // Fork to create child
    if(fork() == 0){
        child_work(sharedMemory,semFull,semEmpty);
        exit(0);
    }

    // Put some information in the shared memory segment
    int value = 16;
    semDown(semEmpty);
        printf("parent in CS, about to sleep\n");
        sleep(1);
        memcpy(sharedMemory,&value,sizeof(int));
    semUp(semFull);

    wait(NULL);

    shmDetach(sharedMemory);
    shmDelete(shmid);

    semDelete(semEmpty);
    semDelete(semFull);
}
