/*
 * The buffer mangement and sliding window logic for sending data
 */


#ifndef _OUTPUTBUF_H
#define _OUTPUTBUF_H

#include "stdbool.h"
#include "net_include.h"

/******************************************************************************
 * This function initiate the output buffer and required parameter for the    *
 * outbuffer module.                                                          *
 ******************************************************************************/
void init_outputbuffer();


/******************************************************************************
 * This function send packets from this node. The number of packets to be sent*
 * are determined by the seq number and aru present in the token.             *
 ******************************************************************************/
int send_packets(int seq, int aru);


/******************************************************************************
 * This function returns true if the data is available to sent;               *
 * false otherwise.                                                           *
 ******************************************************************************/
bool data_to_send();

#endif
