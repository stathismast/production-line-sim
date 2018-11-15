#include <stdio.h>
#include <time.h>       // time
#include <unistd.h>     // fork
#include <string.h>     // memcpy
#include "psm.h"

int paintTime[3];
int checkTime[3];
int assemblyTime;

void construction(TriplePSM * output, int rank, int numOfItems){
    srand(time(NULL)*getpid());
    Part part;
    part.type = rank;

    for(int i=0; i<numOfItems; i++){
        part.id = randomID(); 
        semDown(output->psm[rank]->semEmpty);
            // printf("Sending:  %d/%d\n", part.type, part.id);
            usleep(randomNumber(2000,2500)); // Sleep for half a second
            memcpy(output->psm[rank]->sharedMemory,&part,sizeof(Part));
        semUp(output->psm[rank]->semFull);
    }

    freeTriplePSM(output);
    exit(0);
}

void painter(TriplePSM * input, TriplePSM * output, int numOfItems){
    Part part;

    for(int i=0; i<numOfItems/3; i++){

        if(i%10 == 0) printf("%d\n", i);

        for(int j=0; j<3; j++){
            semDown(input->psm[j]->semFull);
                memcpy(&part, input->psm[j]->sharedMemory,sizeof(Part));
            semUp(input->psm[j]->semEmpty);

            // printf("Received: %d/%d\n", part.type, part.id);
            usleep(paintTime[part.type]);
            
            semDown(output->psm[part.type]->semEmpty);
                memcpy(output->psm[part.type]->sharedMemory,&part,sizeof(Part));
            semUp(output->psm[part.type]->semFull);
        }
    }
    
    freeTriplePSM(input);
    freeTriplePSM(output);
    exit(0);
}

void checker(PSM * input, TriplePSM * output, int numOfItems){
    Part part;

    for(int i=0; i<numOfItems; i++){
        semDown(input->semFull);
            memcpy(&part, input->sharedMemory,sizeof(Part));
        semUp(input->semEmpty);

        // printf("Checking: %d/%d\n", part.type, part.id);

        // semDown(output->semAllEmpty);

            semDown(output->psm[part.type]->semEmpty);
                // printf("CSending: %d/%d\n", part.type, part.id);
                usleep(checkTime[part.type]);
                memcpy(output->psm[part.type]->sharedMemory,&part,sizeof(Part));
            semUp(output->psm[part.type]->semFull);

        semUp(output->semAllFull);
    }
}

void assembler(TriplePSM * input, int numOfItems){
    Part part[3];
    int received[3] = {0};

    for(int i=0; i<numOfItems; i++){

        // if(i%100 == 0) printf("%d\n", i);

        semDown(input->semAllFull); // If there is atleast one part ready

            if(semValue(input->psm[0]->semFull) && !received[0]){
                semDown(input->psm[0]->semFull);
                    memcpy(&part[0], input->psm[0]->sharedMemory,sizeof(Part));
                    received[0] = 1;
            }
            else if(semValue(input->psm[1]->semFull) && !received[1]){
                semDown(input->psm[1]->semFull);
                    memcpy(&part[1], input->psm[1]->sharedMemory,sizeof(Part));
                    received[1] = 1;
            }
            else if(semValue(input->psm[2]->semFull) && !received[2]){
                semDown(input->psm[2]->semFull);
                    memcpy(&part[2], input->psm[2]->sharedMemory,sizeof(Part));
                    received[2] = 1;
            }

        if(received[0] && received[1] && received[2]){
            // printf("Assembler: %d/%d & %d/%d & %d/%d\n", part[0].type, part[0].id, part[1].type, part[1].id, part[2].type, part[2].id);
            received[0] = 0; received[1] = 0; received[2] = 0; 
            semUp(input->psm[0]->semEmpty);
            semUp(input->psm[1]->semEmpty);
            semUp(input->psm[2]->semEmpty);
            usleep(assemblyTime);
            // semUp(input->semAllEmpty);
        }
    }
}

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

    assemblyTime = 1248;

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
