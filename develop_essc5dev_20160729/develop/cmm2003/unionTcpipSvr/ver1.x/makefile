# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionTcpipSvr
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

#INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(CMM2003OPENDIR)/include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@

# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)
SPEC_DEFINES = -D _UNIX  $(OSSPEC) -D _maxSizeOfClientMsg_8192_
SPEC_DEFINES1 = -D _UNIX  -D _client_use_ebcdic_ $(OSSPEC)
SPEC_DEFINES2 = -D _UNIX  $(OSSPEC) -D _isXMLPackage_ -D _maxSizeOfClientMsg_8192_
SPEC_DEFINES3 = -D _UNIX  $(OSSPEC) -D _isXMLPackage_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
SPEC_COMPILE3	= $(CC) $(SPEC_DEFINES3) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs))

objs = tcpipSvrMain.20080511.o			\
	tcpipSvrMain.20130301.o			\
	tcpipSvrMainForHsmSvr.20080511.o	\
	shortConnTcpipSvrMain.20080511.o	\
	shortConnTcpipSvrMain.20130301.o	\
	shortConnTcpipSvrMainForShared.20080511.o	\
	shortConnTcpipSvrMainForHsmSvr.20080511.o	\
	unionServiceErrCounter.20080511.o	\
	tcpipSvrRunningEnv.default.o	\
	tcpipSvrRunningEnv.null.o	\
	synchTCPIPSvr.20080511.o	\
	defaultRunningEnv.20080511.o	\
	defaultRunningEnv.withoutFreeRes.20080511.o	\
	synchTCPIPSvr.withoutLen.20080511.o	\
	tcpipSvrTaskName.default.o	\
	simuFunSvr.1.0.o	\
	synchTCPIPSvr.20080511.8192.o	\
	synchTCPIPSvr.withoutLen.20080511.8192.o	\
	synchTCPIPSvr.ebcdic.20080511.o	\
	synchTCPIPSvr.20080511.8192.isXMLPackage.o	\
	synchTCPIPSvr.20080511.isXMLPackage.o	\
	setXMLResponsePackageForError.20080511.o	\
	synchTCPIPSvr.xml.20130301.o			\
	packageConvert.null.20130301.o

all:	makeall		

makeall:	$(objs)

.SUFFIXES:.c.o
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
synchTCPIPSvr.20080511.8192.o:  synchTCPIPSvr.20080511.c synchTCPIPSvr.20080511.d
else
synchTCPIPSvr.20080511.8192.o:  synchTCPIPSvr.20080511.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
synchTCPIPSvr.withoutLen.20080511.8192.o:  synchTCPIPSvr.withoutLen.20080511.c synchTCPIPSvr.withoutLen.20080511.d
else
synchTCPIPSvr.withoutLen.20080511.8192.o:  synchTCPIPSvr.withoutLen.20080511.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
synchTCPIPSvr.ebcdic.20080511.o:  synchTCPIPSvr.20080511.c synchTCPIPSvr.20080511.d
else
synchTCPIPSvr.ebcdic.20080511.o:  synchTCPIPSvr.20080511.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@
	
ifeq ($(UNION_USE_GCC),1)
synchTCPIPSvr.20080511.8192.isXMLPackage.o:  synchTCPIPSvr.20080511.c synchTCPIPSvr.20080511.d
else
synchTCPIPSvr.20080511.8192.isXMLPackage.o:  synchTCPIPSvr.20080511.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
synchTCPIPSvr.20080511.isXMLPackage.o:  synchTCPIPSvr.20080511.c synchTCPIPSvr.20080511.d
else
synchTCPIPSvr.20080511.isXMLPackage.o:  synchTCPIPSvr.20080511.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
setXMLResponsePackageForError.20080511.o:  setXMLResponsePackageForError.20080511.c setXMLResponsePackageForError.20080511.d
else
setXMLResponsePackageForError.20080511.o:  setXMLResponsePackageForError.20080511.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = 	$(INCDIR)synchTCPIPSvr.h	\
		$(INCDIR)unionServiceErrCounter.h

.PHONY: clean install
install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)

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
	
