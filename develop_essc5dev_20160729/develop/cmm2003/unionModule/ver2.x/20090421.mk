#		unionModule2.1.20071026.o

HOMEDIR = $(CMM2003DIR)/unionModule
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _useComplexDB_

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs = unionModule2.1.20090421.o	\
	unionModule2.1.20090421.defFile.o
makeall:	$(objs)

finalProducts:  $(null)
	cp $(INCDIR)/unionModule.h              $(CMM2003INCDIR)
	mv *.o                                  $(CMM2003LIBDIR)
