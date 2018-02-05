#	2003/09/19£¬Wolfgang Wang
#	Libs
#		01	unionModule
#		02	mngModule
#	2003/09/23
#	Libs
#		unionModule2.1.20060414.o
#		mngModule2.1.20060414.o

HOMEDIR = $(CMM2003DIR)/unionModule
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs = unionModule2.1.20060414.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)/unionModule.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
