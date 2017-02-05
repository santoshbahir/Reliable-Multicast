#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include "nrtime.h"
#include "token.h"
#include "inputbuf.h"
#include "outputbuf.h"
#include "misc.h"
#include "ring.h"
#include "typedefine.h"
#include "debugmsg.h"
#include "bitmap.h"

struct packet_r my_token;
int unmodified_token=-1; /*new*/
//bool    pkt_missing=false;

void show_token_stats(struct packet_r *token);
void send_token(struct packet_r *new_token)
{
    int ret_value=0;
    struct sockaddr_in succ_addr=get_successor_addr();
    int u_ss=get_ss();

    PRINTNL;
    PINOUT_TOKEN("Entered\n");
    PINOUT_TOKEN("packet_type is %d\n", new_token->type);

    PDEBUG_TOKEN("I am :->%d\n",my_machine_index);
    PDEBUG_TOKEN("Successor is:->%s\n",inet_ntoa(succ_addr.sin_addr));

    ret_value=sendto(u_ss, new_token, sizeof(struct packet_r), 0,
      (struct sockaddr *)&succ_addr, sizeof(struct sockaddr_in));

    printf("\n\n:<DEBUG-TOKEN>TOKEN-STAT AT EXIT:\t\t\t\t\n");
    show_token_stats(new_token);
    printf("\n\n:<DEBUG-TOKEN--LOCAL data>my aru=%d\tdelivered:%d\t\t\t\t\n",get_myaru(), get_last_processed_pkt());
    PINOUT_TOKEN("Global token:->%d",my_token.type);
    PINOUT_TOKEN("Exiting\n");
    PRINTNL;
    return;
}

void handle_token(struct packet_r *token)
{
    int my_aru;
    int num_of_pktsent=0;
    struct token_header     *thp;
    struct token_data       *tdp;
//    struct packet_r         *new_token;

//    bool has_data_sent=false;

    PRINTNL;
    PINOUT_TOKEN("packet_type is %d\n", token->type);


    GET_TOKEN_HEADER(token, thp);
    GET_TOKEN_DATA(token, tdp);

    PDEBUG_TOKEN("thp->token_index %d\n", thp->token_index);
    PDEBUG_TOKEN("my_token.token_index %d\n", ((struct token_header *)&my_token)->token_index);

    /*If processed token received; discard the token*/
    if(thp->token_index <= ((struct token_header *)&my_token)->token_index){
        printf("<DEBUG-TOKEN>Duplicate token, discarding:Token index=%d\n",thp->token_index);
        
//        duplicate_token=true;
        return;
    }

    /*Termination condition*/
    struct token_header     *lthp;
    struct token_data       *ltdp;
    struct packet_r         *local_token;
    
    local_token=&my_token;
    GET_TOKEN_HEADER(local_token, lthp);
    GET_TOKEN_DATA(local_token, ltdp);

    if(ltdp->seq==tdp->seq && ltdp->aru==tdp->aru && get_last_processed_pkt()>=num_of_packets)
        unmodified_token++;

    printf("\n:<DEBUG-TOKEN-------------------------------------------------------------------------------\n");
    printf("\n:<DEBUG-TOKEN--Previous Token\n");
    show_token_stats(&my_token);
    printf("\n\n:<DEBUG-TOKEN--LOCAL data>my aru=%d\tdelivered:%d\t\t\t\t\n",get_myaru(), get_last_processed_pkt());
    printf("\n:<DEBUG-TOKEN-------------------------------------------------------------------------------\n");

    if((unmodified_token > (num_of_machines)) && (!pkt_missing)){
        PDEBUG_TOKEN("No change in token..unmodified token=%d\t!! Hence existing:\n",unmodified_token);
        fclose(fw);
        gettimeofday(&end_time, NULL);
        elapsed_time=sub_timeval(end_time, start_time);
        printf("Execution Time=%f\n",(tvtous(elapsed_time)/(float)1000000)); 
        
        exit(0); 
    }
    
    /*Process rtl list for retransmission and adding our missing packets*/
    PDEBUG_TOKEN("Processing RTL list\n");
    process_rtl_list(tdp);

    /*Deliverying messages in safe order*/
    process_packets(tdp);
    /*Send my packets*/
    if(data_to_send()){
            num_of_pktsent=send_packets(tdp->seq, tdp->aru);
            tdp->seq=tdp->seq+num_of_pktsent;
            PDEBUG_TOKEN("updated: tdp->seq=%d\n",tdp->seq);
    }   
    
    my_aru=get_myaru();
    PDEBUG_TOKEN("Changed aru is: %d\n",tdp->aru);

    if ((my_aru < tdp->aru) || (my_machine_index==tdp->aru_id) ||
            (tdp->aru_id==0)) {

        tdp->aru=get_myaru();          

        if (tdp->aru==tdp->seq) {
            tdp->aru_id=0; 
        } else {
            tdp->aru_id=my_machine_index;
        }

        PDEBUG_TOKEN("updated: tdp->aru=%d\ttdp->aru_id=%d\n",tdp->aru,tdp->aru_id);
    }


    /*Updating the token index*/
    thp->token_index=thp->token_index+1;
    token=build_packet((void *)thp, (void *)tdp);
    my_token=*token;
    PDEBUG_TOKEN("updated: token index=%d\n",lthp->token_index);
    PDEBUG_TOKEN("updated: token index=%d\n",((struct token_header *)&my_token)->token_index);


    send_token(&my_token);


    return;
}

void create_token()
{
    PRINTNL;
    PINOUT_TOKEN("Entered\n");
    struct token_header *thp;
    struct token_data *tdp;
    struct packet_r *first_token = malloc(sizeof(struct packet_r));

    GET_TOKEN_HEADER(&my_token, thp);
    GET_TOKEN_DATA(&my_token, tdp);

    thp->type=packet_Token; 
    thp->token_index=-1;
    thp->token_size=sizeof(struct token_header)+sizeof(struct token_data);

    tdp->seq=-1;
    tdp->aru=-1;
    tdp->aru_id=0;
    tdp->rrl_size=0;
    memset(tdp->rrl_bitmap, 0, sizeof(tdp->rrl_bitmap));

    memcpy((void *)first_token, (void *)&my_token, sizeof(struct packet_r));
    GET_TOKEN_HEADER(first_token, thp);
    thp->token_index=0;

    handle_token(first_token);
    free(first_token);
    PINOUT_TOKEN("Exiting\n");
    PRINTNL;
    return;
}

struct packet_r *get_token()
{
    PINOUT_TOKEN("Entered:->%d\n", ((struct token_header *)&my_token)->token_index);
    PINOUT_TOKEN("Exiting:->%d\n", ((struct token_header *)&my_token)->token_index);
    return &my_token;
}

void process_rtl_list(struct token_data *token)
{
    struct  message_header *msg_head;
    int     start   = get_last_processed_pkt() + 1;
    int     current = start;
    int     ss      = get_ss();
    struct  sockaddr_in multicast_addr  = get_multicast_addr();
    int     index   = current % RECVBUFFER_SIZE;
    int     pkt_seq = 0;    
    int     bitmap_index = 0;

    total_retrans=0;

    PINOUT_TOKEN("Entered:->\n");
    while(current <= token->seq){
        PDEBUG_TOKEN("current:-%d\n",current); 
        index   = current % RECVBUFFER_SIZE;
        GET_MSG_HEADER(input_buffer[index],msg_head);
        pkt_seq = msg_head->seq;
        
        bitmap_index = current%(bitmap_bytes(NUMBER_OF_NODES, (SCALING_FACTOR*SENDBUFFER_SIZE)) * 8);
        //% (bitmap_bytes(NUMBER_OF_NODES,(2 * SENDBUFFER_SIZE)));
        PDEBUG_TOKEN("current:-%d\tbitmap_index:-%d\n",current,bitmap_index); 

        if(pkt_seq != current){ /*Packet is Missing */
//            PDEBUG_TOKEN("packet is missing:pkt_seq%d\n",pkt_seq); 
/*            PDEBUG_TOKEN("missing packet %d 's status in bitmap is %s\n", \
                        current, test(token->rrl_bitmap, bitmap_index) ? "SET" : "UNSET");
*/            set(token->rrl_bitmap,bitmap_index);
            pkt_missing=true;
/*            PDEBUG_TOKEN("missing packet %d 's status in bitmap is %s\n", \
                        current, test(token->rrl_bitmap, bitmap_index) ? "SET" : "UNSET");
*/        }
        else if(test(token->rrl_bitmap,bitmap_index)){
            PDEBUG_TOKEN("packet is present:pkt_seq%d\n",pkt_seq); 
            sendto(ss,input_buffer[index],sizeof(struct packet_r),0,\
                  (struct sockaddr*)&multicast_addr,sizeof(multicast_addr));
            unset(token->rrl_bitmap,bitmap_index);
            total_retrans++;
        }
        current++;
    }


    int colsize=1;
    int i=0, tmp_seq=0;
    for(i=token->aru; i<=token->seq; i++)
    {   
        tmp_seq=i%(bitmap_bytes(NUMBER_OF_NODES, (SCALING_FACTOR*SENDWINDOW_SIZE))*8);
            
//        printf("<seq=%d,STATUS=>%s ; ",i,test(token->rrl_bitmap, tmp_seq) ? "SET  " : "UNSET");
        if(colsize <=3){
            colsize++;
        }   
        else{
//            printf("\n");
            colsize=1;
        }   
    }   
//    printf("\n");

    PINOUT_TOKEN("Exiting:->\n");
    return;
}


void show_token_stats(struct packet_r *token)
{
    struct token_header *th;
    struct token_data   *td;
    
    GET_TOKEN_HEADER(token, th);
    GET_TOKEN_DATA(token, td);

/* Print Token Header Information */

    printf("<DEBUG-%d-TOKEN> \t\tPacket Type:%d\n",my_machine_index, th->type);
    printf("<DEBUG-%d-TOKEN> \t\tToken Index:%d\n",my_machine_index, th->token_index);
    printf("<DEBUG-%d-TOKEN> \t\tToken Size:%d\n",my_machine_index, th->token_size);

/* Print Token Data */

    
    printf("<DEBUG-%d-TOKEN> \t\tToken Seq:%d\n",my_machine_index, td->seq);
    printf("<DEBUG-%d-TOKEN> \t\tToken aru:%d\n",my_machine_index, td->aru);
    printf("<DEBUG-%d-TOKEN> \t\tToken aru_id:%d\n",my_machine_index, td->aru_id);
    printf("<DEBUG-%d-TOKEN> \t\tToken rrl_size:%d\n",my_machine_index, td->rrl_size);
    printf("<DEBUG-%d-TOKEN> \t\tToken rrl:\n",my_machine_index);


    int colsize=1;
    int i=0, tmp_seq=0;
    for(i=td->aru; i<=td->seq; i++)
    {
        tmp_seq=i%(bitmap_bytes(NUMBER_OF_NODES, (SCALING_FACTOR*SENDWINDOW_SIZE))*8);
    
//        printf("<seq=%d,STATUS=>%s ; ",i,test(td->rrl_bitmap, tmp_seq) ? "SET  " : "UNSET");
        if(colsize <=3){
//printf("bit %d is %s\n", i, test(bitmap, i) ? "SET" : dd"UNSET");
            colsize++;
        }
        else{
//            printf("\n");
            colsize=1;
        }
    }
//    printf("\n");
    return;
}
