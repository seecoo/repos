HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall		\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionModuleDef.20080925.o	\
	unionStructDef.20080925.o	\
	unionUnionDef.20080925.o	\
	unionPointerDef.20080925.o		\
	unionSimpleTypeDef.20080925.o		\
	unionConstDef.20080925.o		\
	unionArrayDef.20080925.o		\
	unionVariableDef.20080925.o		\
	unionFunDef.20080925.o		\
	unionEnumDef.20080925.o		\
	unionVarTypeDef.20080925.o		\
	unionVarValue.20080925.o		\
	unionProgramDef.20080925.o		\
	unionCalcuOperator.20080925.o		\
	unionDesignKeyWord.20080925.o		\
	unionVarDef.20080925.o
makeall:	$(allobjs)

libs=	$(CMM2003LIBDIR)UnionLog1.2.o		\
	$(CMM2003LIBDIR)externLogFun.20080717.o	\
	$(CMM2003LIBDIR)UnionStr1.2.a		\
	$(CMM2003LIBDIR)unionInput1.0.o
keyWord:	$(allobjs)
	cc -o keyWord $(allobjs) $(libs) $(OSSPECLIBS)

finalProduct:
		cp $(INCDIR)unionCalcuOperator.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionAutoGenFileFunGrp.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionVariableDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionStructDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionUnionDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionEnumDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionArrayDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionSimpleTypeDef.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionPointerDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionFunDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionVarDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionVarTypeDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionVarValue.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionModuleDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionProgramDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionConstDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionGenFunTestProgram.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionDesignKeyWord.h	$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
