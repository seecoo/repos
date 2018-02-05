HOMEDIR = $(CMM2003DIR)/unionREC
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	makeall		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionREC.20060825.o
makeall:	$(objs0110)

finalProducts:	$(null)
		cp $(INCDIR)/unionREC.h		$(CMM2003INCDIR)
		mv *.o				$(CMM2003LIBDIR)

