#include <stdio.h>
#include <time.h>       // time
#include <unistd.h>     // fork
#include <string.h>     // memcpy
#include "processes.h"
#include "queue.h"

int paintTime[3];
int checkTime[3];
int assemblyTime;

int queueTest(){
    Queue * queue = newQueue();

    Part part;

    for(int i=0; i<10; i++){
        part.id = i;
        // printf("%d\n",i);
        addToQueue(queue, part);
        // printQueue(queue);
        // printf("%d\n", queue->value);
    }
    printQueue(queue);

    for(int i=0; i<10; i++){
        popFromQueue(queue);
    }
    printQueue(queue);

    free(queue);
    exit(0);
}

int main(int argc, char * argv[]){

    // queueTest();

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
    PSM * psm1 = getPSM(sizeof(Part));

    // Create three part producers
    if(fork() == 0)
        construction(psm1, 0, numOfItems);
    if(fork() == 0)
        construction(psm1, 1, numOfItems);
    if(fork() == 0)
        construction(psm1, 2, numOfItems);

    // Set up shared memory between the painter and the three checkers
    TriplePSM * tpsm2 = getTriplePSM(sizeof(Part));

    // Create painter
    if(fork() == 0)
        painter(psm1, tpsm2, 3*numOfItems);

    // Set up shared memory between the checkers and the assembler
    PSM * psm3 = getPSM(sizeof(Part));

    // Create three part producers
    if(fork() == 0){
        checker(tpsm2->psm[0], psm3, numOfItems);
        free(psm1);
        freeTriplePSM(tpsm2);
        free(psm3);
        exit(0);
    }
    if(fork() == 0){
        checker(tpsm2->psm[1], psm3, numOfItems);
        free(psm1);
        freeTriplePSM(tpsm2);
        free(psm3);
        exit(0);
    }
    if(fork() == 0){
        checker(tpsm2->psm[2], psm3, numOfItems);
        free(psm1);
        freeTriplePSM(tpsm2);
        free(psm3);
        exit(0);
    }

    if(fork() == 0){
        assembler(psm3, 3*numOfItems);
        free(psm1);
        freeTriplePSM(tpsm2);
        free(psm3);
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
    detachPSM(psm1);
    free(psm1);

    detachTriplePSM(tpsm2);
    freeTriplePSM(tpsm2);

    detachPSM(psm3);
    free(psm3);
}
