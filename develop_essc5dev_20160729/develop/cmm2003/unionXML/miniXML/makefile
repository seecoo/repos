# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionXML
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES 	= $(OSSPEC)

COMPILE		= $(CC) $(DEFINES) $(INCL) -c
DEPEND		= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs =	mxml-attr.c	\
	mxml-entity.c	\
	mxml-file.c	\
	mxml-get.c	\
	mxml-node.c	\
	mxml-private.c	\
	mxml-search.c	\
	mxml-set.c	\
	mxml-string.c

objs =	$(patsubst %.c,%.o,$(srcs)) 

libs = libmxml.a

deps =	$(patsubst %.c,%.d,$(srcs)) 

all:	makeall		

makeall:	$(objs)		\
		$(libs)

.SUFFIXES:.c.o

.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@
	
$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
libmxml.a:	$(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))
else
#libmxml.a:	$(addprefix $(OBJDIR),$(objs))
libmxml.a:
endif
	@$(AR) $(OSSPECPACK) r $(OBJDIR)$@ $(addprefix $(OBJDIR),$(objs))

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

.PHONY: clean install show
install:
	@sh cp_head.x  "$(INCDIR)/mxml.h" $(CMM2003INCDIR)

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
	
