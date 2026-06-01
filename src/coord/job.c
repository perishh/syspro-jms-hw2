#include "job.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "list.h"
#include "map.h"
#include "state.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_not_empty_cond = PTHREAD_COND_INITIALIZER;

static LinkedList pending_jobs;
static Map* job_map = NULL;

static int job_key = 1;

int job_init() {
  ll_init(&pending_jobs);
  job_map = map_init();
  if (job_map == NULL) {
    return -1;
  }
  return 0;
}

int job_add(int client, int len, char* raw) {
  job_lock();

  char* argv = malloc((len + 1) * sizeof(char));
  if (argv == NULL) {
    job_unlock();
    return -1;
  }

  memcpy(argv, raw, len);
  // Ensure null termination
  argv[len] = '\0';

  Job* job = malloc(sizeof(Job));
  if (job == NULL) {
    job_unlock();
    free(argv);
    return -1;
  }

  job->id = job_key++;
  job->state = QUEUED;
  job->submit_time = time(NULL);
  job->start_time = 0;
  job->raw_argv = argv;

  if (map_insert(job_map, job->id, job) < 0) {
    job_unlock();
    free(job);
    free(argv);
    return -1;
  }

  if (ll_push_back(&pending_jobs, job->id, job) < 0) {
    map_remove(job_map, job->id);
    job_unlock();
    free(job);
    free(argv);
    return -1;
  }

  pthread_cond_signal(&queue_not_empty_cond);

  sendf(client, "JobID: %d\n", job->id);

  job_unlock();
  return 0;
}

void broadcast_queue_not_empty() {
  pthread_cond_broadcast(&queue_not_empty_cond);
}

void job_status(int client, int id) {
  job_lock();

  Job* j = map_get(job_map, id);
  if (j == NULL) {
    sendf(client, "JobID %d not found\n", id);
  } else {
    switch (j->state) {
      case QUEUED:
        sendf(client, "JobID %d Status:\tQueued (waiting in job queue)\n", id);
        break;
      case SUSPENDED:
      case ACTIVE: {
        time_t now = time(NULL);
        long elapsed = now - (j->submit_time);
        sendf(client, "JobID %d Status:\tActive (running for %ld sec)\n", id,
              elapsed);
        break;
      }
      case FINISHED:
        sendf(client, "JobID %d Status:\tFinished\n", id);
        break;
    }
  }

  job_unlock();
}

void job_show_active(int client) {
  job_lock();

  sendf(client, "Active Jobs:\n");
  for (int i = 0; i < BUCKETS; i++) {
    Node* current = job_map->buckets[i];
    while (current != NULL) {
      Job* j = (Job*)current->data;
      if (j->state == ACTIVE) {
        sendf(client, "JobID %d\n", j->id);
      }
      current = current->next;
    }
  }

  job_unlock();
}

void job_show_finished(int client) {
  job_lock();

  sendf(client, "Finished Jobs:\n");
  for (int i = 0; i < BUCKETS; i++) {
    Node* current = job_map->buckets[i];
    while (current != NULL) {
      Job* j = (Job*)current->data;
      if (j->state == FINISHED) {
        sendf(client, "JobID %d\n", j->id);
      }
      current = current->next;
    }
  }

  job_unlock();
}

void job_status_all(int client, int n) {
  job_lock();
  time_t now = time(NULL);

  for (int i = 0; i < BUCKETS; i++) {
    Node* current = job_map->buckets[i];
    while (current != NULL) {
      Job* j = (Job*)current->data;
      long elapsed = now - j->submit_time;
      if (n <= 0 || elapsed <= n) {
        switch (j->state) {
          case QUEUED:
            sendf(client, "JobID %d Status:\tQueued (waiting in job queue)\n",
                  j->id);
            break;
          case SUSPENDED:
          case ACTIVE: {
            elapsed = now - j->start_time;
            sendf(client, "JobID %d Status:\tActive (running for %ld sec)\n",
                  j->id, elapsed);
          } break;
          case FINISHED:
            sendf(client, "JobID %d Status:\tFinished\n", j->id);
            break;
        }
      }
      current = current->next;
    }
  }
  job_unlock();
}

Job* job_get_available() {
  // TODO: Check if error handling is needed
  job_lock();

  while (pending_jobs.size == 0) {
    if (is_terminating()) {
      job_unlock();
      return NULL;
    }
    pthread_cond_wait(&queue_not_empty_cond, &mutex);
    if (is_terminating()) {
      // Signal received + terminating = exit
      job_unlock();
      return NULL;
    }
  }

  Job* job = ll_pop_front(&pending_jobs);

  job_unlock();
  return job;
}

void job_lock() { pthread_mutex_lock(&mutex); }
void job_unlock() { pthread_mutex_unlock(&mutex); }

struct job_stats job_collect_stats() {
  struct job_stats stats = {0, 0, 0};

  job_lock();

  for (int i = 0; i < BUCKETS; i++) {
    Node* current = job_map->buckets[i];
    while (current != NULL) {
      Job* j = (Job*)current->data;
      stats.total_jobs++;
      if (j->state == ACTIVE) {
        stats.running_jobs++;
      } else if (j->state == QUEUED) {
        stats.queued_jobs++;
      }
      current = current->next;
    }
  }

  job_unlock();
  return stats;
}

void job_free() {
  ll_free(&pending_jobs);
  for (int i = 0; i < BUCKETS; i++) {
    Node* current = job_map->buckets[i];
    while (current != NULL) {
      Job* j = (Job*)current->data;
      free(j);
      current = current->next;
    }
  }
  map_free(job_map);
}