#	Wolfgang Wang
#	2004/07/21


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionDesKey.20040714.a	\
	unionDesKeyDBLog.20040714.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0120 =	unionDesKey.20040714.o
unionDesKey.20040714.a:	$(objs0120)
			ar $(OSSPECPACK) rv unionDesKey.20040714.a $(objs0120)

objs0220 =	unionDesKeyDBLog1.0.o
unionDesKeyDBLog.20040714.a:	$(objs0220)
			ar $(OSSPECPACK) rv unionDesKeyDBLog.20040714.a $(objs0220)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	unionDesKey.20040714.a			$(CMM2003LIBDIR)
		mv	unionDesKeyDBLog.20040714.a		$(CMM2003LIBDIR)
