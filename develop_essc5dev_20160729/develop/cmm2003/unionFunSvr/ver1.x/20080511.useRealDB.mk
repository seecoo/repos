HOMEDIR = $(CMM2003DIR)/unionFunSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC) -D _useRealDB_

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionFunSvrMain.20080511.o		\
	unionFunSvrMainForHsmSvr.20080511.o
makeall:	$(objs)


libProducts:	$(null)
	mv unionFunSvrMain.20080511.o			$(CMM2003LIBDIR)unionFunSvrMain.useRealDB.20080511.o
	mv unionFunSvrMainForHsmSvr.20080511.o		$(CMM2003LIBDIR)unionFunSvrMainForHsmSvr.useRealDB.20080511.o
