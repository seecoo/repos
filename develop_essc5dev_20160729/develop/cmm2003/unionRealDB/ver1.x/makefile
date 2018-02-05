COMPILEFLAG=ORACLE
# 定义开发目录
INCDIR = ../include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I$(INCDIR) -I$(CMM2003INCDIR)
PCINCL = $(INCL) -I$(ORACLE_HOME)/precomp/public
SQCINCL = $(INCL) -I$(DIROFDB2)/include/
MYCINCL = $(INCL) $$(mysql_config --include)
INFINCL = $(INCL) -I$(INFORMIXDIR)/incl/esql/

#传给脚本的参数
SLP 	= "$*.o"	"$@ $*.o"	$@
SLP1  	= "$*.o"	"$*.d $*.o"	$@
SLP2 	= "$*.c" 	$< 		$@

# 这个编译开关必须增加
DEFINES = $(OSSPEC)
SQCDEFINES = $(OSSPEC) -D _IS_USE_DB2_
ifneq ($(OS),AIX)
ifeq ($(UNION_USE_BITS),64)
        SQCDEFINES = $(OSSPEC) -D _IS_USE_DB2_  -D __64BIT__
endif
endif

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
COMPILE_INF = $(CC) $(DEFINES) $(INFINCL) -c
DEPEND	= $(CC) $(INCL) -MM -c
SQC_DEPEND = $(CC) $(SQCINCL) -MM -c
ESQL_DEPEND	= $(ESQL) $(INCL) -MM -c

VPATH	= $(OBJDIR)

PROC = proc
ESQL = esql 
DB2 = db2 

srcs	= $(wildcard *.c)

ifeq ($(COMPILEFLAG),ALL)
	pcsrcs	= $(wildcard *.pc)
	ecsrcs	= $(wildcard *.ec)
	sqcsrcs	= $(wildcard *.sqc)
	mycsrcs	= $(wildcard *.myc)
endif

ifeq ($(COMPILEFLAG),MYSQL)
	mycsrcs	= $(wildcard *.myc)
endif

ifeq ($(COMPILEFLAG),ORACLE)
	pcsrcs	= $(wildcard *.pc)
endif

ifeq ($(COMPILEFLAG),DB2)
	sqcsrcs	= $(wildcard *.sqc)
endif

ifeq ($(COMPILEFLAG),INFORMIX)
	ecsrcs	= $(wildcard *.ec)
endif

ifeq ($(COMPILEFLAG),NODB2)
	pcsrcs	= $(wildcard *.pc)
	ecsrcs	= $(wildcard *.ec)
	mycsrcs	= $(wildcard *.myc)
endif

objs =	$(patsubst %.c,%.o,$(srcs)) 
objs +=	$(patsubst %.pc,%.o,$(pcsrcs)) 
objs +=	$(patsubst %.ec,%.o,$(ecsrcs)) 
objs +=	$(patsubst %.sqc,%.o,$(sqcsrcs))
objs +=	$(patsubst %.myc,%.o,$(mycsrcs))

deps =	$(patsubst %.o,%.d,$(objs)) 

all:	makeall		

makeall:	$(objs)

.SUFFIXES:.o .c .pc .ec .sqc .myc
.c.o:
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@
	
.pc.o:
	@echo "PROC $@ $<"
	@$(PROC) $(subst -I,INCLUDE=,$(PCINCL)) PARSE=none INAME=$<
	@$(CC) $(DEFINES) $(PCINCL) -c $(<:%.pc=%.c) -o $(OBJDIR)$*.o
	@rm -f $*.c 
	@rm -f *.lis
	@rm -f tp*

.ec.o:
	@echo "ESQL $@ $<"
	@$(ESQL) $(INCL) -e $<
	@$(COMPILE_INF) $*.c -o $(OBJDIR)$@
	rm -f $*.c 

#	$(ESQL) $(OSSPEC) $(INCL) -c $< -o $(OBJDIR)$*.o

.sqc.o:
	@echo "DB2 $@ $<"
	$(DB2) connect to ${DBNAME} user ${CURUSER} using ${CURPWDOFUSER}
	#$(DB2) prep $< PACKAGE BINDFILE USING $(word 1,$(subst ., ,$<)).$(word 2,$(subst ., ,$<)).$(UNIONCURRENTSRCVERSION).bnd VERSION $(UNIONCURRENTSRCVERSION)
	#$(DB2) bind $(word 1,$(subst ., ,$<)).$(word 2,$(subst ., ,$<)).$(UNIONCURRENTSRCVERSION).bnd grant public
	$(DB2) prep $< PACKAGE BINDFILE USING $*.$(UNIONCURRENTSRCVERSION).bnd VERSION $(UNIONCURRENTSRCVERSION)
	$(DB2) bind $*.$(UNIONCURRENTSRCVERSION).bnd grant public
	$(DB2) connect reset
	$(DB2) terminate
	$(CC) $(SQCDEFINES) $(SQCINCL) -c $(<:%.sqc=%.c) -o $(OBJDIR)$*.o
	rm -f $*.c 
	cp $(OBJDIR)$*.o $(CMM2003BNDDIR)/$*.$(UNIONCURRENTSRCVERSION).o
	cp *.bnd  $(CMM2003BNDDIR)

.myc.o:
	@echo "CC $@ $<"
	cp $< $*.c
	$(CC) $(DEFINES) $(MYCINCL) -c $*.c -o $(OBJDIR)$*.o
	@rm -f $*.c 

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP)
	
$(OBJDIR)%.d:%.pc
	@echo "DEP $@	$<"
	@$(PROC) $(subst -I,INCLUDE=,$(PCINCL)) PARSE=none INAME=$< > /dev/null
	@$(DEPEND) $*.c > $@
	@sh file_substr.x $(SLP1)
	@sh file_substr.x $(SLP2)
	@rm -f $*.c 
	@rm -f *.lis
	@rm -f tp*

$(OBJDIR)%.d:%.ec
	@echo "DEP $@	$<"
	@$(ESQL) $(OSSPEC) $(INCL) -c $<
	@$(ESQL_DEPEND) $*.c > $@
	@sh file_substr.x $(SLP1)
	@sh file_substr.x $(SLP2)
	@rm -f $*.o
	@rm -f $*.c
	
$(OBJDIR)%.d:%.sqc
	@echo "DEP $@	$<"
	@$(DB2) connect to ${DBNAME} user ${CURUSER} using ${CURPWDOFUSER}
	@$(DB2) prep $< PACKAGE BINDFILE USING $(word 1,$(subst ., ,$<)).$(word 2,$(subst ., ,$<)).$(UNIONCURRENTSRCVERSION).bnd VERSION $(UNIONCURRENTSRCVERSION)
	@$(DB2) connect reset
	@$(DB2) terminate
	@$(SQC_DEPEND) $*.c > $@
	@sh file_substr.x $(SLP1)
	@sh file_substr.x $(SLP2)
	@rm -f $*.c 

$(OBJDIR)%.d:%.myc
	@echo "DEP $@	$<"
	@cp $< $*.c
	$(DEPEND) $*.c > $@
	@sh file_substr.x $(SLP1)
	@sh file_substr.x $(SLP2)
	@rm -f $*.c 

ifeq ($(filter $(MAKECMDGOALS),clean install show),)
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes =	../include/unionRealBaseDB.h		\
		../include/unionRealDBCommon.h		\
		../include/unionRealDBTransformStr.h

install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)

.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))
	rm -f $(patsubst %.pc,%.c,$(pcsrcs)) $(patsubst %.ec,%.c,$(ecsrcs)) $(patsubst %.sqc,%.c,$(sqcsrcs)) $(patsubst %.myc,%.c,$(mycsrcs))

show:
	make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
