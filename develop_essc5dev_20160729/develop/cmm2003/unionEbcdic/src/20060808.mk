#	2003/09/19	Wolfgang Wang
HOMEDIR = $(CMM2003DIR)/unionEbcdic
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	ebcdicAscii.20060808.o
makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)ebcdicAscii.h		$(CMM2003INCDIR)
