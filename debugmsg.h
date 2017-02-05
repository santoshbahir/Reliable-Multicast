#ifndef DEBUGMSG_H
#define DEBUGMSG_H

/*New Line Printing*/
#define PRINT_NEWLINE

#ifdef PRINT_NEWLINE
#define PRINTNL printf("\n")
#else
#define PRINTNL
#endif

/*MISC.H*/
#define DEBUG_MSG_MISC

#ifdef DEBUG_MSG_MISC
//#define PDEBUG_MISC(fmt, args...) printf("MI=%d :::(%s:%d) " fmt, my_machine_index, __func__, __LINE__, ## args)
//#define PINOUT_MISC(fmt, args...) printf("MI=%d :::(%s:%d) IO ARGUMENTS: " fmt, my_machine_index, __func__, __LINE__, ## args)
#define PDEBUG_MISC(fmt, args...) printf("(%s:%d) " fmt, __func__, __LINE__, ## args)
#define PINOUT_MISC(fmt, args...) printf("(%s:%d) IO ARGUMENTS: " fmt, __func__, __LINE__, ## args)
#else
#define PDEBUG_MISC(fmt, args...)
#define PINOUT_MISC(fmt, args...) 
#endif


/*OUTPUTBUF.H*/
#define DEBUG_MSG_OUTPUTBUF

#ifdef DEBUG_MSG_OUTPUTBUF
#define PDEBUG_OUTPUTBUF(fmt, args...) printf("(%s:%d) " fmt, __func__, __LINE__, ## args)
#define PINOUT_OUTPUTBUF(fmt, args...) printf("(%s:%d) IO ARGUMENTS: " fmt, __func__, __LINE__, ## args)
#else
#define PDEBUG_OUTPUTBUF(fmt, args...)
#define PINOUT_OUTPUTBUF(fmt, args...) 
#endif


/*INPUTBUF.H*/
#define DEBUG_MSG_INPUTBUF

#ifdef DEBUG_MSG_INPUTBUF
#define PDEBUG_INPUTBUF(fmt, args...) printf("(%s:%d) " fmt, __func__, __LINE__, ## args)
#define PINOUT_INPUTBUF(fmt, args...) printf("(%s:%d) IO ARGUMENTS: " fmt, __func__, __LINE__, ## args)
#else
#define PDEBUG_INPUTBUF(fmt, args...)
#define PINOUT_INPUTBUF(fmt, args...) 
#endif


/*TOKEN.H*/
#define DEBUG_MSG_TOKEN

#ifdef DEBUG_MSG_TOKEN
#define PDEBUG_TOKEN(fmt, args...) printf("(%s:%d) " fmt, __func__, __LINE__, ## args)
#define PINOUT_TOKEN(fmt, args...) printf("(%s:%d) IO ARGUMENTS: " fmt, __func__, __LINE__, ## args)
#else
#define PDEBUG_TOKEN(fmt, args...)
#define PINOUT_TOKEN(fmt, args...) 
#endif


/*RING.H*/
#define DEBUG_MSG_RING

#ifdef DEBUG_MSG_RING
#define PDEBUG_RING(fmt, args...) printf("(%s:%d) " fmt, __func__, __LINE__, ## args)
#define PINOUT_RING(fmt, args...) printf("(%s:%d) IO ARGUMENTS: " fmt, __func__, __LINE__, ## args)
#else
#define PDEBUG_RING(fmt, args...)
#define PINOUT_RING(fmt, args...) 
#endif


/*MCAST.c*/
#define DEBUG_MSG_MCAST

#ifdef DEBUG_MSG_MCAST
#define PDEBUG_MCAST(fmt, args...) printf("(%s:%d) " fmt, __func__, __LINE__, ## args)
#define PINOUT_MCAST(fmt, args...) printf("(%s:%d) IO ARGUMENTS: " fmt, __func__, __LINE__, ## args)
#else
#define PDEBUG_MCAST(fmt, args...)
#define PINOUT_MCAST(fmt, args...) 
#endif


#endif    /*DEBUGMSG*/
