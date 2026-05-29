#include "queue.h"

#include <stdlib.h>

void queue_init(Queue* q) {
  q->front = NULL;
  q->rear = NULL;
  q->size = 0;
}

int queue_enqueue(Queue* q, void* data) {
  Node* node = malloc(sizeof(Node));
  if (node == NULL) {
    return -1;
  }

  node->data = data;
  node->next = NULL;

  if (q->rear == NULL) {
    q->front = q->rear = node;
  } else {
    q->rear->next = node;
    q->rear = node;
  }

  q->size++;
  return 0;
}

void* queue_dequeue(Queue* q) {
  if (q->front == NULL) {
    return NULL;
  }

  Node* node = q->front;
  void* data = node->data;

  q->front = node->next;
  if (q->front == NULL) {
    q->rear = NULL;
  }

  free(node);
  q->size--;

  return data;
}

void queue_free(Queue* q) {
  Node* n = q->front;
  Node* temp;
  while (n != NULL) {
    temp = n;
    n = n->next;

    free(temp);
  }
  q->size = 0;
}