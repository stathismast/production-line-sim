#include <stdlib.h>     // malloc
#include <wait.h>       // wait

#include "semaphores.h"
#include "sharedMemory.h"

typedef struct Part{
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
    PSM * first;
    PSM * second;
    PSM * third;
    int semAllEmpty;
    int semAllFull;
} TriplePSM;

int randomNumber(int lowerLimit, int upperLimit);
TriplePSM * getTriplePSM(int size);
PSM * getPSM(int size);
void detachPSM(PSM * psm);
void detachTriplePSM(TriplePSM * triplePSM);
