#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "sha256.h"
#include "xmodem.h"

#define rGPFCON    (*(volatile unsigned *)0x56000050) //Port F control
#define rGPFDAT    (*(volatile unsigned *)0x56000054) //Port F data
#define rGPFUP     (*(volatile unsigned *)0x56000058) //Pull-up control F

#define rGPCCON    (*(volatile unsigned *)0x56000020) //Port C control
#define rGPCDAT    (*(volatile unsigned *)0x56000024) //Port C data
#define rGPCUP     (*(volatile unsigned *)0x56000028) //Pull-up control C
                        
#define rGPDCON    (*(volatile unsigned *)0x56000030) //Port D control
#define rGPDDAT    (*(volatile unsigned *)0x56000034) //Port D data
#define rGPDUP     (*(volatile unsigned *)0x56000038) //Pull-up control D
                        
#define rGPECON    (*(volatile unsigned *)0x56000040) //Port E control
#define rGPEDAT    (*(volatile unsigned *)0x56000044) //Port E data
#define rGPEUP     (*(volatile unsigned *)0x56000048) //Pull-up control E
//for LTV350QV  TFT 320240
#define CLKVAL_TFT_320240	(6)
                        
// LCD CONTROLLER
#define rLCDCON1    (*(volatile unsigned *)0x4d000000) //LCD control 1
#define rLCDCON2    (*(volatile unsigned *)0x4d000004) //LCD control 2
#define rLCDCON3    (*(volatile unsigned *)0x4d000008) //LCD control 3
#define rLCDCON4    (*(volatile unsigned *)0x4d00000c) //LCD control 4
#define rLCDCON5    (*(volatile unsigned *)0x4d000010) //LCD control 5
#define rLCDSADDR1  (*(volatile unsigned *)0x4d000014) //STN/TFT Frame buffer start address 1
#define rLCDSADDR2  (*(volatile unsigned *)0x4d000018) //STN/TFT Frame buffer start address 2
#define rLCDSADDR3  (*(volatile unsigned *)0x4d00001c) //STN/TFT Virtual screen address set
#define rREDLUT     (*(volatile unsigned *)0x4d000020) //STN Red lookup table
#define rGREENLUT   (*(volatile unsigned *)0x4d000024) //STN Green lookup table 
#define rBLUELUT    (*(volatile unsigned *)0x4d000028) //STN Blue lookup table
#define rDITHMODE   (*(volatile unsigned *)0x4d00004c) //STN Dithering mode
#define rTPAL       (*(volatile unsigned *)0x4d000050) //TFT Temporary palette
#define rLCDINTPND  (*(volatile unsigned *)0x4d000054) //LCD Interrupt pending
#define rLCDSRCPND  (*(volatile unsigned *)0x4d000058) //LCD Interrupt source
#define rLCDINTMSK  (*(volatile unsigned *)0x4d00005c) //LCD Interrupt mask
#define rLPCSEL     (*(volatile unsigned *)0x4d000060) //LPC3600 Control
#define PALETTE     0x4d000400                         //Palette start address

//TFT 320240
#define LCD_XSIZE_TFT_320240 	(320)	
#define LCD_YSIZE_TFT_320240 	(240)

//TFT 320240
#define SCR_XSIZE_TFT_320240 	(320)
#define SCR_YSIZE_TFT_320240 	(240)

#define LTV350QV_VBPD     5
#define LTV350QV_VFPD     5
#define LTV350QV_VSPW    15
#define LTV350QV_HBPD    5
#define LTV350QV_HFPD    15
#define LTV350QV_HSPW   8
#define HOZVAL_TFT_320240	(LCD_XSIZE_TFT_320240-1)
#define LINEVAL_TFT_320240	(LCD_YSIZE_TFT_320240-1)
#define MVAL		(13)
#define MVAL_USED 	(0)		//0=each frame   1=rate by MVAL
#define BSWP		(0)		//Byte swap control
#define HWSWP		(1)		//Half word swap control
#define M5D(n) ((n) & 0x1fffff)	// To get lower 21bits

// UART
#define rULCON0     (*(volatile unsigned *)0x50000000) //UART 0 Line control
#define rUCON0      (*(volatile unsigned *)0x50000004) //UART 0 Control
#define rUFCON0     (*(volatile unsigned *)0x50000008) //UART 0 FIFO control
#define rUMCON0     (*(volatile unsigned *)0x5000000c) //UART 0 Modem control
#define rUTRSTAT0   (*(volatile unsigned *)0x50000010) //UART 0 Tx/Rx status
#define rUERSTAT0   (*(volatile unsigned *)0x50000014) //UART 0 Rx error status
#define rUFSTAT0    (*(volatile unsigned *)0x50000018) //UART 0 FIFO status
#define rUMSTAT0    (*(volatile unsigned *)0x5000001c) //UART 0 Modem status
#define rUBRDIV0    (*(volatile unsigned *)0x50000028) //UART 0 Baud rate divisor

#define rULCON1     (*(volatile unsigned *)0x50004000) //UART 1 Line control
#define rUCON1      (*(volatile unsigned *)0x50004004) //UART 1 Control
#define rUFCON1     (*(volatile unsigned *)0x50004008) //UART 1 FIFO control
#define rUMCON1     (*(volatile unsigned *)0x5000400c) //UART 1 Modem control
#define rUTRSTAT1   (*(volatile unsigned *)0x50004010) //UART 1 Tx/Rx status
#define rUERSTAT1   (*(volatile unsigned *)0x50004014) //UART 1 Rx error status
#define rUFSTAT1    (*(volatile unsigned *)0x50004018) //UART 1 FIFO status
#define rUMSTAT1    (*(volatile unsigned *)0x5000401c) //UART 1 Modem status
#define rUBRDIV1    (*(volatile unsigned *)0x50004028) //UART 1 Baud rate divisor

#define rULCON2     (*(volatile unsigned *)0x50008000) //UART 2 Line control
#define rUCON2      (*(volatile unsigned *)0x50008004) //UART 2 Control
#define rUFCON2     (*(volatile unsigned *)0x50008008) //UART 2 FIFO control
#define rUMCON2     (*(volatile unsigned *)0x5000800c) //UART 2 Modem control
#define rUTRSTAT2   (*(volatile unsigned *)0x50008010) //UART 2 Tx/Rx status
#define rUERSTAT2   (*(volatile unsigned *)0x50008014) //UART 2 Rx error status
#define rUFSTAT2    (*(volatile unsigned *)0x50008018) //UART 2 FIFO status
#define rUMSTAT2    (*(volatile unsigned *)0x5000801c) //UART 2 Modem status
#define rUBRDIV2    (*(volatile unsigned *)0x50008028) //UART 2 Baud rate divisor

#if 0//big endian
#define rUTXH0      (*(volatile unsigned char *)0x50000023) //UART 0 Transmission Hold
#define rURXH0      (*(volatile unsigned char *)0x50000027) //UART 0 Receive buffer
#define rUTXH1      (*(volatile unsigned char *)0x50004023) //UART 1 Transmission Hold
#define rURXH1      (*(volatile unsigned char *)0x50004027) //UART 1 Receive buffer
#define rUTXH2      (*(volatile unsigned char *)0x50008023) //UART 2 Transmission Hold
#define rURXH2      (*(volatile unsigned char *)0x50008027) //UART 2 Receive buffer

#define WrUTXH0(ch) (*(volatile unsigned char *)0x50000023)=(unsigned char)(ch)
#define RdURXH0()   (*(volatile unsigned char *)0x50000027)
#define WrUTXH1(ch) (*(volatile unsigned char *)0x50004023)=(unsigned char)(ch)
#define RdURXH1()   (*(volatile unsigned char *)0x50004027)
#define WrUTXH2(ch) (*(volatile unsigned char *)0x50008023)=(unsigned char)(ch)
#define RdURXH2()   (*(volatile unsigned char *)0x50008027)

#define UTXH0       (0x50000020+3)  //Byte_access address by DMA
#define URXH0       (0x50000024+3)
#define UTXH1       (0x50004020+3)
#define URXH1       (0x50004024+3)
#define UTXH2       (0x50008020+3)
#define URXH2       (0x50008024+3)

#else //Little Endian
#define rUTXH0 (*(volatile unsigned char *)0x50000020) //UART 0 Transmission Hold
#define rURXH0 (*(volatile unsigned char *)0x50000024) //UART 0 Receive buffer
#define rUTXH1 (*(volatile unsigned char *)0x50004020) //UART 1 Transmission Hold
#define rURXH1 (*(volatile unsigned char *)0x50004024) //UART 1 Receive buffer
#define rUTXH2 (*(volatile unsigned char *)0x50008020) //UART 2 Transmission Hold
#define rURXH2 (*(volatile unsigned char *)0x50008024) //UART 2 Receive buffer

#define WrUTXH0(ch) (*(volatile unsigned char *)0x50000020)=(unsigned char)(ch)
#define RdURXH0()   (*(volatile unsigned char *)0x50000024)
#define WrUTXH1(ch) (*(volatile unsigned char *)0x50004020)=(unsigned char)(ch)
#define RdURXH1()   (*(volatile unsigned char *)0x50004024)
#define WrUTXH2(ch) (*(volatile unsigned char *)0x50008020)=(unsigned char)(ch)
#define RdURXH2()   (*(volatile unsigned char *)0x50008024)

#define UTXH0       (0x50000020)    //Byte_access address by DMA
#define URXH0       (0x50000024)
#define UTXH1       (0x50004020)
#define URXH1       (0x50004024)
#define UTXH2       (0x50008020)
#define URXH2       (0x50008024)
#endif


typedef unsigned int U32;
typedef unsigned short int U16;

void delay(U32 tt);
void Lcd_Tft_320X240_Init( void );
void Lcd_Tft_320X240_Init_from_uboot( void );
void draw_sq(int x1, int y1, int x2, int y2, int color);

uint32_t whichUart;
void s3c2440_serial_send_byte(unsigned char c)
{
        while(!(rUTRSTAT0 & 0x2));
        WrUTXH0(c&0xff);
}

static unsigned short random[16];
static uint32_t newrandom = 0, userandom = 0, runner = 0;
void random_init()
{
	userandom = 0;
	newrandom = 0;
	memset(random, 0, 32);
}
unsigned short random_u16()
{
	if((userandom&0xf) != (newrandom&0xf))/*random not empty*/
		return random[(userandom++)&0xf];
	return 0;
}
uint32_t random_st()
{
	return (newrandom - userandom)&0xf;
}
unsigned char s3c2440_serial_recv_byte()
{
        while(!(rUTRSTAT0 & 0x1))
        return RdURXH0();
}

uint32_t s3c2440_is_serial_recv()
{
	return (rUTRSTAT0 & 0x1);
}

char getkey(void)
{
    //if(whichUart==0)
    if(1)
    {       
        if(rUTRSTAT0 & 0x1)    //Receive data ready
            return RdURXH0();
        else
            return 0;
    }
    else if(whichUart==1)
    {
        if(rUTRSTAT1 & 0x1)    //Receive data ready
            return RdURXH1();
        else
            return 0;
    }
    else if(whichUart==2)
    {       
        if(rUTRSTAT2 & 0x1)    //Receive data ready
            return RdURXH2();
        else
            return 0;
    }    
    return 0;	
}
void putch(int data)
{
    //if(whichUart==0)
		runner++;
	if(((newrandom - userandom)&0xf) != 15)/*random not full*/
		random[(newrandom++)&0xf]=runner&0xffff;	
    if(1)
    {
        if(data=='\n')
        {
            while(!(rUTRSTAT0 & 0x2));
            //delay(1);                 //because the slow response of hyper_terminal 
            WrUTXH0('\r');
        }
        while(!(rUTRSTAT0 & 0x2));   //Wait until THR is empty.
        //delay(1);
        WrUTXH0(data);
    }
    else if(whichUart==1)
    {
        if(data=='\n')
        {
            while(!(rUTRSTAT1 & 0x2));
            delay(1);                 //because the slow response of hyper_terminal 
            rUTXH1 = '\r';
        }
        while(!(rUTRSTAT1 & 0x2));   //Wait until THR is empty.
        delay(1);
        rUTXH1 = data;
    }   
    else if(whichUart==2)
    {
        if(data=='\n')
        {
            while(!(rUTRSTAT2 & 0x2));
            delay(1);                 //because the slow response of hyper_terminal 
            rUTXH2 = '\r';
        }
        while(!(rUTRSTAT2 & 0x2));   //Wait until THR is empty.
        delay(1);
        rUTXH2 = data;
    }       
}               

unsigned char halfbyte2char(unsigned char c)
{
        return ((c & 0x0f) < 0x0a)?(0x30 + c):('A' + c - 0x0a);
}

void puthexch(char c)
{
    putch(halfbyte2char(c>>4));
    putch(halfbyte2char(c&0xf));
}

void put_hex_uint(U32 i)
{
    int c = 8;
    while(c--){
        putch(halfbyte2char((char)((i&0xf0000000)>>28)));
        i<<=4;
    }
}

void puthexchars(char *pt)
{
    while(*pt){
        puthexch(*pt++);
        putch(' ');
    }
}

void putchars(const char *pt)
{
    while(*pt)
        putch(*pt++);
}

#define lprint lprintf
#define print_string putchars
unsigned char * num2str(uint32_t jt, unsigned char * s, unsigned char n)
{
        unsigned char * st, k = 1, j;
        uint32_t tmp;

        st = s;
        if(n > 16 || n < 2){
                *st++ = 0x30;
                *st = 0;
                return s;
        }
        tmp = 1;
        while(jt/tmp >= n){
                k++;
                tmp *= n;
        }

        while(k--){
                j = jt/tmp;
                *st++ = halfbyte2char(j);
                jt -= tmp * j;
                tmp = tmp/n;
        }
        *st = 0;
        return s;
}

void print_uint(uint32_t num)
{
        unsigned char nc[11];
        num2str(num, nc, 10);
        print_string(nc);
}

void print_hex(uint32_t num)
{
        unsigned char nc[9];
        num2str(num, nc, 16);
        print_string(nc);
}

void print_binary(uint32_t num)
{
        unsigned char nc[33];
        num2str(num, nc, 2);
        print_string(nc);
}

void lprintf(char *fmt, ...)
{
#if 0
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    putchars(string);
    va_end(ap);
#endif
    const unsigned char *s;
    uint32_t d;
    va_list ap;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') {
            con_send(*fmt++);
            continue;
        }
        switch (*++fmt) {
	    case '%':
	        con_send(*fmt);
		break;
            case 's':
                s = va_arg(ap, const unsigned char *);
                print_string(s);
                break;
            case 'u':
                d = va_arg(ap, uint32_t);
                print_uint(d);
                break;
	    /*
	    case 'c':
                d = va_arg(ap, char);
                send_int(d);
                break;
	    */
	    case 'x':
                d = va_arg(ap, uint32_t);
                print_hex(d);
                break;
	    case 'b':
                d = va_arg(ap, uint32_t);
                print_binary(d);
                break;
            /* Add other specifiers here... */             
            default: 
                con_send(*(fmt-1));
		con_send(*fmt);
                break;
        }
        fmt++;
    }
    va_end(ap);
}

/*
 *ADC
 * */
#define Uart_Printf lprintf
#define Uart_GetKey getkey
#define FCLK 200000000
#define HCLK (FCLK/2)
#define PCLK (HCLK/2)
#define ADCPRS 9
// ADC
#define rADCCON    (*(volatile unsigned *)0x58000000) //ADC control
#define rADCTSC    (*(volatile unsigned *)0x58000004) //ADC touch screen control
#define rADCDLY    (*(volatile unsigned *)0x58000008) //ADC start or Interval Delay
#define rADCDAT0   (*(volatile unsigned *)0x5800000c) //ADC conversion data 0
#define rADCDAT1   (*(volatile unsigned *)0x58000010) //ADC conversion data 1                   
                        
#define LOOP 1
int ReadAdc(int ch)
{
    int i;
    static int prevCh=-1;

    if(prevCh!=ch)
        {
        rADCCON=(1<<14)+(ADCPRS<<6)+(ch<<3);
        for(i=0;i<LOOP;i++);
        prevCh=ch;
        }
    rADCCON=(1<<14)+(ADCPRS<<6)+(ch<<3);
    rADCTSC = rADCTSC & 0xfb;
    rADCCON|=0x1;

    while(rADCCON & 0x1);
    while(!(rADCCON & 0x8000));
    return (rADCDAT0&0x3ff);
}

void Test_Adc(void)
{
    int a0=0,a1=0;
    int i,j;

    Uart_Printf("The ADC_IN are adjusted to the following values.\n");        
    Uart_Printf("Push any key to exit!\n");    
    Uart_Printf("ADC conv. freq.=%d(Hz)\n",(int)(PCLK/(ADCPRS+1.)));
    
    while(Uart_GetKey()==0)
    {

    a0=ReadAdc(0);
    a1=ReadAdc(1);
    Uart_Printf("AIN0: %04d,   AIN1: %04d\n", a0 ,a1);
    put_hex_uint((U32)a0);
    putch(' ');
    put_hex_uint((U32)a1);
    putch('\n');
   
    for (i=0;i<3000;i++)
    {
    	for (j=0;j<1000;j++) ;
    }
    }
    
    rADCCON=(0<<14)+(19<<6)+(7<<3)+(1<<2); 
    Uart_Printf("rADCCON = 0x%x\n", rADCCON);
}

/****
 * touch screen test
 * */
// ADC
#define rADCCON    (*(volatile unsigned *)0x58000000) //ADC control
#define rADCTSC    (*(volatile unsigned *)0x58000004) //ADC touch screen control
#define rADCDLY    (*(volatile unsigned *)0x58000008) //ADC start or Interval Delay
#define rADCDAT0   (*(volatile unsigned *)0x5800000c) //ADC conversion data 0
#define rADCDAT1   (*(volatile unsigned *)0x58000010) //ADC conversion data 1                   
// INTERRUPT
#define rSRCPND     (*(volatile unsigned *)0x4a000000) //Interrupt request status
#define rINTMOD     (*(volatile unsigned *)0x4a000004) //Interrupt mode control
#define rINTMSK     (*(volatile unsigned *)0x4a000008) //Interrupt mask control
#define rPRIORITY   (*(volatile unsigned *)0x4a00000a) //IRQ priority control
#define rINTPND     (*(volatile unsigned *)0x4a000010) //Interrupt request status
#define rINTOFFSET  (*(volatile unsigned *)0x4a000014) //Interruot request source offset
#define rSUBSRCPND  (*(volatile unsigned *)0x4a000018) //Sub source pending
#define rINTSUBMSK  (*(volatile unsigned *)0x4a00001c) //Interrupt sub mask

#define BIT_ADC        (0x1<<31)
#define BIT_SUB_TC     (0x1<<9)

int count=0;
volatile int xdata, ydata;

#define LESS 0x54
#define MAX 0x3AA
U32 div_local(U32 c1, U32 c2)
{
    U32 ret=0;
    while(c1 >= c2){
        ret++;
        c1 -= c2;
    }
    return ret;
}

U32 transfer_to_xy_ord(U32 in, U32 max)
{
    U32 ret;
    ret = max * (in - LESS) /(U32) (MAX - LESS);
    //ret = div_local(max * (in - LESS), (U32) (MAX - LESS));
    return ret;
}

void TS_handle(void)
{
	int i;
	U32 saveAdcdly;
    U32 x, y;

    if(rADCDAT0&0x8000)
    {
	Uart_Printf("\nStylus Up!\n");
	rADCTSC&=0xff;
    }
    else 
	Uart_Printf("\nStylus Down!\n");

	rADCTSC=(1<<3)|(1<<2);
	saveAdcdly=rADCDLY;
	rADCDLY=40000;

	rADCCON|=0x1;

		while(rADCCON & 0x1);
    lprintf("1111\n");
		while(!(rADCCON & 0x8000));
    lprintf("2222\n");
		
            //while(!(rSRCPND & (BIT_ADC)));
    lprintf("3333\n");

            xdata=(rADCDAT0&0x3ff);
            ydata=(rADCDAT1&0x3ff);

	 rSUBSRCPND|=BIT_SUB_TC;
	rSRCPND = BIT_ADC;
	rINTPND = BIT_ADC;
	 rINTSUBMSK=~(BIT_SUB_TC);
	 rINTMSK=~(BIT_ADC);
			 
	 rADCTSC =0xd3;
	 rADCTSC=rADCTSC|(1<<8);

			while(1)
			{
			 if(rSUBSRCPND & (BIT_SUB_TC))
				 {
					Uart_Printf("Stylus Up Interrupt!\n");
					break;
				}
			}	

    Uart_Printf("count=%d XP=%04d, YP=%04d\n", count++, xdata, ydata);     

    putch('\n');
    put_hex_uint((U32)xdata);
    putch(' ');
    put_hex_uint((U32)ydata);
    putch('\n');
    x = transfer_to_xy_ord(xdata, 320);
    y = transfer_to_xy_ord(ydata, 240);
    y = 240 - y;
    draw_sq(x, y, x+20, y+20, 0x7e0);
	rADCDLY=saveAdcdly; 
	rADCTSC=rADCTSC&~(1<<8);
    rSUBSRCPND|=BIT_SUB_TC;
    rINTSUBMSK=~(BIT_SUB_TC); 
	rSRCPND = BIT_ADC;
	rINTPND = BIT_ADC;
}

void AdcTS_init()
{
    rADCDLY=50000;
    rADCCON=(1<<14)+(ADCPRS<<6);
    rADCTSC=0xd3;
	rINTMSK|=BIT_ADC;
	rINTSUBMSK|=(BIT_SUB_TC);
}

void Test_AdcTs(void)
{
    AdcTS_init();

	Uart_Printf("\nType any key to exit!\n");
	Uart_Printf("\nStylus Down, please...... \n");
    while(!getkey()){
        if(rSUBSRCPND & BIT_SUB_TC){
            TS_handle();
        }
    }
	//Uart_Getch();
    //if(getkey())return;

	rINTSUBMSK|=BIT_SUB_TC;
	rINTMSK|=BIT_ADC;
	Uart_Printf("[Touch Screen Test.]\n");

}
/****
 * */

static volatile unsigned short* LCD_BUFER;
/**
 *
 */
#define COM_MAX_LEN 32
#define ENTER_CHAR 0x0d
#define CLEAN_OS_VERSION "0.1"
#define PLATFORM "S3C2440"
typedef uint32_t uint;
static uint32_t * mrw_addr = 0x0;
uint32_t cmd_buf_p = 0;
uint32_t exit_os = 0;
struct command{
     unsigned char * cmd_name;
     void (*cmd_fun)(unsigned char *);
     unsigned char * cmd_des;
};

static unsigned char cmd_buf[COM_MAX_LEN];
void print_help(unsigned char *para);
unsigned char * str_to_hex(unsigned char *s, uint * result);
uint get_howmany_para(unsigned char *s);
void reboot(unsigned char *p)
{
	lprintf("rebooting...\r\n");
	delay(100);
	((void(*const)())0)();
}

void exit_clean_os(unsigned char *p)
{
	lprintf("Exiting Clean OS...Bye!\r\n");
    exit_os = 1;
}

void rw_byte(unsigned char *p)
{
    uint addr, tmp, c;

    tmp = get_howmany_para(p);
    if(tmp != 1 && tmp != 2)
        goto error;
    p = str_to_hex(p, &addr);
    if(tmp == 1)
	goto read;
    p = str_to_hex(p, &c);
    c &= 0xff;
write:
    *(unsigned char*)addr = c;
    lprint("write %x to addr %x\r\n", c, addr);
    return;
read:
    lprint("read %x at addr %x\r\n", *(unsigned short*)addr&0xff, addr);
    return;

error:
    lprint("Error para!\r\nrww (hex addr) [value], will write if have value para\r\n");

}

void rw_word(unsigned char *p)
{
    uint addr, tmp, c;

    tmp = get_howmany_para(p);
    if(tmp != 1 && tmp != 2)
        goto error;
    p = str_to_hex(p, &addr);
    addr &= ~1;
    if(tmp == 1)
	goto read;
    p = str_to_hex(p, &c);
    c &= 0xffff;
write:
    *(unsigned short*)addr = c;
    lprint("write %x to addr %x\r\n", c, addr);
    return;
read:
    lprint("read %x at addr %x\r\n", *(unsigned short*)addr&0xffff, addr);
    return;

error:
    lprint("Error para!\r\nrww (hex addr) [value], will write if have value para\r\n");

}

uint get_howmany_para(unsigned char *s)
{
	uint tmp = 0;
	while(1){
		while(*s == ' ')
			s++;
		if(*s)
			tmp++;
		while(*s != ' ' && *s)
			s++;
		if(!*s)
			return tmp;
	}
}

uint asc_to_hex(unsigned char c)
{
	uint v;

	if(c >= '0' && c <= '9')
		return c - '0';	
	if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0;
}

unsigned char * str_to_hex(unsigned char *s, uint * result)
{
	uint  i = 0;

	*result = 0;
	while(*s == ' ')s++;
	for(i=0;i<8;i++){
		if(*s == ' ' || *s == 0)
			break;
		*result = *result*16 + asc_to_hex(*s++);
	}
	while(*s == ' ')s++;
	return s;
}

void write_mem(unsigned char *p)
{
    uint value, tmp;

    tmp = get_howmany_para(p);
    if(tmp == 0 || tmp > 2)
	goto error;
    p = str_to_hex(p, &value);
    if(tmp == 1)
        goto write;
    str_to_hex(p, (uint*)&mrw_addr);
    mrw_addr = (uint*)((uint)mrw_addr & 0xfffffffc);
write:
    *(uint*)mrw_addr = value;
    lprint("Write 0x%x to memory 0x%x done!\r\n",value,mrw_addr);
    return;

error:
    lprint("Error para!\r\nw (hex value) [(hex addr)](last addr if no this argu)\r\n");

}

void read_mem(unsigned char *p)
{
    uint value, tmp;

    tmp = get_howmany_para(p);
    if( tmp > 1)
	goto error;
    if(tmp == 0)
    	goto read;
    str_to_hex(p, (uint*)&mrw_addr);
    mrw_addr = (uint*)((uint)mrw_addr & 0xfffffffc);
read:
    value = *(uint*)mrw_addr;
    lprint("Read 0x%x at memory 0x%x\r\n",value,mrw_addr);

    return;

error:
    lprint("Error para!\r\nr [(hex addr)](last addr if no this argu)\r\n");

}

void print_mem(unsigned char *cp, uint length)
{
    uint i;

    while(length){
	lprint("\r\n");
	for(i=0;i<8;i++){
		puthexch(*cp++);
        putch(' ');
		length--;
		if(!length)
			break;
	}
    }
}

void pm(unsigned char *p)
{
    uint length = 0x80, tmp, i;

    tmp = get_howmany_para(p);
    if( tmp > 1)
        goto error;
    if(tmp == 0)
        goto print;
    str_to_hex(p, &length);
print:
    lprint("Start print 0x%x mem content @%x:\r\n", length, (uint)mrw_addr);
    print_mem((unsigned char*)mrw_addr, length);
    lprint("\r\nPrint end @%x.\r\n", (uint)mrw_addr);
    return;

error:
    lprint("Error para!\r\npm [length](default 0x80 if no this argu)\r\n");

}

void fillmem(unsigned char *p)
{
    int i;
    uint addr, tmp, len, fdata;
    uint*ip;

    tmp = get_howmany_para(p);
    if(tmp != 3)
        goto error;
    p = str_to_hex(p, &addr);
    addr &= ~1;
    p = str_to_hex(p, &len);
    p = str_to_hex(p, &fdata);

    len/=4;
    ip =  (uint*)addr;
    while(len--){
        *ip++ = fdata;
    }

    return;

error:
    lprint("Error para!\r\nfm (hex addr) (hex len of bytes) (hex 32bit data)\r\n");

}

char result[SHA256_SUM_LEN];
void sha256(unsigned char *p)
{
    int i;
    uint addr, tmp, len;

    tmp = get_howmany_para(p);
    if(tmp != 2)
        goto error;
    p = str_to_hex(p, &addr);
    addr &= ~1;
    p = str_to_hex(p, &len);

    sha256_context my_ctx;
    sha256_starts(&my_ctx);
    sha256_update(&my_ctx, (char*)addr, len);
    sha256_finish(&my_ctx, result);
    lprint("sha256 digest of addr %x len %x:\r\n", addr, len);
    for(i=0;i<SHA256_SUM_LEN;i++){
    puthexch(result[i]&0xff);
    }
    lprintf("\n");
    return;

error:
    lprint("Error para!\r\nsha256 (hex addr) (len)\r\n");

}

void go(unsigned char *para)
{
	lprint("This will go at the addr you just used with the 'r' cmd. Any problem please check!\r\n");
	(*((void (*)())mrw_addr))();
}

void get_file_by_serial(unsigned char *para)
{
	uint tmp;
	lprint("Please send file...\r\n");
    tmp = get_howmany_para(para);
	xmodem_1k_recv((unsigned char*)mrw_addr);
	lprint("done.\r\n");
}

extern uint get_psr();
void prt(unsigned char *p)
{
	//get_psr();
	lprint("current CPSR = 0x%x\r\n", get_psr());
}

void test(unsigned char *p)
{
    uint addr, tmp, len, start_ip, ffcount, last, loopct, data;
    uint mininumb = 0x8;
    uint*ip;

    data = 0xffffffff;
    tmp = get_howmany_para(p);
    if(tmp < 2)
        goto error;
    p = str_to_hex(p, &addr);
    addr &= ~1;
    p = str_to_hex(p, &len);
    if(tmp == 3){
        p = str_to_hex(p, &data);
    }
    if(tmp == 4){
        p = str_to_hex(p, &mininumb);
    }

    ip = (uint*)addr;
    ffcount = 0;
    start_ip = (uint)ip;
    loopct = len/4;
    last = 0;
    while(loopct--){
        if(data == *ip){
            ffcount++;
            if(last != data){
                start_ip = (uint)ip;
            }
        }
        else{
            if(ffcount > mininumb){
                lprintf("%x:%x(%x)\r\n", (uint)start_ip, ffcount*4, data);
            }
            ffcount = 0;
        }
        last = *ip++;
    }
    return;

error:
    lprint("Error para!\r\nmc (hex addr) (hex len) [data] [miniN]\r\n");

}
void tftpget(unsigned char *p)
{
	if(get_howmany_para(p) != 1)
		goto error;
	while(*p == ' ')
		p++;
	tftp_get(p, mrw_addr);
	return;
error:
    lprint("Error para!\r\ntftpget (name)\r\n");
}
void tftpput(unsigned char *p)
{
	uint sz;
	if(get_howmany_para(p) != 2)
		goto error;
    	p = str_to_hex(p, &sz);
	tftp_put(p, sz, mrw_addr);
	return;
error:
    lprint("Error para!\r\ntftpput (filesize) (name)\r\n");
}

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
void nand_reset()
{
	uint tmp = 10;
	NFCONF = (7<<12)|(7<<8)|(7<<4)|(0<<0);
	NFCONT = (1<<4)|(0<<1)|(1<<0);
	NFSTAT = 0x4;
	NFCMD = 0xff;
	while(tmp--);
        NAND_CHIP_DISABLE;
}

/* low level nand read function */
int nand_read_ll(unsigned char *buf, unsigned long start_addr, int size)
{
        int i, j;
	
	lprint("Copy Command:membuf=%x, nandaddr=%x, size=%x\r\n", buf, start_addr, size);
        if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
                return -1;      /* invalid alignment */
        }
	if(!(NFSTAT&0x1)){
		lprint("nand flash may have some problem, quit!\r\n");
		return -1;
	}

        NAND_CHIP_ENABLE;

        for(i=start_addr; i < (start_addr + size);) {
                /* READ0 */
                NAND_CLEAR_RB;
                NFCMD = 0;

                /* Write Address */
                NFADDR = i & 0xff;
                NFADDR = (i >> 9) & 0xff;
                NFADDR = (i >> 17) & 0xff;
                NFADDR = (i >> 25) & 0xff;

                NAND_DETECT_RB;

                for(j=0; j < NAND_SECTOR_SIZE; j++, i++) {
                        *buf = (NFDATA & 0xff);
                        buf++;
                }
		if(!((i>>9) & 0x3f))
			s3c2440_serial_send_byte('>');
        }
        NAND_CHIP_DISABLE;
        return 0;
}

#define ERASE_BLOCK_ADDR_MASK (512 * 32 -1)
int nand_erase_ll(uint addr)
{
        lprint("Erase Command:addr=%x\r\n", addr);
	if(addr & ERASE_BLOCK_ADDR_MASK){
		lprint("erase addr not correct!\r\n");
		return -1;
	}	
        if(!(NFSTAT&0x1)){
                lprint("nand flash may have some problem, quit!\r\n");
                return -1;
        }
/*
	if(is_marked_bad_block(addr)){
                lprint("block %x is bad block, quit!\r\n",addr);
                return -1;
	}
*/
        NAND_CHIP_ENABLE;
        NAND_CLEAR_RB;
	NFCMD = 0x60;
        NFADDR = (addr >> 9) & 0xff;
        NFADDR = (addr >> 17) & 0xff;
        NFADDR = (addr >> 25) & 0xff;
	NFCMD = 0xd0;
	while(!(NFSTAT & 0x1)){
#ifdef NAND_DEBUG
		lprint("%x\r\n", NFSTAT);
#endif	
	}
	NFCMD = 0x70;
	if(NFDATA & 0x1){
		lprint("erase failed! may get bad.\r\n");
        	NAND_CHIP_DISABLE;
		return -1;
	}
	lprint("erase successfully! \r\n");
        NAND_CHIP_DISABLE;
	return 0;
}

int nand_write_ll(unsigned char *buf, unsigned long start_addr, int size)
{
        uint i, j;

        lprint("Write Command:membuf=%x, nandaddr=%x, size=%x\r\n", buf, start_addr, size);
        if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
                return -1;      /* invalid alignment */
        }
        if(!(NFSTAT&0x1)){
                lprint("nand flash may have some problem, quit!\r\n");
                return -1;
        }

        NAND_CHIP_ENABLE;

        for(i=start_addr; i < (start_addr + size);) {
                /* READ0 */
                NAND_CLEAR_RB;
                NFCMD = 0x80;

                /* Write Address */
                NFADDR = i & 0xff;
                NFADDR = (i >> 9) & 0xff;
                NFADDR = (i >> 17) & 0xff;
                NFADDR = (i >> 25) & 0xff;


                for(j=0; j < NAND_SECTOR_SIZE; j++, i++) {
                        NFDATA = *buf++;
                }
		NFCMD = 0x10;
        	NAND_DETECT_RB;
		
		while(!NFSTAT&0x1);
		NFCMD = 0x70;
		if(NFDATA & 0x1){
			lprint("current block(%x)program failed! may get bad.\r\n", (i-512)&~ERASE_BLOCK_ADDR_MASK);
        		NAND_CHIP_DISABLE;
			return -1;
		}	
		
                if(!((i>>9) & 0x3f))
                        s3c2440_serial_send_byte('<');
        }
        NAND_CHIP_DISABLE;
        return 0;
}
                    
void nandcp(unsigned char *p)
{
    uint addr, size, pages, tmp;

    tmp = get_howmany_para(p);
    if(tmp != 2)
        goto error;
    p = str_to_hex(p, &addr);
    str_to_hex(p, &size);
    addr = addr & 0xfffffe00;
    pages = size/512;
cp:
    nand_reset();
    nand_read_ll((unsigned char*)mrw_addr, addr, 512 * pages);
    lprint("cp size 0x%x(0x%x pages) from nand addr %x to memory 0x%x done!\r\n",
            size, pages,addr,mrw_addr);
    return;

error:
    lprint("Error para!\r\nnandcp (hex addr) (hex size)\r\n");

}

void nander(unsigned char *p)
{
    uint addr, tmp, size, n;

    tmp = get_howmany_para(p);
    if(tmp != 2)
        goto error;
    p = str_to_hex(p, &addr);
    p = str_to_hex(p, &size);
    if(addr%0x4000 != 0){
        lprint("addr must be 0x4000 x n\r\n");
        goto error;
    }
    if(size%0x4000 != 0){
        lprint("size must be 0x4000 x n\r\n");
        goto error;
    }
    n = size/0x4000;
cp:
    nand_reset();
    while(n--){
        if(nand_erase_ll(addr)){
            lprint("erase error\r\n");
        }
        else{
            lprint("erase nand block 0x%x done!\r\n",addr);
        }
        addr+=0x4000;
    }
    return;

error:
    lprint("Error para!\r\nnander (hex block addr) (hex size)\r\n");

}

void nandpp(unsigned char *p)
{
    uint addr, pages, tmp, size;

    tmp = get_howmany_para(p);
    if(tmp != 2)
        goto error;
    p = str_to_hex(p, &addr);
    str_to_hex(p, &size);
    if(size%512 != 0){
        lprintf("size should be 512*n\r\n");
        goto error;
    }
    pages = size/512;
    addr = addr & 0xfffffe00;

    nand_reset();
    if(nand_write_ll(mrw_addr, addr, 512 * pages)){
	lprint("failed\r\n");
	return;
    }
    lprint("program 0x%x pages:size %x from memory 0x%x to nand addr %x done!\r\n",
            pages,size,mrw_addr,addr);
    return;

error:
    lprint("Error para!\r\nnandcp (hex addr) (hex size)\r\n");

}

void lcd_drawing();
void lcddraw(unsigned char *p)
{
    lcd_drawing();
}
#define IPADDR(A, B, C, D) ((A)|(B)<<8|(C)<<16|(D)<<24)
uint local_ip = IPADDR(192, 168, 58, 60);
uint server_ip = IPADDR(192, 168, 58, 43);
const unsigned char cs8900_mac[]={0x00, 0x43, 0x33, 0x2f, 0xde, 0x22};
static const struct command cmd_list[]=
{
    {"cpsr",prt,"display the value in CPSR of cpu"},
    {"gfbs",get_file_by_serial,"get file by serial"},
    {"go",go,"jump to ram specified addr to go"},
    {"exit",exit_clean_os,"exit clean os"},
    {"fm",fillmem,"fill memory with data"},
    {"help",print_help,"help message"},
    {"lcddraw",lcddraw,"lcd drawing"},
    {"nandcp",nandcp, "copy nand data to ram specified addr"},
    {"nander",nander, "erase nand"},
    {"nandpp",nandpp, "nand program page from memory"},
#if 0
    {"nandr",nandr,"random read nand data"},
    {"nandspr",nandspr,"random read nand spare data"},
    {"nandwb",nandwb,"random write nand byte"},
    {"ndbb",ndbb,"check if one nand block is marked bad"},
    {"ndchkbb",ndchkbb,"scan all flash marked bad block"},
    {"pfbs",put_file_by_serial,"put file by serial"},
#endif
    {"pm",pm,"print memory content"},
    {"r",read_mem,"read mem, can set specified addr for other cmd"},
    {"reboot",reboot,"restart run program to zero addr"},
    {"rww",rw_word,"read/write word"},
    {"rwb",rw_byte,"read/write byte"},
    //{"setip",setip,"set ip addr of local & server"},
    {"sha256",sha256,"compute the sha256 digest of memory"},
    {"test",test,"use for debug new command or function"},
    {"tftpget",tftpget,"get file from tftp server"},
    {"tftpput",tftpput,"put file to tftp server from membase"},
    {"w",write_mem,"write mem, also can set mem addr"},
    {NULL, NULL, NULL},
};
void print_help(unsigned char *para)
{
    uint32_t i = 0;
    lprintf("Clean OS V%s\r\nAvailable cmd is:\r\n\r\n", CLEAN_OS_VERSION);
    while(1){
            if(cmd_list[i].cmd_name == NULL)
                    break;
	    lprintf("--%s: %s\r\n", cmd_list[i].cmd_name, cmd_list[i].cmd_des);
	    //lprintf("--%s\r\n\t%s\r\n", cmd_list[i].cmd_name, cmd_list[i].cmd_des);
            i++;
    }
    lprintf("\r\n'r' is a special command, if a address followed, it will be set as a memory base for many other command, such as 'pm', and so on\r\nESC will cancel current command\r\n");
}

void handle_cmd()
{
    unsigned char i = 0, *p_cmd, *p_buf, *cmd_start;

    lprintf("\r\n");
    cmd_start = cmd_buf;
    while(*cmd_start == ' ')
	cmd_start++;
    if(!*cmd_start)
	return;
    while(1){
	    if(cmd_list[i].cmd_name == NULL)
		    break;
	    p_cmd=cmd_list[i].cmd_name;
	    p_buf=cmd_start;
	    while(*p_cmd){
		    if(*p_buf != *p_cmd)
			    break;
		    p_buf++;
		    p_cmd++;
	    }
	    if(!(*p_cmd) && (*p_buf == ' ' || !(*p_buf))){
            	    cmd_list[i].cmd_fun(p_buf);
            	    return;
       	    }
	    i++;
    }
    lprintf("Unknow command:%s\r\n",cmd_buf);
}
static void get_cmd()
{
	unsigned char c;
start_get_cmd:
	lprintf("\r\nCleanOS@%s>", PLATFORM);
	memset(cmd_buf, 0, COM_MAX_LEN);
	cmd_buf_p = 0;
	while(1){
        while( !(c = getkey()));
/*
		if(c == ENTER_CHAR){
			return;
		}else if(c == 0x1b){
			goto start_get_cmd;
		}else{
			if(cmd_buf_p < (COM_MAX_LEN - 1)){
				cmd_buf[cmd_buf_p++] = c;
				con_send(c);
			}
		}
*/

		switch(c){
			case ENTER_CHAR:
				return;
			case 0x1b:
				goto start_get_cmd;
			case 0x08:
				if(cmd_buf_p > 0){
					cmd_buf[--cmd_buf_p] = 0;
					lprintf("\b \b");
				}
				continue;
			default:
				if(cmd_buf_p < (COM_MAX_LEN - 1)){
					cmd_buf[cmd_buf_p++] = c;
					putch(c);
				}
		}

	}
}	

void run_clean_os()
{
	lprintf("\r\n\r\nHello, this is clean_boot v%s%s build on %s %s.\r\n", CLEAN_OS_VERSION,
            GIT_SHA1, __DATE__,__TIME__);
	memset(cmd_buf, 0, COM_MAX_LEN);
	cmd_buf_p = 0;
    exit_os = 0;
	while(!exit_os){
		get_cmd();
		handle_cmd();
	}
}
void some_init()
{
    LCD_BUFER = (volatile unsigned short*)0x37000000;
    whichUart = 0;
    Lcd_Tft_320X240_Init_from_uboot();
}

int enter_confirm()
{
    int ct = 5;
    int ct1 = 0;
    AdcTS_init();
    lprint("Any key or touch screen in 5 seconds will go CleanOS, or quit...\r\n");
    while(ct){
        if(s3c2440_is_serial_recv()){
            lprint("Uart Key down\r\n");
            return 1;
        }
        if(rSUBSRCPND & BIT_SUB_TC){
            rSUBSRCPND|=BIT_SUB_TC;
            lprint("Touch screen down\r\n");
            return 1;
        }
        delay(10);
        if(ct1++ > 40){
            ct1 = 0;
            lprint("Count down %u\r\n", ct--);
        }
    }
    return 0;
}

int main(void)
{
    if(enter_confirm() != 1)
    {
        return 0;
    }
    random_init();
    cs8900_init(cs8900_mac);
    some_init();
#if 0
    int a = 10;
    char * strprint="helloworkd";
    LCD_BUFER = (volatile unsigned short*)0x37000000;
    rGPFCON = 0x55aa;
    whichUart = 0;
    Lcd_Tft_320X240_Init_from_uboot();
    while(a--){
        if(rGPFDAT & 0x10){
            rGPFDAT = 0;
            //rUTXH0 = '#';
            puthexch('#');
            putch('#');
            putch('7');
            putch('\n');
        }
        else{
            rGPFDAT = 0xff;
            putch('@');
            putch('8');
            putch('\n');
        }
        delay(10);
        //lprintf("+++++++\n");
        //puthexchars("++++++");
        put_hex_uint((U32)strprint);
        put_hex_uint(0x1234abcd);
        putchars(strprint);
    }
    Test_Adc();
    Test_AdcTs();
#endif
    run_clean_os();
    return 0;
}
void delay(U32 tt)
{
    U32 i;
    for(;tt>0;tt--)
    {
        for(i=0;i<10000;i++){}
    }
}
#if 1

/**************************************************************
320¡Á240 16Bpp TFT LCDÊý¾ÝºÍ¿ØÖÆ¶Ë¿Ú³õÊ¼»¯
**************************************************************/
static void Lcd_Port_Init(void)
{
	
    rGPCUP=0xffffffff; // Disable Pull-up register
    rGPCCON=0xaa9556a9; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
                                           //LCDVF[0],[1],[2]---output;VD[0],[1],[2]----output.
    rGPDUP=0xffffffff; // Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]     VD[8]---output
	
    #if 0
	
    //rGPCUP = 0xffffffff; // Disable Pull-up register
    rGPCUP = 0x0; // enable Pull-up register
    rGPCCON = 0xaaaa56a9; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 

    //rGPDUP = 0xffffffff ; // Disable Pull-up register
    rGPDUP = 0x0 ; // enable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]

   #endif
}

/**************************************************************
320¡Á240 16Bpp TFT LCD¹¦ÄÜÄ£¿é³õÊ¼»¯
**************************************************************/
static void Lcd_Init(void)
{
	
    lprintf("Lcd Init enter\r\n");
	rLCDCON1=(CLKVAL_TFT_320240<<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;
    	// TFT LCD panel,16bpp TFT,ENVID=off
	rLCDCON2=(LTV350QV_VBPD<<24)|(LINEVAL_TFT_320240<<14)|(LTV350QV_VFPD<<6)|(LTV350QV_VSPW);
	rLCDCON3=(LTV350QV_HBPD<<19)|(HOZVAL_TFT_320240<<8)|(LTV350QV_HFPD);
	rLCDCON4=(MVAL<<8)|(LTV350QV_HSPW);
	rLCDCON5=(1<<11)|(1<<10)|(1<<9)|(1<<8)|(0<<6)|(1<<3)|(BSWP<<1)|(HWSWP);	//FRM5:6:5,VCLK,HSYNC and VSYNC are inverted
	rLCDSADDR1=(((U32)LCD_BUFER>>22)<<21)|M5D((U32)LCD_BUFER>>1);
	rLCDSADDR2=M5D( ((U32)LCD_BUFER+(SCR_XSIZE_TFT_320240*LCD_YSIZE_TFT_320240*2))>>1 );
	rLCDSADDR3=(((SCR_XSIZE_TFT_320240-LCD_XSIZE_TFT_320240)/1)<<11)|(LCD_XSIZE_TFT_320240/1);
	

	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
//	rTCONSEL&=(~7); // Disable LPC3600 diff 2410
	rTPAL=0; // Disable Temp Palette
    lprintf("Lcd Init quit\r\n");
}

/**************************************************************
LCDÊÓÆµºÍ¿ØÖÆÐÅºÅÊä³ö»òÕßÍ£Ö¹£¬1¿ªÆôÊÓÆµÊä³ö
**************************************************************/
static void Lcd_EnvidOnOff(int onoff)
{
    if(onoff==1)
	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}


void SET_IF();
void CLR_IF();
/**************************************************************
320¡Á240 16Bpp TFT LCDÒÆ¶¯¹Û²ì´°¿Ú
**************************************************************/
static void Lcd_MoveViewPort(int vx,int vy)
{
    U32 addr;

    SET_IF(); 
	#if (LCD_XSIZE_TFT_320240<32)
    	    while((rLCDCON1>>18)<=1); // if x<32
	#else	
    	    while((rLCDCON1>>18)==0); // if x>32
	#endif
    
    addr=(U32)LCD_BUFER+(vx*2)+vy*(SCR_XSIZE_TFT_320240*2);
	rLCDSADDR1= ( (addr>>22)<<21 ) | M5D(addr>>1 );
	rLCDSADDR2= M5D(((addr+(SCR_XSIZE_TFT_320240*LCD_YSIZE_TFT_320240*2))>>1));
	CLR_IF();
}    

/**************************************************************
320¡Á240 16Bpp TFT LCDÒÆ¶¯¹Û²ì´°¿Ú
**************************************************************/
static void MoveViewPort(void)
{
    int vx=0,vy=0,vd=1;

    lprintf("\n*Move the LCD view windos:\n");
    lprintf(" press 8 is up\n");
    lprintf(" press 2 is down\n");
    lprintf(" press 4 is left\n");
    lprintf(" press 6 is right\n");
    lprintf(" press Enter to exit!\n");

    while(1)
    {
    	switch(getkey())
    	{
    	case '8':
	    if(vy>=vd)vy-=vd;    	   	
        break;

    	case '4':
    	    if(vx>=vd)vx-=vd;
    	break;

    	case '6':
                if(vx<=(SCR_XSIZE_TFT_320240-LCD_XSIZE_TFT_320240-vd))vx+=vd;    

				lprintf("vx=%3d,vy=%3d\n",vx,vy);
   	    break;

    	case '2':
                if(vy<=(SCR_YSIZE_TFT_320240-LCD_YSIZE_TFT_320240-vd))vy+=vd;   

				lprintf("vx=%3d,vy=%3d\n",vx,vy);
   	    break;

    	case '\r':
   	    return;

    	default:
	    break;
		}
	//lprintf("vx=%3d,vy=%3d\n",vx,vy);
	Lcd_MoveViewPort(vx,vy);
    }
}

/**************************************************************
320¡Á240 16Bpp TFT LCDµ¥¸öÏóËØµÄÏÔÊ¾Êý¾ÝÊä³ö
**************************************************************/
static void PutPixel(U32 x,U32 y,U32 c)
{
    volatile unsigned short* lbp;
	if ( (x < SCR_XSIZE_TFT_320240) && (y < SCR_YSIZE_TFT_320240) ){
	//LCD_BUFER[(y)][(x)] = c;
        lbp = LCD_BUFER + y*SCR_XSIZE_TFT_320240 + x;
        *lbp = c;
    }
}

/**************************************************************
320¡Á240 16Bpp TFT LCDÈ«ÆÁÌî³äÌØ¶¨ÑÕÉ«µ¥Ôª»òÇåÆÁ
**************************************************************/
static void Lcd_ClearScr(U16 c)
{
	unsigned int x,y ;
    volatile unsigned short* lbp;
		
    for( y = 0 ; y < SCR_YSIZE_TFT_320240 ; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE_TFT_320240 ; x++ )
    	{
			//LCD_BUFER[y][x] = c;
            lbp = LCD_BUFER + y*SCR_XSIZE_TFT_320240 + x;
            *lbp = c;
#if 0
            if(c != 0xffff){
            put_hex_uint((U32)lbp);
            putch('\n');
            }
#endif
        }
    }
}

/**************************************************************
LCDÆÁÄ»ÏÔÊ¾´¹Ö±·­×ª
// LCD display is flipped vertically
// But, think the algorithm by mathematics point.
//   3I2
//   4 I 1
//  --+--   <-8 octants  mathematical cordinate
//   5 I 8
//   6I7
**************************************************************/
static void Glib_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}


/**************************************************************
ÔÚLCDÆÁÄ»ÉÏÓÃÑÕÉ«Ìî³äÒ»¸ö¾ØÐÎ
**************************************************************/
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}


void Lcd_Tft_320X240_Init( void )
{
    int x,y;
    lprintf("Lcd_Tft_320X240_Init enter\r\n");
   	Lcd_Port_Init();

    Lcd_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio

	Lcd_ClearScr(0xffff);		//fill all screen with some color
	Lcd_ClearScr(0x0000);		//fill all screen with some color
	Lcd_ClearScr(0xf81f);		//fill all screen with some color

    for(x = 0; x<320; x++){
        y=x/2;
        PutPixel(x,y,0x7e0);
    }

	//Glib_FilledRectangle( 0, 0, 100, 100,0x0000);		//fill a Rectangle with some color
#if 0
	#define LCD_BLANK		16
	#define C_UP		( LCD_XSIZE_TFT_320240 - LCD_BLANK*2 )
	#define C_RIGHT		( LCD_XSIZE_TFT_320240 - LCD_BLANK*2 )
	#define V_BLACK		( ( LCD_YSIZE_TFT_320240 - LCD_BLANK*4 ) / 6 )
	Glib_FilledRectangle( LCD_BLANK, LCD_BLANK, ( LCD_XSIZE_TFT_320240 - LCD_BLANK ), ( LCD_YSIZE_TFT_320240 - LCD_BLANK ),0x0000);		//fill a Rectangle with some color

	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*0), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*1),0x001f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*1), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*2),0x07e0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*2), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*3),0xf800);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*3), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*4),0xffe0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*4), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*5),0xf81f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*5), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*6),0x07ff);		//fill a Rectangle with some color
#endif
    lprintf("Lcd_Tft_320X240_Init quit\r\n");
}

#define diff(x,y) (x>y?x-y:y-x)

void draw_sq(int x1, int y1, int x2, int y2, int color)
{
    int d, x, y;
    d = 1;
    if(x1>x2)d=-1;
    x=x1;
    do{
        x += d;
        PutPixel(x,y1,color);
        PutPixel(x,y2,color);
    }while(x!=x2);
    d = 1;
    if(y1>y2)d=-1;
    y=y1;
    do{
        y += d;
        PutPixel(x1,y,color);
        PutPixel(x2,y,color);
    }while(y!=y2);
}
#if 0
void draw_line(int x1, int y1, int x2, int y2, int color)
{
    float lv;
    int d, x, y;
    if(diff(x1,x2)>diff(y1,y2)){
        d=(x2-x1)/diff(x1,x2);
        lv = (float)(y2-y1)/(x2-x1);
        x=x1;
        do{
            x += d;
            y = y1+lv*(float)(x-x1);
            PutPixel(x,y,color);
        }while(x!=x2);
    }
    else
    {
        d=(y2-y1)/diff(y1,y2);
        lv = (float)(x2-x1)/(y2-y1);
        y=y1;
        do{
            y += d;
            x = x1+lv*(float)(y-y1);
            PutPixel(x,y,color);
        }while(y!=y2);
    }
}
#endif

/*
 * uboot register value of LCD control
4d000000: 01700779 033bc14f 00a13f00 0000002b    y.p.O.;..?..+...
4d000010: 0001cb09 19800000 00012c00 00000140    .........,..@...
*/
void Lcd_Tft_320X240_Init_from_uboot( void )
{
    lprintf("Lcd_Tft_320X240_Init enter\r\n");
    //Lcd_Port_Init();

    Lcd_EnvidOnOff(0);
	rLCDSADDR1=(((U32)LCD_BUFER>>22)<<21)|M5D((U32)LCD_BUFER>>1);
    //Lcd_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio
	Lcd_ClearScr(0xffff);		//fill all screen with some color

	//Glib_FilledRectangle( 0, 0, 100, 100,0x0000);		//fill a Rectangle with some color
#if 1
	#define LCD_BLANK		16
	#define C_UP		( LCD_XSIZE_TFT_320240 - LCD_BLANK*2 )
	#define C_RIGHT		( LCD_XSIZE_TFT_320240 - LCD_BLANK*2 )
	#define V_BLACK		( ( LCD_YSIZE_TFT_320240 - LCD_BLANK*4 ) / 6 )
	Glib_FilledRectangle( LCD_BLANK, LCD_BLANK, ( LCD_XSIZE_TFT_320240 - LCD_BLANK ), ( LCD_YSIZE_TFT_320240 - LCD_BLANK ),0x0000);		//fill a Rectangle with some color

	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*0), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*1),0x001f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*1), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*2),0x07e0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*2), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*3),0xf800);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*3), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*4),0xffe0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*4), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*5),0xf81f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*5), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*6),0x07ff);		//fill a Rectangle with some color
#endif
    lprintf("Lcd_Tft_320X240_Init quit\r\n");
}

void lcd_drawing()
{
    int x,y;
	Lcd_ClearScr(0x0000);		//fill all screen with some color
	Lcd_ClearScr(0xf81f);		//fill all screen with some color

r:
    for(x = 0; x<320; x++){
        y=x/2;
        //y=sqrt(x)*10.0f;
        PutPixel(x,y,0x7e0);
        draw_sq(x,y, x+20, y+20, 0x7e0);
        delay(10);
        if(getkey())return;
        draw_sq(x,y, x+20, y+20, 0xf81f);
    }
    for(x = 320; x>0; x--){
        y=x/2;
        PutPixel(x,y,0x7e0);
        draw_sq(x,y, x+20, y+20, 0x7e0);
        delay(10);
        if(getkey())return;
        draw_sq(x,y, x+20, y+20, 0xf81f);
    }
    goto r;

}
#endif
