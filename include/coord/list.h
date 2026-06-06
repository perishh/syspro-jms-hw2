#ifndef LIST_H
#define LIST_H

typedef struct Node {
  long key;  // TODO: Maybe switch back to int
  void* data;
  struct Node* next;
} Node;

typedef struct {
  Node* front;
  Node* rear;
  int size;
} LinkedList;

/**
 * @brief Initialized linked list values
 * @param l pointer to the linked list
 */
void ll_init(LinkedList* l);

/**
 * @brief Deallocates memory used for linked list
 * @note Doesn't deallocate the LinkedList itself
 * @param l pointer to the linked list
 */
void ll_free(LinkedList* l);

/**
 * @brief Pushes item to end of linked list
 * @param l pointer to the linked list
 * @param key the key of the item to push
 * @param data pointer to store to linked list
 * @return 0 on success, -1 otherwise
 */
int ll_push_back(LinkedList* l, long key, void* data);

/**
 * @brief Pushes item to start of linked list
 * @param l pointer to the linked list
 * @param key the key of the item to push
 * @param data pointer to store to linked list
 * @return 0 on success, -1 otherwise
 */
int ll_push_front(LinkedList* l, long key, void* data);

/**
 * @brief Removes item from start of linked list
 * @param l pointer to the linked list
 * @return pointer to the removed item on success, NULL otherwise
 */
void* ll_pop_front(LinkedList* l);

/**
 * @brief Removes node from linked list
 * @param l pointer to the linked list
 * @param key the key of the node to remove
 * @return 0 on success, -1 otherwise
 */
int ll_remove(LinkedList* l, long key);

#endif