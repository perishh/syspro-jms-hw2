#include "worker.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "args.h"
#include "client.h"
#include "job.h"
#include "proc.h"

struct worker_info {
  pthread_t thread;
  int current_job_id;
  int total_jobs;
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct worker_info* workers;

void worker_lock() { pthread_mutex_lock(&mutex); }
void worker_unlock() { pthread_mutex_unlock(&mutex); }

void* worker_main(void* arg) {
  long index = (long)arg;

  while (1) {
    worker_lock();
    workers[index].current_job_id = -1;
    worker_unlock();

    Job* job = job_get_available();

    worker_lock();
    workers[index].current_job_id = job->id;
    workers[index].total_jobs++;
    worker_unlock();

    job_lock();

    pid_t pid = proc_start(job);

    free(job->raw_argv);
    job->raw_argv = NULL;

    if (pid < 0) {
      job->state = FINISHED;

      job_unlock();
      continue;
    }

    job_unlock();

    while (1) {
      int wstatus;
      if (waitpid(pid, &wstatus, WUNTRACED) < 0) {
        break;
      }

      if (WIFSTOPPED(wstatus)) {
        job_lock();
        job->state = SUSPENDED;
        job_unlock();
      } else if (WIFCONTINUED(wstatus)) {
        job_lock();
        job->state = ACTIVE;
        job_unlock();
      } else if (WIFEXITED(wstatus)) {
        break;
      }
    }

    job_lock();
    job->state = FINISHED;
    job_unlock();
  }
  return NULL;
}

int worker_start() {
  // TODO: Maybe start detached or join at shutdown?
  worker_lock();

  for (int i = 0; i < get_workers(); i++) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, worker_main, (void*)(long)i) != 0) {
      worker_unlock();
      return -1;
    }
    workers[i].thread = thread;
    workers[i].current_job_id = -1;
    workers[i].total_jobs = 0;
  }

  worker_unlock();
  return 0;
}

int worker_init() {
  workers = malloc(sizeof(struct worker_info) * get_workers());
  if (!workers) {
    return -1;
  }
  return 0;
}

void worker_free() { free(workers); }

void worker_show(int client) {
  worker_lock();

  sendf(client, "Worker TID, State, Served:\n");
  for (int i = 0; i < get_workers(); i++) {
    struct worker_info* w = &workers[i];
    if (w->current_job_id == -1) {
      sendf(client, "0x%08lx\tidle\tserved %d\n", w->thread, w->total_jobs);
    } else {
      sendf(client, "0x%08lx\trunning JobID %d\tserved %d\n", w->thread,
            w->current_job_id, w->total_jobs);
    }
  }

  worker_unlock();
}