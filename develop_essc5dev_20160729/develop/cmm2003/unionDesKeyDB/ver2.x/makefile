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
SPEC_DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
objs =	$(patsubst %.c,%.o,$(srcs)) 
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs += batchEditDesKeys.20050616.forGDB.o	\
	batchEditDesKeys.20050616.o	\
	unionDesKey.20040714.a	\
	unionDesKeyDBLog.20040714.a	\
	unionDesKeyDBLog.20041120.a	\
	unionDesKeyDB.20041120.a	\
	batchEditDesKeys.desKey2.0.20050616.forGDB.a	\
	batchEditDesKeys.desKey2.0.20050616.a	\
	unionDesKey2.0.a	\
	unionDesKeyDB2.0.a	\
	mngDesKeyDB2.0.a	\
	mngDesKeyDBLog1.0.a

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

ifeq ($(UNION_USE_GCC),1)
batchEditDesKeys.20050616.forGDB.o:  batchEditDesKeys.20050616.c batchEditDesKeys.20050616.d
else
batchEditDesKeys.20050616.forGDB.o:  batchEditDesKeys.20050616.c
endif
	@echo "CC $(SPEC_DEFINES) -D _GangdongDevelopmentBank_ $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) -D _GangdongDevelopmentBank_ $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
batchEditDesKeys.20050616.o:  batchEditDesKeys.20050616.c batchEditDesKeys.20050616.d
else
batchEditDesKeys.20050616.o:  batchEditDesKeys.20050616.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@


objs02 = unionDesKeyDBLog1.0.o
$(OBJDIR)unionDesKeyDBLog.20040714.a:	$(objs02)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs02))

objs04 = unionDesKeyDB.20041120.o unionDesKeyDBLog.20041120.o unionDesKey.20040714.o
$(OBJDIR)unionDesKeyDB.20041120.a:	$(objs04)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs04))
	$(RM) $(addprefix $(OBJDIR),$(objs04))

objs05 = batchEditDesKeys.20050616.forGDB.o
$(OBJDIR)batchEditDesKeys.desKey2.0.20050616.forGDB.a:	$(objs05)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs05))
	$(RM) $(addprefix $(OBJDIR),$(objs05))

objs06 = batchEditDesKeys.20050616.o
$(OBJDIR)batchEditDesKeys.desKey2.0.20050616.a:	$(objs06)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs06))
	$(RM) $(addprefix $(OBJDIR),$(objs06))

objs07 = unionDesKey2.0.o unionDesKeyDBLog1.0.o
$(OBJDIR)unionDesKey2.0.a:	$(objs07)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs07))
	$(RM) $(addprefix $(OBJDIR),$(objs07))

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)unionDesKeyDBLog.h	\
	$(INCDIR)unionDesKey.h	\
	$(INCDIR)unionDesKeyDB.h	\
	$(INCDIR)unionDesKeyDBLog.h	\
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
	
