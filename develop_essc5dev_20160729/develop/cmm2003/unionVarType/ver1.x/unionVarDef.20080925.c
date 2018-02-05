// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarDef.h"
#include "UnionLog.h"

/*
功能	
	从一个定义口串中读取一个变量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarDeclareTypeDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef)
{
	int	ret;
	
	memset(pdef,0,sizeof(*pdef));

	// 变量类型前缀
	UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagTypePrefix,pdef->typePrefix,sizeof(pdef->typePrefix));
	// 变量类型
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarDefTagNameOfType,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: nameOfType must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	// 指针标识
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conVarDefTagIsPointer,&(pdef->isPointer))) < 0)
		pdef->isPointer = 0;
	return(0);
}

/*
功能	
	从一个定义口串中读取一个变量定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef)
{
	int			ret;
	int			index;
	char			var[128];
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// 变量类型定义
	if ((ret = UnionReadVarDeclareTypeDefFromStr(str,lenOfStr,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: UnionReadVarDeclareTypeDefFromStr str = [%s]!\n",str);
		return(ret);
	}
	// 变量名称
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagVarName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: UnionReadRecFldFromRecStr [%s]! str = [%s]\n",conVarDefTagVarName,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarNameNotDefined);
	}
	// 变量说明
	UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagRemark,pdef->remark,sizeof(pdef->remark));
	// 数组的维度
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conVarDefTagDimisionNum,&(pdef->dimisionNum))) < 0)
		pdef->dimisionNum = 0;
	if (pdef->dimisionNum >= conMaxNumOfArrayDimision)
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: dimisionNum [%d] is out of range in [%s]!\n",pdef->dimisionNum,str);
		return(ret);
	}
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		sprintf(var,"%s%02d",conVarDefTagSizeOfDimision,index+1);
		UnionReadRecFldFromRecStr(str,lenOfStr,var,pdef->sizeOfDimision[index],sizeof(pdef->sizeOfDimision[index]));
	}
	return(0);
}

/*
功能	
	打印一个变量的类型定义
输入参数
	pdef			要打印的变量定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarTypeDeclareCProgramFormatToFp(PUnionVarDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (strlen(pdef->typePrefix) != 0)
		fprintf(outFp,"%s ",pdef->typePrefix);
	fprintf(outFp,"%s ",pdef->nameOfType);
	if (pdef->isPointer)
		fprintf(outFp,"*");
	return(0);
}

/*
功能	
	打印一个变量定义，如果数组变量的大小未定义，则使用缺省大小
输入参数
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefSetArraySizeToFp(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	UnionPrintVarTypeDeclareCProgramFormatToFp(pdef,outFp);
	if (strlen(pdef->name) != 0)
		fprintf(outFp,"%s",pdef->name);
	else if ((defaultName != NULL) && (strlen(defaultName) != 0))
		fprintf(outFp,"%s",defaultName);
	else
		fprintf(outFp,"unnamedVar");
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		if (strlen(pdef->sizeOfDimision[index]) > 0)
			fprintf(outFp,"[%s]",pdef->sizeOfDimision[index]);
		else
		{
			if (defaultArraySizeWhenSizeNotSet > 0)
				fprintf(outFp,"[%d]",defaultArraySizeWhenSizeNotSet);
			else
				fprintf(outFp,"[]");
		}
	}
	return(0);
}

/*
功能	
	打印一个变量定义，在变量名前面增加前缀，如果数组变量的大小未定义，则使用缺省大小
输入参数
	prefix				变量前缀
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefWithPrefixSetArraySizeToFp(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if ((prefix == NULL) || (strlen(prefix) == 0))
		return(UnionPrintVarDefSetArraySizeToFp(pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	UnionPrintVarTypeDeclareCProgramFormatToFp(pdef,outFp);
	if (strlen(pdef->name) != 0)
		fprintf(outFp,"%s%s",prefix,pdef->name);
	else if ((defaultName != NULL) && (strlen(defaultName) != 0))
		fprintf(outFp,"%s%s",prefix,defaultName);
	else
		fprintf(outFp,"%sunnamedVar",prefix);
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		if (strlen(pdef->sizeOfDimision[index]) > 0)
			fprintf(outFp,"[%s]",pdef->sizeOfDimision[index]);
		else
		{
			if (defaultArraySizeWhenSizeNotSet > 0)
				fprintf(outFp,"[%d]",defaultArraySizeWhenSizeNotSet);
			else
				fprintf(outFp,"[]");
		}
	}
	return(0);
}

/*
功能	
	打印一个变量定义
输入参数
	pdef			要打印的变量定义
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefToFp(PUnionVarDef pdef,int inCProgramFormat,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (inCProgramFormat)
	{
		return(UnionPrintVarDefSetArraySizeToFp(pdef,"",-1,outFp));
	}
	else
	{
		fprintf(outFp,"name=%s|nameOfType=%s|typePrefix=%s|isPointer=%d|dimisionNum=%d|remark=%s|",pdef->name,pdef->nameOfType,pdef->typePrefix,pdef->isPointer,pdef->dimisionNum,pdef->remark);
		for (index = 0; index < pdef->dimisionNum; index++)
			fprintf(outFp,"sizeOfDimision%02d=%s|",index+1,pdef->sizeOfDimision[index]);
	}
	
	return(0);
}

/*
功能	
	将函数的参数定义，以变量定义的形式打印到文件中
输入参数
	prefixBlankNum			变量名前缀的空格数
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefToFpOfCProgramWithPrefixBlank(int prefixBlankNum,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	for (index = 0; index < prefixBlankNum; index++)
		fprintf(outFp," ");
	// 定义函数变量
	if ((ret = UnionPrintVarDefSetArraySizeToFp(pdef,defaultName,defaultArraySizeWhenSizeNotSet,outFp)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarDefToFpOfCProgramWithPrefixBlank:: UnionPrintVarDefSetArraySizeToFp!\n");
		return(ret);
	}
	fprintf(outFp,";");
	if (strlen(pdef->remark) != 0)
		fprintf(outFp,"  // %s\n",pdef->remark);
	else
		fprintf(outFp,"\n");
	return(0);
}


/*
功能	
	将函数的参数定义，以变量定义的形式打印到文件中
输入参数
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefToFpOfCProgram(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	return(UnionPrintVarDefToFpOfCProgramWithPrefixBlank(0,pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
}

/*
功能	
	将函数的参数定义，以变量定义的形式打印到文件中，在变量的名称前加前缀
输入参数
	prefix				变量前缀
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefWithPrefixToFpOfCProgram(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	
	if ((prefix == NULL) || (strlen(prefix) == 0))
		return(UnionPrintVarDefToFpOfCProgram(pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
		
	// 定义函数变量
	if ((ret = UnionPrintVarDefWithPrefixSetArraySizeToFp(prefix,pdef,defaultName,defaultArraySizeWhenSizeNotSet,outFp)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarDefWithPrefixToFpOfCProgram:: UnionPrintVarDefWithPrefixSetArraySizeToFp!\n");
		return(ret);
	}
	fprintf(outFp,";");
	if (strlen(pdef->remark) != 0)
		fprintf(outFp,"  // %s\n",pdef->remark);
	else
		fprintf(outFp,"\n");
	return(0);
}
	
/*
功能	
	打印一个变量定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefInStrDefToFp(char *str,int lenOfStr,int inCProgramFormat,FILE *fp)
{
	TUnionVarDef	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarDefFromStr(str,lenOfStr,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarDefInStrDefToFp:: UnionReadVarDefFromStr!\n");
		return(ret);
	}
	return(UnionPrintVarDefToFp(&def,inCProgramFormat,fp));
}

/*
功能	
	打印一个变量定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputVarDefInStrDef(char *str,int lenOfStr,int inCProgramFormat)
{
	return(UnionPrintVarDefInStrDefToFp(str,lenOfStr,inCProgramFormat,stdout));
}
