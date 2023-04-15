#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdint.h>

struct Message {
    uint8_t header;
    uint8_t cmd_ident;
    uint8_t payload;
    uint8_t end;
};

enum CMD_IDENT {
    MOTOR = 0x00,
    SENSOR = 0x01,
};

enum CMD_MOTOR_IDENT {
    // Forward wheel drive (Right)
    FWD_R = 0x01,
    // Forward wheel drive (Left)
    FWD_L = 0x02,
    // Rear wheel drive (Right)
    RWD_R = 0x04,
    // Rear wheel drive (Left)
    RWD_L = 0x08,
    // Join to make a bitmask
};

Message msg_gen (CMD_IDENT ident, uint8_t payload);
uint32_t msg_to_bytes(Message * msg);
#endif