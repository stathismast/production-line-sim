#include "psm.h"
#include <stdio.h>

// Return a random number in the range [lowerLimit, upperLimit)
int randomNumber(int lowerLimit, int upperLimit){
    return (rand() % (upperLimit-lowerLimit)) + lowerLimit;
}

// Return a random 4-digit number
int randomID(){
    return randomNumber(1000,10000);
}

TriplePSM * getTriplePSM(){
    TriplePSM * triplePSM = malloc(sizeof(TriplePSM));

    triplePSM->psm[0] = getPSM();
    triplePSM->psm[1] = getPSM();
    triplePSM->psm[2] = getPSM();
}

// Create and return a new PSM structure with
// two semaphores and one shared memeory segment
PSM * getPSM(){
    PSM * psm = malloc(sizeof(PSM));

    int shmKey = randomNumber(0,10000000);
    int semEmptyKey = randomNumber(0,10000000);
    int semFullKey = randomNumber(0,10000000);

    // Set up shared memory segment
    psm->shmid = shmCreate(shmKey,sizeof(Part));
    psm->sharedMemory = shmGet(psm->shmid);

    // Set up empty-ness semaphore
    psm->semEmpty = semCreate(semEmptyKey,1);

    // Set up full-ness semaphore
    psm->semFull = semCreate(semFullKey,0);

    return psm;
}

void detachTriplePSM(TriplePSM * triplePSM){
    detachPSM(triplePSM->psm[0]);
    detachPSM(triplePSM->psm[1]);
    detachPSM(triplePSM->psm[2]);
}

void freeTriplePSM(TriplePSM * triplePSM){
    free(triplePSM->psm[0]);
    free(triplePSM->psm[1]);
    free(triplePSM->psm[2]);

    free(triplePSM);
}

// Detach the shared memory segment and the two semaphores of a PSM structure
void detachPSM(PSM * psm){
    shmDetach(psm->sharedMemory);
    shmDelete(psm->shmid);

    semDelete(psm->semEmpty);
    semDelete(psm->semFull);
}
