//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionStructDef.h"
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
int UnionReadStructDeclareDefFromStr(char *str,int lenOfStr,PUnionStructDeclareDef pdeclareDef)
{
	int	ret;

	if ((str == NULL) || (lenOfStr <= 0))
		return(0);
	memset(pdeclareDef,0,sizeof(*pdeclareDef));
	if (pdeclareDef == NULL)
	{
		UnionUserErrLog("in UnionReadStructDeclareDefFromStr:: pdeclareDef is null!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conStructDefTagTypeDefName,pdeclareDef->typeDefName,sizeof(pdeclareDef->typeDefName))) < 0)		
	{
		UnionUserErrLog("in UnionReadStructDeclareDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",conStructDefTagTypeDefName,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conStructDefTagStructName,pdeclareDef->structName,sizeof(pdeclareDef->structName));
	UnionReadRecFldFromRecStr(str,lenOfStr,conStructDefTagRemark,pdeclareDef->remark,sizeof(pdeclareDef->remark));
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
int UnionPrintStructHeaderDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp)
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
	fprintf(outFp,"typedef struct");
	if (strlen(pdef->structName) != 0)
		fprintf(outFp," %s",pdef->structName);
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
int UnionPrintStructTailDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp)
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
int UnionPrintStructDefToFp(PUnionStructDef pdef,FILE *fp)
{
	int		fldNum;
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	UnionPrintStructHeaderDeclareDefToFp(&(pdef->declareDef),outFp);
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
	UnionPrintStructTailDeclareDefToFp(&(pdef->declareDef),outFp);
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
int UnionPrintStructDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionStructDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadStructDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintStructDefInFileToFp:: UnionReadStructDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintStructDefToFp(&def,fp));
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
int UnionOutputStructDefInFile(char *fileName)
{
	return(UnionPrintStructDefInFileToFp(fileName,stdout));

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
int UnionPrintSpecStructDefToFp(char *nameOfType,FILE *fp)
{
	TUnionStructDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadStructDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecStructDefToFp:: UnionReadStructDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintStructDefToFp(&def,fp));
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
int UnionOutputSpecStructDef(char *nameOfType)
{
	return(UnionPrintSpecStructDefToFp(nameOfType,stdout));

}

/* 将指定结构的定义以定义格式打印到文件中
输入参数
	pdef	结构定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintStructDefToFpInDefFormat(PUnionStructDef pdef,FILE *fp)
{
	int		fldNum;
	FILE		*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;

	fprintf(outFp,"%s=%s|%s=%s|%s=%s|\n",conStructDefTagTypeDefName,pdef->declareDef.typeDefName,conStructDefTagStructName,pdef->declareDef.structName,conStructDefTagRemark,pdef->declareDef.remark);
	fprintf(outFp,"\n");
	for (fldNum = 0; fldNum < pdef->fldNum; fldNum++)
	{
		UnionPrintVarDefToFp(&(pdef->fldGrp[fldNum]),0,outFp);
		fprintf(outFp,"\n");
	}
	return(0);
}
