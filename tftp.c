#include "cs8900.h"
#include "type.h"
#include "tcp.h"
#include "common.h"
#include "debug.h"
#include "net.h"
#include <string.h>

#define code
code const char tftp_req[]=
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x08,0x00,0x45,0x00,
0x00,0x32,0x00,0x00,0x40,0x00,0x40,0x11,  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x70,0x23,0x00,0x45,0x00,0x1e,  0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
#define MAX_PACKAGE 1518
static struct eth_device* local_eth = NULL;
static struct tftp_status
{
	uint filesize;
	unsigned short port;//local port
	unsigned short server_port;
	unsigned short block_n;
	unsigned short max_block;
	unsigned char *filename;
	unsigned char *membase;
	unsigned char running;
	unsigned char operation;
} t_s;
static unsigned char rsp_arp_buf[60];
static unsigned char s_buf[MAX_PACKAGE];
static unsigned char r_buf[MAX_PACKAGE];
static unsigned char server_mac[6];
extern uint server_ip;
extern uint local_ip;
static struct eth_h*sep = (struct eth_h*)s_buf, *rep = (struct eth_h*)r_buf;
static struct ip_udp_tftp* siutp;
static struct ip_udp_tftp* riutp;
static uint send_len;
static unsigned short ipid = 1;
static void tftp_run();

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

unsigned char _ctype[] = {
_C,_C,_C,_C,_C,_C,_C,_C,			/* 0-7 */
_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,		/* 8-15 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 16-23 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 24-31 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,			/* 32-39 */
_P,_P,_P,_P,_P,_P,_P,_P,			/* 40-47 */
_D,_D,_D,_D,_D,_D,_D,_D,			/* 48-55 */
_D,_D,_P,_P,_P,_P,_P,_P,			/* 56-63 */
_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,	/* 64-71 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 72-79 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 80-87 */
_U,_U,_U,_P,_P,_P,_P,_P,			/* 88-95 */
_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,	/* 96-103 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 104-111 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 112-119 */
_L,_L,_L,_P,_P,_P,_P,_C,			/* 120-127 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 128-143 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 144-159 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)
static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
}

static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)
#define __cpu_to_be32(x) ___swab32((x))
#define ___swab32(x) \
	((__u32)( \
		(((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
		(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
		(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
		(((__u32)(x) & (__u32)0xff000000UL) >> 24) ))
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

u32 string_to_ip(char *s)
{
	u32 addr;
	char *e;
	int i;

	if (s == NULL)
		return(0);

	for (addr=0, i=0; i<4; ++i) {
		ulong val = s ? simple_strtoul(s, &e, 10) : 0;
		addr <<= 8;
		addr |= (val & 0xFF);
		if (s) {
			s = (*e) ? e+1 : e;
		}
	}

	return __cpu_to_be32(addr);
}

int eth_register(struct eth_device*dev)
{
    char* tmpcp;
    if(dev){
        local_eth = dev;
    }
    //init ip from env
    if(tmpcp = getenv("serverip")){
        lprintf("serverip:%s %X\n", tmpcp, server_ip);
        server_ip=string_to_ip(tmpcp);
        lprintf("serverip:%s %X\n", tmpcp, server_ip);
    }
    if(tmpcp = getenv("ipaddr")){
        lprintf("ipaddr:%s %X\n", tmpcp, local_ip);
        local_ip=string_to_ip(tmpcp);
        lprintf("ipaddr:%s %X\n", tmpcp, local_ip);
    }
}

unsigned short for_check(unsigned char *crcdata,uint len)
{
        unsigned short *t,sum=0,new_sum;
        uint jw=0,i;

        t = (unsigned short *)crcdata;
    	for(i=0;i<len/2;i++){
                new_sum = *t + sum;
                if(new_sum<sum)jw++;
                sum=new_sum;
                t++;
        }
    	sum+=jw;
        sum=~sum;
        //*(unsigned int xdata*)crccode = sum;
        return sum;
}

uint lstrlen(unsigned char*s)
{
	uint l = 0;
	while(*s++)
		l++;
	return l;
}

uint lmemequ(unsigned char *a, unsigned char * b, uint len)
{
	while(len--)
		if(*a++ != *b++)
			return 0;
	return 1;
}

void lstrcpy(unsigned char *d, unsigned char*s)
{
	while(*d++ = *s++);
}

unsigned short change_end(unsigned short a)
{
	return ((0xff & a)<<8) + ((0xff00 & a)>>8);
}	

void tftp_put(unsigned char* name, uint sz, unsigned char *buf)
{
	/*init t_s*/
	t_s.block_n = 0;
	t_s.filename = name;
	t_s.membase = buf;
	t_s.operation = 0;//put file to server
	t_s.filesize = sz;
	t_s.max_block = (t_s.filesize>>9) + 1;
    compute_sha256((uint8*)buf, t_s.filesize);
	tftp_run();
}
	
void tftp_get(unsigned char* name, unsigned char *buf)
{
	/*init t_s*/
	t_s.filesize = 0;
	t_s.block_n = 1;
	t_s.filename = name;
	t_s.membase = buf;
	t_s.operation = 1;//get file from server
	tftp_run();
    if(t_s.filesize != 0){
        compute_sha256((uint8*)buf, t_s.filesize);
    }
}

static void setup_arp_req(unsigned char * buf)
{
	struct eth_h* tmp_ep = (struct eth_h*)buf;
	struct arp_p *arp_p_p = (struct arp_p *)tmp_ep->datas;
	/*clear buffer*/
	memset(buf, 0, 60);
	/*set 802.3 header*/
	memset(tmp_ep->dest_mac, 0xff, 6);
	memcpy(tmp_ep->src_mac, local_eth->enetaddr, 6);
	tmp_ep->protocol = 0x0608;
	/*setup arp package*/
	arp_p_p->hardware = 0x100;
	arp_p_p->protocol = 0x08;
    	arp_p_p->hardware_addr_len = 6;
    	arp_p_p->protocol_addr_len = 4;
    	arp_p_p->operation = 0x100;
	memcpy(arp_p_p->sender_mac, local_eth->enetaddr, 6);
	memset(arp_p_p->target_mac, 0x0, 6);
    	memcpy(arp_p_p->sender_ip, &local_ip, 4);
    	memcpy(arp_p_p->target_ip, &server_ip, 4);
	send_len = 60;
}
	
uint anlz_arq()
{
	struct arp_p *arp_p_p = (struct arp_p *)rep->datas;
	if(rep->protocol != 0x0608 || arp_p_p->operation != 0x0200 \
				|| !lmemequ(arp_p_p->sender_ip, (unsigned char*)&server_ip, 4)){
        lprintf("Not arp reply\n");
		return 0;//failed
    }
    lprintf("Get arp reply\n");
    memcpy(server_mac, arp_p_p->sender_mac, 6);
    for(int i=0;i<6;i++){
        lprintf("%b", server_mac[i]&0xff);
    }
    lprintf("\n");
	return 1;
}

uint recv_p()
{
	struct eth_h*tmp_ep = (struct eth_h*)rsp_arp_buf;
	struct arp_p *arp_p_p = (struct arp_p *)rep->datas, *rsp_arp = (struct arp_p *)tmp_ep->datas;
	uint len;

	len = local_eth->recv(local_eth, (unsigned short*)r_buf);
	if(!len)
		return 0;
	if(rep->protocol == 0x0608 && arp_p_p->operation == 0x0100){
		if( lmemequ(arp_p_p->target_ip, (unsigned char*)&local_ip, 4)
		    && lmemequ(arp_p_p->sender_ip, (unsigned char*)&server_ip, 4)){
			setup_arp_req(rsp_arp_buf);
			memcpy(tmp_ep->dest_mac, rep->src_mac, 6);
    			rsp_arp->operation = 0x200;
			memcpy(rsp_arp->target_mac, arp_p_p->sender_mac, 6);
    			memcpy(rsp_arp->target_ip, arp_p_p->sender_ip, 4);
			if(local_eth->send(local_eth, (unsigned short*)rsp_arp_buf, 60) != 60){
				lprintf("answer asp failed!\r\n");
            }
			else{
				lprintf("recv asp and answered\r\n");
            }
			//while(1);
		}
		return 0;
	}
	return len;
}
	
uint get_response(uint (*anlz)(), uint try)
{
    uint len, wait;

send:
    len = local_eth->send(local_eth, (unsigned short*)s_buf, send_len);
    if(len != send_len){
        lprintf("send packages error len %u send_len %u\r\n",
                len, send_len);
        return 0;
    }
try_recv:
    wait = 300;
    do{
        len = recv_p();
        //lprintf("recv Len %u\n", len);
        if(!len){
            delay_us(10000);
        }
        else{
            break;
        }
    }while(wait--);
    if(!len){
        if(try--){
            lprintf("no response, retrying\r\n");
            delay_us(1000000);
            goto send;
        }
        else{
            lprintf("give up.\r\n");
            return 0;
        }
    }
    if(!(*anlz)())
        goto try_recv;
    return 1;
}	

void setup_tftp_package()
{
	unsigned char * s;
	unsigned short local_port, udp_len, data_len;

	/*setup tftp req package*/
	memset(s_buf, 0, MAX_PACKAGE);
	memcpy(s_buf, tftp_req, 64);
	/*set 802.3 header*/
	memcpy(sep->dest_mac, server_mac, 6);
	memcpy(sep->src_mac, local_eth->enetaddr, 6);

    	memcpy(siutp->ip_header.sender_ip, &local_ip, 4);
    	memcpy(siutp->ip_header.target_ip, &server_ip, 4);
    	siutp->udp_header.check_sum = 0;
	siutp->udp_header.dest_port = change_end(t_s.server_port);
	if(t_s.running == 0){
    		local_port = random_u16();
   	 	while(local_port < 2000)
			local_port *= 3;
		t_s.port = local_port;
		siutp->udp_header.src_port = change_end(local_port);
    		udp_len = 17 + lstrlen(t_s.filename);
    		siutp->udp_header.length = change_end(udp_len);
		/*setup ip header*/
    		siutp->ip_header.id = change_end(ipid++);
    		siutp->ip_header.total_len = change_end(20+udp_len);
		if(t_s.operation){
    			siutp->tftp_packet.operation = change_end(1);
		}
		else{
    			siutp->tftp_packet.operation = change_end(2);
		}
		/*setup filename */
    		s = (char*)&siutp->tftp_packet.block_n;
    		s = (char*)&siutp->tftp_packet.block_n;
    		lstrcpy(s, t_s.filename);
		s += lstrlen(s) + 1;
	    	lstrcpy(s, "octet");
		send_len = 51 + lstrlen(t_s.filename);
		if(send_len < 60)
			send_len = 60;
	}
	else if(t_s.operation == 1){
		//setup ack package
		siutp->udp_header.src_port = change_end(t_s.port);
		/*setup ip header*/
		siutp->ip_header.total_len = change_end(32);
		siutp->ip_header.id = change_end(ipid++);
		/*udp & tftp*/
		siutp->udp_header.length = change_end(12);
		siutp->tftp_packet.operation = 0x400;
		siutp->tftp_packet.block_n = change_end(t_s.block_n);

		send_len = 60;
		
	}
	else if(t_s.operation == 0){
		data_len = (t_s.filesize > 512)?512:t_s.filesize;		
		memcpy(siutp->tftp_packet.tftp_data, t_s.membase, data_len);
		t_s.membase += data_len;
		t_s.filesize -= data_len;
		/*set 802.3 header*/
		memcpy(sep->dest_mac, server_mac, 6);
		memcpy(sep->src_mac, local_eth->enetaddr, 6);

		siutp->udp_header.src_port = change_end(t_s.port);
		/*setup ip header*/
		siutp->ip_header.total_len = change_end(32 + data_len);
		siutp->ip_header.id = change_end(ipid++);
		/*udp & tftp*/
		siutp->udp_header.length = change_end(data_len + 12);		
		siutp->tftp_packet.operation = 0x300;
		siutp->tftp_packet.block_n = change_end(t_s.block_n);

		send_len = 46 + data_len;
		if(send_len<60)
			send_len = 60;
		
	}
	siutp->ip_header.check_sum = 0;
	siutp->ip_header.check_sum = for_check(&siutp->ip_header.ipv_hdl, 20);
}
	
#define con_send(X) s3c2440_serial_send_byte(X)
uint anlz_tftp()
{
	unsigned short data_len;
	uint tmp;
	if(rep->protocol != 0x0008 || riutp->ip_header.protocol != 0x11 
				   || riutp->udp_header.dest_port != change_end(t_s.port))
		return 0;
	if(riutp->tftp_packet.operation == 0x500){
		t_s.running = 0;
		lprintf("Error message from server: code %x, '%s'\r\n", change_end(riutp->tftp_packet.block_n),riutp->tftp_packet.tftp_data); 
		return 1;
	}
	t_s.server_port = change_end(riutp->udp_header.src_port);
	if(t_s.operation){
		//tftp get case
		if(riutp->tftp_packet.operation != 0x300 
			|| change_end(riutp->tftp_packet.block_n) != t_s.block_n){
			lprintf("Unusual error!\r\n");
			return 0;
		}
		t_s.running = 1;	
		setup_tftp_package();
		data_len = change_end(riutp->udp_header.length) - 12;		
		con_send('.');
		memcpy(t_s.membase, riutp->tftp_packet.tftp_data, data_len);
		t_s.membase += 512;
		if(data_len != 512){
			t_s.running = 0;
        		local_eth->send(local_eth, (unsigned short*)s_buf, send_len);
			lprintf("\r\nfile size:0x%x(%d)\r\n", t_s.filesize = (t_s.block_n-1)*512 + data_len, t_s.filesize);
		}
		t_s.block_n++;
		return 1;
	}
	else{
		//tftp put case
		if(riutp->tftp_packet.operation != 0x400 
			|| change_end(riutp->tftp_packet.block_n) != t_s.block_n){
			lprintf("Unusual error!\r\n");
			return 0;
		}
		if(t_s.block_n == t_s.max_block){
			t_s.running = 0;
			lprintf("\r\n");
			return 1;
		}
		t_s.running = 1;
		t_s.block_n++;
		con_send('`');
		setup_tftp_package();
		return 1;
	}
	return 0;
}
	
static void tftp_run()
{
        sep = (struct eth_h*)s_buf; 
	rep = (struct eth_h*)r_buf;
        siutp = (struct ip_udp_tftp*) sep->datas;
        riutp = (struct ip_udp_tftp*) rep->datas;

	t_s.server_port = 69;
	t_s.port = 0;
	t_s.running = 0;
	if(!local_eth->dev_is_ready()){
        local_eth->init(local_eth, NULL);
        if(!local_eth->dev_is_ready()){
            lprintf("can't init dm9000, quit\r\n");
            local_eth->halt(local_eth);
            return;
        }
	}
	setup_arp_req(s_buf);	
	if(!get_response(anlz_arq, 3)){
		lprintf("server no response!\r\n");
        local_eth->halt(local_eth);
		return;
	}
	setup_tftp_package();
	if(!get_response(anlz_tftp,0)){
		lprintf("server no tftp response!\r\n");
        local_eth->halt(local_eth);
		return;
	}
	while(t_s.running){
		if(!get_response(anlz_tftp, 0)){
			lprintf("lost connection with server while transfering!\r\n");
            local_eth->halt(local_eth);
			return;
		}
	}
	local_eth->halt(local_eth);
	lprintf("tftp operation finished successfully!\r\n");
	return;
}
#if 0
void tftp_put(unsigned char* name)
{
	struct eth_h*ep = p_buf;
	struct arp_p *arp_p_p = (struct arp_p *)ep->datas;
	struct ip_udp_tftp* iutp = (struct ip_udp_tftp*) ep->datas;
	uint len, wait, len1;
	char *s;

lprintf("name is %s\n", name);
	if(!local_eth->dev_is_ready()){
		lprintf("cs8900 not ready!\r\n");
		return;
	}
	local_eth->init();
	/*clear buffer*/
	memset(p_buf, 0, MAX_PACKAGE);
	/*set 802.3 header*/
	memset(ep->dest_mac, 0xff, 6);
	memcpy(ep->src_mac, local_eth->enetaddr, 6);
	ep->protocol = 0x0608;
	/*setup arp package*/
	arp_p_p->hardware = 0x100;
	arp_p_p->protocol = 0x08;
    	arp_p_p->hardware_addr_len = 6;
    	arp_p_p->protocol_addr_len = 4;
    	arp_p_p->operation = 0x100;
	memcpy(arp_p_p->sender_mac, local_eth->enetaddr, 6);
	memset(arp_p_p->target_mac, 0x0, 6);
    	memcpy(arp_p_p->sender_ip, &local_ip, 4);
    	memcpy(arp_p_p->target_ip, &server_ip, 4);
	/*send package*/
send_arp:
        len = local_eth->send(p_buf, 60);
	if(len != 60){
        	lprintf("send packages error\r\n");
		return;
	}
	wait = 10;
recv_package:
	while(!(len = local_eth->recv(p_buf))){
		delay_us(1000);
		if(!wait--)
			break;
	}
	if(!len){
		lprintf("get arp response failed, retrying\r\n");
		delay_us(1000000);
		goto send_arp;
	}
	if(ep->protocol != 0x0608 || arp_p_p->operation != 0x0200)
		goto recv_package;
	memcpy(server_mac, arp_p_p->sender_mac, 6);
	print_mem(server_mac, 6);

	/*start send tftp request*/
	/*setup tftp req package*/
	memset(p_buf, 0, MAX_PACKAGE);
	//memcpy(p_buf, tftp_req_exam, 64);

	memcpy(p_buf, tftp_req, 64);
	/*set 802.3 header*/
	memcpy(ep->dest_mac, server_mac, 6);
	memcpy(ep->src_mac, local_eth->enetaddr, 6);

    	iutp->ip_header.id = 0x01;
    	memcpy(iutp->ip_header.sender_ip, &local_ip, 4);
    	memcpy(iutp->ip_header.target_ip, &server_ip, 4);
	print_mem(p_buf, 60);
lprintf("orgin:\r\n");
print_mem(iutp->ip_header.ipv_hdl, 20);
	lprintf("check sum is %x bf\r\n", iutp->ip_header.check_sum);
	iutp->ip_header.check_sum = 0;
	iutp->ip_header.check_sum = for_check(&iutp->ip_header.ipv_hdl, 20);
	lprintf("check sum is %x after\r\n", iutp->ip_header.check_sum);
	lprintf("check %x after sum\r\n", for_check(&iutp->ip_header.ipv_hdl, 20));
    	iutp->udp_header.check_sum = 0;
    	iutp->udp_header.src_port = random_u16();
    	while(iutp->udp_header.src_port < 2000)
		iutp->udp_header.src_port *= 3;
    	iutp->udp_header.length = change_end(17 + lstrlen(name));
    	s = (char*)&iutp->tftp_packet.block_n;
print_mem(s, 32);
    	lstrcpy(s, name);
print_mem(s, 32);
	s += lstrlen(s) + 1;
print_mem(s, 32);
    	lstrcpy(s, "octet");
print_mem(s, 32);
	len = 50 + lstrlen(name);
	if(len < 60)
		len = 60;
	len = 64;
	lprintf("send len is %x \r\n", len);
send_tftp_req:
        len1 = local_eth->send(p_buf, len);
	if(len != len){
        	lprintf("send packages error\r\n");
		return;
	}
	wait = 10;
	
	local_eth->halt();
	lprintf("cs8900 is ready, please wait code ready!\r\n");
}
#endif

