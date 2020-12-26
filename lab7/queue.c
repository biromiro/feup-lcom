#include "queue.h"

Queue* new_queue(){
    Queue* queue = malloc(sizeof(Queue));
    queue->values = malloc(sizeof(uint8_t)*QUEUESIZE);
    queue->frontIndex = 0;
    queue->rearIndex = QUEUESIZE -1;
    queue->numElements = 0;

    return queue;
}

bool push(Queue* q, uint8_t element){
    if(!is_full(q)){
        q->numElements++;
        q->rearIndex = (q->rearIndex + 1)%QUEUESIZE;
        q->values[q->rearIndex] = element;
        return true;
    }else return false;
}

uint8_t pop(Queue* q){
    if(!is_empty(q)){
        uint8_t val = front(q);
        q->frontIndex = (q->frontIndex + 1)%QUEUESIZE;
        q->numElements--;
        return val;
    }else return 0;
}

uint8_t front(Queue* q){
    if(!is_empty(q)){
        return q->values[q->frontIndex];
    }else return 0;
}

uint8_t back(Queue* q){
    if(!is_empty(q)){
        return q->values[q->rearIndex];
    }else return 0;
}

bool is_empty(Queue* q){
    return (q->numElements == 0);
}

bool is_full(Queue* q){
    return (q->numElements == QUEUESIZE);
}
