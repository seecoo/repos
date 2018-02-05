# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

INCLUDE = $(CMM2003DIR)/unionComplexDB/include/
INCLUDE2= $(CMM2003DIR)/unionMngSvr/include/

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@
# 这个编译开关必须增加
DEFINES = -D _UnionDesKey_3_x_ -D _UnionSJL06_2_x_Above_ $(OSSPEC)
SPEC_DEFINES = -O -DUNIX  $(OSSPEC) -D _UnionDesKey_3_x_ -D _UnionSJL06_2_x_Above_ -D _UnionTask_3_x_
SPEC_DEFINES1 = -D _UnionDesKey_3_x_ -D _UnionSJL06_2_x_Above_ -D _UnionDesKey_4_x_ $(OSSPEC)
SPEC_DEFINES2 = -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ $(OSSPEC)
SPEC_DEFINES3 = -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ -D _UnionDesKey_4_x_ -D _UnionSJL06_2_x_Above_ $(OSSPEC)
SPEC_DEFINES4 = -D _UnionTask_3_x_ -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ $(OSSPEC)
SPEC_DEFINES5 = -D _UnionTask_3_x_ -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ -D _UnionDesKey_4_x_ $(OSSPEC)
SPEC_DEFINES6 = -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ -D _UnionDesKey_4_x_ $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
SPEC_COMPILE3	= $(CC) $(SPEC_DEFINES3) $(INCL) -c
SPEC_COMPILE4	= $(CC) $(SPEC_DEFINES4) $(INCL) -c
SPEC_COMPILE5	= $(CC) $(SPEC_DEFINES5) $(INCL) -c
SPEC_COMPILE6	= $(CC) $(SPEC_DEFINES6) $(INCL) -I $(INCLUDE) -I $(INCLUDE2) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = batchInsertDesKeys.useDesKey4.x.20070907.o	\
	batchPrintDesKeys.useDesKey4.x.useSJL06_2.x.20070907.o	\
	desKeyDBMon.useDesKey3.x.20070925.o	\
	desKeyDBMon.useDesKey4.x.20070925.o	\
	mngKeyGrp.useDesKey4.x.20070925.o	\
	printDesKeys.useDesKey4.x.20070925.o	\
	desKeyDBMon.useDesKey4.x.20071211.o	\
	directInputDesKeys3.x.o	\
	directOutputDesKeys3.x.o	\
	desKeyRecOperation3.x.o	\
	directInputDesKeys4.x.o	\
	directOutputDesKeys4.x.o	\
	desKeyRecOperation4.x.o	\
	desKeyDBRecord1.0.useDesKey4.x.o	\
	desKeyDBSvrOperation1.0.useDesKey4.x.o	\
	genDesKey.desKey3.x.20050725.a	\
	genDesKey.desKey3.x.20060614.a	\
	outputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a	\
	inputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a

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
outputDesKeys.20050607.o:  outputDesKeys.20050607.c outputDesKeys.20050607.d
else
outputDesKeys.20050607.o:  outputDesKeys.20050607.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
inputDesKeys.20050607.o:  inputDesKeys.20050607.c inputDesKeys.20050607.d
else
inputDesKeys.20050607.o:  inputDesKeys.20050607.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
batchInsertDesKeys.useDesKey4.x.20070907.o:  batchInsertDesKeys.20070907.c batchInsertDesKeys.20070907.d
else
batchInsertDesKeys.useDesKey4.x.20070907.o:  batchInsertDesKeys.20070907.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
batchPrintDesKeys.useDesKey4.x.useSJL06_2.x.20070907.o:  batchPrintDesKeys.20070907.c batchPrintDesKeys.20070907.d
else
batchPrintDesKeys.useDesKey4.x.useSJL06_2.x.20070907.o:  batchPrintDesKeys.20070907.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
desKeyDBMon.useDesKey3.x.20070925.o:  desKeyDBMon.20070925.c desKeyDBMon.20070925.d
else
desKeyDBMon.useDesKey3.x.20070925.o:  desKeyDBMon.20070925.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
mngKeyGrp.useDesKey4.x.20070925.o:  mngKeyGrp.20070925.c mngKeyGrp.20070925.d
else
mngKeyGrp.useDesKey4.x.20070925.o:  mngKeyGrp.20070925.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
printDesKeys.useDesKey4.x.20070925.o:  printDesKeys.20070925.c printDesKeys.20070925.d
else
printDesKeys.useDesKey4.x.20070925.o:  printDesKeys.20070925.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
desKeyDBMon.useDesKey4.x.20071211.o:  desKeyDBMon.20071211.c desKeyDBMon.20071211.d
else
desKeyDBMon.useDesKey4.x.20071211.o:  desKeyDBMon.20071211.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
directInputDesKeys3.x.o:  directInputDesKeys.1.0.c directInputDesKeys.1.0.d
else
directInputDesKeys3.x.o:  directInputDesKeys.1.0.c
endif
	@echo "CC $(SPEC_DEFINES4) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE4) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
directOutputDesKeys3.x.o:  directOutputDesKeys.1.0.c directOutputDesKeys.1.0.d
else
directOutputDesKeys3.x.o:  directOutputDesKeys.1.0.c
endif
	@echo "CC $(SPEC_DEFINES4) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE4) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
desKeyRecOperation3.x.o:  desKeyRecOperation.1.0.c desKeyRecOperation.1.0.d
else
desKeyRecOperation3.x.o:  desKeyRecOperation.1.0.c
endif
	@echo "CC $(SPEC_DEFINES4) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE4) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
directInputDesKeys4.x.o:  directInputDesKeys.1.0.c directInputDesKeys.1.0.d
else
directInputDesKeys4.x.o:  directInputDesKeys.1.0.c
endif
	@echo "CC $(SPEC_DEFINES5) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE5) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
directOutputDesKeys4.x.o:  directOutputDesKeys.1.0.c directOutputDesKeys.1.0.d
else
directOutputDesKeys4.x.o:  directOutputDesKeys.1.0.c
endif
	@echo "CC $(SPEC_DEFINES5) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE5) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
desKeyRecOperation4.x.o:  desKeyRecOperation.1.0.c desKeyRecOperation.1.0.d
else
desKeyRecOperation4.x.o:  desKeyRecOperation.1.0.c
endif
	@echo "CC $(SPEC_DEFINES5) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE5) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)	
desKeyDBRecord1.0.useDesKey4.x.o:  desKeyDBRecord1.0.c desKeyDBRecord1.0.d
else
desKeyDBRecord1.0.useDesKey4.x.o:  desKeyDBRecord1.0.c
endif
	@echo "CC $(SPEC_DEFINES6) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE6) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
desKeyDBSvrOperation1.0.useDesKey4.x.o:  desKeyDBSvrOperation1.0.c desKeyDBSvrOperation1.0.d
else
desKeyDBSvrOperation1.0.useDesKey4.x.o:  desKeyDBSvrOperation1.0.c
endif
	@echo "CC $(SPEC_DEFINES6) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE6) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
desKeyDBMon.useDesKey4.x.20070925.o:  desKeyDBMon.20070925.c desKeyDBMon.20070925.d
else
desKeyDBMon.useDesKey4.x.20070925.o:  desKeyDBMon.20070925.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

objs01 = genDesKey.20050725.o
$(OBJDIR)genDesKey.desKey3.x.20050725.a:	genDesKey.20050725.c
	@$(COMPILE) $< -o $(OBJDIR)$(objs01)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs01))
	$(RM) $(addprefix $(OBJDIR),$(objs01))

objs02 = genDesKey.20060614.o
$(OBJDIR)genDesKey.desKey3.x.20060614.a:	genDesKey.20060614.c
	@$(COMPILE) $< -o $(OBJDIR)$(objs02)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs02))
	$(RM) $(addprefix $(OBJDIR),$(objs02))
	
objs03 = outputDesKeys.20050607.o
$(OBJDIR)outputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a:	outputDesKeys.20050607.c
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$(objs03)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs03))
	$(RM) $(addprefix $(OBJDIR),$(objs03))

objs04 = inputDesKeys.20050607.o
$(OBJDIR)inputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a:	inputDesKeys.20050607.c
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$(objs04)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs04))
	$(RM) $(addprefix $(OBJDIR),$(objs04))

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
	
