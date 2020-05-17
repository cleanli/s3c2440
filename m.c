#include <stdarg.h>

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

//If you don't use vsprintf(), the code size is reduced very much.
typedef int *__va_list[1];
int vsprintf(char * /*s*/, const char * /*format*/, __va_list /*arg*/);
void delay(U32 tt);
void Lcd_Tft_320X240_Init( void );

static int whichUart=0;
char getkey(void)
{
    if(whichUart==0)
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
            delay(1);                 //because the slow response of hyper_terminal 
            WrUTXH0('\r');
        }
        while(!(rUTRSTAT0 & 0x2));   //Wait until THR is empty.
        delay(1);
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

void main(void)
{
    rGPFCON = 0x55aa;
    Lcd_Tft_320X240_Init();
    while(1){
        if(rGPFDAT & 0x10)
            rGPFDAT = 0;
        else
            rGPFDAT = 0xff;
        delay(1);
    }
    return;
}
void delay(U32 tt)
{
    U32 i;
    for(;tt>0;tt--)
    {
        for(i=0;i<10000;i++){}
    }
}

volatile unsigned short * LCD_BUFER = (volatile unsigned short *)0x31010000;
#define short_lcd_buffer(x,y) (*((volatile unsigned short *)(LCD_BUFER+SCR_XSIZE_TFT_320240*y+x)))

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
	if ( (x < SCR_XSIZE_TFT_320240) && (y < SCR_YSIZE_TFT_320240) )
	short_lcd_buffer(x,y) = c;
}

/**************************************************************
320¡Á240 16Bpp TFT LCDÈ«ÆÁÌî³äÌØ¶¨ÑÕÉ«µ¥Ôª»òÇåÆÁ
**************************************************************/
static void Lcd_ClearScr(U16 c)
{
	unsigned int x,y ;
		
    for( y = 0 ; y < SCR_YSIZE_TFT_320240 ; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE_TFT_320240 ; x++ )
    	{
			short_lcd_buffer(x,y) = c;
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
    lprintf("Lcd_Tft_320X240_Init enter\r\n");
   	Lcd_Port_Init();

    Lcd_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio

	Lcd_ClearScr(0xffff);		//fill all screen with some color

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
    lprintf("Lcd_Tft_320X240_Init quit\r\n");
}


