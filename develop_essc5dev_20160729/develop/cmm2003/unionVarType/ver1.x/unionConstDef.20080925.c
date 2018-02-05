// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionConstDefFile.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/*
功能	
	从缺省定义文件读取整数常量的值
输入参数
	constName	常量名称
输出参数
	constValue	常量值
返回值
	>=0		常量值
	<0		出错代码
*/
int UnionCalculateIntConstValue(char *constName)
{
	char	constValue[512+1];
	int	ret;
	
	if (UnionIsDigitStr(constName))
		return(atoi(constName));
	memset(constValue,0,sizeof(constValue));
	if ((ret = UnionReadConstValueFromDefaultDefFile(constName,"",constValue)) < 0)
	{
		UnionUserErrLog("in UnionCalculateIntConstValue:: UnionReadConstValueFromDefaultDefFile!\n");
		return(ret);
	}
	if (UnionIsDigitStr(constValue))
		return(atoi(constValue));
	else
	{
		UnionUserErrLog("in UnionCalculateIntConstValue:: [%s] not int const!\n",constName);
		return(errCodeCDPMDL_NotIntConst);
	}
}
	
/*
功能	
	从一个定义口串中读取一个常量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		常量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadConstDefFromStr(char *str,int lenOfStr,PUnionConstDef pdef)
{
	int	ret;
	char	type[100];
	
	memset(pdef,0,sizeof(*pdef));
	// 常量类型
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conConstDefTagName,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_ConstNameNotDefined);
	}
	// 常量值
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagValue,pdef->value,sizeof(pdef->value))) < 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conConstDefTagValue,str);
		return(ret);
	}
	// 常量类型
	memset(type,0,sizeof(type));
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagType,type,sizeof(type))) < 0)
		pdef->type = conVarTypeTagInt;
	else
		pdef->type = UnionConvertCVarDefTypeIntoTag(type);
	// 说明
	UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}

/* 将指定类型标识的定义打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintConstDefToFp(PUnionConstDef pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	switch (pdef->type)
	{
		case	conVarTypeTagInt:
			fprintf(outFp,"#define %s %s",pdef->name,pdef->value);
			break;
		case	conVarTypeTagChar:
			if (strlen(pdef->value) == 1)
				fprintf(outFp,"#define %s '%s'",pdef->name,pdef->value);
			else
				fprintf(outFp,"#define %s \"%s\"",pdef->name,pdef->value);
			break;
		case	conVarTypeTagEnum:
			fprintf(outFp,"        %s = %s,",pdef->name,pdef->value);
			break;
		default:
			UnionUserErrLog("in UnionPrintConstDefToFp:: invalid const type [%d]!\n",pdef->type);
			return(errCodeCDPMDL_NoValidConstType);
	}
	if (strlen(pdef->remark))
		fprintf(outFp,"  //%s\n",pdef->remark);
	else
		fprintf(outFp,"\n");
	return(0);
}

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintConstDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionConstDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadConstDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintConstDefInFileToFp:: UnionReadConstDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintConstDefToFp(&def,fp));
}
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputConstDefInFile(char *fileName)
{
	return(UnionPrintConstDefInFileToFp(fileName,stdout));

}

/* 从缺省定义文件读指定名称的常量的定义
输入参数
	nameOfConst	常量类型
输出参数
	pdef	读出的常量定义
返回值：
	>=0 	读出的常量的大小
	<0	出错代码	
	
*/
int UnionReadConstDefFromDefaultDefFile(char *nameOfConst,PUnionConstDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfConstDef(nameOfConst,fileName);
	if ((ret = UnionReadConstDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromDefaultDefFile:: UnionReadConstDefFromSpecFile [%s] from [%s]\n",nameOfConst,fileName);
		return(ret);
	}
	return(ret);
}

/* 将指定名称的常量定义输出到文件中
输入参数
	nameOfConst	常量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecConstDefToFp(char *nameOfConst,FILE *fp)
{
	TUnionConstDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadConstDefFromDefaultDefFile(nameOfConst,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecConstDefToFp:: UnionReadConstDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintConstDefToFp(&def,fp));
}
	
/* 将指定文件中定义的常量打印到屏幕上
输入参数
	nameOfConst	常量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecConstDef(char *nameOfConst)
{
	return(UnionPrintSpecConstDefToFp(nameOfConst,stdout));

}

