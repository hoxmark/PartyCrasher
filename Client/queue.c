#ifndef QUEUE_C
#define QUEUE_C

#include <stdio.h>

typedef struct PairTuple {
    int i;
    int j;
} PairTuple;

#define QUEUE_SIZE 23
struct PairTuple Queue[QUEUE_SIZE];
struct PairTuple bestFlip;

int cur_index = 0;
int num_elements = 0;

void QueuePut(int i, int j) {
    Queue[cur_index % QUEUE_SIZE].i = i;
    Queue[cur_index % QUEUE_SIZE].j = j;
    cur_index++;
    if(num_elements < QUEUE_SIZE) num_elements++;
}

int QueueContains(int i, int j) {
    int idx;
    for(idx = 0; idx < num_elements; idx++){
        if(Queue[idx].i == i && Queue[idx].j == j){
            return 1;
        }
    }
    return 0;
}

void QueueReset(){
    cur_index = 0;
    num_elements = 0;
}

void QueuePrint(){
    int i;
    for(i = 0; i < num_elements; i++){
        printf("(%d, %d)\n", Queue[i].i, Queue[i].j);
    }
}

#endif
