HOMEDIR = $(CMM2003DIR)/unionMsgQueue
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionMsgQueue.20070615.o	\
	msgQueueMon.20070615.o
makeall:	$(allobjs)
finalProduct:	$(null)
	cp $(INCDIR)unionMsgQueue.h		$(CMM2003INCDIR)
	mv *.o					$(CMM2003LIBDIR)
