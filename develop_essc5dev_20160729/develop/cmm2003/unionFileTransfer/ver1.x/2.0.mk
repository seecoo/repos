HOMEDIR = $(CMM2003DIR)/unionFileTransfer/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	finalProducts
	
DEFINES = -D _UnionSocket_3_x_ -D _UnionTask_3_x_ -D _UnionLogMDL_3_x_  -D _unionFileTransfer_2_x_ -D _FILE_OFFSET_BITS=64 $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c

allobjs=	unionFileTransfer.20040723.o	\
		unionFileSender.20040723.o	\
		unionFileReceiver.20040723.o	\
		unionFileTransferService.20040723.o	\
		unionIO.20120521.o
makeall:	$(allobjs)

finalProducts:		$(null)
	#includes
			cp $(INCDIR)unionFileTransfer.h	$(CMM2003INCDIR)
	#libs
			mv unionFileTransfer.20040723.o		$(CMM2003LIBDIR)unionFileTransfer.2.0.o
			mv unionFileSender.20040723.o		$(CMM2003LIBDIR)unionFileSender.2.0.o
			mv unionFileReceiver.20040723.o		$(CMM2003LIBDIR)unionFileReceiver.2.0.o
			mv unionFileTransferService.20040723.o  $(CMM2003LIBDIR)unionFileTransferService.20040723.2.0.o
			mv unionIO.20120521.o			$(CMM2003LIBDIR)unionIO.20120521.o
