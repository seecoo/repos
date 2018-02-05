include dirAndLib.def

all:	genProductGeneratedTimeLib	\
	testdb				\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs11=	$(CMM2003LIBDIR)testdb.o				\
		$(realDBLibs)                                 \
                $(libHighCachedLibs)                            \
                $(libsOfCommConf)                               \
                $(commLibs)
libs12= $(CMM2003LIBDIR)/unionFileTransfer.20040723.o		\
	$(CMM2003LIBDIR)/unionRecFile.20080724.o		\
	$(CMM2003LIBDIR)/unionIO.20120521.o			\
	$(CMM2003LIBDIR)/unionWorkingDir.20080724.o
testdb:
	$(COMPILECMD) $(TESTBINDIR)testdb $(libs11) $(libs12)

