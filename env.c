#include "type.h"
#include "debug.h"
#include "common.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#define CONFIG_ENV_OFFSET 0x8000
#define CONFIG_ENV_OFFSET_REDUND 0xc000
#define CONFIG_ENV_SIZE 0x4000
unsigned long env_addr;	/* Address  of Environment struct */
unsigned long env_valid;	/* Checksum of Environment valid? */
u32 next_write_page1, next_write_page2;

# define ENV_HEADER_SIZE	(sizeof(uint32_t) + 1)
#define ENV_SIZE (CONFIG_ENV_SIZE - ENV_HEADER_SIZE)
typedef	struct environment_s {
	uint32_t	crc;		/* CRC32 over data bytes	*/
	unsigned char	flags;		/* active/obsolete flags	*/
	unsigned char	data[ENV_SIZE]; /* Environment data		*/
} env_t;

env_t inram_env1, inram_env2;
env_t *env_ptr = 0;
//uint8 env_ram1[CONFIG_ENV_SIZE];
//uint8 env_ram2[CONFIG_ENV_SIZE];
uint8* env_ram1 = (uint8*)&inram_env1;
uint8* env_ram2 = (uint8*)&inram_env2;

#define tole(x) (x)
//#define tole(x) cpu_to_le32(x)
const uint32_t crc_table[256] = {
tole(0x00000000L), tole(0x77073096L), tole(0xee0e612cL), tole(0x990951baL),
tole(0x076dc419L), tole(0x706af48fL), tole(0xe963a535L), tole(0x9e6495a3L),
tole(0x0edb8832L), tole(0x79dcb8a4L), tole(0xe0d5e91eL), tole(0x97d2d988L),
tole(0x09b64c2bL), tole(0x7eb17cbdL), tole(0xe7b82d07L), tole(0x90bf1d91L),
tole(0x1db71064L), tole(0x6ab020f2L), tole(0xf3b97148L), tole(0x84be41deL),
tole(0x1adad47dL), tole(0x6ddde4ebL), tole(0xf4d4b551L), tole(0x83d385c7L),
tole(0x136c9856L), tole(0x646ba8c0L), tole(0xfd62f97aL), tole(0x8a65c9ecL),
tole(0x14015c4fL), tole(0x63066cd9L), tole(0xfa0f3d63L), tole(0x8d080df5L),
tole(0x3b6e20c8L), tole(0x4c69105eL), tole(0xd56041e4L), tole(0xa2677172L),
tole(0x3c03e4d1L), tole(0x4b04d447L), tole(0xd20d85fdL), tole(0xa50ab56bL),
tole(0x35b5a8faL), tole(0x42b2986cL), tole(0xdbbbc9d6L), tole(0xacbcf940L),
tole(0x32d86ce3L), tole(0x45df5c75L), tole(0xdcd60dcfL), tole(0xabd13d59L),
tole(0x26d930acL), tole(0x51de003aL), tole(0xc8d75180L), tole(0xbfd06116L),
tole(0x21b4f4b5L), tole(0x56b3c423L), tole(0xcfba9599L), tole(0xb8bda50fL),
tole(0x2802b89eL), tole(0x5f058808L), tole(0xc60cd9b2L), tole(0xb10be924L),
tole(0x2f6f7c87L), tole(0x58684c11L), tole(0xc1611dabL), tole(0xb6662d3dL),
tole(0x76dc4190L), tole(0x01db7106L), tole(0x98d220bcL), tole(0xefd5102aL),
tole(0x71b18589L), tole(0x06b6b51fL), tole(0x9fbfe4a5L), tole(0xe8b8d433L),
tole(0x7807c9a2L), tole(0x0f00f934L), tole(0x9609a88eL), tole(0xe10e9818L),
tole(0x7f6a0dbbL), tole(0x086d3d2dL), tole(0x91646c97L), tole(0xe6635c01L),
tole(0x6b6b51f4L), tole(0x1c6c6162L), tole(0x856530d8L), tole(0xf262004eL),
tole(0x6c0695edL), tole(0x1b01a57bL), tole(0x8208f4c1L), tole(0xf50fc457L),
tole(0x65b0d9c6L), tole(0x12b7e950L), tole(0x8bbeb8eaL), tole(0xfcb9887cL),
tole(0x62dd1ddfL), tole(0x15da2d49L), tole(0x8cd37cf3L), tole(0xfbd44c65L),
tole(0x4db26158L), tole(0x3ab551ceL), tole(0xa3bc0074L), tole(0xd4bb30e2L),
tole(0x4adfa541L), tole(0x3dd895d7L), tole(0xa4d1c46dL), tole(0xd3d6f4fbL),
tole(0x4369e96aL), tole(0x346ed9fcL), tole(0xad678846L), tole(0xda60b8d0L),
tole(0x44042d73L), tole(0x33031de5L), tole(0xaa0a4c5fL), tole(0xdd0d7cc9L),
tole(0x5005713cL), tole(0x270241aaL), tole(0xbe0b1010L), tole(0xc90c2086L),
tole(0x5768b525L), tole(0x206f85b3L), tole(0xb966d409L), tole(0xce61e49fL),
tole(0x5edef90eL), tole(0x29d9c998L), tole(0xb0d09822L), tole(0xc7d7a8b4L),
tole(0x59b33d17L), tole(0x2eb40d81L), tole(0xb7bd5c3bL), tole(0xc0ba6cadL),
tole(0xedb88320L), tole(0x9abfb3b6L), tole(0x03b6e20cL), tole(0x74b1d29aL),
tole(0xead54739L), tole(0x9dd277afL), tole(0x04db2615L), tole(0x73dc1683L),
tole(0xe3630b12L), tole(0x94643b84L), tole(0x0d6d6a3eL), tole(0x7a6a5aa8L),
tole(0xe40ecf0bL), tole(0x9309ff9dL), tole(0x0a00ae27L), tole(0x7d079eb1L),
tole(0xf00f9344L), tole(0x8708a3d2L), tole(0x1e01f268L), tole(0x6906c2feL),
tole(0xf762575dL), tole(0x806567cbL), tole(0x196c3671L), tole(0x6e6b06e7L),
tole(0xfed41b76L), tole(0x89d32be0L), tole(0x10da7a5aL), tole(0x67dd4accL),
tole(0xf9b9df6fL), tole(0x8ebeeff9L), tole(0x17b7be43L), tole(0x60b08ed5L),
tole(0xd6d6a3e8L), tole(0xa1d1937eL), tole(0x38d8c2c4L), tole(0x4fdff252L),
tole(0xd1bb67f1L), tole(0xa6bc5767L), tole(0x3fb506ddL), tole(0x48b2364bL),
tole(0xd80d2bdaL), tole(0xaf0a1b4cL), tole(0x36034af6L), tole(0x41047a60L),
tole(0xdf60efc3L), tole(0xa867df55L), tole(0x316e8eefL), tole(0x4669be79L),
tole(0xcb61b38cL), tole(0xbc66831aL), tole(0x256fd2a0L), tole(0x5268e236L),
tole(0xcc0c7795L), tole(0xbb0b4703L), tole(0x220216b9L), tole(0x5505262fL),
tole(0xc5ba3bbeL), tole(0xb2bd0b28L), tole(0x2bb45a92L), tole(0x5cb36a04L),
tole(0xc2d7ffa7L), tole(0xb5d0cf31L), tole(0x2cd99e8bL), tole(0x5bdeae1dL),
tole(0x9b64c2b0L), tole(0xec63f226L), tole(0x756aa39cL), tole(0x026d930aL),
tole(0x9c0906a9L), tole(0xeb0e363fL), tole(0x72076785L), tole(0x05005713L),
tole(0x95bf4a82L), tole(0xe2b87a14L), tole(0x7bb12baeL), tole(0x0cb61b38L),
tole(0x92d28e9bL), tole(0xe5d5be0dL), tole(0x7cdcefb7L), tole(0x0bdbdf21L),
tole(0x86d3d2d4L), tole(0xf1d4e242L), tole(0x68ddb3f8L), tole(0x1fda836eL),
tole(0x81be16cdL), tole(0xf6b9265bL), tole(0x6fb077e1L), tole(0x18b74777L),
tole(0x88085ae6L), tole(0xff0f6a70L), tole(0x66063bcaL), tole(0x11010b5cL),
tole(0x8f659effL), tole(0xf862ae69L), tole(0x616bffd3L), tole(0x166ccf45L),
tole(0xa00ae278L), tole(0xd70dd2eeL), tole(0x4e048354L), tole(0x3903b3c2L),
tole(0xa7672661L), tole(0xd06016f7L), tole(0x4969474dL), tole(0x3e6e77dbL),
tole(0xaed16a4aL), tole(0xd9d65adcL), tole(0x40df0b66L), tole(0x37d83bf0L),
tole(0xa9bcae53L), tole(0xdebb9ec5L), tole(0x47b2cf7fL), tole(0x30b5ffe9L),
tole(0xbdbdf21cL), tole(0xcabac28aL), tole(0x53b39330L), tole(0x24b4a3a6L),
tole(0xbad03605L), tole(0xcdd70693L), tole(0x54de5729L), tole(0x23d967bfL),
tole(0xb3667a2eL), tole(0xc4614ab8L), tole(0x5d681b02L), tole(0x2a6f2b94L),
tole(0xb40bbe37L), tole(0xc30c8ea1L), tole(0x5a05df1bL), tole(0x2d02ef8dL)
};

#  define DO_CRC(x) crc = tab[(crc ^ (x)) & 255] ^ (crc >> 8)
/* ========================================================================= */

/* No ones complement version. JFFS2 (and other things ?)
 * don't use ones compliment in their CRC calculations.
 */
uint32_t crc32_no_comp(uint32_t crc, const uint8 *buf, u32 len)
{
    const uint32_t *tab = crc_table;
    const uint32_t *b =(const uint32_t *)buf;
    size_t rem_len;
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
      make_crc_table();
#endif
    crc = cpu_to_le32(crc);
    /* Align it */
    if (((long)b) & 3 && len) {
	 uint8_t *p = (uint8_t *)b;
	 do {
	      DO_CRC(*p++);
	 } while ((--len) && ((long)p)&3);
	 b = (uint32_t *)p;
    }

    rem_len = len & 3;
    len = len >> 2;
    for (--b; len; --len) {
	 /* load data 32 bits wide, xor data 32 bits wide. */
	 crc ^= *++b; /* use pre increment for speed */
	 DO_CRC(0);
	 DO_CRC(0);
	 DO_CRC(0);
	 DO_CRC(0);
    }
    len = rem_len;
    /* And the last few bytes */
    if (len) {
	 uint8_t *p = (uint8_t *)(b + 1) - 1;
	 do {
	      DO_CRC(*++p); /* use pre increment for speed */
	 } while (--len);
    }

    return le32_to_cpu(crc);
}
#undef DO_CRC

uint32_t crc32 (uint32_t crc, const uint8*p, u32 len)
{
     return crc32_no_comp(crc ^ 0xffffffffL, p, len) ^ 0xffffffffL;
}

static char default_environment[] = {
	"bootdelay=3\0"
	"baudrate=115200\0"
	"ipaddr=192.168.8.88\0"
	"serverip=192.168.8.66\0"
	"netmask=255.255.255.0\0"
	"loadaddr=31000000\0"
	"\0"		/* Termimate struct environment data with 2 NULs */
};

uchar env_get_char (int index)
{
	uchar c;

	c = ( *((uchar *)(env_addr + index)) );
    return c;
}

static size_t get_page(size_t n)
{
	return (n+511)/512;
}

u_char find_env(u_char*tmpchar, env_t *tmpenv)
{
	u_char * p = tmpchar + CONFIG_ENV_SIZE;
	unsigned int offset, len;
	
	while((unsigned int)(--p) > (unsigned int)tmpchar){
		if(*p != 0xff)
			break;	
	}
	offset = 512 * (*p);
	lprintf("find effective env at offset %x ", offset);
	//this indicate the block not init, need erase
	if(tmpchar + offset >= p)
		goto err;
	len = (unsigned int)(p - tmpchar) - offset;
	lprintf("len %x\n", len);
	memcpy(tmpenv, tmpchar+offset, len);
	memset((uint8*)tmpenv+len, 0, CONFIG_ENV_SIZE-len);
	return offset/512 + get_page(len+1);
err:
	lprintf("error in env\n");
	return CONFIG_ENV_SIZE/512;
}

static void use_default()
{
	lprintf("*** Warning - bad CRC or NAND, using default environment\n\n");
	if (sizeof(default_environment) > ENV_SIZE) {
		lprintf("*** Error - default environment is too large\n\n");
		return;
	}

	memset(env_ptr, 0, sizeof(env_t));
	memcpy(env_ptr->data, default_environment,
	       sizeof(default_environment));
	env_ptr->flags = 0xFF;
	env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);
	env_valid = 1;
}

void env_init()
{
	int crc1_ok = 0, crc2_ok = 0;
    nand_reset();
    nand_read_ll(env_ram1, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE);
    nand_read_ll(env_ram2,
            CONFIG_ENV_OFFSET_REDUND, CONFIG_ENV_SIZE);
	lprintf("\nCheck %x: ", CONFIG_ENV_OFFSET);
	next_write_page1 = find_env(env_ram1, &inram_env1);
	lprintf("\nCheck %x: ", CONFIG_ENV_OFFSET_REDUND);
	next_write_page2 = find_env(env_ram2, &inram_env2);

	crc1_ok = (crc32(0, inram_env1.data, ENV_SIZE) == inram_env1.crc);
	crc2_ok = (crc32(0, inram_env2.data, ENV_SIZE) == inram_env2.crc);

	if(!crc1_ok && !crc2_ok) {
		env_ptr = &inram_env1;
		use_default();
        env_addr = (ulong)&(env_ptr->data);
        return;
	} else if(crc1_ok && !crc2_ok)
		env_valid = 1;
	else if(!crc1_ok && crc2_ok)
		env_valid = 2;
	else {
		/* both ok - check serial */
		if(inram_env1.flags == 255 && inram_env2.flags == 0)
			env_valid = 2;
		else if(inram_env2.flags == 255 && inram_env1.flags == 0)
			env_valid = 1;
		else if(inram_env1.flags > inram_env2.flags)
			env_valid = 1;
		else if(inram_env2.flags > inram_env1.flags)
			env_valid = 2;
		else /* flags are equal - almost impossible */
			env_valid = 1;

	}

    lprintf("valid env index %u\n", env_valid);
	if(env_valid == 1) {
		env_ptr = &inram_env1;
	} else {
		env_ptr = &inram_env2;
	}
	env_addr = (ulong)&(env_ptr->data);
}

int ctrlc(void)
{
	if(getkey()==0x03)
        return 1;
    return 0;   
}

/************************************************************************
 * Match a name / name=value pair
 *
 * s1 is either a simple 'name', or a 'name=value' pair.
 * i2 is the environment index for a 'name2=value2' pair.
 * If the names match, return the index for the value2, else NULL.
 */

int envmatch (uchar *s1, int i2)
{

	while (*s1 == env_get_char(i2++))
		if (*s1++ == '=')
			return(i2);
	if (*s1 == '\0' && env_get_char(i2-1) == '=')
		return(i2);
	return(-1);
}

/************************************************************************
 * Command interface: print one or all environment variables
 */

/*
 * state 0: finish printing this string and return (matched!)
 * state 1: no matching to be done; print everything
 * state 2: continue searching for matched name
 */
int printenv(char *name, int state)
{
	int i, j;
	char c, buf[17];

	i = 0;
	buf[16] = '\0';

	while (state && env_get_char(i) != '\0') {
		if (state == 2 && envmatch((uchar *)name, i) >= 0)
			state = 0;

		j = 0;
		do {
			buf[j++] = c = env_get_char(i++);
			if (j == sizeof(buf) - 1) {
				if (state <= 1)
					lprintf(buf);
				j = 0;
			}
		} while (c != '\0');

		if (state <= 1) {
			if (j)
				lprintf(buf);
			putch('\n');
		}

		if (ctrlc())
			return -1;
	}

	if (state == 0)
		i = 0;
	return i;
}

uchar *env_get_addr (int index)
{
	if (env_valid) {
		return ( ((uchar *)(env_addr + index)) );
	} else {
		return (&default_environment[index]);
	}
}

/************************************************************************
 * Look up variable from environment,
 * return address of storage for that variable,
 * or NULL if not found
 */

char *getenv (char *name)
{
	int i, nxt;

	for (i=0; env_get_char(i) != '\0'; i=nxt+1) {
		int val;

		for (nxt=i; env_get_char(nxt) != '\0'; ++nxt) {
			if (nxt >= CONFIG_ENV_SIZE) {
				return (NULL);
			}
		}
		if ((val=envmatch((uchar *)name, i)) < 0)
			continue;
		return ((char *)env_get_addr(val));
	}

	return (NULL);
}
