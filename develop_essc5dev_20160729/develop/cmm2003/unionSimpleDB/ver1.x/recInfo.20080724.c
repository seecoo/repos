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
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionErrCode.h"

#include "database.h"
#include "recInfo.h"

// ˵�������ļ��еĳ�������ļ����ݿ�Ĳ���

// ��ü�¼��Ϣ�ļ�
int UnionGetFileNameOfRecInfo(char *tableName,char *fileName)
{
        sprintf(fileName,"%s/%s/rec.info",UnionGetCurrentDatabaseName(),tableName);
        return(0);
}

/*
���ܣ�����һ����ļ�¼��Ϣ�ļ����ļ���ֻ�����У���һ��Ϊ��0������2��Ϊ��Ĺؼ���
���������
	tableName����Ӧ�ı���
	keyStr����Ĺؼ�������������ؼ����ɶ������ɣ��������֮����'|'�ָ�
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateRecInfo(char *tableName,char *keyStr)
{
	char	fileName[512];
	int	lenOfKey;
	char	tmpBuf[512+1];
	char	tmpKey[512+1];
        int     ret;
		
	if (tableName == NULL || keyStr == NULL)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	// ����ļ���
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	// д��¼��Ŀ��ؼ���
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionPutRecFldIntoRecStr("recNum","0",1,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionPutRecFldIntoRecStr recNum of [%s]\n",tableName);
		return(ret);
	}
        if ((ret = UnionAppendRecStrToFile(fileName,tmpBuf,strlen(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionAppendRecStrToFile recNum to [%s]\n",tableName);
		return(ret);
	}
	// д�ؼ���
	memset(tmpKey,0,sizeof(tmpKey));
	if ((ret = UnionConvertOneFldSeperatorInRecStrIntoAnother(keyStr,strlen(keyStr),'|',',',tmpKey,sizeof(tmpKey))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionConvertOneFldSeperatorInRecStrIntoAnother! key = [%s] for [%s]\n",keyStr,tableName);
		return(ret);
	}
	if ((ret = UnionPutRecFldIntoRecStr("primaryKey",tmpKey,strlen(tmpKey),tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionPutRecFldIntoRecStr key [%s] of [%s]\n",tmpKey,tableName);
		return(ret);
	}
        if ((ret = UnionAppendRecStrToFile(fileName,tmpBuf,strlen(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionAppendRecStrToFile primaryKey to [%s]\n",tableName);
		return(ret);
	}
	return(ret);
}

/*
���ܣ��ڼ�¼��Ϣ�ļ�������һ����¼�ļ�¼�ļ����ƣ�ͬʱ�޸ļ�¼����
���������
	tableName����Ӧ�ı���
	recFileName��Ҫ���ӵļ�¼�ļ�����
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionAddToRecInfo(char *tableName,char *recFileName)
{
	char	fileName[512];
        int     ret;
	
	if (tableName == NULL || recFileName == NULL)
	{
		UnionUserErrLog("in UnionAddToRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionAddToRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	return(UnionAppendRecStrToFile(fileName,recFileName,strlen(recFileName)));
}

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ɾ��һ����¼�ļ�¼�ļ����ƣ�ͬʱ�޸ļ�¼����
���������
	tableName����Ӧ�ı���
	recFileName��Ҫɾ���ļ�¼�ļ�����
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDeleteFromRecInfo(char *tableName,char *recFileName)
{
	char	fileName[512];
        int     ret;
	
	if (tableName == NULL || recFileName == NULL)
	{
		UnionUserErrLog("in UnionDeleteFromRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteFromRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	return(UnionDeleteRecStrFromFile(fileName,recFileName,strlen(recFileName)));
}

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ȡһ�ű�ļ�¼����
���������
	tableName����Ӧ�ı���
���������
	��
����ֵ��
	>=0���ɹ������ر�ļ�¼����
	<0��ʧ��
*/
int UnionGetTableRecordNumber(char *tableName)
{
	char	fileName[512+1];
        int     ret;

	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionGetTableRecordNumber:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecordNumber:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	return(UnionGetTotalRecNumInFile(fileName)-2);
}

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ȡһ�ű�ļ�¼�����͹ؼ���
���������
	tableName����Ӧ�ı���
���������
	primaryKey      �ؼ���
����ֵ��
	>=0���ɹ������ر�ļ�¼����
	<0��ʧ��
*/
int UnionGetTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey)
{
	int	ret;
	int	recNum;

	if ((recNum = UnionGetTableRecordNumber(tableName)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionGetTableRecordNumber [%s]!\n",tableName);
		return(recNum);
	}
	if ((ret = UnionGetPrimaryKeyFromTableRecInfo(tableName,primaryKey,sizeOfPrimaryKey)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionGetPrimaryKeyFromTableRecInfo [%s]!\n",tableName);
		return(ret);
	}
	return(recNum);
}

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ȡһ�ű�ļ�¼�����͹ؼ���
���������
	tableName����Ӧ�ı���
���������
	primaryKey      �ؼ���
����ֵ��
	>=0��		�ؼ��ֵĳ���
	<0��ʧ��
*/
int UnionGetPrimaryKeyFromTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey)
{
	char	fileName[512+1];
	char	recStr[512+1];
	int	lenOfRecStr;
        int     ret;

	if ((tableName == NULL) || (primaryKey == NULL))
	{
		UnionUserErrLog("in UnionGetPrimaryKeyFromTableRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetPrimaryKeyFromTableRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionReadRecOfIndexDirectlyFromFile(fileName,1,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionReadRecOfIndexDirectlyFromFile [%s] index = 1\n",tableName);
		return(lenOfRecStr);
	}
	memset(primaryKey,0,sizeOfPrimaryKey);
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"primaryKey",primaryKey,sizeOfPrimaryKey)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionReadRecFldFromRecStr [primaryKey] from [%s] of [%s]\n",recStr,tableName);
		return(ret);
	}
	UnionConvertOneFldSeperatorInRecStrIntoAnother(primaryKey,ret,',','|',primaryKey,sizeOfPrimaryKey);
	return(ret);
}
