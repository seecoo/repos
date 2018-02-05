//	Wolfgang Wang
//	2008/10/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionTestConf.h"

char gunionTestLibDefFileName[128+1] = "libFile";

/*
����	
	���ò���ʹ�õĿⶨ���ļ�
�������
	fileName	�ļ�����
�������
	��
����ֵ
	��
*/
void UnionSetTestLibDefFileName(char *fileName)
{
	strcpy(gunionTestLibDefFileName,fileName);
}

/*
����	
	������ɵĺ������Գ��������
	����ļ�����Ϊ��$DIROFTEST/demo/test-funName.c
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetTestFunCProgramFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/demo/test-%s.c",getenv("DIROFTEST"),funName);
	return;
}

/*
����	
	������ɵĺ������Կ�ִ�г��������
	����ļ�����Ϊ��$DIROFTEST/demo/test-funName
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetTestFunExeProgramFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/demo/test-%s",getenv("DIROFTEST"),funName);
	return;
}

/*
����	
	������ɵĺ������Ա�����������
	����ļ�����Ϊ��$DIROFTEST/demo/mkfunName
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetTestFunMakeFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/demo/mk%s",getenv("DIROFTEST"),funName);
	return;
}

/*
����	
	��ö����˿��ļ����ļ�������
	����ļ�����Ϊ��$DIROFTEST/conf/libFile.def
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetLibFileDefFileName(char *fileName)
{
	sprintf(fileName,"%s/conf/%s.def",getenv("DIROFTEST"),gunionTestLibDefFileName);
	return;
}

/*
����	
	��ö�����ͷ�ļ�Ŀ¼�����ļ�������
	����ļ�����Ϊ��$DIROFTEST/conf/includeFileDir.def
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetIncludeFileDirDefFileName(char *fileName)
{
	sprintf(fileName,"%s/conf/includeFileDir.def",getenv("DIROFTEST"));
	return;
}

/*
����	
	��ö�����ͷ�ļ����ļ�������
	����ļ�����Ϊ��$DIROFTEST/conf/includeFile.def
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetIncludeFileDefFileName(char *fileName)
{
	sprintf(fileName,"%s/conf/includeFile.def",getenv("DIROFTEST"));
	return;
}

/*
����	
	��ö������������ֵ����������ļ�
	����ļ�����Ϊ��$DIROFTEST/funVarValueListIndex/funName.input
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFunVarInputValueListIndexFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/funVarValueListIndex/%s.input",getenv("DIROFTEST"),funName);
	return;
}

/*
����	
	��ò��������ļ�ȫ��
	����ļ�����Ϊ��$DIROFTEST/testdata/fileName
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFullFileNameOfTestData(char *fileName,char *fullFileName)
{
	sprintf(fullFileName,"%s/testdata/%s",getenv("DIROFTEST"),fileName);
	return;
}

/*
����	
	��ö������������ֵ����������ļ�
	����ļ�����Ϊ��$DIROFTEST/funVarValueListIndex/funName.output
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFunVarOutputValueListIndexFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/funVarValueListIndex/%s.output",getenv("DIROFTEST"),funName);
	return;
}

/*
����	
	��ö���������ֵ����ֵ����������ļ�
	����ļ�����Ϊ��$DIROFTEST/funVarValueListIndex/funName.return
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFunVarReturnValueListIndexFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/funVarValueListIndex/%s.return",getenv("DIROFTEST"),funName);
	return;
}

