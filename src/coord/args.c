#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"

static int workers = 0;
static int port = 0;

void print_usage() {
  fprintf(stderr, "Usage: jms_coord -p <port> -l <path> -n <workers>\n");
}

int get_workers() { return workers; }
int get_port() { return port; }

int args_init(int argc, char** argv) {
  char* path = NULL;

  // getopt(3)
  int opt;
  while ((opt = getopt(argc, argv, "p:l:n:")) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'l':
        path = optarg;
        break;
      case 'n':
        workers = atoi(optarg);
        break;
      default:
        // Empty or unknown argument
        print_usage();
        return -1;
    }
  }

  if (path == NULL || workers <= 0 || !PORT_IN_USER_RANGE(port)) {
    // Ensure arguments are valid
    print_usage();
    return -1;
  }

  // Change working directory to path
  // chdir(2)
  if (chdir(path) < 0) {
    perror("chdir");
    return -1;
  }

  return 0;
}