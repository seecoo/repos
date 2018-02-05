//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionFunDef.h"
#include "unionVarValue.h"
#include "unionErrCode.h"
#include "unionGenFunTestProgram.h"
#include "unionGenFunListTestProgram.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionFunDefFile.h"
#include "unionTestConf.h"

#ifndef conDefaultArraySize
#define	conDefaultArraySize	3
#endif

char gunionConfFileNameOfFixedCodes[128] = "";
char gunionCurrentVarValueLen[128] = "";
int gunionVarIsBinary = 0;

/*
功能	
	设置当前设置的变量是否是二进制
输入参数
	isBinary	变量是否是二进制的标志
输入出数
	无
返回值
	无
*/
void UnionSetCurrentVarValueAsBinary(int isBinary)
{
	gunionVarIsBinary = isBinary;
	return;
}

/*
功能	
	设置当前设置的变量的长度
输入参数
	varValueLen	变量的长度
输入出数
	无
返回值
	无
*/
void UnionSetCurrentVarValueLenName(char *varValueLen)
{
	strcpy(gunionCurrentVarValueLen,varValueLen);
	return;
}
/*
功能	
	设置固定代码的定义文件
输入参数
	fileName	存入固定代码的定义文件
输入出数
	无
返回值
	>=0		正确
	<0		错误码
*/
int UnionSetFileNameOfFixedCodesConfFile(char *filename)
{
	strcpy(gunionConfFileNameOfFixedCodes,filename);
	return(0);
}

/*
功能	
	获取固定代码的定义文件
输入参数
	无
输入出数
	fileName	存入固定代码的定义文件
返回值
	>=0		正确
	<0		错误码
*/
int UnionFileNameOfFixedCodesConfFile(char *filename)
{
	if (strlen(gunionConfFileNameOfFixedCodes) == 0)
		return(0);
	sprintf(filename,"%s/conf/%s.def",getenv("DIROFTEST"),gunionConfFileNameOfFixedCodes);
	return(strlen(filename));
}

/*
功能	
	写程序的生成信息
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	无
*/
void UnionAddAutoGenerateInfoToTestProgram(FILE *fp)
{
	char		systemDateTime[14+1];
	FILE		*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"// This is a group of automatically created c program-language codes by Union Tech Development Platform.\n");
	memset(systemDateTime,0,sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);
	fprintf(outFp,"// Generated Time:: %s\n\n",systemDateTime);
	return;
}

/*
功能	
	将固定的测试代码写入到测试程序中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	代码的数量
*/
int UnionAddFixedCodesToFile(FILE *fp)
{
	FILE		*includeFp;
	char		fileName[256+1];
	int		includeNum = 0;
	char		lineStr[1024+1];
	int		lineLen;
	int		lineNum = 0;
	FILE		*outFp = stdout;
	
	if ((UnionFileNameOfFixedCodesConfFile(fileName)) <= 0)
		return(0);

	if (fp != NULL)
		outFp = fp;
		
	if ((includeFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionAddFixedCodesToFile:: no specical include files defined in [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"\n\n");
	fprintf(outFp,"// this is fixed codes testing users defined!\n");
	while (!feof(includeFp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(includeFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionUserErrLog("in UnionAddFixedCodesToFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d]\n",lineNum);
			continue;
		}
		if (lineLen <= 0)
			continue;
		fprintf(outFp,"%s\n",lineStr);
		includeNum++;
	}
	fclose(includeFp);
	return(includeNum);
}

/*
功能	
	将头文件写入到测试程序中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	头文件的数量
*/
int UnionAddIncludeFileToTestProgram(FILE *fp)
{
	FILE		*includeFp;
	char		fileName[256+1];
	int		includeNum = 0;
	char		lineStr[1024+1];
	int		lineLen;
	int		lineNum = 0;
	
	fprintf(fp,"#include <stdio.h>\n");
	fprintf(fp,"#include <string.h>\n");
	fprintf(fp,"#include <stdlib.h>\n");
	fprintf(fp,"\n");
	includeNum = 3;
	
	UnionGetIncludeFileDefFileName(fileName);
	if ((includeFp = fopen(fileName,"r")) == NULL)
	{
		UnionLog("in UnionAddIncludeFileToTestProgram:: no specical include files defined!\n");
		return(includeNum);
	}
	while (!feof(includeFp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(includeFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionUserErrLog("in UnionAddIncludeFileToTestProgram:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d]\n",lineNum);
			continue;
		}
		if (lineLen <= 0)
			continue;
		fprintf(fp,"#include \"%s\"\n",lineStr);
		includeNum++;
	}
	fclose(includeFp);
	return(includeNum);
}

/*
功能	
	产生一个为指针型函数变量分配空间的代码
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMallocCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	return(0);
}

/*
功能	
	产生一个为指针型函数变量分配空间的函数
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMallocFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionMallocForPointerFunVar()\n");
	fprintf(outFp,"{\n");
	/*
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	*/
	UnionGenerateMallocCodesForPointerFunVarToFp(pdef,fp);
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
功能	
	产生一个为指针型函数变量释放空间的代码
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFreeCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	return(0);
}

/*
功能	
	产生一个为指针型函数变量释放空间的函数
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFreeFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionFreeForPointerFunVar()\n");
	fprintf(outFp,"{\n");
	/*
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	*/
	UnionGenerateFreeCodesForPointerFunVarToFp(pdef,fp);
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
功能	
	产生一个为函数生成调用函数
输入参数
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCallFunOfFunToTestProgram(char *funName,FILE *fp)
{
	return(UnionGenerateCallFunOfFunGrpToTestProgram(-1,funName,fp));
	/*
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionCallFunOfFun()\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        ret = %s(",pdef->funName);
	for (index = 0; index < pdef->varNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s",pdef->varGrp[index].varDef.name);
	}
	fprintf(outFp,");\n");
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	return(0);
	*/
}

/*
功能	
	产生一个读取函数值定义的索引文件
输入参数
	pdef		函数定义指针
	fileName	索引文件名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunVarValueTagListIndexFile(PUnionFunDef pdef)
{
	int	index;
	FILE	*inputFp = stdout,*outputFp = stdout,*returnFp = stdout;
	char	tmpBuf[1024+1];
	int	ret;
	FILE	*fp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	UnionGetFunVarInputValueListIndexFileName(pdef->funName,tmpBuf);
	if ((inputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFile:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarOutputValueListIndexFileName(pdef->funName,tmpBuf);
	if ((outputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFile:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarReturnValueListIndexFileName(pdef->funName,tmpBuf);
	if ((returnFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFile:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		fclose(outputFp);
		return(errCodeUseOSErrCode);
	}
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFilePrintToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if (pdef->varGrp[index].isOutput)	// 输出参数
			fp = outputFp;
		else
			fp = inputFp;
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),fp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFile:: UnionPrintValueTagListToFile!\n");
			goto errExit;
		}
	}
	if ((ret = UnionPrintValueTagListToFile("ret",&(pdef->returnType),returnFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFile:: UnionPrintValueTagListToFile!\n");
		goto errExit;
	}
	ret = 0;
errExit:
	fclose(inputFp);
	fclose(outputFp);
	fclose(returnFp);
	return(ret);
}

/*
功能	
	将函数定义及为函数参数分配的变量定义写入到测试程序中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddFunDefAndFunVarListToTestProgram(char *funName,FILE *fp)
{
	int		ret;
	TUnionFunDef	funDef;
	
	// 读函数定义
	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}
	// 将函数定义写入到程序中
	if ((ret = UnionPrintFunDefToFp(&funDef,1,1,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionPrintFunDefToFp [%s]!\n",funName);
		return(ret);
	}
	// 设置数组的缺省大小
	UnionSetDefaultArraySizeWhenNoSizeSet(conDefaultArraySize);
	// 定义函数变量
	if ((ret = UnionPrintFunVarDefToFpOfCPragram(&funDef,"ret",conDefaultArraySize,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionPrintFunVarDefToFpOfCPragram [%s]!\n",funName);
		return(ret);
	}
	// 写一个为指针型函数变量初始化空间的函数
	if ((ret = UnionGenerateMallocFunForPointerFunVarToTestProgram(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionGenerateMallocFunForPointerFunVarToTestProgram [%s]!\n",funName);
		return(ret);
	}
	// 写一个释放指针型函数变量空间的函数
	if ((ret = UnionGenerateFreeFunForPointerFunVarToTestProgram(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionGenerateFreeFunForPointerFunVarToTestProgram [%s]!\n",funName);
		return(ret);
	}
	// 写一个函数生成测试输入输出索引文件
	if ((ret = UnionGenerateFunVarValueTagListIndexFile(&funDef)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionGenerateFunVarValueTagListIndexFile [%s]!\n",funName);
		return(ret);
	}
	return(0);
}

/*
功能	
	产生一个对字符串类型进行赋值的语句
输入参数
	localVarPrefix	变量前缀
	varValueTag	变量名称
	valueIsVarName	值是不是变量名称
	value		数据值
	varValueLenTag	变量值的长度
	isBinary	变量值是否是二进制,1,是,0,否
	fp		文件句柄
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateStrAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp)
{
	FILE	*outFp = stdout;
	char	varName[128+1];
	
	if (fp != NULL)
		outFp = fp;
	
	sprintf(varName,"%s%s",localVarPrefix,varValueTag);
	fprintf(outFp,"        printf(\"%s = [%s]\\n\");\n",varName,value);
	
	if (isBinary)	// 是二进制,以二进制方式赋值
		goto copyAsBinary;
		
	if ((varValueLenTag == NULL) || (strlen(varValueLenTag) == 0))	// 没有为变量设置长度
	{
		if (valueIsVarName)
			fprintf(outFp,"        strcpy(%s,%s);\n",varName,value);
		else
			fprintf(outFp,"        strcpy(%s,\"%s\");\n",varName,value);
	}
	else
	{
		if (valueIsVarName)
			fprintf(outFp,"        memcpy(%s,%s,%s);\n",varName,value,varValueLenTag);
		else
			fprintf(outFp,"        memcpy(%s,\"%s\",%s);\n",varName,value,varValueLenTag);
	}
	return(0);

copyAsBinary:
	if ((varValueLenTag == NULL) || (strlen(varValueLenTag) == 0))
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,strlen(%s),%s);",value,value,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",strlen(\"%s\"),%s);",value,value,varName);
	}
	else
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,%s,%s);",value,varValueLenTag,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",%s,%s);",value,varValueLenTag,varName);
	}
	
	return(0);
}

/*
功能	
	产生一个对指针类型进行赋值的语句
输入参数
	localVarPrefix	变量前缀
	varValueTag	变量名称
	valueIsVarName	值是不是变量名称
	value		数据值
	varValueLenTag	变量值的长度
	isBinary	变量值是否是二进制,1,是,0,否
	fp		文件句柄
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGeneratePointerAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp)
{
	FILE	*outFp = stdout;
	char	varName[128+1];
	char	lenFlag[128+1];
	
	if (fp != NULL)
		outFp = fp;
	
	sprintf(varName,"%s%s",localVarPrefix,varValueTag);
	fprintf(outFp,"        printf(\"%s = [%s]\\n\");\n",varName,value);
	
	if (strcmp(value,"NULL") == 0)	// 空指针
	{
		fprintf(outFp,"        %s = NULL;\n",varName);
		return(0);
	}
			
	if (isBinary)	// 是二进制,以二进制方式赋值
		goto copyAsBinary;
		
	if (valueIsVarName)
	{
		if (strlen(varValueLenTag) == 0)
		{
			if (value[0] != '&')
				sprintf(lenFlag,"sizeof(*%s)",value);
			else
				sprintf(lenFlag,"sizeof(%s)",value+1);
		}
		else
			strcpy(lenFlag,varValueLenTag);
		fprintf(outFp,"        memcpy(%s,%s,%s);\n",varName,value,lenFlag);
	}
	else
		fprintf(outFp,"        memcpy(%s,\"%s\",strlen(\"%s\"));\n",varName,value,value);
	return(0);
copyAsBinary:
	if ((varValueLenTag == NULL) || (strlen(varValueLenTag) == 0))
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,strlen(%s),(char *)%s);",value,value,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",strlen(\"%s\"),(char *)%s);",value,value,varName);
	}
	else
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,%s,(char *)%s);",value,varValueLenTag,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",%s,(char *)%s);",value,varValueLenTag,varName);
	}
	return(0);
}

/*
功能	
	产生一个对基本数据类型进行赋值的语句
输入参数
	varPrefix	变量前缀
	baseType	基本类型
	varValueTag	变量名称
	valueIsVarName	值是不是变量名称
	value		数据值
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateAssignmentCSentenceToFile(char *varPrefix,int baseType,char *varValueTag,int valueIsVarName,char *value,FILE *fp)
{
	FILE *outFp = stdout;
	char	localVarPrefix[128+1];
	
	if ((varValueTag == NULL) || (value == NULL))
	{
		UnionUserErrLog("in UnionGenerateFunInputVarAssignmentFun:: null pointer!\n");
		return(errCodeParameter);
	}
	memset(localVarPrefix,0,sizeof(localVarPrefix));
	if (varPrefix != NULL)	
		strcpy(localVarPrefix,varPrefix);
	if (fp != NULL)
		outFp = fp;
	switch (baseType)
	{
		case	conVarTypeTagChar:	// 字符串
		case	conVarTypeTagUnsignedChar:	// 字符串
			if (varValueTag[0] == '*')	// 是字符串
			{
				UnionGenerateStrAssignmentCSentenceToFile(localVarPrefix,varValueTag+1,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
			}
			else
			{
				if (strstr(varValueTag,"[") == NULL)	// 是字符
				{
					if (valueIsVarName)
						fprintf(outFp,"        %s%s = %s;\n",localVarPrefix,varValueTag,value);
					else
						fprintf(outFp,"        %s%s = '%c';\n",localVarPrefix,varValueTag,value[0]);
					fprintf(outFp,"        printf(\"%s%s = [%c]\\n\");\n",localVarPrefix,varValueTag,value[0]);
				}
				else
				{
					UnionGenerateStrAssignmentCSentenceToFile(localVarPrefix,varValueTag,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
				}
			}
			break;
		case	conVarTypeTagString:	// 字符串
		case	conVarTypeTagUnsignedString:	// 字符串
			UnionGenerateStrAssignmentCSentenceToFile(localVarPrefix,varValueTag,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
			break;
		case	conVarTypeTagPointer:	// 指针
			UnionGeneratePointerAssignmentCSentenceToFile(localVarPrefix,varValueTag,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
			/*
			if (valueIsVarName)
			{
				if (strlen(gunionCurrentVarValueLen) == 0)
				{
					if (value[0] != '&')
						sprintf(lenFlag,"sizeof(*%s)",value);
					else
						sprintf(lenFlag,"sizeof(%s)",value+1);
				}
				else
					strcpy(lenFlag,gunionCurrentVarValueLen);
				fprintf(outFp,"        memcpy(%s%s,%s,%s);\n",localVarPrefix,varValueTag,value,lenFlag);
			}
			else
				fprintf(outFp,"        memcpy(%s%s,\"%s\",strlen(\"%s\"));\n",localVarPrefix,varValueTag,value,value);
			fprintf(outFp,"        printf(\"%s%s = [%s]\\n\");\n",localVarPrefix,varValueTag,value);
			*/
			break;		
		case	conVarTypeTagArray:	// 数组
			fprintf(outFp,"        memcpy(%s%s,%s,sizeof(%s%s));\n",localVarPrefix,varValueTag,value,localVarPrefix,varValueTag);
			fprintf(outFp,"        printf(\"%s%s = [%s]\\n\");\n",localVarPrefix,varValueTag,value);
			break;		
		default:
			if (UnionIsBaseTypeTag(baseType))
				fprintf(outFp,"        %s%s = %s;\n",localVarPrefix,varValueTag,value);
			else
				fprintf(outFp,"        memcpy(&(%s%s),&(%s),sizeof(%s%s));\n",localVarPrefix,varValueTag,value,localVarPrefix,varValueTag);
			fprintf(outFp,"        printf(\"%s%s = [%s]\\n\");\n",localVarPrefix,varValueTag,value);
			break;
	}
	return(0);
}

/*
功能	
	产生一个对函数的输入参数进行赋值的函数
输入参数
	funName		函数名称
	testDataFileName	测试数据文件名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunInputVarAssignmentFun(char *funName,char *testDataFileName,FILE *fp)
{
	return(UnionGenerateFunInputVarOfFunGrpAssignmentFun(-1,funName,testDataFileName,fp));
}

/*
功能	
	产生一个对基本数据类型进行打印的语句
输入参数
	varPrefix	变量前缀
	baseType	基本类型
	varValueTag	变量名称
	value		数据值
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintOutputVarCSentenceToFp(char *varPrefix,int baseTypeTag,char *varValueTag,FILE *fp)
{
	char	typeTag[48+1];
	int	valueTagOffset = 0;
	FILE	*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
		
	switch (baseTypeTag)
	{
		case	conVarTypeTagUnsignedInt:
		case	conVarTypeTagInt:
		case	conVarTypeTagEnum:
			strcpy(typeTag,"%d");
			break;		
		case	conVarTypeTagLong:
		case	conVarTypeTagUnsignedLong:
			strcpy(typeTag,"%ld");
			break;		
		case	conVarTypeTagDouble:
			strcpy(typeTag,"%lf");
			break;		
		case	conVarTypeTagFloat:
			strcpy(typeTag,"%f");
			break;		
		case	conVarTypeTagString:
			strcpy(typeTag,"%s");
			break;		
		case	conVarTypeTagUnsignedString:
			strcpy(typeTag,"%s");
			break;		
		case	conVarTypeTagChar:
		case	conVarTypeTagUnsignedChar:
			if (varValueTag[0] == '*')	// 是字符串
			{
				strcpy(typeTag,"%s");
				valueTagOffset = 1;
			}
			else
			{
				if (strstr(varValueTag,"[") == NULL)	// 是字符
					strcpy(typeTag,"%c");
				else
					strcpy(typeTag,"%s");
			}
			break;
		default:
			UnionUserErrLog("in UnionPrintOutputVarCSentenceToFp:: invalid baseTypeTag = [%d] for [%s]\n",baseTypeTag,varValueTag);
			return(errCodeCDPMDL_InvalidVarType);
	}
	fprintf(outFp,"        printf(\"%s%s=%s\\n\",%s%s);\n",varPrefix,varValueTag,typeTag,varPrefix,varValueTag+valueTagOffset);
	return(0);
}

/*
功能	
	产生一个对函数的输出参数进行展示的函数
输入参数
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunOutputVarDisplayFun(char *funName,FILE *fp)
{
	return(UnionGenerateFunOutputVarOfFunGrpDisplayFun(-1,funName,fp));
}
		
/*
功能	
	产生一个对函数的返回参数进行展示的函数
输入参数
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunReturnVarDisplayFun(char *funName,FILE *fp)
{
	return(UnionGenerateFunReturnVarOfFunGrpDisplayFun(-1,funName,fp));
}

/*
功能	
	为一个主函数
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMainFunToTestProgram(FILE *fp)
{
	return(UnionGenerateMainFunOfFunGrpToTestProgram(NULL,0,fp));
}

/*
功能	
	为一个函数产生一个测试程序
输入参数
	funName		函数名称
	fileName	文件名称
	testDataFileName	测试数据文件名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateTestProgramForFun(char *funName,char *testDataFileName,char *fileName)
{
	int		ret;
	FILE		*fp = stdout;

	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionGenerateTestProgramForFun:: fopen [%s] for writting error!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	
	// 写程序的生成日期
	UnionAddAutoGenerateInfoToTestProgram(fp);
	// 写包含的头文件信息
	if ((ret = UnionAddIncludeFileToTestProgram(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionAddIncludeFileToTestProgram!\n");
		goto errExit;
	}
	// 写函数信息
	if ((ret = UnionAddFunDefAndFunVarListToTestProgram(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionAddFunDefAndFunVarListToTestProgram!\n");
		goto errExit;
	}
	// 写对函数进行输入参数初始化的函数
	if ((ret = UnionGenerateFunInputVarAssignmentFun(funName,testDataFileName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateFunInputVarAssignmentFun of [%s] from [%s]!\n",funName,testDataFileName);
		goto errExit;
	}
	// 写对函数进行输出参数展示的函数
	if ((ret = UnionGenerateFunOutputVarDisplayFun(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateFunOutputVarDisplayFun!\n");
		goto errExit;
	}
	// 写对函数进行返回参数展示的函数
	if ((ret = UnionGenerateFunReturnVarDisplayFun(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateFunReturnVarDisplayFun!\n");
		goto errExit;
	}
	// 写一个调用函数的函数
	if ((ret = UnionGenerateCallFunOfFunToTestProgram(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateCallFunOfFunToTestProgram [%s]!\n",funName);
		return(ret);
	}
	// 写主函数
	if ((ret = UnionGenerateMainFunToTestProgram(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateMainFunToTestProgram!\n");
		goto errExit;
	}
	ret = 0;
errExit:
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

