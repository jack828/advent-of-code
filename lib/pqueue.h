#ifndef __PQUEUE_H_
#define __PQUEUE_H_

#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

// Element data structure for the priority queue
struct pq_element {
  void *data;              // Element data
  int priority;            // Element priority
  struct pq_element *next; // Pointer to the next element in the queue
};

// Priority queue data structure
struct priority_queue {
  struct pq_element *head; // Head of the linked list
};

// Priority queue type
typedef struct priority_queue pqueue_t;

// Create a new priority queue with the given compare function
pqueue_t *pq_create() {
  pqueue_t *pq = malloc(sizeof(pqueue_t));
  if (!pq) {
    return NULL;
  }

  pq->head = NULL;

  return pq;
}

// Destroy the given priority queue
void pq_destroy(pqueue_t *pq) {
  struct pq_element *curr = pq->head;
  while (curr) {
    struct pq_element *temp = curr;
    curr = curr->next;
    free(temp);
  }

  free(pq);
}

// Check if the given priority queue is empty
bool pq_empty(const pqueue_t *pq) { return pq->head == NULL; }

// Get the element at the top of the given priority queue
void *pq_top(const pqueue_t *pq) {
  if (pq_empty(pq)) {
    return NULL;
  }

  return pq->head->data;
}

// Insert the given element into the given priority queue
bool pq_enqueue(pqueue_t *pq, void *data, int priority) {
  struct pq_element *newElement = malloc(sizeof(struct pq_element));
  if (!newElement)
    return false;

  newElement->data = data;
  newElement->priority = priority;
  newElement->next = NULL;

  if (pq_empty(pq) || priority < pq->head->priority) {
    // Insert at the head of the queue
    newElement->next = pq->head;
    pq->head = newElement;
  } else {
    // Find the position to insert the element
    struct pq_element *curr = pq->head;
    while (curr->next && priority >= curr->next->priority) {
      curr = curr->next;
    }

    // Insert the element
    newElement->next = curr->next;
    curr->next = newElement;
  }

  return true;
}

// Remove the element at the top of the given priority queue
void *pq_dequeue(pqueue_t *pq) {
  if (pq_empty(pq)) {
    return NULL;
  }

  struct pq_element *top = pq->head;
  void *data = top->data;
  pq->head = top->next;
  free(top);

  return data;
}
#endif
