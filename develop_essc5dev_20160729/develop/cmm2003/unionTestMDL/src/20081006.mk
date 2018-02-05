HOMEDIR = $(CMM2003DIR)/unionTestMDL/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall		\
	mvToLib		\
	genProductGeneratedTimeLib	\
	testFun		\
	testFunInDB	\
	testFunList	\
	finalProduct

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionTestConf.20081006.o	\
	unionGenerateMakeFile.20081006.o
objs1=unionFunTestMain.20081006.o
objs2=testFunListMain.20081006.o
makeall:	$(allobjs) $(objs1) $(objs2)

mvToLib:
		cp $(INCDIR)unionTestConf.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionGenerateMakeFile.h	$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)

commLibs=$(CMM2003LIBDIR)unionTestConf.20081006.o		\
	$(CMM2003LIBDIR)unionGenerateMakeFile.20081006.o
	
libs001=$(CMM2003LIBDIR)unionFunTestMain.20081006.o	\
	$(commLibs)

libs002=$(CMM2003LIBDIR)testFunListMain.20081006.o	\
	$(commLibs)

# 生成时间库
genProductGeneratedTimeLib:     $(null)
	genProductGenerateTime

libs=	$(CMM2003LIBDIR)UnionLog1.2.withoutTimeFun.o		\
	$(CMM2003LIBDIR)unionGenFunTestProgram.20080925.o	\
	$(CMM2003LIBDIR)unionGenFunListTestProgram.20080925.o	\
	$(CMM2003LIBDIR)unionVarAssignment.20080925.o	\
	$(CMM2003LIBDIR)unionVarValue.20080925.o	\
	$(CMM2003LIBDIR)unionVarTypeDef.20080925.o	\
	$(CMM2003LIBDIR)unionVarDef.20080925.o	\
	$(CMM2003LIBDIR)unionStructDef.20080925.o	\
	$(CMM2003LIBDIR)unionEnumDef.20080925.o	\
	$(CMM2003LIBDIR)unionVariableDef.20080925.o	\
	$(CMM2003LIBDIR)unionPointerDef.20080925.o	\
	$(CMM2003LIBDIR)unionArrayDef.20080925.o	\
	$(CMM2003LIBDIR)unionConstDef.20080925.o	\
	$(CMM2003LIBDIR)unionSimpleTypeDef.20080925.o	\
	$(CMM2003LIBDIR)unionCalcuOperator.20080925.o	\
	$(CMM2003LIBDIR)unionDataImageInMemory.20081103.o	\
	$(CMM2003LIBDIR)unionDefaultDataImageControl.20081103.o	\
	$(CMM2003LIBDIR)unionLockTBL.1.0.o			\
	$(CMM2003LIBDIR)unionOSLockMachinism.forUnix.useMsgQueue.1.0.o\
	$(CMM2003LIBDIR)unionIndexTBL.20060802.a		\
	$(CMM2003LIBDIR)unionModule2.1.20071026.o	\
	$(CMM2003LIBDIR)unionREC.20060825.o	\
	$(CMM2003LIBDIR)unionEnv.20050804.4096.o	\
	$(CMM2003LIBDIR)union2DirectionQueue.20081103.o	\
	$(CMM2003LIBDIR)unionVersion.20061204.o	\
	$(CMM2003LIBDIR)unionSystemTime1.0.o	\
	unionProductGenerateTime.a		\
	$(VERLIBDIR)unionPredefinedVersion1.0.a	\
	$(VERLIBDIR)unionEffectiveDays.36500.a	\
	$(CMM2003LIBDIR)unionLibName3.0.o	\
	$(CMM2003LIBDIR)unionFunDef.20080925.o	\
	$(CMM2003LIBDIR)unionDefaultVarTypeDefDir.20080925.o	\
	$(CMM2003LIBDIR)externLogFun.20080717.o	\
	$(CMM2003LIBDIR)unionWorkingDir.20080724.o	\
	$(CMM2003LIBDIR)UnionStr1.2.a		\
	$(CMM2003LIBDIR)unionInput1.0.o

fileLibs=$(CMM2003LIBDIR)unionVarTypeDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionSimpleTypeDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionStructDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionPointerDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionFunDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionArrayDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionVariableDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionEnumDefFile.20080925.o	\
	$(CMM2003LIBDIR)unionConstDefFile.20080925.o

testFun:	$(null)
	cc $(OSSPEC) -o testFun $(OSSPECLINK) $(libs001) $(libs) $(fileLibs) $(OSSPECLIBS)

testFunList:	$(null)
	cc $(OSSPEC) -o testFunList $(OSSPECLINK) $(libs002) $(libs) $(fileLibs) $(OSSPECLIBS)

dbLibs=$(CMM2003LIBDIR)unionVarTypeDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionSimpleTypeDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionStructDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionPointerDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionFunDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionArrayDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionConstDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionEnumDefInDB.20080925.o	\
	$(CMM2003LIBDIR)unionVariableDefInDB.20080925.o	\
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
testFunInDB:	$(null)
	cc $(OSSPEC) -o testFunInDB $(OSSPECLINK) $(libs001) $(libs) $(dbLibs) $(OSSPECLIBS)
finalProduct:
		mv testFunList				$(CMM2003BINDIR)
		mv testFun				$(CMM2003BINDIR)
		mv testFunInDB				$(CMM2003BINDIR)
		rm unionProductGenerateTime.a
