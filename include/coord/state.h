#ifndef STATE_H
#define STATE_H

int state_init();
int state_get_fd();
int is_terminating();
void terminate();
void state_free();

#endif