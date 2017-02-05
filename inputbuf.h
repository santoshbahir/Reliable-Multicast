/*
 * Input buffer handling module
 */


#ifndef    _INPUTBUF_H
#define    _INPUTBUF_H

#include "typedefine.h"

struct packet_r  **input_buffer;
/******************************************************************************
 * This function initiate the input buffer and required parameter for this    *
 * inputbuffer module.                                                        *
 ******************************************************************************/
void init_inputbuffer();


/******************************************************************************
 * This function add the packet into the buffer. This activity is done when   *
 * we dont have token. When we have token we add only our packets into the    *
 * inputbufffer.                                                              *
 * INPUT : staging buffer where buffer to be added is temporarily stored      *
 * OUTPUT: None                                                               *
 ******************************************************************************/
void ip_addpacket(struct packet_r *packet);


/******************************************************************************
 * This function removes the packets from the buffer and write it to the file.*
 * This delivery is in agreed order. Once the packets are removed, they are   *
 * not available for the retransmission.                                      *
 ******************************************************************************/
void process_packets(struct token_data *tdp);


/******************************************************************************
 * This function returns the my_aru to requested module. my_aru is the value  *
 * which shows how many packets I have got in order till now.                 *
 ******************************************************************************/
int get_myaru();

int update_myaru(int seq);

void show_message_stats(struct packet_r *packet);

int get_last_processed_pkt();

void set_myaru(int seq);

#endif
