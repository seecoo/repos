HOMEDIR = $(CMM2003DIR)/unionFileTransfer/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	finalProducts
	
DEFINES = -D _UnionSocket_3_x_ -D _UnionTask_3_x_ -D _UnionLogMDL_3_x_  -D _FILE_OFFSET_BITS=64 $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c

allobjs=	unionFileTransfer.20040723.o	\
		unionFileSender.20040723.o	\
		unionFileReceiver.20040723.o	\
		unionTmpFileDir.20040723.o
makeall:	$(allobjs)

finalProducts:		$(null)
	#includes
			cp $(INCDIR)unionFileTransfer.h	$(CMM2003INCDIR)
	#libs
			mv *.o				$(CMM2003LIBDIR)
