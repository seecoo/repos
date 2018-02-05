# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionEnv
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(CMM2003DIR)/unionComplexDB/include/

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)
SPEC_DEFINES = -D _UNIX  $(OSSPEC) -D _MaxVarNum_4096_
SPEC_DEFINES1 = -D _UNIX  $(OSSPEC) -D _MaxVarNum_4096_ -D _UnionEnv_3_x_ -D _LargeEnvVar_
SPEC_DEFINES2 = -D _UNIX  $(OSSPEC) -D _MaxVarNum_4096_ -D _LargeEnvVar_
SPEC_DEFINES3 = -D _UNIX  $(OSSPEC) -D _LargeEnvVar_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
SPEC_COMPILE3	= $(CC) $(SPEC_DEFINES3) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = mngEnv.20080320.o	\
	unionEnv.20050804.4096.o	\
	unioncomplexDBAPIOnUnionEnv.1.0.o	\
	unioncomplexDBAPIOnUnionEnv.2.0.o	\
	unionEnv.20050804.4096.largeEnvVar.o	\
	unionEnv3.0.4096.o	\
	unionEnv.20050804.a	\
	unionEnv3.0.largeEnvVar.a

all:	makeall		

makeall:	$(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

.c.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o
	@$(RM) $*.o

ifeq ($(UNION_USE_GCC),1)
unionEnv.20050804.4096.o:  unionEnv.20050804.c unionEnv.20050804.d
else
unionEnv.20050804.4096.o:  unionEnv.20050804.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unioncomplexDBAPIOnUnionEnv.1.0.o:  unioncomplexDBAPIOnUnionEnv.1.0.c unioncomplexDBAPIOnUnionEnv.1.0.d
else
unioncomplexDBAPIOnUnionEnv.1.0.o:  unioncomplexDBAPIOnUnionEnv.1.0.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unioncomplexDBAPIOnUnionEnv.2.0.o:  unioncomplexDBAPIOnUnionEnv.2.0.c unioncomplexDBAPIOnUnionEnv.2.0.d
else
unioncomplexDBAPIOnUnionEnv.2.0.o:  unioncomplexDBAPIOnUnionEnv.2.0.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionEnv.20050804.4096.largeEnvVar.o:  unionEnv.20050804.c unionEnv.20050804.d
else
unionEnv.20050804.4096.largeEnvVar.o:  unionEnv.20050804.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionEnv3.0.4096.o:  unionEnv3.0.c unionEnv3.0.d
else
unionEnv3.0.4096.o:  unionEnv3.0.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionEnv3.0.LargeEnvVar.o:  unionEnv3.0.c unionEnv3.0.d
else
unionEnv3.0.LargeEnvVar.o:  unionEnv3.0.c
endif
	@echo "CC $(DEFINES) -D _LargeEnvVar_ $(OBJDIR)$@      $<"
	@$(COMPILE) -D _LargeEnvVar_ $< -o $(OBJDIR)$@

objs01 = unionEnv3.0.o
$(OBJDIR)unionEnv3.0.largeEnvVar.a:
	@$(SPEC_COMPILE3) unionEnv3.0.c -o $(addprefix $(OBJDIR),$(objs01))
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs01))

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

.PHONY: clean install
install:
	@sh cp_head.x  "$(INCDIR)/UnionEnv.h" $(CMM2003INCDIR)
#	@sh cp_head.x  ../include/ $(CMM2003INCDIR)

clean:
	$(RM) $(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))
	$(RM) $(OBJDIR)unionEnv3.0.o

show:
	make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
	
