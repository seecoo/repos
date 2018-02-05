// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarDef.h"
#include "unionFunDef.h"
#include "UnionLog.h"

/*
功能	
	为一个函数产生一段调用代码
输入参数
	prefixBlankNum	调用代码前缀的空格数
	retVarName	返回值名称
	pdef		函数定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCallCodesOfFun(int prefixBlankNum,char *retVarName,PUnionFunDef pdef,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	for (index = 0; index < prefixBlankNum; index++)
		fprintf(outFp," ");
	if ((retVarName != NULL) && (strlen(retVarName) != 0))
		fprintf(outFp,"%s = ",retVarName);
	fprintf(outFp,"%s(",pdef->funName);
	for (index = 0; index < pdef->varNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s",pdef->varGrp[index].varDef.name);
	}
	fprintf(outFp,");\n");
	return(0);
}

/*
功能	
	打印一个函数定义的说明
输入参数
	pdef		要打印的定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunDefRemarkToFp(PUnionFunDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	int	varNum;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	fprintf(outFp,"/*\n");
	fprintf(outFp,"函数功能\n        %s\n",pdef->remark);
	fprintf(outFp,"输入参数\n");
	for (index = 0,varNum = 0; index < pdef->varNum; index++)
	{
		if (pdef->varGrp[index].isOutput)
			continue;
		fprintf(outFp,"       %s %s\n",pdef->varGrp[index].varDef.name,pdef->varGrp[index].varDef.remark);
		varNum++;
	}
	if (varNum == 0)
		fprintf(outFp,"       无\n");
	fprintf(outFp,"输出参数\n");
	for (index = 0,varNum = 0; index < pdef->varNum; index++)
	{
		if (!pdef->varGrp[index].isOutput)
			continue;
		fprintf(outFp,"       %s %s\n",pdef->varGrp[index].varDef.name,pdef->varGrp[index].varDef.remark);
		varNum++;
	}
	if (varNum == 0)
		fprintf(outFp,"       无\n");
	fprintf(outFp,"返回值\n");
	UnionLog("***[%s]\n",pdef->returnType.remark);
	fprintf(outFp,"       \n",pdef->returnType.remark);
	fprintf(outFp,"*/\n");
	return(0);
}

/*
功能	
	为一个函数的参数定义一组变量声明
输入参数
	prefixBlankNum			变量名前缀的空格数
	pdef				要打印的函数定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义	
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(int prefixBlankNum,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	index;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);

	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintVarDefToFpOfCProgramWithPrefixBlank(prefixBlankNum,&(pdef->varGrp[index].varDef),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank:: UnionPrintVarDefToFpOfCProgram! [%s]!\n",pdef->funName);
			return(ret);
		}
	}
	if ((ret = UnionPrintVarDefToFpOfCProgramWithPrefixBlank(prefixBlankNum,&(pdef->returnType),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank:: UnionPrintVarDefToFpOfCProgram! [%s]!\n",pdef->funName);
		return(ret);
	}
	return(0);
}

/*
功能	
	为一个函数的参数定义一组变量声明
输入参数
	pdef				要打印的函数定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义	
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunVarDefToFpOfCPragram(PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	return(UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(0,pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
}

/*
功能	
	为一个函数的参数定义一组变量声明,在变量名称前加上前缀
输入参数
	prefix				变量前缀
	pdef				要打印的函数定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义	
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunVarDefWithPrefixToFpOfCPragram(char *prefix,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	index;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((prefix == NULL) || (strlen(prefix) == 0))
		return(UnionPrintFunVarDefToFpOfCPragram(pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintVarDefWithPrefixToFpOfCProgram(prefix,&(pdef->varGrp[index].varDef),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintFunVarDefWithPrefixToFpOfCPragram:: UnionPrintVarDefWithPrefixToFpOfCProgram! [%s]!\n",pdef->funName);
			return(ret);
		}
	}
	if ((ret = UnionPrintVarDefWithPrefixToFpOfCProgram(prefix,&(pdef->returnType),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintFunVarDefWithPrefixToFpOfCPragram:: UnionPrintVarDefWithPrefixToFpOfCProgram! [%s]!\n",pdef->funName);
		return(ret);
	}
	return(0);
}

/*
功能	
	打印一个函数定义
输入参数
	pdef			要打印的函数定义
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
	isDeclaration		1，函数声明，0，函数体
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunDefToFp(PUnionFunDef pdef,int inCProgramFormat,int isDeclaration,FILE *fp)
{
	FILE	*outFp;
	int	index;
	int	varNum;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (inCProgramFormat)
	{
		UnionPrintFunDefRemarkToFp(pdef,outFp);	// 函数说明
		UnionPrintVarTypeDeclareCProgramFormatToFp(&(pdef->returnType),outFp);	// 返回值
		fprintf(outFp,"%s(",pdef->funName); // 函数名称
		for (index = 0; index < pdef->varNum; index++)
		{
			if (index != 0)
				fprintf(outFp,",");
			UnionPrintVarDefToFp(&(pdef->varGrp[index].varDef),inCProgramFormat,outFp);	// 参数
		}
		if (isDeclaration)
			fprintf(outFp,");\n");
		else
			fprintf(outFp,")\n");
	}
	else
	{
		fprintf(outFp,"funName=%s|remark=%s|",pdef->funName,pdef->remark); // 函数名称
		for (index = 0; index < pdef->varNum; index++)
		{
			fprintf(outFp,"varDef=");
			fprintf(outFp,"isOutput=%d|",pdef->varGrp[index].isOutput);
			UnionPrintVarDefToFp(&(pdef->varGrp[index].varDef),inCProgramFormat,outFp);	// 参数
			fprintf(outFp,"\n");
		}
		fprintf(outFp,"returnType=");
		UnionPrintVarDefToFp(&(pdef->returnType),inCProgramFormat,outFp);	// 返回值
		fprintf(outFp,"\n");
	}
	return(0);
}
	
/*
功能	
	打印一个函数定义
输入参数
	fileName		文件名称
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
	isDeclaration		1，函数声明，0，函数体
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunDefInFileDefToFp(char *fileName,int inCProgramFormat,int isDeclaration,FILE *fp)
{
	TUnionFunDef		def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadFunDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintFunDefInFileDefToFp:: UnionReadFunDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintFunDefToFp(&def,inCProgramFormat,isDeclaration,fp));
}

/*
功能	
	打印一个函数定义
输入参数
	fileName		文件名称
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
	isDeclaration		1，函数声明，0，函数体
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputFunDefInFileDef(char *fileName,int inCProgramFormat,int isDeclaration)
{
	return(UnionPrintFunDefInFileDefToFp(fileName,inCProgramFormat,isDeclaration,stdout));
}

/*
功能	
	打印缺省定义文件中的一个函数定义
输入参数
	funName			函数名称
	isDeclaration		1，函数声明，0，函数体
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputFunDefInDefaultDefFile(char *funName,int isDeclaration)
{
	TUnionFunDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionOutputFunDefInDefaultDefFile:: UnionReadFunDefFromDefaultDefFile [%s]\n",funName);
		return(ret);
	}
	return(UnionPrintFunDefToFp(&def,1,0,stdout));
}

/* 将指定名称的函数定义输出到文件中
输入参数
	funName	函数名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecFunDefToFp(char *funName,FILE *fp)
{
	TUnionFunDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecFunDefToFp:: UnionReadFunDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintFunDefToFp(&def,1,1,fp));
}
	
/* 将指定文件中定义的函数打印到屏幕上
输入参数
	funName	函数名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecFunDef(char *funName)
{
	return(UnionPrintSpecFunDefToFp(funName,stdout));

}

		
