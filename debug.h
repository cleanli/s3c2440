#ifndef _DEBUG_H
#define _DEBUG_H

#include"type.h"
//#define USE_LIB_VSPRINTF

void delay_us(uint d);
void random_init();
unsigned short random_u16();
uint32_t random_st();
void putchars(const char *pt);
unsigned char s3c2440_serial_recv_byte();
void s3c2440_serial_send_byte(unsigned char c);
uint32_t s3c2440_is_serial_recv();
extern char lprintf_buf[256];

#ifdef USE_LIB_VSPRINTF
#define vslprintf vsprintf
#define lprintf(fmt,arg...) \
    { \
		vsprintf(lprintf_buf,fmt,##arg); \
		putchars(lprintf_buf); \
    }
#define lcd_printf(x, y, fmt,arg...) \
    { \
		vsprintf(lprintf_buf,fmt,##arg); \
		video_drawstring(x,y, lprintf_buf); \
    }
#else
void vslprintf(char*s_buf, const char *fmt, ...);
#define lprintf(fmt,arg...) \
    { \
		vslprintf(lprintf_buf,fmt,##arg); \
		putchars(lprintf_buf); \
    }
#define lcd_printf(x, y, fmt,arg...) \
    { \
		vslprintf(lprintf_buf,fmt,##arg); \
		video_drawstring(x,y, lprintf_buf); \
    }
#endif

#define CDB lprintf("From debug: %s %s %u \n\r", __FILE__, __func__, __LINE__);

#endif
