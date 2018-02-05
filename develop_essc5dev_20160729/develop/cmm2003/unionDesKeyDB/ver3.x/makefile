# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@
# 这个编译开关必须增加
DEFINES = -O -DUNIX  $(OSSPEC)
SPEC_DEFINES = $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = mngDesKeyDB3.0.20041125.a	\
	mngDesKeyDBLog3.0.20041125.a	\
	unionDesKeyDB3.0.20041125.a	\
	unionDesKey3.0.20041125.a	\
	unionDesKeyDB3.x.20060414.a	\
	unionDesKeyDB3.x.NoIndex.20060421.a

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

objs03 = unionDesKeyDB3.0.20041125.o unionDesKeyDBLog3.0.20041125.o unionDesKey3.0.20041125.o
$(OBJDIR)unionDesKeyDB3.0.20041125.a:	$(objs03)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs03))
	$(RM) $(OBJDIR)unionDesKeyDBLog3.0.20041125.o

objs05 = unionDesKeyDB3.0.20041125.o unionDesKeyDBLog3.0.20060414.o unionDesKey3.0.20041125.o
$(OBJDIR)unionDesKeyDB3.x.20060414.a:	$(objs05)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs05))
	$(RM) $(addprefix $(OBJDIR),$(objs05))

objs06 = unionDesKeyDB3.x.NoIndex.20060421.o unionDesKeyDBLog3.0.20060414.o unionDesKey3.0.20041125.o
$(OBJDIR)unionDesKeyDB3.x.NoIndex.20060421.a:
	@$(SPEC_COMPILE) unionDesKeyDB3.x.NoIndex.20060421.c -o unionDesKeyDB3.x.NoIndex.20060421.o
	@$(SPEC_COMPILE) unionDesKeyDBLog3.0.20060414.c -o unionDesKeyDBLog3.0.20060414.o
	@$(SPEC_COMPILE) unionDesKey3.0.20041125.c -o unionDesKey3.0.20041125.o
	$(AR) $(OSSPECPACK) rv $@ $(objs06)
	$(RM) $(objs06)

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif
	
includes = $(INCDIR)unionDesKeyDBLog.h	\
	$(INCDIR)unionDesKeyDB.h	\
	$(INCDIR)unionDesKey.h	\
	$(INCDIR)unionKeyDB.h	
	
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
	
