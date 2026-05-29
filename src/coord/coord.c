#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "client.h"

int main(int argc, char** argv) {
  if (args_init(argc, argv) < 0) {
    return 1;
  }

  // Initialize TCP Server
  // socket(2)
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    return 1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(get_port());

  if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    close(server_fd);
    return 1;
  }

  // Start listening
  // listen(2)

  if (listen(server_fd, 5) < 0) {
    close(server_fd);
    return 1;
  }

  while (1) {
    // TODO: Handle exiting
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
      continue;
    }

    if (client_start(client_fd) < 0) {
      close(client_fd);
      continue;
    }
  }

  close(server_fd);

  return 0;
}
