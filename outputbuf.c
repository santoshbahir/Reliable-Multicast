#include <stdlib.h>
#include <math.h>
#include "stdbool.h"
#include "net_include.h"
#include "inputbuf.h"
#include "token.h"
#include "typedefine.h"
#include "misc.h"

int pktno=0;

/*Function Declarations*/
struct message_header* __build_message_header(struct message_header *msg_head, int seq);
struct message_data* __build_message_data(struct message_data *msg_data);


void init_outputbuffer()
{
    srand(100000);
}

int send_packets(int seq, int aru)
{
    int free_bufs;        /*empty buffer at the slowest node*/
    int i;
    int min_aru=0;
    int lost_packets=0, empty_space=0;
    int prev_aru=-1;

    struct message_header   *msg_head;
    struct message_data     *msg_data;
    struct packet_r         *packet;

    struct token_data       *token_data;
    struct packet_r         *my_token;

    

    my_token=get_token();
    GET_TOKEN_DATA(my_token, token_data);
    prev_aru=token_data->aru;

    int ss=get_ss();
    struct sockaddr_in send_addr=get_multicast_addr();

    PINOUT_OUTPUTBUF("Entered\n");
    PINOUT_OUTPUTBUF("seq=%d\taru=%d\tprev_aru=%d\n", seq, aru,prev_aru);


    min_aru=(prev_aru <= aru) ? prev_aru : aru;
    min_aru=(min_aru <= get_myaru()) ? min_aru : get_myaru();

    min_aru=min_aru;

    lost_packets=abs(seq - min_aru);
    
    empty_space=(RECVBUFFER_SIZE/2) - lost_packets;
    free_bufs=(empty_space >= SENDWINDOW_SIZE) \
                           ? SENDWINDOW_SIZE : ((empty_space<=0)?0:empty_space);

    free_bufs=((num_of_packets - pktno) > free_bufs)?free_bufs:(num_of_packets - pktno);

    PDEBUG_OUTPUTBUF("empty_space=%d\tfree_bufs=%d\tlost_packets=%d\n", \
                    empty_space,free_bufs, lost_packets);

    if (free_bufs > 0) {
        for(i=0; i<free_bufs; i++){
            msg_head = __build_message_header(msg_head, (seq+1)+i);
            msg_data = __build_message_data(msg_data);
            packet=build_packet((void *)msg_head, (void *)msg_data);
        
            /*Add to the input buffer*/
            ip_addpacket(packet);
            /*multicast to its friends*/
               sendto(ss, packet, sizeof(struct packet_r), 0,
                (struct sockaddr *)&send_addr, sizeof(send_addr));

            free(packet);
        }
    }

    PINOUT_OUTPUTBUF("Exiting\n");
    return free_bufs;
}

struct message_header* __build_message_header(struct message_header *msg_head, int seq)
{
    msg_head=(struct message_header *)malloc(sizeof(struct message_header));
    
    if(msg_head == NULL){
        PDEBUG_OUTPUTBUF("malloc failed\n");
        exit(0);
    }

    msg_head->type=packet_Data;
    msg_head->machine_index=my_machine_index;
    msg_head->seq=seq++;
    msg_head->host_seq=pktno++;
    
    return msg_head;
}

struct message_data* __build_message_data(struct message_data *msg_data)
{
    msg_data=(struct message_data *)malloc(sizeof(struct message_data));
    if(msg_data == NULL){
        PDEBUG_OUTPUTBUF("malloc failed\n");
        exit(0);
    }

    msg_data->data=rand();
    return msg_data;
}

bool data_to_send()
{
    PDEBUG_OUTPUTBUF("pktno=%d\tnum_of_packets=%d\n", pktno, num_of_packets);
    if(pktno>=num_of_packets)
        return false;
    else
        return true;
}
