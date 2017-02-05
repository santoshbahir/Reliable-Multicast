# To add new source files to the compile, take the file's name 
# (e.g.: myfile.c), replace the ".c" with ".o", and add an OBJECTS
# line for it below:

CC=gcc
CFLAGS=-g -ggdb -Wall -Werror
WINDLIB=mcast.a

TESTS=mcast
TESTS+=start_mcast

OBJECTS=mcast.o start_mcast.o
OBJECTS+=misc.o inputbuf.o outputbuf.o token.o recv_dbg.o nrtime.o ring.o

all: $(TESTS)

$(WINDLIB): $(WINDLIB)($(OBJECTS))
$(WINDLIB)(%.o): %.c

$(TESTS): %: %.o $(WINDLIB)
	$(CC) $(CFLAGS) -o $@ $@.o $(WINDLIB)

clean:
	-rm -f *.a *.o *~ 
	rm $(TESTS)
