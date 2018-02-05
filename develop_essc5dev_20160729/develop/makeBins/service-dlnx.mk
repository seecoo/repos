include dirAndLib.def

objs1 = $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEH*.o))
service1 = $(basename $(basename $(subst essc5Interface,lib,$(objs1))))
sos1 =  $(addsuffix .so,$(service1))

#service = $(basename $(basename $(subst essc5Interface,"",$(objs))))
#sos =  $(service)

all:	dllGenerateTimeLib	\
	$(sos1)		\
	$(sos2)		\
	cleanLib

dllGenerateTimeLib:
	genProductGenerateTime  
cleanLib:
	rm -f unionProductGenerateTime.a

dlVersionLibs=$(dbTypeLibs) $(CMM2003LIBDIR)/unionDLVersion.20130301.o unionProductGenerateTime.a

hsmCmdLibs =	$(CMM2003LIBDIR)unionHsmCmd.20120521.o				\
		$(CMM2003LIBDIR)unionHsmCmdJK.20140409.o			\
		$(CMM2003LIBDIR)unionHsmCmd.sjj1127.20120521.o			\
		$(CMM2003LIBDIR)unionHsmCmd.jtb.20141021.o			\
		$(CMM2003LIBDIR)3DesRacalSyntaxRules1.0.o			\
		$(CMM2003LIBDIR)emv2000AndPbocSyntaxRules.20120521.o

commLibs =	$(hsmCmdLibs)		\
		$(keyDBLibs)		\
		$(dlVersionLibs)

libEEH%.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(PROLIBDIR)$(subst .so,.20130301.o,$(subst lib,essc5Interface,$@)) $(commLibs)

