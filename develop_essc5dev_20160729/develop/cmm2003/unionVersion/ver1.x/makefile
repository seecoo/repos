# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionVersion
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES 	= $(OSSPEC) -D _UNIX
SPEC_DEFINES	= $(OSSPEC) -D _UNIX -D _verifyLiscence_

COMPILE		= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND		= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs =	unionVersion1.0.c		\
	unionLibName3.0.c		\
	unionVersion.20041125.c		\
	unionVersion.20061204.c		\
	unionVersion.20111116.c		\
	unionVersion.20120919.c		\
	unionDataBaseType.c		\
	unionDLVersionMain.20130301.c	\
	unionDLVersion.20130301.c

objs =	unionVersion1.0.a			\
	unionLibName3.0.o			\
	unionLibName3.0.a			\
	unionVersion.20041125.a			\
	unionVersion.20061204.o			\
	unionVersion.20111116.o			\
	unionVersion.20120919.o			\
	unionDataBaseType.o			\
	unionVersion.20041125.o			\
	unionVersion.verifyLiscence.20061204.o	\
	unionVersion.verifyLiscence.20111116.o	\
	unionVersion.verifyLiscence.20120919.o	\
	genEffectiveDaysLib.20041125.o		\
	unionDLVersionMain.20130301.o		\
	unionDLVersion.20130301.o

deps =	$(patsubst %.c,%.d,$(srcs)) 

all:	makeall			\
	genEffectiveDaysLib

makeall:	$(objs)

.SUFFIXES:.c.o
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@
	
$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

.c.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o 
	@$(RM) $*.o

ifeq ($(UNION_USE_GCC),1)
unionVersion.verifyLiscence.20061204.o:	unionVersion.20061204.c unionVersion.20061204.d
else
unionVersion.verifyLiscence.20061204.o:	unionVersion.20061204.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@	$<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionVersion.verifyLiscence.20111116.o:	unionVersion.20111116.c unionVersion.20111116.d
else
unionVersion.verifyLiscence.20111116.o:	unionVersion.20111116.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@	$<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionVersion.verifyLiscence.20120919.o:	unionVersion.20120919.c unionVersion.20120919.d
else
unionVersion.verifyLiscence.20120919.o:	unionVersion.20120919.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@	$<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

libs0210 =	$(CMM2003LIBDIR)genEffectiveDaysLib.20041125.o	\
		$(CMM2003LIBDIR)unionInput1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
genEffectiveDaysLib:	
		@echo "building...	$@"
		@$(CC) $(DEFINES) -o $@ $(libs0210) $(OSSPECLIBS)
		mv $@ $(CMM2003TOOLSDIR)/

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

.PHONY: clean install show
install:
	@sh cp_head.x  "$(INCDIR)/unionVersion.h" $(CMM2003INCDIR)
#	@sh cp_head.x  ../include/ $(CMM2003INCDIR)

rmobjs = $(objs) $(deps)
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
	
