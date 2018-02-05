HOMEDIR = $(CMM2003DIR)/unionErrCode
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC) -D _2LenCliErrCode_ -D _maxErrCodeNum_1024_  -D _useComplexDB_ -D _withSoftErrCodeRemark_

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	errCodeTranslater.20060828.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)/errCodeTranslater.h	$(CMM2003INCDIR)
		mv errCodeTranslater.20060828.o	errCodeTranslater.1024ErrCode.1.x.onComplexDB.20060828.o
		mv *.o					$(CMM2003LIBDIR)
