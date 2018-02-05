# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionComplexDB
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)
SPEC_DEFINES = -D _UNIX  $(OSSPEC) -D _UnionEnv_3_x_ -D _MaxVarNum_4096_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs=	unionComplexDBCommon.20080801.o		\
	unionComplexDBDataTransfer.20080801.o	\
	unionComplexDBObjectFileName.20080801.o	\
	unionComplexDBObjectSql.20080801.o		\
	unionComplexDBObjectDef.20080801.o		\
	unionComplexDBObjectChildDef.20080801.o	\
	unionComplexDBPrimaryKey.20080801.o		\
	unionComplexDBUniqueKey.20080801.o		\
	unionComplexDBRecordValue.20080801.o		\
	unionComplexDBRecordParents.20080801.o	\
	unionComplexDBRecordChild.20080801.o		\
	unionComplexDBObject.20080801.o		\
	unionComplexDBOperationDef.20080801.o		\
	unionComplexDBUnionQuery.20080801.o		\
	unionComplexDBTrigger.20080801.o		\
	unionComplexDBRecord.20080801.o \
	unionComplexDBCursors.20080801.o	\
	unionInputTBLData.20090428.o	\
	unionOutputTBLData.20090428.o	\
	InputTBLData.OnComplexDB.o	\
	OutputTBLData.OnComplexDB.o	\
	unionComplexDBObjectDef.readTblDefFromImage.20120601.o

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
InputTBLData.OnComplexDB.o:  unionInputTBLData.20090428.c unionInputTBLData.20090428.d
else
InputTBLData.OnComplexDB.o:  unionInputTBLData.20090428.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
OutputTBLData.OnComplexDB.o:  unionOutputTBLData.20090428.c unionOutputTBLData.20090428.d
else
OutputTBLData.OnComplexDB.o:  unionOutputTBLData.20090428.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@
	
ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)unionComplexDBDataTransfer.h	\
	$(INCDIR)unionComplexDBUnionQuery.h	\
	$(INCDIR)unionComplexDBOperationDef.h	\
	$(INCDIR)unionComplexDBTrigger.h	\
	$(INCDIR)unionComplexDBCommon.h	\
	$(INCDIR)unionComplexDBObjectFileName.h	\
	$(INCDIR)unionComplexDBObjectSql.h	\
	$(INCDIR)unionComplexDBObjectDef.h	\
	$(INCDIR)unionComplexDBObjectChildDef.h	\
	$(INCDIR)unionComplexDBPrimaryKey.h	\
	$(INCDIR)unionComplexDBUniqueKey.h	\
	$(INCDIR)unionComplexDBRecordValue.h	\
	$(INCDIR)unionComplexDBRecordParents.h	\
	$(INCDIR)unionComplexDBRecordChild.h	\
	$(INCDIR)unionComplexDBObject.h	\
	$(INCDIR)unionComplexDBRecord.h	\
	$(INCDIR)unionComplexDBCursors.h	\
	$(INCDIR)unionTBLData.h
	
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
	
