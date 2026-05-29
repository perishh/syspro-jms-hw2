#include "worker.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "job.h"
#include "proc.h"

void* worker_main() {
  while (1) {
    Job* job = job_get_available();

    job_lock();

    pid_t pid = proc_start(job);

    free(job->raw_argv);
    job->raw_argv = NULL;

    if (pid < 0) {
      // TODO: Ensure job finished is checked before checking pid (when it comes
      // to checking if queued)
      job->finished = 1;

      job_unlock();
      continue;
    }

    job->pid = pid;

    job_unlock();

    while (1) {
      int wstatus;
      if (waitpid(pid, &wstatus, WUNTRACED) < 0) {
        break;
      }

      if (WIFSTOPPED(wstatus)) {
        job_lock();
        job->suspended = 1;
        job_unlock();
      } else if (WIFCONTINUED(wstatus)) {
        job_lock();
        job->suspended = 0;
        job_unlock();
      } else if (WIFEXITED(wstatus)) {
        break;
      }
    }

    job_lock();
    job->finished = 1;
    job_unlock();
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