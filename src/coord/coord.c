#include <netinet/in.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "client.h"
#include "job.h"
#include "sig.h"
#include "state.h"
#include "worker.h"

int main(int argc, char** argv) {
  if (sig_init() < 0) {
    return 1;
  }

  if (args_init(argc, argv) < 0) {
    return 1;
  }

  if (job_init() < 0) {
    return 1;
  }

  if (worker_init() < 0) {
    job_free();
    return 1;
  }

  // Start workers
  if (worker_start() < 0) {
    job_free();
    worker_free();
    return 1;
  }

  // Initialize TCP Server
  // socket(2)
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  // setsockopt(2)
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) <
      0) {
    perror("setsockopt");
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(get_port());

  if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(server_fd);
    return 1;
  }

  // Start client pipes
  if (state_init() < 0) {
    perror("state_init");
    close(server_fd);
    job_free();
    worker_free();
    return 1;
  }

  // Start listening
  // listen(2)

  if (listen(server_fd, 5) < 0) {
    perror("listen");
    close(server_fd);
    return 1;
  }

  struct pollfd fds[3];
  fds[0].fd = server_fd;
  fds[0].events = POLLIN;
  fds[1].fd = state_get_fd();
  fds[1].events = POLLIN;
  fds[2].fd = SIG_FILENO;
  fds[2].events = POLLIN;

  while (1) {
    int ret = poll(fds, 3, -1);
    if (ret < 0) {
      perror("poll");
      break;
    }

    if (fds[2].revents & POLLIN) {
      // Termination signal received
      terminate();
      break;
    }

    if (fds[1].revents & POLLIN) {
      // Shutdown command received
      break;
    }

    if (fds[0].revents & POLLIN) {
      int client_fd = accept(server_fd, NULL, NULL);
      if (client_fd < 0) {
        perror("accept");
        continue;
      }

      if (client_start(client_fd) < 0) {
        close(client_fd);
        continue;
      }
    }
  }

  worker_free();
  client_free();
  job_free();
  state_free();
  sig_free();

  close(server_fd);
  return 0;
}
