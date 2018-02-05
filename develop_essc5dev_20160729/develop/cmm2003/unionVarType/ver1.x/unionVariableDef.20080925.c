// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVariableDef.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/*
功能	
	从一个定义口串中读取一个全局变量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		全局变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVariableDefFromStr(char *str,int lenOfStr,PUnionVariableDef pdef)
{
	int	ret;
	char	type[100];
	
	memset(pdef,0,sizeof(*pdef));
	// 全局变量类型
	if ((ret = UnionReadVarDefFromStr(str,lenOfStr,&(pdef->varDef))) < 0)
	{
		UnionUserErrLog("in UnionReadVariableDefFromStr:: UnionReadVarDefFromStr from [%s]!\n",str);
		return(ret);
	}
	// 全局变量值
	UnionReadRecFldFromRecStr(str,lenOfStr,conVariableDefTagDefaultValue,pdef->defaultValue,sizeof(pdef->defaultValue));
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
int UnionPrintVariableDefToFp(PUnionVariableDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	if ((ret = UnionPrintVarDefToFp(&(pdef->varDef),1,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintVariableDefToFp:: UnionPrintVarDefToFp!\n");
		return(ret);
	}
	if (strlen(pdef->defaultValue) > 0)
	{
		switch (UnionConvertCVarDefTypeIntoTag(pdef->varDef.nameOfType))
		{
			case	conVarTypeTagChar:
				if (pdef->varDef.dimisionNum == 0)
					fprintf(outFp," = '%c'",pdef->defaultValue[0]);
				else
					fprintf(outFp," = \"%s\"",pdef->defaultValue);
				break;
			case	conVarTypeTagString:
				fprintf(outFp," = \"%s\"",pdef->defaultValue);
				break;
			case	conVarTypeTagInt:
			case	conVarTypeTagLong:
			case	conVarTypeTagDouble:
			case	conVarTypeTagFloat:
			case	conVarTypeTagFile:
			case	conVarTypeTagPointer:
			case	conVarTypeTagStruct:
			case	conVarTypeTagUnion:
				fprintf(outFp," = %s",pdef->defaultValue);
				break;
			case	conVarTypeTagSimpleType:
				fprintf(outFp," = %s",pdef->defaultValue);
				break;
			default:
				UnionUserErrLog("in UnionPrintVariableDefToFp:: invalid type [%d] having defaultValue [%s]\n",pdef->varDef.nameOfType,pdef->defaultValue);
				break;
		}
	}
	else
	{
		switch (UnionConvertCVarDefTypeIntoTag(pdef->varDef.nameOfType))
		{
			case	conVarTypeTagChar:
				if (pdef->varDef.dimisionNum > 0)
					fprintf(outFp," = \"\"");
				break;
			case	conVarTypeTagString:
				fprintf(outFp," = \"\"");
				break;
			default:
				break;
		}
	}		
	fprintf(outFp,";");
	if (strlen(pdef->varDef.remark) != 0)
		fprintf(outFp,"  // %s",pdef->varDef.remark);
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
int UnionPrintVariableDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionVariableDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVariableDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintVariableDefInFileToFp:: UnionReadVariableDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintVariableDefToFp(&def,fp));
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
int UnionOutputVariableDefInFile(char *fileName)
{
	return(UnionPrintVariableDefInFileToFp(fileName,stdout));

}

/* 将指定名称的全局变量定义输出到文件中
输入参数
	nameOfVariable	全局变量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecVariableDefToFp(char *nameOfVariable,FILE *fp)
{
	TUnionVariableDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVariableDefFromDefaultDefFile(nameOfVariable,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecVariableDefToFp:: UnionReadVariableDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintVariableDefToFp(&def,fp));
}
	
/* 将指定文件中定义的全局变量打印到屏幕上
输入参数
	nameOfVariable	全局变量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecVariableDef(char *nameOfVariable)
{
	return(UnionPrintSpecVariableDefToFp(nameOfVariable,stdout));

}

