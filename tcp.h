#ifndef _TCP_H
#define _TCP_H

#define ARP_RESP 0x0002
#define ARP_REQ 0x0001
#define ARP_TYPE 0x0806

#define REC_PAK_NUM 0x4fff

#define pause // printk("any key...\r\n");while(!received());prepSerialRecv();

#define uch unsigned char

struct eth_h{
    unsigned char dest_mac[6];
    unsigned char src_mac[6];
    unsigned short protocol;
    unsigned char datas[2];
};


typedef struct eth_p send_data;

struct arp_p{
    unsigned short hardware;
    unsigned short protocol;
    unsigned char hardware_addr_len;
    unsigned char protocol_addr_len;
    unsigned short operation;
    unsigned char sender_mac[6];
    unsigned char sender_ip[4];
    unsigned char target_mac[6];
    unsigned char target_ip[4];
    unsigned char other[18];
};

struct ip_h{
    	unsigned char ipv_hdl;
	unsigned char service_type;
	unsigned short total_len;
	unsigned short id;
	unsigned short frg_offset;
	unsigned char live_time;
	unsigned char protocol;
	unsigned short check_sum;
	unsigned char sender_ip[4];
	unsigned char target_ip[4];
};

struct udp_h{
    	unsigned short src_port;
	unsigned short dest_port;
	unsigned short length;
	unsigned short check_sum;
};
 
struct tftp_p{
    	unsigned short operation;
	unsigned short block_n;
	unsigned char tftp_data[512];
};

struct ip_udp_tftp{
    	struct ip_h ip_header;
	struct udp_h udp_header;
	struct tftp_p tftp_packet;
};
#endif
