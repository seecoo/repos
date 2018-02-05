include dirAndLib.def

# 要生成的程序列表

all:	genProductGeneratedTimeLib		\
	hsmTask-Cmd				\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs11 =	$(tcpipSvrMainForHsmSvrLibs)			\
		$(PROLIBDIR)hsmCmd.1.0.o			\
		$(PROLIBDIR)synchTCPIPSvr.1.0.o			\
		$(hsmSvrLibs)					\
		$(useDBSvrLibs)					\
		$(libsOfCommConf)				\
		$(libHighCachedLibs)                            \
		$(commLibs)
hsmTask-Cmd:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs11) 

