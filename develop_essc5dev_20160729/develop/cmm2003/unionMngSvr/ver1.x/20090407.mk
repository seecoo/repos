HOMEDIR = $(CMM2003DIR)/unionMngSvr
INCDIR = $(HOMEDIR)/include/

SQLDIR	= $(CMM2003DIR)/unionSQL
INC2DIR	= $(SQLDIR)/include/

DESKEDBDIR	= $(CMM2003DIR)/unionDesKeyDB
INC3DIR	= $(DESKEDBDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INC2DIR) -I $(INC3DIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _noUseUserSpecOperation_

all:	makeall			\
	libProducts

# DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	localDBComplexSvr.20060808.o

makeall:	$(objs)

libProducts:	$(null)
	mv localDBComplexSvr.20060808.o		$(CMM2003LIBDIR)localDBComplexSvr.noUseUserSpecOperation.20060808.o
	cp $(INCDIR)mngSvrFileSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionResID.h		$(CMM2003INCDIR)
	cp $(INCDIR)operationControl.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrServicePackage.h	$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrCommProtocol.h	$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrTeller.h		$(CMM2003INCDIR)
	cp $(INCDIR)simuMngSvrLocally.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrRecTransferred.h	$(CMM2003INCDIR)
	cp $(INCDIR)unionDBConf.h		$(CMM2003INCDIR)
