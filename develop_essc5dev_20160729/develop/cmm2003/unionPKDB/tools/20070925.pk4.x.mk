#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionPKDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionPKDB_2_x_ -D _UnionPK_3_x_ -D _UnionPK_4_x_ -D _UnionSJL06_2_x_Above_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs = pkDBMon.20070925.o
makeall:	$(objs)

finalProducts:	$(null)
	mv	pkDBMon.20070925.o		$(CMM2003LIBDIR)pkDBMon.usePK4.x.20070925.o
