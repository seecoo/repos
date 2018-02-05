# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionMsgBuf
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@
# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)
SPEC_DEFINES = -D _selfMsgIndex_ -D _IgnoreProvider_  $(OSSPEC)
SPEC_DEFINES1 = -D _selfMsgIndex_  $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = unionMsgBuf5.0.o	\
	mngMsgBuf5.0.o	\
	msgBufMon5.0.o	\
	unionMsgBuf5.1.o	\
	unionMsgBuf5.2.o	\
	unionMsgBuf5.3.o	\
	unionMsgBuf5.0.useSelfMsgIndex.IgnoreProvider.o	\
	unionMsgBuf5.0.useSelfMsgIndex.o	\
	unionMsgBuf5.1.useSelfMsgIndex.IgnoreProvider.o	\
	unionMsgBuf5.1.useSelfMsgIndex.o	\
	unionMsgBuf5.2.useSelfMsgIndex.IgnoreProvider.o	\
	unionMsgBuf5.2.useSelfMsgIndex.o	\
	unionMsgBuf5.3.useSelfMsgIndex.IgnoreProvider.o	\
	unionMsgBuf5.3.useSelfMsgIndex.o	\
	unionMsgBuf5.0.a	\
	mngMsgBuf5.0.a	\
	msgBufMon5.0.a	\
	unionMsgBuf5.0.useSelfMsgIndex.IgnoreProvider.a	\
	unionMsgBuf5.0.useSelfMsgIndex.a	\
	unionMsgBuf5.1.a	\
	unionMsgBuf5.1.useSelfMsgIndex.IgnoreProvider.a	\
	unionMsgBuf5.1.useSelfMsgIndex.a	\
	unionMsgBuf5.2.a	\
	unionMsgBuf5.2.useSelfMsgIndex.IgnoreProvider.a	\
	unionMsgBuf5.2.useSelfMsgIndex.a	\
	unionMsgBuf5.3.a	\
	unionMsgBuf5.3.useSelfMsgIndex.IgnoreProvider.a	\
	unionMsgBuf5.3.useSelfMsgIndex.a

all:	makeall

makeall:	$(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $@	$<"
	@$(COMPILE) $< -o $@

.o.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@	$<"
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o 
	@$(RM) $*.o

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.0.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.0.c unionMsgBuf5.0.d
else
unionMsgBuf5.0.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.0.c
endif
	@echo "CC $(SPEC_DEFINES) $@      $<"
	@$(SPEC_COMPILE) $< -o $@

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.0.useSelfMsgIndex.o:  unionMsgBuf5.0.c unionMsgBuf5.0.d
else
unionMsgBuf5.0.useSelfMsgIndex.o:  unionMsgBuf5.0.c
endif
	@echo "CC $(SPEC_DEFINES1) $@      $<"
	@$(SPEC_COMPILE1) $< -o $@

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.1.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.1.c unionMsgBuf5.1.d
else
unionMsgBuf5.1.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.1.c
endif
	@echo "CC $(SPEC_DEFINES) $@      $<"
	@$(SPEC_COMPILE) $< -o $@

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.1.useSelfMsgIndex.o:  unionMsgBuf5.1.c unionMsgBuf5.1.d
else
unionMsgBuf5.1.useSelfMsgIndex.o:  unionMsgBuf5.1.c
endif
	@echo "CC $(SPEC_DEFINES1) $@      $<"
	@$(SPEC_COMPILE1) $< -o $@

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.2.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.2.c unionMsgBuf5.2.d
else
unionMsgBuf5.2.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.2.c
endif
	@echo "CC $(SPEC_DEFINES) $@      $<"
	@$(SPEC_COMPILE) $< -o $@

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.2.useSelfMsgIndex.o:  unionMsgBuf5.2.c unionMsgBuf5.2.d
else
unionMsgBuf5.2.useSelfMsgIndex.o:  unionMsgBuf5.2.c
endif
	@echo "CC $(SPEC_DEFINES1) $@      $<"
	@$(SPEC_COMPILE1) $< -o $@

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.3.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.3.c unionMsgBuf5.3.d
else
unionMsgBuf5.3.useSelfMsgIndex.IgnoreProvider.o:  unionMsgBuf5.3.c
endif
	@echo "CC $(SPEC_DEFINES) $@      $<"
	@$(SPEC_COMPILE) $< -o $@

ifeq ($(UNION_USE_GCC),1)
unionMsgBuf5.3.useSelfMsgIndex.o:  unionMsgBuf5.3.c unionMsgBuf5.3.d
else
unionMsgBuf5.3.useSelfMsgIndex.o:  unionMsgBuf5.3.c
endif
	@echo "CC $(SPEC_DEFINES1) $@      $<"
	@$(SPEC_COMPILE1) $< -o $@


ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)unionMsgBuf.h	\
	$(INCDIR)UnionMsgBuf3.x.h

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
	
