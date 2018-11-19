#include <stdlib.h>     // malloc
#include <wait.h>       // wait

#include "semaphores.h"
#include "sharedMemory.h"

#ifndef PSM_H
#define PSM_H

typedef struct Part{
    int type;
    int id;
    unsigned long long creationTime;
    unsigned long long painterTime;
} Part;

// Stucture that contains sharedMemory that is protected using two semaphores
typedef struct PSM{
    Part * sharedMemory;
    int shmid;
    int semEmpty;
    int semFull;
} PSM;

typedef struct TriplePSM{
    PSM * psm[3];
} TriplePSM;

#endif //PSM_H

int randomNumber(int lowerLimit, int upperLimit);
int randomID();
TriplePSM * getTriplePSM(int size);
PSM * getPSM(int size);
void detachPSM(PSM * psm);
void freeTriplePSM(TriplePSM * triplePSM);
void detachTriplePSM(TriplePSM * triplePSM);
