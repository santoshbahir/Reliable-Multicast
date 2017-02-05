#include <stdio.h>
#include "nrtime.h"

struct timeval add_timeval(struct timeval a, struct timeval b)
{
    struct timeval result;

    result.tv_sec=a.tv_sec+b.tv_sec;

    if((a.tv_usec+b.tv_usec)>=1000000)
    {
        result.tv_sec=result.tv_sec+1;
        result.tv_usec=((a.tv_usec+b.tv_usec)-1000000);
    }
    else
    {
        result.tv_usec=a.tv_usec+b.tv_usec;
    }

    return result;
}

/*assuming that the a is greater than b*/
struct timeval sub_timeval(struct timeval a, struct timeval b)
{
    struct timeval result;

    result.tv_sec=a.tv_sec-b.tv_sec;

    if((a.tv_usec < b.tv_usec))
    {
        result.tv_sec=result.tv_sec-1;
        result.tv_usec=1000000-(b.tv_usec-a.tv_usec);
    }
    else
    {
        result.tv_usec=a.tv_usec-b.tv_usec;
    }
    
    return result;
}

unsigned long long tvtous(struct timeval tv)
{
    unsigned long long us;

    us=tv.tv_sec*1000000;
    us=us+tv.tv_usec;

    return us;
}

struct timeval ustotv(unsigned long long us)
{
    struct timeval tv;

    tv.tv_sec =us/1000000;
    tv.tv_usec=us%1000000;

    return tv;
}

/*
int main()
{
    struct timeval result,a,b;
    unsigned long long res;

    a.tv_sec=2;    a.tv_usec=9;
    b.tv_sec=1;    b.tv_usec=900000;

//    result=add_timeval(a,b);
//    result=sub_timeval(a,b);
    res=tvtous(a);

    printf("timeval to microsecs=%llu\n", res);

    result=ustotv(res);

    printf("result.tv_sec=%d\t",result.tv_sec);
    printf("result.tv_usec=%d\n",result.tv_usec);
        
    return 0;
}
*/
