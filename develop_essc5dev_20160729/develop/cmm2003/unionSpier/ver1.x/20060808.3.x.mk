HOMEDIR = $(CMM2003DIR)/unionSpier
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)


all:	makeall			\
	libProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	transSpierMain.20060808.o	\
	transSpierBuf.20060808.o	\
	transSpierMon.20060808.o
makeall:	$(objs)

libProducts:	$(null)
	mv transSpierMain.20060808.o		transSpierMain.20060808.2.x_3.x.o
	mv transSpierMon.20060808.o		transSpierMon.20060808.1.x.o
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)transSpierBuf.h		$(CMM2003INCDIR)
	cp $(INCDIR)commWithTransSpier.h	$(CMM2003INCDIR)
