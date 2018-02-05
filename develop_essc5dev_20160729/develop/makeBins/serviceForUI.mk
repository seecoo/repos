include dirAndLib.def

all:	genProductGeneratedTimeLib	\
	baseServiceForUI		\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a


libs01 =	$(funSvrLibs)					\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(hsmCmdLibs)					\
		$(recLibs)					\
		$(libsOfCommConf)				\
		$(baseServiceForUILibs)				\
		$(registerForUILibs)				\
		$(opensslLibs)					\
		$(commLibs)					

baseServiceForUI:	$(null)
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01) $(DLSPECLINK)

