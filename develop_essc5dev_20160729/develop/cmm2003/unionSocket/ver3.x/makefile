# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionSocket/
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
SPEC_DEFINES = $(OSSPEC) -D _sendDataAfterClose_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = 	UnionTCPIPSvr4.0.20061108.o	\
	UnionSocket3.0.o		\
	UnionTCPIPSvr3.0.oldForTest.o	\
	UnionTCPIPSvr3.0.forTest.o	\
	UnionSocket3.0.sendDataAfterClose.forXZBank.o	\
	UnionSocket3.0.sendDataAfterClose.o	\
	UnionSocket3.0.sendDataAfterClose.test.o	\
	UnionSocket3.0.a	\
	UnionTCPIPSvr3.0.a	\
	UnionTCPIPSvr4.0.20061108.a

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
UnionSocket3.0.sendDataAfterClose.forXZBank.o:  UnionSocket3.0.forXZBank.c UnionSocket3.0.forXZBank.d
else
UnionSocket3.0.sendDataAfterClose.forXZBank.o:  UnionSocket3.0.forXZBank.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
UnionSocket3.0.sendDataAfterClose.o:  UnionSocket3.0.c UnionSocket3.0.d
else
UnionSocket3.0.sendDataAfterClose.o:  UnionSocket3.0.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
UnionTCPIPSvr3.0.o:  UnionTCPIPSvr3.0.c UnionTCPIPSvr3.0.d
else
UnionTCPIPSvr3.0.o:  UnionTCPIPSvr3.0.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
UnionSocket3.0.test.o:  UnionSocket3.0.test.c UnionSocket3.0.test.d
else
UnionSocket3.0.test.o:  UnionSocket3.0.test.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
UnionSocket3.0.sendDataAfterClose.test.o:  UnionSocket3.0.test.c UnionSocket3.0.test.d
else
UnionSocket3.0.sendDataAfterClose.test.o:  UnionSocket3.0.test.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

objs02 = UnionTCPIPSvr3.0.o UnionSocket3.0.o
$(OBJDIR)UnionTCPIPSvr3.0.a:	$(objs02)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs02))

objs03 = UnionTCPIPSvr4.0.20061108.o UnionSocket3.0.o
$(OBJDIR)UnionTCPIPSvr4.0.20061108.a:	$(objs03)
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs03))

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif
	
.PHONY: clean install
install:
	@sh cp_head.x  "$(INCDIR)UnionSocket.h" $(CMM2003INCDIR)
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
	
