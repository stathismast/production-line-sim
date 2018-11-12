#include "psm.h"

int randomNumber(int lowerLimit, int upperLimit){
    return (rand() % upperLimit-lowerLimit) + lowerLimit;
}

PSM * getPSM(int size){
    PSM * psm = malloc(sizeof(PSM));

    int shmKey = randomNumber(1000,10000);
    int semEmptyKey = randomNumber(1000,10000);
    int semFullKey = randomNumber(1000,10000);

    // Set up shared memory segment
    psm->shmid = shmCreate(shmKey,size);
    psm->sharedMemory = shmGet(psm->shmid);

    // Set up empty-ness semaphore
    psm->semEmpty = semCreate(semEmptyKey,1);

    // Set up full-ness semaphore
    psm->semFull = semCreate(semFullKey,0);

    return psm;
}

void detachPSM(PSM * psm){
    shmDetach(psm->sharedMemory);
    shmDelete(psm->shmid);

    semDelete(psm->semEmpty);
    semDelete(psm->semFull);
}
