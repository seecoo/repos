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

// ���mngSvr�ͻ��������ļ���Ŀ¼
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

// ���mngSvr�ͻ���ö�����Ͷ����ļ���
char *UnionGetEnumConfFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/enumValueDef/%s.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// ���mngSvr�ͻ�����ͼ�����ļ���
char *UnionGetTBLQueryConfFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/tblQuery/%s.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// ���mngSvr�ͻ��˼�¼˫���˵��ļ���
char *UnionGetDoubleClickRecMenuFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s˫����¼.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// ���mngSvr�ͻ��˼�¼�Ҽ��˵��ļ���
char *UnionGetRecPopupMenuFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s��¼�Ҽ��˵�.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// ���mngSvr�ͻ��˽����Ҽ��˵��ļ���
char *UnionGetFormPopupMenuFileNameByTableAlais(char *tableAlais)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s�����Ҽ��˵�.conf",UnionGetMngSvrClientConfFileDir(),tableAlais);
	return(gunionMngSvrConfFileName);
}

// ���mngSvr����˱����ļ���
char *UnionGetComplexDBTBLDefFileNameByTableName(char *tableName)
{
	sprintf(gunionMngSvrConfFileName,"%s/sql/%s.sql",UnionGetMngSvrClientConfFileDir(),tableName);
	return(gunionMngSvrConfFileName);
}

// ���mngSvr��������ݿ⽨���ļ���
char *UnionGetDBTBLCreateSQLFileNameByTableName(char *tableName)
{
	sprintf(gunionMngSvrConfFileName,"%s/dbSql/%s.sql",UnionGetMngSvrClientConfFileDir(),tableName);
	return(gunionMngSvrConfFileName);
}

// ������˵�����
char *UnionGetMngSvrClientMainMenuFileName()
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/���˵�.conf",UnionGetMngSvrClientConfFileDir());
	return(gunionMngSvrConfFileName);
}

// ��ö����˵�����
char *UnionGetMngSvrClient2LevelMenuFileName(char *nameOf2LevelMenu)
{
	sprintf(gunionMngSvrConfFileName,"%s/menu/%s.conf",UnionGetMngSvrClientConfFileDir(),nameOf2LevelMenu);
	return(gunionMngSvrConfFileName);
}

