#ifndef NRTIME_H
#define NRTIME_H

#include <sys/time.h>

/******************************************************************************
 * This functions adds two structure a and b of timeval type and return result*
 ******************************************************************************/
struct timeval add_timeval(struct timeval a, struct timeval b);


/******************************************************************************
 * This functions subtracts two structure a and b of timeval type and return  *
 * result                                                                     *
 ******************************************************************************/
struct timeval sub_timeval(struct timeval a, struct timeval b);


/******************************************************************************
 * This functions converts timeval into microsecs                             *
 ******************************************************************************/
unsigned long long tvtous(struct timeval tv);


/******************************************************************************
 * This functions converts microsecs into timeval                             *
 * result                                                                     *
 ******************************************************************************/
struct timeval ustotv(unsigned long long us);

#endif
