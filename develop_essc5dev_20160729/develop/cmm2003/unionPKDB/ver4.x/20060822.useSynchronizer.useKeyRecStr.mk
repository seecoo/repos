#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _UnionPK_4_x_ -D _UnionPK_3_x_ $(OSSPEC) -D _useKeySynchronizer_ -D _useKeyRecStr_

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionPKDBLog.4.x.20060822.o
makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)unionPKDBLog.4.x.useSynchronizer.useKeyRecStr.20060822.o
