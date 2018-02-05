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
#include "unionComplexDBDataTransfer.h"
#include "unionMenuItemType.h"
#include "UnionLog.h"

char UnionGetTellerLevel();
char *UnionGetCurrentOperationTellerNo();

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
int UnionDeleteSpecTableSpecifiedInReqStr(char *reqStr,int lenOfReqStr)
{
	int ret = 0;
	char tableName[128+1];
	
	// get name of main menu from request string(reqStr)
	memset(tableName, 0, sizeof tableName);
	if((ret = UnionReadRecFldFromRecStr(reqStr, lenOfReqStr, "tableName", tableName, sizeof tableName)) < 0 ) 
	{
		UnionUserErrLog("in UnionDeleteSpecTableSpecifiedInReqStr:: UnionReadRecFldFromRecStr fld [%tableName] error!\n");
		return(ret);
	}
	return(UnionDeleteSpecTableByName(tableName));
}

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
		case	conResCmdOutputTBLRec:	// 从表中导出记录
			sprintf(tmpBuf,"%s/%s.txt",getenv("UNIONTEMP"),tableName);
			if ((ret = UnionOutputAllRecFromSpecTBL(tableName,"",tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionOutputAllRecFromSpecTBL [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 0;
			return(0);
		case	conResCmdInputTBLRec:	// 向表中导入记录
			sprintf(tmpBuf,"%s/%s.txt",getenv("UNIONTEMP"),tableName);
			if ((ret = UnionInputAllRecIntoSpecTBL(tableName,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionInputAllRecIntoSpecTBL [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 0;
			return(0);
		case	conResCmdPrintRelationsOfSpecTBL:	// 显示指定表的关系
			if ((ret = UnionPrintRelationsForSpecTBLDefToSpecFile(tableName,2,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionPrintRelationsForSpecTBLDefToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdPrintRelationsOfAllTBL:	// 显示所有表的关系
			if ((ret = UnionPrintRelationsForAllTBLDefToSpecFile(1,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionPrintRelationsForAllTBLDefToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdPrintAllTBLInOrder:	// 按引用顺序关系显示所有表
			memset(tmpBuf,0,sizeof(tmpBuf));
			UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"prefix",tmpBuf,sizeof(tmpBuf));
			if ((ret = UnionPrintTBLNameForAllTBLDefInTableListToSpecFile(tmpBuf,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionPrintTBLNameForAllTBLDefInTableListToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdCreateDropAllTBLFile:	// 创建删除所有表的语句
			if ((ret = UnionCreateDropTBLSQLForAllTBLDefInTableListToSpecFile(UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateDropTBLSQLForAllTBLDefInTableListToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateOperationInfo:	// 生成操作提示文件
			if ((ret = UnionGenerateOperationInfoFromAuthDefTBLToSpecFile(UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateOperationInfoFromAuthDefTBLToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateCreateAllTBLSQL:	// 生成所有的建表SQL语句
			//if ((ret = UnionCreateAllObjectDefSQLFromTBLDefInTableListToSpecFile(UnionGenerateMngSvrTempFile())) < 0)
			if ((ret = UnionCreateSQLForAllTBLDefInTableListToSpecFile(UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateSQLForAllTBLDefInTableListToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateAllMenu:	// 生成所有菜单
			if ((ret = UnionCreateAllMenuByTBLType(fileRecved)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateAllMenuByTBLType [%s]!\n",tableName);
				return(ret);
			}
			return(0);
		case	conResCmdGenerateAllConfForTBL:	// 生成表需要的所有配置
			if ((ret = UnionGenerateAllMngSvrConfForTable(tellerTypeID,tableName,fileRecved)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateAllMngSvrConfForTable [%s]!\n",tableName);
				return(ret);
			}
			return(0);
		case	conResCmdInsertInitRec:	// 插入初始记录
			if ((ret = UnionInsertInitRecOfSpecTBLOnTBLDefInTableList(tableName)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionInsertInitRecOfSpecTBLOnTBLDefInTableList [%s]!\n",tableName);
				return(ret);
			}
			sprintf(resStr,"共初始化了%d个记录",ret);
			return(strlen(resStr));
		case	conResCmdGenerateTBLOperation:	// 为表生成操作允可
			if ((ret = UnionGenerateOperationFromTBLDefInTableList(tableName)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateOperationFromTBLDefInTableList [%s]!\n",tableName);
				return(ret);
			}
			sprintf(resStr,"共插入了%d个操作记录",ret);
			return(strlen(resStr));
		case	conResCmdCreateTBLAnyway:	// 重新建表
			if ((ret = UnionCreateObjectFromTBLDefInTableList(tableName)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateObjectFromTBLDefInTableList [%s]!\n",tableName);
				return(ret);
			}
			return(0);
		case	conResCmdGenerateTBLIncAndCFile:	// 生成C代码和头文件
			if ((ret = UnionGenerateIncFileAndCFileFromTBLDef(tableName,1)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateIncFileAndCFileFromTBLDef [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateEnumDef:	// 生成枚举
			//if ((ret = UnionGenerateEnumDefFromTBLToSpecFile(tableName,UnionGenerateMngSvrTempFile())) < 0)
			if ((ret = UnionGenerateEnumDefFromTBLForSpecMainMenu(tableName,mainMenuName,"")) < 0)
			{
				//UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateEnumDefFromTBLToSpecFile [%s]!\n",tableName);
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateEnumDefFromTBLForSpecMainMenu [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateTBLIncFile:	// 生成头文件
			if ((ret = UnionGenerateIncFileFromTBLDef(tableName,UnionGenerateMngSvrTempFile(),1)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateIncFileFromTBLDef [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateTBLStructDefIncFile:	// 生成表结构定义的头文件
			if ((ret = UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile(tableName,UnionGenerateMngSvrTempFile(),1)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateTBLCFile:	// 生成C代码
			if ((ret = UnionGenerateCFileFromTBLDef(tableName,UnionGenerateMngSvrTempFile(),1)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateCFileFromTBLDef [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateMngSvrTBLCreateFile:	// 生成mngSvr建表的SQL语句
			if ((ret = UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList(tableName)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList [%s]!\n",tableName);
				return(ret);
			}
			memset(tmpBuf,0,sizeof(tmpBuf));
			UnionGetObjectDefCreateFileName(tableName,tmpBuf);
			break;
		case	conResCmdGenerateTBLDef:	// 生成表定义
			if ((ret = UnionCreateObjectDefFromTBLDefInTableList(tableName,1)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateObjectDefFromTBLDefInTableList [%s]!\n",tableName);
				return(ret);
			}
			memset(tmpBuf,0,sizeof(tmpBuf));
			UnionGetObjectDefFileName(tableName,tmpBuf);
			break;
		case	conResCmdGenerateTBLCreateSQL:	// 生成建表的SQL语句
			if ((ret = UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile(tableName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateTBLQueryInterface:	// 生成表的视图定义文件
			//if ((ret = UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile(tableName,UnionGenerateMngSvrTempFile())) < 0)
			if ((ret = UnionGenerateTBLQueryConfFromViewDefToSpecFile(tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)			
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateTBLListIncFile:	// 生成表清单定义文件
			if ((ret = UnionGenerateTBLListDefFromTableListTBL(1)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateTBLListDefFromTableListTBL [%s]!\n",tableName);
				return(ret);
			}
			memset(tmpBuf,0,sizeof(tmpBuf));
			UnionGetIncFileNameOfTBLListDef(tmpBuf);
			break;
		case	conResCmdGenerateOperationDefIncFile:	// 生成操作定义文件
			if ((ret = UnionGenerateTBLCmdDefFromTBLDef()) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateTBLCmdDefFromTBLDef [%s]!\n",tableName);
				return(ret);
			}
			UnionLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateTBLCmdDefFromTBLDef OK!\n");
			memset(tmpBuf,0,sizeof(tmpBuf));
			UnionGetIncFileNameOfTBLCmdDef(tmpBuf);
			break;
		case	conResCmdCreateDefaultViewOfTBL:	// 创建表缺省的视图
			if ((ret = UnionGenerateDefaultViewOfTBL(tableName,mainMenuName,resStr,sizeOfResStr)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateTBLCmdDefFromTBLDef [%s]!\n",tableName);
				return(ret);
			}
			UnionLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionGenerateTBLCmdDefFromTBLDef OK!\n");
			return(ret);
		case	conResCmdCreateTableOnRealDB:	// 在数据库中建表
			if( (ret = UnionCreateTableByTBLDefInTableList(tableName)) < 0 )
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateTableByTBLDefInTableList [%s]!\n",tableName);
				return(ret);
			}
			UnionLog("in UnionExcuteDBTableDesignServiceOnTableList:: UnionCreateTableByTBLDefInTableList OK!\n");
			return(ret);
		case	conResCmdDelete:	// 在数据库中删除表
			return(UnionDeleteSpecTableSpecifiedInReqStr(reqStr,lenOfReqStr));
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
	switch (serviceID)
	{
		case	conResCmdGenerateTBLQueryInterface:	// 生成表的视图定义文件
			if ((ret = UnionGenerateTBLQueryConfFromViewDefToSpecFile(tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateTBLQueryConfFromViewDefToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateFormPopupMenu:	// 生成界面右键菜单
			if ((ret = UnionGenerateFormPopMenuFromViewDefToSpecFile(tellerTypeID,tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateFormPopMenuFromViewDefToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateRecPopupMenu:	// 生成记录右键菜单
			if ((ret = UnionGenerateRecPopMenuFromViewDefToSpecFile(tellerTypeID,tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateRecPopMenuFromViewDefToSpecFile [%s][%s]!\n",tableName,mainMenuName);
				return(ret);
			}
			*fileRecved = 1;
			return(0);
		case	conResCmdGenerateRecDoubleClickMenu:	// 生成记录双击菜单
			if ((ret = UnionGenerateRecDoubleClickMenuFromViewDefToSpecFile(tellerTypeID,tableName,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnSpecViewName:: UnionGenerateRecDoubleClickMenuFromViewDefToSpecFile [%s][%s]!\n",tableName,mainMenuName);
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
	UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"generateDoc",&generateDoc);
	if (!generateDoc)
		goto nonGenerateDoc;
	switch (serviceID)
	{
		case	conResCmdGenerateMainMenu:		// 生成主菜单
			if ((ret = UnionGenerateMainMenuDefDocToSpecFile(mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionGenerateMainMenuDefDocToSpecFile [%s]!\n",mainMenuName);
				return(ret);
			}
			sprintf(resStr,"mainMenuName=%s|",mainMenuName);
			*fileRecved = 1;
			break;
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	return(strlen(resStr));
nonGenerateDoc:
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
		case	conResCmdGenerateSpec2LevelMenu:		// 生成主菜单供显示用
			if ((ret = UnionGenerateMainMenuForTestToSpecFile(tellerTypeID,mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionGenerateMainMenuForTestToSpecFile [%s]!\n",mainMenuName);
				return(ret);
			}
			sprintf(resStr,"mainMenuName=%s|",mainMenuName);
			*fileRecved = 1;
			break;
		case	conResCmdSpec011:		// 读取一个主菜单使用的所有界面相关配置
			if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile(mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile [%s]!\n",mainMenuName);
				return(ret);
			}
			sprintf(resStr,"mainMenuName=%s|",mainMenuName);
			*fileRecved = 1;
			break;
		case	conResCmdSpec010:		// 读取一个主菜单使用的建表语句
			if ((ret = UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile(UnionGenerateMngSvrTempFile(),mainMenuName)) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile [%s]!\n",mainMenuName);
				return(ret);
			}
			sprintf(resStr,"mainMenuName=%s|",mainMenuName);
			*fileRecved = 1;
			break;
		case	conResCmdSpec009:		// 读取一个主菜单的所有错误界面相关配置
			if ((ret = UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile(mainMenuName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile [%s]!\n",mainMenuName);
				return(ret);
			}
			sprintf(resStr,"mainMenuName=%s|",mainMenuName);
			*fileRecved = 1;
			break;
		case	conResCmdSpec008:		// 删除所有没有用到的配置
			if ((ret = UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile(UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMainMenu:: UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile [%s]!\n");
				return(ret);
			}
			*fileRecved = 1;
			break;
		default:
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
	UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"generateDoc",&generateDoc);
	if (!generateDoc)
		goto nonGenerateDoc;
	switch (serviceID)
	{
		case	conResCmdGenerateSpec2LevelMenu:	// 生成指定二级菜单
			if ((ret = UnionGenerateMenuGrpDefDocToSpecFile(mainMenuName,menuGrpName,UnionGenerateMngSvrTempFile())) < 0)
			{
				UnionUserErrLog("in UnionExcuteDBTableDesignServiceOnMenuDef:: UnionGenerateMenuGrpDefDocToSpecFile [%s]!\n",mainMenuName,menuGrpName);
				return(ret);
			}
			sprintf(resStr,"menuGrpName=%s|",menuGrpName);
			*fileRecved = 1;
			break;
		default:
			return(errCodeEsscMDL_InvalidService);
	}
	return(strlen(resStr));

nonGenerateDoc:
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
			break;
	}

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
	else if (resID == conResIDSecurityServiceDef)
		return(UnionExcuteDBTableDesignServiceOnSecurityServiceDef(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	else if (resID == conResIDComplexField)
		return(UnionExcuteDBTableDesignServiceOnComplexField(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	else
	{
		//UnionAuditLog("in UnionExcuteDBTableDesignService:: invalid resID [%d]\n",resID);
		return(UnionExcuteUnionDepartmentManagementSoftwareService(tellerTypeID,handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
		//return(errCodeEsscMDL_InvalidService);
	}
}

