//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionSimpleTypeDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"

/*
功能	
	获得指定简单类型对应的c语言的类型
输入参数
	nameOfType	变量类型
输入出数
	无
返回值
	奕量类型的内部标识
*/
int UnionGetFinalTypeTagOfSpecNameOfSimpleType(char *nameOfType)
{
	char			finalNameOfType[256+1];
	int			ret;
	
	memset(finalNameOfType,0,sizeof(finalNameOfType));
	if ((ret = UnionGetFinalTypeNameOfSpecNameOfType(nameOfType,finalNameOfType)) < 0)
	{
		UnionUserErrLog("in UnionGetFinalTypeTagOfSpecNameOfSimpleType:: UnionGetFinalTypeNameOfSpecNameOfType! [%s]\n",nameOfType);
		return(ret);
	}
	return(UnionGetTypeTagOfSpecNameOfType(finalNameOfType));
}

/*
功能	
	获得指定简单类型对应的最终类型
输入参数
	oriNameOfType	变量类型
输入出数
	finalNameOfType	变量最终类型
返回值
	>= 0		成功
	<0		错误代码
*/
int UnionGetFinalTypeNameOfSpecNameOfSimpleType(char *oriNameOfType,char *finalNameOfType)
{
	TUnionSimpleTypeDef	def;
	char			fileName[256+1];
	int			ret;
	
	if (UnionGetTypeTagOfSpecNameOfType(oriNameOfType) != conVarTypeTagSimpleType)
	{
		strcpy(finalNameOfType,oriNameOfType);
		return(0);
	}
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSimpleTypeDefFromDefaultDefFile(oriNameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetFinalTypeNameOfSpecNameOfSimpleType:: UnionReadSimpleTypeDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionGetFinalTypeNameOfSpecNameOfType(def.nameOfType,finalNameOfType));
}

/*
功能	
	从一个定义口串中读取一个简单类型类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	pdef		简单类型类型定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadSimpleTypeDefFromStr(char *str,int lenOfStr,PUnionSimpleTypeDef pdef)
{
	int			ret;
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// 简单类型类型的类型定义
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conSimpleTypeDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conSimpleTypeDefTagNameOfType,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	// 简单类型类型名称
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conSimpleTypeDefTagVarName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conSimpleTypeDefTagVarName,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarNameNotDefined);
	}
	// 简单类型类型说明
	UnionReadRecFldFromRecStr(str,lenOfStr,conSimpleTypeDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}

/* 将指定简单类型的定义打印到文件中
输入参数
	pdef	简单类型定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSimpleTypeDefToFp(PUnionSimpleTypeDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (strlen(pdef->remark) != 0)
		fprintf(outFp,"// %s\n",pdef->remark);
	fprintf(outFp,"typedef %s %s;\n",pdef->nameOfType,pdef->name);
	return(0);
}

/* 将指定简单类型的定义以定义格式打印到文件中
输入参数
	pdef	简单类型定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSimpleTypeDefToFpInDefFormat(PUnionSimpleTypeDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	//if (strlen(pdef->remark) != 0)
	//	fprintf(outFp,"// %s\n",pdef->remark);
	fprintf(outFp,"%s=%s|%s=%s|%s=%s|\n",conSimpleTypeDefTagVarName,pdef->name,conSimpleTypeDefTagNameOfType,pdef->nameOfType,conSimpleTypeDefTagRemark,pdef->remark);
	return(0);
}

/* 将指定文件中定义的简单类型打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSimpleTypeDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionSimpleTypeDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSimpleTypeDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSimpleTypeDefInFileToFp:: UnionReadSimpleTypeDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintSimpleTypeDefToFp(&def,fp));
}
	
/* 将指定文件中定义的简单类型打印到屏幕上
输入参数
	pdef	简单类型定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSimpleTypeDefInFile(char *fileName)
{
	return(UnionPrintSimpleTypeDefInFileToFp(fileName,stdout));

}

/* 将指定名称的简单类型定义输出到文件中
输入参数
	nameOfType	简单类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecSimpleTypeDefToFp(char *nameOfType,FILE *fp)
{
	TUnionSimpleTypeDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSimpleTypeDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecSimpleTypeDefToFp:: UnionReadSimpleTypeDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintSimpleTypeDefToFp(&def,fp));
}
	
/* 将指定文件中定义的简单类型打印到屏幕上
输入参数
	nameOfType	简单类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecSimpleTypeDef(char *nameOfType)
{
	return(UnionPrintSpecSimpleTypeDefToFp(nameOfType,stdout));

}


