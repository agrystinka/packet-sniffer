#include "main.h"
#include "sniffer.h"
#include "logerr.h"

#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>       //Provides declarations for udp header
#include <netinet/tcp.h>       //Provides declarations for tcp header
#include <netinet/ip.h>        //Provides declarations for ip header
#include <netinet/if_ether.h>
#include <time.h>

static void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
static void printTCP(const struct tcphdr *tcp);
static void printUDP(const struct udphdr *udp);
static void printICMP(const struct icmphdr *icmp);

/* for numerating got packets */
time_t rawtime;
struct tm *timeinfo;

pcap_t *handle;

/* declare pointers to packet headers */
const struct tcphdr  *tcp;            /* The TCP header */
const struct udphdr  *udp;            /* The UDP header */
const struct icmphdr *icmp;           /* The ICMP header */
const struct ip      *ip;             /* The IP header */
const struct sniff_ethernet *ethernet;/* The ethernet header */

int size_ip = 0;

/**
 * Starts packet sniffing.
 *
 * Firstly, it gets info about dafault interface, its IP address and mask.
 * Than it starts to sniff all packets transmited through this interface.
 *
 * Return: void.
 */
void sn_start(void)
{
    _log(1, "Sniffer start to catch packets\n");

    char *dev; /* name of the device to use */
    char *net; /* dot notation of the network address */
    int ret;   /* return code */
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 netp; /* ip          */
    bpf_u_int32 maskp;/* subnet mask */
    struct in_addr addr;

    /* filter expression that will allow to catch only IP packets */
    char filter_exp[] = "ip";
    struct bpf_program fp;   /* compiled filter program (expression) */

    /* ask pcap to find a valid device for use to sniff on */
    dev = pcap_lookupdev(errbuf);
    if(!dev)
        err_catch("Couldn't find default device:\n%s", errbuf);

    /* ask pcap for the network address and mask of the device */
    ret = pcap_lookupnet(dev, &netp, &maskp, errbuf);
    if(ret == -1)
        err_catch("Couldn't get netmask:\n%s", errbuf);

    /* get the network address in a human readable form */
    addr.s_addr = netp;
    net = inet_ntoa(addr);

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

    /* start eternal loop by using -1 as number of iterationt*/
    int loopret = pcap_loop(handle, -1, got_packet, NULL);
    _log(3, "pcap_loop : done\n");

    if (loopret == -1)
        err_catch("%s\n", pcap_geterr(handle));
    if (loopret == -2)
        _log(1, "Sniffing finished.");
}

/**
 * Catches packets.
 * @args: pointer on NULL.
 * @header: pointer on pcap header.
 * @packet: pointer on catched packet.
 *
 * If sniffer is in ACTIVE writing mode it writes packets info into dump file.
 *
 * Return: void.
 */
static void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    _log(2, "Sniffer got packet.\n");

    /*if user started to collect sniffed packets by using command START*/
    if(ACTIVE){
        _log(2, "Sniffer is writting packet.\n");

        /*get Current time*/
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        if(!dump)
            err_catch("Cannot open dump file  2.\n");

        fprintf(dump, "-----Packet %s", asctime(timeinfo));

        /* define ethernet header */
        ethernet = (struct sniff_ethernet*)(packet);

        /*IP header offset */
        ip = (struct ip*)(packet + SIZE_ETHERNET);
        size_ip = (((ip)->ip_hl) & 0x0f)*4;
        if (size_ip < 20){
            fprintf(dump, "Invalid IP header length: %d bytes\n\n", size_ip);
            return;
        }

        fprintf(dump, "SRC IP: %s\n", inet_ntoa(ip->ip_src));
        fprintf(dump, "DST IP: %s\n", inet_ntoa(ip->ip_dst));

        /* determine protocol */
        switch(ip->ip_p) {
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
                fprintf(dump, "Protocol: unknown\n");
                break;
        }
        fprintf(dump, "\n\n");
    }
}

/**
 * Prints TCP header into dump file.
 * @tcp: pointer on TCP header.
 *
 * If TCP header is avaliable, prints source port and description port.
 *.
 * Return: void.
 */
static void printTCP(const struct tcphdr *tcp)
{
    fprintf(dump, "Protocol: TCP\n");
    if(tcp != NULL){
        fprintf(dump, "SRC port: %d\n", ntohs(tcp->th_sport));
        fprintf(dump, "DST port: %d\n", ntohs(tcp->th_dport));
    }
    else{
        fprintf(dump, "SRC port: unknown\n");
        fprintf(dump, "SRC port: unknown\n");
    }
}

/**
 * Prints UDP header into dump file.
 * @udp: pointer on UDP header.
 *
 * If UDP header is avaliable, prints source port and description port.
 *
 * Return: void.
 */
static void printUDP(const struct udphdr *udp)
{
    fprintf(dump, "Protocol: UDP\n");
    if(udp != NULL){
        fprintf(dump, "SRC port: %d\n", ntohs(udp->uh_sport));
        fprintf(dump, "DST port: %d\n", ntohs(udp->uh_dport));
    }
    else{
        fprintf(dump, "SRC port: unknown\n");
        fprintf(dump, "SRC port: unknown\n");
    }
}

/**
 * Prints ICMP packet into dump file.
 * @icmp: pointer on ICMP packet.
 *
 * Return: void.
 */
static void printICMP(const struct icmphdr *icmp)
{
    fprintf(dump, "Protocol: ICMP\n");
    fprintf(dump, "Type: %d",(unsigned int)(icmp->type));

    if((unsigned int)(icmp->type) == ICMP_ECHO)
        fprintf(dump, "  (ICMP Echo)\n");
    else if((unsigned int)(icmp->type) == ICMP_TIMXCEED)
        fprintf(dump, "  (TTL Expired)\n");
    else if((unsigned int)(icmp->type) == ICMP_ECHOREPLY)
        fprintf(dump, "  (ICMP Echo Reply)\n");
}
