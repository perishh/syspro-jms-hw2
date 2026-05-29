#ifndef QUEUE_H
#define QUEUE_H

typedef struct Node {
  int key;  // TODO: Check if causes problems
  void* data;
  struct Node* next;
} Node;

typedef struct {
  Node* front;
  Node* rear;
  int size;
} Queue;

void queue_init(Queue* q);
int queue_enqueue(Queue* q, void* data);
void* queue_dequeue(Queue* q);
void queue_free(Queue* q);

#endif