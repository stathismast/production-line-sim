#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

#include "sharedMemory.h"

int main(void){
    key_t key = getSharedKey();

    int shmid = memCreate(key,1024);
    void * shptr = memGet(shmid);
    *((int *)shptr) = 16;

    memDetach(shptr);



    for(int i=0; i<3; i++){
        int pid = fork();
        if(pid == 0){

            char * buff[2];
            buff[0] = (char*) malloc(16);
            strcpy(buff[0],"./child");
            buff[1] = NULL;

            execvp("./child", buff);
        }
    }



    // memDelete(shmid);
}
