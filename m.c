#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "sha256.h"
#include "xmodem.h"
#include "common.h"
#include "cs8900.h"
#include "s3c2410.h"
#include "rtc.h"

enum UI_NAME_INDEX {
    UI_MAIN_MENU,
    UI_ADC,
    UI_MAX
};

#define rTCNTO4 (*(volatile unsigned *)0x51000040) //Timer count observation 4
#define INTNUM_S3C2440 32
//#define WAVE_DISP_VERTICAL
void ui_init();
int set_delayed_work(uint tct_10ms, func_p f, void*pa, int repeat);
void cancel_delayed_work(int index);
int rtc_get(struct rtc_time *tmp);
void adc_ui_init();
void mass_adc_get(uint*, uint);
void clear_touched();
int timer_init(void);
void clk_init();
ulong get_PCLK(void);
ulong get_FCLK(void);
static void PutPixel(U32 x,U32 y,U32 c);
static void Lcd_ClearScr(U16 c);
void draw_line(int x1, int y1, int x2, int y2, int color);
void interrutp_init();
void disable_arm_interrupt();
void enable_arm_interrupt();
volatile int x_ts_adc_data, y_ts_adc_data, touch_up = 0;
volatile int normal_adc_data;
interrupt_func isr_list[INTNUM_S3C2440] = {0};
int total_adc_time_index;
int total_adc_time_list[]={1,2,5,10,20,50,100,200,500,1000};
int debug[16];
struct delay_work_info delayed_works[]={
    {
        NULL,
        0,
        NULL,
        0,
        0
    },
};
#define NUMBER_OF_DELAYED_WORKS \
    (sizeof(delayed_works)/sizeof(struct delay_work_info))
int autotrigger;
int autotriggertime;

// ADC
#define rADCCON    (*(volatile unsigned *)0x58000000) //ADC control
#define rADCTSC    (*(volatile unsigned *)0x58000004) //ADC touch screen control
#define rADCDLY    (*(volatile unsigned *)0x58000008) //ADC start or Interval Delay
#define rADCDAT0   (*(volatile unsigned *)0x5800000c) //ADC conversion data 0
#define rADCDAT1   (*(volatile unsigned *)0x58000010) //ADC conversion data 1

#define BIT_ADC        (0x1<<31)
#define BIT_SUB_TC     (0x1<<9)
// INTERRUPT
#define rSRCPND     (*(volatile unsigned *)0x4a000000) //Interrupt request status
#define rINTMOD     (*(volatile unsigned *)0x4a000004) //Interrupt mode control
#define rINTMSK     (*(volatile unsigned *)0x4a000008) //Interrupt mask control
#define rPRIORITY   (*(volatile unsigned *)0x4a00000a) //IRQ priority control
#define rINTPND     (*(volatile unsigned *)0x4a000010) //Interrupt request status
#define rINTOFFSET  (*(volatile unsigned *)0x4a000014) //Interruot request source offset
#define rSUBSRCPND  (*(volatile unsigned *)0x4a000018) //Sub source pending
#define rINTSUBMSK  (*(volatile unsigned *)0x4a00001c) //Interrupt sub mask

#define BIT_TIMER4     (0x1<<14)
#define BIT_SUB_ADC    (0x1<<10)

#define rGPFCON    (*(volatile unsigned *)0x56000050) //Port F control
#define rGPFDAT    (*(volatile unsigned *)0x56000054) //Port F data
#define rGPFUP     (*(volatile unsigned *)0x56000058) //Pull-up control F

#define rGPCCON    (*(volatile unsigned *)0x56000020) //Port C control
#define rGPCDAT    (*(volatile unsigned *)0x56000024) //Port C data
#define rGPCUP     (*(volatile unsigned *)0x56000028) //Pull-up control C
                        
#define rGPDCON    (*(volatile unsigned *)0x56000030) //Port D control
#define rGPDDAT    (*(volatile unsigned *)0x56000034) //Port D data
#define rGPDUP     (*(volatile unsigned *)0x56000038) //Pull-up control D
                        
#define rGPGCON    (*(volatile unsigned *)0x56000060) //Port G control
#define rGPGDAT    (*(volatile unsigned *)0x56000064) //Port G data
#define rGPGUP     (*(volatile unsigned *)0x56000068) //Pull-up control G

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



void delay(U32 tt);
void Lcd_Tft_320X240_Init( void );
void Lcd_Tft_320X240_Init_from_reset( void );
void draw_sq(int x1, int y1, int x2, int y2, int color);

uint32_t whichUart;
uint32_t timer4_click;
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

int sput_hex_uint(char*s, U32 i)
{
    int c = 8;
    char* p = s;
    while(c--){
        *p++ = (halfbyte2char((char)((i&0xf0000000)>>28)));
        i<<=4;
    }
    return 8;
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

void video_drawstring (int xx, int yy, char *s);
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

int sprint_uint(char*s, uint32_t num)
{
    unsigned char nc[11];
    num2str(num, nc, 10);
    strcpy(s, nc);
    return strlen(nc);
}

void print_hex(uint32_t num)
{
        unsigned char nc[9];
        num2str(num, nc, 16);
        print_string(nc);
}

int sprint_hex(char*s, uint32_t num)
{
    unsigned char nc[9];
    num2str(num, nc, 16);
    strcpy(s, nc);
    return strlen(nc);
}

void print_binary(uint32_t num)
{
        unsigned char nc[33];
        num2str(num, nc, 2);
        print_string(nc);
}

char lprintf_buf[256];
//void lcd_printf(int x, int y, const char *fmt, ...);

#ifndef USE_LIB_VSPRINTF
void vslprintf(char*s_buf, const char *fmt, ...);
void vslprintf(char*s_buf, const char *fmt, ...)
{
    const unsigned char *s;
    uint32_t d;
    va_list ap;
    char*sp = s_buf;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') {
            *sp++ = *fmt++;
            continue;
        }
        switch (*++fmt) {
	    case '%':
	        *sp++ = (*fmt);
		break;
            case 's':
                s = va_arg(ap, const unsigned char *);
                strcpy(sp, s);
                sp += strlen(s);
                break;
            case 'u':
                d = va_arg(ap, uint32_t);
                sp += sprint_uint(sp, d);
                break;
	    /*
	    case 'c':
                d = va_arg(ap, char);
                send_int(d);
                break;
	    */
	    case 'x':
                d = va_arg(ap, uint32_t);
                sp += sprint_hex(sp, d);
                break;
	    case 'X':
                d = va_arg(ap, uint32_t);
                sp += sput_hex_uint(sp, d);
                break;
	    /*
	    case 'b':
                d = va_arg(ap, uint32_t);
                print_binary(d);
                break;
        */
            /* Add other specifiers here... */             
            default: 
                *sp++ = (*(fmt-1));
                *sp++ = (*fmt);
                break;
        }
        fmt++;
    }
    *sp = 0;
    va_end(ap);
}
#endif

#if 0
void lprintf(const char *fmt, ...)
{
#if 1
    va_list ap;

    va_start(ap,fmt);
    vslprintf(lprintf_buf,fmt,ap);
    putchars(lprintf_buf);
    va_end(ap);
#else
    putchars(fmt);
#endif
}
#endif

/*
 *ADC
 * */
#define Uart_Printf lprintf
#define Uart_GetKey getkey
/*
#define FCLK 200000000
#define HCLK (FCLK/2)
#define PCLK (HCLK/2)
*/
#define ADCPRS 24
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

uint timer_start_timer4;
uint timer_start_timer4buf;
void compute_time_start()
{
    timer_start_timer4 = timer4_click;
    timer_start_timer4buf  = rTCNTO4;
}

uint compute_time_end_us()
{
    uint ret, tmp, tmp1;
    tmp = rTCNTO4;
    tmp1 = timer4_click;
    //lprintf("%u %u %u %u\n", timer_start_timer4,timer_start_timer4buf,tmp1,tmp);
    if(tmp < timer_start_timer4buf){
        ret = timer_start_timer4buf - tmp;
    }
    else{
        ret = 0x3dcc + timer_start_timer4buf - tmp;
        tmp1--;
    }
    ret = ret * 10000 / 0x3dcc;
    tmp = tmp1 - timer_start_timer4;
    tmp *= 10000;
    ret += tmp;
    return ret;
}

void ludelay(uint delay_us)
{
    uint startct = rTCNTO4;
    //lprintf("del %u st %u\n", delay_us, startct);
    uint startct_tick = timer4_click;
    uint tmp = delay_us * 791 / 500;
    //lprintf("tmp %u\n", tmp);
    if(startct > tmp){
        tmp = startct - tmp;
        while(rTCNTO4 > tmp);
        return;
    }
    else if(startct == tmp){
        while(timer4_click == startct_tick);
        return;
    }
    else{
        tmp = startct+0x3dcc-tmp;
        while(timer4_click == startct_tick);
        while(rTCNTO4 > tmp);
        return;
    }
}

#define ADC_DATA_PROCESS(data) (((data)&0x3ff)>>2)
#ifdef WAVE_DISP_VERTICAL
#define TOTAL_DATA_NUMBER 240
#else
#define TOTAL_DATA_NUMBER 320
#endif
uint adc_data0[TOTAL_DATA_NUMBER];
uint adc_data1[TOTAL_DATA_NUMBER];
void Test_Adc(void)
{
    uint regbak1, regbak2, regbak3;
    int a0=0,a1=0;
    int i,j;
    int v0last=-1, v1last=-1, tct=0;
    uint back_color = 0x0000;
    uint front_color = 0xffff;
    uint track0_color = 0x0fff;
    uint track1_color = 0xf0ff;
    uint n = TOTAL_DATA_NUMBER;
    uint adc_get_time;
    uint delay_us;
    uint deb1, deb2;

    regbak1 = rADCCON;
    regbak2 = rADCTSC;
    regbak3 = rADCDLY;

    memset(adc_data1, 0, TOTAL_DATA_NUMBER*4);
    rADCDLY = 2;
    lprintf("adc_data0 %x\n", adc_data0);
    lprintf("adc_data1 %x\n", adc_data1);
    Uart_Printf("PCLK %u FCLK %u.\n", get_PCLK(), get_FCLK());
    Uart_Printf("ADC conv. freq.=%u(Hz)\n",(int)(get_PCLK()/(ADCPRS+1.)));
    
    //reg init
    //rADCCON=(1<<14)+(ADCPRS<<6)+(1<<3)+(1<<1);
    rADCCON=(1<<14)+(ADCPRS<<6)+(0<<3);
    rADCTSC = rADCTSC & 0xfb;
    //close adc int
    rINTMSK |= BIT_ADC;
    //read
    //a1 = (rADCDAT0&0x3ff);
    rADCCON|=0x1;
    lprintf("start tc %u %u\n", timer4_click, rTCNTO4);
    lprintf("total_adc_time_index %u %u\n", total_adc_time_index, total_adc_time_list[total_adc_time_index]);
    delay_us = total_adc_time_list[total_adc_time_index] * 1000 / TOTAL_DATA_NUMBER;
    lprintf("delay_us %u\n", delay_us);
    compute_time_start();
    if(total_adc_time_list[total_adc_time_index] == 1){
        mass_adc_get(adc_data1, n);
    }
    else{
        while(n--)
        {
            while(!(rADCCON & 0x8000));
            adc_data1[tct++] = rADCDAT0;
            //Uart_Printf("AIN0: %X,   AIN1: %X\n", a0 ,a1);
            deb1 = rTCNTO4;
            ludelay(delay_us);
            deb2 = rTCNTO4;
            rADCCON|=0x1;
        }
    }
    adc_get_time = compute_time_end_us();
    lprintf("end tc %u %u\n", timer4_click, rTCNTO4);
    //enable adc int
    rSUBSRCPND = BIT_SUB_TC;
    rSUBSRCPND = BIT_SUB_ADC;
    rINTMSK &= ~BIT_ADC;

    lprintf("rADCDLY %x deb %u %u\n", rADCDLY, deb1, deb2);
    lprintf("time %u us\n", adc_get_time);
    lcd_printf(240,5,"%uus", adc_get_time);
    n = TOTAL_DATA_NUMBER;
    while(n--){
        if(n > 1){
#ifdef WAVE_DISP_VERTICAL
            draw_line(256 - ADC_DATA_PROCESS(adc_data1[n]), n, 256 - ADC_DATA_PROCESS(adc_data1[n-1]), n-1, track1_color);
#else
            draw_line(n, 230-ADC_DATA_PROCESS(adc_data1[n]), n-1, 230-ADC_DATA_PROCESS(adc_data1[n-1]), track0_color);
#endif
        }
    }
    
    rADCCON = regbak1;
    rADCTSC = regbak2;
    rADCDLY = regbak3;
}

/****
 * touch screen test
 * */

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

            x_ts_adc_data=(rADCDAT0&0x3ff);
            y_ts_adc_data=(rADCDAT1&0x3ff);

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

    Uart_Printf("XP=%04d, YP=%04d\n", x_ts_adc_data, y_ts_adc_data);

    putch('\n');
    put_hex_uint((U32)x_ts_adc_data);
    putch(' ');
    put_hex_uint((U32)y_ts_adc_data);
    putch('\n');
    x = transfer_to_xy_ord(x_ts_adc_data, 320);
    y = transfer_to_xy_ord(y_ts_adc_data, 240);
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
    rADCCON=(1<<14)+(ADCPRS<<6);
    rADCTSC=0xd3;//wait pen down
    total_adc_time_index = 0;
    autotrigger = -1;
    cancel_delayed_work(0);
}

void Test_AdcTs(void)
{
    int x, y, lastx = -1, lasty;
    AdcTS_init();

    x_ts_adc_data = -1;
    y_ts_adc_data = -1;
	Uart_Printf("\nType any key to exit!\n");
	Uart_Printf("\nStylus Down, please...... \n");
    while(!getkey()){
        if(y_ts_adc_data > 0){
            x = transfer_to_xy_ord(x_ts_adc_data, 320);
            y = transfer_to_xy_ord(y_ts_adc_data, 240);
            y = 240 - y;
            lprint("x %u, y %u\n", x,y);
            draw_sq(x, y, x+20, y+20, 0x7e0);
            if(lastx >= 0){
                draw_line(lastx, lasty, x, y, 0x7e0);
            }
            x_ts_adc_data = -1;
            y_ts_adc_data = -1;
            lastx = x;
            lasty = y;
        }
    }

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

// WATCH DOG TIMER
#define rWTCON   (*(volatile unsigned *)0x53000000) //Watch-dog timer mode
#define rWTDAT   (*(volatile unsigned *)0x53000004) //Watch-dog timer data
#define rWTCNT   (*(volatile unsigned *)0x53000008) //Eatch-dog timer count

void raw_reboot()
{
	lprintf("rebooting...\r\n");
	/* Disable watchdog */
	rWTCON = 0x0000;

	/* Initialize watchdog timer count register */
	rWTCNT = 0x0001;

	/* Enable watchdog timer; assert reset at timer timeout */
	rWTCON = 0x0021;
}

void reboot(unsigned char *p)
{
    raw_reboot();
	while(1);	/* loop forever and wait for reset to happen */

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

unsigned char * str_to_demical(unsigned char *s, uint * result)
{
	uint  i = 0, tmp, err = 0;;

	*result = 0;
	while(*s == ' ')s++;
	for(i=0;i<8;i++){
		if(*s == ' ' || *s == 0)
			break;
        tmp = asc_to_hex(*s++);
        if(tmp > 9){
            tmp = 0;
            err = 1;
        }
		*result = *result*10 + tmp;
	}
	while(*s == ' ')s++;
    if(err == 1){
        result = 0;
    }
	return s;
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
    uint* p_uint = (uint*) cp;

    while(length){
        lprint("\r\n");
        put_hex_uint((uint)p_uint);
        putch(':');
        putch(' ');
        for(i=0;i<16;i+=4){
            if(length<4)
                break;
            length-=4;
            put_hex_uint(*p_uint++);
            putch(' ');
        }
    }
}

void pm(unsigned char *p)
{
    uint length = 0x100, tmp, i;

    tmp = get_howmany_para(p);
    if( tmp > 1)
        goto error;
    if(tmp == 0)
        goto print;
    str_to_hex(p, &length);
print:
    lprint("Start print 0x%x mem content @%x:\r\n", length, (uint)mrw_addr);
    print_mem((unsigned char*)mrw_addr, length);
    lprint("\r\n\r\nPrint end @%x.\r\n", (uint)mrw_addr);
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

void go(unsigned char *p)
{
    uint addr, tmp;

    tmp = get_howmany_para(p);
    if(tmp == 1){
        p = str_to_hex(p, &addr);
        mrw_addr = (uint32_t *) addr;
    }
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

extern void w_psr(uint d);
void wprt(unsigned char *p)
{
    uint tmp,data;
	if(get_howmany_para(p) != 1)
		goto error;
    str_to_hex(p, &data);
	w_psr(data);
	lprint("After write CPSR = 0x%x\r\n", get_psr());
	return;
error:
    lprint("Error para!\r\ncpsrw 32bit data\r\n");
}

void test(unsigned char *p)
{
    uint deb1, deb2,d;
    memset(debug, 0, 64);
    p = str_to_hex(p, &d);
            deb1 = rTCNTO4;
            ludelay(d);
            deb2 = rTCNTO4;
    lprintf("deb %u %u debug %x\n", deb1, deb2, debug);
}

void finddata(unsigned char *p)
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
    lprint("Error para!\r\nfinddata (hex addr) (hex len) [data] [miniN]\r\n");

}
void tftp_get(unsigned char* name, unsigned char *buf);
void tftp_put(unsigned char* name, uint sz, unsigned char *buf);
void tftpget(unsigned char *p)
{
	if(get_howmany_para(p) != 1)
		goto error;
	while(*p == ' ')
		p++;
	tftp_get(p, (unsigned char*)mrw_addr);
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
	tftp_put(p, sz, (unsigned char*)mrw_addr);
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

void memcopy(unsigned char *p)
{
    uint addr, size, pages, tmp;

    tmp = get_howmany_para(p);
    if(tmp != 2)
        goto error;
    p = str_to_hex(p, &addr);
    str_to_hex(p, &size);

    memcpy((char*)mrw_addr, (char*)addr, size);
    lprint("cp size 0x%x from addr %x to 0x%x done!\r\n",
            size, addr,mrw_addr);
    return;

error:
    lprint("Error para!\r\nmcp (hex source addr) (hex size)\r\n");

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
    if(nand_write_ll((unsigned char*)mrw_addr, addr, 512 * pages)){
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

void tstest(unsigned char *p)
{
    Test_AdcTs();
}

void adc(unsigned char *p)
{
    adc_ui_init();
    Test_Adc();
}

void dtoh(unsigned char *p)
{
    uint data,tmp;

    tmp = get_howmany_para(p);
    if(tmp != 1)
        goto error;
    p = str_to_demical(p, &data);
    lprint("Demical %u hex %x\r\n", data, data);
    return;

error:
    lprint("error para!\r\ndtoh (demical data)\r\n");

}

void htod(unsigned char *p)
{
    uint data,tmp;

    tmp = get_howmany_para(p);
    if(tmp != 1)
        goto error;
    p = str_to_hex(p, &data);
    lprint("Hex %x Demical %u\r\n", data, data);
    return;

error:
    lprint("error para!\r\ndtoh (demical data)\r\n");

}
#define IPADDR(A, B, C, D) ((A)|(B)<<8|(C)<<16|(D)<<24)
uint local_ip = IPADDR(192, 168, 58, 60);
uint server_ip = IPADDR(192, 168, 58, 43);
const unsigned char cs8900_mac[]={0x00, 0x43, 0x33, 0x2f, 0xde, 0x22};
static const struct command cmd_list[]=
{
    {"adc",adc,"adc test"},
    {"cpsr",prt,"display the value in CPSR of cpu"},
    {"cpsrw",wprt,"write the value in CPSR of cpu"},
    {"dtoh",dtoh,"transfer from demical to hex"},
    {"gfbs",get_file_by_serial,"get file by serial"},
    {"go",go,"jump to ram specified addr to go"},
    {"exit",exit_clean_os,"exit clean os"},
    {"finddata",finddata,"find 32bit data"},
    {"fm",fillmem,"fill memory with data"},
    {"help",print_help,"help message"},
    {"htod",htod,"transfer to demical from hex"},
    {"lcddraw",lcddraw,"lcd drawing"},
    {"mcp",memcopy,"memory copy"},
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
    {"tstest",tstest,"touch screen test"},
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
    struct rtc_time rtct;
    struct rtc_time* tmp = &rtct;
	lprintf("\r\n\r\nHello, this is clean_boot v%s%s build on %s %s.\r\n", CLEAN_OS_VERSION,
            GIT_SHA1, __DATE__,__TIME__);
    lcd_printf(10,10,"Hello,CleanOS v%s%s", CLEAN_OS_VERSION,GIT_SHA1);
    rtc_get(tmp);
	lprintf("Get DATE: %u-%u-%u (wday=%u)  TIME: %u:%u:%u\n",
	       tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
	       tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	lcd_printf(10,30,"Get DATE: %u-%u-%u (wday=%u)",
	       tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday);
	lcd_printf(10,50,"TIME: %u:%u:%u",
	       tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	memset(cmd_buf, 0, COM_MAX_LEN);
	cmd_buf_p = 0;
    exit_os = 0;
    while(1){
        get_cmd();
        handle_cmd();
        if(exit_os == 1){
            lprintf("exit_os is true\r\n");
            break;
        }
    }
}
#define VIDEO_FB_ADRS 0x37000000
void some_init()
{
    LCD_BUFER = (volatile unsigned short*)VIDEO_FB_ADRS;
    whichUart = 0;
    Lcd_Tft_320X240_Init_from_reset();
    interrutp_init();
}

void clear_touched()
{
    x_ts_adc_data = -1;
    y_ts_adc_data = -1;
    touch_up = 0;
}

int screen_touched()
{
    if(touch_up > 0){
        return 1;
    }
    else{
        return 0;
    }
}

int enter_confirm()
{
    struct rtc_time rtct;
    struct rtc_time* tmp = &rtct;
    int ct = 5;
    int ct1 = 0;
    rtc_get(tmp);
	lcd_printf(10,30,"Get DATE: %u-%u-%u (wday=%u)",
	       tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday);
	lcd_printf(10,50,"TIME: %u:%u:%u",
	       tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    lprint("Any key or touch screen in 5 seconds will go CleanOS, or quit...\r\n");
    clear_touched();
    while(ct){
        if(s3c2440_is_serial_recv()){
            lprint("Uart Key down\r\n");
            return 1;
        }
        if(screen_touched()){
            lprint("Touch screen down\r\n");
            return 2;
        }
        udelay(100*1000);
        if(ct1++ > 10){
            ct1 = 0;
            lcd_printf(10,70,"Count down %u", ct);
            lprint("Count down %u\r\n", ct--);
        }
    }
    return 0;
}

void print_message()
{
    lprintf("\r\n");
    lprintf("\r\n");
    lprintf("---------------------------------|\r\n");
    lprintf("        Cloud2440 Flash boot part|\r\n");
    lprintf("---------------------------------|\r\n");
    lprintf("\r\n");
    lprintf("0x00000\r\n");
    lprintf("|           MiniCleanBoot\r\n");
    lprintf("0x01000\r\n");
    lprintf("|\r\n");
    lprintf("|           MiniCB Config\r\n");
    lprintf("|\r\n");
    lprintf("0x04000\r\n");
    lprintf("|\r\n");
    lprintf("|           Empty\r\n");
    lprintf("|\r\n");
    lprintf("0x08000\r\n");
    lprintf("|\r\n");
    lprintf("|           Uboot config 1\r\n");
    lprintf("|\r\n");
    lprintf("0x0c000\r\n");
    lprintf("|\r\n");
    lprintf("|           Uboot config 2\r\n");
    lprintf("|\r\n");
    lprintf("0x10000\r\n");
    lprintf("|\r\n");
    lprintf("|\r\n");
    lprintf("|\r\n");
    lprintf("|           Uboot\r\n");
    lprintf("|\r\n");
    lprintf("|\r\n");
    lprintf("|\r\n");
    lprintf("0x48000\r\n");
    lprintf("|\r\n");
    lprintf("|\r\n");
    lprintf("|           CleanBoot\r\n");
    lprintf("|\r\n");
    lprintf("|\r\n");
    lprintf("0x60000 -------------------\r\n");
    lprintf("|\r\n");
    lprintf("|\r\n");
    lprintf("|           Kernel\r\n");
    lprintf(".\r\n");
    lprintf(".\r\n");
    lprintf(".\r\n");
    lprintf("---------------------------------|\r\n");
    lprintf("        Cloud2440 Flash boot part|\r\n");
    lprintf("---------------------------------|\r\n");
}

typedef struct button {
    int x1;
    int y1;
    int x2;
    int y2;
    p_func click_func;
    int ui_goto;
    int need_re_init_ui;
    const char* text;
} button_t;

void trigger_adc(void*p)
{
    ui_init();
    Test_Adc();
}

void trigger_config()
{
    if(autotrigger == -1){
        autotrigger = set_delayed_work(500, trigger_adc, NULL, 1);
        //autotrigger = 1;
        lprintf("autotrigger %u\n", autotrigger);
        lcd_printf(10,30,"AutoTriOn ");
        autotriggertime = 500 + timer4_click;
    }
    else{
        cancel_delayed_work(autotrigger);
        autotrigger = -1;
        lcd_printf(10,30,"AutoTriOff");
    }
}

int ui_exit;
void exit_ui()
{
    ui_exit = 1;
}

void adc_less_delay()
{
    if(total_adc_time_index>0){
        total_adc_time_index--;
    }
    lcd_printf(10,10,"Set:%ums    ", total_adc_time_list[total_adc_time_index]);
    lprintf("total_adc_time_index %u %u\n", total_adc_time_index, total_adc_time_list[total_adc_time_index]);
}

void adc_more_delay()
{
    if(total_adc_time_index<(sizeof(total_adc_time_list)/sizeof(int)-1)){
        total_adc_time_index++;
    }
    lcd_printf(10,10,"Set:%ums    ", total_adc_time_list[total_adc_time_index]);
    lprintf("total_adc_time_index %u %u\n", total_adc_time_index, total_adc_time_list[total_adc_time_index]);
}

button_t adc_ctr_button[]={
    {5,235,75, 215, Test_Adc, -1, 1, "Trigger"},
    {5,210,75, 190, trigger_config, -1, 0, "TriAuto"},
    {85,210,155, 190, exit_ui, -1, 0, "Exit"},
    {85,235,155, 215, adc_less_delay, -1, 0, "Faster"},
    {165,235,235, 215, adc_more_delay, -1, 0, "Slower"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};
typedef struct ui_info{
    p_func init;
    button_t* button_info;
    int ui_index;
} ui_t;

button_t main_menu_button[]={
    {130,110,200, 135, NULL, -1, UI_ADC, "ADC"},
    {130,150,200, 175, raw_reboot, -1, 0, "Reboot"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};

ui_t main_menu_ui={
    NULL,
    main_menu_button,
    UI_MAIN_MENU,
};

ui_t adc_ui={
    adc_ui_init,
    adc_ctr_button,
    UI_ADC,
};

void adc_ui_init()
{
    uint back_color = 0x0000;
    uint front_color = 0xffff;
    uint i;
	Lcd_ClearScr(back_color);	//fill all screen with some color
#ifdef WAVE_DISP_VERTICAL
    for(i=0;i<5;i++){
        draw_line(64*i, 0, 64*i, 240, front_color);
    }
    for(i=0;i<4;i++){
        draw_line(0, 60*i, 256, 60*i, front_color);
    }
#else
    for(i=0;i<5;i++){
        draw_line(0, 60*i, 320, 60*i, front_color);
    }
    for(i=0;i<4;i++){
        draw_line(80*i, 0, 80*i, 240, front_color);
    }
#endif
    //total_adc_time_index = 0;
    lcd_printf(10,10,"Set:%ums    ", total_adc_time_list[total_adc_time_index]);
    if(autotrigger == -1){
        lcd_printf(10,30,"AutoTriOff");
    }
    else{
        lcd_printf(10,30,"AutoTriOn ");
    }
}

#define MIN(x,y) (x<y?x:y)
ui_t*current_ui;
void ui_init()
{
    //lprintf("curr ui %x\n", current_ui);
    button_t* p_bt = current_ui->button_info;
    if(current_ui->init){
        current_ui->init();
    }
    else{
        Lcd_ClearScr(0);	//fill all screen with some color
    }
    //lprintf("but x1 %x\n", p_bt->x1);
    while(p_bt->x1 >=0){
        draw_sq(p_bt->x1, p_bt->y1, p_bt->x2, p_bt->y2, 0xffff);
        if(p_bt->text){
            int lx = MIN(p_bt->x1, p_bt->x2);
            int ly = MIN(p_bt->y1, p_bt->y2);
            lcd_printf(lx+5,ly+5,p_bt->text);
        }
        p_bt++;
    }
}

#define IN_RANGE(x, x1, x2) (((x1<x2)&&(x1<x)&&(x<x2)) ||\
    ((x2<x1)&&(x2<x)&&(x<x1)))

void ui_running()
{
    uint x, y;
    button_t* p_bt = current_ui->button_info;
    if(screen_touched()){
        x = transfer_to_xy_ord(x_ts_adc_data, 320);
        y = transfer_to_xy_ord(y_ts_adc_data, 240);
        y = 240 - y;
        lprintf("x %u y %u\n", x,y);
        lprintf("%u %u %u %u\n", p_bt->x1, p_bt->y1, p_bt->x2, p_bt->y2);
        while(p_bt->x1 >=0){
            if(IN_RANGE(x, p_bt->x1, p_bt->x2) &&
                    IN_RANGE(y, p_bt->y1, p_bt->y2)){
                draw_sq(p_bt->x1, p_bt->y1, p_bt->x2, p_bt->y2, 0x0f0f);
                lprintf("in button\n");
                udelay(100*1000);
                draw_sq(p_bt->x1, p_bt->y1, p_bt->x2, p_bt->y2, 0xffff);
                if(p_bt->need_re_init_ui){
                    ui_init();
                }
                if(p_bt->click_func){
                    p_bt->click_func();
                }
            }
            p_bt++;
        }
        clear_touched();
    }
}

void cancel_delayed_work(int index)
{
    delayed_works[index].function = NULL;
}

int set_delayed_work(uint tct_10ms, func_p f, void*pa, int repeat)
{
    for(int i = 0; i<NUMBER_OF_DELAYED_WORKS; i++){
        if(delayed_works[i].function == NULL){
            delayed_works[i].function = f;
            delayed_works[i].ct_10ms = tct_10ms + timer4_click;
            delayed_works[i].delay_time_10ms = tct_10ms;
            delayed_works[i].para = pa;
            delayed_works[i].repeat = repeat;
            return i;
        }
    }
    return -1;
}

void task_misc()
{
    /*
    if(!stop_feed_wtd){
        feed_watch_dog();
    }
    */
    for(int i = 0; i<NUMBER_OF_DELAYED_WORKS; i++){
        if(delayed_works[i].function != NULL){
            if(delayed_works[i].ct_10ms > 0 && delayed_works[i].ct_10ms < timer4_click){
                delayed_works[i].function(delayed_works[i].para);
                if(delayed_works[i].repeat == 0){
                    delayed_works[i].function = NULL;
                }
                else{
                    delayed_works[i].ct_10ms = timer4_click + delayed_works[i].delay_time_10ms;
                }
            }
        }
    }
}

void run_touch_screen_app()
{
    int x, y, lastx = -1, lasty;
    AdcTS_init();

    clear_touched();
	Uart_Printf("\nStylus Down, please...... \n");
    lcd_printf(10,10,"Please touch screen again!");
    current_ui = & main_menu_ui;
    ui_init();
    while(1){
        ui_running();
        task_misc();
        if(ui_exit == 1){
            break;
        }
        /*no used code
        if(autotrigger != -1){
            if(timer4_click > autotriggertime){
                ui_init();
                Test_Adc();
                autotriggertime = timer4_click + 500;
            }
        }
        */
    }
}

int main(void)
{
    int ret;
    clk_init();
    timer_init();
    AdcTS_init();
    random_init();
    cs8900_init(cs8900_mac);
    some_init();
#if 0
    int a = 10;
    char * strprint="helloworkd";
    LCD_BUFER = (volatile unsigned short*)VIDEO_FB_ADRS;
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
    print_message();
    timer4_click = 0;
    enable_arm_interrupt();
    ret=enter_confirm();
    if(ret == 1)
    {
        run_clean_os();
    }
    else if(ret == 2)
    {
        run_touch_screen_app();
    }
    disable_arm_interrupt();
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
	
    rGPGUP = rGPGUP&(~(1<<4))|(1<<4);
    rGPGCON = rGPGCON&(~(3<<8))|(3<<8);
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


/**************************************************************
320¡Á240 16Bpp TFT LCDÒÆ¶¯¹Û²ì´°¿Ú
**************************************************************/
static void Lcd_MoveViewPort(int vx,int vy)
{
    U32 addr;

    disable_arm_interrupt();
	#if (LCD_XSIZE_TFT_320240<32)
    	    while((rLCDCON1>>18)<=1); // if x<32
	#else	
    	    while((rLCDCON1>>18)==0); // if x>32
	#endif
    
    addr=(U32)LCD_BUFER+(vx*2)+vy*(SCR_XSIZE_TFT_320240*2);
	rLCDSADDR1= ( (addr>>22)<<21 ) | M5D(addr>>1 );
	rLCDSADDR2= M5D(((addr+(SCR_XSIZE_TFT_320240*LCD_YSIZE_TFT_320240*2))>>1));
    enable_arm_interrupt();
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
    //lprintf("%u %u %u %u %u\n", x1, y1, x2 ,y2, color);
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

void draw_line(int x1, int y1, int x2, int y2, int color)
{
    float lv;
    int d, x, y;
    //lprintf("dl %u %u %u %u\n",x1,y1,x2,y2);
    PutPixel(x1,y1,color);
    if(diff(x1,x2)>diff(y1,y2)){
        d=(x2-x1)/diff(x1,x2);
        lv = (float)(y2-y1)/(x2-x1);
        x=x1;
        do{
            x += d;
            y = y1+lv*(float)(x-x1);
            //lprintf("1 %u %u\n",x,y);
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
            //lprintf("2 %u %u\n",x,y);
            PutPixel(x,y,color);
        }while(y!=y2);
    }
    PutPixel(x2,y2,color);
}

/*
 * uboot register value of LCD control
4d000000: 01700779 033bc14f 00a13f00 0000002b    y.p.O.;..?..+...
4d000010: 0001cb09 19800000 00012c00 00000140    .........,..@...
*/
static int video_init (void);
void Lcd_Tft_320X240_Init_from_reset( void )
{
    lprintf("Lcd_Tft_320X240_Init enter\r\n");
    Lcd_Port_Init();

    Lcd_EnvidOnOff(0);
    //Lcd_Init();
    video_init();
	rLCDSADDR1=(((U32)LCD_BUFER>>22)<<21)|M5D((U32)LCD_BUFER>>1);
    Lcd_EnvidOnOff(1);		//turn on vedio
	Lcd_ClearScr(0xf00f);		//fill all screen with some color

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


/**************************video**********************************************/
#include "video_font.h"
#include "video_fb.h"
void clear_screen();
typedef uint32_t u32;
typedef uint8_t u8;
static u32 eorx, fgx, bgx;  /* color pats */
static u8* video_fb_address;
#define VIDEO_DATA_FORMAT GDF_16BIT_565RGB
#define VIDEO_PIXEL_SIZE 2	
#define VIDEO_LINE_LEN		(VIDEO_COLS*VIDEO_PIXEL_SIZE)
#define VIDEO_COLS 320	
#define VIDEO_ROWS 240
/* Macros */
//#define VIDEO_FB_LITTLE_ENDIAN
#define VIDEO_FB_16BPP_WORD_SWAP
#ifdef	VIDEO_FB_LITTLE_ENDIAN
#define SWAP16(x)	 ((((x) & 0x00ff) << 8) | ( (x) >> 8))
#define SWAP32(x)	 ((((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8)|\
			  (((x) & 0x00ff0000) >>  8) | (((x) & 0xff000000) >> 24) )
#define SHORTSWAP32(x)	 ((((x) & 0x000000ff) <<  8) | (((x) & 0x0000ff00) >> 8)|\
			  (((x) & 0x00ff0000) <<  8) | (((x) & 0xff000000) >> 8) )
#else
#define SWAP16(x)	 (x)
#define SWAP32(x)	 (x)
#if defined(VIDEO_FB_16BPP_WORD_SWAP)
#define SHORTSWAP32(x)	 ( ((x) >> 16) | ((x) << 16) )
#else
#define SHORTSWAP32(x)	 (x)
#endif
#endif

static const int video_font_draw_table8[] = {
	    0x00000000, 0x000000ff, 0x0000ff00, 0x0000ffff,
	    0x00ff0000, 0x00ff00ff, 0x00ffff00, 0x00ffffff,
	    0xff000000, 0xff0000ff, 0xff00ff00, 0xff00ffff,
	    0xffff0000, 0xffff00ff, 0xffffff00, 0xffffffff };

static const int video_font_draw_table15[] = {
	    0x00000000, 0x00007fff, 0x7fff0000, 0x7fff7fff };

static const int video_font_draw_table16[] = {
	    0x00000000, 0x0000ffff, 0xffff0000, 0xffffffff };

static const int video_font_draw_table24[16][3] = {
	    { 0x00000000, 0x00000000, 0x00000000 },
	    { 0x00000000, 0x00000000, 0x00ffffff },
	    { 0x00000000, 0x0000ffff, 0xff000000 },
	    { 0x00000000, 0x0000ffff, 0xffffffff },
	    { 0x000000ff, 0xffff0000, 0x00000000 },
	    { 0x000000ff, 0xffff0000, 0x00ffffff },
	    { 0x000000ff, 0xffffffff, 0xff000000 },
	    { 0x000000ff, 0xffffffff, 0xffffffff },
	    { 0xffffff00, 0x00000000, 0x00000000 },
	    { 0xffffff00, 0x00000000, 0x00ffffff },
	    { 0xffffff00, 0x0000ffff, 0xff000000 },
	    { 0xffffff00, 0x0000ffff, 0xffffffff },
	    { 0xffffffff, 0xffff0000, 0x00000000 },
	    { 0xffffffff, 0xffff0000, 0x00ffffff },
	    { 0xffffffff, 0xffffffff, 0xff000000 },
	    { 0xffffffff, 0xffffffff, 0xffffffff } };

static const int video_font_draw_table32[16][4] = {
	    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
	    { 0x00000000, 0x00000000, 0x00000000, 0x00ffffff },
	    { 0x00000000, 0x00000000, 0x00ffffff, 0x00000000 },
	    { 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff },
	    { 0x00000000, 0x00ffffff, 0x00000000, 0x00000000 },
	    { 0x00000000, 0x00ffffff, 0x00000000, 0x00ffffff },
	    { 0x00000000, 0x00ffffff, 0x00ffffff, 0x00000000 },
	    { 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff },
	    { 0x00ffffff, 0x00000000, 0x00000000, 0x00000000 },
	    { 0x00ffffff, 0x00000000, 0x00000000, 0x00ffffff },
	    { 0x00ffffff, 0x00000000, 0x00ffffff, 0x00000000 },
	    { 0x00ffffff, 0x00000000, 0x00ffffff, 0x00ffffff },
	    { 0x00ffffff, 0x00ffffff, 0x00000000, 0x00000000 },
	    { 0x00ffffff, 0x00ffffff, 0x00000000, 0x00ffffff },
	    { 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00000000 },
	    { 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff } };

void set_draw_color(u32 fgcol, u32 bgcol)
{
	unsigned char color8;

	/* Init drawing pats */
	switch (VIDEO_DATA_FORMAT) {
	case GDF__8BIT_332RGB:
		color8 = ((fgcol & 0xe0) |
			  ((fgcol >> 3) & 0x1c) | fgcol >> 6);
		fgx = (color8 << 24) | (color8 << 16) | (color8 << 8) | color8;
		color8 = ((bgcol & 0xe0) |
			  ((bgcol >> 3) & 0x1c) | bgcol >> 6);
		bgx = (color8 << 24) | (color8 << 16) | (color8 << 8) | color8;
		break;
	case GDF_15BIT_555RGB:
		fgx = (((fgcol >> 3) << 26) |
		       ((fgcol >> 3) << 21) | ((fgcol >> 3) << 16) |
		       ((fgcol >> 3) << 10) | ((fgcol >> 3) << 5) |
		       (fgcol >> 3));
		bgx = (((bgcol >> 3) << 26) |
		       ((bgcol >> 3) << 21) | ((bgcol >> 3) << 16) |
		       ((bgcol >> 3) << 10) | ((bgcol >> 3) << 5) |
		       (bgcol >> 3));
		break;
	case GDF_16BIT_565RGB:
		fgx = (((fgcol >> 3) << 27) |
		       ((fgcol >> 2) << 21) | ((fgcol >> 3) << 16) |
		       ((fgcol >> 3) << 11) | ((fgcol >> 2) << 5) |
		       (fgcol >> 3));
		bgx = (((bgcol >> 3) << 27) |
		       ((bgcol >> 2) << 21) | ((bgcol >> 3) << 16) |
		       ((bgcol >> 3) << 11) | ((bgcol >> 2) << 5) |
		       (bgcol >> 3));
		break;
	case GDF_32BIT_X888RGB:
		fgx = (fgcol << 16) | (fgcol << 8) | fgcol;
		bgx = (bgcol << 16) | (bgcol << 8) | bgcol;
		break;
	case GDF_24BIT_888RGB:
		fgx = (fgcol << 24) | (fgcol << 16) |
			(fgcol << 8) | fgcol;
		bgx = (bgcol << 24) | (bgcol << 16) |
			(bgcol << 8) | bgcol;
		break;
	}
	eorx = fgx ^ bgx;

}
static int video_init (void)
{
	/*init framebuffer*/
#if 1
	*(volatile unsigned int *)0x4d000060 = 0xf82;
	*(volatile unsigned int *)0x4d00001c = 0x140;
	*(volatile unsigned int *)0x4d000018 = 0x12c00;
	//*(volatile unsigned int *)0x4d000014 = 0x19800000;
	*(volatile unsigned int *)0x4d000010 = 0x14b09;
	*(volatile unsigned int *)0x4d00000c = 0x2b;
	*(volatile unsigned int *)0x4d000008 = 0xa13f00;
	*(volatile unsigned int *)0x4d000004 = 0x33bc14f;
	*(volatile unsigned int *)0x4d000000 = 0x3180778;
	//*(volatile unsigned int *)0x4d000000 = 0x3180779;
#endif

	video_fb_address = (void *) VIDEO_FB_ADRS;

	set_draw_color(CONSOLE_FG_COL, CONSOLE_BG_COL);  
	clear_screen();

	return 0;
}


/*****************************************************************************/

static void video_drawchars (int xx, int yy, char *s, int count)
{
	u8 *cdat, *dest, *dest0;
	int rows, offset, c;

	offset = yy * VIDEO_LINE_LEN + xx * VIDEO_PIXEL_SIZE;
	dest0 = video_fb_address + offset;

	switch (VIDEO_DATA_FORMAT) {
	case GDF__8BIT_INDEX:
	case GDF__8BIT_332RGB:
		while (count--) {
			c = *s;
			cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
			for (rows = VIDEO_FONT_HEIGHT, dest = dest0;
			     rows--;
			     dest += VIDEO_LINE_LEN) {
				u8 bits = *cdat++;

				((u32 *) dest)[0] = (video_font_draw_table8[bits >> 4] & eorx) ^ bgx;
				((u32 *) dest)[1] = (video_font_draw_table8[bits & 15] & eorx) ^ bgx;
			}
			dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
			s++;
		}
		break;

	case GDF_15BIT_555RGB:
		while (count--) {
			c = *s;
			cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
			for (rows = VIDEO_FONT_HEIGHT, dest = dest0;
			     rows--;
			     dest += VIDEO_LINE_LEN) {
				u8 bits = *cdat++;

				((u32 *) dest)[0] = SHORTSWAP32 ((video_font_draw_table15 [bits >> 6] & eorx) ^ bgx);
				((u32 *) dest)[1] = SHORTSWAP32 ((video_font_draw_table15 [bits >> 4 & 3] & eorx) ^ bgx);
				((u32 *) dest)[2] = SHORTSWAP32 ((video_font_draw_table15 [bits >> 2 & 3] & eorx) ^ bgx);
				((u32 *) dest)[3] = SHORTSWAP32 ((video_font_draw_table15 [bits & 3] & eorx) ^ bgx);
			}
			dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
			s++;
		}
		break;

	case GDF_16BIT_565RGB:
		while (count--) {
			c = *s;
			cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
			for (rows = VIDEO_FONT_HEIGHT, dest = dest0;
			     rows--;
			     dest += VIDEO_LINE_LEN) {
				u8 bits = *cdat++;

				((u32 *) dest)[0] = SHORTSWAP32 ((video_font_draw_table16 [bits >> 6] & eorx) ^ bgx);
				((u32 *) dest)[1] = SHORTSWAP32 ((video_font_draw_table16 [bits >> 4 & 3] & eorx) ^ bgx);
				((u32 *) dest)[2] = SHORTSWAP32 ((video_font_draw_table16 [bits >> 2 & 3] & eorx) ^ bgx);
				((u32 *) dest)[3] = SHORTSWAP32 ((video_font_draw_table16 [bits & 3] & eorx) ^ bgx);
			}
			dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
			s++;
		}
		break;

	case GDF_32BIT_X888RGB:
		while (count--) {
			c = *s;
			cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
			for (rows = VIDEO_FONT_HEIGHT, dest = dest0;
			     rows--;
			     dest += VIDEO_LINE_LEN) {
				u8 bits = *cdat++;

				((u32 *) dest)[0] = SWAP32 ((video_font_draw_table32 [bits >> 4][0] & eorx) ^ bgx);
				((u32 *) dest)[1] = SWAP32 ((video_font_draw_table32 [bits >> 4][1] & eorx) ^ bgx);
				((u32 *) dest)[2] = SWAP32 ((video_font_draw_table32 [bits >> 4][2] & eorx) ^ bgx);
				((u32 *) dest)[3] = SWAP32 ((video_font_draw_table32 [bits >> 4][3] & eorx) ^ bgx);
				((u32 *) dest)[4] = SWAP32 ((video_font_draw_table32 [bits & 15][0] & eorx) ^ bgx);
				((u32 *) dest)[5] = SWAP32 ((video_font_draw_table32 [bits & 15][1] & eorx) ^ bgx);
				((u32 *) dest)[6] = SWAP32 ((video_font_draw_table32 [bits & 15][2] & eorx) ^ bgx);
				((u32 *) dest)[7] = SWAP32 ((video_font_draw_table32 [bits & 15][3] & eorx) ^ bgx);
			}
			dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
			s++;
		}
		break;

	case GDF_24BIT_888RGB:
		while (count--) {
			c = *s;
			cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
			for (rows = VIDEO_FONT_HEIGHT, dest = dest0;
			     rows--;
			     dest += VIDEO_LINE_LEN) {
				u8 bits = *cdat++;

				((u32 *) dest)[0] = (video_font_draw_table24[bits >> 4][0] & eorx) ^ bgx;
				((u32 *) dest)[1] = (video_font_draw_table24[bits >> 4][1] & eorx) ^ bgx;
				((u32 *) dest)[2] = (video_font_draw_table24[bits >> 4][2] & eorx) ^ bgx;
				((u32 *) dest)[3] = (video_font_draw_table24[bits & 15][0] & eorx) ^ bgx;
				((u32 *) dest)[4] = (video_font_draw_table24[bits & 15][1] & eorx) ^ bgx;
				((u32 *) dest)[5] = (video_font_draw_table24[bits & 15][2] & eorx) ^ bgx;
			}
			dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
			s++;
		}
		break;
	}
}

/*****************************************************************************/

void video_drawstring (int xx, int yy, char *s)
{
	//printf("lcd printing:%s\n",s);
	//getc();
	video_drawchars (xx, yy, s, strlen ((char *)s));
}

void clear_screen()
{
	u32 *pp;
	for(pp = (u32*)0x33000000; (u32)pp < 0x33025800;pp++)
		*pp = bgx;

}
#if 0
void lcd_printf(int x, int y, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	vslprintf(lprintf_buf, fmt, args);
	va_end(args);

	/* Print the string */
	video_drawstring (x, y, lprintf_buf);
}
#endif

void timer4_isr()
{
    timer4_click++;
}

u32 saveAdcdly;
void adc_isr()
{
    //CDB;
    if(rSUBSRCPND & BIT_SUB_TC){
        if(rADCDAT0&0x8000)
        {
            //Uart_Printf("Stylus Up!\n");
            rADCTSC=0xd3;//wait pen down
            touch_up = 1;
        }
        else{
            //Uart_Printf("\nStylus Down!\n");

            //pen down, prepare ADC
            rADCTSC=(1<<3)|(1<<2);
            saveAdcdly = rADCDLY;
            rADCDLY=40000;

            //start ADC
            rADCCON|=0x1;
        }
        rSUBSRCPND = BIT_SUB_TC;
    }
    else if(rSUBSRCPND & BIT_SUB_ADC){
        //ADC finish
		if(rADCCON & 0x8000)
        {
            if(rADCTSC & (1<<7)){//TS 0x73: Normal 0xD3
                //normal ADC
                normal_adc_data=(rADCDAT0&0x3ff);
            }
            else{
                x_ts_adc_data=(rADCDAT0&0x3ff);
                y_ts_adc_data=(rADCDAT1&0x3ff);
                Uart_Printf("\nXP=%x, YP=%x\n", x_ts_adc_data, y_ts_adc_data);
                rADCTSC=0x1d3;//wait pen up
                rADCDLY = saveAdcdly;
            }
        }
        else{
            lprintf("Error! No adc finished!\n");
        }
        rSUBSRCPND = BIT_SUB_ADC;
    }
#if 0
    lprintf("adc reg dump:%X\n",  rADCCON  );//ADC control
    lprintf("adc reg dump:%X\n",  rADCTSC  ); //ADC touch screen control
    lprintf("adc reg dump:%X\n",  rADCDLY  ); //ADC start or Interval Delay
    lprintf("adc reg dump:%X\n",  rADCDAT0 ); //ADC conversion data 0
    lprintf("adc reg dump:%X\n",  rADCDAT1 ); //ADC conversion data 1                   
#endif
}

void interrutp_init()
{
    for(int i = 0; i < INTNUM_S3C2440;i++){
        isr_list[i] = NULL;
    }
    isr_list[14] = timer4_isr;
    isr_list[31] = adc_isr;
    rINTMSK &= ~BIT_TIMER4;
    rINTMSK &= ~BIT_ADC;
    rINTSUBMSK &= ~BIT_SUB_TC;
    rINTSUBMSK &= ~BIT_SUB_ADC;
}

void do_irq ()
{
    uint flag = 1;
    for(int i = 0; i < INTNUM_S3C2440;i++){
        if(flag & rINTPND){
            if(isr_list[i] != NULL){
                isr_list[i]();
            }
            else{
                lprint("Warning! No ISR for %x\n", flag);
                lprintf ("irq: SRCPND %x \n", rSRCPND);
                lprintf ("irq: INTPND %x \n", rINTPND);
                lprintf ("irq: SUBSRCPND %x \n", rSUBSRCPND);
            }
        }
        flag <<= 1;
    }
	rSRCPND = rINTPND;
	rINTPND = rINTPND;
}

//according s3c2440 datasheet page 7-21 table, 12M fin, 405.00M out
#define M_MDIV	0x7f
#define M_PDIV	0x2
#define M_SDIV	0x1
#define U_M_MDIV	0x38
#define U_M_PDIV	0x2
#define U_M_SDIV	0x2
static inline void asm_delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

void clk_init()
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	clk_power->LOCKTIME = 0xFFFFFF;

	/* configure MPLL */
	clk_power->MPLLCON = ((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV);

	/* some delay between MPLL and UPLL */
	asm_delay (4000);
    lprintf("w mpll %x\r\n", ((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV));

	/* configure UPLL */
	clk_power->UPLLCON = ((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV);

	/* some delay between MPLL and UPLL */
	asm_delay (8000);
	clk_power->CLKDIVN = 5;//usb 48M, 4:2:1
}
