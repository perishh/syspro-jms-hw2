#ifndef WORKER_H
#define WORKER_H

/**
 * @brief Initializes memory and data structures for the worker
 * @return 0 on success, -1 otherwise
 */
int worker_init();

/**
 * @brief Starts the worker threads
 * @return 0 on success, -1 otherwise
 */
int worker_start();

/**
 * @brief Shows information about the worker
 * @param client the client fd to send information to
 */
void worker_show(int client);

/**
 * @brief Joins worker threads and deallocates memory used for the workers
 */
void worker_free();

#endif