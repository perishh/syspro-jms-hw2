#ifndef JOB_H
#define JOB_H

#include <time.h>
#include <unistd.h>

typedef struct {
  int id;
  pid_t pid;
  int suspended;
  int finished;
  time_t timestamp;
  char* raw_argv;  // TODO: reminder to free
} Job;

int job_init();
int job_add(int len, char* raw);
Job* job_get_available();
void job_free();
void job_lock();
void job_unlock();

#endif