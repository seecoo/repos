include dirAndLib.def

# Ҫ���ɵĳ����б�

all:	genProductGeneratedTimeLib		\
	hsmTask-Cmd				\
	rmTmpFiles

# ����ʱ���
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

