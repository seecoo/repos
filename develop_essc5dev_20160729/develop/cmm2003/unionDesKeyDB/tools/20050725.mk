#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/genDesKeyDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionDesKey_3_x_ -D _UnionSJL06_2_x_Above_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genDesKey.desKey3.x.20050725.a	\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0120 =	genDesKey.20050725.o
genDesKey.desKey3.x.20050725.a:	$(objs0120)
			ar $(OSSPECPACK) rv genDesKey.desKey3.x.20050725.a $(objs0120)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	genDesKey.desKey3.x.20050725.a		$(CMM2003LIBDIR)
