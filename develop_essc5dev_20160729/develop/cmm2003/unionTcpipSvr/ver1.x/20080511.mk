HOMEDIR = $(CMM2003DIR)/unionTcpipSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	tcpipSvrMain.20080511.o			\
	tcpipSvrMainForHsmSvr.20080511.o	\
	shortConnTcpipSvrMain.20080511.o	\
	shortConnTcpipSvrMainForShared.20080511.o	\
	shortConnTcpipSvrMainForHsmSvr.20080511.o	\
	unionServiceErrCounter.20080511.o	\
	tcpipSvrRunningEnv.default.o	\
	tcpipSvrRunningEnv.null.o	\
	synchTCPIPSvr.20080511.o	\
	defaultRunningEnv.20080511.o	\
	defaultRunningEnv.withoutFreeRes.20080511.o	\
	synchTCPIPSvr.withoutLen.20080511.o	\
	tcpipSvrTaskName.default.o	\
	simuFunSvr.1.0.o
makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)synchTCPIPSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionServiceErrCounter.h	$(CMM2003INCDIR)
