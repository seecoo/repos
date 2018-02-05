include dirAndLib.def

# 要生成的程序列表

all:	genProductGeneratedTimeLib		\
	longTcpipSvr-essc3x			\
	shortTcpipSvr-essc3x			\
	longTcpipSvr-essc2x			\
	shortTcpipSvr-essc2x			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs11 =	$(CMM2003LIBDIR)tcpipSvrMain.20130301.o		\
		$(tcpipSvrForESSC3XLibs)			\
		$(commBetweenMDLLibs)				\
		$(mdlLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(compressLibs)					\
		$(commLibs)
longTcpipSvr-essc3x:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs11) 

libs12 =	$(shortConnMainLibs)				\
		$(tcpipSvrForESSC3XLibs)			\
		$(commBetweenMDLLibs)				\
		$(mdlLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(compressLibs)					\
		$(commLibs)
shortTcpipSvr-essc3x:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs12)

libs21 =	$(CMM2003LIBDIR)tcpipSvrMain.20130301.o		\
		$(tcpipSvrForESSC2XLibs)			\
		$(commBetweenMDLLibs)				\
		$(mdlLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(compressLibs)					\
		$(commLibs)
longTcpipSvr-essc2x:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs21) 

libs22 =	$(shortConnMainLibs)				\
		$(tcpipSvrForESSC2XLibs)			\
		$(commBetweenMDLLibs)				\
		$(mdlLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(compressLibs)					\
		$(commLibs)
shortTcpipSvr-essc2x:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs22)

