#include "client.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "command.h"
#include "job.h"
#include "worker.h"

void* client_main(void* argv) {
  long client_fd = (long)argv;

  Command* cmd;

  while (1) {
    int ret = unpack_command(client_fd, &cmd);
    if (ret != 1) {
      if (ret == 0) {
        // EOF
        break;
      } else {
        continue;  // TODO: Maybe break?
      }
    }

    switch (cmd->action) {
      case SUBMIT:
        // TODO: Handle error
        job_add(client_fd, cmd->len, cmd->args);
        break;
      case STATUS:
        job_status(client_fd, atoi(cmd->args));
        break;
      case SHOW_ACTIVE:
        job_show_active(client_fd);
        break;
      case SHOW_FINISHED:
        job_show_finished(client_fd);
        break;
      case STATUS_ALL: {
        int n = 0;
        if (cmd->len > 0) {
          n = atoi(cmd->args);
        }
        job_status_all(client_fd, n);
      } break;
      case SHOW_WORKERS:
        worker_show(client_fd);
        break;
    }

    free(cmd);
  }

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