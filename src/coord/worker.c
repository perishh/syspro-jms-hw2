#include "worker.h"

#include <pthread.h>

#include "job.h"

void* worker_main() {
  while (1) {
    Job* job = job_get_available();
  }
  return NULL;
}

int worker_start() {
  // TODO: Maybe store thread/fd for comms
  // TODO: Maybe start detached or join at shutdown?
  pthread_t thread;

  if (pthread_create(&thread, NULL, worker_main, NULL) != 0) {
    return -1;
  }

  return 0;
}