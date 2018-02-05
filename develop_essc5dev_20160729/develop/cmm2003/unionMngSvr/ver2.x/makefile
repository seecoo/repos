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

objs=	localDBComplexSvr.20120521.o			\
	mngSvrDBTableDesignService.20120521.o		\
	unionGenMenuDefFromTBL.20120521.o				\
	unionVerifyOperatorLogin.20120521.o

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
#	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)

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
	
