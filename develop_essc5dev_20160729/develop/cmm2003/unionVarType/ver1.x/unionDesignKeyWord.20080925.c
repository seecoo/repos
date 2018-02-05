//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionDesignKeyWord.h"
#include "unionVarTypeDef.h"
#include "unionModuleDef.h"
#include "unionProgramDef.h"
#include "unionErrCode.h"
#include "UnionLog.h"

/*
功能	
	获得一个函数所在程序的全名
输入参数
	funName		函数名称
输入出数
	fullProgramName	程序的全名
	incFileName	程序应包括的头文件配置
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetFullProgramFileNameOfFun(char *funName,char *fullProgramName,char *incFileName)
{
	char	nameOfProgram[128+1];
	char	nameOfModule[128+1];
	char	devDir[256+1];
	char	version[128+1];
	int	ret;
	
	if ((ret = UnionGetProgramFileNameOfKeyWord(funName,nameOfProgram)) < 0)
	{
		UnionUserErrLog("in UnionGetFullProgramFileNameOfFun:: UnionGetProgramFileNameOfKeyWord [%s]\n",funName);
		return(ret);
	}
	if ((ret = UnionGetModuleNameAndVersionOfProgram(nameOfProgram,nameOfModule,version)) < 0)
	{
		UnionUserErrLog("in UnionGetFullProgramFileNameOfFun:: UnionGetModuleNameAndVersionOfProgram [%s]\n",nameOfProgram);
		return(ret);
	}
	if ((ret = UnionGetDevDirAndIncFileConfOfModule(nameOfModule,devDir,incFileName)) < 0)
	{
		UnionUserErrLog("in UnionGetFullProgramFileNameOfFun:: UnionGetDevDirAndIncFileConfOfModule [%s]\n",nameOfModule);
		return(ret);
	}
	if (fullProgramName != NULL)
		sprintf(fullProgramName,"%s/%s.%s.c",devDir,nameOfProgram,version);
	return(0);
}
	
/*
功能	
	显示一个类型的定义
输入参数
	typeTag		指定的类型
	keyWord		类型名称
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionDisplayKeyWordDefOfSpecTypeToFp(int typeTag,char *keyWord,FILE *fp)
{
	int	ret;
	int	tag;

	if (keyWord == NULL)
		return(errCodeParameter);
		
	switch (typeTag)
	{
		case	conDesginKeyWordTagFun:	// 函数
			ret = UnionPrintSpecFunDefToFp(keyWord,fp);
			break;
		case	conDesginKeyWordTagConst:	// 常量
			ret = UnionPrintSpecConstDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagSimpleType:	// 简单类型
			ret = UnionPrintSpecSimpleTypeDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagStruct:	// 结构
			ret = UnionPrintSpecStructDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagUnion:	// 联合
			ret = UnionPrintSpecUnionDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagArray:	// 数组
			ret = UnionPrintSpecArrayDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagPointer:	// 指针
			ret = UnionPrintSpecPointerDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagEnum:	// 枚举
			ret = UnionPrintSpecEnumDefToFp(keyWord,fp);
			break;
		case	conDesginKeyWordTagGlobalVar:	// 全局变量
			ret = UnionPrintSpecVariableDefToFp(keyWord,fp);
			break;
		default:
			UnionUserErrLog("in UnionDisplayKeyWordDefOfSpecTypeToFp:: invalid keyWord [%s]\n",keyWord,fp);
			return(errCodeCDPMDL_InvalidKeyWord);
	}
	return(ret);
}

/*
功能	
	显示一个类型的定义
输入参数
	keyWord		类型名称
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionDisplayKeyWordDefToFp(char *keyWord,FILE *fp)
{
	return(UnionDisplayAllDefOfSpecKeyWordToFp(keyWord,fp));
}

/*
功能	
	显示一个类型的定义
输入参数
	keyWord		类型名称
	fileName	将一个类型定义写入到文件中
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputKeyWordDefToSpecFile(char *keyWord,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionOutputKeyWordDefToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionDisplayAllDefOfSpecKeyWordToFp(keyWord,fp)) < 0)
	{
		UnionUserErrLog("in UnionOutputKeyWordDefToSpecFile:: UnionDisplayAllDefOfSpecKeyWordToFp!\n");
	}
	fclose(fp);
	return(ret);
}

/*
功能	
	显示一个类型的所有定义
输入参数
	keyWord		类型名称
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionDisplayAllDefOfSpecKeyWordToFp(char *keyWord,FILE *fp)
{
	FILE			*indexFp;
	char			fileName[256+1];
	char			lineStr[1024+1];
	int			lineLen;
	int			ret;
	TUnionVarTypeDef	def;
	int			keyWordType;
	int			totalNum = 0;
	
	UnionGetDefaultFileNameOfVarTypeTagDef("",fileName);	
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	keyWordType = UnionConvertUnionKeyWordIntoTag(keyWord);
	while (!feof(indexFp))
	{
		memset(&def,0,sizeof(def));
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (keyWordType > 0)
		{
			if (keyWordType != def.typeTag)
				continue;
			if ((ret = UnionDisplayKeyWordDefOfSpecTypeToFp(def.typeTag,def.nameOfType,fp)) < 0)
			{
				UnionUserErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: UnionDisplayKeyWordDefOfSpecTypeToFp [%s]!\n",def.nameOfType);
				printf("[%s] not defined!\n",def.nameOfType);
				continue;
			}
			totalNum++;
		}
		else if (strcmp(keyWord,def.nameOfType) == 0)
		{
			if ((ret = UnionDisplayKeyWordDefOfSpecTypeToFp(def.typeTag,def.nameOfType,fp)) < 0)
			{
				UnionUserErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: UnionDisplayKeyWordDefOfSpecTypeToFp [%s]!\n",def.nameOfType);
				printf("[%s] not defined!\n",def.nameOfType);
				continue;
			}
			totalNum++;
			break;
		}
	}
	fclose(indexFp);
	return(0);
}	
	
