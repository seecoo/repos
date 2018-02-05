
HOMEDIR = $(CMM2003DIR)/unionDatagram/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall			\
	datagram		\
	genCmmPackClientAPI	\
	genAllCmmPackClientAPI	\
	genCmmPackSvrAPI	\
	genAllCmmPackSvrAPI	\
	genCmmPackSvrFun	\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionDatagramDef.20081003.o			\
	unionGenSvrFunUsingCmmPack.20081003.o	\
	unionGenClientFunUsingCmmPack.20081003.o	\
	unionPackAndFunRelation.20081003.o		\
	unionDatagramClassDef.20081003.o
makeall:	$(allobjs)

# 定义库文件
libs=$(CMM2003LIBDIR)unionModuleDef.20080925.o\
	$(CMM2003LIBDIR)unionProgramDef.20080925.o\
	$(CMM2003LIBDIR)unionTestConf.20081006.o\
	$(CMM2003LIBDIR)unionDesignKeyWord.20080925.o\
	$(CMM2003LIBDIR)unionGenFunTestProgram.20080925.o\
	$(CMM2003LIBDIR)unionVarAssignment.20080925.o\
	$(CMM2003LIBDIR)unionAutoGenFileFunGrp.20080925.o\
	$(CMM2003LIBDIR)unionVarValue.20080925.o\
	$(CMM2003LIBDIR)unionVarTypeDef.20080925.o\
	$(CMM2003LIBDIR)unionVarDef.20080925.o\
	$(CMM2003LIBDIR)unionStructDef.20080925.o\
	$(CMM2003LIBDIR)unionUnionDef.20080925.o\
	$(CMM2003LIBDIR)unionEnumDef.20080925.o\
	$(CMM2003LIBDIR)unionPointerDef.20080925.o\
	$(CMM2003LIBDIR)unionVariableDef.20080925.o\
	$(CMM2003LIBDIR)unionArrayDef.20080925.o\
	$(CMM2003LIBDIR)unionConstDef.20080925.o\
	$(CMM2003LIBDIR)unionSimpleTypeDef.20080925.o\
	$(CMM2003LIBDIR)unionFunDef.20080925.o\
	$(CMM2003LIBDIR)unionDefaultVarTypeDefDir.20080925.o\
	$(CMM2003LIBDIR)unionVarTypeDefFile.20080925.o\
	$(CMM2003LIBDIR)unionModuleDefFile.20080925.o\
	$(CMM2003LIBDIR)unionProgramDefFile.20080925.o\
	$(CMM2003LIBDIR)unionSimpleTypeDefFile.20080925.o\
	$(CMM2003LIBDIR)unionStructDefFile.20080925.o\
	$(CMM2003LIBDIR)unionUnionDefFile.20080925.o\
	$(CMM2003LIBDIR)unionEnumDefFile.20080925.o\
	$(CMM2003LIBDIR)unionPointerDefFile.20080925.o\
	$(CMM2003LIBDIR)unionArrayDefFile.20080925.o\
	$(CMM2003LIBDIR)unionConstDefFile.20080925.o\
	$(CMM2003LIBDIR)unionFunDefFile.20080925.o\
	$(CMM2003LIBDIR)unionVariableDefFile.20080925.o\
	$(CMM2003LIBDIR)externLogFun.20080717.o\
	$(CMM2003LIBDIR)UnionStr1.2.o\
	$(CMM2003LIBDIR)UnionLog1.2.o\
	$(CMM2003LIBDIR)unionGenFunListTestProgram.20080925.o \
	$(CMM2003LIBDIR)unionInput1.0.o

objs1=outputDatagramDef.20081003.o
datagram:	$(objs1)
	cc -o datagram $(OSSPECLINK) $(objs1) $(allobjs) $(libs) $(OSSPECLIBS)

objs2=genCmmPackClientAPI.20081003.o
genCmmPackClientAPI:	$(objs2)
	cc -o genCmmPackClientAPI $(OSSPECLINK) $(objs2) $(allobjs) $(libs) $(OSSPECLIBS)

objs3=genAllCmmPackClientAPI.20081003.o
genAllCmmPackClientAPI:	$(objs3)
	cc -o genAllCmmPackClientAPI $(OSSPECLINK) $(objs3) $(allobjs) $(libs) $(OSSPECLIBS)

objs4=genCmmPackSvrFun.20081003.o
genCmmPackSvrFun:	$(objs4)
	cc -o genCmmPackSvrFun $(OSSPECLINK) $(objs4) $(allobjs) $(libs) $(OSSPECLIBS)

objs5=genCmmPackSvrAPI.20081003.o
genCmmPackSvrAPI:	$(objs5)
	cc -o genCmmPackSvrAPI $(OSSPECLINK) $(objs5) $(allobjs) $(libs) $(OSSPECLIBS)

objs6=genAllCmmPackSvrAPI.20081003.o
genAllCmmPackSvrAPI:	$(objs6)
	cc -o genAllCmmPackSvrAPI $(OSSPECLINK) $(objs6) $(allobjs) $(libs) $(OSSPECLIBS)

finalProduct:
		cp $(INCDIR)unionDatagramDef.h			$(CMM2003INCDIR)
		cp $(INCDIR)unionGenClientFunUsingCmmPack.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionGenSvrFunUsingCmmPack.h	$(CMM2003INCDIR)
		cp $(INCDIR)unionDatagramClassDef.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionPackAndFunRelation.h		$(CMM2003INCDIR)
		mv *.o						$(CMM2003LIBDIR)
		mv datagram					$(CMM2003BINDIR)
		mv genCmmPackClientAPI				$(CMM2003BINDIR)
		mv genAllCmmPackClientAPI			$(CMM2003BINDIR)
		mv genCmmPackSvrAPI				$(CMM2003BINDIR)
		mv genAllCmmPackSvrAPI				$(CMM2003BINDIR)
		mv genCmmPackSvrFun				$(CMM2003BINDIR)
