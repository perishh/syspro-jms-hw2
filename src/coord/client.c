#include "client.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "command.h"

void* client_main(void* argv) {
  long client_fd = (long)argv;

  char* network_buffer = malloc(CMD_RAW_HEADER_SIZE);
  if (network_buffer == NULL) {
    close(client_fd);
    return NULL;
  }

  Command* cmd;

  while (1) {
    int ret = unpack_command(client_fd, network_buffer, &cmd);
    if (ret != 1) {
      if (ret == 0) {
        // EOF
        break;
      } else {
        continue;  // TODO: Maybe break?
      }
    }

    // TODO: Handle command

    free(cmd);
  }

  free(network_buffer);
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