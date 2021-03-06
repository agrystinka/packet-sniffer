#ifndef SNIFFER
#define SNIFFER

#include <pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN 6

/* Ethernet header */
struct sniff_ethernet {
        u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination address */
        u_char  ether_shost[ETHER_ADDR_LEN];    /* source address */
        u_short ether_type;                     /* IP, ARP, etc. */
};

void sn_start();

#endif
