// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "mngSvrConfFileName.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionErrCode.h"

char 	gunionMngSvrConfFileName[256+1] = "";
char	gunionMngSvrConfFileDir[256+1] = "";
int	gunionMsgSvrConfFileDirSet = 0;

// 获得mngSvr客户端配置文件的目录
char *UnionGetMngSvrClientConfFileDir()
{
	char	*ptr;
	
	if (gunionMsgSvrConfFileDirSet)
		return(gunionMngSvrConfFileDir);
		
#ifndef _WIN32
	if ((ptr = getenv("UNIONOBJECTDIR")) == NULL)
		sprintf(gunionMngSvrConfFileDir,"%s",getenv("UNIONTEMP"));
	else
		strcpy(gunionMngSvrConfFileDir,ptr);
#else
	memset(gunionMngSvrConfFileDir,0,sizeof(gunionMngSvrConfFileDir));
	UnionGetMainWorkingDir(gunionMngSvrConfFileDir);
#endif
	gunionMsgSvrConfFileDirSet = 1;
	return(gunionMngSvrConfFileDir);
}

// 获得mngSvr客户端枚举类型定义文件名
char *UnionGetEnumConfFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/enumValueDef/%s.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// 获得mngSvr客户端视图定义文件名
char *UnionGetTBLQueryConfFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/tblQuery/%s.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// 获得mngSvr客户端记录双击菜单文件名
char *UnionGetDoubleClickRecMenuFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s双击记录.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// 获得mngSvr客户端记录右键菜单文件名
char *UnionGetRecPopupMenuFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s记录右键菜单.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// 获得mngSvr客户端界面右键菜单文件名
char *UnionGetFormPopupMenuFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s界面右键菜单.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// 获得mngSvr服务端表定义文件名
char *UnionGetComplexDBTBLDefFileNameByTableName(char *tableName)
{
	sprintf(gunionMngSvrConfFileName,"%s/sql/%s.sql",UnionGetMngSvrClientConfFileDir(),tableName);
	return(gunionMngSvrConfFileName);
}

// 获得mngSvr服务端数据库建表文件名
char *UnionGetDBTBLCreateSQLFileNameByTableName(char *tableName)
{
	sprintf(gunionMngSvrConfFileName,"%s/dbSql/%s.sql",UnionGetMngSvrClientConfFileDir(),tableName);
	return(gunionMngSvrConfFileName);
}

// 获得主菜单名称
char *UnionGetMngSvrClientMainMenuFileName()
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/主菜单.conf",UnionGetMngSvrClientConfFileDir());
	return(gunionMngSvrConfFileName);
}

// 获得二级菜单名称
char *UnionGetMngSvrClient2LevelMenuFileName(char *nameOf2LevelMenu)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s.conf",UnionGetMngSvrClientConfFileDir(),nameOf2LevelMenu);
	return(gunionMngSvrConfFileName);
}

