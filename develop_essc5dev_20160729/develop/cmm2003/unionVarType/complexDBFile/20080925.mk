HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall		\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionConstDefInDB.20080925.o		\
	unionProgramDefInDB.20080925.o		\
	unionArrayDefInDB.20080925.o		\
	unionEnumDefInDB.20080925.o		\
	unionSimpleTypeDefInDB.20080925.o		\
	unionStructDefInDB.20080925.o		\
	unionUnionDefInDB.20080925.o		\
	unionFunDefInDB.20080925.o		\
	unionVariableDefInDB.20080925.o		\
	unionModuleDefInDB.20080925.o		\
	unionPointerDefInDB.20080925.o		\
	unionVarTypeDefInDB.20080925.o
makeall:	$(allobjs)

finalProduct:
		cp $(INCDIR)unionDefaultVarTypeDefDir.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionConstDefFile.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionProgramDefFile.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionArrayDefFile.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionEnumDefFile.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionSimpleTypeDefFile.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionStructDefFile.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionUnionDefFile.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionFunDefFile.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionVariableDefFile.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionModuleDefFile.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionPointerDefFile.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionVarTypeDefFile.h	$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
