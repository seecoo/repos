# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@
# 这个编译开关必须增加
DEFINES = $(OSSPEC)
SPEC_DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)
SPEC_DEFINES1 = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ $(OSSPEC)
SPEC_DEFINES2 = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ -D _checkHsmCmdGrp_ $(OSSPEC)
SPEC_DEFINES3 = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ $(OSSPEC) -D _traceInfo_
SPEC_DEFINES4 = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ -D _useComplexDB_ $(OSSPEC)
SPEC_DEFINES5 = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ -D _nullTaskName_ $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
SPEC_COMPILE3	= $(CC) $(SPEC_DEFINES3) $(INCL) -c
SPEC_COMPILE4	= $(CC) $(SPEC_DEFINES4) $(INCL) -c
SPEC_COMPILE5	= $(CC) $(SPEC_DEFINES5) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = sjl06Grp1.0.o	\
	mngSJL06Grp1.0.o	\
	sjl06GrpMon1.0.o	\
	mngSJL06Grp.checkHsmCmdGrp.20060726.o	\
	sjl06Grp2.0.20060726.killHsmTaskAutomatically.o	\
	mngSJL06Grp.20060726.o	\
	sjl06Grp2.x.20060808.o.killHsmTaskAutomatically.o	\
	sjl06GrpMon2.x.20060808.o	\
	sjl06Grp2.x.20060808.o.killHsmTaskAutomatically.trace.o	\
	sjl06Grp2.x.20060808.useComplexDB.o	\
	sjl06GrpMon2.x.20090424.o	\
	sjl06GrpMon2.x.20090421.o	\
	sjl06Grp1.0.a	\
	mngSJL06Grp1.0.a	\
	sjl06GrpMon1.0.a	\
	sjl06Grp2.0.a	\
	mngSJL06Grp2.0.a	\
	sjl06GrpMon2.0.a	\
	sjl06Grp2.0.20041209.a	\
	mngSJL06Grp.20050713.a	\
	sjl06Grp2.0.20050816.killHsmTaskAutomatically.a
	

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

mngSJL06Grp.checkHsmCmdGrp.20060726.o:  mngSJL06Grp.20060726.c
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

sjl06Grp2.0.20060726.killHsmTaskAutomatically.o:  sjl06Grp2.0.20060726.c
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@
	
mngSJL06Grp.20060726.o:  mngSJL06Grp.20060726.c
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

sjl06Grp2.x.20060808.o.killHsmTaskAutomatically.o:  sjl06Grp2.x.20060808.c
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

sjl06GrpMon2.x.20060808.o:  sjl06GrpMon2.x.20060808.c
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

sjl06Grp2.x.20060808.o.killHsmTaskAutomatically.trace.o:  sjl06Grp2.x.20060808.c
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

sjl06Grp2.x.20060808.useComplexDB.o:  sjl06Grp2.x.20060808.c
	@echo "CC $(SPEC_DEFINES4) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE4) $< -o $(OBJDIR)$@

sjl06GrpMon2.x.20090424.o:  sjl06GrpMon2.x.20060808.c
	@echo "CC $(SPEC_DEFINES5) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE5) $< -o $(OBJDIR)$@
	
sjl06GrpMon2.x.20090421.o:  sjl06GrpMon2.x.20090421.c
	@echo "CC $(SPEC_DEFINES5) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE5) $< -o $(OBJDIR)$@
	

objs04 = sjl06Grp2.0.o
$(OBJDIR)sjl06Grp2.0.a:
	@$(SPEC_COMPILE) sjl06Grp2.0.c -o $(objs04)
	$(AR) $(OSSPECPACK) rv $@ $(objs04)
	$(RM) $(objs04)

objs05 = mngSJL06Grp1.0.o
$(OBJDIR)mngSJL06Grp2.0.a:
	@$(SPEC_COMPILE) mngSJL06Grp1.0.c -o $(objs05)
	$(AR) $(OSSPECPACK) rv $@ $(objs05)
	$(RM) $(objs05)

objs06 = sjl06GrpMon2.0.o
$(OBJDIR)sjl06GrpMon2.0.a:
	@$(SPEC_COMPILE) sjl06GrpMon2.0.c -o $(objs06)
	$(AR) $(OSSPECPACK) rv $@ $(objs06)
	$(RM) $(objs06)

objs07 = sjl06Grp2.0.20041209.o
$(OBJDIR)sjl06Grp2.0.20041209.a:
	@$(SPEC_COMPILE) sjl06Grp2.0.20041209.c -o $(objs07)
	$(AR) $(OSSPECPACK) rv $@ $(objs07)
	$(RM) $(objs07)

objs08 = mngSJL06Grp.20050713.o
$(OBJDIR)mngSJL06Grp.20050713.a:	
	@$(SPEC_COMPILE) mngSJL06Grp.20050713.c -o $(objs08)
	$(AR) $(OSSPECPACK) rv $@ $(objs08)
	$(RM) $(objs08)

objs09 = sjl06Grp2.0.20050816.o
$(OBJDIR)sjl06Grp2.0.20050816.killHsmTaskAutomatically.a:
	@$(SPEC_COMPILE1) sjl06Grp2.0.20050816.c -o $(objs09)
	$(AR) $(OSSPECPACK) rv $@ $(objs09)
	$(RM) $(objs09)

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)sjl06Grp.h
	
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
	
