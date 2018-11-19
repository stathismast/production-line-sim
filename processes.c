#include "processes.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>       // time
#include <string.h>     // memcpy
#include "queue.h"

extern int paintTime[3];
extern int checkTime[3];
extern int assemblyTime;

unsigned long long currentTime() {
    struct timeval te; 
    gettimeofday(&te, 0);
    return te.tv_sec*1000000 + te.tv_usec;
}

void construction(PSM * output, int rank, int numOfItems){
    srand(time(NULL)*getpid());
    Part part;
    part.type = rank;

    for(int i=0; i<numOfItems; i++){
        // Create part at a random time interval
        usleep(randomNumber(3000,3600));
        part.id = randomID();
        part.creationTime = currentTime();

        // Put the part safely in the shared memory
        semDown(output->semEmpty);
            // printf("Sending:  %d/%d\n", part.type, part.id);
            memcpy(output->sharedMemory,&part,sizeof(Part));
        semUp(output->semFull);
    }

    free(output);
    exit(0);
}

void painter(PSM * input, TriplePSM * output, int numOfItems){
    Part part;

    for(int i=0; i<numOfItems; i++){

        if(i%30 == 0) printf("%d\n", i/3);

        // Take part from the contruction
        semDown(input->semFull);
            memcpy(&part, input->sharedMemory,sizeof(Part));
        semUp(input->semEmpty);

        // Paint part
        part.painterTime = currentTime();
        usleep(paintTime[part.type]);
        
        // Send part to checker
        semDown(output->psm[part.type]->semEmpty);
            memcpy(output->psm[part.type]->sharedMemory,&part,sizeof(Part));
        semUp(output->psm[part.type]->semFull);

    }
    
    free(input);
    freeTriplePSM(output);
    exit(0);
}

void checker(PSM * input, PSM * output, int numOfItems){
    Part part;

    for(int i=0; i<numOfItems; i++){
        semDown(input->semFull);
            memcpy(&part, input->sharedMemory,sizeof(Part));
        semUp(input->semEmpty);

        usleep(checkTime[part.type]);

        semDown(output->semEmpty);
            memcpy(output->sharedMemory,&part,sizeof(Part));
        semUp(output->semFull);
    }
}

void assembler(PSM * input, int numOfItems){
    Part part;

    Queue * partQueue[3] = {0};
    partQueue[0] = newQueue();
    partQueue[1] = newQueue();
    partQueue[2] = newQueue();

    unsigned long long avgWaitTime = 0;
    unsigned long long avgTotalTime = 0;    

    for(int i=0; i<numOfItems; i++){

        // Take a part form any checker and add it to the appropriate queue
        semDown(input->semFull);
            memcpy(&part, input->sharedMemory, sizeof(Part));
            addToQueue(partQueue[part.type], part);
        semUp(input->semEmpty);

        // If we have atleast one of each part, we can assemble it
        if(notEmpty(partQueue[0]) &&
           notEmpty(partQueue[1]) &&
           notEmpty(partQueue[2])){

            Part part[3];
            part[0] = popFromQueue(partQueue[0]);
            part[1] = popFromQueue(partQueue[1]);
            part[2] = popFromQueue(partQueue[2]);
            // printf("Assembler: %d/%d & %d/%d & %d/%d\n", part[0].type, part[0].id, part[1].type, part[1].id, part[2].type, part[2].id);

            usleep(assemblyTime);

            for(int j=0; j<3; j++) 
                avgTotalTime += (currentTime() - part[j].creationTime);

            for(int j=0; j<3; j++) 
                avgWaitTime += (part[j].painterTime - part[j].creationTime);
        }
    }

    printf("a) %lld\n", avgWaitTime/numOfItems);
    printf("b) %lld\n", avgTotalTime/numOfItems);

    free(partQueue[0]);
    free(partQueue[1]);
    free(partQueue[2]);
}
