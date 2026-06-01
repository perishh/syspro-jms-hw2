#ifndef JOB_H
#define JOB_H

#include <time.h>
#include <unistd.h>

// TODO: Remove suspended, no longer needed
typedef enum { QUEUED, ACTIVE, SUSPENDED, FINISHED } JobState;

typedef struct {
  int id;
  JobState state;
  time_t submit_time;
  time_t start_time;
  char* raw_argv;  // TODO: reminder to free
} Job;

struct job_stats {
  int total_jobs;
  int running_jobs;
  int queued_jobs;
};

int job_init();
int job_add(int client, int len, char* raw);
void job_status(int client, int id);
void job_show_active(int client);
void job_show_finished(int client);
void job_status_all(int client, int n);
Job* job_get_available();
void job_free();
void job_lock();
void job_unlock();
struct job_stats job_collect_stats();
void broadcast_queue_not_empty();

#endif