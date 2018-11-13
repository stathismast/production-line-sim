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

    }
    
    freeTriplePSM(input);
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

    // Create painter
    if(fork() == 0)
        painter(tpsm1, 3*numOfItems);

    // Wait for every process to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Detach protected shared memory
    detachTriplePSM(tpsm1);
    freeTriplePSM(tpsm1);
}
