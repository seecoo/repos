# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionCommConf
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC) -D _longConnServerMustHaveConns_
SPEC_DEFINES = -D _UNIX  $(OSSPEC) -D _longConnServerMustHaveConns_ -D _maxNumOfCommConf_500_
SPEC_DEFINES1 = -D _UNIX  $(OSSPEC) -D _longConnServerMustHaveConns_ -D _maxNumOfCommConf_2000_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
objs =	$(patsubst %.c,%.o,$(srcs)) 
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs += unionCommConf.500.20051009.o	\
	commConfMon.500.20051009.o	\
	unionCommConf.2000.20051009.o	\
	commConfMon.2000.20051009.o

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
unionCommConf.500.20051009.o:  unionCommConf.20051009.c unionCommConf.20051009.d
else
unionCommConf.500.20051009.o:  unionCommConf.20051009.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
commConfMon.500.20051009.o:  commConfMon.20051009.c commConfMon.20051009.d
else
commConfMon.500.20051009.o:  commConfMon.20051009.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionCommConf.2000.20051009.o:  unionCommConf.20051009.c unionCommConf.20051009.d
else
unionCommConf.2000.20051009.o:  unionCommConf.20051009.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
commConfMon.2000.20051009.o:  commConfMon.20051009.c commConfMon.20051009.d
else
commConfMon.2000.20051009.o:  commConfMon.20051009.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

.PHONY: clean install
install:
	@sh cp_head.x  "$(INCDIR)unionCommConf.h" $(CMM2003INCDIR)
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
	
