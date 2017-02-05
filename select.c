#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int
main(void)
{
    fd_set rfds;
    struct timeval tv;
    int retval;
    
    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    
    retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */
    
    if (retval)
	printf("Data is available now.\n");
    /* FD_ISSET(0, &rfds) will be true. */
    else
	printf("No data within five seconds.\n");
    
    exit(0);
}
