#include "job.h"

#include <pthread.h>
#include <stdlib.h>

#include "map.h"
#include "queue.h"
#include "utils.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_not_empty_cond = PTHREAD_COND_INITIALIZER;

static Queue pending_jobs;
static Map* job_map = NULL;

static int job_key = 1;

int job_init() {
  queue_init(&pending_jobs);
  job_map = map_init();
  if (job_map == NULL) {
    return -1;
  }
  return 0;
}

int job_add(char* raw) {
  pthread_mutex_lock(&mutex);

  int argc = count_words(raw);
  char* argv[argc + 1];  // Account for terminating NULL; TODO: Consider malloc

  if (decode_args(raw, argv) < 0) {
    pthread_mutex_unlock(&mutex);
    return -1;
  }

  Job* job = malloc(sizeof(Job));
  if (job == NULL) {
    pthread_mutex_unlock(&mutex);
    return -1;
  }

  job->id = job_key++;
  job->suspended = 0;
  job->finished = 0;
  // time(2)
  job->timestamp = time(NULL);
  job->pid = -1;

  if (map_insert(job_map, job->id, job) < 0) {
    pthread_mutex_unlock(&mutex);
    free(job);
    return -1;
  }

  if (queue_enqueue(&pending_jobs, job) < 0) {
    map_remove(job_map, job->id);
    pthread_mutex_unlock(&mutex);
    free(job);
    return -1;
  }

  pthread_cond_signal(&queue_not_empty_cond);

  pthread_mutex_unlock(&mutex);
  return 0;
}

Job* job_get_available() {
  // TODO: Check if error handling is needed
  pthread_mutex_lock(&mutex);

  while (pending_jobs.size == 0) {
    pthread_cond_wait(&queue_not_empty_cond, &mutex);
  }

  Job* job = queue_dequeue(&pending_jobs);

  pthread_mutex_unlock(&mutex);
  return job;
}

void job_free() {
  // TODO
}