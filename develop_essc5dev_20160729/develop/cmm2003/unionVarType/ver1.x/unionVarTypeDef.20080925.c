// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/*
功能	
	从一个定义口串中读取一个变量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarTypeDefFromStr(char *str,int lenOfStr,PUnionVarTypeDef pdef)
{
	int	ret;
	char	typeTag[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// 变量类型
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarTypeDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarTypeDefTagNameOfType,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: nameOfType must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	memset(typeTag,0,sizeof(typeTag));
	// 变量标识
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarTypeDefTagTypeTag,typeTag,sizeof(typeTag))) < 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarTypeDefTagTypeTag,str);
		return(ret);
	}
	pdef->typeTag = UnionConvertCVarDefKeyWordIntoTag(typeTag);
	// 模块名称
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarTypeDefTagProgramName,pdef->nameOfProgram,sizeof(pdef->nameOfProgram))) < 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarTypeDefTagProgramName,str);
		return(ret);
	}
	return(0);
}

/*
功能	
	从一个定义口串中读取一个变量类型定义
输入参数
	pdef		变量定义
	sizeOfBuf	定义串的大小
输入出数
	str		定义串
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPutVarTypeDefIntoStr(PUnionVarTypeDef pdef,char *str,int sizeOfBuf)
{
	int	ret;
	char	keyWord[128+1];
	
	// 变量类型
	if ((ret = UnionPutRecFldIntoRecStr(conVarTypeDefTagNameOfType,pdef->nameOfType,strlen(pdef->nameOfType),str,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPutVarTypeDefIntoStr:: UnionPutRecFldIntoRecStr [%s] into [%s]!\n",conVarTypeDefTagNameOfType,str);
		return(ret);
	}
	memset(keyWord,0,strlen(keyWord));
	UnionConvertTypeTagIntoCVarDefKeyWord(pdef->typeTag,keyWord);
	// 变量标识
	if ((ret = UnionPutRecFldIntoRecStr(conVarTypeDefTagTypeTag,keyWord,strlen(keyWord),str,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPutVarTypeDefIntoStr:: UnionPutRecFldIntoRecStr [%s] into [%s]!\n",conVarTypeDefTagTypeTag,str);
		return(ret);
	}
	// 模块名称
	if ((ret = UnionPutRecFldIntoRecStr(conVarTypeDefTagProgramName,pdef->nameOfProgram,strlen(pdef->nameOfProgram),str,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPutVarTypeDefIntoStr:: UnionPutRecFldIntoRecStr [%s] into [%s]!\n",conVarTypeDefTagProgramName,str);
		return(ret);
	}
	return(0);
}

/*
功能	
	将一个科友内部关键字标识转换为标识值
输入参数
	keyWord		c言类型定义的关键字
输入出数
	无
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertUnionKeyWordIntoTag(char *keyWord)
{
	int	tag;
	
	if ((tag == UnionConvertCVarDefTypeIntoTag(keyWord)) > 0)
		return(tag);
	if (strcmp(keyWord,conDesginKeyWordTagStrPointer) == 0)
		return(conVarTypeTagPointer);
	else if (strcmp(keyWord,conDesginKeyWordTagStrStruct) == 0)
		return(conVarTypeTagStruct);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnion) == 0)
		return(conVarTypeTagUnion);
	else if (strcmp(keyWord,conDesginKeyWordTagStrArray) == 0)
		return(conVarTypeTagArray);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFun) == 0)
		return(conDesginKeyWordTagFun);
	else if (strcmp(keyWord,conDesginKeyWordTagStrConst) == 0)
		return(conDesginKeyWordTagConst);
	else if (strcmp(keyWord,conDesginKeyWordTagStrGlobalVar) == 0)
		return(conDesginKeyWordTagGlobalVar);
	else 
		return(errCodeCDPMDL_InvalidKeyWord);
}

/*
功能	
	将一个c言类型定义的关键字转化为科友内部标识
输入参数
	keyWord		c言类型定义的关键字
输入出数
	无
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertCVarDefTypeIntoTag(char *keyWord)
{
	if (strcmp(keyWord,conDesginKeyWordTagStrInt) == 0)
		return(conVarTypeTagInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrLong1) == 0))
		return(conVarTypeTagLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrChar) == 0)
		return(conVarTypeTagChar);
	else if (strcmp(keyWord,conDesginKeyWordTagStrDouble) == 0)
		return(conVarTypeTagDouble);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFloat) == 0)
		return(conVarTypeTagFloat);
	else if (strcmp(keyWord,conDesginKeyWordTagStrEnum) == 0)
		return(conVarTypeTagEnum);
	else if (strcmp(keyWord,conDesginKeyWordTagStrVoid) == 0)
		return(conVarTypeTagVoid);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrFile) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrFile1) == 0))
		return(conVarTypeTagFile);
	else if (strcmp(keyWord,conDesginKeyWordTagStrString) == 0)
		return(conVarTypeTagString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedString) == 0)
		return(conVarTypeTagUnsignedString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedInt) == 0)
		return(conVarTypeTagUnsignedInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong1) == 0))
		return(conVarTypeTagUnsignedLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedChar) == 0)
		return(conVarTypeTagUnsignedChar);
	else return(errCodeCDPMDL_NotBaseVarType);
}

/*
功能	
	判断一个类型是否是一个简单类型
输入参数
	nameOfType	变量类型
输入出数
	无
返回值
	1		是
	0		不是
*/
int UnionIsBaseType(char *nameOfType)
{
	return(UnionIsBaseTypeTag(UnionGetTypeTagOfSpecNameOfType(nameOfType)));
}

/*
功能	
	获得指定类型的变量的内部标识
输入参数
	nameOfType	变量类型
输入出数
	无
返回值
	奕量类型的内部标识
*/
int UnionGetTypeTagOfSpecNameOfType(char *nameOfType)
{
	TUnionVarTypeDef	typeTagDef;
	char			fileName[256+1];
	int			ret;
	int			typeTag;
	
	if ((typeTag = UnionConvertCVarDefTypeIntoTag(nameOfType)) != errCodeCDPMDL_NotBaseVarType)
		return(typeTag);
	// 读取变量类型的标识
	UnionGetDefaultFileNameOfVarTypeTagDef(nameOfType,fileName);
	memset(&typeTagDef,0,sizeof(typeTagDef));
	if ((ret = UnionReadVarTypeDefFromSpecFile(fileName,nameOfType,&typeTagDef)) < 0)
	{
		UnionUserErrLog("in UnionGetTypeTagOfSpecNameOfType:: UnionReadVarTypeDefFromSpecFile!\n");
		return(ret);
	}
	return(typeTagDef.typeTag);
}

/*
功能	
	获得指定类型的变量的最原始类型
输入参数
	oriNameOfType	变量类型
输入出数
	finalNameOfType	变量最终类型
返回值
	>= 0		成功
	<0		错误代码
*/
int UnionGetFinalTypeNameOfSpecNameOfType(char *oriNameOfType,char *finalNameOfType)
{
	TUnionVarTypeDef	typeTagDef;
	char			fileName[256+1];
	int			ret;
	int			typeTag;
	
	if ((typeTag = UnionConvertCVarDefTypeIntoTag(oriNameOfType)) != errCodeCDPMDL_NotBaseVarType)
	{
		strcpy(finalNameOfType,oriNameOfType);
		return(0);
	}
	// 读取变量类型的标识
	UnionGetDefaultFileNameOfVarTypeTagDef(oriNameOfType,fileName);
	memset(&typeTagDef,0,sizeof(typeTagDef));
	if ((ret = UnionReadVarTypeDefFromSpecFile(fileName,oriNameOfType,&typeTagDef)) < 0)
	{
		UnionUserErrLog("in UnionGetFinalTypeNameOfSpecNameOfType:: UnionReadVarTypeDefFromSpecFile [%s]!\n",oriNameOfType);
		return(ret);
	}
	if (typeTagDef.typeTag != conVarTypeTagSimpleType)
	{
		strcpy(finalNameOfType,typeTagDef.nameOfType);
		return(0);
	}
	return(UnionGetFinalTypeNameOfSpecNameOfSimpleType(typeTagDef.nameOfType,finalNameOfType));
}

/*
功能	
	判断是否是一个基本类型
输入参数
	keyTag		类型标识
输入出数
	无
返回值
	1		是
	0		不是
*/
int UnionIsBaseTypeTag(TUnionVarTypeTag keyTag)
{
	switch (keyTag)
	{
		case	conVarTypeTagInt:
		case	conVarTypeTagLong:
		case	conVarTypeTagChar:
		case	conVarTypeTagUnsignedInt:
		case	conVarTypeTagUnsignedLong:
		case	conVarTypeTagUnsignedChar:
		case	conVarTypeTagDouble:
		case	conVarTypeTagFloat:
		case	conVarTypeTagVoid:
		case	conVarTypeTagEnum:
		case	conVarTypeTagFile:
		case	conVarTypeTagString:
		case	conVarTypeTagUnsignedString:
			return(1);
		case	conVarTypeTagSimpleType:
		case	conVarTypeTagStruct:
		case	conVarTypeTagUnion:
		case	conVarTypeTagArray:
		case	conVarTypeTagPointer:
			return(0);
		default:
			return(errCodeCDPMDL_InvalidVarType);
	}
}

/*
功能	
	将一个科友内部标识转换为c言类型定义的关键字
输入参数
	keyTag		类型标识
输入出数
	keyWord		c言类型定义的关键字
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertTypeTagIntoCVarDefKeyWord(TUnionVarTypeTag keyTag,char *keyWord)
{
	switch (keyTag)
	{
		case	conVarTypeTagInt:
			strcpy(keyWord,conDesginKeyWordTagStrInt);
			return(0);
		case	conVarTypeTagLong:
			strcpy(keyWord,conDesginKeyWordTagStrLong);
			return(0);
		case	conVarTypeTagChar:
			strcpy(keyWord,conDesginKeyWordTagStrChar);
			return(0);
		case	conVarTypeTagUnsignedInt:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedInt);
			return(0);
		case	conVarTypeTagUnsignedLong:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedLong);
			return(0);
		case	conVarTypeTagUnsignedChar:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedChar);
			return(0);
		case	conVarTypeTagDouble:
			strcpy(keyWord,conDesginKeyWordTagStrDouble);
			return(0);
		case	conVarTypeTagFloat:
			strcpy(keyWord,conDesginKeyWordTagStrFloat);
			return(0);
		case	conVarTypeTagFile:
			strcpy(keyWord,conDesginKeyWordTagStrFile);
			return(0);
		case	conVarTypeTagString:
			strcpy(keyWord,conDesginKeyWordTagStrString);
			return(0);
		case	conVarTypeTagUnsignedString:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedString);
			return(0);
		case	conVarTypeTagEnum:
			strcpy(keyWord,conDesginKeyWordTagStrEnum);
			return(0);
		case	conVarTypeTagSimpleType:
			strcpy(keyWord,conDesginKeyWordTagStrSimpleType);
			return(0);
		case	conVarTypeTagStruct:
			strcpy(keyWord,conDesginKeyWordTagStrStruct);
			return(0);
		case	conVarTypeTagUnion:
			strcpy(keyWord,conDesginKeyWordTagStrUnion);
			return(0);
		case	conVarTypeTagArray:
			strcpy(keyWord,conDesginKeyWordTagStrArray);
			return(0);
		case	conVarTypeTagPointer:
			strcpy(keyWord,conDesginKeyWordTagStrPointer);
			return(0);
		case	conVarTypeTagVoid:
			strcpy(keyWord,conDesginKeyWordTagStrVoid);
			return(0);
		default:
			strcpy(keyWord,conDesginKeyWordTagStrVoid);
			return(0);
	}
}

/*
功能	
	将一个c言类型定义的关键字转化为科友内部标识
输入参数
	keyWord		c言类型定义的关键字
输入出数
	无
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertCVarDefKeyWordIntoTag(char *keyWord)
{
	if (strcmp(keyWord,conDesginKeyWordTagStrInt) == 0)
		return(conVarTypeTagInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrLong1) == 0))
		return(conVarTypeTagLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrChar) == 0)
		return(conVarTypeTagChar);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedInt) == 0)
		return(conVarTypeTagUnsignedInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong1) == 0))
		return(conVarTypeTagUnsignedLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedChar) == 0)
		return(conVarTypeTagUnsignedChar);
	else if (strcmp(keyWord,conDesginKeyWordTagStrDouble) == 0)
		return(conVarTypeTagDouble);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFloat) == 0)
		return(conVarTypeTagFloat);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrFile) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrFile) == 0))
		return(conVarTypeTagFile);
	else if (strcmp(keyWord,conDesginKeyWordTagStrString) == 0)
		return(conVarTypeTagString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedString) == 0)
		return(conVarTypeTagUnsignedString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrEnum) == 0)
		return(conVarTypeTagEnum);
	else if (strcmp(keyWord,conDesginKeyWordTagStrSimpleType) == 0)
		return(conVarTypeTagSimpleType);
	else if (strcmp(keyWord,conDesginKeyWordTagStrStruct) == 0)
		return(conVarTypeTagStruct);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnion) == 0)
		return(conVarTypeTagUnion);
	else if (strcmp(keyWord,conDesginKeyWordTagStrArray) == 0)
		return(conVarTypeTagArray);
	else if (strcmp(keyWord,conDesginKeyWordTagStrPointer) == 0)
		return(conVarTypeTagPointer);
	else if (strcmp(keyWord,conDesginKeyWordTagStrVoid) == 0)
		return(conVarTypeTagVoid);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFun) == 0)
		return(conDesginKeyWordTagFun);
	else if (strcmp(keyWord,conDesginKeyWordTagStrConst) == 0)
		return(conDesginKeyWordTagConst);
	else if (strcmp(keyWord,conDesginKeyWordTagStrGlobalVar) == 0)
		return(conDesginKeyWordTagGlobalVar);
	else return(conVarTypeTagVoid);
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
int UnionPrintVarTypeDefToFp(PUnionVarTypeDef pdef,FILE *fp)
{
	FILE	*outFp;
	char	keyWord[100+1];
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	memset(keyWord,0,sizeof(keyWord));
	UnionConvertTypeTagIntoCVarDefKeyWord(pdef->typeTag,keyWord);
	fprintf(outFp,"nameOfType=%s|typeTag=%s|nameOfProgram=%s|\n",pdef->nameOfType,keyWord,pdef->nameOfProgram);
	return(0);
}

/* 将指定类型标识的定义以定义格式打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintVarTypeDefToFpInDefFormat(PUnionVarTypeDef pdef,FILE *fp)
{
	return(UnionPrintVarTypeDefToFp(pdef,fp));
}

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
	nameOfType	类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintVarTypeDefInFileToFp(char *fileName,char *nameOfType,FILE *fp)
{
	TUnionVarTypeDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarTypeDefFromSpecFile(fileName,nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarTypeDefInFileToFp:: UnionReadVarTypeDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintVarTypeDefToFp(&def,fp));
}
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
	nameOfType	类型名称
输出参数
	无
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputVarTypeDefInFile(char *nameOfType,char *fileName)
{
	return(UnionPrintVarTypeDefInFileToFp(fileName,nameOfType,stdout));

}
