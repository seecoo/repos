include dirAndLib.def

# Ҫ���ɵĳ����б�

all:	genProductGeneratedTimeLib		\
	mngKeyDBMemory				\
	mngCreateKeyDB				\
	rmTmpFiles

# ����ʱ���
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

