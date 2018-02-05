#		unionModule2.1.20071026.o

HOMEDIR = $(CMM2003DIR)/unionModule
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs = unionModule2.1.20071026.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)/unionModule.h		$(CMM2003INCDIR)
		cp unionModule2.1.20071026.o		$(CMM2003LIBDIR)unionModule.2.1.20071026.o
		mv *.o					$(CMM2003LIBDIR)
