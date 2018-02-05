include dirAndLib.def

objs = $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEE[1-9]*.o))
service = $(basename $(basename $(subst essc5Interface,lib,$(objs))))
sos =  $(addsuffix .so,$(service))

all:	dllGenerateTimeLib	\
	$(sos)		\
	cleanLib

dllGenerateTimeLib:
	genProductGenerateTime  
cleanLib:
	rm -f unionProductGenerateTime.a

dlVersionLibs=$(dbTypeLibs) $(CMM2003LIBDIR)/unionDLVersion.20130301.o unionProductGenerateTime.a

hsmCmdLibs =	$(CMM2003LIBDIR)unionHsmCmd.20120521.o				\
		$(CMM2003LIBDIR)unionHsmCmdJK.20140409.o			\
		$(CMM2003LIBDIR)3DesRacalSyntaxRules1.0.o			\
		$(CMM2003LIBDIR)emv2000AndPbocSyntaxRules.20120521.o

commLibs =	$(hsmCmdLibs)		\
		$(keyDBLibs)		\
		$(dlVersionLibs)

libEEE%.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(PROLIBDIR)$(subst .so,.20130301.o,$(subst lib,essc5Interface,$@)) $(commLibs)

libEEE7	=	$(PROLIBDIR)essc5InterfaceEEE7.20130301.o		\
		$(CMM2003LIBDIR)unionCardBinTBL1.0.db.20141025.o

libEEE7.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEE7) $(commLibs)
