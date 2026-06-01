#ifndef MAP_H
#define MAP_H

#include "list.h"

#define BUCKETS 50

typedef struct {
  Node* buckets[BUCKETS];
} Map;

Map* map_init();
int map_insert(Map* map, int key, void* data);
void* map_get(Map* map, int key);
int map_remove(Map* map, int key);
void map_free(Map* map);

#endif