//	Wolfgang Wang
//	2009/4/29

#ifndef _unionUserInterfaceMaintainer_
#define _unionUserInterfaceMaintainer_

#include "unionDataTBLList.h"

#define conInterfaceItemTagMainMenu		conTBLNameMainMenu
#define conInterfaceItemTagMenuGrp		conTBLNameMenuDef
#define conInterfaceItemTagMenuItem		conTBLNameMenuItemDef
#define conInterfaceItemTagMenuOperation	conTBLNameMenuItemOperationDef
#define conInterfaceItemTagOperationAuth	conTBLNameOperationAuthorization
#define conInterfaceItemTagView			conTBLNameViewList
#define conInterfaceItemTagTable		conTBLNameTableList
#define conInterfaceItemTagTableField		conTBLNameTableField
#define conInterfaceItemTagComplexField		conTBLNameComplexField
#define conInterfaceItemTagEnum			conTBLNameEnumValueDef
#define conInterfaceItemTagOperationTagDef	conTBLNameOperationList

#define conTblFldValueMethodUseEnum		100
#define conTblFldValueMethodUseTable		101
#define conTblFldValueMethodUseComplexFld	102
#define conTblFldValueMethodUseNonRes		103

/*
功能	
	从一个表域赋值方法中，读取资源名称和资源类型
输入参数
	valueStr	域赋值串
	sizeOfResName	资源名称缓冲大小
输出参数
	resName		资源名称
返回值
	>=0	成功,资源类型
	<0	错误码
*/
int UnionReadAttrFromFieldMethodDef(char *valueStr,char *resName,int sizeOfResName);

/*
功能	
	生成一个主菜单用到的指定的界面信息
输入参数
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecMainMenu(char *mainMenuName);

/*
功能	
	生成一个主菜单用到的指定的界面信息，并输出到指定文件中
输入参数
	mainMenuName	主菜单名称
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile(char *mainMenuName,char *fileName);

/*
功能	
	判断一个表是否被一个主菜单使用了
输入参数
	mainMenuName	主菜单
	tableName	表名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionIsTableUsedByMainMenu(char *mainMenuName,char *tableName);

/*
功能	
	收集一个主菜单用到的指定的界面信息
输入参数
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCollectAllInterfaceItemsOfSpecMainMenu(char *mainMenuName);

/*
功能	
	释放当前收集到的主菜单用到的指定的界面信息
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionFreeAllCollectedInterfaceItemsOfSpecMainMenu(char *mainMenuName);

/*
功能	
	生成一个菜单组用到的指定的界面信息
输入参数
	mainMenuName	主菜单名称
	menuType	菜单类型
	itemGrpName	菜单组名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecMenuGrp(char *mainMenuName,int menuType,char *menuGrpName);

/*
功能	
	生成一个菜单项用到的指定的界面信息
输入参数
	mainMenuName	主菜单名称
	itemGrpName	菜单组名称
	itemName	菜单项名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecMenuItem(char *mainMenuName,char *menuGrpName,char *itemName);

/*
功能	
	生成一个菜单操作用到的指定的界面信息
输入参数
	mainMenuName	主菜单名称
	viewName	视图名称
	operationName	菜单操作名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecOperation(char *mainMenuName,char *viewName,char *operationName);

/*
功能	
	生成一个视图用到的指定的界面信息
输入参数
	mainMenuName	主菜单
	viewName	视图名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecView(char *mainMenuName,char *viewName);

/*
功能	
	生成一个表用到的指定的界面信息
输入参数
	mainMenuName	主菜单
	tableName	表名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecTable(char *mainMenuName,char *tableName);

/*
功能	
	生成一个域清单用到的指定的界面信息
输入参数
	mainMenuName	主菜单
	tableName	表名称
	fieldList	域清单
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecTableFieldList(char *mainMenuName,char *tableName,char *fieldList);

/*
功能	
	生成一个域名称用到的指定的界面信息
输入参数
	mainMenuName	主菜单
	fieldName	域名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecTableField(char *mainMenuName,char *fieldName);

/*
功能	
	生成一个枚举用到的指定的界面信息
输入参数
	mainMenuName	主菜单
	enumName	枚举名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecEnum(char *mainMenuName,char *enumName);

/*
功能	
	生成一个复合域用到的指定的界面信息
输入参数
	mainMenuName	主菜单
	complexFldName	复合域名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecComplexFld(char *mainMenuName,char *complexFldName);

/*
功能	
	生成一个操作授权用到的指定的界面信息
输入参数
	mainMenuName	主菜单
	resID		资源ID
	operationID	操作ID
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecOperationAuth(char *mainMenuName,int resID,int operationID);

/*
功能	
	生成一个操作标识的界面信息
输入参数
	mainMenuName	主菜单
	serviceName	操作标识
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllInterfaceItemsOfSpecOperationTagDef(char *mainMenuName,char *serviceName);

/*
功能	
	删除所有没有用到的界面项,并将删除的项写入到指定文件中
输入参数
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedInterfaceItems(FILE *fp);

/*
功能	
	删除所有没有用到的界面项,并将删除的项写入到指定文件中
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile(char *fileName);

/*
功能	
	删除所有没有用到的复合域,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedComplexFldItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的枚举定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedEnumDefItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的视图定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedViewListItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的表域定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedTableFieldItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的授权定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedOperationAuthorizationItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的菜单项定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedMenuItemItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的菜单组定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedMenuGrpItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的主菜单定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedMainMenuItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的菜单操作定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedMenuOperationItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的操作清单定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedOperationListItems(char *mainMenuName,FILE *fp);

/*
功能	
	删除所有没有用到的表定义,并将删除的项写入到指定文件中
输入参数
	mainMenuName		主菜单名称
	fp			文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionDeleteAllUnusedTableListItems(char *mainMenuName,FILE *fp);

#endif
