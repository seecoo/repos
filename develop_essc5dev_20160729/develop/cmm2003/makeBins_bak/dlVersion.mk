include dirAndLib.def

all:	genProductGeneratedTimeLib		\
	dlVersion-so				\
	rmTmpFiles

# ����ʱ���
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs17=$(CMM2003LIBDIR)unionDLVersionMain.20130301.o	\
	-ldl
dlVersion-so:    $(null)
	$(COMPILECMD) $(TESTBINDIR)dlVersion-so $(libs17) $(DLSPECLINK_V)


