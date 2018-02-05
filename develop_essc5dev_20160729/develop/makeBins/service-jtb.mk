include dirAndLib.def

objs1 = $(notdir $(shell ls $(PROLIBDIR)essc5InterfaceEEG*.o))
service1 = $(basename $(basename $(subst essc5Interface,lib,$(objs1))))
service2 = $(basename $(basename $(subst essc5UIInterface,lib,$(objs2))))
sos1 =  $(addsuffix .so,$(service1))

#service = $(basename $(basename $(subst essc5Interface,"",$(objs))))
#sos =  $(service)

all:	dllGenerateTimeLib	\
	$(sos1)		\
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

libEEG%.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(PROLIBDIR)$(subst .so,.20130301.o,$(subst lib,essc5Interface,$@)) $(commLibs)

 
libEEG1	=	$(PROLIBDIR)essc5InterfaceEEG1.20130301.o	\
		$(PROLIBDIR)essc5InterfaceEEGI.20130301.o	\

libEEG1.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG1) $(commLibs)

libEEG2	=	$(PROLIBDIR)essc5InterfaceEEG2.20130301.o	\

libEEG2.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG2) $(commLibs)

libEEG3	=	$(PROLIBDIR)essc5InterfaceEEG3.20130301.o	\

libEEG3.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG3) $(commLibs)

libEEG4	=	$(PROLIBDIR)essc5InterfaceEEG4.20130301.o	\

libEEG4.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG4) $(commLibs)

libEEG5	=	$(PROLIBDIR)essc5InterfaceEEG5.20130301.o	\
		$(PROLIBDIR)essc5InterfaceEEGJ.20130301.o	\

libEEG5.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG5) $(commLibs)

libEEG6	=	$(PROLIBDIR)essc5InterfaceEEG6.20130301.o	\

libEEG6.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG6) $(commLibs)

libEEG7	=	$(PROLIBDIR)essc5InterfaceEEG7.20130301.o	\

libEEG7.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG7) $(commLibs)

libEEG8	=	$(PROLIBDIR)essc5InterfaceEEG8.20130301.o	\

libEEG8.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG8) $(commLibs)

libEEG9	=	$(PROLIBDIR)essc5InterfaceEEG9.20130301.o	\

libEEG9.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEG9) $(commLibs)

libEEGA =	$(PROLIBDIR)essc5InterfaceEEGA.20130301.o	\

libEEGA.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGA) $(commLibs)

libEEGB =	$(PROLIBDIR)essc5InterfaceEEGB.20130301.o	\

libEEGB.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGB) $(commLibs)

libEEGC =	$(PROLIBDIR)essc5InterfaceEEGC.20130301.o	\

libEEGC.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGC) $(commLibs)

libEEGD =	$(PROLIBDIR)essc5InterfaceEEGD.20130301.o	\

libEEGD.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGD) $(commLibs)

libEEGE =	$(PROLIBDIR)essc5InterfaceEEGE.20130301.o	\

libEEGE.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGE) $(commLibs)

libEEGF =	$(PROLIBDIR)essc5InterfaceEEGF.20130301.o	\

libEEGF.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGF) $(commLibs)

libEEGG =	$(PROLIBDIR)essc5InterfaceEEGG.20130301.o	\

libEEGG.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGG) $(commLibs)

libEEGH =	$(PROLIBDIR)essc5InterfaceEEGH.20130301.o	\

libEEGH.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGH) $(commLibs)

libEEGI =	$(PROLIBDIR)essc5InterfaceEEGI.20130301.o	\

libEEGI.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGI) $(commLibs)

libEEGJ =	$(PROLIBDIR)essc5InterfaceEEGJ.20130301.o	\

libEEGJ.so:
	$(COMPILESO) $(TESTBINDIR)$@ $(libEEGJ) $(commLibs)


