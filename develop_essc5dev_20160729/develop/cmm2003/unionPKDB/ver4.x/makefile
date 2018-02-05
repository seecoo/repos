# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@

# 这个编译开关必须增加
DEFINES = -D _UnionPK_4_x_ -D _UnionPK_3_x_ $(OSSPEC)
SPEC_DEFINES = -D _UnionPK_4_x_ -D _UnionPK_3_x_ $(OSSPEC) -D _KDBClient_
SPEC_DEFINES1 = -D _UnionPK_4_x_ -D _UnionPK_3_x_ $(OSSPEC) -D _useKeySynchronizer_
SPEC_DEFINES2 = -D _UnionPK_4_x_ -D _UnionPK_3_x_ $(OSSPEC) -D _useKeySynchronizer_ -D _useKeyRecStr_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = unionPK.4.x.20060822.o	\
	unionPKDB.4.x.20060822.o	\
	unionPKDBLog.4.x.20060822.o	\
	mngPKDB.4.x.20060822.o	\
	mngPKDBLog.4.x.20060822.o	\
	unionPKDB.4.x.DBClient.20060822.o	\
	unionPKDBLog.4.x.useSynchronizer.20060822.o	\
	unionPKDBLog.4.x.useSynchronizer.useKeyRecStr.20060822.o

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
unionPKDB.4.x.DBClient.20060822.o:  unionPKDB.4.x.20060822.c unionPKDB.4.x.20060822.d
else
unionPKDB.4.x.DBClient.20060822.o:  unionPKDB.4.x.20060822.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionPKDBLog.4.x.useSynchronizer.20060822.o:  unionPKDBLog.4.x.20060822.c unionPKDBLog.4.x.20060822.d
else
unionPKDBLog.4.x.useSynchronizer.20060822.o:  unionPKDBLog.4.x.20060822.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionPKDBLog.4.x.useSynchronizer.useKeyRecStr.20060822.o:  unionPKDBLog.4.x.20060822.c unionPKDBLog.4.x.20060822.d
else
unionPKDBLog.4.x.useSynchronizer.useKeyRecStr.20060822.o:  unionPKDBLog.4.x.20060822.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)unionPK.h	\
	$(INCDIR)unionPKDB.h	\
	$(INCDIR)unionPKDBLog.h

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
	
