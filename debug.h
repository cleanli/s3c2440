#ifndef _DEBUG_H
#define _DEBUG_H

extern char lprintf_buf[256];
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

#define CDB lprint("From debug: %s %s %u \n\r", __FILE__, __func__, __LINE__);

#endif
