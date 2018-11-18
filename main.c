#include <stdio.h>
#include <time.h>       // time
#include <unistd.h>     // fork
#include <string.h>     // memcpy
#include "processes.h"

int paintTime[3];
int checkTime[3];
int assemblyTime;

int main(int argc, char * argv[]){
    srand(time(NULL));

    if(argc != 2 || atoi(argv[1]) < 1){
        printf("Usage: ./a.out Y\nWhere Y is the number of parts produced.\n");
        exit(0);
    }

    int numOfItems = atoi(argv[1]);

    paintTime[0] = 948;
    paintTime[1] = 831;
    paintTime[2] = 806;

    checkTime[0] = 746;
    checkTime[1] = 732;
    checkTime[2] = 715;

    assemblyTime = 1513;

    // Set up shared memory between the three producers and the painter
    TriplePSM * tpsm1 = getTriplePSM(sizeof(Part));

    // Create three part producers
    if(fork() == 0)
        construction(tpsm1, 0, numOfItems);
    if(fork() == 0)
        construction(tpsm1, 1, numOfItems);
    if(fork() == 0)
        construction(tpsm1, 2, numOfItems);

    // Set up shared memory between the painter and the three checkers
    TriplePSM * tpsm2 = getTriplePSM(sizeof(Part));

    // Create painter
    if(fork() == 0)
        painter(tpsm1, tpsm2, 3*numOfItems);

    // Set up shared memory between the checkers and the assembler
    TriplePSM * tpsm3 = getTriplePSM(sizeof(Part));

    // Create three part producers
    if(fork() == 0){
        checker(tpsm2->psm[0], tpsm3, numOfItems);
        freeTriplePSM(tpsm1);
        freeTriplePSM(tpsm2);
        freeTriplePSM(tpsm3);
        exit(0);
    }
    if(fork() == 0){
        checker(tpsm2->psm[1], tpsm3, numOfItems);
        freeTriplePSM(tpsm1);
        freeTriplePSM(tpsm2);
        freeTriplePSM(tpsm3);
        exit(0);
    }
    if(fork() == 0){
        checker(tpsm2->psm[2], tpsm3, numOfItems);
        freeTriplePSM(tpsm1);
        freeTriplePSM(tpsm2);
        freeTriplePSM(tpsm3);
        exit(0);
    }

    if(fork() == 0){
        assembler(tpsm3, 3*numOfItems);
        freeTriplePSM(tpsm1);
        freeTriplePSM(tpsm2);
        freeTriplePSM(tpsm3);
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

    // Detach protected shared memory
    detachTriplePSM(tpsm1);
    freeTriplePSM(tpsm1);

    detachTriplePSM(tpsm2);
    freeTriplePSM(tpsm2);
}
