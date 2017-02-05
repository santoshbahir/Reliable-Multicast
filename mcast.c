#include <sys/time.h>
#include <time.h>
#include "nrtime.h"
#include "stdbool.h"
#include "net_include.h"
#include "ring.h"
#include "inputbuf.h"
#include "token.h"
#include "misc.h"

bool    init_token;
int     total_token_retrans=0;

int main(int argc, char* argv[])
{
    int                 ss,sr,u_sr;
    struct sockaddr_in  send_addr;
    struct timeval      timeout;
    fd_set              frds;              
    struct packet_r     *packet;
    struct packet_r     *token_packet;
    int                 num;

    init_param(argc, argv); /*Command Line arguments*/
    set_connection();       /*Multicast connection*/
    set_unicast_sockets();  /*Unicast connection*/

    get_grant();

    gettimeofday(&start_time, NULL);
    form_token_ring();      /*Ring formation*/
    init_token=true;

    ss=get_ss();
    sr=get_sr();
    u_sr=get_u_sr();
    send_addr=get_multicast_addr();

    timeout.tv_sec=0;
    timeout.tv_usec=TOKEN_TIMEOUT;

    struct token_data *ltdp;

    int time_scale_factor=1;
    for(;;)
    {
        if(init_token && my_machine_index==1) {
            init_token=false;
            create_token();
            struct packet_r *tp;
            tp=get_token();

            has_token=true;
        }

//        duplicate_token=true;   //Need to remove this part
        PDEBUG_MCAST("Entered in main loop\n");
    if((packet=(struct packet_r *)malloc(sizeof(struct packet_r)))==NULL){
//        printf("Malloc failed\n");
        exit(0);
    }

        num = mcast_select(&frds, timeout, (char *)packet);
        if (num > 0) {
            total_token_retrans=-1;
            PDEBUG_MCAST("Received some packet\n");
            if (FD_ISSET(sr, &frds) || FD_ISSET(u_sr, &frds)){
                handle_packet(packet);
                if(duplicate_token){
//                    duplicate_token=false;
                    timeout.tv_sec=1;
                    time_scale_factor=time_scale_factor*2;
                    timeout.tv_usec=TOKEN_TIMEOUT;
                //   timeout.tv_usec=TOKEN_TIMEOUT/(time_scale_factor);
                }
                else{
                    timeout.tv_sec=0;
                    timeout.tv_usec=TOKEN_TIMEOUT;
                }
            }
        }
        /*Time out*/
        else{
            if(has_token){
                printf("<DEBUG-TOKEN>Retransmitting Token-->%d\n",total_token_retrans);
                printf("<DEBUG-TOKEN>----------------------\n");
                token_packet=get_token();
                PDEBUG_MCAST("TOken value is-> %d\n",token_packet->type);
                send_token(token_packet); 

                GET_TOKEN_DATA(token_packet, ltdp);
                if(ltdp->seq==ltdp->aru && ltdp->aru_id==0 && !pkt_missing)
                    total_token_retrans++;

                if(total_token_retrans > num_of_machines*2){
                
                    PDEBUG_MCAST("No response.. Exiting\n");
//                    fclose(fw);
                    gettimeofday(&end_time, NULL);
//                    elapsed_time=sub_timeval(end_time, start_time);
//                    printf("Execution Time=%f\n",(tvtous(elapsed_time)/(float)1000000));
//                    exit(0);
                }
            }
        }
        PDEBUG_MCAST("Exiting in main loop:%d\n",has_token);
    }

    free(packet);
    return 0;
}

