#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/generateKeyComponentDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionSJL06_2_x_Above_

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	generateKeyComponent.SJL06_2_x.20050718.a	\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0120 =	generateKeyComponent.20050718.o
generateKeyComponent.SJL06_2_x.20050718.a:	$(objs0120)
			ar $(OSSPECPACK) rv generateKeyComponent.SJL06_2_x.20050718.a $(objs0120)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	generateKeyComponent.SJL06_2_x.20050718.a			$(CMM2003LIBDIR)
