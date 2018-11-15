#include <stdlib.h>     // malloc
#include <wait.h>       // wait

#include "semaphores.h"
#include "sharedMemory.h"

typedef struct Part{
    int type;
    int id;
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
    int semAllEmpty;
    int semAllFull;
} TriplePSM;

int randomNumber(int lowerLimit, int upperLimit);
int randomID();
TriplePSM * getTriplePSM(int size);
PSM * getPSM(int size);
void detachPSM(PSM * psm);
void freeTriplePSM(TriplePSM * triplePSM);
void detachTriplePSM(TriplePSM * triplePSM);
