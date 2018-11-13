#include <stdio.h>
#include <time.h>       // time
#include <unistd.h>     // fork
#include <string.h>     // memcpy
#include "psm.h"

void painter(TriplePSM * input, int numOfItems){
    Part part;

    for(int i=0; i<numOfItems; i++){

        // if(i%100 == 0) printf("%d\n", i);

        semDown(input->semAllFull);

            if(semValue(input->first->semFull)){
                semDown(input->first->semFull);
                    memcpy(&part, input->first->sharedMemory,sizeof(Part));
                semUp(input->first->semEmpty);
            }
            else if(semValue(input->second->semFull)){
                semDown(input->second->semFull);
                    memcpy(&part, input->second->sharedMemory,sizeof(Part));
                semUp(input->second->semEmpty);
            }
            else if(semValue(input->third->semFull)){
                semDown(input->third->semFull);
                    memcpy(&part, input->third->sharedMemory,sizeof(Part));
                semUp(input->third->semEmpty);
            }
            
        semUp(input->semAllEmpty);

        printf("\tPainter just received part with ID:\t");
        printf("%d\n\n", part.id);

    }
    
    freeTriplePSM(input);
    exit(0);
}

void construction(TriplePSM * tpsm, int rank, int numOfItems, int startVal){
    Part part;
    part.id = startVal; 

    for(int i=0; i<numOfItems; i++){
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

    freeTriplePSM(tpsm);
    exit(0);
}

int main(int argc, char * argv[]){
    srand(time(NULL));

    if(argc != 2 || atoi(argv[1]) < 1){
        printf("Usage: ./a.out Y\nWhere Y is the number of parts produced.\n");
        exit(0);
    }

    int numOfItems = atoi(argv[1]);

    // Set up shared memory between the three producers and the painter
    TriplePSM * tpsm1 = getTriplePSM(sizeof(Part));

    // Create painter
    if(fork() == 0)
        painter(tpsm1, 3*numOfItems);

    // Create three part producers
    if(fork() == 0)
        construction(tpsm1, 1, numOfItems, 101);
    if(fork() == 0)
        construction(tpsm1, 2, numOfItems, 201);
    if(fork() == 0)
        construction(tpsm1, 3, numOfItems, 301);

    // Wait for every process to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Detach protected shared memory
    detachTriplePSM(tpsm1);
    freeTriplePSM(tpsm1);
}
