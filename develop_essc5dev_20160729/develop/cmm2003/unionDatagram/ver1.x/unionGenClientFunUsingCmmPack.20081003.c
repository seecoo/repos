//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionVarValue.h"
#include "unionFunDef.h"
#include "unionGenFunTestProgram.h"
#include "unionAutoGenFileFunGrp.h"
#include "unionErrCode.h"
#include "unionPackAndFunRelation.h"
#include "unionGenClientFunUsingCmmPack.h"
#include "UnionLog.h"

/*
功能	
	读取一个函数的输入参数的值类型
输入参数
	funName		函数名称
	varValueTag	值类型
	inputOrOutput	是读输入还是输出，1是输入参数，2是输出参数，3是返回值
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadTypeOfSpecVarValueTag(char *funName,char *varValueTag,int inputOrOutputOrReturn)
{
	int			ret;
	char			fileName[256+1];
	FILE			*indexFp;
	char			lineStr[512+1];
	int			lineLen;
	TUnionVarValueTagDef	def;
	
	if (inputOrOutputOrReturn == 1)
		UnionGetFunVarInputValueListIndexFileName(funName,fileName);
	else if (inputOrOutputOrReturn == 2)
		UnionGetFunVarOutputValueListIndexFileName(funName,fileName);
	else if (inputOrOutputOrReturn == 3)
		UnionGetFunVarReturnValueListIndexFileName(funName,fileName);
	else
	{
		UnionUserErrLog("in UnionReadTypeOfSpecVarValueTag:: invalid inputOrOutputOrReturn [%d]\n",inputOrOutputOrReturn);
		return(errCodeParameter);
	}
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadTypeOfSpecVarValueTag:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(indexFp))
	{
		// 读一行
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionReadTypeOfSpecVarValueTag:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		// 从文件中读取取值标识
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarValueTagDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionUserErrLog("in UnionReadTypeOfSpecVarValueTag:: UnionReadVarValueTagDefFromStr [%s]!\n",lineStr);
			continue;
		}
		if (strcmp(def.varValueTag,varValueTag) == 0)
		{
			fclose(indexFp);
			return(def.baseTypeTag);
		}
	}
	fclose(indexFp);
	UnionUserErrLog("in UnionReadTypeOfSpecVarValueTag:: [%s] not defined in [%s]\n",varValueTag,fileName);
	return(errCodeCDPMDL_VarValueTagNotDefined);
}

/*
功能	
	产生一段对报文域进行赋值的代码
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	ppackAndFunDef	域赋值表
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackSetCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;
	int				baseType;
	char				varName[128+1];
	char				fldFunName[128+1];
	char				lenOfVar[256+1];
	char				putFldIntVarName[48+1],reaFldFromVarName[48+1];
	int				blankNum = 0;
	
	UnionSetCmmPackReqAndResVarName(isClientFun,putFldIntVarName,reaFldFromVarName);

	if (ppackAndFunDef == NULL)
	{
		UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: ppackAndFunDef is NULL!\n");
		return(errCodeParameter);
	}
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // 以下对请求报文进行赋值\n");
	if (isClientFun)
	{
		blankNum = 8;
		fprintf(outFp,"        // 初始化请求报文\n");
		fprintf(outFp,"        UnionInitCmmPackFldDataList(%s);\n",putFldIntVarName);
	}
	else
	{
		fprintf(outFp,"        // 初始化响应报文\n");
		fprintf(outFp,"        UnionInitCmmPackFldDataList(%s);\n",putFldIntVarName);
	}
	
	for (index = 0; index < ppackAndFunDef->relationNum; index++)
	{
		if (ppackAndFunDef->relationGrp[index].isPackToFun  == isClientFun)	// 是将报文域赋值为参数
			continue;
		if (strlen(ppackAndFunDef->relationGrp[index].remark) != 0)
		{
			fprintf(outFp,"        // 设置%s\n",ppackAndFunDef->relationGrp[index].remark);
		}
		strcpy(varName,ppackAndFunDef->relationGrp[index].varValueTag);
		switch (baseType = UnionConvertCVarDefKeyWordIntoTag(ppackAndFunDef->relationGrp[index].fldType))
		{
			case	conVarTypeTagChar:
				strcpy(fldFunName,"UnionPutCharTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagUnsignedChar:
				strcpy(fldFunName,"UnionPutUnsignedCharTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagInt:
			case	conVarTypeTagEnum:
				strcpy(fldFunName,"UnionPutIntTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagUnsignedInt:
				strcpy(fldFunName,"UnionPutUnsignedIntTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagLong:
				strcpy(fldFunName,"UnionPutLongTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagUnsignedLong:
				strcpy(fldFunName,"UnionPutUnsignedLongTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagDouble:
				strcpy(fldFunName,"UnionPutDoubleTypeCmmPackFldIntoFldDataList");				
				break;
			default:
				goto complexVar;
		}
		fprintf(outFp,"        if ((ret = %s(%s,%s,%s)) < 0)\n",fldFunName,ppackAndFunDef->relationGrp[index].packFldID,varName,putFldIntVarName);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,fldFunName,ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                goto %s;\n",errExit);
		else
			fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"        }\n");
		continue;
complexVar:
		memset(lenOfVar,0,sizeof(lenOfVar));
		if (strlen(ppackAndFunDef->relationGrp[index].varNameOfRealLen) != 0)
			strcpy(lenOfVar,ppackAndFunDef->relationGrp[index].varNameOfRealLen);
		else
		{
			switch (baseType)
			{
				case	conVarTypeTagString:
					sprintf(lenOfVar,"strlen(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				case	conVarTypeTagVoid:
					UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: [%s] of type [%s] must specified a len!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
					return(errCodeCDPMDL_NotBaseVarType);
				case	conVarTypeTagPointer:
					sprintf(lenOfVar,"sizeof(*(%s))",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				case	conVarTypeTagArray:
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				default:
					goto otherVar;
			}
		}
		if (isClientFun)
		{
			fprintf(outFp,"        if (%s != NULL)\n",ppackAndFunDef->relationGrp[index].varValueTag);
			fprintf(outFp,"        {\n");
		}
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        if ((ret = UnionPutCmmPackFldIntoFldDataList(%s,%s,%s,%s)) < 0)\n",
					ppackAndFunDef->relationGrp[index].packFldID,
					lenOfVar,
					ppackAndFunDef->relationGrp[index].varValueTag,
					putFldIntVarName);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        {\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionPutCmmPackFldIntoFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                goto %s;\n",errExit);
		else
			fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        }\n");
		if (isClientFun)
			fprintf(outFp,"        }\n");
		continue;
otherVar:
		switch (baseType)
		{
			case	conVarTypeTagStruct:
			case	conVarTypeTagUnion:
				if (strlen(lenOfVar) == 0)
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
				break;
			default:
				UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: [%s] of type [%s] not invalid!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
				return(errCodeCDPMDL_NotBaseVarType);
		}
		fprintf(outFp,"        if ((ret = UnionPutCmmPackFldIntoFldDataList(%s,&(%s),%s,%s)) < 0)\n",
				ppackAndFunDef->relationGrp[index].packFldID,
				lenOfVar,
				ppackAndFunDef->relationGrp[index].varValueTag,
				putFldIntVarName);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionPutCmmPackFldIntoFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                goto %s;\n",errExit);
		else
			fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
功能	
	产生一个对报文域进行赋值的函数
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackFldSetCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp)
{
	int				ret;
	TUnionPackAndFunRelation	packAndFunDef;
	
	memset(&packAndFunDef,0,sizeof(packAndFunDef));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&packAndFunDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCmmPackFldSetCodesForFun:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	return(UnionGenerateCmmPackSetCodes(isClientFun,funName,errExit,&packAndFunDef,fp));
}

/*
功能	
	设置请求报文和响应报文变量名称
输入参数
	isClientFun	标识是否是客户端函数
输入出数
	putFldIntVarName	请求报文名称
	reaFldFromVarName	响应报文名称
返回值
	>=0		成功
	<0		出错代码
*/
void UnionSetCmmPackReqAndResVarName(int isClientFun,char *putFldIntVarName,char *reaFldFromVarName)
{
	if (isClientFun)
	{
		strcpy(putFldIntVarName,"&packReq");
		strcpy(reaFldFromVarName,"&packRes");
	}
	else
	{
		strcpy(putFldIntVarName,"ppackRes");
		strcpy(reaFldFromVarName,"ppackReq");
	}
}
		
/*
功能	
	产生一个从报文域读值代码的代码
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	ppackAndFunDef	域赋值表
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackReadCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;
	int				baseType;
	char				varName[128+1];
	char				fldFunName[128+1];
	int				returnValueSet = 0;
	char				lenOfVar[128+1];
	char				putFldIntVarName[48+1],reaFldFromVarName[48+1];
	int				blankNum = 0;
	
	if (isClientFun)
		blankNum = 8;
	UnionSetCmmPackReqAndResVarName(isClientFun,putFldIntVarName,reaFldFromVarName);
	
	if (ppackAndFunDef == NULL)
	{
		UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: ppackAndFunDef is NULL!\n");
		return(errCodeParameter);
	}
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // 以下从报文域读代值\n");
	for (index = 0; index < ppackAndFunDef->relationNum; index++)
	{
		if (ppackAndFunDef->relationGrp[index].isPackToFun != isClientFun)	// 是将报文域赋值为参数的定义
			continue;
		if (isClientFun)
		{
			returnValueSet = 0;
			if (strcmp(ppackAndFunDef->relationGrp[index].varValueTag,"ret") == 0)	// 读返回值
			{
				fprintf(outFp,"        returnValueSet = 1;\n");
				returnValueSet = 1;
			}
		}
		if (strlen(ppackAndFunDef->relationGrp[index].remark) != 0)
		{
			fprintf(outFp,"        // 读取%s\n",ppackAndFunDef->relationGrp[index].remark);
		}
		switch (baseType = UnionConvertCVarDefKeyWordIntoTag(ppackAndFunDef->relationGrp[index].fldType))
		{
			case	conVarTypeTagChar:
				strcpy(fldFunName,"UnionReadCharTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagUnsignedChar:
				strcpy(fldFunName,"UnionReadUnsignedCharTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagInt:
			case	conVarTypeTagEnum:
				strcpy(fldFunName,"UnionReadIntTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagUnsignedInt:
				strcpy(fldFunName,"UnionReadUnsignedIntTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagLong:
				strcpy(fldFunName,"UnionReadLongTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagUnsignedLong:
				strcpy(fldFunName,"UnionReadUnsignedLongTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagDouble:
				strcpy(fldFunName,"UnionReadDoubleTypeCmmPackFldFromFldDataList");				
				break;
			default:
				goto complexVar;
		}
		fprintf(outFp,"        if ((ret = %s(%s,%s,&(%s))) < 0)\n",fldFunName,reaFldFromVarName,ppackAndFunDef->relationGrp[index].packFldID,ppackAndFunDef->relationGrp[index].varValueTag);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                if (!%d)\n",ppackAndFunDef->relationGrp[index].isOptional);
		fprintf(outFp,"                {\n");
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,fldFunName,ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                        goto %s;\n",errExit);
		else
			fprintf(outFp,"                        return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"                }\n");
		fprintf(outFp,"        }\n");
		if (isClientFun)
		{
			if (returnValueSet)
			{
				fprintf(outFp,"        if (returnValueSet)\n");
				fprintf(outFp,"                returnValue = ret;\n");
			}
		}
		continue;
complexVar:
		memset(lenOfVar,0,sizeof(lenOfVar));
		if (strlen(ppackAndFunDef->relationGrp[index].varNameOfRealLen) != 0)
			strcpy(lenOfVar,ppackAndFunDef->relationGrp[index].varNameOfRealLen);
		else
		{
			switch (baseType)
			{
				case	conVarTypeTagString:
				case	conVarTypeTagVoid:
					UnionUserErrLog("in UnionGenerateCmmPackReadCodes:: [%s] of type [%s] must specified a len!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
					return(errCodeCDPMDL_NotBaseVarType);
				case	conVarTypeTagPointer:
					sprintf(lenOfVar,"sizeof(*(%s))",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				case	conVarTypeTagArray:
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				default:
					goto otherVar;
			}
		}
		if (isClientFun)
		{
			fprintf(outFp,"        if (%s != NULL)\n",ppackAndFunDef->relationGrp[index].varValueTag);
			fprintf(outFp,"        {\n");
		}
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        if ((ret = UnionReadCmmPackFldFromFldDataList(%s,%s,%s,%s)) < 0)\n",
					reaFldFromVarName,
					ppackAndFunDef->relationGrp[index].packFldID,
					lenOfVar,
					ppackAndFunDef->relationGrp[index].varValueTag);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        {\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                if (!%d)\n",ppackAndFunDef->relationGrp[index].isOptional);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                {\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionReadCmmPackFldFromFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                        goto %s;\n",errExit);
		else
			fprintf(outFp,"                        return(UnionTranslateClientErrCode(ret));\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                }\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        }\n");
		if (isClientFun)
			fprintf(outFp,"        }\n");
		continue;
otherVar:
		switch (baseType)
		{
			case	conVarTypeTagStruct:
			case	conVarTypeTagUnion:
				if (strlen(lenOfVar) == 0)
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
				break;
			default:
				UnionUserErrLog("in UnionGenerateCmmPackReadCodes:: [%s] of type [%s] not invalid!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
				return(errCodeCDPMDL_NotBaseVarType);
		}
		fprintf(outFp,"        if ((ret = UnionReadCmmPackFldFromFldDataList(%s,%s,%s,&(%s))) < 0)\n",
				reaFldFromVarName,
				ppackAndFunDef->relationGrp[index].packFldID,
				lenOfVar,
				ppackAndFunDef->relationGrp[index].varValueTag);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                if (!%d)\n",ppackAndFunDef->relationGrp[index].isOptional);
		fprintf(outFp,"                {\n");
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionReadCmmPackFldFromFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                        goto %s;\n",errExit);
		else
			fprintf(outFp,"                        return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"                }\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
功能	
	产生一个从报文域读值代码的代码
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackFldReadCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp)
{
	int				ret;
	TUnionPackAndFunRelation	packAndFunDef;
	
	memset(&packAndFunDef,0,sizeof(packAndFunDef));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&packAndFunDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCmmPackFldReadCodesForFun:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	return(UnionGenerateCmmPackReadCodes(isClientFun,funName,errExit,&packAndFunDef,fp));
}

/*
功能	
	产生变量声明
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackAndFunVarDeclareCodes(FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"{\n");
	fprintf(outFp,"        int               ret;\n");
	fprintf(outFp,"        int               returnValueSet = 0;\n");
	fprintf(outFp,"        int               returnValue;\n");
	fprintf(outFp,"        TUnionCmmPackData packReq;  // 请求报文\n");
	fprintf(outFp,"        TUnionCmmPackData packRes;  // 响应报文\n");
	return(0);
}

/*
功能	
	产生与服务器通讯的代码
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCommWithCmmPackSvrCodes(char *funName,FILE *fp)
{
	FILE	*outFp = stdout;
	char	serviceID[100];
	int	ret;
	
	if (fp != NULL)
		outFp = fp;

	memset(serviceID,0,sizeof(serviceID));
	if ((ret = UnionGetServiceIDFromPackAndFunRelationDef(funName,serviceID)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCommWithCmmPackSvrCodes:: UnionGetServiceIDFromPackAndFunRelationDef [%s]!\n",funName);
		return(ret);
	}
	fprintf(outFp,"\n        // 与服务器通讯\n");
	fprintf(outFp,"        // 初始化响应报文\n");
	fprintf(outFp,"        UnionInitCmmPackFldDataList(&packRes);\n");
	fprintf(outFp,"        if ((ret = UnionCommWithCmmPackSvr(%d,&packReq,&packRes)) < 0)\n",atoi(serviceID));
	fprintf(outFp,"        {\n");
	fprintf(outFp,"                UnionUserErrLog(\"in %s:: UnionCommWithCmmPackSvr! ret = [%%d]!\\n\",ret);\n",funName);
	fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
	fprintf(outFp,"        }\n");
	return(0);
}	

/*
功能	
	生成一个客户端函数
输入参数
	funName		函数名称
	incConfFileName	生成的函数要使用的头文件
	fp		文件名柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateClientFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp)
{
	int		ret;
	FILE		*outFp = stdout;
	TUnionFunDef	funDef;
	int		index;
	
	// 读函数定义
	memset(&funDef,0,sizeof(&funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}

	if (fp != NULL)
		outFp = fp;

	if ((incConfFileName != NULL) && (strlen(incConfFileName) > 0))
	{
		// 写包含的头文件信息
		if ((ret = UnionAddIncludeFileToFile(incConfFileName,outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionAddIncludeFileToFile!\n");
			goto errExit;
		}
		// 包含打包文件
		fprintf(outFp,"#ifndef _unionCmmPackData_\n");
		fprintf(outFp,"#include \"unionCmmPackData.h\"\n");
		fprintf(outFp,"#endif\n");
		fprintf(outFp,"\n");
	}
	
	// 将函数定义写入到程序中
	if ((ret = UnionPrintFunDefToFp(&funDef,1,0,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionPrintFunDefToFp [%s]!\n",funName);
		goto errExit;
	}
	/*
	// 写一个函数生成测试输入输出索引文件
	if ((ret = UnionGenerateFunVarValueTagListIndexFile(&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateFunVarValueTagListIndexFile [%s]!\n",funName);
		goto errExit;
	}
	*/
	// 产生变量声明代码
	if ((ret = UnionGenerateCmmPackAndFunVarDeclareCodes(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCmmPackAndFunVarDeclareCodes [%s]!\n",funName);
		goto errExit;
	}

	// 产生报文赋值代码
	if ((ret = UnionGenerateCmmPackFldSetCodesForFun(1,funName,NULL,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCmmPackFldSetCodesForFun [%s]!\n",funName);
		goto errExit;
	}

	// 产生与服务器通讯的代码
	if ((ret = UnionGenerateCommWithCmmPackSvrCodes(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCommWithCmmPackSvrCodes [%s]!\n",funName);
		goto errExit;
	}

	// 产生从报文读值的代码
	if ((ret = UnionGenerateCmmPackFldReadCodesForFun(1,funName,NULL,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCmmPackFldReadCodesForFun [%s]!\n",funName);
		goto errExit;
	}
	
	// 产生结尾代码
	fprintf(outFp,"\n");
	fprintf(outFp,"        // 设置了返回值\n");
	fprintf(outFp,"        if (returnValueSet)\n");
	fprintf(outFp,"                 return(UnionTranslateClientErrCode(returnValue));\n");
	fprintf(outFp,"        return(UnionTranslateClientErrCode(ret));\n");
	fprintf(outFp,"}\n\n");
errExit:
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

/*
功能	
	生成一个客户端函数
输入参数
	funName		函数名称
	toNewFile	是否写到新文件中
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateClientFunUsingCmmPack(char *funName,int toNewFile)
{
	char	fileName[256+1];
	char	incFileConf[256+1];
	int	ret;
	FILE	*fp;
		
	memset(fileName,0,sizeof(fileName));
	memset(incFileConf,0,sizeof(incFileConf));
	if ((ret = UnionGetFullProgramFileNameOfFun(funName,fileName,incFileConf)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPack:: UnionGetFullProgramFileNameOfFun [%s]!\n",funName);
		return(ret);
	}
	if (!toNewFile)
	{
		fp = fopen(fileName,"a");
		memset(incFileConf,0,sizeof(incFileConf));
	}	
	else
		fp = fopen(fileName,"w");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionGenerateClientFunUsingCmmPack:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateClientFunUsingCmmPackToFp(funName,incFileConf,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPack:: UnionGenerateClientFunUsingCmmPackToFp [%s]!\n",funName);
		fclose(fp);
		return(ret);
	}
	fclose(fp);
	return(0);
}

/*
功能	
	为指定的程序生成所有客户端API
输入参数
	nameOfProgram	程序名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateAllCmmPackClientAPIInSpecProgram(char *nameOfProgram)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	fileName[512+1];
	int	funNum = 0,failNum = 0;
	TUnionVarTypeDef	def;
	
	if (nameOfProgram == NULL)
		return(errCodeParameter);
	// 获得文件名称
	UnionGetDefaultFileNameOfVarTypeTagDef("",fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(&def,0,sizeof(def));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (def.typeTag != conDesginKeyWordTagFun)	// 不是函数
			continue;
		if (strcmp(nameOfProgram,def.nameOfProgram) != 0)	// 不是这个程序包括的函数
			continue;
		if ((ret = UnionGenerateClientFunUsingCmmPack(def.nameOfType,!funNum)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: UnionGenerateClientFunUsingCmmPack [%s]!\n",def.nameOfType);
			printf("函数生成失败::%s!\n",def.nameOfType);
			failNum++;
		}
		else
		{
			printf("函数生成成功::%s!\n",def.nameOfType);
			funNum++;
		}
	}
	fclose(fp);
	UnionLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: [%d] functions generated!\n",funNum);
	printf("生成成功的函数数目::%04d\n",funNum);
	printf("生成失败的函数数目::%04d\n",failNum);
	return(funNum);
}

/*
功能	
	生成一个服务端函数
输入参数
	funName		函数名称
	incConfFileName	生成的函数要使用的头文件
	fp		文件名柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateServerFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp)
{
	int		ret;
	FILE		*outFp = stdout;
	TUnionFunDef	funDef;
	int		index;
	
	// 读函数定义
	memset(&funDef,0,sizeof(&funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}

	if (fp != NULL)
		outFp = fp;

	if ((incConfFileName != NULL) && (strlen(incConfFileName) > 0))
	{
		// 写包含的头文件信息
		if ((ret = UnionAddIncludeFileToFile(incConfFileName,outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionAddIncludeFileToFile!\n");
			goto errExit;
		}
		// 包含打包文件
		fprintf(outFp,"#ifndef _unionCmmPackData_\n");
		fprintf(outFp,"#include \"unionCmmPackData.h\"\n");
		fprintf(outFp,"#endif\n");
		fprintf(outFp,"\n");
	}
	
	// 将函数定义写入到程序中
	fprintf(outFp,"int %sAtSvr(PUnionCmmPackData ppackReq,PUnionCmmPackData ppackRes)\n",funDef.funName);
	fprintf(outFp,"{\n");
	// 设置数组的缺省大小
	UnionSetDefaultArraySizeWhenNoSizeSet(8192);
	// 定义函数变量
	if ((ret = UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(8,&funDef,"ret",8192,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank [%s]!\n",funName);
		return(ret);
	}
	fprintf(outFp,"        int returnValue;\n");
	fprintf(outFp,"        int finishOK = 0;\n");
	// 产生对指针变量分配空间的代码
	fprintf(outFp,"\n");
	//UnionSetDefaultArraySizeWhenNoSizeSet(8192);
	if ((ret = UnionGenerateMallocCodesForPointerFunVarToFp(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateMallocCodesForPointerFunVarToFp [%s]!\n",funName);
		return(ret);
	}
	// 产生从报文读值的代码
	if ((ret = UnionGenerateCmmPackFldReadCodesForFun(0,funName,"freePointer",fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateCmmPackFldReadCodesForFun [%s]!\n",funName);
		goto errExit;
	}
	// 产生调用函数的代码
	if ((ret = UnionGenerateCallCodesOfFun(8,"returnValue",&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateCallCodesOfFun [%s]!\n",funName);
		goto errExit;
	}
	fprintf(outFp,"        if (returnValue != 0)\n");
	fprintf(outFp,"        {\n");
	fprintf(outFp,"                 UnionUserErrLog(\"in %sAtSvr:: %s error! ret = %%d\\n\",returnValue);\n",funDef.funName,funDef.funName);
	fprintf(outFp,"                 goto freePointer;\n");
	fprintf(outFp,"        }\n");
	// 产生报文赋值代码
	if ((ret = UnionGenerateCmmPackFldSetCodesForFun(0,funName,"freePointer",fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateCmmPackFldSetCodesForFun [%s]!\n",funName);
		goto errExit;
	}
	fprintf(outFp,"        finishOK = 1;\n");
	// 产生释放指针的代码
	fprintf(outFp,"freePointer:\n");
	if ((ret = UnionGenerateFreeCodesForPointerFunVarToFp(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateFreeCodesForPointerFunVarToFp [%s]!\n",funName);
		return(ret);
	}
	fprintf(outFp,"        if (finishOK)\n");
	fprintf(outFp,"                return(0);\n");
	fprintf(outFp,"        else\n");
	fprintf(outFp,"                return(ret);\n");
	fprintf(outFp,"}\n\n");
errExit:
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

/*
功能	
	生成一个服务端函数
输入参数
	funName		函数名称
	toNewFile	是否写到新文件中
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateServerFunUsingCmmPack(char *funName,int toNewFile)
{
	char	fileName[256+1];
	char	incFileConf[256+1];
	int	ret;
	FILE	*fp;
	int	len;
		
	memset(fileName,0,sizeof(fileName));
	memset(incFileConf,0,sizeof(incFileConf));
	if ((ret = UnionGetFullProgramFileNameOfFun(funName,fileName,incFileConf)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPack:: UnionGetFullProgramFileNameOfFun [%s]!\n",funName);
		return(ret);
	}
	if (((len = strlen(fileName)) < 2) || (strcmp(fileName+len-2,".c") != 0))
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPack:: fileName [%s] too short or not a c file!\n",fileName);
		return(errCodeParameter);
	}
	sprintf(fileName+len-2,".AtSvr.c");
	if (!toNewFile)
	{
		fp = fopen(fileName,"a");
		memset(incFileConf,0,sizeof(incFileConf));
	}	
	else
		fp = fopen(fileName,"w");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionGenerateServerFunUsingCmmPack:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateServerFunUsingCmmPackToFp(funName,incFileConf,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPack:: UnionGenerateServerFunUsingCmmPackToFp [%s]!\n",funName);
		fclose(fp);
		return(ret);
	}
	fclose(fp);
	return(0);
}

/*
功能	
	为指定的程序生成所有服务端API
输入参数
	nameOfProgram	程序名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateAllCmmPackServerAPIInSpecProgram(char *nameOfProgram)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	fileName[512+1];
	int	funNum = 0,failNum = 0;
	TUnionVarTypeDef	def;
	
	if (nameOfProgram == NULL)
		return(errCodeParameter);
	// 获得文件名称
	UnionGetDefaultFileNameOfVarTypeTagDef("",fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(&def,0,sizeof(def));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (def.typeTag != conDesginKeyWordTagFun)	// 不是函数
			continue;
		if (strcmp(nameOfProgram,def.nameOfProgram) != 0)	// 不是这个程序包括的函数
			continue;
		if ((ret = UnionGenerateServerFunUsingCmmPack(def.nameOfType,!funNum)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: UnionGenerateServerFunUsingCmmPack [%s]!\n",def.nameOfType);
			printf("函数生成失败::%s!\n",def.nameOfType);
			failNum++;
		}
		else
		{
			printf("函数生成成功::%s!\n",def.nameOfType);
			funNum++;
		}
	}
	fclose(fp);
	UnionLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: [%d] functions generated!\n",funNum);
	printf("生成成功的函数数目::%04d\n",funNum);
	printf("生成失败的函数数目::%04d\n",failNum);
	return(funNum);
}

