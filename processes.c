#include "processes.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>       // time
#include <string.h>     // memcpy

extern int paintTime[3];
extern int checkTime[3];
extern int assemblyTime;

unsigned long long currentTime() {
    struct timeval te; 
    gettimeofday(&te, 0);
    return te.tv_sec*1000000 + te.tv_usec;
}

void construction(TriplePSM * output, int rank, int numOfItems){
    srand(time(NULL)*getpid());
    Part part;
    part.type = rank;

    for(int i=0; i<numOfItems; i++){
        // Create part at a random time interval
        usleep(randomNumber(2000,2500));
        part.id = randomID();
        part.creationTime = currentTime();

        // Put the part safely in the shared memory
        semDown(output->psm[rank]->semEmpty);
            // printf("Sending:  %d/%d\n", part.type, part.id);
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
            part.painterTime = currentTime();
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
        usleep(checkTime[part.type]);

        semDown(output->psm[part.type]->semEmpty);
            // printf("CSending: %d/%d\n", part.type, part.id);
            memcpy(output->psm[part.type]->sharedMemory,&part,sizeof(Part));
        semUp(output->psm[part.type]->semFull);

        semUp(output->semAllFull);
    }
}

void assembler(TriplePSM * input, int numOfItems){
    Part part[3];
    int received[3] = {0};

    unsigned long long avgWaitTime = 0;
    unsigned long long avgTotalTime = 0;    

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
            for(int j=0; j<3; j++) semUp(input->psm[j]->semEmpty);
            semUp(input->psm[0]->semEmpty);
            semUp(input->psm[1]->semEmpty);
            semUp(input->psm[2]->semEmpty);

            usleep(assemblyTime);

            for(int j=0; j<3; j++) 
                avgTotalTime += (currentTime() - part[j].creationTime);

            for(int j=0; j<3; j++) 
                avgWaitTime += (part[j].painterTime - part[j].creationTime);
        }
    }

    printf("a) %lld\n", avgWaitTime/numOfItems);
    printf("b) %lld\n", avgTotalTime/numOfItems);
}
