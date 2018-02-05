include dirAndLib.def

all:	genProductGeneratedTimeLib		\
	aliyunTransBill				\
	rmTmpFiles

# ����ʱ���
genProductGeneratedTimeLib:
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs02 =	$(PROLIBDIR)aliyunTransBill.o			\
		$(useDBSvrLibs)					\
		$(libHighCachedLibs)				\
		$(mdlLibs)					\
		$(commBetweenMDLLibs)				\
		$(compressLibs)					\
		$(commLibs)
aliyunTransBill:$(null)
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs02)

