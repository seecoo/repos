HOMEDIR = $(CMM2003DIR)/unionWorkingDir
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = unionWorkingDir.20080724.o
makeall:	$(objs0130)


finalProduct:	$(null)
		cp $(INCDIR)unionWorkingDir.h		$(CMM2003INCDIR)
		mv *.o	$(CMM2003LIBDIR)
