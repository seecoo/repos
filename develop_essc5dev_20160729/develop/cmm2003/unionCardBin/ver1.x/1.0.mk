HOMEDIR = $(CMM2003DIR)/unionCardBin
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionCardBinTBL1.0.o \
	mngCardBinTBL1.0.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)/unionCardBinTBL.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
