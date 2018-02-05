# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionCopyrightRegistry
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
objs =	$(patsubst %.c,%.o,$(srcs)) 
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs =	unionLisence3.0.a		\
	genStaticLisenceAttr3.0.a	\
	mngLisence3.0.a			\
	genStaticLisenceAttr3.0

all:	makeall

makeall:	$(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

.c.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o 
	@$(RM) $*.o

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

objs04 = $(OBJDIR)genStaticLisenceAttr3.0.a
genStaticLisenceAttr3.0:	$(objs04)
	@echo "building...	$@"
	$(CC) $(DEFINES) -o $(CMM2003TOOLSDIR)$@ $(objs04) $(OSSPECLIBS)

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif
	
.PHONY: clean install
install:
	@sh cp_head.x  "$(INCDIR)/unionLisence.h" $(CMM2003INCDIR)
#	@sh cp_head.x  ../include/ $(CMM2003INCDIR)

clean:
	$(RM) $(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))
	$(RM) $(CMM2003TOOLSDIR)genStaticLisenceAttr3.0
	

show:
	make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
	
