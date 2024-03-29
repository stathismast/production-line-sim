#include <stdio.h>
#include <time.h>       // time
#include <unistd.h>     // fork
#include <string.h>     // memcpy
#include "processes.h"

int creationRange[2];
int paintTime[3];
int checkTime[3];
int assemblyTime;

int main(int argc, char * argv[]){

    srand(time(NULL));

    // Check for corrent arguments
    if(argc != 2 || atoi(argv[1]) < 1){
        printf("Usage: ./a.out Y\nWhere Y is the number of parts produced.\n");
        exit(0);
    }

    // Store given command line argument
    int numOfItems = atoi(argv[1]);

    // Assign lower and upper bound values for the
    // random time that a new part is created
    creationRange[0] = 1500;
    creationRange[1] = 2000;

    // Assign values for total paint and check time for
    // each part type as well as a total assembly time 
    paintTime[0] = 948;
    paintTime[1] = 831;
    paintTime[2] = 806;

    checkTime[0] = 746;
    checkTime[1] = 732;
    checkTime[2] = 715;

    assemblyTime = 1513;


    // Set up shared memory between the three producers and the painter
    PSM * psm1 = getPSM();

    // Create three part producers
    if(fork() == 0){
        construction(psm1, 0, numOfItems);
        free(psm1);
        exit(0);
    }
    if(fork() == 0){
        construction(psm1, 1, numOfItems);
        free(psm1);
        exit(0);
    }
    if(fork() == 0){
        construction(psm1, 2, numOfItems);
        free(psm1);
        exit(0);
    }

    // Set up shared memory for the painter to send a part to the checkers
    // This structure will have a single shared memory segment, a single 
    // semEmpty semaphore, but three different semFull semaphores, one for 
    // each checker process. That way each checker process will access the
    // shared memory only if it contains the type of part they can check
    TriplePSM * tpsm = getTriplePSM();

    // Create painter
    if(fork() == 0){
        painter(psm1, tpsm, 3*numOfItems);
        free(psm1);
        free(tpsm);
        exit(0);
    }

    // Set up shared memory between the checkers and the assembler
    PSM * psm2 = getPSM();

    // Create three checker
    if(fork() == 0){
        // Create a PSM for the checker process. It should have the same
        // shared memory segment as all the other checkers, the same
        // semEmpty semaphore but a different semFull semaphore
        PSM * input = getSpecificPSM(tpsm->sharedMemory,tpsm->shmid,tpsm->semEmpty, tpsm->semFull[0]);
        checker(input, psm2, numOfItems);
        free(psm1);
        free(tpsm);
        free(psm2);
        free(input);
        exit(0);
    }
    if(fork() == 0){
        // Create a PSM for the checker process. It should have the same
        // shared memory segment as all the other checkers, the same
        // semEmpty semaphore but a different semFull semaphore
        PSM * input = getSpecificPSM(tpsm->sharedMemory, tpsm->shmid,tpsm->semEmpty, tpsm->semFull[1]);
        checker(input, psm2, numOfItems);
        free(psm1);
        free(tpsm);
        free(psm2);
        free(input);
        exit(0);
    }
    if(fork() == 0){
        // Create a PSM for the checker process. It should have the same
        // shared memory segment as all the other checkers, the same
        // semEmpty semaphore but a different semFull semaphore
        PSM * input = getSpecificPSM(tpsm->sharedMemory, tpsm->shmid,tpsm->semEmpty, tpsm->semFull[2]);
        checker(input, psm2, numOfItems);
        free(psm1);
        free(tpsm);
        free(psm2);
        free(input);
        exit(0);
    }

    // Prints for a progress bar that what percentage
    // of the total items has been assembled
    printf("Assembly Progress:\n");
    printf("0   10   20   30   40   50   60   70   80   90   100\n");
    printf("["); fflush(0);

    // Create assembler
    if(fork() == 0){
        assembler(psm2, 3*numOfItems);
        free(psm1);
        free(tpsm);
        free(psm2);
        exit(0);
    }

    // Wait for every process to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Detach all shared memory and semaphores
    detachPSM(psm1);

    detachTriplePSM(tpsm);

    detachPSM(psm2);
    free(tpsm);
    free(psm1);
    free(psm2);
}
