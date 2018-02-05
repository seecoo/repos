#	Wolfgang Wang
#	2004/11/20


HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _UnionPK_4_x_ -D _UnionPK_3_x_ $(OSSPEC) -D _KDBClient_

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionPKDB.4.x.20060822.o
makeall:	$(objs)

libProducts:	$(null)
	mv unionPKDB.4.x.20060822.o	$(CMM2003LIBDIR)unionPKDB.4.x.DBClient.20060822.o
	cp $(INCDIR)unionPK.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionPKDB.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionPKDBLog.h	$(CMM2003INCDIR)
