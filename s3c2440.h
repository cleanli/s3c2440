/*s3c2440.h:define the related func & header for s3c2440*/

#ifndef _S3C2440_H
#define _S3C2440_H
#include "type.h"
#include "debug.h"

/*Serial reg*/
#define USCON0 *(volatile unsigned long *)0x50000010
#define UTXH0 *(volatile unsigned long *)0x50000020
#define URXH0 *(volatile unsigned long *)0x50000024
#define __REGb(x)       (*(volatile unsigned char *)(x))
#define __REGi(x)       (*(volatile unsigned int *)(x))
#define NF_BASE         0x4e000000

#define NFCONF          __REGi(NF_BASE + 0x0)
#define NFCONT          __REGi(NF_BASE + 0x4)
#define NFCMD           __REGb(NF_BASE + 0x8)
#define NFADDR          __REGb(NF_BASE + 0xC)
#define NFDATA          __REGb(NF_BASE + 0x10)
#define NFSTAT          __REGb(NF_BASE + 0x20)

#define NAND_CHIP_ENABLE  (NFCONT &= ~(1<<1))
#define NAND_CHIP_DISABLE (NFCONT |=  (1<<1))
#define NAND_CLEAR_RB     (NFSTAT |=  (1<<2))
#define NAND_DETECT_RB    { while(! (NFSTAT&(1<<2)) );}

#define NAND_SECTOR_SIZE        512
#define NAND_BLOCK_MASK         (NAND_SECTOR_SIZE - 1)

/*cs8900*/
#define CS8900_IO_BASE 0x19000300
#define CS8900_MEM_BASE 0x18000000
#define rio(PORT) *(volatile unsigned short*)(CS8900_IO_BASE + PORT)
#define wio(PORT, DATA) *(volatile unsigned short*)(CS8900_IO_BASE + PORT) = DATA
#define mem_r(RADDR) *(volatile unsigned short*)(CS8900_MEM_BASE + RADDR)
#define mem_w(RADDR, DATA) *(volatile unsigned short*)(CS8900_MEM_BASE + RADDR) = DATA
#define MEM_ADDR(RADDR) (CS8900_MEM_BASE + RADDR)

/*Serial reg*/
#define SRCPND *(volatile unsigned long *)0x4a000000
#define INTMOD *(volatile unsigned long *)0x4a000004
#define INTMASK *(volatile unsigned long *)0x4a000008
#define INTPND *(volatile unsigned long *)0x4a000010

/* low level nand read function */
int nand_read_ll(unsigned char *buf, unsigned long start_addr, int size);
void s3c2440_serial_init();
void s3c2440_serial_send_byte(unsigned char c);
unsigned char s3c2440_serial_recv_byte();
uint s3c2440_is_serial_recv();
void random_init();
void test_random();
unsigned short random_u16();
uint random_st();

#endif /*_S3C2440_H*/
