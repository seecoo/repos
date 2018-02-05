//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarValue.h"
#include "unionPointerDef.h"
#include "unionArrayDef.h"
#include "unionStructDef.h"
#include "UnionStr.h"
#include "unionVarTypeDef.h"
#include "unionFunDef.h"

// 定义一个控制将变量显示到文件中的格式
int gunionTagOfPrintValueTagListToFile = conTagOfPrintValueTagListToFilePrintToFile;

// 定义一个变量，控制未定义数组大小时，数组的缺省大小
int gunionDefaultArraySizeWhenNoSizeSet = 0;

// 设置将变量的取值清单打印到文件中时，打印的内容
void UnionSetTagOfPrintValueTagListToFile(int tag)
{
	gunionTagOfPrintValueTagListToFile = tag;
}

// 设置数组的缺省大小
void UnionSetDefaultArraySizeWhenNoSizeSet(int size)
{
	gunionDefaultArraySizeWhenNoSizeSet = size;
}

/*
功能	
	从文件读取一个指定取值标识的变量的值
输入参数
	fileName	文件名称
输出参数
	pdef		读出的函数定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadValueOfSpecValueTagDefFromSpecFile(char *fileName,PUnionVarValueTagDef pdef,char *value,int sizeOfValue)
{
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			ret;
	char			varValueTag[128+1];
	int			lenOfVarValueTag;
	
	if ((pdef == NULL) || (fileName == NULL))
	{
		UnionUserErrLog("in UnionReadValueOfSpecValueTagDefFromSpecFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// 打开定义文件
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadValueOfSpecValueTagDefFromSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// 从文件中读取定义
	sprintf(varValueTag,"%s=",pdef->varValueTag);
	lenOfVarValueTag = strlen(varValueTag);
	while (!feof(fp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeEnviMDL_NullLine) && (lineLen != errCodeFileEnd))
		{
			UnionUserErrLog("in UnionReadValueOfSpecValueTagDefFromSpecFile:: UnionReadOneDataLineFromTxtFile [%s] ! ret = [%04d]\n",fileName,lineLen);
			continue;
		}
		if (strncmp(lineStr,varValueTag,lenOfVarValueTag) != 0)
			continue;
		if (sizeOfValue <= lineLen - lenOfVarValueTag)
		{
			UnionUserErrLog("in UnionReadValueOfSpecValueTagDefFromSpecFile:: sizeOfBuf [%d] < expected [%d]\n",sizeOfValue,lineLen - lenOfVarValueTag);
			ret = errCodeSmallBuffer;
		}
		else
		{
			ret = lineLen - lenOfVarValueTag;
			memcpy(value,lineStr+lenOfVarValueTag,ret);
		}
		fclose(fp);
		return(ret);
	}
	fclose(fp);
	return(errCodeCDPMDL_NoValueForSpecVarValueTagDefined);
}

/*
功能	
	从一个定义口串中读取一个变量取值标识定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarValueTagDefFromStr(char *str,int lenOfStr,PUnionVarValueTagDef pdef)
{
	int			ret;
	int			index;
	char			var[128];
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadVarValueTagDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// 变量取值标志
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarValueTagValueTag,pdef->varValueTag,sizeof(pdef->varValueTag))) < 0)
	{
		UnionUserErrLog("in UnionReadVarValueTagDefFromStr:: UnionReadRecFldFromRecStr [%s]! str = [%s]\n",conVarValueTagValueTag,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: valueTag must be defined!\n");
		return(errCodeCDPMDL_VarValueTagNotDefined);
	}
	// 变量的类型标志
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conVarValueTagBaseTypeTag,&(pdef->baseTypeTag))) < 0)
	{
		UnionUserErrLog("in UnionReadVarValueTagDefFromStr:: UnionReadRecFldFromRecStr [%s]! str = [%s]\n",conVarValueTagBaseTypeTag,str);
		return(ret);
	}
	return(0);
}

/*
功能	
	将一个简单变量取值写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintValueTagListToFile(char *varNamePrefix,PUnionVarDef pdef,FILE *fp)
{
	int			ret;
	char			varName[128+1];
	int			offset = 0;
	
	if (pdef == NULL)
		return(errCodeParameter);
		
	// 拼装变量对应的值的名称
	memset(varName,0,sizeof(varName));
	/*
	if (pdef->isPointer)
	{
		sprintf(varName,"*");
		offset++;
	}
	*/
	if ((varNamePrefix != NULL) && (strlen(varNamePrefix) > 0))
	{
		sprintf(varName+offset,"%s",varNamePrefix);
		offset += strlen(varNamePrefix);
	}
	sprintf(varName+offset,"%s",pdef->name);
	return(UnionPrintArrayVarValueTagListToFile(varName,pdef,0,fp));
}

/*
功能	
	将一个数组变量取值写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintArrayVarValueTagListToFile(char *varNamePrefix,PUnionVarDef pdef,int dimisionIndex,FILE *fp)
{
	int	index;
	int	dimisionSize = 0;
	char	arrayTag[128+1];
	int	ret;
	int	isBaseType;
	
	if (pdef == NULL)
		return(errCodeParameter);	
	if ((dimisionIndex >= pdef->dimisionNum) || (pdef->dimisionNum <= 0))	// 数组遍历完了
	{
		if ((isBaseType = UnionIsBaseType(pdef->nameOfType)) > 0)	// 是基础类型
			return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,pdef->isPointer,UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType),fp));
		// 复杂变量类型
		switch (UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType))
		{
			case	conVarTypeTagStruct:	// 结构类型
				return(UnionPrintStructTypeValueTagListToFile(varNamePrefix,pdef->isPointer,pdef->nameOfType,fp));
			case	conVarTypeTagSimpleType:	// 简单类型
				return(UnionPrintSimpleTypeValueTagListToFile(varNamePrefix,pdef->isPointer,pdef->nameOfType,fp));
			case	conVarTypeTagPointer:	// 指针类型
				return(UnionPrintPointerTypeValueTagListToFile(varNamePrefix,pdef->isPointer,pdef->nameOfType,fp));
			case	conVarTypeTagArray:	// 数组类型
				return(UnionPrintArrayTypeValueTagListToFile(varNamePrefix,pdef->isPointer,pdef->nameOfType,0,fp));
			default:
				UnionUserErrLog("in UnionPrintArrayVarValueTagListToFile:: [%s] is not valid type!\n",pdef->nameOfType);
				return(errCodeCDPMDL_InvalidVarType);
		}	
	}
	// 是字符串数组
	if ((UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType) == conVarTypeTagChar) && (dimisionIndex + 1 == pdef->dimisionNum))
	{
		return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,pdef->isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrString),fp));
	}
	if ((UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType) == conVarTypeTagUnsignedChar)&& (dimisionIndex + 1 == pdef->dimisionNum))
	{
		return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,pdef->isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrUnsignedString),fp));
	}
	
	if (strlen(pdef->sizeOfDimision[dimisionIndex]) != 0)	// 定义了数组大小
		dimisionSize = UnionCalculateIntConstValue(pdef->sizeOfDimision[dimisionIndex]);
	else
		dimisionSize = gunionDefaultArraySizeWhenNoSizeSet;			
	if (dimisionSize > 0)
	{
		for (index = 0; index < dimisionSize; index++)
		{
			sprintf(arrayTag,"%s[%d]",varNamePrefix,index);
			if ((ret = UnionPrintArrayVarValueTagListToFile(arrayTag,pdef,dimisionIndex+1,fp)) < 0)
			{
				UnionUserErrLog("in UnionPrintArrayVarValueTagListToFile:: UnionPrintArrayVarValueTagListToFile!\n");
				return(ret);
			}
		}
	}
	else
	{
		sprintf(arrayTag,"%s[*]",varNamePrefix);
		if ((ret = UnionPrintArrayVarValueTagListToFile(arrayTag,pdef,dimisionIndex+1,fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintArrayVarValueTagListToFile:: UnionPrintArrayVarValueTagListToFile!\n");
			return(ret);
		}
	}
	
	return(0);
}	

/*
功能	
	将一个数组变量取值写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	isPointer	指针标识
	nameOfType	变量的类型
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintArrayTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,int dimisionIndex,FILE *fp)
{
	int		index;
	int		dimisionSize = 0;
	char		arrayTag[128+1];
	int		ret;
	int		isBaseType;
	TUnionArrayDef	def;
	
	if (nameOfType == NULL)
		return(errCodeParameter);	
	if ((ret = UnionReadArrayDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintArrayTypeValueTagListToFile:: UnionReadArrayDefFromDefaultDefFile [%s]!\n",nameOfType);
		return(ret);
	}
	
		//UnionLog("**** in UnionPrintArrayTypeValueTagListToFile:: varNamePrefix = [%s] isPointer = [%d]\n",varNamePrefix,isPointer+1);
	if ((dimisionIndex >= def.dimisionNum) || (def.dimisionNum <= 0))	// 数组遍历完了
	{
		if ((isBaseType = UnionIsBaseType(def.nameOfType)) > 0)	// 是基础类型
			return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(def.nameOfType),fp));
		// 复杂变量类型
		switch (UnionGetTypeTagOfSpecNameOfType(def.nameOfType))
		{
			case	conVarTypeTagStruct:	// 结构类型
				return(UnionPrintStructTypeValueTagListToFile(varNamePrefix,isPointer,def.nameOfType,fp));
			case	conVarTypeTagSimpleType:	// 简单类型
				return(UnionPrintSimpleTypeValueTagListToFile(varNamePrefix,isPointer,def.nameOfType,fp));
			case	conVarTypeTagPointer:	// 指针类型
				return(UnionPrintPointerTypeValueTagListToFile(varNamePrefix,isPointer,def.nameOfType,fp));
			default:
				UnionUserErrLog("in UnionPrintArrayTypeValueTagListToFile:: [%s] is not valid type!\n",def.nameOfType);
				return(errCodeCDPMDL_InvalidVarType);
		}	
	}
	// 是字符串数组
	if ((UnionGetTypeTagOfSpecNameOfType(def.nameOfType) == conVarTypeTagChar) && (dimisionIndex + 1 == def.dimisionNum))
	{
		return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrString),fp));
	}
	if ((UnionGetTypeTagOfSpecNameOfType(def.nameOfType) == conVarTypeTagUnsignedChar)&& (dimisionIndex + 1 == def.dimisionNum))
	{
		return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrUnsignedString),fp));
	}
	
	if (strlen(def.sizeOfDimision[dimisionIndex]) != 0)	// 定义了数组大小
		dimisionSize = UnionCalculateIntConstValue(def.sizeOfDimision[dimisionIndex]);
	else
		dimisionSize = gunionDefaultArraySizeWhenNoSizeSet;			
	if (dimisionSize > 0)
	{
		for (index = 0; index < dimisionSize; index++)
		{
			sprintf(arrayTag,"%s[%d]",varNamePrefix,index);
			if ((ret = UnionPrintArrayTypeValueTagListToFile(arrayTag,isPointer,nameOfType,dimisionIndex+1,fp)) < 0)
			{
				UnionUserErrLog("in UnionPrintArrayTypeValueTagListToFile:: UnionPrintArrayTypeValueTagListToFile!\n");
				return(ret);
			}
		}
		return(0);
	}
	else
	{
		UnionUserErrLog("in UnionPrintArrayTypeValueTagListToFile:: size of array %s not set for dimision index %d!\n",nameOfType,dimisionIndex);
		return(errCodeCDPMDL_ArraySizeNotSet);
	}
}	

/*
功能	
	将一个简单c语言类型取值方法写入到指定文件中
输入参数
	varName		变量名称
	typeTag		类型
	isPointer	指标标识，1表示是指针，0表示不是指针
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintCProgramSimpleTypeValueTagListToFile(char *varName,int isPointer,int typeTag,FILE *fp)
{
	int			ret;
	FILE			*outFp = stdout;
	char			nameOfType[64+1];
	int			index;
	char			pointerTag[10];

	//UnionLog("*** %d %s %d %d\n",gunionTagOfPrintValueTagListToFile,varName,isPointer,typeTag);	
	if (fp != NULL)
		outFp = fp;
	switch (gunionTagOfPrintValueTagListToFile)
	{
		case	conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile:	// 将分配指针变量空间的语句打印到文件中
			if (!isPointer)
				return(0);
			// 是指针
			memset(nameOfType,0,sizeof(nameOfType));
			if ((ret = UnionConvertTypeTagIntoCVarDefKeyWord(typeTag,nameOfType)) < 0)
			{
				UnionUserErrLog("in UnionPrintCProgramSimpleTypeValueTagListToFile:: UnionConvertTypeTagIntoCVarDefKeyWord [%d]\n",typeTag);
				return(ret);
			}
			if (strcmp(nameOfType,"file") == 0)	// 是文件指针，不分配
			{
				fprintf(outFp,"        %s = stdout;\n",varName);
				return(0);
			}
			//printf("nameOfType = [%s] of var [%s]\n",nameOfType,varName);
			if (strcmp(nameOfType,"enum") == 0)
				strcpy(nameOfType,"int");
			if ((strcmp(nameOfType,"char") == 0) || (strcmp(nameOfType,"unsigned char") == 0))	// 字符串
			{
				fprintf(outFp,"        %s = (%s *)malloc(sizeof(%s)*%d);\n",varName,nameOfType,nameOfType,conMaxDefaultSizeOfVarString);
				fprintf(outFp,"        memset(%s,0,%d);\n",varName,conMaxDefaultSizeOfVarString);
			}
			else if (strcmp(nameOfType,"void") == 0)
			{
				fprintf(outFp,"        %s = (%s *)malloc(sizeof(%s)*%d);\n",varName,nameOfType,"char",conMaxDefaultSizeOfVarString);
				fprintf(outFp,"        memset((char *)%s,0,%d);\n",varName,conMaxDefaultSizeOfVarString);
			}
			else
			{
				fprintf(outFp,"        %s = (%s *)malloc(sizeof(%s));\n",varName,nameOfType,nameOfType);
				fprintf(outFp,"        memset(%s,0,sizeof(%s));\n",varName,nameOfType);
			}				
			return(0);
		case	conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile:	// 将释放指针变量空间的语句打印到文件中
			if (!isPointer)
				return(0);
			if (typeTag == conVarTypeTagFile)
				return(0);
			// 是指针
			fprintf(outFp,"        free(%s);\n",varName);
			return(0);
		default:	// 将变量的取值清单打印到文件中
			memset(pointerTag,0,sizeof(pointerTag));
			for (index = 0; index < isPointer; index++)
			{
				if (index == isPointer - 1)
				{
					if (typeTag == conVarTypeTagChar)
					{
						typeTag = conVarTypeTagString;
						continue;
					}
					else if (typeTag == conVarTypeTagUnsignedChar)
					{
						typeTag = conVarTypeTagUnsignedString;
						continue;
					}
				}
				memcpy(pointerTag+index,"*",1);
			}
			fprintf(outFp,"%s=%d|%s=%s%s|\n",conVarValueTagBaseTypeTag,typeTag,conVarValueTagValueTag,pointerTag,varName);
			return(0);
	}	
}

/*
功能	
	将一个结构取值方法写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	isPointer	指明变量是否是指针
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintStructTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp)
{
	int			ret;
	TUnionStructDef		structDef;
	int			varIndex;
	char			varName[128+1];
	FILE			*outFp;
	
	if (nameOfType == NULL)
		return(errCodeParameter);
	if (isPointer)
		sprintf(varName,"%s->",varNamePrefix);
	else
		sprintf(varName,"%s.",varNamePrefix);
	
	if (fp != NULL)
		outFp = fp;	

	// 读结构定义	
	memset(&structDef,0,sizeof(structDef));
	if ((ret = UnionReadStructDefFromDefaultDefFile(nameOfType,&structDef)) < 0)
	{
		UnionUserErrLog("in UnionPrintStructTypeValueTagListToFile:: UnionReadStructDefFromDefaultDefFile [%s]!\n",nameOfType);
		return(ret);
	}
		
	if (isPointer)
	{
		switch (gunionTagOfPrintValueTagListToFile)
		{
			case	conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile:	// 将分配指针变量空间的语句打印到文件中
				if (strcmp(structDef.declareDef.typeDefName,nameOfType) == 0)
					fprintf(outFp,"        %s = (%s *)malloc(sizeof(%s));\n",varNamePrefix,nameOfType,nameOfType);
				else
					fprintf(outFp,"        %s = (struct %s *)malloc(sizeof(struct %s));\n",varNamePrefix,nameOfType,nameOfType);
				fprintf(outFp,"        memset(%s,0,sizeof(*%s));\n",varNamePrefix,varNamePrefix);
				break;
			default:
				break;
		}
	}
			
	for (varIndex = 0; varIndex < structDef.fldNum; varIndex++)
	{
		if ((ret = UnionPrintValueTagListToFile(varName,&(structDef.fldGrp[varIndex]),fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintStructTypeValueTagListToFile:: UnionPrintValueTagListToFile [%s]!\n",nameOfType);
			return(ret);
		}
	}
	if (!isPointer)
		return(0);
	switch (gunionTagOfPrintValueTagListToFile)
	{
		case	conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile:	// 将释放指针变量空间的语句打印到文件中
			// 是指针
			fprintf(outFp,"        free(%s);\n",varNamePrefix);
			break;
		default:
			break;
	}
	return(0);
}

/*
功能	
	将一个简单类型取值方法写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSimpleTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp)
{
	int			ret;
	int			typeTag;
	char			finalNameOfType[128+1];
	
	memset(finalNameOfType,0,sizeof(finalNameOfType));
	if ((ret = UnionGetFinalTypeNameOfSpecNameOfType(nameOfType,finalNameOfType)) < 0)
	{
		UnionUserErrLog("in UnionPrintSimpleTypeValueTagListToFile:: UnionGetFinalTypeNameOfSpecNameOfType [%s]!\n",nameOfType);
		return(ret);
	}
	if ((typeTag = UnionIsBaseType(finalNameOfType)) > 0)	// 是基础类型
		return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(finalNameOfType),fp));
	// 复杂变量类型
	switch (UnionGetTypeTagOfSpecNameOfType(finalNameOfType))
	{
		case	conVarTypeTagStruct:	// 结构类型
			return(UnionPrintStructTypeValueTagListToFile(varNamePrefix,isPointer,finalNameOfType,fp));
		case	conVarTypeTagSimpleType:	// 简单类型
			return(UnionPrintSimpleTypeValueTagListToFile(varNamePrefix,isPointer,finalNameOfType,fp));
		case	conVarTypeTagPointer:	// 指针类型
			return(UnionPrintPointerTypeValueTagListToFile(varNamePrefix,isPointer,finalNameOfType,fp));
		case	conVarTypeTagArray:	// 数组类型
			return(UnionPrintArrayTypeValueTagListToFile(varNamePrefix,isPointer,finalNameOfType,0,fp));
		default:
			UnionUserErrLog("in UnionPrintArrayVarValueTagListToFile:: [%s] is not valid type!\n",finalNameOfType);
			return(errCodeCDPMDL_InvalidVarType);
	}	
}

/*
功能	
	将一个指针类型取值方法写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintPointerTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp)
{
	int			ret;
	int			typeTag;
	char			typeNameOfPointer[128+1];
	char			varName[128+1];
		
	memset(typeNameOfPointer,0,sizeof(typeNameOfPointer));
	if ((ret = UnionReadTypeOfPointerDefFromDefaultDefFile(nameOfType,typeNameOfPointer)) < 0)
	{
		UnionUserErrLog("in UnionPrintPointerTypeValueTagListToFile:: UnionReadTypeOfPointerDefFromDefaultDefFile [%s]!\n",nameOfType);
		return(ret);
	}
	if (UnionIsBaseType(typeNameOfPointer) > 0)	// 是基础类型
	{
		if (isPointer)
			sprintf(varName,"*%s",varNamePrefix);
		else
			sprintf(varName,"%s",varNamePrefix);
		return(UnionPrintCProgramSimpleTypeValueTagListToFile(varName,1,UnionGetTypeTagOfSpecNameOfType(typeNameOfPointer),fp));
		//return(UnionPrintCProgramSimpleTypeValueTagListToFile(varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(typeNameOfPointer),fp));
	}

	// 复杂变量类型
	switch (UnionGetTypeTagOfSpecNameOfType(typeNameOfPointer))
	{
		case	conVarTypeTagStruct:	// 结构类型
			return(UnionPrintStructTypeValueTagListToFile(varNamePrefix,1,typeNameOfPointer,fp));
		case	conVarTypeTagSimpleType:	// 简单类型
			if (isPointer)
				sprintf(varName,"*%s",varNamePrefix);
			else
				sprintf(varName,"%s",varNamePrefix);			
			return(UnionPrintSimpleTypeValueTagListToFile(varName,1,typeNameOfPointer,fp));
		case	conVarTypeTagPointer:	// 指针类型
			if (isPointer)
				sprintf(varName,"*%s",varNamePrefix);
			else
				sprintf(varName,"%s",varNamePrefix);			
			return(UnionPrintPointerTypeValueTagListToFile(varName,1,typeNameOfPointer,fp));
		default:
			UnionUserErrLog("in UnionPrintPointerTypeValueTagListToFile:: [%s] is invalid type!\n",typeNameOfPointer);
			return(errCodeCDPMDL_InvalidVarType);
	}
}	

/*
功能	
	打印一个变量定义的赋值列表
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintValueTagListDefinedInStrToFp(char *str,int lenOfStr,FILE *fp)
{
	TUnionVarDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarDefFromStr(str,lenOfStr,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintValueTagListDefinedInStrToFp:: UnionReadVarDefFromStr!\n");
		return(ret);
	}
	return(UnionPrintValueTagListToFile("",&def,fp));
}

/*
功能	
	打印一个变量定义的赋值列表
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputValueTagListDefinedInStr(char *str,int lenOfStr)
{
	return(UnionPrintValueTagListDefinedInStrToFp(str,lenOfStr,stdout));
}


/*
功能	
	读一个变量的指定域的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
输出参数	
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,PUnionVarDef pdef)
{
	int			ret;
	char			varName[128+1];
	int			offset = 0;
	
	if (pdef == NULL)
		return(errCodeParameter);
		
	// 拼装变量对应的值的名称
	memset(varName,0,sizeof(varName));
	if ((varNamePrefix != NULL) && (strlen(varNamePrefix) > 0))
	{
		sprintf(varName+offset,"%s",varNamePrefix);
		offset += strlen(varNamePrefix);
	}
	sprintf(varName+offset,"%s",pdef->name);
	if ((ret = UnionReadArrayVarTypeTagOfSpecFldOfVar(specFldOfVar,varName,pdef,0)) < 0)
	{
		//UnionUserErrLog("in UnionReadTypeTagOfSpecFldOfVar:: varName = [%s] not defined in varDef [%s]! ret = [%d]\n",varName,pdef->name,ret);
		return(ret);
	}
	return(ret);
}

/*
功能	
	读一个数组变量取值的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
输出参数	
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadArrayVarTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,PUnionVarDef pdef,int dimisionIndex)
{
	int	index;
	int	dimisionSize = 0;
	char	arrayTag[128+1];
	int	ret;
	int	isBaseType;
	
	if (pdef == NULL)
		return(errCodeParameter);	
	if ((dimisionIndex >= pdef->dimisionNum) || (pdef->dimisionNum <= 0))	// 数组遍历完了
	{
		if ((isBaseType = UnionIsBaseType(pdef->nameOfType)) > 0)	// 是基础类型
			return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,pdef->isPointer,UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType)));
		// 复杂变量类型
		switch (UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType))
		{
			case	conVarTypeTagStruct:	// 结构类型
				return(UnionReadStructTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,pdef->isPointer,pdef->nameOfType));
			case	conVarTypeTagSimpleType:	// 简单类型
				return(UnionReadSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,pdef->isPointer,pdef->nameOfType));
			case	conVarTypeTagPointer:	// 指针类型
				return(UnionReadPointerTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,pdef->isPointer,pdef->nameOfType));
			case	conVarTypeTagArray:	// 数组类型
				return(UnionReadArrayTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,pdef->isPointer,pdef->nameOfType,0));
			default:
				UnionUserErrLog("in UnionReadArrayVarTypeTagOfSpecFldOfVar:: [%s] is not valid type!\n",pdef->nameOfType);
				return(errCodeCDPMDL_InvalidVarType);
		}	
	}
	// 是字符串数组
	if ((UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType) == conVarTypeTagChar) && (dimisionIndex + 1 == pdef->dimisionNum))
	{
		return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,pdef->isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrString)));
	}
	if ((UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType) == conVarTypeTagUnsignedChar) && (dimisionIndex + 1 == pdef->dimisionNum))
	{
		return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,pdef->isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrUnsignedString)));
	}
	if (strcmp(specFldOfVar,varNamePrefix) == 0)	// 是数组变量
	{
		return(conVarTypeTagArray);
	}
	if (strlen(pdef->sizeOfDimision[dimisionIndex]) != 0)	// 定义了数组大小
		dimisionSize = UnionCalculateIntConstValue(pdef->sizeOfDimision[dimisionIndex]);
	else
		dimisionSize = gunionDefaultArraySizeWhenNoSizeSet;			
	if (dimisionSize > 0)
	{
		for (index = 0; index < dimisionSize; index++)
		{
			sprintf(arrayTag,"%s[%d]",varNamePrefix,index);
			if (strcmp(arrayTag,specFldOfVar) == 0)
				return(UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType));
			if ((ret = UnionReadArrayVarTypeTagOfSpecFldOfVar(specFldOfVar,arrayTag,pdef,dimisionIndex+1)) < 0)
			{
				UnionAuditLog("in UnionReadArrayVarTypeTagOfSpecFldOfVar:: UnionReadArrayVarTypeTagOfSpecFldOfVar!\n");
				continue;
			}
			return(ret);
		}
	}
	else
	{
		sprintf(arrayTag,"%s[*]",varNamePrefix);
		if (strcmp(arrayTag,specFldOfVar) == 0)
			return(UnionGetTypeTagOfSpecNameOfType(pdef->nameOfType));
		if ((ret = UnionReadArrayVarTypeTagOfSpecFldOfVar(specFldOfVar,arrayTag,pdef,dimisionIndex+1)) < 0)
		{
			UnionAuditLog("in UnionReadArrayVarTypeTagOfSpecFldOfVar:: UnionReadArrayVarTypeTagOfSpecFldOfVar!\n");
			return(ret);
		}
	}
	return(ret);
}	

/*
功能	
	读一个数组变量取值的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	isPointer	指针标识
	nameOfType	变量的类型
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadArrayTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType,int dimisionIndex)
{
	int		index;
	int		dimisionSize = 0;
	char		arrayTag[128+1];
	int		ret;
	int		isBaseType;
	TUnionArrayDef	def;
	
	if (nameOfType == NULL)
		return(errCodeParameter);	
	if ((ret = UnionReadArrayDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionReadArrayTypeTagOfSpecFldOfVar:: UnionReadArrayDefFromDefaultDefFile [%s]!\n",nameOfType);
		return(ret);
	}
	
	//UnionLog("**** in UnionReadArrayTypeTagOfSpecFldOfVar:: varNamePrefix = [%s] isPointer = [%d]\n",varNamePrefix,isPointer+1);
	if ((dimisionIndex >= def.dimisionNum) || (def.dimisionNum <= 0))	// 数组遍历完了
	{
		if ((isBaseType = UnionIsBaseType(def.nameOfType)) > 0)	// 是基础类型
			return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(def.nameOfType)));
		// 复杂变量类型
		switch (UnionGetTypeTagOfSpecNameOfType(def.nameOfType))
		{
			case	conVarTypeTagStruct:	// 结构类型
				return(UnionReadStructTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,def.nameOfType));
			case	conVarTypeTagSimpleType:	// 简单类型
				return(UnionReadSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,def.nameOfType));
			case	conVarTypeTagPointer:	// 指针类型
				return(UnionReadPointerTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,def.nameOfType));
			default:
				UnionUserErrLog("in UnionReadArrayTypeTagOfSpecFldOfVar:: [%s] is not valid type!\n",def.nameOfType);
				return(errCodeCDPMDL_InvalidVarType);
		}	
	}
	// 是字符串数组
	if ((UnionGetTypeTagOfSpecNameOfType(def.nameOfType) == conVarTypeTagChar) && (dimisionIndex + 1 == def.dimisionNum))
	{
		return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrString)));
	}
	if ((UnionGetTypeTagOfSpecNameOfType(def.nameOfType) == conVarTypeTagUnsignedChar)&& (dimisionIndex + 1 == def.dimisionNum))
	{
		return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(conDesginKeyWordTagStrUnsignedString)));
	}
	
	if (strcmp(specFldOfVar,varNamePrefix) == 0)	// 是数组变量
	{
		return(conVarTypeTagArray);
	}

	if (strlen(def.sizeOfDimision[dimisionIndex]) != 0)	// 定义了数组大小
		dimisionSize = UnionCalculateIntConstValue(def.sizeOfDimision[dimisionIndex]);
	else
		dimisionSize = gunionDefaultArraySizeWhenNoSizeSet;			
	if (dimisionSize > 0)
	{
		for (index = 0; index < dimisionSize; index++)
		{
			sprintf(arrayTag,"%s[%d]",varNamePrefix,index);
			if (strcmp(arrayTag,specFldOfVar) == 0)
				return(UnionGetTypeTagOfSpecNameOfType(nameOfType));
			if ((ret = UnionReadArrayTypeTagOfSpecFldOfVar(specFldOfVar,arrayTag,isPointer,nameOfType,dimisionIndex+1)) < 0)
			{
				UnionAuditLog("in UnionReadArrayTypeTagOfSpecFldOfVar:: UnionReadArrayTypeTagOfSpecFldOfVar!\n");
				continue;
			}
			return(ret);
		}
		return(ret);
	}
	else
	{
		UnionUserErrLog("in UnionReadArrayTypeTagOfSpecFldOfVar:: size of array %s not set for dimision index %d!\n",nameOfType,dimisionIndex);
		return(errCodeCDPMDL_ArraySizeNotSet);
	}
}	

/*
功能	
	读一个简单c语言类型取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varName		变量名称
	typeTag		类型
	isPointer	指标标识，1表示是指针，0表示不是指针
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varName,int isPointer,int typeTag)
{
	int			ret;
	char			nameOfType[64+1];
	int			index;
	char			pointerTag[10];

	if (strcmp(specFldOfVar,varName) != 0)
	{
		//UnionUserErrLog("in UnionReadCProgramSimpleTypeTagOfSpecFldOfVar:: specFldOfVar [%s] != [%s]\n",specFldOfVar,varName);
		//UnionAuditLog("in UnionReadCProgramSimpleTypeTagOfSpecFldOfVar:: specFldOfVar [%s] != [%s]\n",specFldOfVar,varName);
		return(errCodeVarNameNotCorrect);
	}
	if (isPointer)
	{
		if ((typeTag != conVarTypeTagChar) && (typeTag != conVarTypeTagUnsignedChar))
			return(conVarTypeTagPointer);
		else if (typeTag == conVarTypeTagChar)
			return(conVarTypeTagString);
		else
			return(conVarTypeTagUnsignedString);
	}
	else
		return(typeTag);
}

/*
功能	
	读一个结构取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	isPointer	指明变量是否是指针
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadStructTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType)
{
	int			ret;
	TUnionStructDef		structDef;
	int			varIndex;
	char			varName[128+1];
	
	if (nameOfType == NULL)
		return(errCodeParameter);

	if (strcmp(specFldOfVar,varNamePrefix) == 0)	// 是结构变量
	{
		return(conVarTypeTagStruct);
	}

	if (isPointer)
		sprintf(varName,"%s->",varNamePrefix);
	else
		sprintf(varName,"%s.",varNamePrefix);

	if (strcmp(specFldOfVar,varName) == 0)
	{
		/*
		if (isPointer)
			return(conVarTypeTagPointer);
		else
		*/
			return(UnionGetTypeTagOfSpecNameOfType(nameOfType));
	}

	// 读结构定义	
	memset(&structDef,0,sizeof(structDef));
	if ((ret = UnionReadStructDefFromDefaultDefFile(nameOfType,&structDef)) < 0)
	{
		UnionUserErrLog("in UnionReadStructTypeTagOfSpecFldOfVar:: UnionReadStructDefFromDefaultDefFile [%s]!\n",nameOfType);
		return(ret);
	}
		
	for (varIndex = 0; varIndex < structDef.fldNum; varIndex++)
	{
		if ((ret = UnionReadTypeTagOfSpecFldOfVar(specFldOfVar,varName,&(structDef.fldGrp[varIndex]))) < 0)
		{
			//UnionAuditLog("in UnionReadStructTypeTagOfSpecFldOfVar:: UnionReadTypeTagOfSpecFldOfVar [%s] from struct [%s]! varNamePrefix = [%s]! ret = [%d]!\n",
			//	specFldOfVar,nameOfType,varName,ret);
			continue;
		}
		return(ret);
	}
	UnionUserErrLog("in UnionReadStructTypeTagOfSpecFldOfVar:: UnionReadTypeTagOfSpecFldOfVar [%s] from struct [%s]! varNamePrefix = [%s]! ret = [%d]!\n",specFldOfVar,nameOfType,varName,ret);
	return(ret);
}

/*
功能	
	读一个简单类型取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadSimpleTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType)
{
	int			ret;
	int			typeTag;
	char			finalNameOfType[128+1];
	
	memset(finalNameOfType,0,sizeof(finalNameOfType));
	if ((ret = UnionGetFinalTypeNameOfSpecNameOfType(nameOfType,finalNameOfType)) < 0)
	{
		UnionUserErrLog("in UnionReadSimpleTypeTagOfSpecFldOfVar:: UnionGetFinalTypeNameOfSpecNameOfType [%s]!\n",nameOfType);
		return(ret);
	}
	if ((typeTag = UnionIsBaseType(finalNameOfType)) > 0)	// 是基础类型
		return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(finalNameOfType)));
	// 复杂变量类型
	switch (UnionGetTypeTagOfSpecNameOfType(finalNameOfType))
	{
		case	conVarTypeTagStruct:	// 结构类型
			return(UnionReadStructTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,finalNameOfType));
		case	conVarTypeTagSimpleType:	// 简单类型
			return(UnionReadSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,finalNameOfType));
		case	conVarTypeTagPointer:	// 指针类型
			return(UnionReadPointerTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,finalNameOfType));
		case	conVarTypeTagArray:	// 数组类型
			return(UnionReadArrayTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,finalNameOfType,0));
		default:
			UnionUserErrLog("in UnionReadArrayVarTypeTagOfSpecFldOfVar:: [%s] is not valid type!\n",finalNameOfType);
			return(errCodeCDPMDL_InvalidVarType);
	}	
}

/*
功能	
	读一个指针类型取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadPointerTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType)
{
	int			ret;
	int			typeTag;
	char			typeNameOfPointer[128+1];
	char			varName[128+1];
	
	if (strcmp(specFldOfVar,varNamePrefix) == 0)
		return(conVarTypeTagPointer);	
	memset(typeNameOfPointer,0,sizeof(typeNameOfPointer));
	if ((ret = UnionReadTypeOfPointerDefFromDefaultDefFile(nameOfType,typeNameOfPointer)) < 0)
	{
		UnionUserErrLog("in UnionReadPointerTypeTagOfSpecFldOfVar:: UnionReadTypeOfPointerDefFromDefaultDefFile [%s]!\n",nameOfType);
		return(ret);
	}
	if (UnionIsBaseType(typeNameOfPointer) > 0)	// 是基础类型
		return(UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,isPointer,UnionGetTypeTagOfSpecNameOfType(typeNameOfPointer)));

	// 复杂变量类型
	switch (UnionGetTypeTagOfSpecNameOfType(typeNameOfPointer))
	{
		case	conVarTypeTagStruct:	// 结构类型
			return(UnionReadStructTypeTagOfSpecFldOfVar(specFldOfVar,varNamePrefix,1,typeNameOfPointer));
		case	conVarTypeTagSimpleType:	// 简单类型
			if (isPointer)
				sprintf(varName,"*%s",varNamePrefix);
			else
				sprintf(varName,"%s",varNamePrefix);			
			return(UnionReadSimpleTypeTagOfSpecFldOfVar(specFldOfVar,varName,1,typeNameOfPointer));
		case	conVarTypeTagPointer:	// 指针类型
			if (isPointer)
				sprintf(varName,"*%s",varNamePrefix);
			else
				sprintf(varName,"%s",varNamePrefix);			
			return(UnionReadPointerTypeTagOfSpecFldOfVar(specFldOfVar,varName,1,typeNameOfPointer));
		default:
			UnionUserErrLog("in UnionReadPointerTypeTagOfSpecFldOfVar:: [%s] is invalid type!\n",typeNameOfPointer);
			return(errCodeCDPMDL_InvalidVarType);
	}
}	

/*
功能	
	读一个变量域的类型
输入参数
	specFldOfVar	指定的变量域
	str		定义串
	lenOfStr	定义串的长度
输入出数
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadValueTagFromVarDefStrOfSpecFldOfVar(char *specFldOfVar,char *str,int lenOfStr)
{
	TUnionVarDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarDefFromStr(str,lenOfStr,&def)) < 0)
	{
		UnionUserErrLog("in UnionReadValueTagFromVarDefStrOfSpecFldOfVar:: UnionReadVarDefFromStr!\n");
		return(ret);
	}
	return(UnionReadTypeTagOfSpecFldOfVar(specFldOfVar,"",&def));
}

/*
功能	
	读一个函数的变量域的类型
输入参数
	pdef				要打印的函数定义
	specFldOfVar			缺省的名字，如果未定义名字，则采用这个名字
输出参数
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadTypeTagOfSpecVarFldOfFun(PUnionFunDef pdef,char *specFldOfVar)
{
	int	index;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);

	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionReadTypeTagOfSpecFldOfVar(specFldOfVar,"",&(pdef->varGrp[index].varDef))) < 0)
		{
			//UnionUserErrLog("in UnionReadTypeTagOfSpecVarFldOfFun:: UnionReadTypeTagOfSpecFldOfVar! var [%s] not valid var of fun [%s]! ret = [%d]\n",specFldOfVar,pdef->funName,ret);
			continue;
		}
		return(ret);
	}
	UnionUserErrLog("in UnionReadTypeTagOfSpecVarFldOfFun:: [%s] not valid fldOfVar of fun [%s]! ret = [%d]\n",specFldOfVar,pdef->funName,ret);
	return(ret);
}

/*
功能	
	读一个函数的变量域的类型
输入参数
	funName				函数名称
	specFldOfVar			缺省的名字，如果未定义名字，则采用这个名字
输出参数
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadTypeTagOfSpecVarFldOfSpecFun(char *funName,char *specFldOfVar)
{
	TUnionFunDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionReadTypeTagOfSpecVarFldOfSpecFun:: UnionReadFunDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionReadTypeTagOfSpecVarFldOfFun(&def,specFldOfVar));
}
	
