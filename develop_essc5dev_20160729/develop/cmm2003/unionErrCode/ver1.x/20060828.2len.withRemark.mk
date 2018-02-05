HOMEDIR = $(CMM2003DIR)/unionErrCode
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC) -D _2LenCliErrCode_ -D _128ErrCode_ -D _withSoftErrCodeRemark_

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionErrCodeTBL.20060828.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)/unionErrCodeTBL.h		$(CMM2003INCDIR)
		mv unionErrCodeTBL.20060828.o		unionErrCodeTBL.128ErrCode.2Len.withRemark.20060828.o
		mv *.o					$(CMM2003LIBDIR)
