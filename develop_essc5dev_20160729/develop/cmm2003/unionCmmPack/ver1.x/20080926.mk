HOMEDIR = $(CMM2003DIR)/unionCmmPack/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall	\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionCmmPackData.20080926.o	\
	unionCmmPackHelp.20080926.o	\
	unionServicePackage.20080926.o
makeall:	$(allobjs)

finalProduct:
		cp $(INCDIR)unionCmmPackData.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionServicePackage.h	$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
