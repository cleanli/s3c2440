#include "s3c2440.h"
#include "cs8900.h"
#include "debug.h"
#include <string.h>

#define NAMESIZE 20
struct eth_device {
        char name[NAMESIZE];
        unsigned char enetaddr[6];
        int iobase;
        int state;

        int  (*init) ();
        int  (*send) (volatile unsigned char* packet, uint length);
        int  (*recv) (volatile unsigned char* packet, uint *length);
        void (*halt) ();
        struct eth_device *next;
        void *priv;
};
/*
void lmemcpy(unsigned char*d, unsigned char*s, uint len)
{
	while(len--)
		*d++ = *s++;
}
*/
static uint cs8900_not_ready = 1;
static inline unsigned short io_r(unsigned short raddr)
{
	wio(POINTER, raddr);
	return rio(DATA0);
}

static inline void io_w(unsigned short raddr, unsigned short data)
{
	wio(POINTER, raddr);
	wio(DATA0, data);
}

uint cs8900_halt()
{
	mem_w(LineCTL, 0); 
}
uint cs8900_open()
{
	/*start rx and tx*/
	mem_w(LineCTL, (1<<LineCTL_SetTxON)|(1<<LineCTL_SetRxON)); 
}

uint cs8900_proble()
{
	if(io_r(PRDID) != CS8900_PRDID)
		return 0;
	lprintf("Found cs8900!\r\n");
	return 1;
}
void delay_us(uint d)
{
	while(d--){
		__asm__ __volatile__("nop\n\rnop\n\tnop\n\t");
	}
}
uint cs8900_reset()
{
	io_w(SelfCTL, io_r(SelfCTL)|(1<<SelfCTL_Reset));

	/*need up 10ms*/
	delay_us(100000);
	if(!(io_r(SelfST)|(1<<SelfST_InitD)))
		return 0;
	lprintf("reset cs8900 sucess\r\n");
	return 1;
}
uint cs8900_swi_mem()
{
	if((io_r(SelfST)|(1<<SelfST_ELpres)) == 0)
		return 0;
	/*left memory base addr default zero*/
	io_w(BusCTL, io_r(BusCTL)|(1<<BusCTL_MemE)|(1<<BusCTL_UseSA));
	if(mem_r(PRDID) != CS8900_PRDID)
		return 0;
	lprintf("switch to mem mode.\r\n");
	return 1;
}
void cs8900_setmac(const unsigned char *mac)
{
	unsigned char *ia_p = (unsigned char *)MEM_ADDR(IA);

	memcpy(ia_p, mac, 6);
}
void cs8900_init_reg()
{
	/*receive boardcast & individual & crc ok packages*/
	mem_w(RxCTL, (1<<RxCTL_BoardCastA)|(1<<RxCTL_IndividualA)|(1<<RxCTL_RxOKA));
	/*no interrupt*/
	mem_w(RxCFG, 0);
	mem_w(TxCFG, 0);
	mem_w(BufCFG, 0);
}
uint cs8900_init(const unsigned char * mac)
{
	lprintf("\r\nloading cs8900 driver...");
	if(!cs8900_proble())
		goto error;
	if(!cs8900_reset())
		goto error;
	if(!cs8900_swi_mem())
		goto error;
	cs8900_setmac(mac);
	cs8900_init_reg();
	cs8900_not_ready = 0;
	lprintf("cs8900 is ready.");
	return 0;
error:
	lprintf("error, cs8900 not in use!\r\n");
	return -1;
}

uint cs8900_is_ready()
{
	return !cs8900_not_ready;
}

uint cs8900_recv(unsigned short *buf)
{
	uint i, rxlen;
	unsigned short status;

	status = mem_r(RxEvt);

	if ((status & (1<<RxEvt_RxOK)) == 0)
		return 0;

	status = rio(RTDATA);		/* stat */
	rxlen = rio(RTDATA);		/* len */

	for (i = rxlen >> 1; i > 0; i--)
		*buf++ = rio(RTDATA);
	if (rxlen & 1)
		*buf++ = rio(RTDATA);

	return rxlen;
}

uint cs8900_send (unsigned short *buf, uint length)
{
	uint i, delay = 0x10000;
	unsigned short s;
retry:
	/* initiate a transmit sequence */
	wio(TxCmd, TxCmd_TxStart_Full);
	wio(TxLen, length);

	/* Test to see if the chip has allocated memory for the packet */
	if (mem_r(BusST) & (1<<BusST_TxRDY) == 0) 
		return 0;	

	/* Write the contents of the packet */
	/* assume even number of bytes */
	for (i = length>>1; i > 0; i--)
		wio(RTDATA, *buf++);
	if(length&1)
		wio(RTDATA, *buf++);
	/* wait for transfer to succeed */
	while(delay--);
	delay == 0xe0000;
	while((s=mem_r(TxEvt)) & ~0x1F == 0) {
		if (!delay--)
			break;
	}

	/* nothing */ ;
	if ((s & (1<<TxEvt_CRS | 1<<TxEvt_TxOK)) != 1<<TxEvt_TxOK) 
		return 0;
	return length;
}
