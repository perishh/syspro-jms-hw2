#include "command.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

char* pack_command(const Command* cmd) {
  char* buffer = malloc(CMD_RAW_HEADER_SIZE + cmd->len);
  if (!buffer) {
    return NULL;
  }

  // byteorder(3)
  uint16_t action = htons(cmd->action);
  uint32_t data = htonl(cmd->data);
  uint32_t len = htonl(cmd->len);

  memcpy(buffer, &action, sizeof(action));
  memcpy(buffer + sizeof(action), &data, sizeof(data));
  memcpy(buffer + sizeof(action) + sizeof(data), &len, sizeof(len));
  memcpy(buffer + CMD_RAW_HEADER_SIZE, cmd->args, cmd->len);

  return buffer;
}

int unpack_command(int fd, char* buffer, Command** cmd) {
  ssize_t nread = read_all(fd, buffer, CMD_RAW_HEADER_SIZE);
  if (nread != CMD_RAW_HEADER_SIZE) {
    if (nread == 0) {
      return 0;
    }
    return -1;
  }

  uint32_t len;
  memcpy(&len, buffer + sizeof(uint16_t) + sizeof(uint32_t), sizeof(uint32_t));
  len = ntohl(len);

  Command* cmd_buffer = malloc(sizeof(Command) + len);
  if (!cmd_buffer) {
    return -1;
  }

  *cmd = cmd_buffer;

  memcpy(&(cmd_buffer->action), buffer, sizeof(uint16_t));
  cmd_buffer->action = ntohs(cmd_buffer->action);

  memcpy(&(cmd_buffer->data), buffer + sizeof(uint16_t), sizeof(uint32_t));
  cmd_buffer->data = ntohl(cmd_buffer->data);

  cmd_buffer->len = len;
  memcpy(cmd_buffer->args, buffer + CMD_RAW_HEADER_SIZE, len);

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
    return 1;
  }

  // TODO: Maybe read blocking
  nread = read_all(fd, cmd_buffer->args, len + 1);  // \0
  if (nread < 0) {
    free(cmd_buffer);
    return -1;
  }

  // Ensure null termination
  if (cmd_buffer->args[len] != '\0') {
    free(cmd_buffer);
    return -1;
  }

  return 1;
}