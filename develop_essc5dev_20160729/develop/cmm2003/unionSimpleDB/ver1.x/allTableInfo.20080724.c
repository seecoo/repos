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
#include "UnionStr.h"
#include "recInfo.h"
#include "allTableInfo.h"
#include "database.h"
#include "unionErrCode.h"
#include "UnionLog.h"

// ˵�������ļ��еĳ�������ļ����ݿ�Ĳ���

// ����ļ�����
int UnionGetAllTableInfoFileName(char *fileName)
{
	sprintf(fileName,"%s/alltable.info",UnionGetCurrentDatabaseName());		
        return(0);
}

/*
���ܣ�����һ���µı���Ϣ�ļ����ļ���ֻ��һ��ֵ"0"
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateAllTableInfo()
{
	char	fileName[512];
	char	tmpBuf[512+1];
        int     ret;
		
	// ����ļ���
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetAllTableInfoFileName(fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateAllTableInfo:: UnionGetAllTableInfoFileName\n");
		return(ret);
	}

	// д����Ŀ
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionPutRecFldIntoRecStr("tableNum","0",1,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateAllTableInfo:: UnionPutRecFldIntoRecStr tableNum\n");
		return(ret);
	}
        if ((ret = UnionAppendRecStrToFile(fileName,tmpBuf,strlen(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateAllTableInfo:: UnionAppendRecStrToFile tableNum\n");
		return(ret);
	}
	return(ret);
}

/*
���ܣ��ڱ���Ϣ�ļ�������һ�ű�����ƣ�ͬʱ�޸ı������
���������
	databaseName��Ҫ���ӵı���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionAddToAllTableInfo(char *tableName)
{
	char	fileName[512];
        int     ret;
	
	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionAddToAllTableInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetAllTableInfoFileName(fileName)) < 0)
	{
		UnionUserErrLog("in UnionAddToAllTableInfo:: UnionGetAllTableInfoFileName [%s]\n",tableName);
		return(ret);
	}
	return(UnionAppendRecStrToFile(fileName,tableName,strlen(tableName)));
}

/*
���ܣ��ӱ���Ϣ�ļ���ɾ��һ�ű�����ƣ�ͬʱ�޸ı������
���������
	databaseName��Ҫɾ���ı���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDeleteFromAllTableInfo(char *tableName)
{
	char	fileName[512];
        int     ret;
	
	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionDeleteFromAllTableInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetAllTableInfoFileName(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteFromAllTableInfo:: UnionGetAllTableInfoFileName [%s]\n",tableName);
		return(ret);
	}
	return(UnionDeleteRecStrFromFile(fileName,tableName,strlen(tableName)));
}

/*
���ܣ��ӱ���Ϣ�ļ���ȡһ�����ݿ��б������
���������
	��
���������
	��
����ֵ��
	>=0���ɹ������ر������
	<0��ʧ��
*/
int UnionGetAllTableNumber()
{
	char	fileName[512+1];
        int     ret;

	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetAllTableInfoFileName(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetAllTableNumber:: UnionGetAllTableInfoFileName\n");
		return(ret);
	}
	return(UnionGetTotalRecNumInFile(fileName)-1);
}

/*
�����еı���Ϣд�뵽�ļ���
*/
int UnionPrintAllTableNameToFile(char *fileName)
{
        char    primaryKey[512+1];
        char    infoFileName[512+1];
        int     ret;
        TUnionRecFileHDL    fromFp;
        FILE	*toFp;
        int     fileOpened = 0;
        char    tmpBuf[512+1];
        int     len;
        int     recNum = 0;
        int     tableRecNum;

 
        memset(infoFileName,0,sizeof(infoFileName));
        if ((ret = UnionGetAllTableInfoFileName(infoFileName)) < 0)
        {
                UnionUserErrLog("in UnionPrintAllTableNameToFile:: UnionGetAllTableInfoFileName!\n");
                return(ret);
        }
        if ((fromFp = UnionOpenRecFileHDL(infoFileName)) == NULL)
        {
                UnionSystemErrLog("in UnionPrintAllTableNameToFile:: UnionOpenRecFileHDL [%s]!\n",infoFileName);
                return(errCodeUseOSErrCode);
        }
        if (strcmp(fileName,"stderr") == 0)
                toFp = stderr;
        else if (strcmp(fileName,"stdout") == 0)
                toFp = stdout;
        else
        {
                fileOpened = 1;
                if ((toFp = fopen(fileName,"w")) == NULL)
                {
                        UnionSystemErrLog("in UnionPrintAllTableNameToFile:: fopen [%s]!\n",fileName);
                        fclose(fromFp);
                        return(errCodeUseOSErrCode);
                }
        }
        for (;;)
        {
                memset(tmpBuf,0,sizeof(tmpBuf));
		if (recNum == 0)
			len = UnionReadRecOfIndexFromFile(fromFp,1,tmpBuf,sizeof(tmpBuf));
		else
			len = UnionReadNextRecFromFile(fromFp,tmpBuf,sizeof(tmpBuf));
		if (len <= 0)
			break;
                recNum++;
                memset(primaryKey,0,sizeof(primaryKey));
                tableRecNum = UnionGetTableRecInfo(tmpBuf,primaryKey,sizeof(primaryKey));
                UnionConvertOneFldSeperatorInRecStrIntoAnother(primaryKey,strlen(primaryKey),'|',',',primaryKey,sizeof(primaryKey));
                fprintf(toFp,"tableName=%s|recNum=%d|primaryKey=%s\n",tmpBuf,tableRecNum,primaryKey);
        }
        UnionCloseRecFileHDL(fromFp);
        if (fileOpened)
                fclose(toFp);
        return(recNum);
}

