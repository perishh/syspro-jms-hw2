#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>

typedef enum {
  UNKNOWN = 0,
  SUBMIT = 1,
  STATUS = 2,
  STATUS_ALL = 4,
  SHOW_ACTIVE = 8,
  SHOW_WORKERS = 16,
  SHOW_FINISHED = 32,
  SUSPEND = 64,
  RESUME = 128,
  SHUTDOWN = 256
} Action;

#define ZERO_ARG_ACTIONS (SHOW_ACTIVE | SHOW_WORKERS | SHOW_FINISHED | SHUTDOWN)

// FAM pattern
typedef struct {
  uint16_t action;
  uint32_t data;
  uint32_t len;
  char args[];
} Command;
#define CMD_RAW_HEADER_SIZE (sizeof(uint16_t) + 2 * sizeof(uint32_t))

/**
 * @brief Packs a command to network friendly format and writes it to the given
 * file descriptor
 * @param fd file descriptor to write the command to
 * @param arg_buffer buffer containing the command arguments
 * @param cmd pointer to the command to pack and write
 * @return 0 on success, -1 on error
 */
int pack_command(int fd, char* arg_buffer, Command* cmd);

/**
 * @brief Unpacks a command from the given file descriptor
 * @param fd file descriptor to read the command from
 * @param cmd pointer to the write the address of the unpacked command to
 * @note The caller is responsible for freeing the memory allocated for the
 * command
 * @return 0 on success, -1 on error
 */
int unpack_command(int fd, Command** cmd);

#endif