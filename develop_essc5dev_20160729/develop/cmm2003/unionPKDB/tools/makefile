# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@

# 这个编译开关必须增加
DEFINES = -D _UnionPKDB_2_x_ -D _UnionPK_3_x_ $(OSSPEC)
SPEC_DEFINES = -D _UnionPKDB_2_x_ -D _UnionPK_3_x_ -D _UnionPK_4_x_ -D _UnionSJL06_2_x_Above_ $(OSSPEC)
SPEC_DEFINES1 = -D _UnionTask_3_x_ -D _UnionPKDB_2_x_ -D _UnionPK_3_x_ -D _UnionPK_4_x_ $(OSSPEC)
SPEC_DEFINES2 = -D _UnionPKDB_2_x_ -D _UnionPK_3_x_ -D _UnionPK_4_x_ $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL)  -I $(CMM2003DIR)/unionComplexDB/include/ -I $(CMM2003DIR)/unionMngSvr/include/ -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs =	pkDBMon.usePK3.x.20070925.o	\
	pkDBMon.usePK4.x.20070925.o	\
	directOutputPKs4.x.o	\
	pkDBRecord1.0.usePK4.x.o	\
	pkDBSvrOperation1.0.usePK4.x.o
	
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
pkDBMon.usePK3.x.20070925.o:  pkDBMon.20070925.c pkDBMon.20070925.d
else
pkDBMon.usePK3.x.20070925.o:  pkDBMon.20070925.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
pkDBMon.usePK4.x.20070925.o:  pkDBMon.20070925.c pkDBMon.20070925.d
else
pkDBMon.usePK4.x.20070925.o:  pkDBMon.20070925.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
directOutputPKs4.x.o:  directOutputPKs.1.0.c directOutputPKs.1.0.d
else
directOutputPKs4.x.o:  directOutputPKs.1.0.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
pkDBRecord1.0.usePK4.x.o:  pkDBRecord1.0.c pkDBRecord1.0.d
else
pkDBRecord1.0.usePK4.x.o:  pkDBRecord1.0.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
pkDBSvrOperation1.0.usePK4.x.o:  pkDBSvrOperation1.0.c pkDBSvrOperation1.0.d
else
pkDBSvrOperation1.0.usePK4.x.o:  pkDBSvrOperation1.0.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

.PHONY: clean install
install:

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
	
