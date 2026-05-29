#include "proc.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "globals.h"
#include "utils.h"

#define DATETIME_SIZE 16  // Including \0

int proc_cd(int id, time_t timestamp) {
  char* buffer = malloc(64);
  if (buffer == NULL) {
    return -1;
  }

  // getpid(2)
  pid_t pid = getpid();

  // printf(3)
  int base = snprintf(buffer, 64 - DATETIME_SIZE, "outputs_%d_%d_", id, pid);
  if (base >= 64 - DATETIME_SIZE || base < 0) {
    free(buffer);
    return -1;
  }

  // strftime(3)
  // Begin writing on null byte of previous snprintf
  if (strftime(buffer + base, DATETIME_SIZE, "%Y%m%d_%H%M%S",
               localtime(&timestamp)) <= 0) {
    free(buffer);
    return -1;
  }

  // mkdir(2), inode(7)
  // Grant all permissions
  if (mkdir(buffer, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
    free(buffer);
    return -1;
  }

  // chdir(2)
  if (chdir(buffer) < 0) {
    // rmdir(2)
    rmdir(buffer);
    free(buffer);
    return -1;
  }

  free(buffer);
  return 0;
}

int proc_redirect(int id) {
  char path[32];

  int ret = snprintf(path, 32, "stdout_%d", id);
  if (ret >= 32 || ret < 0) {
    perror("snprintf");
    return -1;
  }

  int out_file = open(path, O_WRONLY | O_CREAT | O_TRUNC, MODE_RW);
  if (out_file < 0) {
    return -1;
  }

  if (dup2(out_file, STDOUT_FILENO) < 0) {
    close(out_file);
    unlink(path);
    return -1;
  }

  ret = snprintf(path, 32, "stderr_%d", id);
  if (ret >= 32 || ret < 0) {
    close(out_file);
    return -1;
  }

  int err_file = open(path, O_WRONLY | O_CREAT | O_TRUNC, MODE_RW);
  if (err_file < 0) {
    close(out_file);
    return -1;
  }

  if (dup2(err_file, STDERR_FILENO) < 0) {
    close(out_file);
    close(err_file);
    unlink(path);
    return -1;
  }

  return 0;
}

pid_t proc_start(Job* job) {
  int argc = count_words(job->raw_argv);
  char* argv[argc + 1];  // Account for terminating NULL

  if (decode_args(job->raw_argv, argv) < 0) {
    return -1;
  }

  pid_t pid = fork();
  if (pid == 0) {
    // Job process

    // Change working directory
    if (proc_cd(job->id, job->timestamp) < 0) {
      _exit(1);
    }

    // Redirect stdout & stderr to files
    if (proc_redirect(job->id) < 0) {
      _exit(1);
    }

    // exec(3)
    int ret = execvp(argv[0], argv);
    if (ret < 0) {
      _exit(1);
    }
    _exit(0);
  }

  job->timestamp = time(NULL);

  if (pid < 0) {
    return -1;
  }

  return pid;
}