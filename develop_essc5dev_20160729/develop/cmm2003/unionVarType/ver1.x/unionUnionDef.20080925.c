//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionUnionDef.h"
#include "UnionStr.h"

/* 从一个字符串中读取结构声明的定义
输入参数
	str		声明定义串
	lenOfStr	声明定义串的长度
输出参数
	pdeclareDef	读出的声明定义
返回值：
	>=0 	读出的声明数目
	<0	出错代码	
	
*/
int UnionReadUnionDeclareDefFromStr(char *str,int lenOfStr,PUnionUnionDeclareDef pdeclareDef)
{
	int	ret;

	if ((str == NULL) || (lenOfStr <= 0))
		return(0);
	memset(pdeclareDef,0,sizeof(*pdeclareDef));
	if (pdeclareDef == NULL)
	{
		UnionUserErrLog("in UnionReadUnionDeclareDefFromStr:: pdeclareDef is null!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,UnionDefTagTypeDefName,pdeclareDef->typeDefName,sizeof(pdeclareDef->typeDefName))) < 0)		
	{
		UnionUserErrLog("in UnionReadUnionDeclareDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",UnionDefTagTypeDefName,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,UnionDefTagUnionName,pdeclareDef->unionName,sizeof(pdeclareDef->unionName));
	UnionReadRecFldFromRecStr(str,lenOfStr,UnionDefTagRemark,pdeclareDef->remark,sizeof(pdeclareDef->remark));
	return(1);
}

/* 将指定结构的声明头打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintUnionHeaderDeclareDefToFp(PUnionUnionDeclareDef pdef,FILE *fp)
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
	fprintf(outFp,"typedef union");
	if (strlen(pdef->unionName) != 0)
		fprintf(outFp," %s",pdef->unionName);
	return(0);
}

/* 将指定结构的声明尾打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintUnionTailDeclareDefToFp(PUnionUnionDeclareDef pdef,FILE *fp)
{
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
		
	if (pdef == NULL)
		return(0);

	fprintf(outFp,"} %s;\n",pdef->typeDefName);
	//fprintf(outFp,"typedef %s *%s;\n",pdef->typeDefName,pdef->typeDefName);
	return(0);
}

/* 将指定结构的定义打印到文件中
输入参数
	pdef	结构定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintUnionDefToFp(PUnionUnionDef pdef,FILE *fp)
{
	int		fldNum;
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	UnionPrintUnionHeaderDeclareDefToFp(&(pdef->declareDef),outFp);
	if (pdef->fldNum <= 0)
	{
		fprintf(outFp," %s;\n",pdef->declareDef.typeDefName);
		return(0);
	}
	fprintf(outFp,"\n{\n");
	for (fldNum = 0; fldNum < pdef->fldNum; fldNum++)
	{
		fprintf(outFp,"        ");
		UnionPrintVarDefToFp(&(pdef->fldGrp[fldNum]),1,outFp);
		fprintf(outFp,";");
		if (strlen(pdef->fldGrp[fldNum].remark) != 0)
			fprintf(outFp,"  // %s",pdef->fldGrp[fldNum].remark);
		fprintf(outFp,"\n");
	}
	UnionPrintUnionTailDeclareDefToFp(&(pdef->declareDef),outFp);
}

/* 将指定文件中定义的结构打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintUnionDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionUnionDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadUnionDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintUnionDefInFileToFp:: UnionReadUnionDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintUnionDefToFp(&def,fp));
}
	
/* 将指定文件中定义的结构打印到屏幕上
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputUnionDefInFile(char *fileName)
{
	return(UnionPrintUnionDefInFileToFp(fileName,stdout));

}

/* 将指定名称的结构定义输出到文件中
输入参数
	nameOfType	结构名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecUnionDefToFp(char *nameOfType,FILE *fp)
{
	TUnionUnionDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadUnionDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecUnionDefToFp:: UnionReadUnionDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintUnionDefToFp(&def,fp));
}
	
/* 将指定文件中定义的结构打印到屏幕上
输入参数
	nameOfType	结构名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecUnionDef(char *nameOfType)
{
	return(UnionPrintSpecUnionDefToFp(nameOfType,stdout));

}

