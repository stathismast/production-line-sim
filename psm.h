#include <stdlib.h>     // malloc
#include <string.h>     // memcpy
#include <unistd.h>     // fork
#include <wait.h>       // wait
#include <time.h>       // time

#include "semaphores.h"
#include "sharedMemory.h"

// Stucture that contains sharedMemory that is protected using two semaphores
typedef struct PSM{
    int * sharedMemory;
    int shmid;
    int semEmpty;
    int semFull;
} PSM;

int randomNumber(int lowerLimit, int upperLimit);
PSM * getPSM(int size);
void detachPSM(PSM * psm);
