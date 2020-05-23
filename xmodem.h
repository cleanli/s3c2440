#ifndef _XMODEM_H
#define _XMODEM_H
#include<stdint.h>

#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NACK 0x15
#define CAN 0x18

uint32_t time_limit_recv_byte(uint32_t limit, unsigned char * c);
uint32_t xmodem_1k_recv(unsigned char *);
#define con_send(X) s3c2440_serial_send_byte(X)
#define con_recv() s3c2440_serial_recv_byte()
#define is_con_recv() s3c2440_is_serial_recv()
#endif
