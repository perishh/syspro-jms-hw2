#include "globals.h"

#include <errno.h>
#include <unistd.h>

int write_all(int fd, char* buf, size_t n) {
  size_t total = 0;
  while (total < n) {
    ssize_t written = write(fd, buf, n - total);
    if (written <= 0) {
      if (errno == EINTR) {
        // We can try again
        continue;
      }
      return written;
    }
    total += written;
    buf += written;
  }
  return n;
}

int read_all(int fd, char* buf, size_t n) {
  ssize_t remaining = n;
  while (remaining > 0) {
    ssize_t nread = read(fd, buf, remaining);
    if (nread <= 0) {
      if (errno == EINTR) {
        // We can try again
        continue;
      }
      return nread;
    }
    remaining -= nread;
    buf += nread;
  }
  return n;
}