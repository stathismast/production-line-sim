#include <stdio.h>
#include "psm.h"

void painter(PSM * psm, int numberOfItems){
    for(int i=0; i<numberOfItems; i++){
        printf("child is ready to enter CS for the #%d time\n", i);

        semDown(psm->semFull);
            printf("child in CS\n");
            printf("%d: %d\n", (int)getpid(), *psm->sharedMemory);
        semUp(psm->semEmpty);

    }

    free(psm);
    exit(0);
}

void partProducer(PSM * psm, int numberOfItems, int startVal){
    int value = startVal;

    for(int i=0; i<numberOfItems; i++){

        semDown(psm->semEmpty);
            printf("parent in CS, about to sleep\n");
            usleep(500000); // Sleep for half a second
            memcpy(psm->sharedMemory,&value,sizeof(int));
        semUp(psm->semFull);

        value++;
    }

    free(psm);
    exit(0);
}



int main(int argc, char * argv[]){
    srand(time(NULL));

    int numberOfItems = 3;

    // Set up shared memory segment
    PSM * psm = getPSM(sizeof(int));

    // Create painter
    if(fork() == 0)
        painter(psm, 3*numberOfItems);

    // Create three part producers
    if(fork() == 0)
        partProducer(psm, numberOfItems, 5);

    if(fork() == 0)
        partProducer(psm, numberOfItems, 10);

    if(fork() == 0)
        partProducer(psm, numberOfItems, 15);

    // Wait for every process to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Detach protected shared memory
    detachPSM(psm);
    free(psm);
}
