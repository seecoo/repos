HOMEDIR = $(CMM2003DIR)/unionTblDef
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	makeall		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = 	mngTblDef.20090421.o	\
		unionTblDef.20090421.o
makeall:	$(objs0110)

finalProducts:	$(null)
		cp $(INCDIR)/unionTblDef.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)

