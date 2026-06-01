#include "state.h"

#include <pthread.h>
#include <unistd.h>

#include "job.h"

static pthread_mutex_t terminating_mutex = PTHREAD_MUTEX_INITIALIZER;
static int terminating = 0;

static int pipefd[2];  // TODO: maybe use eventfd(2) instead?

int state_init() {
  if (pipe(pipefd) < 0) {
    return -1;
  }
  return 0;
}

void state_free() {
  close(pipefd[0]);
  close(pipefd[1]);
}

int state_get_fd() { return pipefd[0]; }

int is_terminating() {
  pthread_mutex_lock(&terminating_mutex);
  int term = terminating;
  pthread_mutex_unlock(&terminating_mutex);
  return term;
}

void terminate() {
  pthread_mutex_lock(&terminating_mutex);
  if (terminating) {
    pthread_mutex_unlock(&terminating_mutex);
    return;
  }
  terminating = 1;
  pthread_mutex_unlock(&terminating_mutex);
  broadcast_queue_not_empty();
  write(pipefd[1], "\0", 1);
}