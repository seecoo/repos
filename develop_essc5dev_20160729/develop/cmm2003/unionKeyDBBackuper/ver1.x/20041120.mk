#	Wolfgang Wang
#	2004/11/20


DEF = $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionKeyDBBackuper/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	keyDBBackuper.20041120.a		\
	keyDBBackuperMain.20041120.a		\
	finalProducts				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0220 =	keyDBBackuperMain.20041120.o
keyDBBackuperMain.20041120.a:	$(objs0220)
			ar $(OSSPECPACK) rv keyDBBackuperMain.20041120.a $(objs0220)

objs0420 =	keyDBBackuper.20041120.o
keyDBBackuper.20041120.a:	$(objs0420)
			ar $(OSSPECPACK) rv keyDBBackuper.20041120.a $(objs0420)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	keyDBBackuperMain.20041120.a		$(CMM2003LIBDIR)
		mv	keyDBBackuper.20041120.a		$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)keyDBBackuper.h		$(CMM2003INCDIR)
