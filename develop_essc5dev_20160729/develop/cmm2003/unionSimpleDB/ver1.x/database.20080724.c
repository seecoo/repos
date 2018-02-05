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
#include "database.h"
#include "allTableInfo.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionErrCode.h"

// ˵�������ļ��еĳ�������ļ����ݿ�Ĳ���

char	gunionDatabaseName[512] = "";	// ��ŵ�ǰ���ݿ�Ŀ¼
int	gunionIsDatabaseConnected=0;	// �ñ�־����ֻ������һ�����ݿ⣬��ֻ���ڹر�ԭ�������ݿ�󣬲������µ����ݿ⽨������

// ��õ�ǰ���ݿ������
char *UnionGetCurrentDatabaseName()
{
        return(gunionDatabaseName);
}

// ������ݿ��Ŀ¼
int UnionGetDatabaseDir(char *databaseName,char *dir)
{
	char	*ptr;

#ifdef _WIN32
	char	mainDir[512+1];
	
	memset(mainDir,0,sizeof(mainDir));
	UnionGetMainWorkingDir(mainDir);
	sprintf(dir,"%s/%s",mainDir,databaseName);
	return(0);
#else
	if ((ptr = (char *)getenv("UNIONDB")) == NULL)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UNIONDB not defined in .profile!\n");
		return(errCodeParameter);
	}
	sprintf(dir,"%s/%s",ptr,databaseName);	
	return(0);
#endif
}

/*
���ܣ�����һ�����ݿ�
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateDatabase(char *databaseName)
{
	char	dir[512];
	int	ret;

	if (UnionExistDatabase(databaseName))
	{
		UnionUserErrLog("in UnionCreateDatabase:: database [%s] already exist!\n",databaseName);
		return(errCodeDatabaseMDL_DatabaseAlreadyExist);
	}

	// �������ݿ�Ŀ¼
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(ret);
	}
	if ((ret = UnionCreateDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionCreateDir [%s]!\n",dir);
		return(ret);
	}
        if ((ret = UnionConnectDatabase(databaseName)) < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionConnectDatabase [%s]!\n",databaseName);
		return(ret);
	}
	// ��������Ϣ�ļ�
	ret=UnionCreateAllTableInfo();
	if (ret < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionCreateAllTableInfo for [%s] fail! return=[%d]\n",databaseName,ret);
		UnionRemoveDir(dir);
		return(ret);
	}
	return(0);
}

/*
���ܣ�ɾ��һ�����ݿ�
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDropDatabase(char *databaseName)
{
	char	dir[512];
	int	ret;

	if (!UnionExistDatabase(databaseName))
	{
		UnionUserErrLog("in UnionDropDatabase:: database [%s] already exist!\n",databaseName);
		return(errCodeDatabaseMDL_DatabaseNotFound);
	}

	// �������ݿ�Ŀ¼
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionDropDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(ret);
	}
	if ((ret = UnionRemoveDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionDropDatabase:: UnionRemoveDir [%s]!\n",dir);
		return(ret);
	}
	return(ret);
}

/*
���ܣ��ж�һ�����ݿ��Ƿ����
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	>0�����ݿ����
	0�����ݿⲻ����
	<0��ʧ��
*/
int UnionExistDatabase(char *databaseName)
{
	char	dir[512+1];
	int	ret;
	
	// �������ݿ�Ŀ¼
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionExistDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(0);
	}
	return(UnionExistsDir(dir));
}

/*
���ܣ����������ݿ������
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionConnectDatabase(char *databaseName)
{
	char	dir[512];
        int     ret;
	
	if (gunionIsDatabaseConnected)
		return(0);
	if (!UnionExistDatabase(databaseName))
	{
		UnionUserErrLog("in UnionConnectDatabase:: database [%s] does not exist!\n",databaseName);
		return(errCodeDatabaseMDL_DatabaseNotFound);
	}
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionConnectDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(ret);
	}
	strcpy(gunionDatabaseName,dir);
	gunionIsDatabaseConnected = 1;
	return(0);
}

/*
���ܣ��Ͽ������ݿ������
���������
	��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDisconnectDatabase()
{
	memset(gunionDatabaseName,0,sizeof(gunionDatabaseName));
	gunionIsDatabaseConnected = 0;
	return(0);
}
