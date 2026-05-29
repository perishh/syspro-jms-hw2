#include "map.h"

#include <stdlib.h>

#include "list.h"

#define BUCKETS 50

struct map {
  Node* buckets[BUCKETS];
};

int hash(int key) {
  int hash = key % BUCKETS;
  if (hash < 0) {
    hash += BUCKETS;
  }
  return hash;
}

Map* map_init() {
  Map* map = calloc(1, sizeof(Map));
  if (map == NULL) {
    return NULL;
  }

  return map;
}

int map_insert(Map* map, int key, void* data) {
  int hash_key = hash(key);

  Node* head = map->buckets[hash_key];
  Node* current = head;

  while (current != NULL) {
    if (current->key == key) {
      current->data = data;
      return 0;
    }
    current = current->next;
  }

  Node* node = malloc(sizeof(Node));
  if (node == NULL) {
    return -1;
  }

  node->key = key;
  node->data = data;
  node->next = head;
  map->buckets[hash_key] = node;

  return 0;
}

void* map_get(Map* map, int key) {
  int hash_key = hash(key);

  Node* current = map->buckets[hash_key];
  while (current != NULL) {
    if (current->key == key) {
      return current->data;
    }
    current = current->next;
  }

  return NULL;
}

int map_remove(Map* map, int key) {
  int hash_key = hash(key);

  Node* current = map->buckets[hash_key];
  Node* prev = NULL;

  while (current != NULL) {
    if (current->key == key) {
      if (prev == NULL) {
        map->buckets[hash_key] = current->next;
      } else {
        prev->next = current->next;
      }
      free(current);
      return 0;
    }
    prev = current;
    current = current->next;
  }

  return -1;
}

void map_free(Map* map) {
  for (int i = 0; i < BUCKETS; i++) {
    Node* current = map->buckets[i];
    while (current != NULL) {
      Node* temp = current;
      current = current->next;
      free(temp);
    }
  }
  free(map);
}