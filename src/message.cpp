
#include <string.h>
#include "message.h"

Message msg_gen(CMD_IDENT ident, uint8_t payload)
{
    return Message{0xAA, ident, payload, 0xFF};
}

uint32_t msg_to_bytes(Message *msg)
{
    uint32_t raw = 0;

    raw |= msg->end;
    raw |= msg->payload << 8;
    raw |= msg->cmd_ident << 8 * 2;
    raw |= msg->header << 8 * 3;
    return raw;
}
