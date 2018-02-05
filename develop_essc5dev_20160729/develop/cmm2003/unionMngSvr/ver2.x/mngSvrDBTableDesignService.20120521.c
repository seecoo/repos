// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionResID.h"
//#include "unionTableList.h"
//#include "unionOperationList.h"
#include "unionGenIncFromTBL.h"
#include "unionGenTBLDefFromTBL.h"
#include "mngSvrCommProtocol.h"
#include "unionMngSvrOperationType.h"
#include "unionGenSQLFromTBLDefList.h"
#include "unionMenuItemType.h"
#include "UnionLog.h"

char UnionGetTellerLevel();
char *UnionGetCurrentOperationTellerNo();

int UnionExcuteDBTableDesignServiceOnSpecTBLName(char *tellerTypeID,char *tableName,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	tmpBuf[1024+1];
	int	ret;
	char	mainMenuName[128+1];
	
	memset(mainMenuName,0,sizeof(mainMenuName));
	switch (serviceID)
	{
		case	conResCmdGenerateEnumDef:	// 生成枚举
		case	conResCmdGenerateTBLQueryInterface:	// 生成表的视图定义文件
		case	conResCmdCreateDefaultViewOfTBL:	// 创建表缺省的视图
			if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"mainMenuName",mainMenuName,sizeof(mainMenuName))) <= 0)
				strcpy(mainMenuName,"科友密钥管理系统");
			break;
	}

	*fileRecved = 0;
	switch (serviceID)
	{
		case	conResCmdGenerateTBLQueryInterface:	// 生成表的视图定义文件
			//if ((ret = UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile(tableName,UnionGenerateMngSvrTempFile())) < 0)
			if ((ret = UnionGenerateTBLQueryConfFromViewDefToSpecFile(tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)			
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	if ((ret = UnionCopyFile(tmpBuf,UnionGenerateMngSvrTempFile())) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCopyFile [%s] to [%s]!\n",tmpBuf,UnionGenerateMngSvrTempFile());
		return(ret);
	}
	strcpy(resStr,tmpBuf);
	*fileRecved = 1;
	return(strlen(resStr));
}

int UnionExcuteDBTableDesignServiceOnSpecViewName(char *tellerTypeID,char *tableName,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	tmpBuf[1024+1];
	int	ret;
	char	mainMenuName[128+1];
	int	generateDoc = 0;
	
	*fileRecved = 0;
	memset(mainMenuName,0,sizeof(mainMenuName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"mainMenuName",mainMenuName,sizeof(mainMenuName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionReadRecFldFromRecStr [%s]!\n","mainMenuName");
		return(ret);
	}
	UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"generateDoc",&generateDoc);
	//UnionLog("generateDoc = [%d]\n",generateDoc);
	if (!generateDoc)
		goto generateNonDocNow;
	switch (serviceID)
	{
		case	conResCmdGenerateTBLQueryInterface:	// 生成表的视图定义文件
			if ((ret = UnionGenerateViewAuthorizationToSpecFile(mainMenuName,tableName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateViewAuthorizationToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateFormPopupMenu:	// 生成界面右键菜单
			if ((ret = UnionGenerateFormPopMenuDefDocFromViewDefToSpecFile(tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateFormPopMenuDefDocFromViewDefToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateRecPopupMenu:	// 生成记录右键菜单
			if ((ret = UnionGenerateRecPopMenuDefDocFromViewDefToSpecFile(tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateRecPopMenuDefDocFromViewDefToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateRecDoubleClickMenu:	// 生成记录双击菜单
			if ((ret = UnionGenerateRecDoubleClickMenuDefDocFromViewDefToSpecFile(tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateRecDoubleClickMenuDefDocFromViewDefToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGetViewAuthorization:	// 读取视图的操作授权
			if ((ret = UnionGenerateViewAuthorizationToSpecFile(mainMenuName,tableName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateViewAuthorizationToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		default:
			return(errCodeEsscMDL_InvalidService);
	}
generateNonDocNow:
	UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: errCodeFunctionNotSupported\n");
	return(errCodeFunctionNotSupported);
	/*
	if ((ret = UnionCopyFile(tmpBuf,UnionGenerateMngSvrTempFile())) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionCopyFile [%s] to [%s]!\n",tmpBuf,UnionGenerateMngSvrTempFile());
		return(ret);
	}
	strcpy(resStr,tmpBuf);
	*fileRecved = 1;
	return(strlen(resStr));
	*/
}

int UnionExcuteDBTableDesignServiceOnTableList(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	tableName[128+1];
	int	ret;
	
	// 读表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"tableName",tableName,sizeof(tableName))) < 0)
	{
		//UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionReadRecFldFromRecStr [tableName] from [%s] error!\n",reqStr);
		UnionAuditLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionReadRecFldFromRecStr [tableName] from [%s] error!\n",reqStr);
		//return(ret);
	}
	return(UnionExcuteDBTableDesignServiceOnSpecTBLName(tellerTypeID,tableName,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
}

int UnionExcuteDBTableDesignServiceOnViewList(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	tableName[128+1];
	int	ret;
	
	// 读表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"ID",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionReadRecFldFromRecStr [ID] from [%s] error!\n",reqStr);
		return(ret);
	}
	return(UnionExcuteDBTableDesignServiceOnSpecViewName(tellerTypeID,tableName,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
}

int UnionExcuteDBTableDesignServiceOnMainMenu(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	mainMenuName[128+1];
	char	tmpBuf[1024+1];
	int	ret;
	int	generateDoc = 0;
	
	// 读菜单组名
	memset(mainMenuName,0,sizeof(mainMenuName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"mainMenuName",mainMenuName,sizeof(mainMenuName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionReadRecFldFromRecStr [mainMenuName] from [%s] error!\n",reqStr);
		return(ret);
	}
	*fileRecved = 0;

	switch (serviceID)
	{
		case	conResCmdGenerateMainMenu:		// 生成主菜单
			if ((ret = UnionCreateMainMenuByMainMenuName(tellerTypeID,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionCreateMainMenuByMainMenuName [%s]!\n",mainMenuName);
				return(ret);
			}
			sprintf(resStr,"mainMenuName=%s|",mainMenuName);
			*fileRecved = 1;
			break;
		default:
			UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: errCodeEsscMDL_InvalidService !\n");
			return(errCodeEsscMDL_InvalidService);
	}
	return(strlen(resStr));

}

int UnionExcuteDBTableDesignServiceOnMenuDef(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	mainMenuName[128+1];
	char	menuGrpName[128+1];
	char	tmpBuf[1024+1];
	int	ret;
	int	generateDoc = 0;
	
	// 读菜单组名
	memset(mainMenuName,0,sizeof(mainMenuName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"mainMenuName",mainMenuName,sizeof(mainMenuName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMenuDef:: UnionReadRecFldFromRecStr [mainMenuName] from [%s] error!\n",reqStr);
		return(ret);
	}
	memset(menuGrpName,0,sizeof(menuGrpName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"menuGrpName",menuGrpName,sizeof(menuGrpName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMenuDef:: UnionReadRecFldFromRecStr [menuGrpName] from [%s] error!\n",reqStr);
		return(ret);
	}
	*fileRecved = 0;
	switch (serviceID)
	{
		case	conResCmdGenerateSpec2LevelMenu:	// 生成指定二级菜单
			if ((ret = UnionCreate2LevelMenuByMenuGrpName(tellerTypeID,mainMenuName,menuGrpName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMenuDef:: UnionCreate2LevelMenuByMenuGrpName [%s]!\n",mainMenuName,menuGrpName);
				return(ret);
			}
			sprintf(resStr,"menuGrpName=%s|",mainMenuName);
			*fileRecved = 1;
			break;
		default:
			UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMenuDef:: errCodeEsscMDL_InvalidService !\n");
			return(errCodeEsscMDL_InvalidService);
	}
	return(strlen(resStr));

}

int UnionOperationOfOperatorGetAuthority(char *tellerNo,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	mainMenuName[128+1];
	char	menuGrpName[128+1];
	char	viewName[128+1];
	int	ret;
	char	tellerTypeID[40+1];
	
	switch (serviceID)
	{
		case	conResCmdReadAllMainMenuDef:	// 读取所有主菜单
			*fileRecved = 1;
			return(UnionGenerateAllMainMenuDefToSpecFile(UnionGenerateMngSvrTempFile()));
		default:
			return(errCodeEsscMDL_InvalidService);
			break;
	}
	

	/*
	switch (serviceID)
	{
		case	conResCmdReadMainMenuItem:	// 读取主菜单的菜单项
		case	conResCmdReadSecondaryMenuItem:	// 读取二级菜单的菜单项
		case	conResCmdReadInterfacePopupMenuItem:	// 读取界面右键菜单项
		case	conResCmdReadRecordPopupMenuItem:	// 读取记录右键菜单项
		case	conResCmdReadRecordDoubleClickMenuItem:	// 读取记录双击菜单项
			break;
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	// 读主菜单名
	memset(mainMenuName,0,sizeof(mainMenuName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"mainMenuName",mainMenuName,sizeof(mainMenuName))) < 0)
	{
		UnionUserErrLog("in UnionOperationOfOperatorGetAuthority:: UnionReadRecFldFromRecStr [mainMenuName] from [%s] error!\n",reqStr);
		return(ret);
	}
	*fileRecved = 0;
	memset(tellerTypeID,0,sizeof(tellerTypeID));
	if ((ret = UnionGetTellerLevelTypeID(tellerNo,tellerTypeID)) < 0)
	{
		UnionUserErrLog("in UnionOperationOfOperatorGetAuthority:: UnionGetTellerLevelTypeID [%s] error!\n",tellerNo);
		return(ret);
	}

	switch (serviceID)
	{
		case	conResCmdReadMainMenuItem:	// 读取主菜单的菜单项
			return(UnionReadMenuItemListOfSpecMainMenu(tellerTypeID,mainMenuName,resStr,sizeOfResStr));
		default:
			break;
	}
	// 读视图菜单
	switch (serviceID)
	{
		case	conResCmdReadInterfacePopupMenuItem:	// 读取界面右键菜单项
		case	conResCmdReadRecordPopupMenuItem:	// 读取记录右键菜单项
		case	conResCmdReadRecordDoubleClickMenuItem:	// 读取记录双击菜单项
			memset(viewName,0,sizeof(viewName));
			if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"viewName",viewName,sizeof(viewName))) < 0)
			{
				UnionUserErrLog("in UnionOperationOfOperatorGetAuthority:: UnionReadRecFldFromRecStr [%s] error!\n","viewName");
				return(ret);
			}
			break;
	}
	switch (serviceID)
	{
		case	conResCmdReadInterfacePopupMenuItem:	// 读取界面右键菜单项
			return(UnionReadMenuItemListOfSpecTable(tellerTypeID,viewName,mainMenuName,conMenuItemTypeFormPopMenu,resStr,sizeOfResStr));
		case	conResCmdReadRecordPopupMenuItem:	// 读取记录右键菜单项
			return(UnionReadMenuItemListOfSpecTable(tellerTypeID,viewName,mainMenuName,conMenuItemTypeRecPopMenu,resStr,sizeOfResStr));
		case	conResCmdReadRecordDoubleClickMenuItem:	// 读取记录双击菜单项
			return(UnionReadMenuItemListOfSpecTable(tellerTypeID,viewName,mainMenuName,conMenuItemTypeDoubleClickMenu,resStr,sizeOfResStr));
		default:
			break;
	}
	// 读菜单组名
	memset(menuGrpName,0,sizeof(menuGrpName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"menuGrpName",menuGrpName,sizeof(menuGrpName))) < 0)
	{
		UnionUserErrLog("in UnionOperationOfOperatorGetAuthority:: UnionReadRecFldFromRecStr [menuGrpName] from [%s] error!\n",reqStr);
		return(ret);
	}
	switch (serviceID)
	{
		case	conResCmdReadSecondaryMenuItem:	// 读取二级菜单的菜单项
			return(UnionReadMenuItemListOfSpecSecondaryMenu(tellerTypeID,mainMenuName,menuGrpName,resStr,sizeOfResStr));
		//case	conResCmdReadMenuItemCmd:	// 读取菜单项的执行命令
		//	return(UnionReadMenuItemListOfSpecMainMenu(tellerTypeID,menuName,resStr,sizeOfResStr));
		//case	conResCmdReadPopupMenuItem:	// 读取右键菜单项的执行命令
		//	return(UnionReadMenuItemListOfSpecMainMenu(tellerTypeID,menuName,resStr,sizeOfResStr));
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	*/
}

int UnionExcuteDBTableDesignServiceOnEnumValueDef(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	mainMenuName[128+1];
	char	enumFileName[128+1];
	char	tmpBuf[1024+1];
	int	ret;
	
	// 读菜单组名
	memset(mainMenuName,0,sizeof(mainMenuName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"mainMenuName",mainMenuName,sizeof(mainMenuName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnEnumValueDef:: UnionReadRecFldFromRecStr [mainMenuName] from [%s] error!\n",reqStr);
		return(ret);
	}
	memset(enumFileName,0,sizeof(enumFileName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"enumFileName",enumFileName,sizeof(enumFileName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnEnumValueDef:: UnionReadRecFldFromRecStr [enumFileName] from [%s] error!\n",reqStr);
		return(ret);
	}
	*fileRecved = 0;
	switch (serviceID)
	{
		case	conResCmdDownloadEnumDef:	// 生成指定二级菜单
			if ((ret = UnionGenerateEnumValueDefFile(tellerTypeID,mainMenuName,enumFileName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnEnumValueDef:: UnionGenerateEnumValueDefFile [%s]!\n",mainMenuName,enumFileName);
				return(ret);
			}
			sprintf(resStr,"enumFileName=%s|",enumFileName);
			*fileRecved = 1;
			break;
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	return(strlen(resStr));
}

/*
int UnionExcuteDBTableDesignServiceOnSecurityServiceDef(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret = 0;
	int iServiceID = 0;

	// 读取服务代码
	if( (ret = UnionReadIntTypeRecFldFromRecStr(reqStr, lenOfReqStr, "serviceID", &iServiceID)) < 0 )
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSecurityServiceDef:: UnionReadIntTypeRecFldFromRecStr [serviceID] from [%s] error!\n", reqStr);
		return(ret);
	}

	*fileRecved = 0;
	switch (serviceID)
	{
		case	conResSpecCmdOfSecuServiceDef_GenerateServiceDef:	// 生成服务报文域定义文件
			if( (ret = UnionGenerateSecuServiceDefDocToSpecFile(iServiceID, UnionGenerateMngSvrTempFile())) < 0 )
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSecurityServiceDef:: UnionGenerateSecuServiceDefFile [%d]!\n", iServiceID);
				return(ret);
			}
			sprintf(resStr,"serviceID=%d|", iServiceID);
			*fileRecved = 1;
			break;
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	return(strlen(resStr));
}
*/

int UnionExcuteDBTableDesignServiceOnComplexField(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret = 0;
	char	mainMenuName[128+1];
	char	fldID[100+1];
	
	memset(mainMenuName,0,sizeof(mainMenuName));
	// 读取主菜单名称
	if( (ret = UnionReadRecFldFromRecStr(reqStr, lenOfReqStr, "mainMenuName", mainMenuName,sizeof(mainMenuName))) < 0 )
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnComplexField:: UnionReadRecFldFromRecStr [mainMenuName] from [%s] error!\n", reqStr);
		return(ret);
	}
	memset(fldID,0,sizeof(fldID));
	// 读取域名称
	if( (ret = UnionReadRecFldFromRecStr(reqStr, lenOfReqStr, "fldID", fldID,sizeof(fldID))) < 0 )
	{
		UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnComplexField:: UnionReadRecFldFromRecStr [fldID] from [%s] error!\n", reqStr);
		return(ret);
	}
	*fileRecved = 0;
	switch (serviceID)
	{
		case	conResCmdSpec001:	// 下载域赋值方法
			if( (ret = UnionGenerateComplexFldAssignmentMethodToSpecFile(mainMenuName,fldID, UnionGenerateMngSvrTempFile())) < 0 )
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnComplexField:: UnionGenerateComplexFldAssignmentMethodToSpecFile mainMenuName = [%s] fldID = [%s]\n",mainMenuName,fldID);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	return(strlen(resStr));
}

int UnionExcuteDBTableDesignService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	tellerTypeID[40+1];
	
	UnionLog("in UnionExcuteDBTableDesignService:: [%03d][%03d]\n",resID,serviceID);
	if ((ret = UnionGetMngSvrOperationType(resID,serviceID)) != conMngSvrOperationTypeReservedCmd)
	{
		UnionAuditLog("in UnionExcuteDBTableDesignService:: [%03d] [%03d] not reserved command!\n",resID,serviceID);
		return(errCodeEsscMDL_InvalidService);
	}
	memset(tellerTypeID,0,sizeof(tellerTypeID));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"tellerTypeID",tellerTypeID,sizeof(tellerTypeID))) <= 0)
	{
		if ((ret = UnionGetTellerLevelTypeID(UnionGetCurrentOperationTellerNo(),tellerTypeID)) < 0)
		{
			UnionUserErrLog("in UnionExcuteDBTableDesignService:: UnionGetTellerLevelTypeID [%s] error!\n",UnionGetCurrentOperationTellerNo());
			return(ret);
		}
	}

	if (resID == conResIDTableList)
		return(UnionExcuteDBTableDesignServiceOnTableList(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	if (resID == conResIDViewList)
		return(UnionExcuteDBTableDesignServiceOnViewList(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	else if (resID == conResIDMainMenu)
		return(UnionExcuteDBTableDesignServiceOnMainMenu(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	else if (resID == conResIDMenuDef)
		return(UnionExcuteDBTableDesignServiceOnMenuDef(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));	
	else if (resID == conResIDEnumValueDef)
		return(UnionExcuteDBTableDesignServiceOnEnumValueDef(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));	
	//else if (resID == conResIDSecurityServiceDef)
	//	return(UnionExcuteDBTableDesignServiceOnSecurityServiceDef(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	else if (resID == conResIDComplexField)
		return(UnionExcuteDBTableDesignServiceOnComplexField(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	else
	{
		//UnionAuditLog("in UnionExcuteDBTableDesignService:: invalid resID [%d]\n",resID);
		return(UnionExcuteUnionDepartmentManagementSoftwareService(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
		//return(errCodeEsscMDL_InvalidService);
	}
}

