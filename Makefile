CC=gcc
LINK=gcc

LIBDIR=/usr/lib
INCDIR=/usr/include

CFLAGS= -g -Wall -I. -I$(INCPATH)
LIBS=
LFLAGS=

.SUFFIXES:.c

.c.o	:
	$(CC) -c $(CFLAGS) $(INCDIR) $<

clean :
	rm -f *.o core

ini.o : ini.h ini.c

config.o : config.h config.c

nremind.o : nremind.c

nremind : nremind.o config.o ini.o
	$(LINK) $(LFLAGS) -o $@ ini.o config.o nremind.o -lnewt


