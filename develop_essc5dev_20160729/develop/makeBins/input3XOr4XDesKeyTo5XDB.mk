include dirAndLib.def

# 要生成的程序列表

all:	genProductGeneratedTimeLib		\
	input3XOr4XDesKeyTo5XDB			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs11 =	$(PROLIBDIR)inputDesKeyToDB.20140714.o	\
		$(PROLIBDIR)transDesKey.20140714.o	\
		$(CMM2003LIBDIR)unionDesKey.4.x.20060814.o		\
		$(useDBSvrLibs)					\
		$(hsmSvrLibs)					\
		$(hsmCmdLibs)					\
		$(libsOfCommConf)				\
		$(libHighCachedLibs)                            \
		$(compressLibs)					\
		$(commLibs)
input3XOr4XDesKeyTo5XDB:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs11) 

