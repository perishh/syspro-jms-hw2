#include "command.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

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

Command* unpack_command(const char* buffer) {
  uint32_t len;
  memcpy(&len, buffer + sizeof(uint16_t) + sizeof(uint32_t), sizeof(uint32_t));
  len = ntohl(len);

  Command* cmd = malloc(sizeof(Command) + len);
  if (!cmd) {
    return NULL;
  }

  memcpy(&(cmd->action), buffer, sizeof(uint16_t));
  cmd->action = ntohs(cmd->action);

  memcpy(&(cmd->data), buffer + sizeof(uint16_t), sizeof(uint32_t));
  cmd->data = ntohl(cmd->data);

  cmd->len = len;
  memcpy(cmd->args, buffer + CMD_RAW_HEADER_SIZE, len);

  return cmd;
}