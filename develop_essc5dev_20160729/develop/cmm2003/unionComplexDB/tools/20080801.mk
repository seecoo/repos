HOMEDIR = $(CMM2003DIR)/unionDatagram/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall			\
	finalProduct

all1:	makeall			\
	genProductGeneratedTimeLib	\
	createObject		\
	deleteObject		\
	appObjectFld		\
	finalProduct1
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

makeall:	outputRecOfTBL.20080801.o \
		outputRecOfTBL.20090421.o \
		inputRecOfTBL.20080801.o

# 生成时间库
genProductGeneratedTimeLib:     $(null)
	genProductGenerateTime

# 定义库文件
libs=	$(CMM2003LIBDIR)unionComplexDBCommon.20080801.o		\
	$(CMM2003LIBDIR)unionComplexDBDataTransfer.20080801.o	\
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
	$(CMM2003LIBDIR)unionCalcuOperator.20080925.o	\
	$(CMM2003LIBDIR)unionDataImageInMemory.20081103.o	\
	$(CMM2003LIBDIR)unionDefaultDataImageControl.20081103.o	\
	$(CMM2003LIBDIR)unionLockTBL.1.0.o			\
	$(CMM2003LIBDIR)unionOSLockMachinism.forUnix.useMsgQueue.1.0.o\
	$(CMM2003LIBDIR)unionIndexTBL.20060802.a		\
	$(CMM2003LIBDIR)unionModule2.1.20071026.o	\
	$(CMM2003LIBDIR)unionEnv.20050804.4096.o	\
	$(CMM2003LIBDIR)union2DirectionQueue.20081103.o	\
	$(CMM2003LIBDIR)unionREC.20060825.o	\
	$(CMM2003LIBDIR)unionVersion.20061204.o	\
	unionProductGenerateTime.a	\
	$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
	$(CMM2003VERLIB)unionEffectiveDays.36500.a	\
	$(CMM2003LIBDIR)unionLibName3.0.o	\
	$(CMM2003LIBDIR)unionRec.20080724.o	\
	$(CMM2003LIBDIR)unionWorkingDir.20080724.o	\
	$(CMM2003LIBDIR)externLogFun.20080717.o \
	$(CMM2003LIBDIR)UnionStr1.2.o \
	$(CMM2003LIBDIR)unionSystemTime1.0.o \
	$(CMM2003LIBDIR)UnionLog1.2.withoutTimeFun.o \
	$(CMM2003LIBDIR)unionInput1.0.o

objs1=createObject.20080801.o
createObject:	$(objs1)
	cc $(OSSPEC) -o createObject $(objs1) $(libs) $(OSSPECLIBS)

objs2=deleteObject.20080801.o
deleteObject:	$(objs2)
	cc $(OSSPEC) -o deleteObject $(objs2) $(libs) $(OSSPECLIBS)

objs3=appObjectFld.20080801.o
appObjectFld:	$(objs3)
	cc $(OSSPEC) -o appObjectFld $(objs3) $(libs) $(OSSPECLIBS)

objs4=outputRecOfTBL.20080801.o
outputRecOfTBL:	$(objs4)
	cc $(OSSPEC) -o outputRecOfTBL $(objs4) $(libs) $(OSSPECLIBS)

objs5=inputRecOfTBL.20080801.o
inputRecOfTBL:	$(objs5)
	cc $(OSSPEC) -o inputRecOfTBL $(objs5) $(libs) $(OSSPECLIBS)

finalProduct:
		mv *.o						$(CMM2003LIBDIR)
finalProduct1:
		mv createObject					$(CMM2003BINDIR)
		mv deleteObject					$(CMM2003BINDIR)
		mv appObjectFld					$(CMM2003BINDIR)
		mv *.o						$(CMM2003LIBDIR)
		rm *.a
