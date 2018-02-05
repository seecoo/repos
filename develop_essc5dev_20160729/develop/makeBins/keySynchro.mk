include dirAndLib.def

# 要生成的程序列表

all:	genProductGeneratedTimeLib		\
	libKeySynchro.so			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

dlVersionLibs=$(dbTypeLibs) $(CMM2003LIBDIR)/unionDLVersion.20130301.o unionProductGenerateTime.a

hsmCmdLibs =    $(CMM2003LIBDIR)unionHsmCmd.20120521.o                          \
                $(CMM2003LIBDIR)3DesRacalSyntaxRules1.0.o                       \
                $(CMM2003LIBDIR)emv2000AndPbocSyntaxRules.20120521.o

commLibs =     	$(hsmCmdLibs) 					\
		$(keyDBLibs)            			\
		$(hsmSvrLibs)            			\
		$(compressLibs)					\
                $(dlVersionLibs)

libs13 =	$(PROLIBDIR)keyAutoManager.20130301.o		\
		$(PROLIBDIR)essc5InterfaceE111.20130301.o	\
		$(PROLIBDIR)essc5InterfaceE114.20130301.o	\
		$(PROLIBDIR)essc5InterfaceE115.20130301.o	\
		$(commLibs)

libKeySynchro.so:
		$(COMPILESO) $(TESTBINDIR)$@ $(libs13)
		
