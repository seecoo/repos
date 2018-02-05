HOMEDIR = $(CMM2003DIR)/unionFileDeal
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionFileDeal_tar.1.0.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)/unionFileDeal.h	$(CMM2003INCDIR)
		mv *.o				$(CMM2003LIBDIR)
