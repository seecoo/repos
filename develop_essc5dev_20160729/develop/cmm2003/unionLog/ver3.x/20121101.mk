#UnionLog.20121101.o

COMMDIR = $(UNIONLIBDIR)/
COMMLIBDIR = $(COMMDIR)lib/
COMMINCDIR = $(COMMDIR)include/

HOMEDIR = $(CMM2003DIR)/unionLog
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(COMMINCDIR)
DEFINES = $(OSSPEC)

all:	makeall		\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	UnionLog.20121101.o	

makeall:        $(objs)

finalProduct:	$(null)
		cp $(INCDIR)UnionLog.h		$(COMMINCDIR)
		mv *.o		$(CMM2003LIBDIR)
