HOMEDIR = $(CMM2003DIR)/unionMngSvr
INCDIR = $(HOMEDIR)/include/

SQLDIR	= $(CMM2003DIR)/unionSQL
INC2DIR	= $(SQLDIR)/include/

DESKEDBDIR	= $(CMM2003DIR)/unionDesKeyDB
INC3DIR	= $(DESKEDBDIR)/include/

#INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INC2DIR) -I $(INC3DIR) -I $(ESSC4xINCL)
INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INC2DIR) -I $(INC3DIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)  -D _unionCmm2003_


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	fileSvrStandardInteprotor.20060808.o	\
	fileSvrService.20060808.o

makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
