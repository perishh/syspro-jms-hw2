#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "client.h"
#include "job.h"
#include "worker.h"

int main(int argc, char** argv) {
  // sigsetops(3)
  sigset_t signals;
  if (sigemptyset(&signals) < 0 || sigaddset(&signals, SIGCHLD) < 0) {
    return -1;
  }

  // Block default handling of signals
  // sigprocmask(2)
  if (sigprocmask(SIG_BLOCK, &signals, NULL) < 0) {
    return -1;
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

  // Start listening
  // listen(2)

  if (listen(server_fd, 5) < 0) {
    perror("listen");
    close(server_fd);
    return 1;
  }

  while (1) {
    // TODO: Handle exiting
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

  close(server_fd);
  job_free();
  worker_free();

  return 0;
}
