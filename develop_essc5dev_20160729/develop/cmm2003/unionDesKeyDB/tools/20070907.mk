#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/batchInsertDesKeysDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionDesKey_3_x_ -D _UnionSJL06_2_x_Above_ -D _UnionDesKey_4_x_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs = batchInsertDesKeys.20070907.o	\
	batchPrintDesKeys.20070907.o
makeall:	$(objs)

finalProducts:	$(null)
	mv	batchInsertDesKeys.20070907.o		$(CMM2003LIBDIR)batchInsertDesKeys.useDesKey4.x.20070907.o
	mv	batchPrintDesKeys.20070907.o		$(CMM2003LIBDIR)batchPrintDesKeys.useDesKey4.x.useSJL06_2.x.20070907.o
