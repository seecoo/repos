HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall		\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionDefaultVarTypeDefDir.20080925.o		\
	unionConstDefFile.20080925.o		\
	unionProgramDefFile.20080925.o		\
	unionArrayDefFile.20080925.o		\
	unionEnumDefFile.20080925.o		\
	unionSimpleTypeDefFile.20080925.o		\
	unionStructDefFile.20080925.o		\
	unionUnionDefFile.20080925.o		\
	unionFunDefFile.20080925.o		\
	unionVariableDefFile.20080925.o		\
	unionModuleDefFile.20080925.o		\
	unionPointerDefFile.20080925.o		\
	unionVarTypeDefFile.20080925.o
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
