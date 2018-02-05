HOMEDIR = $(CMM2003DIR)/unionSocket/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC) -D _sendDataAfterClose_

all:	makeall		\
	finalProducts
	

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c

objs = 	UnionSocket3.0.o	\
	UnionTCPIPSvr3.0.o
makeall:	$(objs)

finalProducts:		$(null)
			mv UnionSocket3.0.o	$(CMM2003LIBDIR)UnionSocket3.0.sendDataAfterClose.o
			mv *.o			$(CMM2003LIBDIR)
