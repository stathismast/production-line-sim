#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#include "semaphores.h"
#include "sharedMemory.h"

void painter(int * sharedMemory,
             int semFull,
             int semEmpty,
             int numberOfItems){

    for(int i=0; i<numberOfItems; i++){
        printf("child is ready to enter CS for the #%d time\n", i);
        semDown(semFull);
            printf("child in CS\n");
            printf("%d: %d\n", (int)getpid(), *sharedMemory);
        semUp(semEmpty);
    }
    exit(0);
}

void partProducer(int * sharedMemory,
                 int semFull,
                 int semEmpty,
                 int numberOfItems,
                 int startVal){

    int value = startVal;
    for(int i=0; i<numberOfItems; i++){
        semDown(semEmpty);
            printf("parent in CS, about to sleep\n");
            usleep(500000); // Sleep for half a second
            memcpy(sharedMemory,&value,sizeof(int));
        semUp(semFull);
        value++;
    }
    exit(0);
}



int main(int argc, char * argv[]){
    srand(time(NULL));

    int numberOfItems = 3;

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

    // Create painter
    if(fork() == 0)
        painter(sharedMemory,semFull,semEmpty,3*numberOfItems);

    // Create three part producers
    if(fork() == 0)
        partProducer(sharedMemory,semFull,semEmpty,numberOfItems,5);

    if(fork() == 0)
        partProducer(sharedMemory,semFull,semEmpty,numberOfItems,10);

    if(fork() == 0)
        partProducer(sharedMemory,semFull,semEmpty,numberOfItems,15);

    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    shmDetach(sharedMemory);
    shmDelete(shmid);

    semDelete(semEmpty);
    semDelete(semFull);
}
