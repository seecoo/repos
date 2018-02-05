HOMEDIR = $(CMM2003DIR)/unionFunSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC) -D _sharedHsmTaskLog_

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionFunSvr.20080511.o	\
	unionFunSvrForKms.20080511.o

makeall:	$(objs)

libProducts:	$(null)
	mv unionFunSvr.20080511.o		$(CMM2003LIBDIR)unionFunSvr.sharedHsmTaskLog.20080511.o
	mv unionFunSvrForKms.20080511.o		$(CMM2003LIBDIR)unionFunSvrForKms.sharedHsmTaskLog.20080511.o
	cp $(INCDIR)unionFunSvrName.h		$(CMM2003INCDIR)
