
HOMEDIR = $(CMM2003DIR)/unionVersion
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs =	unionVersion.20061204.o

makeall:	$(objs)

finalProducts:	$(null)
	# includes
		cp $(INCDIR)/unionVersion.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
