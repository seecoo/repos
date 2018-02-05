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
SPEC_DEFINES1 = -D _UnionSJL06_2_x_Above_ -D _Use_SJL06Server_ $(OSSPEC)
SPEC_DEFINES2 = -D _UnionSJL06_2_x_Above_  -D _UnionDesKey_3_x_ $(OSSPEC)
SPEC_DEFINES3 = -D _UnionSJL06_2_x_Above_ -D _Use_SJL06Server_ -D _UnionDesKey_3_x_ $(OSSPEC)
SPEC_DEFINES4 = -D _UnionSJL06_2_x_Above_ -D _Suport3Des_ $(OSSPEC)
SPEC_DEFINES5 = -D _UnionSJL06_2_x_Above_ -D _Use_SJL06Server_ -D _Suport3Des_ $(OSSPEC)

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

objs = sjl06CmdForJK.IC.RSA.1.0.o	\
	sjl06CmdForNewRacal.1.0.o	\
	3DesRacalCmd1.0.o	\
	3DesRacalSyntaxRules1.0.o	\
	racalRsaCmd.20070828.o	\
	racalSM2Cmd.20111117.o	\
	sjl06CmdForJK.IC.RSA.1.0.a	\
	sjl06CmdForNewRacal.1.0.a	\
	sjl06CmdForJK.IC.RSA.2.0.a	\
	sjl06CmdForNewRacal.2.0.a	\
	sjl06CmdForJK.IC.RSA.20040729.a	\
	sjl06CmdForNewRacal.20040729.a	\
	sjl06CmdForJK.IC.RSA.20041129.useDesKey3.xNonSJL06Server.a	\
	sjl06CmdForNewRacal.20041129.useDesKey3.xNonSJL06Server.a	\
	sjl06CmdForJK.IC.RSA.20041129.useDesKey3.xSJL06Server.a	\
	sjl06CmdForNewRacal.20041129.useDesKey3.xSJL06Server.a	\
	sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xnonSJL06Server.a	\
	sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xSJL06Server.a
	

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

ifeq ($(UNION_USE_GCC),1)
racalRsaCmd.20070828.o:  racalRsaCmd.20070828.c racalRsaCmd.20070828.d
else
racalRsaCmd.20070828.o:  racalRsaCmd.20070828.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
racalSM2Cmd.20111117.o:  racalSM2Cmd.20111117.c racalSM2Cmd.20111117.d
else
racalSM2Cmd.20111117.o:  racalSM2Cmd.20111117.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

objs01 = sjl06CmdForJK.IC.RSA.1.0.o
$(OBJDIR)sjl06CmdForJK.IC.RSA.1.0.a:	$(objs01)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs01))

objs02 = sjl06CmdForNewRacal.1.0.o 3DesRacalCmd1.0.o 3DesRacalSyntaxRules1.0.o
$(OBJDIR)sjl06CmdForNewRacal.1.0.a:	$(objs02)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs02))

objs03 = sjl06CmdForJK.IC.RSA.1.0.o
$(OBJDIR)sjl06CmdForJK.IC.RSA.2.0.a:
	@$(SPEC_COMPILE) sjl06CmdForJK.IC.RSA.1.0.c -o $(objs03)
	$(AR) $(OSSPECPACK) rv $@ $(objs03)
	$(RM) $(objs03)

objs04 = sjl06CmdForNewRacal.1.0.o 3DesRacalCmd1.0.o 3DesRacalSyntaxRules1.0.o
$(OBJDIR)sjl06CmdForNewRacal.2.0.a:
	@$(SPEC_COMPILE) sjl06CmdForNewRacal.1.0.c -o sjl06CmdForNewRacal.1.0.o
	@$(SPEC_COMPILE) 3DesRacalCmd1.0.c -o 3DesRacalCmd1.0.o
	@$(SPEC_COMPILE) 3DesRacalSyntaxRules1.0.c -o 3DesRacalSyntaxRules1.0.o
	$(AR) $(OSSPECPACK) rv $@ $(objs04)	
	$(RM) $(objs04)

objs05 = sjl06CmdForJK.IC.RSA.20040729.o
$(OBJDIR)sjl06CmdForJK.IC.RSA.20040729.a:
	@$(SPEC_COMPILE1) sjl06CmdForJK.IC.RSA.20040729.c -o $(objs05)
	$(AR) $(OSSPECPACK) rv $@ $(objs05)
	$(RM) $(objs05)

objs06 = sjl06CmdForNewRacal.20040729.o 3DesRacalCmd1.0.o 3DesRacalSyntaxRules1.0.o
$(OBJDIR)sjl06CmdForNewRacal.20040729.a:
	@$(SPEC_COMPILE1) sjl06CmdForNewRacal.20040729.c -o sjl06CmdForNewRacal.20040729.o
	@$(SPEC_COMPILE1) 3DesRacalCmd1.0.c -o 3DesRacalCmd1.0.o
	@$(SPEC_COMPILE1) 3DesRacalSyntaxRules1.0.c -o 3DesRacalSyntaxRules1.0.o
	$(AR) $(OSSPECPACK) rv $@ $(objs06)
	$(RM) $(objs06)

objs07 = sjl06CmdForJK.IC.RSA.1.0.o
$(OBJDIR)sjl06CmdForJK.IC.RSA.20041129.useDesKey3.xNonSJL06Server.a:
	@$(SPEC_COMPILE2) sjl06CmdForJK.IC.RSA.1.0.c -o $(objs07)
	$(AR) $(OSSPECPACK) rv $@ $(objs07)
	$(RM) $(objs07)

objs08 = sjl06CmdForNewRacal.1.0.o 3DesRacalCmd1.0.o 3DesRacalSyntaxRules1.0.o
$(OBJDIR)sjl06CmdForNewRacal.20041129.useDesKey3.xNonSJL06Server.a:
	@$(SPEC_COMPILE2) sjl06CmdForNewRacal.1.0.c -o sjl06CmdForNewRacal.1.0.o
	@$(SPEC_COMPILE2) 3DesRacalCmd1.0.c -o 3DesRacalCmd1.0.o
	@$(SPEC_COMPILE2) 3DesRacalSyntaxRules1.0.c -o 3DesRacalSyntaxRules1.0.o
	$(AR) $(OSSPECPACK) rv $@ $(objs08)
	$(RM) $(objs08)

objs09 = sjl06CmdForJK.IC.RSA.20040729.o
$(OBJDIR)sjl06CmdForJK.IC.RSA.20041129.useDesKey3.xSJL06Server.a:
	@$(SPEC_COMPILE3) sjl06CmdForJK.IC.RSA.20040729.c -o $(objs09)
	$(AR) $(OSSPECPACK) rv $@ $(objs09)
	$(RM) $(objs09)

objs10 = sjl06CmdForNewRacal.20040729.o 3DesRacalCmd1.0.o 3DesRacalSyntaxRules1.0.o
$(OBJDIR)sjl06CmdForNewRacal.20041129.useDesKey3.xSJL06Server.a:
	@$(SPEC_COMPILE3) sjl06CmdForNewRacal.20040729.c -o sjl06CmdForNewRacal.20040729.o
	@$(SPEC_COMPILE3) 3DesRacalCmd1.0.c -o 3DesRacalCmd1.0.o
	@$(SPEC_COMPILE3) 3DesRacalSyntaxRules1.0.c -o 3DesRacalSyntaxRules1.0.o
	$(AR) $(OSSPECPACK) rv $@ $(objs10)
	$(RM) $(objs10)

objs11 = sjl06CmdForJK.IC.RSA.for3Des.20050712.nonSJL06Server.o
$(OBJDIR)sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xnonSJL06Server.a:
	@$(SPEC_COMPILE4) sjl06CmdForJK.IC.RSA.for3Des.20050712.nonSJL06Server.c -o $(objs11)
	$(AR) $(OSSPECPACK) rv $@ $(objs11)
	$(RM) $(objs11)

objs12 = sjl06CmdForJK.IC.RSA.for3Des.20050712.o
$(OBJDIR)sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xSJL06Server.a:
	@$(SPEC_COMPILE5) sjl06CmdForJK.IC.RSA.for3Des.20050712.c -o $(objs12)
	$(AR) $(OSSPECPACK) rv $@ $(objs12)
	$(RM) $(objs12)


ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)sjl06Cmd.h	\
	$(INCDIR)3DesRacalCmd.h	\
	$(INCDIR)3DesRacalSyntaxRules.h	\
	$(INCDIR)racalRsaCmd.h	\
	$(INCDIR)racalSM2Cmd.h	\
	$(INCDIR)sjl06Grp.h
	
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
	
