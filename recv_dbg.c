#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

static int cutoff = 64; /* default is 25% loss */

/* Initialize recv_dbg with two parameters 
 * percent = the loss in percent
 * machine_index = the unique number of the machine (1-10) */
void recv_dbg_init(int percent, int machine_index)
{
    struct timeval t;
    unsigned int seed;
        /* percent is in integer form (i.e. 1 = 1%, 5 = 5%) */
        cutoff  = (percent * 0.01 * 0xff);
    gettimeofday( &t, NULL );
    seed = t.tv_sec + t.tv_usec + machine_index;    
    srand( seed );
        printf("\nrecv_dbg_init: percent loss = %d, cutoff value"
               " = %d, seed = %d\n", 
        percent, cutoff, seed );
}

int recv_dbg(int s, char *buf, int len, int flags)
{
    int     ret;
    int    decision;

    decision = rand() & 0xff;
    
    ret = recv( s, buf, len, flags );
    if( (decision < cutoff) || (cutoff == 0xff) ) return(0);
    return( ret );
}

