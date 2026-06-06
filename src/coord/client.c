#include "client.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <unistd.h>

#include "command.h"
#include "job.h"
#include "list.h"
#include "state.h"
#include "worker.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static LinkedList client_threads;
static int client_termination_fd = -1;

void client_lock() { pthread_mutex_lock(&mutex); }
void client_unlock() { pthread_mutex_unlock(&mutex); }

void client_remove(pthread_t thread) {
  client_lock();

  ll_remove(&client_threads, thread);

  client_unlock();
}

void* client_main(void* argv) {
  long client_fd = (long)argv;

  Command* cmd;

  struct pollfd fds[2];
  fds[0].fd = client_fd;
  fds[0].events = POLLIN;
  fds[1].fd = client_termination_fd;
  fds[1].events = POLLIN;

  while (1) {
    int ret = poll(fds, 2, -1);
    if (ret < 0) {
      perror("poll");
      break;
    }

    if (fds[1].revents & POLLIN) {
      // Termination signal received
      break;
    }

    if (fds[0].revents & POLLIN) {
      // Command received
      ret = unpack_command(client_fd, &cmd);

      if (ret != 1) {
        // Error occurred, disconnect client
        break;
      }

      switch (cmd->action) {
        case SUBMIT:
          if (!is_terminating()) {
            job_add(client_fd, cmd->len, cmd->args);
          }
          break;
        case SHUTDOWN:
          terminate();
          break;
        case STATUS:
          job_status(client_fd, atoi(cmd->args));
          break;
        case SHOW_ACTIVE:
          job_show_active(client_fd);
          break;
        case SHOW_FINISHED:
          job_show_finished(client_fd);
          break;
        case STATUS_ALL: {
          int n = 0;
          if (cmd->len > 0) {
            n = atoi(cmd->args);
          }
          job_status_all(client_fd, n);
        } break;
        case SHOW_WORKERS:
          worker_show(client_fd);
          break;
      }

      free(cmd);
    }
  }

  close(client_fd);
  if (!is_terminating()) {  // To prevent deadlock
    client_remove(pthread_self());
  }
  return NULL;
}

int client_start(int fd) {
  // TODO: Maybe start detached or join at shutdown?
  pthread_t thread;

  // pthread_create(3)
  if (pthread_create(&thread, NULL, client_main, (void*)(long)fd) != 0) {
    return -1;
  }

  client_lock();
  ll_push_back(&client_threads, thread, (void*)(long)fd);
  client_unlock();

  return 0;
}

int client_init() {
  if ((client_termination_fd = eventfd(0, EFD_CLOEXEC)) < 0) {
    return -1;
  }
  ll_init(&client_threads);
  return 0;
}

void client_free() {
  write(client_termination_fd, &(uint64_t){1}, sizeof(uint64_t));
  client_lock();

  Node* node = client_threads.front;
  while (node != NULL) {
    pthread_join(node->key, NULL);
    node = node->next;
  }

  ll_free(&client_threads);
  client_unlock();
  close(client_termination_fd);
}

#define STATS_MSG                                                \
  "Served %d jobs, %d were running, %d were still queued\n\x04", \
      stats->total_jobs, stats->running_jobs, stats->queued_jobs

void broadcast_stats(const struct job_stats* stats) {
  int length = snprintf(NULL, 0, STATS_MSG);
  char* buffer = malloc(length + 1);
  if (buffer != NULL) {
    snprintf(buffer, length + 1, STATS_MSG);
    client_lock();
    Node* node = client_threads.front;
    while (node != NULL) {
      int fd = (int)(long)node->data;
      if (buffer != NULL) {
        write(fd, buffer, length);
      }
      node = node->next;
    }
    client_unlock();
    free(buffer);
  }
}