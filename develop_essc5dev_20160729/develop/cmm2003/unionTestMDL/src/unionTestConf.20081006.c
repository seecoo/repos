//	Wolfgang Wang
//	2008/10/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionTestConf.h"

char gunionTestLibDefFileName[128+1] = "libFile";

/*
功能	
	设置测试使用的库定义文件
输入参数
	fileName	文件名称
输入出数
	无
返回值
	无
*/
void UnionSetTestLibDefFileName(char *fileName)
{
	strcpy(gunionTestLibDefFileName,fileName);
}

/*
功能	
	获得生成的函数测试程序的名称
	这个文件名称为：$DIROFTEST/demo/test-funName.c
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetTestFunCProgramFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/demo/test-%s.c",getenv("DIROFTEST"),funName);
	return;
}

/*
功能	
	获得生成的函数测试可执行程序的名称
	这个文件名称为：$DIROFTEST/demo/test-funName
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetTestFunExeProgramFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/demo/test-%s",getenv("DIROFTEST"),funName);
	return;
}

/*
功能	
	获得生成的函数测试编译程序的名称
	这个文件名称为：$DIROFTEST/demo/mkfunName
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetTestFunMakeFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/demo/mk%s",getenv("DIROFTEST"),funName);
	return;
}

/*
功能	
	获得定义了库文件的文件的名称
	这个文件名称为：$DIROFTEST/conf/libFile.def
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetLibFileDefFileName(char *fileName)
{
	sprintf(fileName,"%s/conf/%s.def",getenv("DIROFTEST"),gunionTestLibDefFileName);
	return;
}

/*
功能	
	获得定义了头文件目录定义文件的名称
	这个文件名称为：$DIROFTEST/conf/includeFileDir.def
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetIncludeFileDirDefFileName(char *fileName)
{
	sprintf(fileName,"%s/conf/includeFileDir.def",getenv("DIROFTEST"));
	return;
}

/*
功能	
	获得定义了头文件的文件的名称
	这个文件名称为：$DIROFTEST/conf/includeFile.def
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetIncludeFileDefFileName(char *fileName)
{
	sprintf(fileName,"%s/conf/includeFile.def",getenv("DIROFTEST"));
	return;
}

/*
功能	
	获得读函数输入参数值定义的索引文件
	这个文件名称为：$DIROFTEST/funVarValueListIndex/funName.input
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFunVarInputValueListIndexFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/funVarValueListIndex/%s.input",getenv("DIROFTEST"),funName);
	return;
}

/*
功能	
	获得测试数据文件全名
	这个文件名称为：$DIROFTEST/testdata/fileName
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFullFileNameOfTestData(char *fileName,char *fullFileName)
{
	sprintf(fullFileName,"%s/testdata/%s",getenv("DIROFTEST"),fileName);
	return;
}

/*
功能	
	获得读函数输出参数值定义的索引文件
	这个文件名称为：$DIROFTEST/funVarValueListIndex/funName.output
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFunVarOutputValueListIndexFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/funVarValueListIndex/%s.output",getenv("DIROFTEST"),funName);
	return;
}

/*
功能	
	获得读函数返回值参数值定义的索引文件
	这个文件名称为：$DIROFTEST/funVarValueListIndex/funName.return
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFunVarReturnValueListIndexFileName(char *funName,char *fileName)
{
	sprintf(fileName,"%s/funVarValueListIndex/%s.return",getenv("DIROFTEST"),funName);
	return;
}

