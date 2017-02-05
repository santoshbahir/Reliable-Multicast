
#ifndef RING_H
#define RING_H

/******************************************************************************
 * The current node takes part in the ring formation from this function. it   *
 * remains in this function until it learns the ip of its own successor.      *
 ******************************************************************************/
void form_token_ring();


/******************************************************************************
 * Send response when Current node is in main loop and its predessor requests *
 * for the ip address of current node i.e successor of predecessor.           *
 ******************************************************************************/
void send_response(struct sockaddr_in pred_addr);


/******************************************************************************
 * This function returns the address of the successor if somebody requests    *
 * from the other modules. The successor resides in this module as it is      *
 * learned in this module.                                                    *
 ******************************************************************************/
struct sockaddr_in get_successor_addr();

#endif
