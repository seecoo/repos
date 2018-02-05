include dirAndLib.def

# 要生成的程序列表
all:	genProductGeneratedTimeLib	\
	dbSvr				\
	hsmSvr				\
	hsmSvr3				\
	fileReceiver			\
	memcached			\
	libDBDataClear.so		\
	libBaseUI.so			\
	transMonSvr			\
	libFileClear.so			\
	daemonTask			\
	dataExecute			\
	dataSync			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

dlVersionLibs=$(dbTypeLibs) $(CMM2003LIBDIR)/unionDLVersion.20130301.o unionProductGenerateTime.a

libs01 =	$(CMM2003LIBDIR)dbSvr.2.0.o			\
		$(realDBLibs)					\
		$(commLibs)
dbSvr:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01)

libs02 =	$(CMM2003LIBDIR)hsmSvrMain.20130301.o		\
		$(realDBLibs)					\
		$(libsOfCommConf)				\
		$(commLibs)

hsmSvr:
		$(COMPILECMD) $(TESTBINDIR)hsmSvr $(libs02)

libs021 =	$(CMM2003LIBDIR)hsmSvrMain.ver3.20130301.o	\
		$(realDBLibs)					\
		$(libsOfCommConf)				\
		$(commLibs)
hsmSvr3:
		$(COMPILECMD) $(TESTBINDIR)hsmSvr.ver3 $(libs021)

libs03=		$(CMM2003LIBDIR)unionFileReceiver.2.0.o	\
		$(CMM2003LIBDIR)unionFileTransfer.2.0.o	\
		$(CMM2003LIBDIR)unionTmpFileDir.20040723.o	\
		$(CMM2003LIBDIR)unionIO.20120521.o		\
		$(realDBLibs)					\
		$(recFileLibs)					\
		$(commLibs)
fileReceiver:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs03)

memcached:
		cp $(CMM2003OPENDIR)/bin/$@ $(TESTBINDIR)

ttserver:
		cp $(CMM2003OPENDIR)/bin/$@ $(TESTBINDIR)

libs05=		$(CMM2003LIBDIR)magent.o			\
		$(CMM2003LIBDIR)ketama.o			\
		$(CMM2003OPENDIR)/lib/libevent.a -lm -lrt
magent:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs05)

libs06 =	$(CMM2003LIBDIR)dbDataClear.2.0.o		\
		$(dlVersionLibs)

libDBDataClear.so:
		$(COMPILESO) $(TESTBINDIR)$@ $(libs06)


libs07 =	$(CMM2003LIBDIR)transMonSvr.2.0.o		\
		$(realDBLibs)					\
		$(libHighCachedLibs)				\
		$(commLibs)

transMonSvr:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs07)

libs08 =	$(CMM2003LIBDIR)fileClear.o			\
		$(dlVersionLibs)
libFileClear.so:
		$(COMPILESO) $(TESTBINDIR)$@ $(libs08)

libs09 =	$(CMM2003LIBDIR)unionDaemonTask.20130301.o	\
		$(libHighCachedLibs)				\
		$(recFileLibs)					\
		$(realDBLibs)					\
		$(commLibs)
		
daemonTask:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs09) $(DLSPECLINK)

hsmCmdLibs =	$(CMM2003LIBDIR)unionHsmCmd.20120521.o			\
		$(CMM2003LIBDIR)3DesRacalSyntaxRules1.0.o		\
		$(CMM2003LIBDIR)emv2000AndPbocSyntaxRules.20120521.o

libs10 =	$(baseServiceForUILibs)				\
		$(opensslLibs)					\
		$(hsmCmdLibs)					\
		$(dlVersionLibs)
libBaseUI.so:
		$(COMPILESO) $(TESTBINDIR)$@ $(libs10)

libs11 =	$(CMM2003LIBDIR)dataExecute.2.0.o		\
		$(libHighCachedLibs)				\
		$(tcpipSvrSharedLibs)                           \
		$(realDBLibs)					\
		$(commLibs)
dataExecute:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs11) $(DLSPECLINK)

libs12 =	$(CMM2003LIBDIR)dataSync.2.0.o			\
		$(realDBLibs)					\
		$(commLibs)
dataSync:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs12)

