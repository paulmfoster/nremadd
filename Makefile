CC=gcc
LINK=gcc

LIBDIR=/usr/lib
INCDIR=/usr/include

CFLAGS= -g -Wall -I. -I$(INCPATH)
LIBS=
LFLAGS=

DESTDIR=/usr/local/bin

.SUFFIXES:.c

.c.o	:
	$(CC) -c $(CFLAGS) $(INCDIR) $<

all: nremadd

clean :
	rm -f *.o core

ini.o : ini.h ini.c

config.o : config.h config.c

nremadd.o : nremadd.c

nremadd : nremadd.o 
	$(LINK) $(LFLAGS) -o $@ nremadd.o -lnewt

install: nremadd
	sudo cp nremadd $(DESTDIR)
	sudo cp remadd $(DESTDIR)

.PHONY: all clean install

