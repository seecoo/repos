HOMEDIR = $(CMM2003DIR)/unionTcpipSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) `$(CMM2003OPENDIR)/bin/xml2-config --cflags`
DEFINES = -D _UNIX  $(OSSPEC) -D _isXMLPackage_

all:	makeall			\
	libProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	synchTCPIPSvr.20080511.o		\
	setXMLResponsePackageForError.20080511.o
makeall:	$(objs)

libProducts:	$(null)
	mv synchTCPIPSvr.20080511.o			$(CMM2003LIBDIR)synchTCPIPSvr.20080511.isXMLPackage.o
	mv setXMLResponsePackageForError.20080511.o	$(CMM2003LIBDIR)setXMLResponsePackageForError.20080511.o
	cp $(INCDIR)synchTCPIPSvr.h			$(CMM2003INCDIR)
