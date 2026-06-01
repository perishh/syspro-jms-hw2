#include "list.h"

#include <stdlib.h>

void ll_init(LinkedList* l) {
  l->front = NULL;
  l->rear = NULL;
  l->size = 0;
}

void ll_free(LinkedList* l) {
  Node* current = l->front;

  while (current != NULL) {
    Node* next = current->next;
    free(current);
    current = next;
  }

  l->front = NULL;
  l->rear = NULL;
  l->size = 0;
}

int ll_push_back(LinkedList* l, long key, void* data) {
  Node* new_node = malloc(sizeof(Node));
  if (new_node == NULL) {
    return -1;
  }

  new_node->key = key;
  new_node->data = data;
  new_node->next = NULL;

  if (l->rear == NULL) {
    l->front = new_node;
    l->rear = new_node;
  } else {
    l->rear->next = new_node;
    l->rear = new_node;
  }

  l->size++;
  return 0;
}

int ll_push_front(LinkedList* l, long key, void* data) {
  Node* new_node = malloc(sizeof(Node));
  if (new_node == NULL) {
    return -1;
  }

  new_node->key = key;
  new_node->data = data;
  new_node->next = l->front;

  l->front = new_node;
  if (l->rear == NULL) {
    l->rear = new_node;
  }

  l->size++;
  return 0;
}

void* ll_pop_front(LinkedList* l) {
  if (l->front == NULL) {
    return NULL;
  }

  Node* temp = l->front;
  void* data = temp->data;

  l->front = l->front->next;
  if (l->front == NULL) {
    l->rear = NULL;
  }

  free(temp);
  l->size--;

  return data;
}

int ll_remove(LinkedList* l, long key) {
  Node* current = l->front;
  Node* prev = NULL;

  while (current != NULL) {
    if (current->key == key) {
      if (prev == NULL) {
        l->front = current->next;
      } else {
        prev->next = current->next;
      }

      if (current->next == NULL) {
        l->rear = prev;
      }

      free(current);
      l->size--;
      return 0;
    }

    prev = current;
    current = current->next;
  }

  return -1;
}