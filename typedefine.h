/*
 * This file contains all the message type declarations and global data
 */

#ifndef _TYPEDEFINE_H
#define _TYPEDEFINE_H

#define PACKET_SIZE            1200

#define SENDBUFFER_SIZE          10
#define SENDWINDOW_SIZE          10

#define SCALING_FACTOR          2
#define NUMBER_OF_NODES         10

//#define RECVBUFFER_SIZE        NUMBER_OF_NODES*SENDBUFFER_SIZE

#define bitmap_bytes(a, b)  ((((a) * (b)) % (sizeof(unsigned int) * 8)) ? \
                            (((a) * (b)) / (sizeof(unsigned int) * 8)) + 1 : \
                            (((a) * (b)) / (sizeof(unsigned int) * 8)))

#define TOKEN_TIMEOUT           100000

#define GET_MSG_HEADER(prp,pmh)         (pmh=(struct message_header *)prp)
#define GET_MSG_DATA(prp,pmd)           (pmd=(struct message_data *)((char *)prp+sizeof(struct message_header)))
#define GET_TOKEN_HEADER(prp,pth)       (pth=(struct token_header *)prp)
#define GET_TOKEN_DATA(prp,ptd)         (ptd=(struct token_data *)((char *)prp+sizeof(struct token_header)))


#include <stdbool.h>
#include "net_include.h"

typedef enum {
    packet_Data,                // Data packet
    packet_Token,                // Data packet
    packet_Start,                // Data packet
    packet_Request,                // Successor Req packet
    packet_Response                // Successor Resp packet
}packet_t;

/*
 * Header of the packet which carries user data
 */
struct message_header{
    packet_t    type;
    int         machine_index;
    int         seq;
    int         host_seq;
    int         msg_size;
};

struct message_data{
    int        data;
};

/*
 * Header of the packet which carries token
 */
struct token_header{
    packet_t    type;
    int         token_index;
    int         token_size;
};


/*
 * Header of the packet which carries token data
 */
struct token_data{
    int seq;
    int aru;
    int aru_id;
    int rrl_size;
    unsigned int rrl_bitmap[bitmap_bytes(10, (SCALING_FACTOR*SENDBUFFER_SIZE))];        
};


struct succ_req{
    packet_t            type;
    int                 from_machine_index;
    int                 to_machine_index;
    int                 req_pkt_size;
    struct sockaddr_in  my_ip;
};

struct succ_resp{
    packet_t            type;
    int                 from_machine_index;
    int                 to_machine_index;
    int                 resp_pkt_size;
    struct sockaddr_in  my_ip;
};

struct start_packet{
    packet_t            type;
};


struct packet_r{
    packet_t            type;
    unsigned char       bytes[PACKET_SIZE];
};

int my_machine_index;
int num_of_machines;
int num_of_packets;    /*I will send this many packets*/
int RECVBUFFER_SIZE;
int loss_rate;

FILE    *fw;
bool    duplicate_token;
bool    has_token;
int     total_retrans;

bool    pkt_missing;
struct  timeval start_time;      /*Starting time of mcast*/
struct  timeval end_time;        /*End time of mcast*/
struct  timeval elapsed_time;    /*Diff in time*/
#endif
