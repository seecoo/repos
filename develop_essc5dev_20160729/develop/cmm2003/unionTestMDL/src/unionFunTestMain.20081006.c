//	Wolfgang Wang
//	2008/10/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionTestConf.h"
#include "unionGenerateMakeFile.h"

void main(int argc,char *argv[])
{
	int		ret;
	char		testDataFileName[256+1];
	int		index;
	char		cFileName[256+1];
	char		makeFileName[256+1];
	char		funName[256+1];
	char		*ptr;
	FILE		*fp;

reTestFun:
	ptr = UnionInput("\n\n������Ҫ���Եĺ��������ļ�������(exit�˳�)::\n");
	strcpy(funName,ptr);
	if (UnionIsQuit(ptr))
		return;
	
reTestData:
	ptr = UnionInput("\n\n��������������ļ�������(exit�˳�)::\n");
	strcpy(testDataFileName,ptr);
	if (UnionIsQuit(ptr))
		goto reTestFun;
	
	// ��ú������Գ�������ƣ������ɲ��Գ���
	memset(cFileName,0,sizeof(cFileName));
	UnionGetTestFunCProgramFileName(funName,cFileName);
	if ((ret = UnionGenerateTestProgramForFun(funName,testDataFileName,cFileName)) < 0)
	{
		printf("UnionGenerateTestProgramForFun failure! ret = [%d]\n",ret);
		goto reTestData;
	}
	printf("���ɲ��Գ���[%s]OK...\n",cFileName);
	// ���ɱ������		
	memset(makeFileName,0,sizeof(makeFileName));
	UnionGetTestFunMakeFileName(funName,makeFileName);
	if ((ret = UnionGenerateMakeFileForTestProgramForFun(cFileName,"a.out",makeFileName)) < 0)
	{
		printf("UnionGenerateMakeFileForTestProgramForFun failure! ret = [%d]\n",ret);
		goto reTestData;
	}
	printf("����make�ļ�[%s]OK...\n",makeFileName);
	
	// ִ�б����
	system(makeFileName);
	
	// ִ�п�ִ�г���
	system("a.out");
	sprintf(makeFileName,"rm a.out");
	system(makeFileName);
	
	goto reTestData;
}

