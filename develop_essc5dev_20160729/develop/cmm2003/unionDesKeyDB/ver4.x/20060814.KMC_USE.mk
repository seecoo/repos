#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _KMC_USE_ -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_  $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionDesKey.4.x.20060814.o

makeall:	$(objs)

libProducts:	$(null)
	mv unionDesKey.4.x.20060814.o					$(CMM2003LIBDIR)/unionDesKey.4.x.20060814.KMC_USE.o
