include dirAndLib.def

# Ҫ���ɵĳ����б�
all:	genProductGeneratedTimeLib	\
	magent		\
	rmTmpFiles

# ����ʱ���
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

