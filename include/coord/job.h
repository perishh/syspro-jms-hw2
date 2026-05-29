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
} Job;

int job_init();
int job_add(char* raw);
Job* job_get_available();
void job_free();

#endif