HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall				\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs =	hsmSvrMain.20120521.o 
	
makeall:	$(objs)

libProducts:	$(null)
	mv *.o						$(CMM2003LIBDIR)

