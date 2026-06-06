#include "state.h"

#include <pthread.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "job.h"

static pthread_mutex_t terminating_mutex = PTHREAD_MUTEX_INITIALIZER;
static int terminating = 0;

static int termfd = -1;

int state_init() {
  if ((termfd = eventfd(0, EFD_CLOEXEC)) < 0) {
    return -1;
  }
  return 0;
}

void state_free() { close(termfd); }

int state_get_fd() { return termfd; }

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
  write(termfd, &(uint64_t){1}, sizeof(uint64_t));
}