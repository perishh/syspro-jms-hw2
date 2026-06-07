#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "job.h"

#define sendf(fd, fmt, ...)                                         \
  {                                                                 \
    int length = snprintf(NULL, 0, fmt __VA_OPT__(, ) __VA_ARGS__); \
    char* buffer = malloc(length + 1);                              \
    if (buffer != NULL) {                                           \
      snprintf(buffer, length + 1, fmt __VA_OPT__(, ) __VA_ARGS__); \
      write(fd, buffer, length);                                    \
      free(buffer);                                                 \
    }                                                               \
  }

/**
 * @brief Broadcasts job stats to all clients.
 * @param stats Pointer to job stats to broadcast
 */
void broadcast_stats(const struct job_stats* stats);

/**
 * @brief Initializes client subsystem, creates termination eventfd.
 * @return 0 on success, -1 otherwise
 */
int client_init();

/**
 * @brief Starts a client thread to handle the given client file descriptor.
 * @param fd File descriptor of the accepted client connection
 * @return 0 on success, -1 otherwise
 */
int client_start(int fd);

/**
 * @brief Frees client subsystem, signals all client threads to terminate and
 * waits for them to finish.
 */
void client_free();

#endif