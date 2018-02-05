# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	mvLibs			\
	libProducts
	

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

mvLibs:	$(null)
	cp *.o							$(CMM2003LIBDIR)
libProducts:	$(null)
	mv *.o							$(CMM2003LIBDIR)
	cp $(INCDIR)/unionComplexField*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionGenTBLDefFromTBL*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionTableList*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionDataTBLList*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionTableField*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionOperationList*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionOperationAuthorization*.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionMngSvrOperationType*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionMenuItemType*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionTableMenu*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionLevel2Menu*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionMainMenu*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionOperatorType*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionViewList*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionViewList_struct_20090421*.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionSecondaryMenu*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionGenSQLFromTBLDefList*.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionMenuDef*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionMenuItemDef*.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionMenuItemOperationDef*.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionEnumValueDef*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionTransferComplexDBDef*.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionDataPackFldList*.h			$(CMM2003INCDIR)
	cp $(INCDIR)/unionSecurityServiceDef*.h			$(CMM2003INCDIR)
	

objs = 	unionGenTBLDefFromTBL.20090421.o	\
	unionComplexField.20090421.o	\
	unionMainMenu.20090421.o	\
	unionTransferComplexDBDef.20090421.o	\
	moveComplexDBDefIntoDBMain.20090421.o	\
	unionTableList.20090421.o	\
	unionGenIncFromTBL.20080925.o	\
	unionOperationList.20090421.o	\
	unionOperationAuthorization.20090421.o	\
	unionTableMenu.20090421.o	\
	unionMenuItemType.20090421.o	\
	unionMngSvrOperationType.20090421.o	\
	unionOperatorType.20090421.o	\
	unionLevel2Menu.20090421.o	\
	unionSecondaryMenu.20090421.o	\
	unionGenSQLFromTBLDefList.20090421.o	\
	unionViewList.20090421.o	\
	unionMenuDef.20090421.o	\
	unionMenuItemDef.20090421.o	\
	unionMenuItemOperationDef.20090421.o	\
	unionEnumValueDef.20090421.o	\
	unionDataPackFldList.20090421.o	\
	unionSecurityServiceDef.20090421.o	\
	unionTableField.20090421.o
makeall:	$(objs)

