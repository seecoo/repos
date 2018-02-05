HOMEDIR = $(CMM2003DIR)/unionErrCode
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC) -D _2LenCliErrCode_ -D _maxErrCodeNum_1024_

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionErrCodeTBL.20060726.o	\
	mngErrCodeTBL.20060726.o	\
	mngErrCodeTranslater.20060726.o	\
	errCodeTranslater.20060726.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)/unionErrCodeTBL.h		$(CMM2003INCDIR)
		cp $(INCDIR)/errCodeTranslater.h	$(CMM2003INCDIR)
		mv errCodeTranslater.20060726.o	errCodeTranslater.1024ErrCode.20060726.o
		mv *.o					$(CMM2003LIBDIR)
