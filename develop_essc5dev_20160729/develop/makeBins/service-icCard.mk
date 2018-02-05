include dirAndLib.def

objs = $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceE8*.o))
service = $(basename $(basename $(subst essc5Interface,lib,$(objs))))
sos =  $(addsuffix .so,$(service))

#service = $(basename $(basename $(subst essc5Interface,"",$(objs))))
#sos =  $(service)

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
		$(CMM2003LIBDIR)unionHsmCmd.sjj1127.20120521.o			\
		$(CMM2003LIBDIR)3DesRacalSyntaxRules1.0.o			\
		$(CMM2003LIBDIR)emv2000AndPbocSyntaxRules.20120521.o

commLibs =	$(hsmCmdLibs)		\
		$(keyDBLibs)		\
		$(dlVersionLibs)

libE%.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(PROLIBDIR)$(subst .so,.20130301.o,$(subst lib,essc5Interface,$@)) $(commLibs)

libE801 =       $(PROLIBDIR)essc5InterfaceE801.20130301.o       \
                $(unionCertLibs)

libE801.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE801) $(commLibs)
