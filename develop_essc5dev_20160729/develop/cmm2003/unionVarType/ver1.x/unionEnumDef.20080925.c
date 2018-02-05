//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionEnumDef.h"
#include "UnionStr.h"

/* 从一个字符串中读取枚举声明的定义
输入参数
	str		声明定义串
	lenOfStr	声明定义串的长度
输出参数
	pdeclareDef	读出的声明定义
返回值：
	>=0 	读出的声明数目
	<0	出错代码	
	
*/
int UnionReadEnumDeclareDefFromStr(char *str,int lenOfStr,PUnionEnumDeclareDef pdeclareDef)
{
	int	ret;

	if ((str == NULL) || (lenOfStr <= 0))
		return(0);
	memset(pdeclareDef,0,sizeof(*pdeclareDef));
	if (pdeclareDef == NULL)
	{
		UnionUserErrLog("in UnionReadEnumDeclareDefFromStr:: pdeclareDef is null!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conEnumDefTagTypeDefName,pdeclareDef->name,sizeof(pdeclareDef->name))) < 0)		
	{
		UnionUserErrLog("in UnionReadEnumDeclareDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",conEnumDefTagTypeDefName,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conEnumDefTagRemark,pdeclareDef->remark,sizeof(pdeclareDef->remark));
	return(1);
}

/* 将指定枚举的声明头打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintEnumHeaderDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp)
{
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
		
	if (pdef == NULL)
		return(0);

	if (strlen(pdef->remark) > 0)
		fprintf(outFp,"// %s\n", pdef->remark);
	fprintf(outFp,"typedef enum\n");
	fprintf(outFp,"{\n");
	return(0);
}

/* 将指定枚举的声明尾打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintEnumTailDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp)
{
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
		
	if (pdef == NULL)
		return(0);

	fprintf(outFp,"} %s;\n",pdef->name);
	//fprintf(outFp,"typedef %s *%s;\n",pdef->name,pdef->name);
	return(0);
}

/* 将指定枚举的定义打印到文件中
输入参数
	pdef	枚举定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintEnumDefToFp(PUnionEnumDef pdef,FILE *fp)
{
	int		constNum;
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	UnionPrintEnumHeaderDeclareDefToFp(&(pdef->declareDef),outFp);
	for (constNum = 0; constNum < pdef->constNum; constNum++)
	{
		fprintf(outFp,"        ");
		UnionPrintConstDefToFp(&(pdef->constGrp[constNum]),outFp);
		if (strlen(pdef->constGrp[constNum].remark) != 0)
			fprintf(outFp,"  // %s",pdef->constGrp[constNum].remark);
		fprintf(outFp,"\n");
	}
	UnionPrintEnumTailDeclareDefToFp(&(pdef->declareDef),outFp);
}

/* 将指定文件中定义的枚举打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintEnumDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionEnumDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadEnumDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintEnumDefInFileToFp:: UnionReadEnumDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintEnumDefToFp(&def,fp));
}
	
/* 将指定文件中定义的枚举打印到屏幕上
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputEnumDefInFile(char *fileName)
{
	return(UnionPrintEnumDefInFileToFp(fileName,stdout));

}

/* 将指定名称的枚举定义输出到文件中
输入参数
	nameOfType	枚举名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecEnumDefToFp(char *nameOfType,FILE *fp)
{
	TUnionEnumDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadEnumDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecEnumDefToFp:: UnionReadEnumDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintEnumDefToFp(&def,fp));
}
	
/* 将指定文件中定义的枚举打印到屏幕上
输入参数
	nameOfType	枚举名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecEnumDef(char *nameOfType)
{
	return(UnionPrintSpecEnumDefToFp(nameOfType,stdout));

}

