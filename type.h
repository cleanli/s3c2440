#ifndef _TYPE_H
#define _TYPE_H
#include<stdint.h>

typedef uint8_t __u8;
typedef uint8_t uint8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint32_t uint;
typedef uint32_t u32;
typedef uint32_t U32;
typedef uint32_t ulong;
typedef uint16_t u16;
typedef uint16_t U16;
typedef uint16_t ushort;
typedef uint8_t u8;
typedef uint8_t uchar;
typedef uint8_t u_char;
typedef void (*interrupt_func)(void);
typedef void (*p_func)(void);
typedef void (*func_p)(void*);

struct delay_work_info {
    func_p function;
    uint ct_10ms;
    void * para;
    int repeat;
    uint delay_time_10ms;
};

typedef struct bd_info {
    int			bi_baudrate;	/* serial console baudrate */
    unsigned long	bi_ip_addr;	/* IP Address */
    struct environment_s	       *bi_env;
    ulong	        bi_arch_number;	/* unique id for this board */
    ulong	        bi_boot_params;	/* where this board expects params */
    struct				/* RAM configuration */
    {
	ulong start;
	ulong size;
    }			
    bi_dram[4];
} bd_t;
#endif
