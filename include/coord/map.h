#ifndef MAP_H
#define MAP_H

#include "list.h"

#define BUCKETS 50

typedef struct {
  Node* buckets[BUCKETS];
} Map;

/**
 * @brief Initializes a new map
 * @return pointer to the new map on success, NULL otherwise
 */
Map* map_init();

/**
 * @brief Inserts a key-value pair into the map
 * @param map pointer to the map
 * @param key the key to insert
 * @param data pointer to the value to insert
 * @return 0 on success, -1 otherwise
 */
int map_insert(Map* map, int key, void* data);

/**
 * @brief Retrieves a value from the map by key
 * @param map pointer to the map
 * @param key the key to look up
 * @return pointer to the value on success, NULL otherwise
 */
void* map_get(Map* map, int key);

/**
 * @brief Removes a key-value pair from the map
 * @param map pointer to the map
 * @param key the key to remove
 * @return 0 on success, -1 otherwise
 */
int map_remove(Map* map, int key);

/**
 * @brief Deallocates memory used for the map
 * @param map pointer to the map
 */
void map_free(Map* map);

#endif