#include "net_include.h"
#include "typedefine.h"
#include "recv_dbg.h"
#include "inputbuf.h"
#include "outputbuf.h"
#include "misc.h"
#include "ring.h"
#include "token.h"
#include "debugmsg.h"
#include "nrtime.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#define NAME_LENGTH    80

struct sockaddr_in     send_addr;    /*Muclticast addr structure*/
int                    mcast_addr;    /*Multicast address*/
int                    ss,sr;        /*Sending and receiving socket*/

struct sockaddr_in     u_recvaddr;

int                    u_ss;        /*Unicast sending socket*/
int                    u_sr;        /*Unicast receiving socket*/

void init_param(int argc, char* argv[])
{
    PRINTNL;
    
    PINOUT_MISC("Entered\n");
    PINOUT_MISC("argc:%d\n",argc);

//    int loss_rate=0;
    /* START Handle commandline arguments */
    if(argc!=5)
     {
        printf("\nMore Arguments Required");
        printf("\nUsage: mcast <num_of_packets> <machine_index>"\
                " <number_of_machines> <loss_rate>\n");
        exit(0);
    }
    else
     {
        printf("\nNumber of Packets: %s", argv[1]);
        printf("\nMachine Index: %s", argv[2]);
        printf("\nNumber of Machines: %s", argv[3]);
        printf("\nLoss Rate: %s\n", argv[4]);
    }
    /* END Handle commandline arguments */
    
    num_of_packets=atoi(argv[1]);
    my_machine_index=atoi(argv[2]);
    num_of_machines=atoi(argv[3]);
    loss_rate=atoi(argv[4]);
    has_token=false;

    RECVBUFFER_SIZE=num_of_machines*SENDWINDOW_SIZE*SCALING_FACTOR;

    recv_dbg_init(loss_rate, my_machine_index);
    init_inputbuffer();
    init_outputbuffer();

    create_file();

    /*Initialize base token: start*/ 
    struct packet_r *tp;
    struct token_header *th;
    struct token_data *td;
    tp=get_token();
    GET_TOKEN_HEADER(tp,th);
    GET_TOKEN_DATA(tp,td);
    
    th->token_index =-1;
    td->seq         =-1;
    td->aru         =-1;
    td->aru_id      =0;    
    /*Initialize token: end*/ 
    
    duplicate_token=false;

    PDEBUG_MISC("I am '%d'\n",my_machine_index);

    PINOUT_MISC("Exiting\n");
    PRINTNL;
}


void set_unicast_sockets()
{
    struct hostent        h_ent;
    struct hostent        *p_h_ent;
    char                  host_name[NAME_LENGTH] = {'\0'};
    long                  host_num;

    int retval;

    retval=gethostname(host_name, NAME_LENGTH);
    
    p_h_ent = gethostbyname(host_name);
    if ( p_h_ent == NULL ) {
        printf("Ucast: gethostbyname error.\n");
        exit(1);
    }

    memcpy( &h_ent, p_h_ent, sizeof(h_ent));
    memcpy( &host_num, h_ent.h_addr_list[0], sizeof(host_num) );


    u_sr = socket(AF_INET, SOCK_DGRAM, 0); /* socket for receiving */
    if (u_sr<0) {
        perror("Unicast: socket");
        exit(1);
    }

    /*Sending address for unicast*/    
    u_recvaddr.sin_family = AF_INET;
    u_recvaddr.sin_addr.s_addr = host_num;
    u_recvaddr.sin_port = htons(RECV_PORT);

    if ( bind( u_sr, (struct sockaddr *)&u_recvaddr, sizeof(u_recvaddr)) < 0){
        perror("Ucast: bind");
        exit(1);
    }

    u_ss = socket(AF_INET, SOCK_DGRAM, 0); /* socket for sending */
    if (u_ss<0) {
        perror("Unicast: socket");
        exit(1);
    }

    PDEBUG_MISC("My recving ip is %s\n", inet_ntoa(u_recvaddr.sin_addr));
    /*Address for unicast receive*/
}

void get_grant()
{
    int sr=get_sr();
    struct packet_r start_packet;
    struct start_packet *sp;
    
    PDEBUG_MISC("Waiting for grant from start_mcast\n");
    recv(sr, (char *)(&start_packet),sizeof(struct packet_r),0);
    sp=(struct start_packet *)(&start_packet);

    if(sp->type!=packet_Start){
        printf("Received something before start packet..! Exiting...");
        exit(0);
    }

    return;
}

void set_connection()
{
    struct sockaddr_in     name;

    struct ip_mreq         mreq;
    unsigned char        ttl_val;

    PRINTNL;
    PINOUT_MISC("Entered\n");
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

    mreq.imr_multiaddr.s_addr = htonl(mcast_addr);

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

    PINOUT_MISC("ss:->%d\tsr:->%d\n", ss, sr);
    PINOUT_MISC("Multicast IP:%s\n", inet_ntoa(send_addr.sin_addr));
    PINOUT_MISC("Exiting\n");
}


int mcast_select(fd_set *rfds, struct timeval tv, char *packet)
{
    struct timeval t_start, t_end, t_diff;
    int retval;    
    int sr;
    int u_sr;
    int num=0;
    int read_bytes;

    PRINTNL;
    PINOUT_MISC("Entered\n");

    FD_ZERO(rfds);

    sr=get_sr();
    u_sr=get_u_sr();

    FD_SET(sr,rfds);
    FD_SET(u_sr,rfds);

    for(;;)
    {
        PINOUT_MISC("Inside loop\n");

        if(gettimeofday(&t_start, NULL) == -1){
            printf("Failed to get start time; so exiting\n");
            exit(0);
        }

        retval = select(FD_SETSIZE, rfds, NULL, NULL, &tv);

        if(gettimeofday(&t_end, NULL) == -1){
            printf("Failed to get select ret time; so exiting\n");
            exit(0);
        }

        if(retval){
            if(FD_ISSET(sr, rfds))    
                read_bytes=recv_dbg(sr, packet, sizeof(struct packet_r),0);
            else if(FD_ISSET(u_sr, rfds))
                read_bytes=recv_dbg(u_sr, packet, sizeof(struct packet_r),0);

            if(read_bytes==0){
                t_diff=sub_timeval(t_end, t_start);

                if(tvtous(t_diff)<=10)
                    return 0; /*Waited for timeout period*/
                else
                    tv=t_diff;
            }
            else
                return read_bytes;
        }
        else
            return 0;
    }

    PINOUT_MISC("Exiting\n");
    PRINTNL;
    return num;
}


void handle_packet(struct packet_r *packet)
{
    packet_t  p_type;
    struct succ_req *ptr_req=(struct succ_req *)packet;
    struct message_header *ptr_msg=(struct message_header *)packet;

    PRINTNL;
    PINOUT_MISC("Entered\n");

    p_type=packet->type;
    
    switch(p_type){
        case packet_Request:
            PDEBUG_MISC("Got Request:\n");
            PDEBUG_MISC("Request from :%d\n", ptr_req->from_machine_index);
            if(ptr_req->to_machine_index == my_machine_index){
                PDEBUG_MISC("No response is sent\n");
                send_response(ptr_req->my_ip);
            }
            break;
        case packet_Response:
            PDEBUG_MISC("Got response from %d\n",((struct succ_resp *)packet)->from_machine_index);
            PDEBUG_MISC("Got response and hence discarding\n");
            break;
        case packet_Token:
            printf("\n\n<DEBUG-TOKEN>-----------------LOGICAL SEPARATION-------------------::\t\t\t\t\n");
            printf("\n\n<DEBUG-TOKEN>:TOKEN-STAT AT ENTRY:\t\t\t\t\n");
            show_token_stats(packet);
            PDEBUG_MISC("Got Token:TOKEN-TYPE->%d\n",packet->type);
            has_token=true;
            handle_token(packet);
            break;            
        case packet_Data:
            PDEBUG_MISC("Got Data:\n");
            printf("\n\n<DEBUG-MSG>-----------------LOGICAL SEPARATION-------------------::\t\t\t\t\n");
            printf("\n<DEBUG-MSG>MSG-STAT AT ENTRY:\n");
            if(ptr_msg->machine_index==my_machine_index){
                printf("<DEBUG-MSG>My own packet discard\n");
                printf("<DEBUG-MSG>Discarding packet:-<machine index=%d,seq=%d,host_seq=%d>\n",\
                            ptr_msg->machine_index, ptr_msg->seq, ptr_msg->host_seq);
                return;
            }
            ip_addpacket(packet);
            show_message_stats(packet);
            break;
        default:
            PDEBUG_MISC("Getting unhandled packet type:\n");
            break;
    }
    PINOUT_MISC("Exiting\n");
    PRINTNL;
    return;
}

struct packet_r *build_packet(void *header,void *data)
{
    struct packet_r *packet;
    void *start_addr;
    int header_len;
    int data_len;
    int packetType;


//    PDEBUG_MISC("header:%d\t%d\n", (int *)*header, (int *)*data);

    if((packet=(struct packet_r *)malloc(sizeof(struct packet_r)))==\
            NULL)
    {
        PDEBUG_MISC("malloc failed: Hence cant build the packet\n");
        exit(0);
    }

//    packet=(struct packet_r *)header;
    packetType=((struct packet_r *)header)->type;

    if(packetType==packet_Data){
        header_len=sizeof(struct message_header);
        data_len=sizeof(struct message_data);
    }
    else if(packetType==packet_Token){
        header_len=sizeof(struct token_header);
        data_len=sizeof(struct token_data);
    }

    PDEBUG_MISC("PACKET_TYPE=%d\n",((struct packet_r *)header)->type);
    start_addr=(char *)packet;
    memcpy(start_addr, header, sizeof(struct packet_r));
    //memcpy((char *)start_addr+header_len, data,data_len);

    return packet;
}


void create_file()
{
    char filename[20];
    char extn[]=".out";

    sprintf(filename, "%d", my_machine_index);
    strcat(filename,extn);

    if((fw=fopen(filename, "w"))==NULL){
        perror("File creation failed\n");    
        exit(0);
    }
    
    PDEBUG_MISC("filename=%s\n", filename);

    return;

}

int get_ss()
{
    return ss;
}


int get_sr()
{
    return sr;
}

int get_u_ss()
{
    return u_ss;
}

int get_u_sr()
{
    return u_sr;
}

struct sockaddr_in get_multicast_addr()
{
    return send_addr;
}

struct sockaddr_in get_unicast_recvaddr()
{
    return u_recvaddr;
}        
