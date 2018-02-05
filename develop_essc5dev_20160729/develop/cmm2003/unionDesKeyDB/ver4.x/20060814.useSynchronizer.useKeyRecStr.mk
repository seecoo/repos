#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_ $(OSSPEC) -D _useKeySynchronizer_ -D _useKeyRecStr_

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionDesKeyDBLog.4.x.20060814.o
makeall:	$(objs)

libProducts:	$(null)
	mv unionDesKeyDBLog.4.x.20060814.o		$(CMM2003LIBDIR)unionDesKeyDBLog.4.x.useKeySynchronizer.useKeyRecStr.20060814.o
