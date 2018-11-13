#include "psm.h"
#include <stdio.h>

int randomNumber(int lowerLimit, int upperLimit){
    return (rand() % upperLimit-lowerLimit) + lowerLimit;
}

TriplePSM * getTriplePSM(int size){
    TriplePSM * triplePSM = malloc(sizeof(TriplePSM));

    triplePSM->first = getPSM(size);
    triplePSM->second = getPSM(size);
    triplePSM->third = getPSM(size);

    int semEmptyKey = randomNumber(1000,10000);
    int semFullKey = randomNumber(1000,10000);

    // Set up empty-ness semaphore
    triplePSM->semAllEmpty = semCreate(semEmptyKey,3);

    // Set up full-ness semaphore
    triplePSM->semAllFull = semCreate(semFullKey,0);
}

PSM * getPSM(int size){
    PSM * psm = malloc(sizeof(PSM));

    int shmKey = randomNumber(1000,10000);
    int semEmptyKey = randomNumber(1000,10000);
    int semFullKey = randomNumber(1000,10000);

    printf("%d\n",shmKey);
    printf("%d\n",semEmptyKey);
    printf("%d\n",semFullKey);

    // Set up shared memory segment
    psm->shmid = shmCreate(shmKey,size);
    psm->sharedMemory = shmGet(psm->shmid);

    // Set up empty-ness semaphore
    psm->semEmpty = semCreate(semEmptyKey,1);

    // Set up full-ness semaphore
    psm->semFull = semCreate(semFullKey,0);

    return psm;
}

void detachTriplePSM(TriplePSM * triplePSM){
    detachPSM(triplePSM->first);
    detachPSM(triplePSM->second);
    detachPSM(triplePSM->third);

    free(triplePSM->first);
    free(triplePSM->second);
    free(triplePSM->third);

    semDelete(triplePSM->semAllEmpty);
    semDelete(triplePSM->semAllFull);
}

void detachPSM(PSM * psm){
    shmDetach(psm->sharedMemory);
    shmDelete(psm->shmid);

    semDelete(psm->semEmpty);
    semDelete(psm->semFull);
}
