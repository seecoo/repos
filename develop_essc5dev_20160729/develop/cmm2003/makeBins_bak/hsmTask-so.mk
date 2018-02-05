include dirAndLib.def

all:	genProductGeneratedTimeLib	\
	hsmTask-so			\
	hsmTask-db-so			\
	rmTmpFiles

# ����ʱ���
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs01 =	$(CMM2003LIBDIR)unionFunSvrInterfaceService.useDL.20130301.o	\
		$(funSvrLibs)							\
		$(useDBSvrLibs)							\
		$(libHighCachedLibs)						\
		$(commLibs)
hsmTask-so:	$(null)
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01) $(DLSPECLINK)

libs02 =	$(CMM2003LIBDIR)unionFunSvrInterfaceService.useDL.20130301.o	\
		$(funSvrLibs)							\
		$(useDBSvrLibs)							\
		$(libHighCachedLibs)						\
		$(commLibs)
hsmTask-db-so:	$(null)
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs02) $(DLSPECLINK)
