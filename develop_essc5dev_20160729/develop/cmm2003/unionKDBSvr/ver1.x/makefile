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
DEFINES = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = keyDBSvr.useDesKey3.xPK3.x.20070609.o	\
	keyDBSvrMain.useDesKey3.xPK3.x.20070609.o	\
	keyDBMaintainer.useDesKey3.xPK3.x.20070609.o

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
keyDBSvr.useDesKey3.xPK3.x.20070609.o:  keyDBSvr.20070609.c keyDBSvr.20070609.d
else
keyDBSvr.useDesKey3.xPK3.x.20070609.o:  keyDBSvr.20070609.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
keyDBSvrMain.useDesKey3.xPK3.x.20070609.o:  keyDBSvrMain.20070609.c keyDBSvrMain.20070609.d
else
keyDBSvrMain.useDesKey3.xPK3.x.20070609.o:  keyDBSvrMain.20070609.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
keyDBMaintainer.useDesKey3.xPK3.x.20070609.o:  keyDBMaintainer.20070609.c keyDBMaintainer.20070609.d
else
keyDBMaintainer.useDesKey3.xPK3.x.20070609.o:  keyDBMaintainer.20070609.c
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
	
