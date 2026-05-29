#ifndef WORKER_H
#define WORKER_H

int worker_init();
int worker_start();
void worker_show(int client);
void worker_free();

#endif