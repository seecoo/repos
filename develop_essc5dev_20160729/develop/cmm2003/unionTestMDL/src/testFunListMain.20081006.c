//	Wolfgang Wang
//	2008/10/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionTestConf.h"
#include "unionGenerateMakeFile.h"
#include "unionGenFunTestProgram.h"
#include "unionGenFunListTestProgram.h"
#include "UnionLog.h"

int UnionTestUsingProcAndData(char *format,char *dataFile,char *procFile,int display)
{
	int		ret;
	char		cFileName[256+1];
	char		makeFileName[256+1];
	char		tmpDataFileName[256+1];

	if ((dataFile != NULL) && (strlen(dataFile) != 0))
	{
		sprintf(tmpDataFileName,"%s%s",format,dataFile);
		if ((ret = UnionSetFileNameOfFixedCodesConfFile(tmpDataFileName)) < 0)
		{
			if (display)
				printf("UnionSetFileNameOfFixedCodesConfFile [%s]!\n",tmpDataFileName);
			else
				UnionUserErrLog("in UnionTestUsingProcAndData:: UnionSetFileNameOfFixedCodesConfFile [%s]!\n",tmpDataFileName);
			return(ret);
		}
		if (display)
			printf("UnionSetFileNameOfFixedCodesConfFile from [%s] OK!\n",tmpDataFileName);
		else
			UnionLog("in UnionTestUsingProcAndData:: UnionSetFileNameOfFixedCodesConfFile [%s] OK!\n",tmpDataFileName);
	}
	
	// 获得函数测试程序的名称，并生成测试程序
	memset(cFileName,0,sizeof(cFileName));
	UnionGetTestFunCProgramFileName(procFile,cFileName);
	if ((ret = UnionAutoGenerateFunForFunGrpToSpecCFile(procFile,cFileName)) < 0)
	{
		if (display)
			printf("UnionAutoGenerateFunForFunGrpToSpecCFile from [%s] to [%s] failure! ret = [%d]\n",procFile,cFileName,ret);
		else
			UnionUserErrLog("in UnionTestUsingProcAndData:: UnionAutoGenerateFunForFunGrpToSpecCFile from [%s] to [%s] failure! ret = [%d]\n",procFile,cFileName,ret);
		return(ret);
	}
	if (display)
		printf("生成测试程序[%s]OK...\n",cFileName);
	else
		UnionNullLog("生成测试程序[%s]OK...\n",cFileName);
	// 生成编译程序		
	memset(makeFileName,0,sizeof(makeFileName));
	UnionGetTestFunMakeFileName(procFile,makeFileName);
	if ((ret = UnionGenerateMakeFileForTestProgramForFun(cFileName,"a.out",makeFileName)) < 0)
	{
		if (display)
			printf("UnionGenerateMakeFileForTestProgramForFun failure! ret = [%d]\n",ret);
		else
			UnionUserErrLog("in UnionTestUsingProcAndData:: UnionGenerateMakeFileForTestProgramForFun failure! ret = [%d]\n",ret);
		return(ret);
	}
	if (display)
	{
		printf("生成make文件[%s]OK...\n",makeFileName);
		printf("执行测试程序...\n");
	}
	else
	{
		UnionNullLog("生成make文件[%s]OK...\n",makeFileName);
		UnionNullLog("执行测试程序...\n");
	}
	// 执行编程序
	system(makeFileName);
	
	// 执行可执行程序
	ret = system("a.out");
	sprintf(makeFileName,"rm a.out");
	system(makeFileName);
	
	return(ret);
}

void UnionHelp()
{
	printf("Usage:: testFunList [-lib libDefFileName] [-proc procFileName] [-name nameFormat [01 02 ...]] [procFile ...]\n");
	return;
}

void main(int argc,char *argv[])
{
	int		ret;
	char		defaultDataFile[256+1];
	char		*ptr;
	char		procFile[256];
	int		index;
	char		format[128+1];
	int		userSetDataFile = 0;
	int		useDefaultFile = 1;
	
	memset(procFile,0,sizeof(procFile));
	memset(format,0,sizeof(format));
	memset(defaultDataFile,0,sizeof(defaultDataFile));
	strcpy(defaultDataFile,"defaultFixed");
	if (argc <= 1)
		goto reTestSpecProc;
	if ((strcmp(argv[1],"help") == 0) || (strcmp(argv[1],"?") == 0))
	{
		UnionHelp();
		return;
	}
	for (index = 1; index < argc; index++)
	{
		if (strcmp(argv[index],"-lib") == 0)
		{
			if (argc <= index+1)
			{
				UnionHelp();
				return;
			}
			UnionSetTestLibDefFileName(argv[index+1]);
			index++;
			continue;
		}
		if (strcmp(argv[index],"-proc") == 0)
		{
			if (argc <= index+1)
			{
				UnionHelp();
				return;
			}
			strcpy(procFile,argv[index+1]);
			index++;
			continue;
		}
		else if (strcmp(argv[index],"-name") == 0)
		{
			if (argc <= index+1)
			{
				UnionHelp();
				return;
			}
			strcpy(format,argv[index+1]);
			index++;
			continue;
		}
		break;
	}
	if ((strlen(format) != 0) && (strlen(procFile) == 0))
	{
		UnionHelp();
		return;
	}
	for (; index < argc; index++)
	{
		userSetDataFile = 1;
		UnionTestUsingProcAndData(format,argv[index],procFile,1);
	}
	if (userSetDataFile)
		return;

reTestSpecProc:
	if (strlen(procFile) != 0)
	{
		printf("\n\n*** 正在测试流程::%s...\n",procFile);
		if (strlen(format) == 0)
			ptr = UnionInput("请输入测试数据文件名称(exit退出)::\n");
		else
			ptr = UnionInput("请输入名如%sNN的测试数据文件的序号,NN是序号(exit退出)::\n",format);
		strcpy(defaultDataFile,ptr);
		useDefaultFile = 0;
	}
	else
	{
		printf("\n\n*** 测试使用的测试数据文件::%s...\n",defaultDataFile);
		ptr = UnionInput("请输入测试流程文件名称(exit退出)::\n");
		strcpy(procFile,ptr);
		useDefaultFile = 1;
	}		
	if (UnionIsQuit(ptr))
		return;
	printf("使用测试数据[%s%s]测试流程[%s]...\n",format,defaultDataFile,procFile);
	UnionTestUsingProcAndData(format,defaultDataFile,procFile,1);
	if (useDefaultFile)
		memset(procFile,0,sizeof(procFile));
	goto reTestSpecProc;
}
