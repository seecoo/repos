//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionFunDef.h"
#include "unionVarValue.h"
#include "unionErrCode.h"
#include "unionGenFunTestProgram.h"
#include "unionGenFunListTestProgram.h"
#include "unionVarAssignment.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionTestConf.h"
#include "unionFunDefFile.h"

#ifndef conDefaultArraySize
#define	conDefaultArraySize	3
#endif

/*
功能
	产生一个调用函数的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateCallFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex < 0)
		fprintf(outFp,"UnionCallFunOfFun()");
	else
		fprintf(outFp,"UnionCallFunOfFun%02d_%s()",funIndex,funName);
}

/*
功能
	产生一个对输入参数赋值的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateInputVarSetFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex >= 0)
		fprintf(outFp,"UnionSetFunInputVarOfFun%02d_%s()",funIndex,funName);
	else
		fprintf(outFp,"UnionSetFunInputVar()");
}

/*
功能
	产生一个显示输出参数的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateOutputVarDisplayFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex >= 0)
		fprintf(outFp,"UnionDisplayOutputFunVarOfFun%02d_%s()",funIndex,funName);
	else
		fprintf(outFp,"UnionDisplayOutputFunVar()");
}

/*
功能
	产生一个显示返回值的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateReturnVarDisplayFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex >= 0)
		fprintf(outFp,"UnionDisplayReturnFunVarOfFun%02d_%s()",funIndex,funName);
	else
		fprintf(outFp,"UnionDisplayReturnFunVar()");
}


/*
功能	
	为一个函数变量生成前缀
输入参数
	funIndex	函数序号
输入出数
	funVarPrefix	变量前缀
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunVarNamePrefixOfFunGrp(int funIndex,char *funVarPrefix)
{
	if (funIndex < 0)
		strcpy(funVarPrefix,"");
	else
		sprintf(funVarPrefix,"fun%02d_",funIndex);
	return(0);
}

/*
功能	
	产生一个为一组函数的指针型函数变量释放空间的函数
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	int		index;
	FILE		*outFp = stdout;
	int		ret;
	TUnionFunDef	funDef;
	int		funIndex;
	char		funVarPrefix[10];
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionFreeForPointerFunVar()\n");
	fprintf(outFp,"{\n");

	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile);
	for (funIndex = 0; funIndex < numOfFun; funIndex++)
	{
		memset(&funDef,0,sizeof(funDef));
		if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
			return(ret);
		}
		UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);
		for (index = 0; index < funDef.varNum; index++)
		{
			if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.varGrp[index].varDef),outFp)) < 0)
			{
				UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram:: UnionPrintValueTagListToFile!\n");
				return(ret);
			}
		}
	}
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
功能	
	产生一个为指针型函数变量分配空间的函数
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	int		index;
	FILE		*outFp = stdout;
	int		ret;
	TUnionFunDef	funDef;
	int		funIndex;
	char		funVarPrefix[10];

	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionMallocForPointerFunVar()\n");
	fprintf(outFp,"{\n");
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile);
	for (funIndex = 0; funIndex < numOfFun; funIndex++)
	{
		memset(&funDef,0,sizeof(funDef));
		if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
			return(ret);
		}
		UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);
		for (index = 0; index < funDef.varNum; index++)
		{
			if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.varGrp[index].varDef),outFp)) < 0)
			{
				UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram:: UnionPrintValueTagListToFile!\n");
				return(ret);
			}
		}
	}
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
功能	
	产生一个读取函数值定义的索引文件
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunVarValueTagListIndexFileOfFunGrp(char funGrp[][128+1],int numOfFun)
{
	int		index;
	FILE		*inputFp = stdout,*outputFp = stdout,*returnFp = stdout;
	char		tmpBuf[1024+1];
	int		ret;
	FILE		*fp;
	TUnionFunDef	funDef;
	int		funIndex = 0;
	char		funVarPrefix[40];

loop:	
	if (funIndex >= numOfFun)
		return(0);

	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);	
	UnionGetFunVarInputValueListIndexFileName(funDef.funName,tmpBuf);
	if ((inputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarOutputValueListIndexFileName(funDef.funName,tmpBuf);
	if ((outputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarReturnValueListIndexFileName(funDef.funName,tmpBuf);
	if ((returnFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		fclose(outputFp);
		return(errCodeUseOSErrCode);
	}
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFilePrintToFile);
	for (index = 0; index < funDef.varNum; index++)
	{
		if (funDef.varGrp[index].isOutput)	// 输出参数
			fp = outputFp;
		else
			fp = inputFp;
		if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.varGrp[index].varDef),fp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: UnionPrintValueTagListToFile!\n");
			goto errExit;
		}
	}
	sprintf(funVarPrefix+strlen(funVarPrefix),"ret");
	if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.returnType),returnFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: UnionPrintValueTagListToFile!\n");
		goto errExit;
	}
	ret = 0;
errExit:
	fclose(inputFp);
	fclose(outputFp);
	fclose(returnFp);
	if (ret < 0)
		return(ret);
	funIndex++;
	goto loop;
}

/*
功能	
	将函数定义及为函数参数分配的变量定义写入到测试程序中
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddFunDefAndFunVarListOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	int		ret;
	TUnionFunDef	funDef;
	int		funIndex = 0;
	char		funVarPrefix[10+1];
	
loop:
	if (funIndex >= numOfFun)
		return(0);
	// 读函数定义
	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	// 将函数定义写入到程序中
	if ((ret = UnionPrintFunDefToFp(&funDef,1,1,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListOfFunGrpToTestProgram:: UnionPrintFunDefToFp [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	// 设置数组的缺省大小
	UnionSetDefaultArraySizeWhenNoSizeSet(conDefaultArraySize);
	// 定义函数变量
	UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);
	if ((ret = UnionPrintFunVarDefWithPrefixToFpOfCPragram(funVarPrefix,&funDef,"ret",conDefaultArraySize,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListOfFunGrpToTestProgram:: UnionPrintFunVarDefWithPrefixToFpOfCPragram [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	funIndex++;
	goto loop;
}


/*
功能	
	产生一个对函数的输入参数进行赋值的函数
输入参数
	funIndex	函数在函数系列中的索引号
	funName		函数名称
	testDataFileName	测试数据文件名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunInputVarOfFunGrpAssignmentFun(int funIndex,char *funName,char *testDataFileName,FILE *fp)
{
	int			ret;
	FILE			*outFp=stdout,*valueFp;
	char			lineStr[2048+1];
	int			lineLen;
	//TUnionVarValueTagDef	def;
	char			fullTestDataFileName[256+1];
	int			beginSetValueOfThisFun = 0;
	char			flag[128+1];
	int			varTypeTag;
	char			varPrefix[128+1];
	char			*ptr;
	int			lineNum = 0;
	TUnionVarAssignmentDef	def;	
	int			thisFunIndex = 0;
	
	if (fp != NULL)
		outFp = fp;
	
	// 打开测试数据文件
	UnionGetFullFileNameOfTestData(testDataFileName,fullTestDataFileName);	
	if ((valueFp = fopen(fullTestDataFileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: fopen [%s]!\n",fullTestDataFileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"void ");
	UnionGenerateInputVarSetFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"输入参数::\\n\");\n");
	sprintf(flag,"valueOf::%s",funName);
	while (!feof(valueFp))
	{
		// 读一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(valueFp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen != errCodeFileEnd)
			{
				UnionUserErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionReadOneDataLineFromTxtFile!\n");
				break;
			}
			continue;
		}
		lineNum++;
		if (lineLen == 0)
			continue;
		if (funIndex >= 0)
		{
			if (!beginSetValueOfThisFun)	// 还没有到该函数的输入参数赋值语句
			{
				if ((ptr = strstr(lineStr,flag)) != NULL)	// 到了类似名函数的输入参数赋值语句
				{
					if (strcmp(ptr+strlen("valueOf::"),funName) == 0)	// 到了同名函数的赋值语句
					{
						if (thisFunIndex == funIndex)	// 找到了要赋值的函数
						{
							beginSetValueOfThisFun = 1;
							continue;
						}
					}
				}
				// 没有找到函数
				if (strstr(lineStr,"valueOf::") != NULL)	// 是另一个函数
					thisFunIndex++;
				continue;					
			}
			if (strstr(lineStr,"valueOf::") != NULL)	// 到了另一个函数的输入参数赋值语句
			{
				if (beginSetValueOfThisFun)
					break;
				thisFunIndex++;
				continue;
			}		
		}
		// 是本函数的参数输入语句::
		// 读出变量名及赋值方法
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarAssignmentDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionAuditLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionReadVarAssignmentDefFromStr [%s]!\n",lineStr);
			//return(ret);
			continue;
		}
		// 获得变量名的类型
		if ((varTypeTag = UnionReadTypeTagOfSpecVarFldOfSpecFun(funName,def.varName)) < 0)
		{
			UnionAuditLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionReadTypeTagOfSpecVarFldOfSpecFun var [%s] of fun [%s] in [%s]!\n",def.varName,funName,lineStr);
			//return(varTypeTag);
			continue;
		}
		UnionSetCurrentVarValueLenName(def.lenTag);
		UnionSetCurrentVarValueAsBinary(def.isBinary);
		// 写对变量赋值的语句
		UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
		//printf("xxxxxxx [%s][%d][%s][%d][%s] in lineIndex[%04d][%s]\n",varPrefix,varTypeTag,def.varName,def.isVarName,def.value,lineNum,lineStr);
		if ((ret = UnionGenerateAssignmentCSentenceToFile(varPrefix,varTypeTag,def.varName,def.isVarName,def.value,outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionGenerateAssignmentCSentenceToFile var [%s] of fun [%s] in [%s]\n",
				def.varName,funName,lineStr);
			return(ret);
		}
	}
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	fclose(valueFp);
	if (!beginSetValueOfThisFun)
	{
		if (funIndex < 0)
			return(0);
		UnionUserErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: fun [%s] of index [%d] not found!\n",funName,funIndex);
		return(errCodeParameter);
	}
	return(0);
}

/*
功能	
	产生一个对函数的输出参数进行展示的函数
输入参数
	funIndex	函数的序号
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunOutputVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp)
{
	int			ret;
	char			fileName[256+1];
	FILE			*outFp=stdout,*indexFp;
	char			lineStr[512+1];
	int			lineLen;
	TUnionVarValueTagDef	def;
	int			valueTagOffset=0;
	char			*ptr;
	//char			varPrefix[40+1];
		
	if (fp != NULL)
		outFp = fp;
	
	UnionGetFunVarOutputValueListIndexFileName(funName,fileName);
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"void ");
	UnionGenerateOutputVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"\\n输出参数::\\n\");\n");
	//UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
	while (!feof(indexFp))
	{
		// 读一行
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		// 从文件中读取取值标识
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarValueTagDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: UnionReadVarValueTagDefFromStr [%s]!\n",lineStr);
			continue;
		}
		if (funIndex >= 0)
		{
			if ((ptr = strstr(def.varValueTag,"fun")) != NULL)
				valueTagOffset = abs(ptr - def.varValueTag);
			else
				valueTagOffset = 0;
			def.varValueTag[valueTagOffset+3] = funIndex % 100 / 10 + '0';
			def.varValueTag[valueTagOffset+4] = funIndex % 10 + '0';
		}
		//UnionLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: typeTag = [%d] valueTag = [%s]\n",def.baseTypeTag,def.varValueTag);
		//UnionPrintOutputVarCSentenceToFp(varPrefix,def.baseTypeTag,def.varValueTag,outFp);
		UnionPrintOutputVarCSentenceToFp("",def.baseTypeTag,def.varValueTag,outFp);
	}
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	fclose(indexFp);
	return(0);
}

/*
功能	
	产生一个对函数的返回参数进行展示的函数
输入参数
	funIndex	函数的序号
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunReturnVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp)
{
	int			ret;
	char			fileName[256+1];
	FILE			*outFp=stdout,*indexFp;
	char			lineStr[512+1];
	int			lineLen;
	TUnionVarValueTagDef	def;
	int			valueTagOffset=0;
	char			*ptr;
	//char			varPrefix[40+1];
	
	if (fp != NULL)
		outFp = fp;
	
	UnionGetFunVarReturnValueListIndexFileName(funName,fileName);
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunReturnVarOfFunGrpDisplayFun:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"void ");
	UnionGenerateReturnVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"\\n返回参数::\\n\");\n");
	//UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
	while (!feof(indexFp))
	{
		// 读一行
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionGenerateFunReturnVarOfFunGrpDisplayFun:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		// 从文件中读取取值标识
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarValueTagDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunReturnVarOfFunGrpDisplayFun:: UnionReadVarValueTagDefFromStr [%s]!\n",lineStr);
			continue;
		}
		//UnionPrintOutputVarCSentenceToFp(varPrefix,def.baseTypeTag,def.varValueTag,outFp);
		if (funIndex >= 0)
		{
			if ((ptr = strstr(def.varValueTag,"fun")) != NULL)
				valueTagOffset = abs(ptr - def.varValueTag);
			else
				valueTagOffset = 0;
			def.varValueTag[valueTagOffset+3] = funIndex % 100 / 10 + '0';
			def.varValueTag[valueTagOffset+4] = funIndex % 10 + '0';
		}
		UnionPrintOutputVarCSentenceToFp("",def.baseTypeTag,def.varValueTag,outFp);
	}
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	fclose(indexFp);
	return(0);
}

/*
功能	
	产生一个为函数生成调用函数的函数
输入参数
	funIndex	函数序号
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCallFunOfFunGrpToTestProgram(int funIndex,char *funName,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	TUnionFunDef	funDef;
	char	varPrefix[10+1];
	
	// 读函数定义
	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCallFunOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void ");
	UnionGenerateCallFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"\\n\\n***** 开始执行第%02d个函数%s *****\\n\");\n",funIndex,funName);
	fprintf(outFp,"        ");
	UnionGenerateInputVarSetFunName(funIndex,funName,outFp);
	fprintf(outFp,";\n");
	UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
	fprintf(outFp,"        printf(\"\\n函数执行过程...\\n\");\n");
	fprintf(outFp,"        %sret = %s(",varPrefix,funDef.funName);
	for (index = 0; index < funDef.varNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s%s",varPrefix,funDef.varGrp[index].varDef.name);
	}
	fprintf(outFp,");\n");
	fprintf(outFp,"        ");
	UnionGenerateOutputVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,";\n");
	fprintf(outFp,"        ");
	UnionGenerateReturnVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,";\n");
	fprintf(outFp,"        printf(\"***** 结束执行第%02d个函数%s *****\\n\");\n",funIndex,funName);

	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
功能	
	从文件中读取一个要测试的函数序列
输入参数
	testFileName	测试数据文件名称
	numOfFun	函数清单中可以放的函数的最大数目
输出参数
	funGrp		函数名称清单
返回值
	>=0		读出的函数的数目
	<0		出错代码
*/
int UnionGetFunGrpListFromFile(char *testDataFileName,char funGrp[][128+1],int numOfFun)
{
	FILE			*valueFp;
	char			lineStr[2048+1];
	int			lineLen;
	char			fullTestDataFileName[256+1];
	int			realFunNum = 0;
	
	// 打开测试数据文件
	UnionGetFullFileNameOfTestData(testDataFileName,fullTestDataFileName);	
	if ((valueFp = fopen(fullTestDataFileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGetFunGrpListFromFile:: fopen [%s]!\n",fullTestDataFileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(valueFp))
	{
		// 读一行
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(valueFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionGetFunGrpListFromFile:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		if (strstr(lineStr,"valueOf::") == NULL)
			continue;	// 不是函数的定义
		if (realFunNum >= numOfFun)
		{
			UnionAuditLog("in UnionGetFunGrpListFromFile:: sizeOfFunGrp [%d] is too small, and the left fun not read!\n",numOfFun);
			fclose(valueFp);
			return(realFunNum);
		}
		strcpy(funGrp[realFunNum],lineStr+strlen("valueOf::"));
		realFunNum++;
	}
	fclose(valueFp);
	return(realFunNum);
}

/*
功能	
	生成一个主函数
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMainFunOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if (fp != NULL)
		outFp = fp;
		
	fprintf(outFp,"void main()\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"         // 调用初始化输入参数函数\n");
	//fprintf(outFp,"         #ifdef _initGlobal_\n");
	//fprintf(outFp,"         UnionInitGlobalVar();\n");
	//fprintf(outFp,"         #endif _initGlobal_\n");	
	fprintf(outFp,"         UnionMallocForPointerFunVar();\n");
	fprintf(outFp,"         // 调用函数\n");
	if (numOfFun < 1)
	{
		fprintf(outFp,"         ");
		UnionGenerateCallFunName(-1,"",outFp);
		fprintf(outFp,";\n");
	}
	else
	{
		for (index = 0; index < numOfFun; index++)
		{
			fprintf(outFp,"         ");
			UnionGenerateCallFunName(index,funGrp[index],outFp);
			fprintf(outFp,";\n");
		}
	}
	fprintf(outFp,"         // 调用释放输入参数函数\n");
	fprintf(outFp,"         UnionFreeForPointerFunVar();\n");

	fprintf(outFp,"         return;\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
功能	
	为一组函数自动生成一组处理函数
输入参数
	testFileName	测试数据文件名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAutoGenerateFunForFunGrp(char *testFileName,FILE *fp)
{
	int		ret;
	int		index;
	char 		funGrp[128][128+1];
	int 		numOfFun;
	
	// 写程序的生成日期
	UnionAddAutoGenerateInfoToTestProgram(fp);

	// 写包含的头文件信息
	if ((ret = UnionAddIncludeFileToTestProgram(fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionAddIncludeFileToTestProgram!\n");
		return(ret);
	}
	// 写固定包括的代码
	if ((ret = UnionAddFixedCodesToFile(fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionAddFixedCodesToFile!\n");
		return(ret);
	}
	// 读出函数清单
	if ((numOfFun = UnionGetFunGrpListFromFile(testFileName,funGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGetFunGrpListFromFile from file [%s]\n",testFileName);
		return(numOfFun);
	}
	if (numOfFun == 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: no fun set in file [%s]\n",testFileName);
		return(errCodeTestFileContentError);
	}
	
	// 设置数组的缺省大小
	UnionSetDefaultArraySizeWhenNoSizeSet(conDefaultArraySize);
	// 定义函数变量
	if ((ret = UnionAddFunDefAndFunVarListOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionAddFunDefAndFunVarListOfFunGrpToTestProgram\n");
		return(ret);
	}
	// 写一个为指针型函数变量初始化空间的函数
	if ((ret = UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram\n");
		return(ret);
	}
	// 写一个释放指针型函数变量空间的函数
	if ((ret = UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram\n");
		return(ret);
	}
	// 写一个函数生成测试输入输出索引文件
	if ((ret = UnionGenerateFunVarValueTagListIndexFileOfFunGrp(funGrp,numOfFun)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunVarValueTagListIndexFileOfFunGrp\n");
		return(ret);
	}
	for (index = 0; index < numOfFun; index++)
	{
		// 产生一个对函数的输出参数进行展示的函数
		if ((ret = UnionGenerateFunInputVarOfFunGrpAssignmentFun(index,funGrp[index],testFileName,fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunInputVarOfFunGrpAssignmentFun\n");
			return(ret);
		}
		// 产生一个对函数的输出参数进行展示的函数
		if ((ret = UnionGenerateFunOutputVarOfFunGrpDisplayFun(index,funGrp[index],fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunOutputVarOfFunGrpDisplayFun\n");
			return(ret);
		}
		// 产生一个对函数的返回参数进行展示的函数
		if ((ret = UnionGenerateFunReturnVarOfFunGrpDisplayFun(index,funGrp[index],fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunReturnVarOfFunGrpDisplayFun\n");
			return(ret);
		}
		// 产生一个为函数生成调用函数的函数
		if ((ret = UnionGenerateCallFunOfFunGrpToTestProgram(index,funGrp[index],fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateCallFunOfFunGrpToTestProgram\n");
			return(ret);
		}
	}
	// 写一个主函数
	if ((ret = UnionGenerateMainFunOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateMainFunOfFunGrpToTestProgram\n");
		return(ret);
	}
	return(0);
}

/*
功能	
	为一个函数序列产生一个测试程序
输入参数
	cFileName	文件名称
	testDataFileName	测试数据文件名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAutoGenerateFunForFunGrpToSpecCFile(char *testDataFileName,char *cFileName)
{
	int		ret;
	FILE		*fp = stdout;
	
	if ((cFileName != NULL) && (strlen(cFileName) != 0) && (strcmp(cFileName,"null") != 0))
	{
		if ((fp = fopen(cFileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionAutoGenerateFunForFunGrpToSpecCFile:: fopen [%s] for writting error!\n",cFileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionAutoGenerateFunForFunGrp(testDataFileName,fp)) < 0)
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrpToSpecCFile:: UnionAutoGenerateFunForFunGrp from [%s] to [%s]\n",testDataFileName,cFileName);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
	
