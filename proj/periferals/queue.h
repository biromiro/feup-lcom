#ifndef QUEUE_H
#define QUEUE_H

#include <lcom/lcf.h>

#define QUEUESIZE 20

typedef struct{
    uint8_t* values; /** @brief the values in the queue */ 
    uint8_t frontIndex; /** @brief the index of the front element */
    uint8_t rearIndex; /** @brief the index of the last element */
    uint8_t numElements; /** @brief the number of elements in the queue */
}Queue;

/**
 * @brief Creates a new queue
 * 
 * @return the new queue 
 */
Queue* new_queue();

/**
 * @brief Pushes an element onto the queue
 * 
 * @param q the queue to push to
 * @param element the element to push
 * @return True if the push was successful, false otherwise
 */
bool push(Queue* q, uint8_t element);

/**
 * @brief Pops the front element of the queue
 * 
 * @param q the queue to pop from
 * @return 0 if empty, the front element otherwise
 */
uint8_t pop(Queue* q);

/**
 * @brief Gets the front element of the queue
 * 
 * @param q the queue to get the element from
 * @return 0 if empty, the front element otherwise
 */
uint8_t front(Queue* q);

/**
 * @brief Gets the back element of the queue
 * 
 * @param q the queue to get the element from
 * @return 0 if empty, the back element otherwise
 */
uint8_t back(Queue* q);

/**
 * @brief Checks if the given queue is empty
 * 
 * @param q the queue to check
 * @return whether or not the queue is empty
 */
bool is_empty(Queue* q);

/**
 * @brief Checks if the given queue is full
 * 
 * @param q the queue to check
 * @return whether or not the queue is full
 */
bool is_full(Queue* q);

#endif
