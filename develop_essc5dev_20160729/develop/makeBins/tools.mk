include dirAndLib.def

all:	genProductGeneratedTimeLib		\
	outputDataFromDB			\
	inputDataToDB				\
	encryptDBPassword			\
	dlVerion-so				\
	rmTmpFiles

# 生成时间库
genProductGeneratedTimeLib:    
	genProductGenerateTime

rmTmpFiles:
	rm unionProductGenerateTime.a

libs11=	$(CMM2003LIBDIR)inputDataToDB.2.0.o		\
	$(CMM2003LIBDIR)inputAndOutputDataForDB.2.0.o	\
	$(realDBLibs)					\
	$(commLibs)
inputDataToDB:
	$(COMPILECMD) $(TESTBINDIR)inputDataToDB $(libs11)

libs12=	$(CMM2003LIBDIR)outputDataFromDB.2.0.o		\
	$(CMM2003LIBDIR)inputAndOutputDataForDB.2.0.o	\
	$(realDBLibs)					\
	$(commLibs)
outputDataFromDB:
	$(COMPILECMD) $(TESTBINDIR)outputDataFromDB $(libs12)

libs16=	$(CMM2003LIBDIR)unionDBPasswordMain.1.0.o	\
	$(realDBLibs)					\
	$(commLibs)
encryptDBPassword:
	$(COMPILECMD) $(TESTBINDIR)encryptDBPassword $(libs16)

libs17=$(CMM2003LIBDIR)unionDLVersionMain.20130301.o	\
	$(libHighCachedLibs)				\
	$(commLibs)					\
	$(realDBLibs)					\
	-lz
dlVerion-so:    $(null)
	$(COMPILECMD) $(TESTBINDIR)dlVerion-so $(libs17) $(DLSPECLINK)

