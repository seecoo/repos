# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionKDBSvr/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = -D _UnionDesKey_4_x_ -D _UnionPKDB_4_x_  $(OSSPEC)
SPEC_DEFINES = -D _UnionDesKey_4_x_ -D _UnionPKDB_4_x_  $(OSSPEC) -D _useComplexDB_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = kdbSvrService.20070803.o	\
	keyDBSvrMain.2.x.20070803.o	\
	keyDBSvrMon.2.x.20070803.o	\
	mngKDBSvr.2.x.20070803.o	\
	kdbSvrService.useDesKey4.xPK4.x.20070803.o	\
	kdbSvrService.useDesKey4.xPK4.x.20090323.o
#	kdbSvrService.useComplexDB.20090323.o	\

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
kdbSvrService.useDesKey4.xPK4.x.20070803.o:  kdbSvrService.20070803.c kdbSvrService.20070803.d
else
kdbSvrService.useDesKey4.xPK4.x.20070803.o:  kdbSvrService.20070803.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
kdbSvrService.useComplexDB.20090323.o:  kdbSvrService.20090323.useComplexDB.c kdbSvrService.20090323.useComplexDB.d
else
kdbSvrService.useComplexDB.20090323.o:  kdbSvrService.20090323.useComplexDB.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
kdbSvrService.useDesKey4.xPK4.x.20090323.o:  kdbSvrService.20090323.c kdbSvrService.20090323.d
else
kdbSvrService.useDesKey4.xPK4.x.20090323.o:  kdbSvrService.20090323.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)keyDBSvr.h	\
	$(INCDIR)kdbSvrService.h
	
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
	
