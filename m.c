#include <stdarg.h>
#include <math.h>

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

#define whichUart   (*(volatile unsigned char *)0x34fffff0)
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
    if(whichUart==0)
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

void putchars(char *pt)
{
    while(*pt)
        putch(*pt++);
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
    putchars(fmt);
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

void Test_AdcTs(void)
{
   
    rADCDLY=50000;
    rADCCON=(1<<14)+(ADCPRS<<6);

    Uart_Printf("[ADC touch screen test.]\n");

    rADCTSC=0xd3;

//    pISR_ADC = (int)AdcTsAuto;
	rINTMSK|=BIT_ADC;
	rINTSUBMSK|=(BIT_SUB_TC);

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
int main(void)
{
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
    int x,y;
    lprintf("Lcd_Tft_320X240_Init enter\r\n");
    //Lcd_Port_Init();

    Lcd_EnvidOnOff(0);
	rLCDSADDR1=(((U32)LCD_BUFER>>22)<<21)|M5D((U32)LCD_BUFER>>1);
    //Lcd_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio

	Lcd_ClearScr(0xffff);		//fill all screen with some color
	Lcd_ClearScr(0x0000);		//fill all screen with some color
	Lcd_ClearScr(0xf81f);		//fill all screen with some color

r:
    for(x = 0; x<320; x++){
        y=x/2;
        PutPixel(x,y,0x7e0);
        draw_sq(x,y, x+20, y+20, 0x7e0);
        delay(10);
        if(getkey())return;
        draw_sq(x,y, x+20, y+20, 0xf81f);
    }
    for(x = 320; x>0; x--){
        y=sqrt(x);
        PutPixel(x,y,0x7e0);
        draw_sq(x,y, x+20, y+20, 0x7e0);
        delay(10);
        if(getkey())return;
        draw_sq(x,y, x+20, y+20, 0xf81f);
    }
    goto r;

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

#endif
