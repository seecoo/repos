#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/generateKeyComponentDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

DEF = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	keyPrintFormat.20060614.a	\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0120 =	keyPrintFormat.20060614.o
keyPrintFormat.20060614.a:	$(objs0120)
			ar $(OSSPECPACK) rv keyPrintFormat.20060614.a $(objs0120)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	keyPrintFormat.20060614.a			$(CMM2003LIBDIR)
