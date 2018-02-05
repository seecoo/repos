#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/desKeyDBMonDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionTask_3_x_ -D _UnionPKDB_2_x_ -D _UnionPK_3_x_ -D _UnionPK_4_x_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs = directOutputPKs.1.0.o

makeall:	$(objs)

finalProducts:	$(null)
	mv	directOutputPKs.1.0.o		$(CMM2003LIBDIR)directOutputPKs4.x.o
