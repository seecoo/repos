#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I . -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_  $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionDesKeyDB.4.x.forYLSJ.20090721.o	\
	unionDesKey1x_3x.20090721.o

makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp unionDesKey1x_3x.h			$(CMM2003INCDIR)
	cp unionDesKeyDB.forYLSJ.h		$(CMM2003INCDIR)
