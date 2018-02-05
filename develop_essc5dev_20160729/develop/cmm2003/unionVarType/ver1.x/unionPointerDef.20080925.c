//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionPointerDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"


/* 从缺省文件读指定名称的指针的定义
输入参数
	nameOfType	指针类型的名称
输出参数
	typeNameOfPointer	指针类型的类型
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionReadTypeOfPointerDefFromDefaultDefFile(char *nameOfType,char *typeNameOfPointer)
{
	TUnionPointerDef	def;
	int			ret;
	
	if ((ret = UnionReadPointerDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionReadTypeOfPointerDefFromDefaultDefFile:: UnionReadPointerDefFromDefaultDefFile!\n");
		return(ret);
	}
	strcpy(typeNameOfPointer,def.nameOfType);
	return(0);
}

/*
功能	
	从一个定义口串中读取一个指针类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	pdef		指针类型定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadPointerDefFromStr(char *str,int lenOfStr,PUnionPointerDef pdef)
{
	int			ret;
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadPointerDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// 指针类型的类型定义
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conPointerDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadPointerDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPointerDefTagNameOfType);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	// 指针类型名称
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conPointerDefTagVarName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadPointerDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPointerDefTagVarName);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarNameNotDefined);
	}
	// 指针类型说明
	UnionReadRecFldFromRecStr(str,lenOfStr,conPointerDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}

/* 将指定指针的定义打印到文件中
输入参数
	pdef	指针定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintPointerDefToFp(PUnionPointerDef pdef,FILE *fp)
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
	fprintf(outFp,"typedef %s *%s;\n",pdef->nameOfType,pdef->name);
	return(0);
}

/* 将指定指针的定义打印到定义文件中
输入参数
	pdef	指针定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintPointerDefToFpInDefFormat(PUnionPointerDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	fprintf(outFp,"%s=%s|%s=%s|%s=%s|\n",conPointerDefTagVarName,pdef->name,conPointerDefTagNameOfType,pdef->nameOfType,conPointerDefTagRemark,pdef->remark);
	return(0);
}

/* 将指定文件中定义的指针打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintPointerDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionPointerDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadPointerDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintPointerDefInFileToFp:: UnionReadPointerDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintPointerDefToFp(&def,fp));
}
	
/* 将指定文件中定义的指针打印到屏幕上
输入参数
	pdef	指针定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputPointerDefInFile(char *fileName)
{
	return(UnionPrintPointerDefInFileToFp(fileName,stdout));

}

/* 将指定名称的指针类型定义输出到文件中
输入参数
	nameOfType	指针类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecPointerDefToFp(char *nameOfType,FILE *fp)
{
	TUnionPointerDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadPointerDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecPointerDefToFp:: UnionReadPointerDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintPointerDefToFp(&def,fp));
}
	
/* 将指定文件中定义的指针类型打印到屏幕上
输入参数
	nameOfType	指针类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecPointerDef(char *nameOfType)
{
	return(UnionPrintSpecPointerDefToFp(nameOfType,stdout));

}

