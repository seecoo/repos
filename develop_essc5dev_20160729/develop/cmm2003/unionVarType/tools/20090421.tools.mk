# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	mvLibs			\
	genProductGeneratedTimeLib	\
	genTBLDef		\
	genTBLQuery		\
	genTBLSQL		\
	genOperationID		\
	libProducts		\
	finalProduct
	

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

mvLibs:	$(null)
	cp *.o							$(CMM2003LIBDIR)
libProducts:	$(null)
	mv *.o							$(CMM2003LIBDIR)
	cp $(INCDIR)/unionGenTBLDefFromTBL.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionTableList.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionTableField.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionOperationList.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionOperationAuthorization.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionMngSvrOperationType.h			$(CMM2003INCDIR)

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
	$(CMM2003LIBDIR)mngSvrConfFileName.20060808.o	\
	unionProductGenerateTime.a	\
	$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
	$(CMM2003VERLIB)unionEffectiveDays.36500.a	\
	$(CMM2003LIBDIR)unionLibName3.0.o	\
	$(CMM2003LIBDIR)unionInput1.0.o	\
	$(CMM2003LIBDIR)unionSystemTime1.0.o
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
	$(CMM2003LIBDIR)unionTBLQueryConf.20080724.o	\
	$(CMM2003LIBDIR)unionFldGrp.20080724.o	\
	$(CMM2003LIBDIR)unionRecFile.20080724.o	\
	$(CMM2003LIBDIR)unionRec.20080724.o

objs = 	unionGenTBLDefFromTBL.20090421.o	\
	unionTableList.20090421.o	\
	unionGenIncFromTBL.20080925.o	\
	unionOperationList.20090421.o	\
	unionOperationAuthorization.20090421.o	\
	unionMngSvrOperationType.20090421.o	\
	unionTableField.20090421.o
makeall:	$(objs)

objs1=	$(objs)	\
	unionGenTBLDefMain.20090421.o
genTBLDef:	$(objs1)
	cc -o genTBLDef $(OSSPECLINK) $(OSSPECLIBS) $(objs1) $(libs) $(dbLibs) $(tblLibs)

objs2=	$(objs)	\
	unionGenTBLQueryMain.20090421.o
genTBLQuery:	$(objs2)
	cc -o genTBLQuery $(OSSPECLINK) $(OSSPECLIBS) $(objs2) $(libs) $(dbLibs) $(tblLibs)

objs3=	$(objs)	\
	unionGenTBLSQLMain.20090421.o
genTBLSQL:	$(objs3)
	cc -o genTBLSQL $(OSSPECLINK) $(OSSPECLIBS) $(objs3) $(libs) $(dbLibs) $(tblLibs)

objs4=	$(objs)	\
	genOperationID.o
genOperationID:	$(objs4)
	cc -o genOperationID $(OSSPECLINK) $(OSSPECLIBS) $(objs4) $(libs) $(dbLibs) $(tblLibs)

finalProduct:
		mv genTBLDef				$(CMM2003BINDIR)
		mv genTBLQuery				$(CMM2003BINDIR)
		mv genTBLSQL				$(CMM2003BINDIR)
		mv genOperationID			$(CMM2003BINDIR)
		rm *.a
