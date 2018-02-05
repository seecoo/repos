include dirAndLib.def

objs = $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEA*.o))
objs += $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEB*.o))
objs += $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEC*.o))
objs += $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEED*.o))
objs += $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEE*.o))
#objs += $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEG*.o))
#objs += $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEH*.o))

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
		$(CMM2003LIBDIR)unionHsmCmd.jtb.20141021.o			\
		$(CMM2003LIBDIR)3DesRacalSyntaxRules1.0.o			\
		$(CMM2003LIBDIR)emv2000AndPbocSyntaxRules.20120521.o

commLibs =	$(hsmCmdLibs)		\
		$(keyDBLibs)		\
		$(dlVersionLibs)

libE%.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(PROLIBDIR)$(subst .so,.20130301.o,$(subst lib,essc5Interface,$@)) $(commLibs)

libEED1	=	$(PROLIBDIR)essc5InterfaceEED1.20130301.o	\
		$(unionCertLibs)

libEED1.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEED1) $(commLibs)

libEED3	=	$(PROLIBDIR)essc5InterfaceEED3.20130301.o	\
		$(CMM2003LIBDIR)UnionMD5_1.0.o

libEED3.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEED3) $(commLibs)

