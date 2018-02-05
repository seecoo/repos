#	Wolfgang Wang
#	2004/11/20


DEF = $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionDesKeyDB3.x.NoIndex.20060421.a		\
	finalProducts				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0420 =	unionDesKeyDB3.x.NoIndex.20060421.o unionDesKeyDBLog3.0.20060414.o unionDesKey3.0.20041125.o
unionDesKeyDB3.x.NoIndex.20060421.a:	$(objs0420)
			ar $(OSSPECPACK) rv unionDesKeyDB3.x.NoIndex.20060421.a $(objs0420)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
		mv	unionDesKeyDB3.x.NoIndex.20060421.a		$(CMM2003LIBDIR)
