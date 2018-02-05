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
	ptr = UnionInput("\n\n请输入要测试的函数定义文件的名称(exit退出)::\n");
	strcpy(funName,ptr);
	if (UnionIsQuit(ptr))
		return;
	
reTestData:
	ptr = UnionInput("\n\n请输入测试数据文件的名称(exit退出)::\n");
	strcpy(testDataFileName,ptr);
	if (UnionIsQuit(ptr))
		goto reTestFun;
	
	// 获得函数测试程序的名称，并生成测试程序
	memset(cFileName,0,sizeof(cFileName));
	UnionGetTestFunCProgramFileName(funName,cFileName);
	if ((ret = UnionGenerateTestProgramForFun(funName,testDataFileName,cFileName)) < 0)
	{
		printf("UnionGenerateTestProgramForFun failure! ret = [%d]\n",ret);
		goto reTestData;
	}
	printf("生成测试程序[%s]OK...\n",cFileName);
	// 生成编译程序		
	memset(makeFileName,0,sizeof(makeFileName));
	UnionGetTestFunMakeFileName(funName,makeFileName);
	if ((ret = UnionGenerateMakeFileForTestProgramForFun(cFileName,"a.out",makeFileName)) < 0)
	{
		printf("UnionGenerateMakeFileForTestProgramForFun failure! ret = [%d]\n",ret);
		goto reTestData;
	}
	printf("生成make文件[%s]OK...\n",makeFileName);
	
	// 执行编程序
	system(makeFileName);
	
	// 执行可执行程序
	system("a.out");
	sprintf(makeFileName,"rm a.out");
	system(makeFileName);
	
	goto reTestData;
}

