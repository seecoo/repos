HOMEDIR = $(CMM2003DIR)/unionFunSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionFunSvrMain.20080511.o			\
	unionFunSvrMainForHsmSvr.20080511.o		\
	unionFunSvrName.20080511.o			\
	unionFunSvrForKms.20080511.o			\
	unionFunSvr.20080511.o
makeall:	$(objs)


libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)unionFunSvrName.h		$(CMM2003INCDIR)
