# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES = -D _UNIX $(OSSPEC)
SPEC_DEFINES = -D _UNIX $(OSSPEC) -D _useSigUser2_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs =	unionProc1.0.a			\
	unionProc1.0.o			\
	unionProc1.0.useSigUser2.o	\
	unionProc.20061116.o		\
	unionTask.20090421.useSig2.o	\
	unionTask.defFile.20090421.o	\
	unionTask.db.20090421.o		\
	unionTask.db.20130301.o		\
	unionTask.20060726.useSig2.o	\
	unionTask.20061101.o		\
	unionTask.20050919.o		\
	unionTask.20050909.a		\
	unionTask.20050919.a		\
	unionTask.20050919.noIgnoreSigCLD.a	\
	unionTask.20051009.a		\
	unionTask.20061101.a		\
	unionTask.20051103.useSig2.a	\
	unionTask3.0.o			\
	unionTask3.0.a			\
	unionTask3.1.a			\
	unionTask3.2.a			\
	unionTask3.2.20041125.a		\
	unionUnionTaskTBL.20090421.o	\
	mngTask.20050706.a		\
	mngTask.20050909.a		\
	mngTask.20061101.o		\
	mngTask.20061101.a		\
	taskMon.20050919.a		\
	taskMon3.0.a
	
all:	makeall		

makeall:	$(objs)	\

.SUFFIXES:.c.o
.c.o:
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
unionTask.20090421.useSig2.o:  unionTask.20090421.c unionTask.20090421.d
else
unionTask.20090421.useSig2.o:  unionTask.20090421.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionTask.defFile.20090421.o:  unionTask.defFile.20090421.c unionTask.defFile.20090421.d
else
unionTask.defFile.20090421.o:  unionTask.defFile.20090421.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionProc1.0.useSigUser2.o:  unionProc1.0.c unionProc1.0.d
else
unionProc1.0.useSigUser2.o:  unionProc1.0.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionTask.20051103.o:  unionTask.20051103.c unionTask.20051103.d
else
unionTask.20051103.o:  unionTask.20051103.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionTask.20060726.useSig2.o:  unionTask.20060726.c unionTask.20060726.d
else
unionTask.20060726.useSig2.o:  unionTask.20060726.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

objs01 = unionTask3.2.20041125.o unionProc1.0.o
$(OBJDIR)unionTask3.2.20041125.a:	$(objs01)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs01))

objs03 = unionTask.20050909.o unionProc1.0.o
$(OBJDIR)unionTask.20050909.a:	$(objs03)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs03))

objs05 = unionTask.20050919.o unionProc1.0.o
$(OBJDIR)unionTask.20050919.a:	$(objs05)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs05))

objs07 = unionTask.20051009.o unionProc1.0.o
$(OBJDIR)unionTask.20051009.a:	$(objs07)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs07))

objs08 = unionTask.20061101.o unionProc1.0.o
$(OBJDIR)unionTask.20061101.a:	$(objs08)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs08))

objs09 = unionTask.20050919.o unionProc.20061116.o
$(OBJDIR)unionTask.20050919.noIgnoreSigCLD.a:	$(objs09)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs09))

objs10 = unionTask.20051103.o unionProc1.0.useSigUser2.o
$(OBJDIR)unionTask.20051103.useSig2.a:	$(objs10)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs10))

objs12 = unionTask3.0.o unionProc1.0.o
$(OBJDIR)unionTask3.0.a:	$(objs12)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs12))

objs15 = unionTask3.1.o unionProc1.0.o
$(OBJDIR)unionTask3.1.a:	$(objs15)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs15))

objs16 = unionTask3.2.o unionProc1.0.o
$(OBJDIR)unionTask3.2.a:	$(objs16)
	@$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs16))

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes =	$(INCDIR)UnionTask.h		\
		$(INCDIR)UnionProc.h		\
		$(INCDIR)unionUnionTaskTBL.h

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
	
