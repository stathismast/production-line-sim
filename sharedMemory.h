#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

key_t getSharedKey();       // Return the same key generated from main.c
int memCreate(key_t,int);   // Create shared memory segment
int memID(key_t, int);      // Return the id of shared memory with given key
void * memGet(int);         // Get a pointer in shared memory segment
int memDetach(void *);      // Detached shared memory segment
int memDelete(int);         // Delete shared memory segment
