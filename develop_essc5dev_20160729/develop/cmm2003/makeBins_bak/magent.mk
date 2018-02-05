include dirAndLib.def

# 要生成的程序列表
all:	genProductGeneratedTimeLib	\
	magent		\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs01 =	$(commLibs)				\
		$(CMM2003LIBDIR)magent.o		\
		$(CMM2003LIBDIR)ketama.o		\
		$(realDBLibs)				\
		$(libHighCachedLibs)			\
		$(CMM2003OPENDIR)/lib/libevent.a


magent:
		$(COMPILECMD) $(TESTBINDIR)$@ $(libs01)	$(DLSPECLINK)

