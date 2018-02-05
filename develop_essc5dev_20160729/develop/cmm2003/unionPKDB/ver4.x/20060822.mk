#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _UnionPK_4_x_ -D _UnionPK_3_x_ $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionPK.4.x.20060822.o	\
	unionPKDB.4.x.20060822.o	\
	unionPKDBLog.4.x.20060822.o	\
	mngPKDB.4.x.20060822.o	\
	mngPKDBLog.4.x.20060822.o	\
	unionPK.4.x.20121101.o

makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)unionPK.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionPKDB.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionPKDBLog.h		$(CMM2003INCDIR)
