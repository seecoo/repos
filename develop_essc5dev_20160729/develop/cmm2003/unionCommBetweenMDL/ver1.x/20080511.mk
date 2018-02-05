HOMEDIR = $(CMM2003DIR)/unionCommBetweenMDL/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionMDLID.20080511.o	\
	unionCommBetweenMDL.20080511.o	\
	unionCommBetweenMDL.20080511.forYLSJ.o
makeall:	$(objs)

libProducts:	$(null)
	mv *.o						$(CMM2003LIBDIR)
	cp $(INCDIR)unionMDLID.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionCommBetweenMDL.h		$(CMM2003INCDIR)
