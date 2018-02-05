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
功能	
	设置变量类型定义目录
输入参数
	dir		目录
输入出数
	无
返回值
	>=0		成功
	=		出错代码
*/
int UnionSetDefaultDesignDir(char *dir)
{
	strcpy(gunionDisignDir,dir);
	return(0);
}

/*
功能	
	获得变量类型定义目录
输入参数
	无
输入出数
	无
返回值
	变量定义目录
*/
char *UnionGetDefaultDesignDir()
{
	if (strlen(gunionDisignDir) == 0)
		sprintf(gunionDisignDir,"%s",getenv("UNIONDESIGNDIR"));
	return(gunionDisignDir);
}

/*
功能	
	获得指定奕量类型标识定义文件
输入参数
	varTypeName	变量类型名称
输入出数
	fileName	文件名称
返回值
	无
*/
void UnionGetDefaultFileNameOfVarTypeTagDef(char *varTypeName,char *fileName)
{
	sprintf(fileName,"%s/%s/keyWord.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfBaseDef);
}

/*
功能	
	获得指定模块的定义文件
输入参数
	moduleName	模块名称
输入出数
	fileName	文件名称
返回值
	无
*/
void UnionGetDefaultFileNameOfModuleDef(char *moduleName,char *fileName)
{
	sprintf(fileName,"%s/%s/module.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfBaseDef);
}

/*
功能	
	获得指定程序的定义文件
输入参数
	moduleName	程序名称
输入出数
	fileName	文件名称
返回值
	无
*/
void UnionGetDefaultFileNameOfProgramDef(char *programName,char *fileName)
{
	sprintf(fileName,"%s/%s/program.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfBaseDef);
}

/*
功能	
	获得指定奕量类型定义文件
输入参数
	varTypeName	变量类型名称
输入出数
	fileName	文件名称
返回值
	变量定义文件名
*/
void UnionGetDefaultFileNameOfVarTypeDef(char *varTypeName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfDataStruct,varTypeName);
}

/*
功能	
	获得指定常量定义文件
输入参数
	constName	常量名称
输入出数
	fileName	文件名称
返回值
	变量定义文件名
*/
void UnionGetDefaultFileNameOfConstDef(char *constName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfConst,constName);
}


/*
功能	
	获得指定函数定义文件
输入参数
	funName		函数名称
输入出数
	fileName	文件名称
返回值
	函数定义文件名
*/
void UnionGetDefaultFileNameOfFunDef(char *funName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfFun,funName);
}

/*
功能	
	获得指定全局变量定义文件
输入参数
	funName		函数名称
输入出数
	fileName	文件名称
返回值
	函数定义文件名
*/
void UnionGetDefaultFileNameOfVariableDef(char *funName,char *fileName)
{
	sprintf(fileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfVariable,funName);
}

/*
功能	
	获取完整的头文件定义文件
输入参数
	oriFileName	头文件定义文件
输入出数
	fullFileName	完整的头文件定义文件
返回值
	头文件的数量
*/
void UnionGetFullNameOfIncludeFileConf(char *oriFileName,char *fullFileName)
{
	sprintf(fullFileName,"%s/%s/%s.def",UnionGetDefaultDesignDir(),conDefaultDesignDirOfIncludeConf,oriFileName);
	return;
}
