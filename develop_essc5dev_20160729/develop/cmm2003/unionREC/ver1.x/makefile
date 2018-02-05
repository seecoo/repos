# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionREC
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES = -D _UNIX $(OSSPEC)
SPEC_DEFINES = -D _UNIX $(OSSPEC)  -D _useComplexDB_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs))

objs=	mngREC1.0.o				\
	mngREC1.0.a				\
	recMon1.0.o				\
	recMon1.0.a				\
	unionVarListDef.1.0.o			\
	unionREC1.0.o				\
	unionREC1.0.a				\
	unionREC.20050916.o			\
	unionREC.20050916.a			\
	unionREC.20060825.o			\
	unionREC.20060825.useComplexDB.o	\
	unionREC.20090421.defFile.o		\
	unionREC.20090421.db.o			\
	unionREC.db.20130301.o			\
	unionREC.20090421.o			\
	unionUnionREC.20090421.o		\
	unionUnionRECVar.20090421.o
	
all:	makeall		

makeall:	$(objs)	\

.SUFFIXES:.c.o
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

.c.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o 
	@$(RM) $*.o

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
unionREC.20060825.useComplexDB.o:  unionREC.20060825.c unionREC.20060825.d
else
unionREC.20060825.useComplexDB.o:  unionREC.20060825.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionREC.20090421.defFile.o:  unionREC.20090421.defFile.c unionREC.20090421.defFile.d
else
unionREC.20090421.defFile.o:  unionREC.20090421.defFile.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionREC.20090421.o:  unionREC.20090421.c unionREC.20090421.d
else
unionREC.20090421.o:  unionREC.20090421.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = 	$(INCDIR)/unionREC.h		\
		$(INCDIR)/unionVarListDef.h	\
		$(INCDIR)/unionUnionRECVar.h	\
		$(INCDIR)/unionUnionREC.h
		

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
	
