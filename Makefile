CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -Wall -Wextra -pthread

all: multi-lookup

multi-lookup: multi-lookup.o util.o
		$(CC) $(LFLAGS) multi-lookup.o util.o -o multi-lookup

multi-lookup.o: multi-lookup.c multi-lookup.h
		$(CC) $(CFLAGS) multi-lookup.c

util.o: util.c util.h
		$(CC) $(CFLAGS) util.c

clean:
		rm -f *.o
		rm -f multi-lookup
		rm -f results*
