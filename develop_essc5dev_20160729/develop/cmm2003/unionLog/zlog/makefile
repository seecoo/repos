# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionLog/
INCDIR = $(HOMEDIR)/include/
OBJDIR = ./
LIBDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) 

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 当操作系统是是高字节在前时，打开该编译开关。
#DEFINES = $(OSSPEC) -std=c99 -pedantic
DEFINES = $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 
objs =	$(patsubst %.c,%.o,$(srcs))
#objs -= zlog-chk-conf.o
libs =	libzlog.a

all:	makeall

makeall:	libzlog.a

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

libzlog.a:	$(objs)
	$(AR) $(OSSPECPACK) rv $(LIBDIR)$@ $(addprefix $(OBJDIR),$(objs))
	$(RM) $(addprefix $(OBJDIR),$(objs))

objs01 = $(OBJDIR)libzlog.a
zlog-chk-conf:	$(0bjs01)
		@echo "building...      $@"
		@$(CC) $(DEFINES) -o $(CMM2003TOOLSDIR)$@ $(0bjs01) $(OSSPECLIBS)
	
ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = zlog.h

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
