#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionDesKeyDB3.x.20060414.a		\
	finalProducts				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0420 =	unionDesKeyDB3.0.20041125.o unionDesKeyDBLog3.0.20060414.o unionDesKey3.0.20041125.o
unionDesKeyDB3.x.20060414.a:	$(objs0420)
			ar $(OSSPECPACK) rv unionDesKeyDB3.x.20060414.a $(objs0420)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
		mv	unionDesKeyDB3.x.20060414.a		$(CMM2003LIBDIR)
