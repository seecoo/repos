HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR)


DEFINES = $(OSSPEC)

all:	makeall		\
	mvToLib	
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs1=outputKeyWordDef.20080925.o		\
	unionGenIncFromTBL.20080925.o		\
	unionGenIncFromTBLMain.20080925.o		\
	unionAutoGenFileFunGrp.20080925.o
allobjs2=unionGenFunTestProgram.20080925.o	\
	unionGenFunListTestProgram.20080925.o
commobjs=unionVarAssignment.20080925.o
makeall: 	$(allobjs1) $(allobjs2) $(commobjs)

mvToLib:
		cp $(INCDIR)unionVarAssignment.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionAutoGenFileFunGrp.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionGenFunTestProgram.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionGenFunListTestProgram.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionGenIncFromTBL.h		$(CMM2003INCDIR)
		mv *.o						$(CMM2003LIBDIR)
