#include "queue.h"

Queue * newQueue(){
    Queue * queue = malloc(sizeof(Queue));
    *queue = (Queue){0};
    queue->first == 0;
    queue->last == 0;
    return queue;
}

Node * newNode(Part part){
    Node * node = malloc(sizeof(Node));
    node->part = part;
    node->next = NULL;
    return node;
}

void addToQueue(Queue * queue, Part part){
    if(queue->first == NULL){
        queue->first = newNode(part);
        queue->last = queue->first;
    }
	else{
		queue->last->next = newNode(part);
        queue->last = queue->last->next;
	}
}

Part popFromQueue(Queue * queue){
    Node * node = queue->first;
    queue->first = queue->first->next;

    Part part = node->part;
    free(node);
    return part;
}

void printQueue(Queue * queue){
    if(queue->first == NULL){
        printf("Queue is empty.\n");
        return;
    }

    Node * node = queue->first;
    while(node != NULL){
        printf("%d\n", node->part.id);
        node = node->next;
    }
}

char notEmpty(Queue * queue){
    return !(queue->first == NULL);
}
