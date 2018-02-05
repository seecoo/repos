#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionDesKeyDB.20041120.a		\
	unionDesKeyDBLog.20041120.a		\
	finalProducts				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0220 =	unionDesKeyDBLog.20041120.o
unionDesKeyDBLog.20041120.a:	$(objs0220)
			ar $(OSSPECPACK) rv unionDesKeyDBLog.20041120.a $(objs0220)

objs0420 =	unionDesKeyDB.20041120.o unionDesKeyDBLog.20041120.o unionDesKey.20040714.o
unionDesKeyDB.20041120.a:	$(objs0420)
			ar $(OSSPECPACK) rv unionDesKeyDB.20041120.a $(objs0420)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	unionDesKeyDBLog.20041120.a		$(CMM2003LIBDIR)
		mv	unionDesKeyDB.20041120.a		$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)unionDesKeyDBLog.h		$(CMM2003INCDIR)
