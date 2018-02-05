# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionMngSvr
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

SQLDIR	= $(CMM2003DIR)/unionSQL
INC2DIR	= $(SQLDIR)/include/

DESKEDBDIR	= $(CMM2003DIR)/unionDesKeyDB
INC3DIR	= $(DESKEDBDIR)/include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = $(OSSPEC)  -D _unionCmm2003_
SPEC_DEFINES = -D _UNIX  $(OSSPEC) -D _runningAsBackSvr_
SPEC_DEFINES1 = $(OSSPEC)  -D _unionCmm2003_ -D _useStaticPwd_
SPEC_DEFINES2 = -D _UNIX  $(OSSPEC) -D _useTableDefList_
SPEC_DEFINES3 = $(OSSPEC)  -D _usingEsscService_
SPEC_DEFINES4 = -D _UNIX  $(OSSPEC) -D _noUseUserSpecOperation_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -I $(DEVDEPINCDIR)/dataInterface/ -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
SPEC_COMPILE3	= $(CC) $(SPEC_DEFINES3) $(INCL) -c
SPEC_COMPILE4	= $(CC) $(SPEC_DEFINES4) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

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
	unionMngSvrAutoFld.defspec.20060808.o	\
	localDBComplexSvr.runningAsBackSvr.20060808.o	\
	mngSvrStandardInteprotor.runningAsBackSvr.20060808.o	\
	unionVerifyOperatorLogin.useStaticPwd.20090421.o		\
	unionVerifyOperatorLogin.useStaticPwd.20120521.o	\
	localDBComplexSvr.useTableDefList.20060808.o	\
	unionVerifyOperatorLogin.usingEsscService.20090421.o	\
	unionVerifyOperatorLogin.usingEsscService.20120521.o	\
	localDBComplexSvr.noUseUserSpecOperation.20060808.o

all:	makeall

makeall:	$(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
localDBComplexSvr.runningAsBackSvr.20060808.o:  localDBComplexSvr.20060808.c localDBComplexSvr.20060808.d
else
localDBComplexSvr.runningAsBackSvr.20060808.o:  localDBComplexSvr.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
mngSvrStandardInteprotor.runningAsBackSvr.20060808.o:  mngSvrStandardInteprotor.20060808.c mngSvrStandardInteprotor.20060808.d
else
mngSvrStandardInteprotor.runningAsBackSvr.20060808.o:  mngSvrStandardInteprotor.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionVerifyOperatorLogin.useStaticPwd.20090421.o:  unionVerifyOperatorLogin.20090421.c unionVerifyOperatorLogin.20090421.d
else
unionVerifyOperatorLogin.useStaticPwd.20090421.o:  unionVerifyOperatorLogin.20090421.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionVerifyOperatorLogin.useStaticPwd.20120521.o:  unionVerifyOperatorLogin.20120521.c unionVerifyOperatorLogin.20120521.d
else
unionVerifyOperatorLogin.useStaticPwd.20120521.o:  unionVerifyOperatorLogin.20120521.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
localDBComplexSvr.useTableDefList.20060808.o:  localDBComplexSvr.20060808.c localDBComplexSvr.20060808.d
else
localDBComplexSvr.useTableDefList.20060808.o:  localDBComplexSvr.20060808.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionVerifyOperatorLogin.usingEsscService.20090421.o:  unionVerifyOperatorLogin.20090421.c unionVerifyOperatorLogin.20090421.d
else
unionVerifyOperatorLogin.usingEsscService.20090421.o:  unionVerifyOperatorLogin.20090421.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionVerifyOperatorLogin.usingEsscService.20120521.o:  unionVerifyOperatorLogin.20120521.c unionVerifyOperatorLogin.20120521.d
else
unionVerifyOperatorLogin.usingEsscService.20120521.o:  unionVerifyOperatorLogin.20120521.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
localDBComplexSvr.noUseUserSpecOperation.20060808.o:  localDBComplexSvr.20060808.c localDBComplexSvr.20060808.d
else
localDBComplexSvr.noUseUserSpecOperation.20060808.o:  localDBComplexSvr.20060808.c
endif
	@echo "CC $(SPEC_DEFINES4) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE4) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)mngSvrFldTagDef.h	\
	$(INCDIR)mngSvrAutoAppOnlineLogWriter.h	\
	$(INCDIR)mngSvrAutoAppOnlineLog.h	\
	$(INCDIR)mngSvrOnlineSSN.h	\
	$(INCDIR)unionMngServiceOnline.h	\
	$(INCDIR)unionFileManager.h	\
	$(INCDIR)unionMngSvrList.h	\
	$(INCDIR)mngSvrFileSvr.h	\
	$(INCDIR)unionResID.h	\
	$(INCDIR)operationControl.h	\
	$(INCDIR)mngSvrServicePackage.h	\
	$(INCDIR)mngSvrCommProtocol.h	\
	$(INCDIR)mngSvrTeller.h	\
	$(INCDIR)simuMngSvrLocally.h	\
	$(INCDIR)mngSvrRecTransferred.h	\
	$(INCDIR)unionDBConf.h	\
	$(INCDIR)unionTeller.h	\
	$(INCDIR)mngSvrConfFileName.h	\
	$(INCDIR)unionMngSvrBrothers.h	\
	$(INCDIR)unionLogonMode.h
	
.PHONY: clean install
install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)
#	@sh cp_head.x  ../include/ $(CMM2003INCDIR)

clean:
	$(RM) $(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))

show:
	make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
	
