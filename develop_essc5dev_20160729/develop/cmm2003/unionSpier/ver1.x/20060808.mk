HOMEDIR = $(CMM2003DIR)/unionSpier
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC) -D _transSpier_2_x_


all:	makeall			\
	libProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	mngTransSpierBuf.20060808.o	\
	transSpierBuf.defFile.20060808.o	\
	transSpierMainToLogFile.20060808.o	\
	transSpierMain.20060808.o	\
	transSpierBuf.20060808.o	\
	transSpier.20060808.o	\
	commWithTransSpier.20060808.o
makeall:	$(objs)

libProducts:	$(null)
	mv transSpierBuf.20060808.o		transSpierBuf.20060808.2.x.o
	mv transSpierMainToLogFile.20060808.o	transSpierMainToLogFile.20060808.2.x.o
	mv transSpierMain.20060808.o		transSpierMain.20060808.2.x.o
	mv transSpier.20060808.o		transSpier.20060808.2.x.o
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)transSpierBuf.h		$(CMM2003INCDIR)
	cp $(INCDIR)commWithTransSpier.h	$(CMM2003INCDIR)
