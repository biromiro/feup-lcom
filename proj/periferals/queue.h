#ifndef QUEUE_H
#define QUEUE_H

#include <lcom/lcf.h>

#define QUEUESIZE 20

typedef struct{
    uint8_t* values;
    uint8_t frontIndex, rearIndex, numElements;
}Queue;

Queue* new_queue();

bool push(Queue* q, uint8_t element);
uint8_t pop(Queue* q);
uint8_t front(Queue* q);
uint8_t back(Queue* q);
bool is_empty(Queue* q);
bool is_full(Queue* q);

#endif
