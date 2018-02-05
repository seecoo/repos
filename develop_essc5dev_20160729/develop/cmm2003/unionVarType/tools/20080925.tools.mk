HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall		\
	genProductGeneratedTimeLib	\
	mvToLib		\
	keyWord		\
	keyWordInDB	\
	genIncFromTBL	\
	finalProduct
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

# 生成时间库
genProductGeneratedTimeLib:     $(null)
	genProductGenerateTime


libs=	$(CMM2003LIBDIR)unionDesignKeyWord.20080925.o	\
	$(CMM2003LIBDIR)UnionLog1.2.o		\
	$(CMM2003LIBDIR)unionVarValue.20080925.o	\
	$(CMM2003LIBDIR)unionVarTypeDef.20080925.o	\
	$(CMM2003LIBDIR)unionVarDef.20080925.o	\
	$(CMM2003LIBDIR)unionStructDef.20080925.o	\
	$(CMM2003LIBDIR)unionUnionDef.20080925.o	\
	$(CMM2003LIBDIR)unionProgramDef.20080925.o	\
	$(CMM2003LIBDIR)unionModuleDef.20080925.o	\
	$(CMM2003LIBDIR)unionEnumDef.20080925.o	\
	$(CMM2003LIBDIR)unionVariableDef.20080925.o	\
	$(CMM2003LIBDIR)unionPointerDef.20080925.o	\
	$(CMM2003LIBDIR)unionArrayDef.20080925.o	\
	$(CMM2003LIBDIR)unionConstDef.20080925.o	\
	$(CMM2003LIBDIR)unionSimpleTypeDef.20080925.o	\
	$(CMM2003LIBDIR)unionFunDef.20080925.o	\
	$(CMM2003LIBDIR)unionDefaultVarTypeDefDir.20080925.o	\
	$(CMM2003LIBDIR)externLogFun.20080717.o	\
	$(CMM2003LIBDIR)UnionStr1.2.a		\
	$(CMM2003LIBDIR)unionWorkingDir.20080724.o	\
	$(CMM2003LIBDIR)unionCalcuOperator.20080925.o	\
	$(CMM2003LIBDIR)unionDataImageInMemory.20081103.o	\
	$(CMM2003LIBDIR)unionDefaultDataImageControl.20081103.o	\
	$(CMM2003LIBDIR)unionLockTBL.1.0.o			\
	$(CMM2003LIBDIR)unionOSLockMachinism.forUnix.useMsgQueue.1.0.o\
	$(CMM2003LIBDIR)unionIndexTBL.20060802.a		\
	$(CMM2003LIBDIR)unionModule2.1.20071026.o	\
	$(CMM2003LIBDIR)unionEnv.20050804.4096.o	\
	$(CMM2003LIBDIR)union2DirectionQueue.20081103.o	\
	$(CMM2003LIBDIR)unionVersion.20061204.o	\
	unionProductGenerateTime.a	\
	$(CMM2003VERDIR)unionPredefinedVersion1.0.a	\
	$(CMM2003VERDIR)unionEffectiveDays.36500.a	\
	$(CMM2003LIBDIR)unionLibName3.0.o	\
	$(CMM2003LIBDIR)unionInput1.0.o	\
	$(CMM2003LIBDIR)unionSystemTime1.0.o

fileLibs=$(CMM2003LIBDIR)unionVarTypeDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionSimpleTypeDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionModuleDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionProgramDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionUnionDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionEnumDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionVariableDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionStructDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionPointerDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionFunDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionArrayDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionConstDefFile.20080925.o
keyWord:	$(allobjs1)
	cc -o keyWord $(OSSPECLINK) $(OSSPECLIBS) $(allobjs1) $(fileLibs) $(libs)

dbLibs=	$(CMM2003LIBDIR)unionVarTypeDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionSimpleTypeDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionModuleDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionProgramDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionUnionDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionEnumDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionVariableDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionStructDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionPointerDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionFunDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionArrayDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionConstDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionComplexDBCommon.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBObjectFileName.20080801.o	\
	$(CMM2003LIBDIR)unionComplexDBObjectSql.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBObjectDef.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBObjectChildDef.20080801.o	\
	$(CMM2003LIBDIR)unionComplexDBPrimaryKey.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBUniqueKey.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBRecordValue.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBRecordParents.20080801.o	\
	$(CMM2003LIBDIR)unionComplexDBRecordChild.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBObject.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBRecord.20080801.o		\
	$(CMM2003LIBDIR)unionRecFile.20080724.o	\
	$(CMM2003LIBDIR)unionRec.20080724.o
keyWordInDB:	$(allobjs1)
	cc -o keyWordInDB $(OSSPECLINK) $(OSSPECLIBS) $(allobjs1) $(libs) $(dbLibs) 

allobjs3=$(CMM2003LIBDIR)unionGenIncFromTBLMain.20080925.o	\
	$(CMM2003LIBDIR)unionGenIncFromTBL.20080925.o
genIncFromTBL:	$(allobjs3)
	cc -o genIncFromTBL $(OSSPECLINK) $(OSSPECLIBS) $(allobjs3) $(libs) $(dbLibs) 

mvToLib:
		cp $(INCDIR)unionVarAssignment.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionAutoGenFileFunGrp.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionGenFunTestProgram.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionGenFunListTestProgram.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionGenIncFromTBL.h		$(CMM2003INCDIR)
		cp *.o						$(CMM2003LIBDIR)
finalProduct:
		mv keyWord					$(CMM2003BINDIR)
		mv keyWordInDB					$(CMM2003BINDIR)
		mv genIncFromTBL				$(CMM2003BINDIR)
		rm *.o *.a
