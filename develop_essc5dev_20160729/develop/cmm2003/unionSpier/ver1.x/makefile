# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionSpier
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
SPEC_DEFINES = $(OSSPEC) -D _transSpier_2_x_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = transSpierMain.20060808.2.x_3.x.o	\
	transSpierMon.20060808.1.x.o	\
	transSpierBuf.20060808.o	\
	mngTransSpierBuf.20060808.o	\
	transSpierBuf.defFile.20060808.o	\
	transSpierMainToLogFile.20060808.o	\
	transSpierMain.20060808.o	\
	transSpier.20060808.o	\
	transSpier.defaultHsmIPAddr.20060808.o	\
	transSpierMon.20060808.o	\
	commWithTransSpier.20060808.o	\
	transSpierBuf.20060808.2.x.o	\
	transSpierMainToLogFile.20060808.2.x.o	\
	transSpierMain.20060808.2.x_4.x.o	\
	transSpier.20060808.2.x.o	\
	transSpierMon.20060808.2.x.o	\
	transSpierMain.20060808.2.x.o

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
commWithTransSpier.20060808.o:  commWithTransSpier.20060808.c commWithTransSpier.20060808.d
else
commWithTransSpier.20060808.o:  commWithTransSpier.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpierBuf.defFile.20060808.o:  transSpierBuf.defFile.20060808.c transSpierBuf.defFile.20060808.d
else
transSpierBuf.defFile.20060808.o:  transSpierBuf.defFile.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
mngTransSpierBuf.20060808.o:  mngTransSpierBuf.20060808.c mngTransSpierBuf.20060808.d
else
mngTransSpierBuf.20060808.o:  mngTransSpierBuf.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpier.defaultHsmIPAddr.20060808.o:  transSpier.defaultHsmIPAddr.20060808.c transSpier.defaultHsmIPAddr.20060808.d
else
transSpier.defaultHsmIPAddr.20060808.o:  transSpier.defaultHsmIPAddr.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpierMain.20060808.2.x_3.x.o:  transSpierMain.20060808.c transSpierMain.20060808.d
else
transSpierMain.20060808.2.x_3.x.o:  transSpierMain.20060808.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpierMon.20060808.1.x.o:  transSpierMon.20060808.c transSpierMon.20060808.d
else
transSpierMon.20060808.1.x.o:  transSpierMon.20060808.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpierBuf.20060808.2.x.o:  transSpierBuf.20060808.c transSpierBuf.20060808.d
else
transSpierBuf.20060808.2.x.o:  transSpierBuf.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpierMainToLogFile.20060808.2.x.o:  transSpierMainToLogFile.20060808.c transSpierMainToLogFile.20060808.d
else
transSpierMainToLogFile.20060808.2.x.o:  transSpierMainToLogFile.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpierMain.20060808.2.x_4.x.o:  transSpierMain.20060808.c transSpierMain.20060808.d
else
transSpierMain.20060808.2.x_4.x.o:  transSpierMain.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
transSpier.20060808.2.x.o:  transSpier.20060808.c transSpier.20060808.d
else
transSpier.20060808.2.x.o:  transSpier.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@	

ifeq ($(UNION_USE_GCC),1)
transSpierMon.20060808.2.x.o:  transSpierMon.20060808.c transSpierMon.20060808.d
else
transSpierMon.20060808.2.x.o:  transSpierMon.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@	

ifeq ($(UNION_USE_GCC),1)
transSpierMain.20060808.2.x.o:  transSpierMain.20060808.c transSpierMain.20060808.d
else
transSpierMain.20060808.2.x.o:  transSpierMain.20060808.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@
	
ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)transSpierBuf.h	\
	$(INCDIR)commWithTransSpier.h
	
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
	
