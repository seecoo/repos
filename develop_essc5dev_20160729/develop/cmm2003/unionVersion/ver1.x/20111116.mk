
HOMEDIR = $(CMM2003DIR)/unionVersion
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs =	unionVersion.20111116.o		\
	unionVersion.20120919.o		\
	unionDataBaseType.o

makeall:	$(objs)

finalProducts:	$(null)
	# includes
		cp $(INCDIR)/unionVersion.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
