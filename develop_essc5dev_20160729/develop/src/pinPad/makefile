# 定义开发目录
HOMEDIR = $(DEVDIR)
INCDIR = $(HOMEDIR)include/
OBJDIR = $(PROLIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(PROINCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@

# 这个编译开关必须增加
DEFINES = $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
objs =	$(patsubst %.c,%.o,$(srcs)) 
deps =	$(patsubst %.o,%.d,$(objs)) 

all:	makeall		

makeall:	$(objs)

.SUFFIXES:.c.o
.c.o:
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

includes =	$(INCDIR)pinPadDB.h

.PHONY: clean install show
install:
	@sh cp_head.x  "$(includes)" $(PROINCDIR)

rmobjs = $(objs) $(deps) $(libs)
clean:
	$(RM) $(addprefix $(OBJDIR),$(rmobjs))

show:
	make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
	
