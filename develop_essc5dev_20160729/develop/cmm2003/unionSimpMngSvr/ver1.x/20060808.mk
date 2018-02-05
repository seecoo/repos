HOMEDIR = $(CMM2003DIR)/unionMngSvr
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)  -D _unionCmm2003_


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	simpMngSvrStandardInteprotor.20060808.o

makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
