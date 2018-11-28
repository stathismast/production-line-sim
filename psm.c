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

    int shmKey = randomNumber(0,10000000);
    int semEmptyKey = randomNumber(0,10000000);
    int semFullKey0 = randomNumber(0,10000000);
    int semFullKey1 = randomNumber(0,10000000);
    int semFullKey2 = randomNumber(0,10000000);

    // Set up shared memory segment
    triplePSM->shmid = shmCreate(shmKey,sizeof(Part));
    triplePSM->sharedMemory = shmGet(triplePSM->shmid);

    // Set up empty-ness semaphore
    triplePSM->semEmpty = semCreate(semEmptyKey,1);

    // Set up full-ness semaphore
    triplePSM->semFull[0] = semCreate(semFullKey0,0);
    triplePSM->semFull[1] = semCreate(semFullKey1,0);
    triplePSM->semFull[2] = semCreate(semFullKey2,0);
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

// Create and return a new PSM structure with
// specific specific values for each member
PSM * getSpecificPSM(Part * shm, int shmid, int semEmpty, int semFull){
    PSM * psm = malloc(sizeof(PSM));

    // Set up shared memory segment
    psm->shmid = shmid;
    psm->sharedMemory = shm;

    // Set up empty-ness semaphore
    psm->semEmpty = semEmpty;

    // Set up full-ness semaphore
    psm->semFull = semFull;

    return psm;
}

void detachTriplePSM(TriplePSM * triplePSM){
    shmDetach(triplePSM->sharedMemory);
    shmDelete(triplePSM->shmid);

    semDelete(triplePSM->semEmpty);
    semDelete(triplePSM->semFull[0]);
    semDelete(triplePSM->semFull[1]);
    semDelete(triplePSM->semFull[2]);
}

// Detach the shared memory segment and the two semaphores of a PSM structure
void detachPSM(PSM * psm){
    shmDetach(psm->sharedMemory);
    shmDelete(psm->shmid);

    semDelete(psm->semEmpty);
    semDelete(psm->semFull);
}
