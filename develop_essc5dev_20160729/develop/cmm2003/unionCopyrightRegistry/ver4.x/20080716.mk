#	2003/09/19£¬Wolfgang Wang
#		01	unionLisence
#		02	genStaticLisenceAttr
#		03	mngLisence

HOMEDIR = $(CMM2003DIR)/unionCopyrightRegistry
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall				\
	finalProducts
	
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionLisence.20080716.o	\
	genLisence.20080716.o	\
	inputLisence.20080716.o	\
	verifyLisence.20080716.o
makeall:	$(allobjs)

finalProducts:	$(null)
		cp $(INCDIR)/unionLisence.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)

