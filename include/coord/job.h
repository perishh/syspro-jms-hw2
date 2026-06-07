#ifndef JOB_H
#define JOB_H

#include <time.h>
#include <unistd.h>

typedef enum { QUEUED, ACTIVE, FINISHED } JobState;

typedef struct {
  int id;
  JobState state;
  time_t submit_time;
  time_t start_time;
  char* raw_argv;
} Job;

struct job_stats {
  int total_jobs;
  int running_jobs;
  int queued_jobs;
};

/**
 * @brief Initializes job subsystem, including the pending job queue and job
 * map.
 * @return 0 on success, -1 otherwise
 */
int job_init();

/**
 * @brief Frees memory used for jobs, including the pending job queue and job
 * map.
 */
void job_free();

/**
 * @brief Adds a new job to the pending queue and job map
 * @param client the client fd that submitted the job
 * @param len the length of the raw command arguments
 * @param raw the raw command arguments
 * @return 0 on success, -1 otherwise
 */
int job_add(int client, int len, char* raw);

/**
 * @brief Shows the status of a job with the given ID to the client
 * @param client the client fd to send information to
 * @param id the ID of the job to show
 */
void job_status(int client, int id);

/**
 * @brief Shows all active jobs to the client
 * @param client the client fd to send information to
 */
void job_show_active(int client);

/**
 * @brief Shows all finished jobs to the client
 * @param client the client fd to send information to
 */
void job_show_finished(int client);

/**
 * @brief Prints status of all jobs to coord
 * @param client the client fd to send information to
 * @param n (optional parameter, 0 if empty) max elapsed seconds
 * since job submit
 */
void job_status_all(int client, int n);

/**
 * @brief Locks the job subsystem
 */
void job_lock();

/**
 * @brief Unlocks the job subsystem
 */
void job_unlock();

/**
 * @brief Collects statistics about all jobs
 * @return struct job_stats containing the statistics
 */
struct job_stats job_collect_stats();

/**
 * @brief Broadcasts that the job queue is not empty
 */
void broadcast_queue_not_empty();

/**
 * @brief Gets an available job from the queue
 * @return pointer to the available job, or NULL if none available
 */
Job* job_get_available();

#endif