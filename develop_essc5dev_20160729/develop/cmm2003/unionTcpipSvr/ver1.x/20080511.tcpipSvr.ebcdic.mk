HOMEDIR = $(CMM2003DIR)/unionTcpipSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  -D _client_use_ebcdic_ $(OSSPEC)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC) -D _client_use_ebcdic_

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	synchTCPIPSvr.20080511.o
makeall:	$(objs)

libProducts:	$(null)
	mv synchTCPIPSvr.20080511.o		$(CMM2003LIBDIR)/synchTCPIPSvr.ebcdic.20080511.o
	cp $(INCDIR)synchTCPIPSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionServiceErrCounter.h	$(CMM2003INCDIR)
