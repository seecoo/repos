#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/batchEditDesKeysDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	batchEditDesKeys.desKey2.0.20050616.forGDB.a	\
	batchEditDesKeys.desKey2.0.20050616.a	\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0220 =	batchEditDesKeys.20050616.forGDB.o
batchEditDesKeys.desKey2.0.20050616.forGDB.a:	batchEditDesKeys.20050616.c
		cc -c $(INCL) $(DEF) -D _GangdongDevelopmentBank_ batchEditDesKeys.20050616.c
		mv batchEditDesKeys.20050616.o batchEditDesKeys.20050616.forGDB.o
		ar $(OSSPECPACK) rv batchEditDesKeys.desKey2.0.20050616.forGDB.a $(objs0220)

# Libs 01
objs0120 =	batchEditDesKeys.20050616.o
batchEditDesKeys.desKey2.0.20050616.a:	$(objs0120)
			ar $(OSSPECPACK) rv batchEditDesKeys.desKey2.0.20050616.a $(objs0120)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	batchEditDesKeys.desKey2.0.20050616.a			$(CMM2003LIBDIR)
		mv	batchEditDesKeys.desKey2.0.20050616.forGDB.a		$(CMM2003LIBDIR)
