# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = unionConstDefInDB.20080925.o		\
	unionProgramDefInDB.20080925.o		\
	unionArrayDefInDB.20080925.o		\
	unionEnumDefInDB.20080925.o		\
	unionSimpleTypeDefInDB.20080925.o		\
	unionStructDefInDB.20080925.o		\
	unionUnionDefInDB.20080925.o		\
	unionFunDefInDB.20080925.o		\
	unionVariableDefInDB.20080925.o		\
	unionModuleDefInDB.20080925.o		\
	unionPointerDefInDB.20080925.o		\
	unionVarTypeDefInDB.20080925.o

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

includes = $(INCDIR)unionDefaultVarTypeDefDir.h	\
	$(INCDIR)unionConstDefFile.h	\
	$(INCDIR)unionProgramDefFile.h	\
	$(INCDIR)unionArrayDefFile.h	\
	$(INCDIR)unionEnumDefFile.h	\
	$(INCDIR)unionSimpleTypeDefFile.h	\
	$(INCDIR)unionStructDefFile.h	\
	$(INCDIR)unionUnionDefFile.h	\
	$(INCDIR)unionFunDefFile.h	\
	$(INCDIR)unionVariableDefFile.h	\
	$(INCDIR)unionModuleDefFile.h	\
	$(INCDIR)unionPointerDefFile.h	\
	$(INCDIR)unionVarTypeDefFile.h
	
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
	
