#include <stdio.h>
#include <time.h>       // time
#include <unistd.h>     // fork
#include <string.h>     // memcpy
#include "psm.h"

void painter(TriplePSM * input, TriplePSM * output, int numOfItems){
    Part part;

    for(int i=0; i<numOfItems; i++){

        // if(i%100 == 0) printf("%d\n", i);

        semDown(input->semAllFull);

            if(semValue(input->psm[0]->semFull)){
                semDown(input->psm[0]->semFull);
                    memcpy(&part, input->psm[0]->sharedMemory,sizeof(Part));
                semUp(input->psm[0]->semEmpty);
            }
            else if(semValue(input->psm[1]->semFull)){
                semDown(input->psm[1]->semFull);
                    memcpy(&part, input->psm[1]->sharedMemory,sizeof(Part));
                semUp(input->psm[1]->semEmpty);
            }
            else if(semValue(input->psm[2]->semFull)){
                semDown(input->psm[2]->semFull);
                    memcpy(&part, input->psm[2]->sharedMemory,sizeof(Part));
                semUp(input->psm[2]->semEmpty);
            }

        semUp(input->semAllEmpty);

        printf("Received: %d/%d\n", part.type, part.id);

        semDown(output->psm[part.type]->semEmpty);
            memcpy(output->psm[part.type]->sharedMemory,&part,sizeof(Part));
        semUp(output->psm[part.type]->semFull);

    }
    
    freeTriplePSM(input);
    freeTriplePSM(output);
    exit(0);
}

void construction(TriplePSM * output, int rank, int numOfItems){
    srand(time(NULL)*getpid());
    Part part;
    part.type = rank;

    for(int i=0; i<numOfItems; i++){
        part.id = randomNumber(1000,10000); 
        int contructionTime = randomNumber(5000,10000);

        semDown(output->semAllEmpty);

            semDown(output->psm[rank]->semEmpty);
                printf("Sending:  %d/%d\n", part.type, part.id);
                usleep(contructionTime); // Sleep for half a second
                memcpy(output->psm[rank]->sharedMemory,&part,sizeof(Part));
            semUp(output->psm[rank]->semFull);

        semUp(output->semAllFull);
        part.id++;
    }

    freeTriplePSM(output);
    exit(0);
}

void checker(PSM * input, int numOfItems){
    Part part;

    for(int i=0; i<numOfItems; i++){
        semDown(input->semFull);
            memcpy(&part, input->sharedMemory,sizeof(Part));
        semUp(input->semEmpty);
        printf("Checking: %d/%d\n", part.type, part.id);
    }
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

    // Create three part producers
    if(fork() == 0){
        checker(tpsm2->psm[0], numOfItems);
        freeTriplePSM(tpsm1);
        freeTriplePSM(tpsm2);
        exit(0);
    }
    if(fork() == 0){
        checker(tpsm2->psm[1], numOfItems);
        freeTriplePSM(tpsm1);
        freeTriplePSM(tpsm2);
        exit(0);
    }
    if(fork() == 0){
        checker(tpsm2->psm[2], numOfItems);
        freeTriplePSM(tpsm1);
        freeTriplePSM(tpsm2);
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

    // Detach protected shared memory
    detachTriplePSM(tpsm1);
    freeTriplePSM(tpsm1);

    detachTriplePSM(tpsm2);
    freeTriplePSM(tpsm2);
}
