#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/desKeyDBMonDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs = desKeyDBMon.20070925.o
makeall:	$(objs)

finalProducts:	$(null)
	mv	desKeyDBMon.20070925.o		$(CMM2003LIBDIR)desKeyDBMon.useDesKey3.x.20070925.o
