#include "client.h"

#include <pthread.h>
#include <unistd.h>

void* client_main(void* argv) {
  long client_fd = (long)argv;

  close(client_fd);
  return NULL;
}

int client_start(int fd) {
  // TODO: Maybe store thread/fd for comms
  // TODO: Maybe start detached or join at shutdown?
  pthread_t thread;

  // pthread_create(3)
  if (pthread_create(&thread, NULL, client_main, (void*)(long)fd) != 0) {
    return -1;
  }

  return 0;
}