HOMEDIR = $(CMM2003DIR)/unionTcpipSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) `$(CMM2003OPENDIR)/bin/xml2-config --cflags`
DEFINES = -D _UNIX  $(OSSPEC) -D _isXMLPackage_ -D _maxSizeOfClientMsg_8192_

all:	makeall			\
	libProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	synchTCPIPSvr.20080511.o
makeall:	$(objs)

libProducts:	$(null)
	mv synchTCPIPSvr.20080511.o			$(CMM2003LIBDIR)synchTCPIPSvr.20080511.8192.isXMLPackage.o
	cp $(INCDIR)synchTCPIPSvr.h			$(CMM2003INCDIR)
