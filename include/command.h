#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>

typedef enum {
  UNKNOWN = 0,
  SUBMIT = 1,
  STATUS = 2,
  STATUS_ALL = 4,
  SHOW_ACTIVE = 8,
  SHOW_POOLS = 16,
  SHOW_FINISHED = 32,
  SUSPEND = 64,
  RESUME = 128,
  SHUTDOWN = 256
} Action;

#define ZERO_ARG_ACTIONS (SHOW_ACTIVE | SHOW_POOLS | SHOW_FINISHED | SHUTDOWN)

// FAM pattern
typedef struct {
  uint16_t action;
  uint32_t data;
  uint32_t len;
  char args[];
} Command;
#define CMD_RAW_HEADER_SIZE (sizeof(uint16_t) + 2 * sizeof(uint32_t))

// TODO: Ensure that the returned pointers are freed by the caller

// TODO: Ensure buffers contain the assumed data and
// lengths to prevent buffer overflows

char* pack_command(const Command* cmd);

Command* unpack_command(const char* buffer);

#endif