#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/desKeyDBMonDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionTask_3_x_ -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs = directInputDesKeys.1.0.o \
       directOutputDesKeys.1.0.o \
       desKeyRecOperation.1.0.o

makeall:	$(objs)

finalProducts:	$(null)
	mv	directInputDesKeys.1.0.o		$(CMM2003LIBDIR)directInputDesKeys3.x.o
	mv	directOutputDesKeys.1.0.o		$(CMM2003LIBDIR)directOutputDesKeys3.x.o
	mv	desKeyRecOperation.1.0.o		$(CMM2003LIBDIR)desKeyRecOperation3.x.o
