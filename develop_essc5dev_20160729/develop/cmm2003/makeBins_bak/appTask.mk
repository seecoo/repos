include dirAndLib.def

# 要生成的程序列表
all:	genProductGeneratedTimeLib	\
	appTask				\
	appTask-db			\
	appTask-hsmcmd			\
	appTask-hsmcmd-db		\
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
		$(CMM2003LIBDIR)packageConvert.null.20130301.o		\
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
		$(hsmCmdLibs)					\
		$(compressLibs)					\
		$(opensslLibs)					\
		$(commLibs)
appTask:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01)	$(DLSPECLINK)

libs02 =	$(appConnMainLibs)				\
		$(tcpipSvrLibs)					\
		$(funSvrLibs)					\
		$(recFileLibs)					\
		$(realDBLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(hsmCmdLibs)					\
		$(compressLibs)					\
		$(opensslLibs)					\
		$(commLibs)
appTask-db:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs02)	$(DLSPECLINK)

tcpipHsmLibs=	$(CMM2003LIBDIR)synchTCPIPSvr.hsmcmd.20140904.o		\
		$(CMM2003LIBDIR)hsmcmdPackageInterprotorService.20140904.o	\
		$(CMM2003LIBDIR)accessCheck.20140904.o			\
		$(tcpipSvrSharedLibs)

libs03 =	$(appConnMainLibs)				\
		$(tcpipHsmLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(hsmCmdLibs)					\
		$(compressLibs)					\
		$(commLibs)

appTask-hsmcmd:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs03)

libs04 =	$(appConnMainLibs)				\
		$(tcpipHsmLibs)					\
		$(realDBLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(hsmCmdLibs)					\
		$(compressLibs)					\
		$(commLibs)
appTask-hsmcmd-db:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs04)
