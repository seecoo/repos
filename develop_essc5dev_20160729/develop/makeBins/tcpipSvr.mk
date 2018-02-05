include dirAndLib.def

# 要生成的程序列表
all:	genProductGeneratedTimeLib	\
	longTcpipSvr			\
	shortTcpipSvr			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs01 =	$(CMM2003LIBDIR)tcpipSvrMain.20130301.o		\
		$(tcpipSvrLibs)					\
		$(commBetweenMDLLibs)				\
		$(mdlLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(compressLibs)					\
		$(commLibs)
longTcpipSvr:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01)

libs02 =	$(shortConnMainLibs)				\
		$(tcpipSvrLibs)					\
		$(commBetweenMDLLibs)				\
		$(mdlLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(libsOfCommConf)				\
		$(compressLibs)					\
		$(commLibs)
shortTcpipSvr:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs02)

