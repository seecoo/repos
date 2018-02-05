HOMEDIR = $(CMM2003DIR)/unionMngSvr
INCDIR = $(HOMEDIR)/include/

SQLDIR	= $(CMM2003DIR)/unionSQL
INC2DIR	= $(SQLDIR)/include/

DESKEDBDIR	= $(CMM2003DIR)/unionDesKeyDB
INC3DIR	= $(DESKEDBDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INC2DIR) -I $(INC3DIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)  -D _unionCmm2003_


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	mngSvrMain.20060808.o			\
	mngSvrManagementSoftwareSvr.default.20060808.o	\
	mngSvrDefaultHsm.1.0.o	\
	mngSvrSharedFunctions.20060808.o	\
	mngSvrPreDefinedMain.20060808.o	\
	unionMngSvrAutoFld.20060808.o		\
	unionMngSvrAutoFld.default.20060808.o		\
	unionMngSvrList.20090421.o		\
	mngSvrRunningEnv.20060808.o	\
	nonMngSvrRunningEnv.20060808.o	\
	mngSvrOnlineSSN.20090421.o		\
	mngSvrOnlineSSN.default.20090421.o		\
	mngSvrServicePackage.20060808.o		\
	mngSvrRecTransferred.20060808.o		\
	mngSvrDefaultUserService.20060808.o		\
	mngSvrReservedUserService.20060808.o		\
	mngSvDefaultSpecAppSuffixOperation.1.0.o	\
	mngSvrReservedSuffixOperation.1.0.o		\
	mngSvrReservedSuffixOperation.1.0.o		\
	mngSvrDefaultOtherSvrService.20060808.o	\
	mngSvrCommProtocol.20060808.o		\
	mngSvrAutoAppOnlineLogWriter.20060808.o		\
	mngSvrAutoAppOnlineLog.20060808.o		\
	unionFileManager.20090421.o		\
	unionMngSvrBrothers.20090421.o		\
	unionMngSvrBrothers.default.20090421.o		\
	mngSvrSharedMemoryImage.default.20060808.o		\
	mngSvrStandardInteprotor.20060808.o		\
	mngSvrTeller.20060808.o		\
	mngSvrFileSvr.20060808.o		\
	mngSvrConfFileName.20060808.o		\
	localDBComplexSvr.20060808.o		\
	operationControl.20060808.o		\
	operationControl.useTBL.20060808.o		\
	mngSvrDBTableDesignService.20060808.o		\
	mngSvrDBTableDesignService.default.20060808.o		\
	unionTeller.20090421.o			\
	unionMngServiceOnline.20090421.o		\
	unionResID.20060808.o			\
	mngSvrPassword.20060808.o			\
	unionVerifyOperatorLogin.20090421.o	\
	unionVerifyOperatorLogin.20120521.o	\
	unionVerifyOperatorLogin.default.20090421.o	\
	unionDBConf1.0.o	\
	unionRealDBRecord.noDB.20060808.o  \
	unionMngSvrAutoFld.defspec.20060808.o
makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)mngSvrFldTagDef.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrAutoAppOnlineLogWriter.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrAutoAppOnlineLog.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrOnlineSSN.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionMngServiceOnline.h	$(CMM2003INCDIR)
	cp $(INCDIR)unionFileManager.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionMngSvrList.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrFileSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionResID.h		$(CMM2003INCDIR)
	cp $(INCDIR)operationControl.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrServicePackage.h	$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrCommProtocol.h	$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrTeller.h		$(CMM2003INCDIR)
	cp $(INCDIR)simuMngSvrLocally.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrRecTransferred.h	$(CMM2003INCDIR)
	cp $(INCDIR)unionDBConf.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionTeller.h		$(CMM2003INCDIR)
	cp $(INCDIR)mngSvrConfFileName.h	$(CMM2003INCDIR)
	cp $(INCDIR)unionMngSvrBrothers.h	$(CMM2003INCDIR)
