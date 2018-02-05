# 定义开发目录
INCDIR = ../include/
OBJDIR = $(CMM2003LIBDIR)

OSNAME = $(shell uname)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I$(INCDIR) -I$(CMM2003INCDIR)
INCL +=  -I$(ORACLE_HOME)/precomp/public -I$(ORACLE_HOME)/rdbms/public/
INCL +=  -I$(DIROFDB2)/include/
INCL +=  -I$(INFORMIXDIR)/incl/cli/

ifeq ($(OSNAME),Linux)
INCL +=  $$(mysql_config --include)
endif


#传给脚本的参数
SLP 	= "$*.o"	"$@ $*.o"	$@
SLP1  	= "$*.o"	"$*.d $*.o"	$@
SLP2 	= "$*.c" 	$< 		$@

# 这个编译开关必须增加
DEFINES = $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

PROC = proc
ESQL = esql 

srcs	= $(wildcard *.c)
pcsrcs	= $(wildcard *.pc)

ifeq ($(OSNAME),AIX)
srcs	:=$(filter-out unionRealBaseDB.mysql.2.0.c, $(srcs))
endif

objs =	$(patsubst %.c,%.o,$(srcs)) 
objs +=	$(patsubst %.pc,%.o,$(pcsrcs)) 
deps =	$(patsubst %.o,%.d,$(objs)) 

all:	makeall		

makeall:	$(objs)

.SUFFIXES:.o .c .pc
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP)
	
.pc.o:
	@echo "PROC $@ $<"
	$(PROC) $(subst -I,INCLUDE=,$(INCL)) PARSE=none INAME=$<
	$(CC) $(DEFINES) $(INCL) -c $(<:%.pc=%.c) -o $(OBJDIR)$*.o
	@rm -f $*.c 
	@rm -f *.lis
	@rm -f tp*

$(OBJDIR)%.d:%.pc
	@echo "DEP $@	$<"
	@echo "$(PROC) $(subst -I,INCLUDE=,$(INCL)) PARSE=none INAME=$<"
	$(PROC) $(subst -I,INCLUDE=,$(INCL)) PARSE=none INAME=$<
	@echo "$(DEPEND) $*.c > $@"
	$(DEPEND) $*.c > $@
	@sh file_substr.x $(SLP1)
	@sh file_substr.x $(SLP2)
	@rm -f $*.c 
	@rm -f *.lis
	@rm -f tp*


ifeq ($(filter $(MAKECMDGOALS),clean install show),)
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes =	../include/unionRealBaseDB.h		\
		../include/inputAndOutputDataForDB.h

install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)

.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))
	rm -f $(patsubst %.pc,%.c,$(pcsrcs)) $(patsubst %.ec,%.c,$(ecsrcs)) $(patsubst %.sqc,%.c,$(sqcsrcs)) $(patsubst %.myc,%.c,$(mycsrcs))

show:
	make -v
	@echo '=============================================================='
	@echo 'OSNAME	:' $(OSNAME)
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
