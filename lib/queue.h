#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct q_element {
  void *data;
  struct q_element *next;
} q_element;

struct queue {
  q_element *head;
  q_element *tail;
  int size;
};

typedef struct queue queue_t;

queue_t *q_create() {
  queue_t *q = malloc(sizeof(queue_t));
  q->head = NULL;
  q->tail = NULL;
  q->size = 0;
  return q;
}

bool q_empty(queue_t *q) { return q->size == 0; }

void q_enqueue(queue_t *q, void *item) {
  q_element *node = malloc(sizeof(q_element));
  node->data = item;
  node->next = NULL;
  if (q->tail) {
    q->tail->next = node;
  }
  q->tail = node;
  if (!q->head) {
    q->head = node;
  }
  q->size++;
}

void *q_dequeue(queue_t *q) {
  if (q_empty(q)) {
    printf("Queue is empty!\n");
    return NULL;
  }
  q_element *node = q->head;
  void *item = node->data;
  q->head = node->next;
  if (!q->head) {
    q->tail = NULL;
  }
  free(node);
  q->size--;
  return item;
}

void q_destroy(queue_t *q) {
  while (!q_empty(q)) {
    q_dequeue(q);
  }
  free(q);
}

#endif
