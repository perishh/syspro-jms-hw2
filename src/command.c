#include "command.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

int pack_command(int fd, Command* cmd) {
  char* buffer = malloc(CMD_RAW_HEADER_SIZE);
  if (!buffer) {
    return -1;
  }

  // byteorder(3)
  uint16_t action = htons(cmd->action);
  uint32_t data = htonl(cmd->data);
  uint32_t len = htonl(cmd->len);

  memcpy(buffer, &action, sizeof(action));
  memcpy(buffer + sizeof(action), &data, sizeof(data));
  memcpy(buffer + sizeof(action) + sizeof(data), &len, sizeof(len));

  ssize_t written = write_all(fd, buffer, CMD_RAW_HEADER_SIZE);
  if (written != CMD_RAW_HEADER_SIZE) {
    free(buffer);
    return -1;
  }

  free(buffer);  // No longer needed

  if (cmd->len > 0) {
    written = write_all(fd, cmd->args, cmd->len + 1);  // \0
    if (written != (long)(cmd->len + 1)) {
      return -1;
    }
  }

  return 1;
}

int unpack_command(int fd, Command** cmd) {
  char* buffer = malloc(CMD_RAW_HEADER_SIZE);
  if (!buffer) {
    return -1;
  }

  ssize_t nread = read_all(fd, buffer, CMD_RAW_HEADER_SIZE);
  if (nread != CMD_RAW_HEADER_SIZE) {
    if (nread == 0) {
      free(buffer);
      return 0;
    }

    free(buffer);
    return -1;
  }

  uint32_t len;
  memcpy(&len, buffer + sizeof(uint16_t) + sizeof(uint32_t), sizeof(uint32_t));
  len = ntohl(len);

  Command* cmd_buffer = malloc(sizeof(Command) + len + 1);
  if (!cmd_buffer) {
    free(buffer);
    return -1;
  }

  memcpy(&(cmd_buffer->action), buffer, sizeof(uint16_t));
  cmd_buffer->action = ntohs(cmd_buffer->action);

  memcpy(&(cmd_buffer->data), buffer + sizeof(uint16_t), sizeof(uint32_t));
  cmd_buffer->data = ntohl(cmd_buffer->data);

  cmd_buffer->len = len;

  free(buffer);  // No longer needed

  if (cmd_buffer->action == UNKNOWN) {
    free(cmd_buffer);
    return -1;
  }

  if (len == 0) {
    // No arguments given
    if (cmd_buffer->action != STATUS_ALL &&
        (cmd_buffer->action & ZERO_ARG_ACTIONS) == 0) {
      free(cmd_buffer);
      return -1;
    }

    *cmd = cmd_buffer;
    return 1;
  }

  // TODO: Maybe read blocking
  nread = read_all(fd, cmd_buffer->args, len + 1);  // \0
  if (nread != (long)(len + 1)) {
    free(cmd_buffer);
    return -1;
  }

  // Ensure null termination
  if (cmd_buffer->args[len] != '\0') {
    free(cmd_buffer);
    return -1;
  }

  *cmd = cmd_buffer;

  return 1;
}