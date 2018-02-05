# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@
# 这个编译开关必须增加
DEFINES = $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = unionModuleDef.20080925.o	\
	unionStructDef.20080925.o	\
	unionUnionDef.20080925.o	\
	unionPointerDef.20080925.o		\
	unionSimpleTypeDef.20080925.o		\
	unionConstDef.20080925.o		\
	unionArrayDef.20080925.o		\
	unionVariableDef.20080925.o		\
	unionFunDef.20080925.o		\
	unionEnumDef.20080925.o		\
	unionVarTypeDef.20080925.o		\
	unionVarValue.20080925.o		\
	unionProgramDef.20080925.o		\
	unionCalcuOperator.20080925.o		\
	unionDesignKeyWord.20080925.o		\
	unionVarDef.20080925.o

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

includes = $(INCDIR)unionCalcuOperator.h	\
	$(INCDIR)unionAutoGenFileFunGrp.h	\
	$(INCDIR)unionVariableDef.h	\
	$(INCDIR)unionStructDef.h	\
	$(INCDIR)unionUnionDef.h	\
	$(INCDIR)unionEnumDef.h	\
	$(INCDIR)unionArrayDef.h	\
	$(INCDIR)unionSimpleTypeDef.h	\
	$(INCDIR)unionPointerDef.h	\
	$(INCDIR)unionFunDef.h	\
	$(INCDIR)unionVarDef.h	\
	$(INCDIR)unionVarTypeDef.h	\
	$(INCDIR)unionVarValue.h	\
	$(INCDIR)unionModuleDef.h	\
	$(INCDIR)unionProgramDef.h	\
	$(INCDIR)unionConstDef.h	\
	$(INCDIR)unionGenFunTestProgram.h	\
	$(INCDIR)unionDesignKeyWord.h
	
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
	
