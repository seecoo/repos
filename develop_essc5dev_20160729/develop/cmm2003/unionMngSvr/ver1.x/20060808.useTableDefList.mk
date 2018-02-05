HOMEDIR = $(CMM2003DIR)/unionMngSvr
INCDIR = $(HOMEDIR)/include/

SQLDIR	= $(CMM2003DIR)/unionSQL
INC2DIR	= $(SQLDIR)/include/

DESKEDBDIR	= $(CMM2003DIR)/unionDesKeyDB
INC3DIR	= $(DESKEDBDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INC2DIR) -I $(INC3DIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _useTableDefList_

all:	makeall			\
	libProducts

# DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	localDBComplexSvr.20060808.o

makeall:	$(objs)

libProducts:	$(null)
	mv localDBComplexSvr.20060808.o		$(CMM2003LIBDIR)localDBComplexSvr.useTableDefList.20060808.o
