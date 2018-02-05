//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionDefaultVarTypeDefDir.h"
#include "UnionStr.h"

char gunionDisignDir[256+1] = "";

/*
����	
	���ñ������Ͷ���Ŀ¼
�������
	dir		Ŀ¼
�������
	��
����ֵ
	>=0		�ɹ�
	=		�������
*/
int UnionSetDefaultDesignDir(char *dir)
{
	strcpy(gunionDisignDir,dir);
	return(0);
}

/*
����	
	��ñ������Ͷ���Ŀ¼
�������
	��
�������
	��
����ֵ
	��������Ŀ¼
*/
char *UnionGetDefaultDesignDir()
{
	if (strlen(gunionDisignDir) == 0)
		sprintf(gunionDisignDir,"%s",getenv("UNIONDESIGNDIR"));
	return(gunionDisignDir);
}

/*
����	
	���ָ���������ͱ�ʶ�����ļ�
�������
	varTypeName	������������
�������
	fileName	�ļ�����
����ֵ
	��
*/
void UnionGetDefaultFileNameOfVarTypeTagDef(char *varTypeName,char *fileName)
{
	sprintf(fileName,"%s/%s/keyWord.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfBaseDef);
}

/*
����	
	���ָ��ģ��Ķ����ļ�
�������
	moduleName	ģ������
�������
	fileName	�ļ�����
����ֵ
	��
*/
void UnionGetDefaultFileNameOfModuleDef(char *moduleName,char *fileName)
{
	sprintf(fileName,"%s/%s/module.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfBaseDef);
}

/*
����	
	���ָ������Ķ����ļ�
�������
	moduleName	��������
�������
	fileName	�ļ�����
����ֵ
	��
*/
void UnionGetDefaultFileNameOfProgramDef(char *programName,char *fileName)
{
	sprintf(fileName,"%s/%s/program.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfBaseDef);
}

/*
����	
	���ָ���������Ͷ����ļ�
�������
	varTypeName	������������
�������
	fileName	�ļ�����
����ֵ
	���������ļ���
*/
void UnionGetDefaultFileNameOfVarTypeDef(char *varTypeName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfDataStruct,varTypeName);
}

/*
����	
	���ָ�����������ļ�
�������
	constName	��������
�������
	fileName	�ļ�����
����ֵ
	���������ļ���
*/
void UnionGetDefaultFileNameOfConstDef(char *constName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfConst,constName);
}


/*
����	
	���ָ�����������ļ�
�������
	funName		��������
�������
	fileName	�ļ�����
����ֵ
	���������ļ���
*/
void UnionGetDefaultFileNameOfFunDef(char *funName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfFun,funName);
}

/*
����	
	���ָ��ȫ�ֱ��������ļ�
�������
	funName		��������
�������
	fileName	�ļ�����
����ֵ
	���������ļ���
*/
void UnionGetDefaultFileNameOfVariableDef(char *funName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfVariable,funName);
}

/*
����	
	��ȡ������ͷ�ļ������ļ�
�������
	oriFileName	ͷ�ļ������ļ�
�������
	fullFileName	������ͷ�ļ������ļ�
����ֵ
	ͷ�ļ�������
*/
void UnionGetFullNameOfIncludeFileConf(char *oriFileName,char *fullFileName)
{
	sprintf(fullFileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfIncludeConf,oriFileName);
	return;
}
