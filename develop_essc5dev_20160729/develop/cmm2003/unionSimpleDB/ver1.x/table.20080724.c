// Author:	ChenJiaMei
// Date:	2008-7-16

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "unionRecFile.h"
#include "table.h"
#include "database.h"
#include "recInfo.h"
#include "allTableInfo.h"

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

// ˵�������ļ��еĳ�������ļ����ݿ�Ĳ���

int UnionGetDirOfTable(char *tableName,char *dir)
{
        sprintf(dir,"%s/%s",UnionGetCurrentDatabaseName(),tableName);
        return(0);
}

/*
���ܣ�����һ�ű�
���������
	tableName������
	primaryKeyStr ����Ĺؼ�������������ؼ����ɶ������ɣ����������֮����'|'�ָ�
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateTable(char *tableName,char *primaryKeyStr)
{
	char	tmpBuf[512];
        char    dir[512];
	int	ret;
	
	if (tableName == NULL || primaryKeyStr == NULL || strlen(primaryKeyStr) == 0)
	{
		UnionUserErrLog("in UnionCreateTable:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	if (UnionExistTable(tableName))
	{
		UnionUserErrLog("in UnionCreateTable:: table [%s] already exist!\n",tableName);
		return(errCodeDatabaseMDL_TableAlreadyExist);
	}

	// ������Ŀ¼
        if ((ret = UnionGetDirOfTable(tableName,dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionGetDirOfTable of [%s]!\n",tableName);
		return(ret);
	}
	
	// �޸ı���Ϣ�ļ�
	ret=UnionAddToAllTableInfo(tableName);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionAddToAllTableInfo for %s fail! return=[%d]\n",tableName,ret);
		return(ret);
	}

	if ((ret = UnionCreateDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionCreateDir of [%s]!\n",dir);
		UnionDeleteFromAllTableInfo(tableName);	// ɾ������Ϣ
		return(ret);
	}

	// ������¼��Ϣ�ļ�
	ret=UnionCreateRecInfo(tableName,primaryKeyStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionCreateRecInfo for %s fail! return=[%d]\n",tableName,ret);
		UnionDeleteFromAllTableInfo(tableName);	// ɾ������Ϣ
		UnionRemoveDir(dir);			// ɾ����Ŀ¼		
		return(ret);
	}
	return(0);
}

/*
���ܣ�ɾ��һ�ű�
���������
	tableName������
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDropTable(char *tableName)
{
	char	tmpBuf[512];
	char	dir[512];
	int	ret;

	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionDropTable:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	if (!UnionExistTable(tableName))
	{
		UnionUserErrLog("in UnionDropTable:: table [%s] does not exist!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��Ŀ¼
        if ((ret = UnionGetDirOfTable(tableName,dir)) < 0)
	{
		UnionUserErrLog("in UnionDropTable:: UnionGetDirOfTable of [%s]!\n",tableName);
		return(ret);
	}
	
	// ɾ�����м�¼�ͼ�¼��Ϣ�ļ�
	sprintf(tmpBuf,"%s/*.rec",dir);
	UnionDeleteRecFile(tmpBuf);
	sprintf(tmpBuf,"%s/rec.info",dir);
	UnionDeleteRecFile(tmpBuf);
	
	// �޸ı���Ϣ�ļ�
	ret=UnionDeleteFromAllTableInfo(tableName);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDropTable:: UnionDeleteFromAllTableInfo for %s fail! return=[%d]\n",tableName,ret);
		return(ret);
	}

	// ɾ��Ŀ¼
	ret=UnionRemoveDir(dir);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDropTable:: UnionRemoveDir of %s fail! return=[%d]\n",dir,ret);
		return(ret);
	}
	return(ret);
}

/*
���ܣ��ж�һ�ű��Ƿ����
���������
	tableName������
���������
	��
����ֵ��
	>0�������
	0��������
	<0��ʧ��
*/
int UnionExistTable(char *tableName)
{
	char	dir[512];
        int     ret;

	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionExistTable:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	// ��Ŀ¼
        if ((ret = UnionGetDirOfTable(tableName,dir)) < 0)
	{
		UnionUserErrLog("in UnionExistTable:: UnionGetDirOfTable of [%s]!\n",tableName);
		return(ret);
	}
	return(UnionExistsDir(dir));
}
