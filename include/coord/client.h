#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int client_start(int fd);

#endif