include dirAndLib.def

# 要生成的程序列表
all:	genProductGeneratedTimeLib	\
	mngErrCodeTranslater		\
	mngMsgBuf			\
	msgBufMon			\
	taskMon				\
	mngTask				\
	mngREC				\
	mngCommConf			\
	commConfMon			\
	mngUnionLog			\
	mngModule			\
	mngTableDef			\
	mngTableData			\
	mngHighCached			\
	testHighCache			\
	mngClusterSyn			\
	hsmMon				\
	rmTmpFiles			

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs01 =	$(CMM2003LIBDIR)mngErrCodeTranslater.20060726.o		\
		$(realDBLibs)						\
		$(commLibs)
mngErrCodeTranslater:
	$(COMPILECMD) $(TESTBINDIR)$@ $(libs01)

libs02 =	$(CMM2003LIBDIR)mngMsgBuf.20131201.o		\
		$(realDBLibs)					\
		$(commLibs)
mngMsgBuf:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs02)

libs03 =	$(CMM2003LIBDIR)msgBufMon.20131201.o		\
		$(realDBLibs)					\
		$(monitorLibs)					\
		$(commLibs)
msgBufMon:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs03)

libs04 =	$(CMM2003LIBDIR)mngREC1.0.a			\
		$(realDBLibs)					\
		$(commLibs)
mngREC:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs04)

libs05 =	$(CMM2003LIBDIR)mngTask.20061101.o		\
		$(realDBLibs)					\
		$(commLibs)
mngTask:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs05)

libs06 =	$(CMM2003LIBDIR)taskMon.20050919.a		\
		$(realDBLibs)					\
		$(monitorLibs)					\
		$(commLibs)
taskMon:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs06)

libs07 =	$(CMM2003LIBDIR)mngIndexTBL1.0.a		\
		$(realDBLibs)					\
		$(commLibs)
mngIndexTBL:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs07)

libs08 =	$(CMM2003LIBDIR)mngLockTBL.1.0.o		\
		$(CMM2003LIBDIR)unionLockTBL.1.0.o		\
		$(realDBLibs)					\
		$(commLibs)
mngLockTBL:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs08)

libs09 =	$(CMM2003LIBDIR)lockTBLMon.1.0.o		\
		$(CMM2003LIBDIR)unionLockTBL.1.0.o		\
		$(realDBLibs)					\
		$(libsOfCommConf)				\
		$(monitorLibs)					\
		$(commLibs)
lockTBLMon:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs09)

libs10 =	$(CMM2003LIBDIR)mngCommConf.20051009.o		\
		$(realDBLibs)					\
		$(libsOfCommConf)				\
		$(commLibs)
mngCommConf:
		$(COMPILECMD) $(TESTBINDIR)mngCommConf $(libs10)

libs11 =	$(CMM2003LIBDIR)commConfMon.500.20051009.o	\
		$(realDBLibs)					\
		$(libsOfCommConf)				\
		$(monitorLibs)					\
		$(commLibs)
commConfMon:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs11) 

libs12 =	$(CMM2003LIBDIR)mngUnionLog2.0.a		\
		$(realDBLibs)					\
		$(commLibs)
mngUnionLog:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs12)

libs13 =	$(CMM2003LIBDIR)mngModule2.0.20061227.o		\
		$(realDBLibs)					\
		$(commLibs)
mngModule:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs13)

libs14 =	$(CMM2003LIBDIR)mngTableDef.1.0.o		\
		$(realDBLibs)					\
		$(commLibs)
mngTableDef:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs14)

libs15 =	$(CMM2003LIBDIR)mngHighCached.20130301.o	\
		$(realDBLibs)					\
		$(libHighCachedLibs)				\
		$(commLibs)
mngHighCached:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs15)

libs151 =	$(CMM2003LIBDIR)testHighCache.o	\
		$(realDBLibs)					\
		$(libHighCachedLibs)				\
		$(commLibs)
testHighCache:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs151)
	
libs16 =	$(CMM2003LIBDIR)mngClusterSyn.20130301.o	\
		$(realDBLibs)					\
		$(commLibs)
mngClusterSyn:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs16)

libs17 =	$(CMM2003LIBDIR)mngTableData.1.0.o		\
		$(realDBLibs)					\
		$(commLibs)
mngTableData:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs17)

libs18 =	$(CMM2003LIBDIR)mngCardBinTBL1.0.o			\
		$(CMM2003LIBDIR)unionCardBinTBL1.0.db.20141025.o	\
		$(realDBLibs)						\
		$(commLibs)
mngCardBin:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs18)

libs19 =	$(CMM2003LIBDIR)hsmMon.o			\
		$(realDBLibs)					\
		$(monitorLibs)					\
		$(commLibs)
hsmMon:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs19)	

