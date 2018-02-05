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

objs = unionGenTBLDefFromTBL.20090421.o	\
	unionComplexField.20090421.o	\
	unionMainMenu.20090421.o	\
	unionTransferComplexDBDef.20090421.o	\
	moveComplexDBDefIntoDBMain.20090421.o	\
	unionTableList.20090421.o	\
	unionGenIncFromTBL.20080925.o	\
	unionOperationList.20090421.o	\
	unionOperationAuthorization.20090421.o	\
	unionTableMenu.20090421.o	\
	unionMenuItemType.20090421.o	\
	unionMngSvrOperationType.20090421.o	\
	unionOperatorType.20090421.o	\
	unionLevel2Menu.20090421.o	\
	unionSecondaryMenu.20090421.o	\
	unionGenSQLFromTBLDefList.20090421.o	\
	unionViewList.20090421.o	\
	unionMenuDef.20090421.o	\
	unionMenuItemDef.20090421.o	\
	unionMenuItemOperationDef.20090421.o	\
	unionEnumValueDef.20090421.o	\
	unionDataPackFldList.20090421.o	\
	unionSecurityServiceDef.20090421.o	\
	unionTableField.20090421.o	\
	outputKeyWordDef.20080925.o	\
	unionGenIncFromTBLMain.20080925.o	\
	unionAutoGenFileFunGrp.20080925.o	\
	unionGenFunTestProgram.20080925.o	\
	unionGenFunListTestProgram.20080925.o	\
	unionVarAssignment.20080925.o	\
	unionUserInterfaceMaintainer.1.0.o	\
	unionUserInterfaceItemGrp.1.0.o	\
	unionQueueDef.1.0.o	\
	unionTree.1.0.o

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

.PHONY: clean install
install:
	@sh cp_head.x  "$(INCDIR)" $(CMM2003INCDIR)

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
	
