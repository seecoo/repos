HOMEDIR = $(CMM2003DIR)/unionSQL
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _withFileLock_

all:	makeall			\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130=unionRecFile.20080724.o
makeall:	$(objs0130)


finalProduct:	$(null)
		cp $(INCDIR)unionRecFile.h			$(CMM2003INCDIR)
		mv unionRecFile.20080724.o			unionRecFile.20080724.lockFile.o
		mv *.o	$(CMM2003LIBDIR)
