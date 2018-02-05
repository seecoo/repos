HOMEDIR = $(CMM2003DIR)/unionSocket/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC) -D _sendDataAfterClose_

all:	makeall		\
	finalProducts
	

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c

objs = 	UnionSocket3.0.forXZBank.o
makeall:	$(objs)

finalProducts:		$(null)
			mv UnionSocket3.0.forXZBank.o	$(CMM2003LIBDIR)UnionSocket3.0.sendDataAfterClose.forXZBank.o
