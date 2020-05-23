#ifndef _CS8900_H
#define _CS8900_H

#include "type.h"

#define RTDATA 0x0
#define TxCmd 4
#define TxCmd_TxStart_Full 0x00c0
#define TxLen 6
#define POINTER 0xa
#define DATA0 0xc

#define CS8900_PRDID 0x630e

#define PRDID 0
#define SelfCTL 0x0114
#define SelfCTL_Reset 6
#define SelfST 0x0136
#define SelfST_InitD 7
#define SelfST_ELpres 0xb
#define BusCTL 0x0116
#define BusCTL_MemE 0xa
#define BusCTL_UseSA 9
#define IA 0x0158
#define RxCTL 0x0104
#define RxCTL_BoardCastA 0xb
#define RxCTL_IndividualA 0xa
#define RxCTL_RxOKA 8
#define RxCFG 0x0102
#define TxCFG 0x0106
#define BufCFG 0x010a
#define LineCTL 0x0112
#define LineCTL_SetTxON 7
#define LineCTL_SetRxON 6
#define RxEvt 0x0124
#define RxEvt_RxOK 8
#define BusST 0x0138
#define BusST_TxRDY 8
#define TxEvt 0x0128
#define TxEvt_CRS 6
#define TxEvt_TxOK 8

uint cs8900_init(unsigned char * mac);
uint cs8900_halt();
uint cs8900_open();
uint cs8900_recv(unsigned short *buf);
uint cs8900_send(unsigned short *buf, uint length);
uint cs8900_is_ready();

#endif
