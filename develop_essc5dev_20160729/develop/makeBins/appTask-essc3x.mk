include dirAndLib.def

# 要生成的程序列表
all:	genProductGeneratedTimeLib	\
	appTask-essc3x			\
	appTask-essc3x-db		\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

tcpipSvrSharedLibs=$(CMM2003LIBDIR)UnionTCPIPSvr4.0.20061108.o		\
		$(CMM2003LIBDIR)tcpipSvrRunningEnv.null.o		\
		$(CMM2003LIBDIR)unionRegisterRunningTime.20130301.o	\
		$(svrNameLibs)
		
tcpipSvrLibs=	$(CMM2003LIBDIR)synchTCPIPSvr.xml.20140904.o		\
		$(CMM2003LIBDIR)setXMLResponsePackageForError.20080511.o	\
		$(PROLIBDIR)esscPackageConvert.20130301.o		\
		$(PROLIBDIR)esscPackage.20130301.o			\
		$(tcpipSvrSharedLibs)

funSvrLibs=	$(CMM2003LIBDIR)xmlPackageInterprotorService.20140904.o	\
		$(CMM2003LIBDIR)accessCheck.20140904.o			\
		$(CMM2003LIBDIR)unionBaseUIService.20130301.o		\
		$(registerForUILibs)

libs01 =	$(appConnMainLibs)				\
		$(tcpipSvrLibs)					\
		$(funSvrLibs)					\
		$(recFileLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(hsmSvrLibs)					\
		$(compressLibs)					\
		$(opensslLibs)					\
		$(commLibs)
appTask-essc3x:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01)	$(DLSPECLINK)

libs02 =	$(appConnMainLibs)				\
		$(tcpipSvrLibs)					\
		$(funSvrLibs)					\
		$(recFileLibs)					\
		$(realDBLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(hsmSvrLibs)					\
		$(compressLibs)					\
		$(opensslLibs)					\
		$(commLibs)
appTask-essc3x-db:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs02)	$(DLSPECLINK)

