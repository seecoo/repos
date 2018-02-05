# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionSQL
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
SPEC_DEFINES = -D _UNIX  $(OSSPEC) -D _withFileLock_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs))

objs = unionSQLQueryCondition.20080724.o	\
	unionRecFile.20080724.o	\
	unionRec.20080724.o	\
	unionFldValueAssign.20080724.o	\
	unionFldGrp.20080724.o	\
	unionEnumValueDef.20080724.o	\
	unionTBLRecInterface.20080724.o	\
	unionTBLQueryConf.20080724.o	\
	unionRecFile.20080724.lockFile.o
	
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
unionRecFile.20080724.lockFile.o:  unionRecFile.20080724.c unionRecFile.20080724.d
else
unionRecFile.20080724.lockFile.o:  unionRecFile.20080724.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)unionFldValueAssign.h	\
	$(INCDIR)unionFldGrp.h	\
	$(INCDIR)unionTBLRecInterface.h	\
	$(INCDIR)unionTBLQueryConf.h	\
	$(INCDIR)unionRecFile.h	\
	$(INCDIR)unionRec0.h	\
	$(INCDIR)unionEnumValueDef.20080724.h	\
	$(INCDIR)unionSQLQueryCondition.h	\
	$(INCDIR)unionWordList.h

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
	
