#include "sniffer.h"
#include "logerr.h"

#include <netdb.h>
#include <stdio.h>    //For standard things
#include <stdlib.h>    //malloc
#include <string.h>    //strlen
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>       //Provides declarations for udp header
#include <netinet/tcp.h>       //Provides declarations for tcp header
#include <netinet/ip.h>        //Provides declarations for ip header
#include <netinet/if_ether.h>  //For ETH_P_ALL
#include <net/ethernet.h>      //For ether_header
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <pcap.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

extern int ACTIVE;

/*For numerating got packets*/
time_t rawtime;
struct tm * timeinfo;

pcap_t *handle;

/* declare pointers to packet headers */
const struct tcphdr *tcp;            /* The TCP header */
const struct udphdr *udp;            /* The UDP header */
const struct icmphdr *icmp;          /* The ICMP header */

/* declare pointers to packet headers */
const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
const struct ip *ip;                    /* The IP header */
const char *payload;                    /* Packet payload */

int size_ip;
int size_tcp;
int size_payload;

int size_ip = 0, size_payload = 0;

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void printTCP(const struct tcphdr *tcp);
void printUDP(const struct udphdr *udp);

void sn_start()
{
    _log(1, "Sniffer start to catch packets\n");

    char *dev; /* name of the device to use */
    char *net; /* dot notation of the network address */
    char *mask;/* dot notation of the network mask    */
    int ret;   /* return code */
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 netp; /* ip          */
    bpf_u_int32 maskp;/* subnet mask */
    struct in_addr addr;

    char filter_exp[] = "ip";  /* filter expression */
    struct bpf_program fp;   /* compiled filter program (expression) */

    /* ask pcap to find a valid device for use to sniff on */
    dev = pcap_lookupdev(errbuf);
    if(!dev)
        err_catch("Couldn't find default device:\n%s", errbuf);
    /* print out device name */
    printf("DEV: %s\n", dev);

    /* ask pcap for the network address and mask of the device */
    ret = pcap_lookupnet(dev, &netp, &maskp, errbuf);
    if(ret == -1)
        err_catch("Couldn't get netmask:\n%s", errbuf);

    /* get the network address in a human readable form */
    addr.s_addr = netp;
    net = inet_ntoa(addr);
    printf("NET: %s\n", net);

    /* do the same as above for the device's mask */
    addr.s_addr = maskp;
    mask = inet_ntoa(addr);
    printf("MASK: %s\n",mask);

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    if(!handle)
        err_catch("Couldn't open device %s:\n%s\n", dev, errbuf);
    _log(3, "pcap_open_live : done\n");

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)
        err_catch("Couldn't parse filter %s:\n%s\n", filter_exp, pcap_geterr(handle));
    _log(3, "pcap_compile : done\n");
    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1)
         err_catch("Could not install filter %s:\n%s\n", filter_exp, pcap_geterr(handle));
    _log(3, "pcap_setfilter : done\n");

    int loopret = pcap_loop(handle, -1, got_packet, NULL);
    _log(3, "pcap_loop : done\n");

    if (loopret == -1)
        err_catch("%s\n", pcap_geterr(handle));
    if (loopret == -2)
        _log(1, "Sniffing succesefully finished.");
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    _log(2, "Sniffer got packet.\n");
    ACTIVE = 1;
    /*if user starrted to collect sniffed packets by usiing command START*/
    if(ACTIVE == 1){
        /*get Current time*/
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        if(!dump)
            err_catch("Cannot open dump file.\n");

        fprintf(dump, "\n----------------Packet %s", asctime(timeinfo));

        /* define ethernet header */
        ethernet = (struct sniff_ethernet*)(packet);

        /*IP header offset */
        ip = (struct ip*)(packet + SIZE_ETHERNET);
        size_ip = IP_HL(ip)*4;
        if (size_ip < 20){
            fprintf(dump, "Invalid IP header length: %u bytes\n\n", size_ip);
            return 0;
        }

        fprintf(dump, "SRC IP: %s\n", inet_ntoa(ip->ip_src));
        fprintf(dump, "DST IP: %s\n", inet_ntoa(ip->ip_dst));

        /* determine protocol */
        switch(ip->ip_p) {
            //fprintf(dump, "Protocol: ");
            case IPPROTO_TCP:
                tcp = (struct tcphdr*)(packet + SIZE_ETHERNET + size_ip);
                printTCP(tcp);
                break;
            case IPPROTO_UDP:
                udp = (struct udphdr*)(packet + SIZE_ETHERNET + size_ip);
                printUDP(udp);
                break;
            case IPPROTO_ICMP:
                icmp = (struct icmphdr *)(packet + SIZE_ETHERNET + size_ip);
                printICMP(icmp);
                break;
            default:
                printf("Protocol: unknown\n");
                fprintf(dump, "# Protocol unknown\n\n");
                return;
        }
        fprintf(dump, "\n");
    }
}

void printTCP(const struct tcphdr *tcp)
{
    fprintf(dump, "Protocol: TCP\n");
    if(tcp != NULL)
        fprintf(dump, "SRC port: %d\n", ntohs(tcp->th_sport));
        fprintf(dump, "DST port: %d\n", ntohs(tcp->th_dport));
}

void printUDP(const struct udphdr *udp)
{
    fprintf(dump, "Protocol: UDP\n");
    if(udp != NULL){
        fprintf(dump, "SRC port: %d\n", ntohs(udp->uh_sport));
        fprintf(dump, "DST port: %d\n", ntohs(udp->uh_dport));
    }
}

void printICMP(const struct udphdr *udp){
    fprintf(dump, "Protocol:  ICMP\n");
    fprintf(dump, "Type: %d",(unsigned int)(icmp->type));

    if((unsigned int)(icmp->type) == ICMP_ECHO)
        fprintf(dump, "  (ICMP Echo)\n");
    else if((unsigned int)(icmp->type) == ICMP_TIMXCEED)
        fprintf(dump, "  (TTL Expired)\n");
    else if((unsigned int)(icmp->type) == ICMP_ECHOREPLY)
        fprintf(dump, "  (ICMP Echo Reply)\n");
}

void printPayload()
{

}
