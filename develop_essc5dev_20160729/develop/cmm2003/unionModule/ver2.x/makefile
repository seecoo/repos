# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionModule
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@
# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)
SPEC_DEFINES = -D _UNIX  $(OSSPEC) -D _useComplexDB_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs =	unionModule2.0.20050915.o		\
	unionModule2.1.20060414.o		\
	unionModule2.1.20060726.o		\
	unionModule2.1.20061227.o		\
	unionModule2.1.20071026.o		\
	unionModule2.1.20090421.o		\
	unionModule2.1.20090421.defFile.o	\
	mngModule2.0.20050915.o	\
	mngModule2.0.20061227.o	\
	unionModule.2.1.20071026.o	\
	unionModule2.0.20050915.a	\
	mngModule2.0.20050915.a

all:	makeall

makeall:	$(objs)

.SUFFIXES:.c .o .a
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

.c.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o
	@$(RM) $*.o

ifeq ($(UNION_USE_GCC),1)
unionModule2.1.20090421.o:  unionModule2.1.20090421.c unionModule2.1.20090421.d
else
unionModule2.1.20090421.o:  unionModule2.1.20090421.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionModule2.1.20090421.defFile.o:  unionModule2.1.20090421.defFile.c unionModule2.1.20090421.defFile.d
else
unionModule2.1.20090421.defFile.o:  unionModule2.1.20090421.defFile.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionModule.2.1.20071026.o:  unionModule2.1.20071026.c unionModule2.1.20071026.d
else
unionModule.2.1.20071026.o:  unionModule2.1.20071026.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)/unionModule.h	\
	$(INCDIR)/unionSharedMemory.h

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
	
