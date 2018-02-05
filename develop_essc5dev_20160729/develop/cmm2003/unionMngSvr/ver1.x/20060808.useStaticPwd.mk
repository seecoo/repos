HOMEDIR = $(CMM2003DIR)/unionMngSvr
INCDIR = $(HOMEDIR)/include/

SQLDIR	= $(CMM2003DIR)/unionSQL
INC2DIR	= $(SQLDIR)/include/

DESKEDBDIR	= $(CMM2003DIR)/unionDesKeyDB
INC3DIR	= $(DESKEDBDIR)/include/

#INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INC2DIR) -I $(INC3DIR) -I $(ESSC4xINCL) -I $(ESSC4xINCL)dataInterface/ 
INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INC2DIR) -I $(INC3DIR) -I $(DEVDEPINCDIR)/dataInterface/

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)  -D _unionCmm2003_ -D _useStaticPwd_


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionVerifyOperatorLogin.20090421.o		\
	unionVerifyOperatorLogin.20120521.o
makeall:	$(objs)

libProducts:	$(null)
	mv unionVerifyOperatorLogin.20090421.o		$(CMM2003LIBDIR)unionVerifyOperatorLogin.useStaticPwd.20090421.o
	mv unionVerifyOperatorLogin.20120521.o		$(CMM2003LIBDIR)unionVerifyOperatorLogin.useStaticPwd.20120521.o
