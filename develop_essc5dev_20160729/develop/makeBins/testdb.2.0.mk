include dirAndLib.def

all:	genProductGeneratedTimeLib	\
	testdb.2.0			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

logAndTaskLibs=	$(CMM2003LIBDIR)unionTask.20090421.useSig2.o	\
		$(CMM2003LIBDIR)UnionLog.20130301.o		\
		$(CMM2003LIBDIR)unionSystemTime1.0.o		\
		$(CMM2003LIBDIR)unionLogTBL.20090421.o		\
		$(CMM2003LIBDIR)libzlog.a

libs11=	$(CMM2003LIBDIR)testdb.2.0.o			\
		$(useDBSvrLibs)                                 \
                $(libHighCachedLibs)                            \
                $(libsOfCommConf)                               \
                $(mdlLibs)                                      \
                $(commBetweenMDLLibs)                           \
                $(compressLibs)                                 \
                $(commLibs)
testdb.2.0:
	$(COMPILECMD) $(TESTBINDIR)testdb.2.0 $(libs11)

