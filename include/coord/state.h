#ifndef STATE_H
#define STATE_H

/**
 * @brief Initializes the eventfd for termination signaling
 * @return 0 on success, -1 otherwise
 */
int state_init();

/**
 * @brief Gets the file descriptor of the eventfd used for termination signaling
 * @return the file descriptor
 */
int state_get_fd();

/**
 * @brief Checks if termination signal has been received
 * @return 1 if terminating, 0 otherwise
 */
int is_terminating();

/**
 * @brief Sets the termination flag and signals all threads
 */
void terminate();

/**
 * @brief Closes the eventfd
 */
void state_free();

#endif