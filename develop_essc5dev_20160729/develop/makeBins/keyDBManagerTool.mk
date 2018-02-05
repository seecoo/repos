include dirAndLib.def

# 要生成的程序列表

all:	genProductGeneratedTimeLib		\
	mngKeyDBMemory				\
	mngCreateKeyDB				\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs11 =	$(PROLIBDIR)unionKeyDBTool.o	\
		$(PROLIBDIR)mngKeyDBTool.o	\
		$(keyDBLibs)					\
		$(hsmCmdLibs)					\
		$(hsmSvrLibs)					\
		$(realDBLibs)					\
		$(libsOfCommConf)				\
		$(libHighCachedLibs)                            \
		$(compressLibs)					\
		$(commLibs)
mngKeyDBMemory:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs11) 

libs12 =	$(PROLIBDIR)mngDesKeyDB.1.0.o			\
		$(PROLIBDIR)unionDesKeyDB.1.0.o			\
		$(keyDBLibs)					\
		$(hsmCmdLibs)					\
		$(hsmSvrLibs)					\
		$(useDBSvrLibs)					\
		$(libsOfCommConf)				\
		$(libHighCachedLibs)                            \
		$(compressLibs)					\
		$(commLibs)
		
mngCreateKeyDB:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs12) 

