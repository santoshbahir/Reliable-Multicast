#include "net_include.h"
#include "typedefine.h"

int main()
{
    struct sockaddr_in name;
    struct sockaddr_in send_addr;

    int                mcast_addr;

    struct ip_mreq     mreq;
    unsigned char      ttl_val;

    int                ss,sr;
    char               input_buf[80];
    struct packet_r     rp;

    rp.type=packet_Start;

    memcpy(input_buf, &rp,sizeof(rp));

    mcast_addr = 226 << 24 | 0 << 16 | 1 << 8 | 50; /* (225.0.1.1) */

    sr = socket(AF_INET, SOCK_DGRAM, 0); /* socket for receiving */
    if (sr<0) {
        perror("Mcast: socket");
        exit(1);
    }

    name.sin_family = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons(PORT);

    if ( bind( sr, (struct sockaddr *)&name, sizeof(name) ) < 0 ) {
        perror("Bcast: bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = htonl( mcast_addr );

    /* the interface could be changed to a specific interface if needed */
    mreq.imr_interface.s_addr = htonl( INADDR_ANY );

    if (setsockopt(sr, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq, 
        sizeof(mreq)) < 0) 
    {
        perror("Mcast: problem in setsockopt to join multicast address" );
    }

    ss = socket(AF_INET, SOCK_DGRAM, 0); /* Socket for sending */
    if (ss<0) {
        perror("Bcast: socket");
        exit(1);
    }

    ttl_val = 1;
    if (setsockopt(ss, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&ttl_val, 
        sizeof(ttl_val)) < 0) 
    {
        printf("Mcast: problem in setsockopt of multicast ttl %d - ignore in WinNT or Win95\n", ttl_val );
    }

    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = htonl(mcast_addr);  /* mcast address */
    send_addr.sin_port = htons(PORT);
    
    sendto( ss, input_buf, 80, 0, 
      (struct sockaddr *)&send_addr, sizeof(send_addr));

    exit(0);
}
