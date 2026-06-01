#ifndef LIST_H
#define LIST_H

typedef struct Node {
  int key;  // TODO: Check if causes problems
  void* data;
  struct Node* next;
} Node;

typedef struct {
  Node* front;
  Node* rear;
  int size;
} LinkedList;

void ll_init(LinkedList* l);
void ll_free(LinkedList* l);
int ll_push_back(LinkedList* l, int key, void* data);
int ll_push_front(LinkedList* l, int key, void* data);
void* ll_pop_front(LinkedList* l);
int ll_remove(LinkedList* l, int key);

#endif