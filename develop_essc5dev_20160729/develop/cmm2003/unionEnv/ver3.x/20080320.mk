HOMEDIR = $(CMM2003DIR)/mngEnv
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs = mngEnv.20080320.o
makeall:	$(allobjs)

finalProducts:	$(null)
		mv $(LIBDIR)*.o		$(CMM2003LIBDIR)
