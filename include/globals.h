#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>

#define PORT_IN_USER_RANGE(port) (port >= 1024 && port <= 49151)

// TODO: Add doxygen doc

/**
 * @brief Writes exactly n bytes from buf to fd
 * @param fd file descriptor to write to
 * @param buf buffer to write from
 * @param n number of bytes to write
 * @return n on success, normal write(2) return value otherwise
 */
int write_all(int fd, char* buf, size_t n);

/**
 * @brief Reads exactly n bytes to buf from fd
 * @param fd file descriptor to read from
 * @param buf buffer to read into
 * @param n number of bytes to read
 * @return n on success, normal read(2) return value otherwise
 */
int read_all(int fd, char* buf, size_t n);

#endif