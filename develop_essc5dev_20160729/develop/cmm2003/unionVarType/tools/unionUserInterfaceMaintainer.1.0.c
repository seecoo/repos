//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionErrCode.h"
#include "unionDatabaseCmd.h"
//#include "unionResID.h"
#include "UnionStr.h"
#include "unionGenIncFromTBL.h"
#include "unionGenTBLDefFromTBL.h"
#include "mngSvrConfFileName.h"
#include "mngSvrCommProtocol.h"

#include "unionComplexField.h"
#include "unionFldGrp.h"
#include "unionRecFile.h"
#include "unionTBLQueryConf.h"
#include "unionTableField.h"
#include "unionTableList.h"
#include "unionDataTBLList.h"
#include "unionOperationList.h"
#include "unionOperationAuthorization.h"
#include "unionMenuItemType.h"
#include "unionMainMenu.h"
#include "unionMenuDef.h"
#include "unionMenuItemDef.h"
#include "unionMenuItemOperationDef.h"
#include "unionViewList.h"
#include "unionOperatorType.h"
#include "unionEnumValueDef.h"
#include "unionDataPackFldList.h"
#include "unionSecurityServiceDef.h"
#include "unionComplexField.h"
#include "unionUserInterfaceMaintainer.h"
#include "unionUserInterfaceItemGrp.h"
#include "UnionLog.h"
#include "unionComplexDBRecord.h"

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
int UnionReadAttrFromFieldMethodDef(char *valueStr,char *resName,int sizeOfResName)
{
	int	len;
	char	*ptr = NULL;
	int	resType = conTblFldValueMethodUseNonRes;
	char	*ptr2 = NULL,tmpChar;
	int	copyLen;
	int	offset = 0;
	
	if ((len = strlen(valueStr)) == 0)
		return(resType);
	if ((ptr = strstr(valueStr,"query::table=")) != NULL)
	{
		offset = strlen("query::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"query::table=")) != NULL)
	{
		offset = strlen("form::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"useOtherFld::table=")) != NULL)
	{
		offset = strlen("useOtherFld::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"otherFld::table=")) != NULL)
	{
		offset = strlen("otherFld::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"enum::")) != NULL)
	{
		offset = strlen("enum::");
		resType = conTblFldValueMethodUseEnum;
	}
	else if ((ptr = strstr(valueStr,"auto.enum.")) != NULL)
	{
		offset = strlen("auto.enum.");
		resType = conTblFldValueMethodUseEnum;
	}
	else if ((ptr = strstr(valueStr,"enum.")) != NULL)
	{
		offset = strlen("enum.");
		resType = conTblFldValueMethodUseEnum;
	}
	else if ((ptr = strstr(valueStr,"ruleCols::table=73,condition=fldID=")) != NULL)
	{
		offset = strlen("ruleCols::table=73,condition=fldID=");
		resType = conTblFldValueMethodUseComplexFld;
	}
	else if ((ptr = strstr(valueStr,"complexFld")) != NULL)
	{
		offset = strlen("complexFld");
		resType = conTblFldValueMethodUseComplexFld;
	}
	else if ((ptr = strstr(valueStr,"complexField")) != NULL)
	{
		offset = strlen("complexField");
		resType = conTblFldValueMethodUseComplexFld;
	}
	else
		return(resType);
	
	if (resType == conTblFldValueMethodUseComplexFld)
		copyLen = strlen(ptr);
	else if (resType == conTblFldValueMethodUseTable)
	{
		if ((ptr2 = strstr(ptr,",")) == NULL)
			copyLen = strlen(ptr);
		else
		{
			tmpChar = ptr2[0];
			ptr2[0] = 0;
			copyLen = strlen(ptr);
			ptr2[0] = tmpChar;
		}
	}
	else if (resType == conTblFldValueMethodUseEnum)
	{
		if ((ptr2 = strstr(ptr,"^table=")) == NULL)
			copyLen = strlen(ptr);
		else	// 是表
		{
			resType = conTblFldValueMethodUseTable;
			ptr = ptr2;
			offset = strlen("^table=");
			if ((ptr2 = strstr(ptr,",")) == NULL)
				copyLen = strlen(ptr);
			else
			{
				tmpChar = ptr2[0];
				ptr2[0] = 0;
				copyLen = strlen(ptr);
				ptr2[0] = tmpChar;
			}
		}
	}		
	copyLen = copyLen - offset;
	if (copyLen >= sizeOfResName)
		copyLen = sizeOfResName - 1;
	if (copyLen < 0)
		copyLen = 0;
	memcpy(resName,ptr+offset,copyLen);
	return(resType);
}

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
int UnionGenerateAllInterfaceItemsOfSpecMainMenu(char *mainMenuName)
{
	int			ret;
	char			menuGrpList[1024+1];
	char			menuGrp[48][128];
	int			menuGrpNum;
	int			index;
	int			lenOfMenuGrpList;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
		
	// 读出主菜单的二级菜单组清单
	memset(menuGrpList,0,sizeof(menuGrpList));
	if ((lenOfMenuGrpList = UnionReadMainMenuRecFld(mainMenuName,conMainMenuFldNameMainMenuItemsList,menuGrpList,sizeof(menuGrpList))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: UnionReadMainMenuRecFld [%s]!\n",mainMenuName);
		return(lenOfMenuGrpList);
	}
	
	// 拼分二级菜单组
	if ((menuGrpNum = UnionSeprateVarStrIntoVarGrp(menuGrpList,lenOfMenuGrpList,',',menuGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionSeprateVarStrIntoVarGrp [%s] of [%s]!\n",menuGrpList,mainMenuName);
		return(menuGrpNum);
	}
		
	for (index = 0; index < menuGrpNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeSecondaryMenuGrp,menuGrp[index])) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
					mainMenuName,menuGrp[index]);
			strcpy(remark,"生成二级菜单组出错,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
		}
	}
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMainMenu,"mainMenuName=%s|",mainMenuName));
}

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
int UnionCollectAllInterfaceItemsOfSpecMainMenu(char *mainMenuName)
{
	int	ret;
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionInitInterfaceItemGrp(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionCollectAllInterfaceItemsOfSpecMainMenu:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionCollectAllInterfaceItemsOfSpecMainMenu::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	
	return(ret);
}

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
void UnionFreeAllCollectedInterfaceItemsOfSpecMainMenu(char *mainMenuName __attribute__((unused)))
{
	UnionDeleteInterfaceItemGrp();
	return;
}

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
int UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile(char *mainMenuName,char *fileName)
{
	int	ret;
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionInitInterfaceItemGrp(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	
	if ((ret = UnionPrintInterfaceItemGrpToSpecFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile::UnionPrintInterfaceItemGrpToSpecFile [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	UnionDeleteInterfaceItemGrp();
	return(ret);
}
		
/*
功能	
	生成一个主菜单的错误界面信息，并输出到指定文件中
输入参数
	mainMenuName	主菜单名称
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile(char *mainMenuName,char *fileName)
{
	int	ret;
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionInitInterfaceItemGrp(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	
	if ((ret = UnionPrintErrorInterfaceItemGrpToSpecFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile::UnionPrintErrorInterfaceItemGrpToSpecFile [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	UnionDeleteInterfaceItemGrp();
	return(ret);
}
		
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
int UnionGenerateAllInterfaceItemsOfSpecMenuGrp(char *mainMenuName,int menuType,char *menuGrpName)
{
	int			ret;
	char			menuItemsList[1024+1];
	char			itemGrp[48][128];
	int			itemGrpNum;
	int			index;
	int			lenOfMenuItemList;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	if (strlen(menuGrpName) == 0)
		return(0);
		
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagMenuGrp,"mainMenuName=%s|tableMenuType=%d|menuGrpName=%s|",mainMenuName,menuType,menuGrpName)) > 0)
		return(0);
	
	//UnionRealNullLog("in UnionGenerateAllInterfaceItemsOfSpecMenuGrp:: deal [%s][%s]...\n",mainMenuName,menuGrpName);
	// 读出菜单组的菜单项清单
	memset(menuItemsList,0,sizeof(menuItemsList));
	if ((lenOfMenuItemList = UnionReadMenuDefRecFld(mainMenuName,menuGrpName,conMenuDefFldNameMenuItemsList,menuItemsList,sizeof(menuItemsList))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuGrp:: UnionReadMenuDefRecFld [%s][%s]!\n",mainMenuName,menuGrpName);
		strcpy(remark,"读取菜单组的菜单项清单出错,UnionReadMenuDefRecFld");
		ret = lenOfMenuItemList;
		goto errExit;
	}
	
	// 拼分二级菜单组
	if ((itemGrpNum = UnionSeprateVarStrIntoVarGrp(menuItemsList,lenOfMenuItemList,',',itemGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuGrp::UnionSeprateVarStrIntoVarGrp [%s] of [%s][%s]!\n",menuItemsList,mainMenuName,menuGrpName);
		return(itemGrpNum);
	}
		
	for (index = 0; index < itemGrpNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuItem(mainMenuName,menuGrpName,itemGrp[index])) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuItem::UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s][%s]!\n",
					mainMenuName,menuGrpName,itemGrp[index]);
			strcpy(remark,"生成指定菜单项出错,UnionGenerateAllInterfaceItemsOfSpecMenuItem");
		}
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMenuGrp,"mainMenuName=%s|tableMenuType=%d|menuGrpName=%s|",mainMenuName,menuType,menuGrpName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecMenuItem(char *mainMenuName,char *menuGrpName,char *itemName)
{
	int			ret;
	TUnionMenuItemDef	menuItemRec;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagMenuItem,"mainMenuName=%s|menuGrpName=%s|menuDisplayname=%s|",mainMenuName,menuGrpName,itemName)) > 0)
		return(0);

	//UnionRealNullLog("in UnionGenerateAllInterfaceItemsOfSpecMenuItem:: deal [%s][%s][%s]...\n",mainMenuName,menuGrpName,itemName);

	// 读出菜单项定义
	memset(&menuItemRec,0,sizeof(menuItemRec));
	if ((ret = UnionReadMenuItemDefRec(mainMenuName,menuGrpName,itemName,&menuItemRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: itemName [%s][%s][%s]!\n",mainMenuName,menuGrpName,itemName);
		strcpy(remark,"读取菜单项记录出错,UnionReadMenuItemDefRec");
		goto errExit;
	}
	
	// 生成操作
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecOperation(mainMenuName,menuItemRec.viewName,menuItemRec.menuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView::UnionGenerateAllInterfaceItemsOfSpecOperation [%s] of [%s][%s]\n",
					menuItemRec.menuName,mainMenuName,menuItemRec.viewName);
		strcpy(remark,"生成操作出错,UnionGenerateAllInterfaceItemsOfSpecOperation");
	}

	// 生成视图
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecView(mainMenuName,menuItemRec.viewName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuItem::UnionGenerateAllInterfaceItemsOfSpecView [%s] of [%s][%s][%s]!\n",
				menuItemRec.viewName,mainMenuName,menuGrpName,itemName);
		strcpy(remark,"生成视图出错,UnionGenerateAllInterfaceItemsOfSpecView");
		goto errExit;
	}
errExit:	
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMenuItem,"mainMenuName=%s|menuGrpName=%s|menuDisplayname=%s|",mainMenuName,menuGrpName,itemName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecOperation(char *mainMenuName,char *viewName,char *operationName)
{
	int				ret;
	int				resID;
	TUnionMenuItemOperationDef	operationRec;
	char				remark[256+1];
	char				*ptr;
	char				*ptr2;
	
	memset(remark,0,sizeof(remark));
		
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagMenuOperation,"mainMenuName=%s|menuName=%s|",mainMenuName,operationName)) > 0)
		return(0);

	// 读取视图的资源ID
	if ((ret = UnionReadViewListRecIntTypeFld(viewName,mainMenuName,conViewListFldNameResID,&resID)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionReadViewListRecIntTypeFld [%s][%s]!\n",viewName,mainMenuName);
		strcpy(remark,"读取视图记录的resID域出错,UnionReadViewListRecIntTypeFld");
		goto errExit;
	}
		
	// 读出菜单操作定义
	memset(&operationRec,0,sizeof(operationRec));
	if ((ret = UnionReadMenuItemOperationDefRec(operationName,&operationRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionReadMenuItemOperationDefRec [%s]!\n",operationName);
		strcpy(remark,"读取操作定义记录出错,UnionReadMenuItemOperationDefRec");
		goto errExit;
	}
	
	// 生成操作授权
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecOperationAuth(mainMenuName,resID,operationRec.serviceID)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation::UnionGenerateAllInterfaceItemsOfSpecOperationAuth [%d][%d] of [%s]\n",
					resID,operationRec.serviceID,operationName);
		strcpy(remark,"生成操作授权出错,UnionGenerateAllInterfaceItemsOfSpecOperationAuth");
	}

	// 生成操作授权
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecOperationTagDef(mainMenuName,operationRec.serviceName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation::UnionGenerateAllInterfaceItemsOfSpecOperationTagDef [%s]\n",operationRec.serviceName);
		strcpy(remark,"生成操作标识,UnionGenerateAllInterfaceItemsOfSpecOperationTagDef");
	}

	// 判断是否是批操作
	if (strncmp(operationRec.conditionsMenu,"batchMenu=",10) == 0)
	{
		// 是批操作
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeBatchMenuGrp,operationRec.conditionsMenu+10)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s] of [%s][%s]!\n",
					operationName,mainMenuName,operationRec.conditionsMenu+10);
			strcpy(remark,"生成批操作出错,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
		}
	}
	else if ((ptr = strstr(operationRec.conditionsMenu,"appendfld=append.")) != NULL)
	{
		// 是后续视图操作
		if ((ptr2 = strstr(ptr,",")) != NULL)
			*ptr2 = 0;
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecView(mainMenuName,ptr+strlen("appendfld=append."))) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionGenerateAllInterfaceItemsOfSpecView [%s] of [%s][%s]!\n",
					operationName,mainMenuName,ptr+strlen("appendfld=append."));
			strcpy(remark,"生成视图出错,UnionGenerateAllInterfaceItemsOfSpecView");
		}
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMenuOperation,"mainMenuName=%s|menuName=%s|",mainMenuName,operationName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecView(char *mainMenuName,char *viewName)
{
	int				ret;
	TUnionViewList			viewRec;
	char				remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagView,"mainMenuName=%s|ID=%s|",mainMenuName,viewName)) > 0)
		return(0);

	// 读出视图定义
	memset(&viewRec,0,sizeof(viewRec));
	if ((ret = UnionReadViewListRec(viewName,mainMenuName,&viewRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: UnionReadViewListRec [%s][%s]!\n",viewName,mainMenuName);
		strcpy(remark,"读取视图记录出错,UnionReadViewListRec");
		goto errExit;
	}

	// 生成界面右键菜单
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeFormPopMenu,viewRec.formPopMenuDefFileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
				mainMenuName,viewRec.formPopMenuDefFileName);
		strcpy(remark,"生成界面右键菜单出错,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
	}

	// 生成记录右键菜单
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeRecPopMenu,viewRec.recPopMenuDefFileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
				mainMenuName,viewRec.recPopMenuDefFileName);
		strcpy(remark,"生成记录右键菜单出错,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
	}

	// 生成双击界面菜单
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeDoubleClickMenu,viewRec.recDoubleClickMenuFileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
				mainMenuName,viewRec.recDoubleClickMenuFileName);
		strcpy(remark,"生成双击记录菜单出错,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
	}

	// 生成域清单
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableFieldList(mainMenuName,viewName,viewRec.fieldList)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecTableFieldList [%s] of [%s]\n",
					mainMenuName,viewName);
		strcpy(remark,"生成视图域清单出错,UnionGenerateAllInterfaceItemsOfSpecTableFieldList");
	}
	// 生成表
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTable(mainMenuName,viewRec.tableName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecTable [%s] of [%s]\n",
					viewRec.tableName,mainMenuName);
		strcpy(remark,"生成表定义出错,UnionGenerateAllInterfaceItemsOfSpecTable");
	}
errExit:	
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagView,"mainMenuName=%s|ID=%s|",mainMenuName,viewName));
}

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
int UnionIsTableUsedByMainMenu(char *mainMenuName,char *tableName)
{
	return(UnionIsInterfaceItemExists(conInterfaceItemTagTable,"mainMenuName=%s|tableName=%s|",mainMenuName,tableName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecTable(char *mainMenuName,char *tableName)
{
	int				ret;
	char				fieldList[8192+1];
	char				thisTableName[80+1];
	char				remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	memset(thisTableName,0,sizeof(thisTableName));
	if (UnionIsDigitStr(tableName))
	{
		if ((ret =  UnionReadTableListRecFldByResID(atoi(tableName),conTableListFldNameTableName,thisTableName,
			sizeof(thisTableName))) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableFieldList:: UnionReadTableListRecFldByResID [%s]!\n",tableName);
			strcpy(thisTableName,tableName);
			strcpy(remark,"读取表的名称出错,UnionReadTableListRecFld");
			goto errExit;
		}
	}
	else
		strcpy(thisTableName,tableName);

	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagTable,"mainMenuName=%s|tableName=%s|",mainMenuName,thisTableName)) > 0)
		return(0);

	// 读出表定义
	memset(fieldList,0,sizeof(fieldList));
	if (UnionIsDigitStr(tableName))
		ret =  UnionReadTableListRecFldByResID(atoi(tableName),conTableListFldNameFieldList,fieldList,sizeof(fieldList));
	else
		ret = UnionReadTableListRecFld(tableName,conTableListFldNameFieldList,fieldList,sizeof(fieldList));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: UnionReadTableListRecFld [%s][%s]!\n",thisTableName,mainMenuName);
		strcpy(remark,"读取表的域清单出错,UnionReadTableListRecFld");
		goto errExit;
	}

	// 生成域清单
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableFieldList(mainMenuName,thisTableName,fieldList)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTable:: UnionGenerateAllInterfaceItemsOfSpecTableFieldList [%s] of [%s]\n",
					mainMenuName,thisTableName);
		strcpy(remark,"生成表的域清单出错,UnionGenerateAllInterfaceItemsOfSpecTableFieldList");
	}
	
errExit:	
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagTable,"mainMenuName=%s|tableName=%s|",mainMenuName,thisTableName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecTableFieldList(char *mainMenuName,char *tableName,char *fieldList)
{
	int			ret;
	int			index;
	char			fldGrp[128][128];
	int			fldNum;
	int			failureItems = 0;
	// 拼分域清单
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fldGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionSeprateVarStrIntoVarGrp [%s] of [%s][%s]!\n",
			fieldList,mainMenuName,tableName);
		return(fldNum);
	}
		
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableField(mainMenuName,fldGrp[index])) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionGenerateAllInterfaceItemsOfSpecTableField [%s] of [%s]!\n",
					fldGrp[index],mainMenuName);
			failureItems++;
		}
	}
	if (failureItems > 0)
		return(errCodeObjectMDL_FieldListDefError);
	else
		return(0);
}

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
int UnionGenerateAllInterfaceItemsOfSpecTableField(char *mainMenuName,char *fieldName)
{
	int			ret;
	TUnionTableField	fldRec;
	char			tmpBuf[128+1];
	int			methodType;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagTableField,"mainMenuName=%s|ID=%s|",mainMenuName,fieldName)) > 0)
		return(0);

	// 读出域定义
	memset(&fldRec,0,sizeof(fldRec));
	strcpy(fldRec.ID,fieldName);
	if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField:: UnionReadTableFieldRec [%s][%s]!\n",fieldName,mainMenuName);
		strcpy(remark,"读取域定义记录出错,UnionReadTableFieldRec");
		goto errExit;
	}
		
	// 从域赋值方法中读取表名称
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((methodType = UnionReadAttrFromFieldMethodDef(fldRec.value,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField:: UnionReadAttrFromFieldMethodDef [%s] of [%s][%s]!\n",
				fldRec.value,fieldName,mainMenuName);
		strcpy(remark,"分析域赋值方法出错,UnionReadAttrFromFieldMethodDef");
		goto errExit;
	}
	switch (methodType)
	{
		case	conTblFldValueMethodUseTable:
			// 表
			if ((ret = UnionGenerateAllInterfaceItemsOfSpecTable(mainMenuName,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField::UnionGenerateAllInterfaceItemsOfSpecTable [%s] of [%s][%s]!\n",
					tmpBuf,mainMenuName,fieldName);
				strcpy(remark,"生成表定义出错,UnionGenerateAllInterfaceItemsOfSpecTable");
			}
			break;
		case	conTblFldValueMethodUseEnum:
			// 枚举
			if ((ret = UnionGenerateAllInterfaceItemsOfSpecEnum(mainMenuName,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField::UnionGenerateAllInterfaceItemsOfSpecEnum [%s] of [%s][%s]!\n",
					tmpBuf,mainMenuName,fieldName);
				strcpy(remark,"生成枚举出错,UnionGenerateAllInterfaceItemsOfSpecEnum");
			}
			break;
		case	conTblFldValueMethodUseComplexFld:
			// 复合域
			if ((ret = UnionGenerateAllInterfaceItemsOfSpecComplexFld(mainMenuName,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField::UnionGenerateAllInterfaceItemsOfSpecComplexFld [%s] of [%s][%s]!\n",
					tmpBuf,mainMenuName,fieldName);
				strcpy(remark,"生成复合域出错,UnionGenerateAllInterfaceItemsOfSpecComplexFld");
			}
			break;
		default:
			break;
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagTableField,"mainMenuName=%s|ID=%s|",mainMenuName,fieldName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecEnum(char *mainMenuName,char *enumName)
{
	int	ret;
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagEnum,"mainMenuName=%s|enumFileName=%s|",mainMenuName,enumName)) > 0)
		return(0);
	return(UnionAddInterfaceItemToItemGrp(NULL,conInterfaceItemTagEnum,"mainMenuName=%s|enumFileName=%s|",mainMenuName,enumName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecComplexFld(char *mainMenuName,char *complexFldName)
{
	int	ret;
	char	fldList[1024+1];
	int	lenOfFldList;
	char	remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagComplexField,"mainMenuName=%s|complexFieldID=%s|",mainMenuName,complexFldName)) > 0)
		return(0);
	// 登记复合域定义表
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTable(mainMenuName,"complexField")) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecComplexFld:: UnionGenerateAllInterfaceItemsOfSpecTable [%s]!\n",mainMenuName);
		strcpy(remark,"生成复合域定义表出错UnionGenerateAllInterfaceItemsOfSpecTable");
	}
	// 读出复合域的域定义清单
	memset(fldList,0,sizeof(fldList));
	if ((lenOfFldList = UnionReadComplexFieldRecFld(mainMenuName,complexFldName,conComplexFieldFldNameTableFldList,fldList,sizeof(fldList))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecComplexFld:: UnionReadComplexFieldRecFld [%s]!\n",mainMenuName);
		strcpy(remark,"读取复合域记录出错,UnionReadComplexFieldRecFld");
		goto errExit;
	}
	// 生成域清单
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableFieldList(mainMenuName,complexFldName,fldList)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecComplexFld [%s] of [%s]\n",
					mainMenuName,complexFldName);
		strcpy(remark,"生成复合域清单出错,UnionGenerateAllInterfaceItemsOfSpecTableFieldList");
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagComplexField,"mainMenuName=%s|complexFieldID=%s|",mainMenuName,complexFldName));
}

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
int UnionGenerateAllInterfaceItemsOfSpecOperationAuth(char *mainMenuName,int resID,int operationID)
{
	int	ret;
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagOperationAuth,"mainMenuName=%s|resID=%d|serviceID=%d|",mainMenuName,resID,operationID)) > 0)
		return(0);
	return(UnionAddInterfaceItemToItemGrp(NULL,conInterfaceItemTagOperationAuth,"mainMenuName=%s|resID=%d|serviceID=%d|",mainMenuName,resID,operationID));
}

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
int UnionGenerateAllInterfaceItemsOfSpecOperationTagDef(char *mainMenuName,char *serviceName)
{
	int	ret;
	
	// 判断界面项当前是否正在处理
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagOperationTagDef,"mainMenuName=%s|ID=%s|",mainMenuName,serviceName)) > 0)
		return(0);
	return(UnionAddInterfaceItemToItemGrp(NULL,conInterfaceItemTagOperationTagDef,"mainMenuName=%s|ID=%s|",mainMenuName,serviceName));
}

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
int UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile(char *fileName)
{
	FILE	*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile:: fopen [%s]!\n",fileName);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionDeleteAllUnusedInterfaceItems(fp);
	fclose(fp);
	return(0);
}

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
int UnionDeleteAllUnusedInterfaceItems(FILE *fp)
{
	int		ret;
	TUnionMainMenu	mainMenuGrp[24];
	int		mainMenuNum;
	int		index;

	// 选出所有主菜音
	memset(&mainMenuGrp,0,sizeof(mainMenuGrp));
	if ((mainMenuNum = UnionBatchReadMainMenuRec("",mainMenuGrp,24)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItems:: mainMenuGrp!\n");
		return(mainMenuNum);
	}
	
	// 初始化界面项数组
	if ((ret = UnionInitInterfaceItemGrp("all-mainMenuGrp")) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItems:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	// 依次获得每个主菜单使用的界面项
	for (index = 0; index < mainMenuNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuGrp[index].mainMenuName)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItems::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuGrp[index].mainMenuName);
			UnionDeleteInterfaceItemGrp();
			return(ret);
		}
	}
	// 删除所有没有用到的界面项
	for (index = 0; index < mainMenuNum; index++)
	{
		UnionDeleteAllUnusedComplexFldItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedEnumDefItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedViewListItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedMenuItemItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedMenuGrpItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedMainMenuItems(mainMenuGrp[index].mainMenuName,fp);
	}
	UnionDeleteAllUnusedTableFieldItems("",fp);
	UnionDeleteAllUnusedOperationAuthorizationItems("",fp);
	UnionDeleteAllUnusedMenuOperationItems("",fp);
	UnionDeleteAllUnusedOperationListItems("",fp);
	UnionDeleteAllUnusedTableListItems("",fp);
	
	// 删除界面项数组
	UnionDeleteInterfaceItemGrp();
	return(ret);
}

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
int UnionDeleteAllUnusedComplexFldItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	complexFieldID[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameComplexField,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedComplexFldItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedComplexFldItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(complexFieldID,0,sizeof(complexFieldID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conComplexFieldFldNameComplexFieldID,complexFieldID,sizeof(complexFieldID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedComplexFldItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conComplexFieldFldNameComplexFieldID,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemExists(conTBLNameComplexField,"mainMenuName=%s|complexFieldID=%s|",mainMenuName,complexFieldID))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameComplexField,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedComplexFldItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameComplexField,mainMenuName,complexFieldID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}	

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
int UnionDeleteAllUnusedEnumDefItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	enumFileName[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameEnumValueDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedEnumDefItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedEnumDefItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(enumFileName,0,sizeof(enumFileName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conEnumValueDefFldNameEnumFileName,enumFileName,sizeof(enumFileName))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedEnumDefItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conEnumValueDefFldNameEnumFileName,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemExists(conTBLNameEnumValueDef,"mainMenuName=%s|enumFileName=%s|",mainMenuName,enumFileName))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameEnumValueDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedEnumDefItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameEnumValueDef,mainMenuName,enumFileName);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedViewListItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameViewList,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedViewListItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedViewListItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conViewListFldNameID,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedViewListItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conViewListFldNameID,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemExists(conTBLNameViewList,"mainMenuName=%s|ID=%s|",mainMenuName,ID))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameViewList,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedViewListItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameViewList,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedTableFieldItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameTableField,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedTableFieldItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedTableFieldItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conViewListFldNameID,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableFieldItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conViewListFldNameID,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameTableField,"|ID=%s|",ID))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameTableField,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableFieldItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameTableField,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedOperationAuthorizationItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	resID[128+1];
	char	serviceID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameOperationAuthorization,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(resID,0,sizeof(resID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperationAuthorizationFldNameResID,resID,sizeof(resID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conOperationAuthorizationFldNameResID,lenOfRecStr,recStr);
			continue;
		}
		memset(serviceID,0,sizeof(serviceID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperationAuthorizationFldNameServiceID,serviceID,sizeof(serviceID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conOperationAuthorizationFldNameServiceID,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameOperationAuthorization,"|resID=%s|serviceID=%s|",resID,serviceID))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameOperationAuthorization,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s][%s] ok!\n",conTBLNameOperationAuthorization,mainMenuName,resID,serviceID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedMenuItemItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	menuGrpName[128+1];
	char	menuDisplayname[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMenuItemDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMenuItemItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(menuGrpName,0,sizeof(menuGrpName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuItemDefFldNameMenuGrpName,menuGrpName,sizeof(menuGrpName))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuItemDefFldNameMenuGrpName,lenOfRecStr,recStr);
			continue;
		}
		memset(menuDisplayname,0,sizeof(menuDisplayname));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuItemDefFldNameMenuDisplayname,menuDisplayname,sizeof(menuDisplayname))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuItemDefFldNameMenuDisplayname,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemExists(conTBLNameMenuItemDef,"mainMenuName=%s|%s=%s|%s=%s|",
				mainMenuName,conMenuItemDefFldNameMenuGrpName,menuGrpName,conMenuItemDefFldNameMenuDisplayname,menuDisplayname))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMenuItemDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s][%s] ok!\n",conTBLNameMenuItemDef,mainMenuName,menuGrpName,menuDisplayname);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedMenuGrpItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	menuGrpName[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMenuDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMenuGrpItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMenuGrpItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(menuGrpName,0,sizeof(menuGrpName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuDefFldNameMenuGrpName,menuGrpName,sizeof(menuGrpName))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuGrpItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuDefFldNameMenuGrpName,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemExists(conTBLNameMenuDef,"mainMenuName=%s|%s=%s|",
				mainMenuName,conMenuDefFldNameMenuGrpName,menuGrpName))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMenuDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuGrpItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameMenuDef,mainMenuName,menuGrpName);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedMainMenuItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMainMenu,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMainMenuItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMainMenuItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		// 判断项是否存在
		if (UnionIsInterfaceItemExists(conTBLNameMainMenu,"mainMenuName=%s|",mainMenuName))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMainMenu,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMainMenuItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s] ok!\n",conTBLNameMainMenu,mainMenuName);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedMenuOperationItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMenuItemOperationDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMenuOperationItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMenuOperationItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuItemOperationDefFldNameMenuName,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuOperationItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuItemOperationDefFldNameMenuName,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameMenuItemOperationDef,"|%s=%s|",conMenuItemOperationDefFldNameMenuName,ID))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMenuItemOperationDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuOperationItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameMenuItemOperationDef,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedOperationListItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameOperationList,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedOperationListItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedOperationListItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperationListFldNameID,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationListItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conOperationListFldNameID,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameOperationList,"|%s=%s|",conOperationListFldNameID,ID))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameOperationList,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationListItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameOperationList,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

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
int UnionDeleteAllUnusedTableListItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 选出所有记录
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameTableList,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedTableListItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedTableListItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conTableListFldNameTableName,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableListItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conTableListFldNameTableName,lenOfRecStr,recStr);
			continue;
		}
		// 判断项是否存在
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameTableList,"|%s=%s|",conTableListFldNameTableName,ID))
			continue;
		// 不存在
		// 删除
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameTableList,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableListItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameTableList,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}
