# Where cgichk goes when user decides to install
INSTALLDIR=/usr/local/bin

CC      = gcc
OPTIONS = $(WINCRAP)
CFLAGS  = -O2 -s $(OPTIONS)
OUTPUT  = cgichk
SRC     = cgichk.c cgichk.h config.c misc.c whois.c net.c
CONFIG  = /etc


OBJS = cgichk.o misc.o whois.o config.o net.o

$(OUTPUT):     $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS)

install:
	cp cgichk $(INSTALLDIR)
	cp cgichk.cfg $(CONFIG)

clean:
	rm -f core *.o cgichk *.bkp *~ cgichk_`getver cgichk.h`.tar.gz *.log

dist:
	rm -f core *.o cgichk *.bkp *~
	mkdir cgichk
	cp $(SRC) cgichk
	cp README cgichk
	cp NEWS cgichk
	cp TODO cgichk
	cp CHANGES cgichk
	cp BUGS cgichk
	cp Makefile cgichk
	cp cgichk.cfg cgichk
	chown -R hac cgichk/*
	chgrp -R users cgichk/*
	tar cfz cgichk_`getver cgichk.h`.tar.gz cgichk/*
	rm -r cgichk
