#include "packetcapturer.h"

#include <pcap.h>

#include <qdebug.h>

#include <QApplication>

#define ETHERTYPE_EAP (0x888e) /* eap authentication */

namespace packetprocess {

u_short recentFreq = 100;
QObject* parent;

#define TYPE_TCP (6)

/* Ethernet header */
struct sniff_ethernet {
    u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
    u_short ether_type; /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
    u_char ip_vhl;		/* version << 4 | header length >> 2 */
    u_char ip_tos;		/* type of service */
    u_short ip_len;		/* total length */
    u_short ip_id;		/* identification */
    u_short ip_off;		/* fragment offset field */
#define IP_RF 0x8000		/* reserved fragment flag */
#define IP_DF 0x4000		/* dont fragment flag */
#define IP_MF 0x2000		/* more fragments flag */
#define IP_OFFMASK 0x1fff	/* mask for fragmenting bits */
    u_char ip_ttl;		/* time to live */
    u_char ip_p;		/* protocol */
    u_short ip_sum;		/* checksum */
    struct in_addr ip_src,ip_dst; /* source and dest address */
};
#define IP_HL(ip)		(((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)		(((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
    u_short th_sport;	/* source port */
    u_short th_dport;	/* destination port */
    tcp_seq th_seq;		/* sequence number */
    tcp_seq th_ack;		/* acknowledgement number */
    u_char th_offx2;	/* data offset, rsvd */
#define TH_OFF(th)	(((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;		/* window */
    u_short th_sum;		/* checksum */
    u_short th_urp;		/* urgent pointer */
};

//static callback for libpcap
void Callback_ProcessPacket(u_char *useless, const pcap_pkthdr *pkthdr, const u_char *packet){

    //just a pointer to avoid the extra casts later
    PacketCapturer* p = ((PacketCapturer*)parent);

    const struct sniff_ethernet *ethernet; /* The ethernet header */
    const struct sniff_ip *ip; /* The IP header */
    const struct sniff_tcp *tcp; /* The TCP header */
    //const char *payload; /* Packet payload */

    u_int size_ip;
    u_int size_tcp;

    //the beginning is ethernet header
    ethernet = (struct sniff_ethernet*)(packet);

    //QString("%1").arg(yourNumber, 5, 10, QChar('0'));
    //qDebug() << "ethernet type:" << QString("%1").arg(ntohs(ethernet->ether_type), 4, 16).prepend("0x") << ntohs(ethernet->ether_type);

    if(ntohs(ethernet->ether_type) == ETHERTYPE_EAP){

        p->ChangeEmitter( 1200, 0);
        p->ChangeEmitter( 1215, 0);

        return;
    }

    if(ntohs(ethernet->ether_type) != ETHERTYPE_IP)
        return;

    //continuing UP the stack
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);

    if(ip->ip_p != TYPE_TCP ){

        p->ChangeEmitter( 150, 0);
        p->ChangeEmitter( 148+ip->ip_p, 0);

        return;
    }

    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        qWarning() << "Invalid IP header length:" << size_ip;
        return;
    }

    //The TCP header is after the ethernet and ip headers
    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);

    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        qWarning() << "Invalid TCP header length:" << size_tcp;
        return;
    }

    //p->ChangeEmitter( ntohs(tcp->th_dport)&1023|ntohs(tcp->th_sport), 1);
    //p->ChangeEmitter( ntohs(tcp->th_dport), 1);
    p->ChangeEmitter( ntohs(tcp->th_sport), 1);

}


}//end packetprocess namespace

PacketCapturer::PacketCapturer(QObject *parent) : QObject(parent)
{
}

//constructor
PacketCapturer::PacketCapturer(const char *deviceName){

    qDebug() << Q_FUNC_INFO << "setting filter";
    //define my filter only care about ip stuff to and from this machine
    char filter_exp[] = "ip";

    //copy the deviceName into the device I'll be using
    dev = (char*)malloc(10*sizeof(char));
    strcpy(dev, deviceName);


    //Open the session in promiscuous mode
    qDebug() << Q_FUNC_INFO << "Opening Device" << dev;
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        qDebug() << Q_FUNC_INFO << "Couldn't open device" << dev << errbuf;
        exit(-1);
    }
    // Compile the filter
    qDebug() << Q_FUNC_INFO << "compiling filter";
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        qDebug() << Q_FUNC_INFO << "Couldn't parse filter" << filter_exp << pcap_geterr(handle);
        exit(-1);
    }

    //setting the filter
    if (pcap_setfilter(handle, &fp) == -1) {
        qDebug() << Q_FUNC_INFO << "Couldn't install filter" << filter_exp << pcap_geterr(handle);
        exit(-1);
    }

    //associating the object instance within this static function
    /** \note seems like a bad design but it works */
    packetprocess::parent = this;

    qDebug() << Q_FUNC_INFO << " PCAP setup correctly";
}


//an object callback to send signals from the Static ProcessPacket function
void PacketCapturer::ChangeEmitter(int value, int often){
    emit( SIG_NEW_TONE(value, often) );
}

//a slot is needed for multi thread interfacing
void PacketCapturer::SLOT_CAPTURE(){
    qDebug() << Q_FUNC_INFO << " Setting up capture loop";
    pcap_loop( handle, 0, ((pcap_handler)packetprocess::Callback_ProcessPacket), NULL);
}


PacketCapturer::~PacketCapturer()
{
    /* And close the session */
    pcap_close(handle);
}

