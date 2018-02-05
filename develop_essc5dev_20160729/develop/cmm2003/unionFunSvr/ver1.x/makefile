# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionFunSvr
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)
SPEC_DEFINES = $(OSSPEC) -D _sharedHsmTaskLog_
SPEC_DEFINES1 = $(OSSPEC) -D _useRealDB_
SPEC_DEFINES2 = $(OSSPEC) -D _useRealDB_ -D _useArgv1AsTaskName_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs =	unionFunSvrMain.20080511.o			\
	unionFunSvrMain.20130301.o			\
	unionFunSvrMainForHsmSvr.20080511.o		\
	unionFunSvrName.20080511.o			\
	unionFunSvrForKms.20080511.o			\
	unionFunSvr.20080511.o				\
	unionFunSvr.20130301.o				\
	unionFunSvr.sharedHsmTaskLog.20080511.o		\
	unionFunSvr.sharedHsmTaskLog.20130301.o		\
	unionFunSvrForKms.sharedHsmTaskLog.20080511.o	\
	unionFunSvrMain.useRealDB.20080511.o		\
	unionFunSvrMainForHsmSvr.useRealDB.20080511.o	\
	unionFunSvrMain.useRealDB.useArgv1AsTaskName.20080511.o	\
	unionFunSvrMainForHsmSvr.useRealDB.useArgv1AsTaskName.20080511.o	\
	xmlPackageInterprotorService.20130301.o		\
	unionFunSvrInterfaceService.useDL.20130301.o	\
	hsmTaskSvr.20090421.o				\
	unionRegisterRunningTime.20130301.o		\
	unionDaemonTask.20130301.o

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
unionFunSvr.sharedHsmTaskLog.20080511.o:  unionFunSvr.20080511.c unionFunSvr.20080511.d
else
unionFunSvr.sharedHsmTaskLog.20080511.o:  unionFunSvr.20080511.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFunSvr.sharedHsmTaskLog.20130301.o:  unionFunSvr.20130301.c unionFunSvr.20130301.d
else
unionFunSvr.sharedHsmTaskLog.20130301.o:  unionFunSvr.20130301.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFunSvrForKms.sharedHsmTaskLog.20080511.o:  unionFunSvrForKms.20080511.c unionFunSvrForKms.20080511.d
else
unionFunSvrForKms.sharedHsmTaskLog.20080511.o:  unionFunSvrForKms.20080511.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFunSvrMain.useRealDB.20080511.o:  unionFunSvrMain.20080511.c unionFunSvrMain.20080511.d
else
unionFunSvrMain.useRealDB.20080511.o:  unionFunSvrMain.20080511.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFunSvrMainForHsmSvr.useRealDB.20080511.o:  unionFunSvrMainForHsmSvr.20080511.c unionFunSvrMainForHsmSvr.20080511.d
else
unionFunSvrMainForHsmSvr.useRealDB.20080511.o:  unionFunSvrMainForHsmSvr.20080511.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFunSvrMain.useRealDB.useArgv1AsTaskName.20080511.o:  unionFunSvrMain.20080511.c unionFunSvrMain.20080511.d
else
unionFunSvrMain.useRealDB.useArgv1AsTaskName.20080511.o:  unionFunSvrMain.20080511.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFunSvrMainForHsmSvr.useRealDB.useArgv1AsTaskName.20080511.o:  unionFunSvrMainForHsmSvr.20080511.c unionFunSvrMainForHsmSvr.20080511.d
else
unionFunSvrMainForHsmSvr.useRealDB.useArgv1AsTaskName.20080511.o:  unionFunSvrMainForHsmSvr.20080511.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)unionFunSvrName.h	\
	$(INCDIR)unionFunSvr.h		\
	$(INCDIR)registerRunningTime.h
	
.PHONY: clean install
install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)
#	@sh cp_head.x  ../include/ $(CMM2003INCDIR)

clean:
	$(RM) $(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))

show:
	@make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
	
