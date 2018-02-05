include dirAndLib.def

objs = $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceE[1-8]*.o))
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

libE110	=	$(PROLIBDIR)essc5InterfaceE110.20130301.o	\
		$(PROLIBDIR)essc5InterfaceE111.20130301.o

libE120	=	$(PROLIBDIR)essc5InterfaceE120.20130301.o	\
		$(PROLIBDIR)essc5InterfaceE121.20130301.o

libE110.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE110) $(commLibs)

libE120.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE120) $(commLibs)

libE118 = 	$(PROLIBDIR)essc5InterfaceE118.20130301.o 	\
		$(PROLIBDIR)essc5InterfaceE110.20130301.o 
libE118.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE118) $(commLibs)

libE152	=	$(PROLIBDIR)essc5InterfaceE152.20130301.o	\
		$(PROLIBDIR)essc5InterfaceE151.20130301.o	\
		$(PROLIBDIR)essc5InterfaceE150.20130301.o

libE202 = 	$(PROLIBDIR)essc5InterfaceE202.20130301.o 	\
		$(PROLIBDIR)essc5InterfaceE204.20130301.o 
libE202.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE202) $(commLibs)

libE152.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE152) $(commLibs)

libE401	=	$(PROLIBDIR)essc5InterfaceE401.20130301.o	\
		$(unionCertLibs)

libE401.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE401) $(commLibs)

libE402	=	$(PROLIBDIR)essc5InterfaceE402.20130301.o	\
		$(unionCertLibs)

libE402.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE402) $(commLibs)

libE403	=	$(PROLIBDIR)essc5InterfaceE403.20130301.o	\
		$(CMM2003LIBDIR)base64.o			\
		$(unionCertLibs)

libE403.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE403) $(commLibs)

libE404	=	$(PROLIBDIR)essc5InterfaceE404.20130301.o	\
		$(CMM2003LIBDIR)base64.o			\
		$(unionCertLibs)

libE404.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE404) $(commLibs)

libE405	=	$(PROLIBDIR)essc5InterfaceE405.20130301.o	\
		$(unionCertLibs)

libE405.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE405) $(commLibs)

libE501	=	$(PROLIBDIR)essc5InterfaceE501.20130301.o	\
		$(unionCertLibs)

libE501.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE501) $(commLibs)

libE502	=	$(PROLIBDIR)essc5InterfaceE502.20130301.o	\
		$(unionCertLibs)

libE502.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE502) $(commLibs)

libE503	=	$(PROLIBDIR)essc5InterfaceE503.20130301.o	\
		$(unionCertLibs)

libE503.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE503) $(commLibs)

libE504	=	$(PROLIBDIR)essc5InterfaceE504.20130301.o	\
		$(unionCertLibs)

libE504.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE504) $(commLibs)


libE801 = 	$(PROLIBDIR)essc5InterfaceE801.20130301.o       \
                $(unionCertLibs)

libE801.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libE801) $(commLibs)

libKeySynchro =	$(PROLIBDIR)keyAutoManager.20130301.o

libKeySynchro.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libKeySynchro) $(commLibs)

	
