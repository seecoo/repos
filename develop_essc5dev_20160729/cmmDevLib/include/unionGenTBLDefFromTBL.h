//	Wolfgang Wang
//	2009/4/29

#ifndef _unionGenTBLDefFromTBL_
#define _unionGenTBLDefFromTBL_

#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionFldGrp.h"
#include "unionTBLQueryConf.h"
#include "unionMenuItemOperationDef.h"
#include "unionMenuItemDef.h"
#include "unionOperatorType.h"

/*
功能	
	删除指定表的操作定义
输入参数
	tableName	表名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteOperationOfSpecTable(char *tableName);

/*
功能	
	删除表定义
输入参数
	tableName	表名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteSpecTableByName(char *tableName);

/*
功能	
	下载复杂域的赋值方法到文件中
输入参数
	mainMenuName	主菜单名称
	fldID		域标识名称
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateComplexFldAssignmentMethodToSpecFile(char *mainMenuName,char *fldID,char *fileName);
/*
功能	
	下载复杂域的赋值方法到文件中
输入参数
	mainMenuName	主菜单名称
	fldID		域标识名称
	fp		文件标识
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateComplexFldAssignmentMethodToFp(char *mainMenuName,char *fldID,FILE *fp);

/*
功能	
	根据菜单组名称，产生一个菜单定义文件
输入参数
	tellerTypeID	操作员级别
	mainMenuName	主菜单名称
	menuGrpName	菜单名称
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMenuDefFileFromMenuNameToFp(char *tellerTypeID,char *mainMenuName,char *menuGrpName,FILE *fp);

/*
功能	
	将一个菜单项定义写到文件中
输入参数
	pmenuItemDef	菜单项定义
	prec		菜单项操作定义
	fp		文件指针
	iDownloadAllMenuItem	是否需要读取所有操作级别的数据
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionWriteMenuItemDefToSpecFile(PUnionMenuItemDef pmenuItemDef,PUnionMenuItemOperationDef prec,FILE *fp, int iDownloadAllMenuItem);

/*
功能	
	读取指定表的表右键菜单项
输入参数
	level			操作级别
	tableName		表名
	mainMenuName		主菜单名称
	sizeOfMenuItemList	菜单项清单的最大长度
输出参数
	menuItemList	菜单项清单
返回值
	>=0	菜单项清单的长度
	<0	错误码
*/
int UnionReadMenuItemListOfSpecTable(char *tellerTypeID,char *tableName,char *mainMenuName,int menuType,char *menuItemList,int sizeOfMenuItemList);

/*
功能	
	读取指定的级别对指定的二级菜单的菜单项的操作权限清单
输入参数
	level			操作级别
	mainMenuName		主菜单名称
	menuName		二级菜单名称
	sizeOfMenuItemList	菜单项清单的最大长度
输出参数
	menuItemList	菜单项清单
返回值
	>=0	菜单项清单的长度
	<0	错误码
*/
int UnionReadMenuItemListOfSpecSecondaryMenu(char *tellerTypeID,char *mainMenuName,char *menuName,char *menuItemList,int sizeOfMenuItemList);

/*
功能	
	读取指定的级别对指定的主菜单的菜单项的操作权限清单
输入参数
	tellerTypeID	操作级别
	menuName	主菜单名称
	sizeOfMenuItemList	菜单项清单的最大长度
输出参数
	menuItemList	菜单项清单
返回值
	>=0	菜单项清单的长度
	<0	错误码
*/
int UnionReadMenuItemListOfSpecMainMenu(char *tellerTypeID,char *menuName,char *menuItemList,int sizeOfMenuItemList);

/*
功能	
	根据域清单(每个域都在tableField表中定义)，产生一个域定义清单
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
	maxFldNum	最大域数目
输出参数
	fldDefGrp	读出的域数组
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateFldGrpDefFromFldIDList(char *fldList,int lenOfFldList,TUnionObjectFldDef fldDefGrp[],int maxFldNum);

/*
功能	
	根据域清单(每个域都在tableField表中定义)，产生一个域定义清单
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
输出参数
	pfldListGrp	读出的域组
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateFldListFromFldIDList(char *fldList,int lenOfFldList,PUnionObjectFldGrp pfldListGrp);


/*
功能	
	根据域清单，产生一个外键结构
输入参数
	foreignTBLName		外表名称
	myFldList		本表域标识清单
	lenOfMyFldList		本域标识清单的长度
	foreignFldList		本表域标识清单
	lenOfForeignFldList	本域标识清单的长度
输出参数
	pforeignKeyRec	读出的外键结构
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateForeignKeyRecFromFldIDList(char *foreignTBLName,char *myFldList,int lenOfMyFldList,char *foreignFldList,int lenOfForeignFldList,PUnionDefOfRelatedObject pforeignKeyRec);
/*
功能	
	根据表定义(在tableList表中定义)，产生一个表定义结构
输入参数
	tblName		表名
输出参数
	pobject		表定义
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateObjectDefFromTBLDefInTableList(char *tblName,PUnionObject pobject);

/*
功能	
	根据视图定义(在viewList表中定义)，产生一个表定义结构
输入参数
	tblName		表名
	mainMenuName	主菜单
输出参数
	prec		表定义
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateObjectDefFromTBLDefInViewList(char *tblName,char *mainMenuName,PUnionObject prec);

/*
功能	
	根据表定义(在tableList表中定义)，创建一个表定义文件
输入参数
	tblName		表名
	overwriteMode	覆盖标志，如果文件已存在
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateObjectDefFromTBLDefInTableList(char *tblName,int overwriteMode);

/*
功能	
	根据表清单表中的记录，产生所有表的定义文件
输入参数
	modeWhenFileExists	如果文件已存在的操作方法
输出参数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateAllObjectDefFromTBLDefInTableList(int modeWhenFileExists);

/*
功能	
	根据表定义(在tableList表中定义)，创建一个建表文件
输入参数
	tblName		表名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList(char *tblName);

/*
功能	
	根据表清单表中的记录，产生所有表的建表文件
输入参数
	无
输出参数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateAllComplexDBTBLCreateSQLFileFromTBLDefInTableList();

/*
功能	
	根据表定义(在tableList表中定义)，创建一个建表的sql语句
输入参数
	tblName		表名
	fp		文件句柄
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateObjectDefSQLFromTBLDefInTableList(char *tblName,FILE *fp);

/*
功能	
	根据表定义(在tableList表中定义)，创建一个建表的sql语句
输入参数
	tblName		表名
	fileName	存储建表语句的文件
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile(char *tblName,char *fileName);

/*
功能	
	根据表清单表中的记录，产生所有表的建表SQL语句
输入参数
	fp	文件句柄
输出参数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateAllObjectDefSQLFromTBLDefInTableList(FILE *fp);

/*
功能	
	根据表清单表中的记录，产生所有表的建表SQL语句
输入参数
	fileName	存储建表语句的文件
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateAllObjectDefSQLFromTBLDefInTableListToSpecFile(char *fileName);

/*
功能	
	根据域清单(每个域都在tableField表中定义)，产生一个域定义清单
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
输出参数
	pfldGrp		读出的域定义
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateQueryFldGrpDefFromFldIDList(char *fldList,int lenOfFldList,PUnionQueryFldGrp pfldGrp);

/*
功能	
	根据域清单(每个域都在tableField表中定义)，产生一个域名称数组
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
	maxNum		数组可以接收的最大域数量
输出参数
	fldName		读出的域名称数组
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGetAllFldNameFromFldIDList(char *fldList,int lenOfFldList,char fldNameGrp[][128+1],int maxNum);

/*
功能	
	根据域清单(每个域都在tableField表中定义)，产生一个域定义清单
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
输出参数
	pfldListGrp	读出的域组
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateQueryFldListFromFldIDList(char *fldList,int lenOfFldList,PUnionFldGrp pfldListGrp);

/*
功能	
	将一个赋值方法写入到文件中
输入参数
	method		赋值方法
	fp		文件句柄
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintFldRecValueAssignMethodToFp(char *method,FILE *fp);

/*
功能	
	将域的名称使用指定的别名替换
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
输出参数
	pfldGrp		替换后的域组
返回值
	>=0	域数目
	<0	错误码
*/
int UnionRenameFldOfSpecQueryFldGrpDef(char *fldList,int lenOfFldList,PUnionQueryFldGrp pfldGrp);

/*
功能	
	根据操作授权表生成操作提示
输入参数
	fp	文件句柄
输出参数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateOperationInfoFromAuthDefTBL(FILE *fp);

/*
功能	
	根据操作授权表生成操作提示
输入参数
	fileName	存储建表语句的文件
输出参数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateOperationInfoFromAuthDefTBLToSpecFile(char *fileName);

/*
功能	
	根据域清单(每个域都在tableField表中定义)，写一组域赋值方法
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
	fieldAlaisList	域别名清单
	fieldAlaisListLen	域别名长度
	loopList	循环录入的域清单
	lenOfLoopList	循环录入的域清单的长度
	viewLevelList	可视域级别清单
	lenOfViewLevelList	可视域级别清单长度
	editLevelList	可编辑域级别清单
	lenOfEditLevelList	可编辑域级别清单长度
	fp		文件句柄
输出参数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateAssignmentQueryFldListFromFldIDList(char *fldList,int lenOfFldList,char *fieldAlaisList,int fieldAlaisListLen,
	char *loopList,int lenOfLoopList,char *viewLevelList,int lenOfViewLevelList,char *editLevelList,int lenOfEditLevelList,FILE *fp);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个表定义结构
输入参数
	tblName		表名
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateTBLQueryConfFromTBLDefInTableListToFp(char *tblName,FILE *fp);

/*
功能	
	根据视图定义(在viewList表中定义)，产生一个表定义结构
输入参数
	tblName		表名
	mainMenuName	主菜单名称
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateTBLQueryConfFromViewDefToFp(char *tblName,char *mainMenuName,FILE *fp);

/*
功能	
	根据视图定义(在viewList表中定义)，产生一个表定义结构
输入参数
	tblName		表名
	mainMenuName	主菜单名称
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateTBLQueryConfFromViewDefToSpecFile(char *tblName,char *mainMenuName,char *fileName);

/*
功能	
	产生一组空操作菜单项
输入参数
	tblName		表名
	num		空操作数目
输出参数
	无
返回值
	>=0	插入的记录数目
	<0	错误码
*/
int UnionGenerateNullTableMenuItemGrp(char *tableName,int num);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个表定义结构
输入参数
	tblName		表名
	fileName	文件名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile(char *tblName,char *fileName);

/*
功能	
	根据域清单(每个域都在tableField表中定义)，产生一个域引用清单
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
输出参数
	fldRef		域引用清单
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateQueryFldRefFromFldIDList(char *fldList,int lenOfFldList,char *fldRef);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个记录双击界面菜单
输入参数
	tellerTypeID		操作员级别
	tblName		表名
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个记录双击界面菜单
输入参数
	tellerTypeID	操作员级别
	tblName		表名
	fileName	存储建表语句的文件
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个记录右键界面菜单
输入参数
	tellerTypeID		操作员级别
	tblName		表名
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPopupMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个记录右键界面菜单
输入参数
	tellerTypeID		操作员级别
	tblName		表名
	fileName	存储建表语句的文件
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPopupMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName);

/*
功能	
	根据菜单项清单，产生一个界面菜单
输入参数
	tblName		表名
	tblAlais	表别名
	fldList		菜单项清单
	lenOfFldList	菜单项清单长度
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateInterfaceMenuFromMenuItemListToFp(char *tblName,char *tblAlais,char *fldList,int lenOfFldList,FILE *fp);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个界面菜单
输入参数
	tellerTypeID		操作员级别
	tblName		表名
	menuType	菜单类型
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateInterfaceMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,int menuType,FILE *fp);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个界面右键界面菜单
输入参数
	tellerTypeID		操作员级别
	tblName		表名
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateFormPopupMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个界面右键界面菜单
输入参数
	tellerTypeID		操作员级别
	tblName		表名
	fileName	存储建表语句的文件
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateFormPopupMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName);

/*
功能	
	根据表域的ID，读出域的名称
输入参数
	fldID		表域的ID
输出参数
	fldName		域的名称
返回值
	>=0	成功
	<0	错误码
*/
int UnionReadFieldNameByFieldIDInTableField(char *fldID,char *fldName);

/*
功能	
	根据表定义(在tableList表中定义)，创建一个表
输入参数
	tblName		表名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateObjectFromTBLDefInTableList(char *tblName);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个表的操作允可
输入参数
	tblName		表名
输出参数
	无
返回值
	>=0	插入的记录数目
	<0	错误码
*/
int UnionGenerateOperationFromTBLDefInTableList(char *tblName);

/*
功能	
	根据表定义(在tableList表中定义)，录入一个表的初始记录
输入参数
	tblName		表名
输出参数
	无
返回值
	>=0	插入的记录数目
	<0	错误码
*/
int UnionInsertInitRecOfSpecTBLOnTBLDefInTableList(char *tblName);

/*
功能	
	为一个表生成所有配置
输入参数
	tellerTypeID	操作员级别标识
	tableName	表名
输出参数
	fileRecved	是否有文件生成
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateAllMngSvrConfForTable(char *tellerTypeID,char *tableName,int *fileRecved);

/*
功能	
	为一个表生成所有配置
输入参数
	tellerTypeID	操作员级别
	tableName	表名
	tableAlais	表别名
	isView		是否是视图，1是，0不是
输出参数
	fileRecved	是否有文件生成
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateAllMngSvrConfForSpecTBLOrSpecView(char *tellerTypeID,char *tableName,char *tableAlais,int isView,int *fileRecved);

/*
功能	
	根据表分类，产生主菜单
输入参数
	无
输出参数
	无
返回值
	>=0	创建的数目
	<0	错误码
*/
int UnionCreateMainMenuByTBLType();

/*
功能	
	根据表分类，产生二级菜单
输入参数
	nameOf2LevelMenu	二级菜单名称
输出参数
	无
返回值
	>=0	创建的数目
	<0	错误码
*/
int UnionCreate2LevelMenuByTBLType(char *nameOf2LevelMenu);

/*
功能	
	根据二级菜单名称，产生二级菜单
输入参数
	tellerTypeID	操作员标识
	mainMenuName	主菜单名称
	menuGrpName	二级菜单名称
	fileName		生成的菜单文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreate2LevelMenuByMenuGrpName(char *tellerTypeID,char *mainMenuName,char *menuGrpName,char *fileName);

/*
功能	
	根据主菜单名称，产生主菜单
输入参数
	tellerTypeID	操作员级别标识
	mainMenuName	主菜单名称
	fileName		生成的菜单文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateMainMenuByMainMenuName(char *tellerTypeID,char *mainMenuName,char *fileName);

/*
功能	
	读取指定的级别对指定的菜单组的操作权限清单
输入参数
	tellerTypeID		操作级别
	mainMenuName		主菜单名称
	menuName		菜单组名称
	sizeOfMenuItemList	菜单项清单的最大长度
输出参数
	menuItemList	菜单项清单
返回值
	>=0	菜单项清单的长度
	<0	错误码
*/
int UnionReadMenuItemListOfSpecMenuGrp(char *tellerTypeID,char *mainMenuName,char *menuGrpName,char *menuItemList,int sizeOfMenuItemList);

/*
功能	
	将一个操作级别清单（操作类型）转换为操作级别清单（级别）
输入参数
	oriList		源级别串
	lenOfOriList	源级别串长度
	sizeOfBuf	目标级别串缓冲大小
输出参数
	desList		目标级别串
返回值
	>=0	成功，目标级别串长度
	<0	错误码
*/
int UnionConvertOperatorLevelListFromIDToLevel(char *oriList,int lenOfOriList,char *desList,int sizeOfBuf);
/*
功能	
	根据表分类，产生所有菜单
输入参数
	无
输出参数
	fileRecved	是否有文件生成
返回值
	>=0	创建的数目
	<0	错误码
*/
int UnionCreateAllMenuByTBLType(int *fileRecved);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个界面菜单
输入参数
	tellerTypeID		操作员级别
	viewName		表名
	mainMenuName	主菜单名称
	menuType	菜单类型
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateInterfaceMenuFromViewDefToFp(char *tellerTypeID,char *viewName,char *mainMenuName,int menuType,FILE *fp);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个界面菜单
输入参数
	viewName		表名
	mainMenuName		主菜单名称
	tellerTypeID		操作员级别
	menuType	菜单类型
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateInterfaceMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,int menuType,char *fileName);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个视图界面弹出菜单
输入参数
	viewName		表名
	mainMenuName		主菜单名称
	tellerTypeID		操作员级别
	menuType	菜单类型
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateFormPopMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个视图记录弹出菜单
输入参数
	viewName		表名
	mainMenuName		主菜单名称
	tellerTypeID		操作员级别
	menuType	菜单类型
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPopMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个双击记录弹出菜单
输入参数
	viewName		表名
	mainMenuName		主菜单名称
	tellerTypeID		操作员级别
	menuType	菜单类型
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecDoubleClickMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName);

/*
功能	
	根据表定义(在tableList表中定义)，产生一个表的缺省视图定义
输入参数
	tblName		表名
	mainMenuName	主菜单名称
	sizeOfBuf	缓冲大小
输出参数
	viewRecStr	视图定义串
返回值
	>=0	视图定义串的长度
	<0	错误码
*/
int UnionGenerateDefaultViewOfTBL(char *tblName,char *mainMenuName,char *viewRecStr,int sizeOfBuf);

/*
功能	
	根据枚举文件名称，产生一个枚举定义文件
输入参数
	tellerTypeID	操作员级别
	mainMenuName	主菜单名称
	enumFileName	枚举文件名称
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateEnumValueDefFileToFp(char *tellerTypeID,char *mainMenuName,char *enumFileName,FILE *fp);

/*
功能	
	根据枚举文件名称，产生一个枚举定义文件
输入参数
	tellerTypeID	操作员级别
	mainMenuName	主菜单名称
	enumFileName	枚举文件名称
	fileName		生成的菜单文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateEnumValueDefFile(char *tellerTypeID,char *mainMenuName,char *enumFileName,char *fileName);

/*
功能	
	根据表中的记录，为一个主菜单定义enum，一个记录对应一个enum值
输入参数
	tblName		表名
	mainMenuName	主菜单名称
	enumFileName	枚举名称
输出参数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateEnumDefFromTBLForSpecMainMenu(char *tblName,char *mainMenuName,char *enumFileName);

/*
功能	
	将一个视图的操作授权写入到文件中
输入参数
	mainMenuName	主菜单名称
	viewName	视图的名称
	fp		文件名柄
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateViewAuthorizationToFp(char *mainMenuName,char *viewName,FILE *fp);

/*
功能	
	将一个视图的操作授权写入到文件中
输入参数
	mainMenuName	主菜单名称
	viewName	视图的名称
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateViewAuthorizationToSpecFile(char *mainMenuName,char *viewName,char *fileName);

/*
功能	
	获取所有操作员级别
输入参数
	无
输出参数
	levelStr	所有操作员级别构成的串
返回值
	>=0	成功
	<0	错误码
*/
int UnionReadAllOperatorLevelStr(char *levelStr);

/*
功能	
	将所有级别显示到文件中
输入参数
	levelStr	所有操作员级别构成的串
	levelName	级别显示名称
	authoSet	授权标志
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintAllOperatorLevelStrToFp(char *levelStr,char *levelName,int authoSet,FILE *fp);

/*
功能	
	将授权级别显示到文件中
输入参数
	levelStr	所有操作员级别构成的串
	levelName	级别显示名称
	authLevelStr	授权级别串
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintAllAuthOperatorLevelStrToFp(char *levelStr,char *levelName,char *authLevelStr,FILE *fp);

/*
功能	
	将授权级别显示到文件中
输入参数
	tellerTypeGrp	所有操作员级别定义
	tellerTypeNum	操作员级别数量
	levelName	级别显示名称
	authLevelIDStr	授权级别标识串
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintAllAuthOperatorLevelIDStrToFp(TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelName,char *authLevelIDStr,FILE *fp);

/*
功能	
	获取操作员级别标识清单对应的操作员级别清单
输入参数
	tellerTypeGrp	所有操作员级别定义
	tellerTypeNum	操作员级别数量
	levelIDStr	操作员级别标识清单
输出参数
	levelStr	操作员级别构成的串
返回值
	>=0	成功
	<0	错误码
*/
int UnionReadAllOperatorLevelStrOfSpecLevelIDList(char *levelIDStr,TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelStr);

/*
功能	
	组成所有操作员级别清单串
输入参数
	tellerTypeGrp	所有操作员级别定义
	tellerTypeNum	操作员级别数量
输出参数
	levelStr	操作员级别构成的串
返回值
	>=0	成功
	<0	错误码
*/
int UnionFormAllOperatorLevelStr(TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelStr);

/*
功能	
	生成一个菜单组的定义文档
输入参数
	mainMenuName	主菜单名称
	menuGrpName	菜单名称
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMenuGrpDefDocToFp(char *mainMenuName,char *menuGrpName,FILE *fp);

/*
功能	
	生成一个菜单组的定义文档
输入参数
	mainMenuName	主菜单名称
	menuGrpName	菜单名称
	fileName	文件名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMenuGrpDefDocToSpecFile(char *mainMenuName,char *menuGrpName,char *fileName);


/*
功能	
	根据视图定义(在tableList表中定义)，产生一个界面菜单定义文档
输入参数
	viewName	视图名
	mainMenuName	主菜单名称
	menuType	菜单类型
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateInterfaceMenuDefDocFromViewDefToFp(char *viewName,char *mainMenuName,int menuType,FILE *fp);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个界面菜单定义文档
输入参数
	viewName	视图名
	mainMenuName	主菜单名称
	menuType	菜单类型
	fileName		文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,int menuType,char *fileName);
	
/*
功能	
	根据视图定义(在tableList表中定义)，产生一个视图界面弹出菜单定义文档
输入参数
	viewName		表名
	mainMenuName		主菜单名称
	fileName		文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateFormPopMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个视图记录弹出菜单定义文档
输入参数
	viewName		表名
	mainMenuName		主菜单名称
	fileName		文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPopMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName);

/*
功能	
	根据视图定义(在tableList表中定义)，产生一个视图记录弹出菜单定义文档
输入参数
	viewName		表名
	mainMenuName		主菜单名称
	fileName		文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecDoubleClickMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName);

/*
功能	
	生成指定主菜单定义文档
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0			菜单项清单的长度
	<0			错误码
*/
int UnionGenerateMainMenuDefDocToFp(char *mainMenuName,FILE *fp);
/*
功能	
	生成指定主菜单定义文档
输入参数
	mainMenuName		主菜单名称
	fileName		文件名称
输出参数
	无
返回值
	>=0			菜单项清单的长度
	<0			错误码
*/
int UnionGenerateMainMenuDefDocToSpecFile(char *mainMenuName,char *fileName);

/*
功能	
	根据主菜单名称，产生主菜单显示文件
输入参数
	tellerTypeID	操作员级别标识
	mainMenuName	主菜单名称
	fileName		生成的菜单文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMainMenuForTestToSpecFile(char *tellerTypeID,char *mainMenuName,char *fileName);

/*
功能	
	读取所有主菜单定义
输入参数
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllMainMenuDefToFp(FILE *fp);

/*
功能	
	读取所有主菜单定义
输入参数
	
	fileName		生成的菜单文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllMainMenuDefToSpecFile(char *fileName);

/*
功能	
	拆分域值,读取服务报文域定义
输入参数
	cpField		安全服务中定义的请求域或响应域
	iLenOfField	域长度
	cDel		域的分隔符
	iType		域的类型,1:请求必选域,2:请求可选域,3:响应必选域,4:响应可选域
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSecuServiceDefInDataPackFldListFormatToFp(char *cpField, int iLenOfField, char cDel, int iType, FILE *fp);

/*
功能	
	读取服务报文域定义
输入参数
	iServiceID	服务代码
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSecuServiceDefDocToFp(int iServiceID, FILE *fp);

/*
功能	
	根据服务代码产生服务报文域定义文件
输入参数
	iServiceID	服务代码
	fileName	生成的菜单文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSecuServiceDefDocToSpecFile(int iServiceID, char *fileName);

/*
功能	
	将一个域组写成串
输入参数
	flag		域组的标识
	pfldGrp		域组
	iSizeofStr	字符串缓冲区大小
输入出数
	str			字符串
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateFldGrpIntoSQLToString(char *flag, PUnionObjectFldGrp pfldGrp, char *str, int iSizeofStr);

/*
功能	
	从一个表定义创建SQL语句
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGetCreateSQLFromObjectDef(PUnionObject prec, char *cpCreateSQL, int iSizeOfCreateSQL);

/*
功能	
	在数据库中建表
输入参数
	tblName		表名
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateTableByTBLDefInTableList(char *tblName);

#endif
