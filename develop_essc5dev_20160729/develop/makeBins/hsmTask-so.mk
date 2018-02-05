include dirAndLib.def

all:	genProductGeneratedTimeLib	\
	hsmTask-so			\
	hsmTask-db-so			\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

hsmCmdLibs =	$(CMM2003LIBDIR)commWithHsmSvr.20130301.o			\
		$(mdlLibs)							\
		$(commBetweenMDLLibs)

libs01 =	$(CMM2003LIBDIR)unionFunSvrInterfaceService.useDL.20130301.o	\
		$(funSvrLibs)							\
		$(useDBSvrLibs)							\
		$(libHighCachedLibs)						\
		$(hsmCmdLibs)							\
		$(commLibs)
hsmTask-so:	$(null)
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01) $(DLSPECLINK)

libs02 =	$(CMM2003LIBDIR)unionFunSvrInterfaceService.useDL.20130301.o	\
		$(funSvrLibs)							\
		$(useDBSvrLibs)							\
		$(libHighCachedLibs)						\
		$(hsmCmdLibs)							\
		$(commLibs)
hsmTask-db-so:	$(null)
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs02) $(DLSPECLINK)
