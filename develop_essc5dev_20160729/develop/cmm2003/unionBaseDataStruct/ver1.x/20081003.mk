HOMEDIR = $(CMM2003DIR)/unionBaseDataStruct/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall	\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=union2DirectionQueue.20081103.o	\
	test2DirectionQueue.20081103.o	\
	unionDefaultDataImageControl.20081103.o	\
	unionPKCS11DataImageControl.20081103.o  \
	unionDataImageInMemory.20081103.o
makeall:	$(allobjs)

finalProduct:
		cp $(INCDIR)union2DirectionQueue.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionDataImageInMemory.h	$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
