include dirAndLib.def

# 要生成的程序列表
all:	genProductGeneratedTimeLib	\
	fileSender			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

dlVersionLibs=$(dbTypeLibs) $(CMM2003LIBDIR)/unionDLVersion.20130301.o unionProductGenerateTime.a

libs03=		$(CMM2003LIBDIR)unionFileSender.2.0.o	\
		$(CMM2003LIBDIR)unionFileTransfer.2.0.o	\
		$(CMM2003LIBDIR)unionTmpFileDir.20040723.o	\
		$(CMM2003LIBDIR)unionIO.20120521.o		\
		$(realDBLibs)					\
		$(recFileLibs)					\
		$(commLibs)
fileSender:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs03)
