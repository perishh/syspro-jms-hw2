#ifndef PROC_H
#define PROC_H

#include "job.h"

/**
 * @brief Starts a new process for the given job
 * @param job pointer to the job to start
 * @return the pid of the new process on success, -1 otherwise
 */
pid_t proc_start(Job* job);

#endif