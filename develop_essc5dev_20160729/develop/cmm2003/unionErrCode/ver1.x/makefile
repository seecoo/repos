# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionErrCode
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) `$(CMM2003OPENDIR)/bin/xml2-config --cflags`

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@
# 这个编译开关必须增加
DEFINES = $(OSSPEC) -D _2LenCliErrCode_ -D _maxErrCodeNum_1024_
SPEC_DEFINES = $(OSSPEC) -D _2LenCliErrCode_ -D _maxErrCodeNum_1024_ -D _unionErrCodeTranslater_2_x_
SPEC_DEFINES1 = $(OSSPEC) -D _2LenCliErrCode_ -D _maxErrCodeNum_1024_ -D _unionErrCodeTranslater_2_x_ -D _useComplexDB_ -D _withSoftErrCodeRemark_
SPEC_DEFINES2 = $(OSSPEC) -D _2LenCliErrCode_ -D _128ErrCode_ -D _withSoftErrCodeRemark_
SPEC_DEFINES3 = $(OSSPEC) -D _2LenCliErrCode_ -D _maxErrCodeNum_1024_  -D _useComplexDB_ -D _withSoftErrCodeRemark_
SPEC_DEFINES4 = $(OSSPEC) -D _2LenCliErrCode_ -D _maxErrCodeNum_1024_ -D _REMARK_128_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
SPEC_COMPILE3	= $(CC) $(SPEC_DEFINES3) $(INCL) -c
SPEC_COMPILE4	= $(CC) $(SPEC_DEFINES4) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs =	errCodeTranslater.1024ErrCode.2.x.20060726.o	\
	errCodeTranslater.1024ErrCode.20060726.o	\
	errCodeTranslater.20130301.o	\
	unionErrCodeTBL.20060726.o	\
	mngErrCodeTBL.20060726.o	\
	mngErrCodeTranslater.20060726.o	\
	errCodeTranslater.1024ErrCode.2.x.onComplexDB.20060726.o	\
	unionErrCodeTBL.onComplexDB.20060828.o	\
	unionErrCodeTBL.128ErrCode.2Len.withRemark.20060828.o	\
	errCodeTranslater.1024ErrCode.1.x.onComplexDB.20060828.o

all:	makeall

makeall:	$(objs)

.SUFFIXES:.c.o
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@        $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
errCodeTranslater.1024ErrCode.2.x.20060726.o:  errCodeTranslater.20060726.c errCodeTranslater.20060726.d
else
errCodeTranslater.1024ErrCode.2.x.20060726.o:  errCodeTranslater.20060726.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
errCodeTranslater.1024ErrCode.20060726.o:  errCodeTranslater.20060726.c errCodeTranslater.20060726.d
else
errCodeTranslater.1024ErrCode.20060726.o:  errCodeTranslater.20060726.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
errCodeTranslater.1024ErrCode.2.x.onComplexDB.20060726.o:  errCodeTranslater.20060828.c errCodeTranslater.20060828.d
else
errCodeTranslater.1024ErrCode.2.x.onComplexDB.20060726.o:  errCodeTranslater.20060828.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionErrCodeTBL.onComplexDB.20060828.o:  unionErrCodeTBL.20060828.c unionErrCodeTBL.20060828.d
else
unionErrCodeTBL.onComplexDB.20060828.o:  unionErrCodeTBL.20060828.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionErrCodeTBL.128ErrCode.2Len.withRemark.20060828.o:  unionErrCodeTBL.20060828.c unionErrCodeTBL.20060828.d
else
unionErrCodeTBL.128ErrCode.2Len.withRemark.20060828.o:  unionErrCodeTBL.20060828.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
errCodeTranslater.1024ErrCode.1.x.onComplexDB.20060828.o:  errCodeTranslater.20060828.c errCodeTranslater.20060828.d
else
errCodeTranslater.1024ErrCode.1.x.onComplexDB.20060828.o:  errCodeTranslater.20060828.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
errCodeTranslater.20130301.o:  errCodeTranslater.20130301.c errCodeTranslater.20130301.d
else
errCodeTranslater.20130301.o:  errCodeTranslater.20130301.c
endif
	@echo "CC $(SPEC_DEFINES4) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE4) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)/unionErrCodeTBL.h	\
	$(INCDIR)/errCodeTranslater.h
	
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
	
