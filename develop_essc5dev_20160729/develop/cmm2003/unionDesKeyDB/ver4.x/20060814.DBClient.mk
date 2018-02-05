#	Wolfgang Wang
#	2004/11/20


HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_ $(OSSPEC) -D _KDBClient_

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionDesKeyDB.4.x.20060814.o
makeall:	$(objs)

libProducts:	$(null)
	mv unionDesKeyDB.4.x.20060814.o		$(CMM2003LIBDIR)unionDesKeyDB.4.x.DBClient.20060814.o
	cp $(INCDIR)unionDesKey.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionDesKeyDB.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionDesKeyDBLog.h		$(CMM2003INCDIR)
