/*
 * Creating connection, handling token and packets
 */

#ifndef _MISC_H
#define _MISC_H

#include <time.h>
#include <sys/time.h>

#include "typedefine.h"
#include "net_include.h"
#include "debugmsg.h"


/******************************************************************************
 * handles the command line argument.                                         *
 ******************************************************************************/
void init_param(int argc, char* argv[]);


/******************************************************************************
 * Setting up socket for unicast purpose, namely respose packet in ring       *
 * formation.                                                                 *
 ******************************************************************************/
void set_unicast_sockets();


/******************************************************************************
 * creates two sockets - one for sending and another for receiving multicast  *
 * messages.                                                                  *
 ******************************************************************************/
void set_connection();


/******************************************************************************
 * Waiting for the start message from the start_mcast program.                *
 ******************************************************************************/
void get_grant();


/******************************************************************************
 * Wrapper around existing select function. This will not return if the       *
 * recv_dbg decided to drop the packets.                                      *
 ******************************************************************************/
int mcast_select(fd_set *rfds, struct timeval tv, char *buf);


/******************************************************************************
 * Handles all type of packets except start packet. This will just call the   *
 * corresponding packet handler.                                              *
 ******************************************************************************/
void handle_packet(struct packet_r *packet);


/******************************************************************************
 * BUilds the packet from the logical packet. It could be token, message      *
 * packet,ack, nack, control packet, etc.                                     *
 ******************************************************************************/
struct packet_r *build_packet(void *header,void *data);
    

/******************************************************************************
 * This set of functions returns different socket descriptor for to rest of   *
 * modules.                                                                   *
 * socket ss:   For mulitcast send                                            *
 * socket sr:   For mulitcast receive                                         *
 * socket u_ss: For unicast send                                              *
 * socket u_sr: For unicast receive                                           *
 ******************************************************************************/
int get_ss();
int get_sr();
int get_u_ss();
int get_u_sr();


/******************************************************************************
 * This set of function returns the sockaddr_in structure to rest of modules. *
 * get_multicast_addr - returns the multicast address                         *
 * get_unicast_addr   - returns the unicast address                           *
 ******************************************************************************/
struct sockaddr_in get_multicast_addr();
struct sockaddr_in get_unicast_recvaddr();

void create_file();
#endif
