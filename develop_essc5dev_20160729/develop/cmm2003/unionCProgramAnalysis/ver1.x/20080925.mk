HOMEDIR = $(CMM2003DIR)/unionCProgramAnalysis/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall	\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionCProgramPhase.20080925.o	\
	unionReadStructFromCFile.20080925.o
makeall:	$(allobjs)

finalProduct:
		cp $(INCDIR)unionCProgramPhase.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionReadStructFromCFile.h	$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
