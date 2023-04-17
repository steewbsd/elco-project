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
    MOTOR = 0x01,
    SENSOR = 0x02,
    ACK = 0x03,
};

enum CMD_MOTOR_IDENT {
    /* These 4 values represent moving the weel forwards */
    // Forward wheel drive (Right)
    FWD_R = 0x01,
    // Forward wheel drive (Left)
    FWD_L = 0x02,
    // Rear wheel drive (Right)
    RWD_R = 0x04,
    // Rear wheel drive (Left)
    RWD_L = 0x08,
    /* These 4 values represent moving the weel backwards */
    // Forward wheel drive (Right) - go backwards
    FWD_R_BACK = 0x010,
    // Forward wheel drive (Left) - go backwards
    FWD_L_BACK = 0x20,
    // Rear wheel drive (Right) - go backwards
    RWD_R_BACK = 0x40,
    // Rear wheel drive (Left) - go backwards
    RWD_L_BACK = 0x80,
};

struct Message msg_gen (enum CMD_IDENT ident, uint8_t payload);
uint32_t msg_to_bytes(struct Message * msg);
#endif