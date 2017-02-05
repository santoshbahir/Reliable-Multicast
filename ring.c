#include <netinet/in.h>
#include <arpa/inet.h>
#include "net_include.h"
#include "typedefine.h"
#include "recv_dbg.h"
#include "misc.h"
#include "debugmsg.h"
#include "nrtime.h"

struct sockaddr_in  succ_addr;  /*Muclticast addr structure*/

void form_token_ring()
{
/*  **determistic algorithm**
    #*  SENDING STATE
    get multicast address
    mulitcast succ_id_req;
    *#
    
    #*  RECEIVING STATE
    if request from predeccor received; send response to it;
    if response/or request message of succesor received;
        save the succssor_id; change the state;
    if anything else received discard it

    if timeout; goto SENDING STATE:
    *#
*/

    //int read_bytes=0;
    struct sockaddr_in mcast_addr=get_multicast_addr();
    int ss=get_ss();
    int u_ss=get_u_ss();
    int sr=get_sr();
    int u_sr=get_u_sr();
    struct succ_req     pkt_req;
    struct succ_req     pkt_resp;
    struct packet_r     *packet;
    struct succ_req     *req_tmp;
    struct succ_resp    *resp_tmp;

    fd_set             frds;
    int num=0;

    PRINTNL;
    PINOUT_RING("Entered\n");

    if((packet=(struct packet_r *)malloc(sizeof(struct packet_r)))==NULL){
        printf("Malloc failed\n");
    }
    /*BUild and send Request to Successor for its ip address*/
    pkt_req.type=packet_Request;
    pkt_req.from_machine_index=my_machine_index;
    pkt_req.to_machine_index=(my_machine_index%num_of_machines)+1;
    pkt_req.my_ip=get_unicast_recvaddr();
    pkt_req.req_pkt_size=sizeof(struct succ_req);


    sendto(ss, &pkt_req, sizeof(pkt_req), 0,
          (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));

    /*Build Response to Predessor with my ip address*/
    pkt_resp.type=packet_Response;
    pkt_resp.from_machine_index=my_machine_index;

    if(my_machine_index==1)
        pkt_resp.to_machine_index=num_of_machines;
    else
        pkt_resp.to_machine_index=((my_machine_index-2)%num_of_machines)+1;

    pkt_resp.my_ip=get_unicast_recvaddr();
    pkt_resp.req_pkt_size=sizeof(struct succ_resp);


    struct timeval timeout;
    timeout.tv_sec=TOKEN_TIMEOUT;
    timeout.tv_usec=0;

    for(;;)
    {
        PDEBUG_RING("Entered ring formation loop:%d\n",num);
        num = mcast_select(&frds, timeout, (char *)packet);
        if (num > 0) {
            if (FD_ISSET(sr, &frds) || FD_ISSET(u_sr, &frds)) {
                PDEBUG_RING("packet type=%d\n",packet->type);
                if(packet->type==packet_Response){
                    resp_tmp=(struct succ_resp *)packet;
                    PDEBUG_RING("Got response from %d:\n",resp_tmp->from_machine_index);
                    PDEBUG_RING("Response was for me %d:\n",resp_tmp->to_machine_index);

                    if(resp_tmp->to_machine_index==my_machine_index){
                        succ_addr=resp_tmp->my_ip;
                        PDEBUG_RING("My successor is: %d\t ITS IP=%s\n",resp_tmp->from_machine_index,
                                        inet_ntoa(succ_addr.sin_addr));
                        break;
                    }
                }
                else if(packet->type==packet_Request){
                    req_tmp=(struct succ_req *)packet;
                    PDEBUG_RING("Got request from %d:\n",req_tmp->from_machine_index);

                    if(req_tmp->from_machine_index==\
                      ((my_machine_index%num_of_machines)+1)){
                        succ_addr=req_tmp->my_ip;
                        PDEBUG_RING("My successor is: %d\t ITS IP=%s\n",req_tmp->from_machine_index,
                                        inet_ntoa(succ_addr.sin_addr));
                        break;
                    }

                    /*Send response to predecessor*/
                    if(req_tmp->to_machine_index==my_machine_index){
                        sendto(u_ss, &pkt_resp, sizeof(pkt_resp), 0,
                       (struct sockaddr *)&(req_tmp->my_ip), sizeof(req_tmp->my_ip));
                    }
                }
                else{   /*Drop packet as I am not in the ring yet*/
                    PDEBUG_RING("Dropping packets\n");
                    ;
                }
            }
        }
        /*Time out*/
        else{
            /*Resend the request as time out took place*/
            PDEBUG_RING("Retransmitting\n");
            sendto(ss, &pkt_req, sizeof(pkt_req), 0,
                (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));
       }
    }
    PINOUT_RING("Exiting\n");
    PRINTNL;
}


void send_response(struct sockaddr_in pred_addr)
{
    struct succ_resp pkt_resp;
    int u_ss=0;
    int ss=0;
    int ret_value;

    PRINTNL;
    PINOUT_RING("Entered\n");
    /*Build Response to Predessor with my ip address*/
    pkt_resp.type=packet_Response;
    pkt_resp.from_machine_index=my_machine_index;

    if(my_machine_index==1)
        pkt_resp.to_machine_index=num_of_machines;
    else
        pkt_resp.to_machine_index=((my_machine_index-2)%num_of_machines)+1;

    pkt_resp.my_ip=get_unicast_recvaddr();
    pkt_resp.resp_pkt_size=sizeof(struct succ_resp);
    
    u_ss=get_u_ss();
    ss=get_ss();

//    struct sockaddr_in mcast_addr=get_multicast_addr();

    PDEBUG_RING("u_ss=%d\n",u_ss);

    ret_value=sendto(u_ss, &pkt_resp, sizeof(pkt_resp), 0,
        (struct sockaddr *)&pred_addr, sizeof(pred_addr));

    PINOUT_RING("ret_value=%d\n",ret_value);
    PDEBUG_RING("Sending to ->%s\n", inet_ntoa(pred_addr.sin_addr));
    PINOUT_RING("Exiting\n");
    PRINTNL;
}

struct sockaddr_in get_successor_addr()
{
    return succ_addr;
}

