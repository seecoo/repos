//	Author: Wolfgang Wang
//	Date: 2008/3/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include "unionREC.h"
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>

#include "unionWorkingDir.h"
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionRecFile.h"
#include "unionComplexDBObjectFileName.h"

const char OBJECTDIR[64] = "object";

// ���ܣ���ö����������Ŀ¼
/*
���������
	��
���������
	mainDir		�����������Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetMainObjectDir(char *mainDir)
{
#ifndef _WIN32
	char *ptr;
	if ((ptr = getenv("UNIONOBJECTDIR")) == NULL)
	{
		if ((ptr = UnionReadStringTypeRECVar("unionObjectDir")) == NULL)
		{
			sprintf(mainDir,"%s/object",getenv("UNIONREC"));
			return(0);
		}
	}
	sprintf(mainDir,"%s",ptr);
	//UnionLog("in UnionGetMainObjectDir:: mainDir = [%s]\n",mainDir);
#else
	char	tmpBuf[512];
	memset(tmpBuf,0,sizeof(tmpBuf));
 	UnionGetMainWorkingDir(tmpBuf);
        sprintf(mainDir,"%s/%s",tmpBuf, OBJECTDIR);
#endif
	return(0);
}

// ���ܣ����һ���������ִ��ǰ�Ĵ������������壬���ж��ļ��Ƿ����
/*
���������
	idOfObject	���������
	operationTag	������ʶ
	beforeOrAfter	�ǲ���֮ǰִ�л���֮��ִ��
���������
	fileName	�ļ�����
����ֵ��
	>0���ļ�����
	=0���ļ�������
	<0��������
*/
int UnionGetObjectTriggerFileName(TUnionIDOfObject idOfObject,char *operationTag,char *beforeOrAfter,char *fileName)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (operationTag == NULL) || (beforeOrAfter == NULL) || (fileName == NULL))
		return(0);
        memset(mainDir,0,sizeof(mainDir));
	UnionGetMainObjectDir(mainDir);
        sprintf(fileName,"%s/trigger/%s.%s.%s.sql",mainDir,idOfObject,operationTag,beforeOrAfter);
        
        UnionProgramerLog("in UnionGetObjectTriggerFileName, fileName is: [%s].\n", fileName);
        return(UnionExistsFile(fileName));
}

/* **** ��һ����	����Ĵ���	Ŀ¼���ļ����� **** */

// ���ܣ����һ������һ��������ļ�����
/*
���������
	idOfObject	���������
���������
	fileName	������Ĵ����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectDefCreateFileName(TUnionIDOfObject idOfObject,char *fileName)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (fileName == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(fileName,"%s/sql/%s.sql",mainDir,idOfObject);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(fileName,"%s/%s/sql/%s.sql",mainDir, OBJECTDIR, idOfObject);
#endif
	return(0);
}

/* **** �ڶ�����	����Ķ���	Ŀ¼���ļ����� **** */

// ���ܣ����һ��������Ĵ洢Ŀ¼
/*
���������
	idOfObject	���������
���������
	objectDir	������Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectDefDirName(TUnionIDOfObject idOfObject,char *objectDir)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (objectDir == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(objectDir,"%s/objDef",mainDir);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(objectDir,"%s/%s/objDef", mainDir, OBJECTDIR);
#endif
	return(0);
}

// ���ܣ����һ������Ķ����ļ�����
/*
���������
	idOfObject	���������
���������
	objectDir	����Ķ����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectDefFileName(TUnionIDOfObject idOfObject,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectDefDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/%s.def",dirName,idOfObject);
	return(0);
}

// ���ܣ����һ��������ⲿ�ؼ��ֶ����嵥�ļ�����
/*
���������
	idOfObject	���������
���������
	fileName	������ⲿ�ؼ��ֶ����嵥�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectChildrenDefFileName(TUnionIDOfObject idOfObject,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectDefDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/%s.child.def",dirName,idOfObject);
	return(0);
}

/* **** ��������	����ļ�¼	Ŀ¼���ļ����� **** */

// ���ܣ�������ж����¼�Ĵ洢Ŀ¼
/*
���������
	��
���������
	objectDir	�����¼�Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetAllObjectRecDirName(char *objectDir)
{
        char	mainDir[512+1];

	if (objectDir == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(objectDir,"%s/objRec",mainDir);
	//UnionLog("in UnionGetAllObjectRecDirName:: objectDir = [%s]\n",objectDir);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(objectDir,"%s/%s/objRec", mainDir, OBJECTDIR);
#endif
	return(0);
}

// ���ܣ����һ�������¼�Ĵ洢Ŀ¼
/*
���������
	idOfObject	���������
���������
	objectDir	����Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecMainDirName(TUnionIDOfObject idOfObject,char *objectDir)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (objectDir == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(objectDir,"%s/objRec/%s",mainDir,idOfObject);
	//UnionLog("in UnionGetObjectRecMainDirName:: objectDir = [%s]\n",objectDir);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(objectDir,"%s/%s/objRec/%s", mainDir, OBJECTDIR, idOfObject);
#endif
	return(0);
}

// ���ܣ����һ������ļ�ֵ�����ļ�����
/* ���������
	idOfObject	���������
���������
	fileName	��ֵ�����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjecRecPrimaryKeyIndexFileName(TUnionIDOfObject idOfObject,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecMainDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/primaryKey",dirName);
	return(0);
}

// ���ܣ����һ�������Ψһֵ�ļ�����
/*
���������
	idOfObject	���������
	fldGrpName	Ψһֵ������
���������
	objectDir	��ֵ�����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecUniqueKeyIndexFileName(TUnionIDOfObject idOfObject,char *fldGrpName,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecMainDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/unique-%s",dirName,fldGrpName);
	return(0);
}

// ���ܣ����һ������ʵ���Ĵ洢Ŀ¼
/*
���������
	idOfObject	���������
	primaryKey	����ʵ���Ĺؼ���
���������
	dir		����ʵ���Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecDirName(TUnionIDOfObject idOfObject,char *primaryKey,char *dir)
{
	char	dirName[512];
	int	ret;
	
	if (dir == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecMainDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(dir,"%s/%s",dirName,primaryKey);
	return(0);
}

// ���ܣ����һ������ʵ����ֵ�ļ�����
/*
���������
	idOfObject	���������
	primaryKey	����ʵ���Ĺؼ���
���������
	fileName	����ʵ����ֵ�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecValueFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecDirName(idOfObject,primaryKey,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/value",dirName);
	return(0);
}

// ���ܣ����һ������ʵ�����õĶ���ʵ���嵥�ļ�����
/*
���������
	idOfObject	���������
	primaryKey	����ʵ���Ĺؼ���
���������
	fileName	���õĶ���ʵ���嵥�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecParentsListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecDirName(idOfObject,primaryKey,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/parents",dirName);
	return(0);
}

// ���ܣ����һ��ʹ�øö���ʵ���Ķ���ʵ���嵥�ļ�����
/*
���������
	objectName�������������
	primaryKey��������ʵ���Ĺؼ���
	foreignObjectName���ⲿ���������
���������
	fileName	ʹ�øö���ʵ���Ķ���ʵ���嵥�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecChildListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *foreignObjectName,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecDirName(idOfObject,primaryKey,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/child-%s",dirName,foreignObjectName);
	return(0);
}

// ���ܣ����˵� ������Ŀ¼ �ַ���
/*
���������
	absolutePath������·��
���������
	relativelyPath:	���·��
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionFilterMainWorkingDirStr(char *absolutePath, char *relativelyPath)
{
        char	mainDir[512+1];
        char    *pos;

	if ((absolutePath == NULL) || (relativelyPath == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
        sprintf(mainDir,"%s",mainDir);

        // �жϾ���·�����Ƿ���  ������Ŀ¼ �ַ���
        pos = strstr(absolutePath, mainDir);
        if((pos - absolutePath) < 0)
                return (UnionSetUserDefinedErrorCode(errCodeParameter));

        sprintf(relativelyPath, "%s/%s", OBJECTDIR, absolutePath + strlen(mainDir) + 1);
#else
	UnionGetMainWorkingDir(mainDir);

        // �жϾ���·�����Ƿ���  ������Ŀ¼ �ַ���
        pos = strstr(absolutePath, mainDir);
        if((pos - absolutePath) < 0)
                return (UnionSetUserDefinedErrorCode(errCodeParameter));

        sprintf(relativelyPath, "%s", absolutePath + strlen(mainDir) + 1);
#endif
	return(0);
}

// ���ܣ���ֵ�ļ������·����Ϊ����·��
/*
���������
	valueFileName		ֵ�ļ�����
���������
	fullValueFileName:	ֵ�ļ�����·��
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionPatchMainDirToObjectValueFileName(char *valueFileName, char *fullValueFileName)
{
        char	mainDir[512+1];
        char	value[512+1];
        char    *pos;

	if ((valueFileName == NULL) || (fullValueFileName == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));

        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
        memset(value,0,sizeof(value));
        UnionFilterDirFromDirStr(valueFileName,strlen(valueFileName),1,value);
        sprintf(fullValueFileName,"%s/%s",mainDir,value);
        //UnionLog("in UnionPatchMainDirToObjectValueFileName:: mainDir = [%s]fullValueFileName = [%s]\n",mainDir,fullValueFileName);
#else
	UnionGetMainWorkingDir(mainDir);
	sprintf(fullValueFileName,"%s/%s",mainDir,valueFileName);
#endif
	return(0);
}	

