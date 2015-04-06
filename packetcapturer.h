#ifndef PACKETCAPTURER_H
#define PACKETCAPTURER_H

#include <QObject>
#include <pcap.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

class PacketCapturer : public QObject
{
    Q_OBJECT
public:
    explicit PacketCapturer(QObject *parent = 0);
    PacketCapturer(const char* deviceName);
    void ChangeEmitter(int value);
    ~PacketCapturer();

private:
    pcap_t *handle;			/* Session handle */
    char *dev;			/* The device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    struct bpf_program fp;		/* The compiled filter */
    //char* filter_exp;	/* The filter expression */
    bpf_u_int32 mask;		/* Our netmask */
    bpf_u_int32 net;		/* Our IP */
    struct pcap_pkthdr header;	/* The header that pcap gives us */
    const u_char *packet;		/* The actual packet */


/* ethernet headers are always exactly 14 bytes */
#define SIZE_ETHERNET 14

signals:
    void SIG_NEW_TONE(int);

public slots:
    void SLOT_CAPTURE();
};

#endif // PACKETCAPTURER_H
