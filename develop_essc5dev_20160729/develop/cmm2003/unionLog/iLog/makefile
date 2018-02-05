# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionLog/
INCDIR = $(HOMEDIR)/include/
OBJDIR = ./
LIBDIR = $(CMM2003LIBDIR)

OSNAME = $(shell uname)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) 

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 当操作系统是是高字节在前时，打开该编译开关。
DEFINES = $(OSSPEC)

ifeq ($(OSNAME),AIX)
COMPILE	= $(CC) $(DEFINES) -qtls $(INCL) -c
else
COMPILE	= $(CC) $(DEFINES) $(INCL) -c
endif
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 
objs =	$(patsubst %.c,%.o,$(srcs))
libs =	libilog.a

all:	makeall

makeall:	libilog.a

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

libilog.a:	$(objs)
	$(AR) $(OSSPECPACK) rv $(LIBDIR)$@ $(addprefix $(OBJDIR),$(objs))
	$(RM) $(addprefix $(OBJDIR),$(objs))

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = 	LOG.h	\
		LOGS.h

.PHONY: clean install
install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)

clean:
	$(RM) $(addprefix $(LIBDIR),$(libs)) $(addprefix $(OBJDIR),$(objs),$(addprefix $(LIBDIR),$(deps)))

show:
	make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
