#include <stdio.h>
#include <time.h>       // time
#include <unistd.h>     // fork
#include <string.h>     // memcpy
#include "psm.h"

void painter(TriplePSM * tpsm, int numberOfItems){
    Part part;

    for(int i=0; i<numberOfItems; i++){

        // if(i%100 == 0) printf("%d\n", i);

        semDown(tpsm->semAllFull);

            if(semValue(tpsm->first->semFull)){
                semDown(tpsm->first->semFull);
                    memcpy(&part, tpsm->first->sharedMemory,sizeof(Part));
                semUp(tpsm->first->semEmpty);
            }
            else if(semValue(tpsm->second->semFull)){
                semDown(tpsm->second->semFull);
                    memcpy(&part, tpsm->second->sharedMemory,sizeof(Part));
                semUp(tpsm->second->semEmpty);
            }
            else if(semValue(tpsm->third->semFull)){
                semDown(tpsm->third->semFull);
                    memcpy(&part, tpsm->third->sharedMemory,sizeof(Part));
                semUp(tpsm->third->semEmpty);
            }
            
        semUp(tpsm->semAllEmpty);

        printf("\tPainter just received part with ID:\t");
        printf("%d\n\n", part.id);

    }

    free(tpsm->first);
    free(tpsm->second);
    free(tpsm->third);
    free(tpsm);
    exit(0);
}

void construction(TriplePSM * tpsm, int rank, int numberOfItems, int startVal){
    Part part;
    part.id = startVal; 

    for(int i=0; i<numberOfItems; i++){
        semDown(tpsm->semAllEmpty);
            int contructionTime = randomNumber(5000,10000);
            if(rank == 1){
                semDown(tpsm->first->semEmpty);
                    printf("Parent%d ready to send part with ID:\t\t%d\n", rank, part.id);
                    usleep(contructionTime); // Sleep for half a second
                    memcpy(tpsm->first->sharedMemory,&part,sizeof(Part));
                semUp(tpsm->first->semFull);
            }

            else if(rank == 2){
                semDown(tpsm->second->semEmpty);
                    printf("Parent%d ready to send part with ID:\t\t%d\n", rank, part.id);
                    usleep(contructionTime); // Sleep for half a second
                    memcpy(tpsm->second->sharedMemory,&part,sizeof(Part));
                semUp(tpsm->second->semFull);
            }

            else if(rank == 3){
                semDown(tpsm->third->semEmpty);
                    printf("Parent%d ready to send part with ID:\t\t%d\n", rank, part.id);
                    usleep(contructionTime); // Sleep for half a second
                    memcpy(tpsm->third->sharedMemory,&part,sizeof(Part));
                semUp(tpsm->third->semFull);
            }

        semUp(tpsm->semAllFull);
        part.id++;
    }

    free(tpsm->first);
    free(tpsm->second);
    free(tpsm->third);
    free(tpsm);
    exit(0);
}

int main(int argc, char * argv[]){
    srand(time(NULL));

    if(argc != 2 || atoi(argv[1]) < 1){
        printf("Usage: ./a.out Y\nWhere Y is the number of parts produced.\n");
        exit(0);
    }

    int numberOfItems = atoi(argv[1]);

    // Set up shared memory between the three producers and the painter
    TriplePSM * toBePainted = getTriplePSM(sizeof(Part));

    // Create painter
    if(fork() == 0)
        painter(toBePainted, 3*numberOfItems);

    // Create three part producers
    if(fork() == 0)
        construction(toBePainted, 1, numberOfItems, 101);
    if(fork() == 0)
        construction(toBePainted, 2, numberOfItems, 201);
    if(fork() == 0)
        construction(toBePainted, 3, numberOfItems, 301);

    // Wait for every process to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Detach protected shared memory
    detachTriplePSM(toBePainted);
    free(toBePainted);
}
