# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionLog
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES =$(OSSPEC)
SPEC_DEFINES =$(OSSPEC) -D _useSpier_
SPEC_DEFINES1 =$(OSSPEC) -std=c99

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs))

objs =	UnionLog.20060811.o		\
	UnionLog.20121101.o		\
	UnionLog.20130301.o		\
	UnionLog.20140912.o		\
	UnionLog.20060811.a		\
	UnionLog.20060811.useSpier.o	\
	UnionLog.20060811.useSpier.a	\
	unionLogTBL1.0.o		\
	unionLogTBL1.0.20041209.o	\
	UnionESBLog.20160322.o		\
	unionLogTBL.20050909.o		\
	unionLogTBL.20060811.o		\
	unionLogTBL.20060811.useSpier.o	\
	unionLogTBL.20090421.o		\
	unionLogTBL.20130301.o		\
	unionLogTBL.20140912.o		\
	unionLogTBL.defFile.20090421.o	\
	unionLogTBL.withoutLogTBL.o	\
	unionSystemTime1.0.o		\
	unionSystemTime1.0.a		\
	unionSystemTime1.0.useSpier.o	\
	unionLogTBL.db.20090421.o	\
	unionLogTBL.db.20130301.o	\
	unionLogTBL.db.20140912.o	\
	logMon1.0.o			\
	logMon1.0.a			\
	mngUnionLog1.0.o		\
	mngUnionLog1.0.a		\
	mngUnionLog2.0.o		\
	mngUnionLog2.0.a		\
	unionUnionLogTBL.20090421.o
			

all:	makeall		

makeall:	$(objs)

.SUFFIXES:.c.o
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	$(COMPILE) $< -o $(OBJDIR)$@

.c.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $(OBJDIR)$*.o

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
UnionLog.20060811.useSpier.o:  UnionLog.20060811.c UnionLog.20060811.d
else
UnionLog.20060811.useSpier.o:  UnionLog.20060811.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@
	
ifeq ($(UNION_USE_GCC),1)
unionSystemTime1.0.useSpier.o:  unionSystemTime1.0.c unionSystemTime1.0.d
else
unionSystemTime1.0.useSpier.o:  unionSystemTime1.0.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionLogTBL.20060811.useSpier.o:  unionLogTBL.20060811.c unionLogTBL.20060811.d
else
unionLogTBL.20060811.useSpier.o:  unionLogTBL.20060811.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

objs01 = UnionLog3.0.o unionSystemTime1.0.o unionLogTBL1.0.20041209.o
$(OBJDIR)UnionLog3.0.20041209.a:	$(objs01)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs01))

objs02 = UnionLog.20050909.o unionSystemTime1.0.o unionLogTBL.20050909.o
$(OBJDIR)UnionLog.20050909.a:	$(objs02)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs02))

objs03 = UnionLog.20060811.o unionSystemTime1.0.o unionLogTBL.20060811.o
$(OBJDIR)UnionLog.20060811.a:	$(objs03)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs03))

objs04 = UnionLog.20060811.useSpier.o unionSystemTime1.0.useSpier.o unionLogTBL.20060811.useSpier.o
$(OBJDIR)UnionLog.20060811.useSpier.a:	$(objs04)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs04))

objs05 = UnionLog3.0.o unionSystemTime1.0.o unionLogTBL1.0.o
$(OBJDIR)UnionLog3.0.a:	$(objs05)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs05))
	
ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = 	$(INCDIR)UnionLog.h		\
		$(INCDIR)unionUnionLogTBL.h

.PHONY: clean install
install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)

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
	
