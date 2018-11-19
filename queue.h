#include <stdio.h>
#include <stdlib.h>
#include "psm.h"

typedef struct Queue{
    struct Node * first;
    struct Node * last;
} Queue;

typedef struct Node{
    Part part;
    struct Node * next;
} Node;

Queue * newQueue();
Node * newNode(Part part);
void addToQueue(Queue * queue, Part part);
Part popFromQueue(Queue * queue);
void printQueue(Queue * queue);
char notEmpty(Queue * queue);
