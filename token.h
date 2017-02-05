#ifndef _TOKEN_H
#define _TOKEN_H

#include "typedefine.h"


/******************************************************************************
 * Forward the token to the successor. In this module the copy of the last    *
 * token received is maintained.                                              *
 ******************************************************************************/
void send_token(struct packet_r *token);


/******************************************************************************
 * The main function. When the token is received. This function is called and *
 * it does all important activity of the protocol.And in the end forward it to*
 * next person on the ring.                                                   *
 ******************************************************************************/
void handle_token(struct packet_r *token);


/******************************************************************************
 * This function is called only once on the node number 1 to create the first *
 * token.                                                                     *
 ******************************************************************************/
void create_token();


/******************************************************************************
 * This returns the last token available on this node.                        *
 ******************************************************************************/
struct packet_r *get_token();

void process_rtl_list(struct token_data *token);

void show_token_stats(struct packet_r *token);
#endif
