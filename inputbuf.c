#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inputbuf.h"
#include "debugmsg.h"
#include "token.h"

int my_aru=-1;
int wbase=0;
int last_processed_pkt=-1;
void init_inputbuffer()
{
    int i;
    input_buffer=(struct packet_r **)malloc(sizeof(struct packet_r *)*RECVBUFFER_SIZE);

     for(i=0; i<RECVBUFFER_SIZE; i++)
     {
        input_buffer[i]=(struct packet_r *) \
                         malloc(sizeof(struct packet_r));
        memset(input_buffer[i],-2,sizeof(struct packet_r));  
     }
}


void ip_addpacket(struct packet_r *packet)
{
    int index;
    struct message_header *pmh;
    struct message_data *pmd;
    
    PDEBUG_INPUTBUF("Entered\n");

    GET_MSG_HEADER(packet, pmh);
    GET_MSG_DATA(packet, pmd);
    PDEBUG_INPUTBUF("seq num is=%ddata:%d\n",pmh->seq,pmd->data);
    if(pmh->seq < my_aru){
        PDEBUG_INPUTBUF("The duplicate packet\n");
        return;
    }
    
    index=pmh->seq % RECVBUFFER_SIZE;

    if(((struct message_header *)(input_buffer[index]))->seq == pmh->seq){
        PDEBUG_INPUTBUF("The duplicate packet");
        return;
    }
    else{
        memcpy((void *)input_buffer[index],(void *)packet, sizeof(struct packet_r));
        PDEBUG_INPUTBUF("ADDED PACKET IN INPUT BUFFER: %5d, %6d, %d\n", pmh->machine_index, \
        pmh->seq, pmd->data);
        PDEBUG_INPUTBUF("Got seq is :pmh->seq=%d\n",pmh->seq);
        PDEBUG_INPUTBUF("my aru before update %d\n",my_aru);
        my_aru=update_myaru(pmh->seq);
        PDEBUG_INPUTBUF("Updated  my aru is %d\n",my_aru);
    }

    PDEBUG_INPUTBUF("Exiting\n");
    
    return;
}

void process_packets(struct token_data *tdp)
{
    int tmp_seq=last_processed_pkt+1; 
    struct packet_r *my_token;
    int agreed_max_seq=0;
    struct token_data *token_data;

    my_token=get_token();

    GET_TOKEN_DATA(my_token, token_data);
    int prev_aru=token_data->aru;

    PDEBUG_INPUTBUF("Entered\n");
    PDEBUG_INPUTBUF("RECVBUFFER_SIZE=%d\n",RECVBUFFER_SIZE);

    agreed_max_seq=(prev_aru <= tdp->aru) ? prev_aru : tdp->aru;
    agreed_max_seq=(agreed_max_seq <= get_myaru()) ? agreed_max_seq: get_myaru();

    struct message_header *msg_head;
    struct message_data   *msg_data;
    

    msg_head=(struct message_header *)input_buffer[tmp_seq%RECVBUFFER_SIZE];

    PDEBUG_INPUTBUF("prev_aru=%d\tcur_aru=%d\n", prev_aru, tdp->aru);
    PDEBUG_INPUTBUF("agreed_max_seq=%d\n", agreed_max_seq);

    if(msg_head->seq <= last_processed_pkt){
        PDEBUG_INPUTBUF("Nothing to process\n");
        return;
    }

    tmp_seq=msg_head->seq;
    PDEBUG_INPUTBUF("wbase:%d\ttmp_seq=%d\tmy_aru=%d\n",wbase,tmp_seq,my_aru);

    while(tmp_seq<(agreed_max_seq+1)){
        GET_MSG_HEADER(input_buffer[tmp_seq%RECVBUFFER_SIZE], msg_head); 
        GET_MSG_DATA(input_buffer[tmp_seq%RECVBUFFER_SIZE], msg_data); 
        fprintf(fw,"AGREED ORDER: %5d, %6d, %d\n", msg_head->machine_index, \
        msg_head->seq, msg_data->data);
        PDEBUG_INPUTBUF(":tmp_seq=%d\n",tmp_seq);
        PDEBUG_INPUTBUF(":msg_head=%d\n",msg_head->seq);
        tmp_seq++;
//        msg_head=(struct message_header *)input_buffer[tmp_seq%RECVBUFFER_SIZE];  
//        msg_data=(struct message_data *)input_buffer[tmp_seq%RECVBUFFER_SIZE];  
        last_processed_pkt=agreed_max_seq;
        wbase=(agreed_max_seq+1)%RECVBUFFER_SIZE;
    }
   

    PINOUT_INPUTBUF("Exiting\n");
    return;
}

int get_myaru()
{
    return my_aru;
}

int update_myaru(int seq)
{
    int cur_aru=my_aru;
    int actual_aru=0;  //new
    int tmp_seq=cur_aru;
    struct message_header *msg_head;

    while(cur_aru <= tmp_seq){
        if(tmp_seq==cur_aru){
            actual_aru=cur_aru;       //new
            cur_aru++;
            GET_MSG_HEADER(input_buffer[cur_aru%RECVBUFFER_SIZE],msg_head);
            tmp_seq=msg_head->seq;
        }
        else{
//            cur_aru--;
            break;
        }
    }
//    return cur_aru;
    return actual_aru;
}

int get_last_processed_pkt()
{
    
    return last_processed_pkt;

}

void show_message_stats(struct packet_r *packet)
{
    struct message_header *mh;
    struct message_data   *md;

    GET_MSG_HEADER(packet,mh);
    GET_MSG_DATA(packet,md);

/* Print Message  Header Information */
    printf("<DEBUG-MSG>\t\tPacket Type:%d\n",mh->type);
    printf("<DEBUG-MSG>\t\tMachine Index:%d\n",mh->machine_index);
    printf("<DEBUG-MSG>\t\tSeq:%d\n",mh->seq);
    printf("<DEBUG-MSG>\t\tHost Seq:%d\n",mh->host_seq);
    printf("<DEBUG-MSG>\t\tMessage Size:%d\n",mh->msg_size);

/* Print Message Data */
    printf("\t\tMessage Data :%d\n",md->data);
}

void set_myaru(int seq){
    PDEBUG_INPUTBUF("Entered:%d\n",update_myaru(seq)); 
    update_myaru(seq);
    return;
}
