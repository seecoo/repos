include dirAndLib.def

# Ҫ���ɵĳ����б�
all:	genProductGeneratedTimeLib	\
	fileSender			\
	rmTmpFiles

# ����ʱ���
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
