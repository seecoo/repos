// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionPackAndFunRelation.h"
#include "UnionLog.h"

/*
功能	
	获得一个报文和函数对应关系对应的服务代码
输入参数
	appID		报文的类别
	datagramID	报文的标识
输出参数
	fileName	获得的报文定义的文件名称
返回值
	>= 0	成功
	<0	出错代码
*/
int UnionGetServiceIDFromPackAndFunRelationDef(char *funName,char *serviceID)
{
	int	ret;
	TUnionPackAndFunRelation	def;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetServiceIDFromPackAndFunRelationDef:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	if (serviceID != NULL)
		strcpy(serviceID,def.serviceID);
	return(0);
}


/*
功能	
	获得一个报文定义的文件名
输入参数
	appID		报文的类别
	datagramID	报文的标识
输出参数
	fileName	获得的报文定义的文件名称
返回值
	无
*/
void UnionGetFileNameOfPackAndFunRelationDef(char *funName,char *fileName)
{
	UnionGetDefaultFileNameOfFunDef(funName,fileName);
	return;
}

/*
功能	
	从一个数据串中读取一个报文域与函数参数对应关系定义
输入参数
	str		域定义串
	lenOfStr	串长度
输出参数
	pdef		读出的报文域与函数参数对应关系定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadPackFldAndFunVarRelationFromStr(char *str,int lenOfStr,PUnionPackFldAndFunVarRelation pdef)
{
	int	ret;

	if (pdef == NULL)
		return(errCodeParameter);
	memset(pdef,0,sizeof(*pdef));
	// 报文域标识
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagPackFldID,pdef->packFldID,sizeof(pdef->packFldID))) < 0)
	{
		UnionUserErrLog("in UnionReadPackFldAndFunVarRelationFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPackFldAndFunVarRelationTagPackFldID);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadPackFldAndFunVarRelationFromStr:: conPackFldAndFunVarRelationTagPackFldID [%s]! not defined!\n",conPackFldAndFunVarRelationTagPackFldID);
		return(errCodeCDPMDL_DatagramFldTagNotDefined);
	}
	// 值类型
	if (UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagPackFldType,pdef->fldType,sizeof(pdef->fldType)) < 0)
		strcpy(pdef->fldType,"string");
	// 函数值标识
	UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagVarValueTag,pdef->varValueTag,sizeof(pdef->varValueTag));
	// 说明
	UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagRemark,pdef->remark,sizeof(pdef->remark));
	// 是否是报文向函数输出参数赋值
	UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagIsPackToFun,&(pdef->isPackToFun));
	// 读域长度的指示变量的值
	UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagVarNameOfReaLen,pdef->varNameOfRealLen,sizeof(pdef->varNameOfRealLen));
	// 读是否可选域
	if (UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagIsOptional,&(pdef->isOptional)) < 0)
		pdef->isOptional = 0;
	return(0);
}

/*
功能	
	从表中读取一个报文与函数的转换关系
输入参数
	funName		函数名称
	datagramID	报文的标识
输出参数
	pdef		读出的报文定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadSpecPackAndFunRelationFromFile(char *funName,PUnionPackAndFunRelation pdef)
{
	char			fileName[256+1];
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			lineNum = 0;
	int			ret;
	
	if ((funName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// 打开定义文件
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfPackAndFunRelationDef(funName,fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSpecPackAndFunRelationFromFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// 从文件中读取定义
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		if (pdef->relationNum >= conMaxNumOfPackFldAndFunVarRelation)
		{
			UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: too many datagram fld [%d] defined!\n",pdef->relationNum);
			ret = errCodeCDPMDL_TooManyRequestDatagramFld;
			goto errExit;
		}
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeFileEnd)
				continue;
			UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d] ret = [%d]\n",lineNum,lineLen);
			ret = lineLen;
			goto errExit;
		}
		lineNum++;
		if (lineLen == 0)	// 空行
			continue;
		if (strncmp(lineStr,conPackAndFunRelatioTagServiceID,strlen(conPackAndFunRelatioTagServiceID)) == 0)
		{
			if ((ret = UnionReadRecFldFromRecStr(lineStr,lineLen,conPackAndFunRelatioTagServiceID,pdef->serviceID,sizeof(pdef->serviceID))) < 0)
			{
				UnionUserErrLog("in UnionReadPackFldAndFunVarRelationFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPackAndFunRelatioTagServiceID);
				goto errExit;
			}
			continue;
		}
		if (strncmp(lineStr,conPackAndFunRelatioTagFunAndPack,strlen(conPackAndFunRelatioTagFunAndPack)) != 0)
			continue;
		// 读关系定义
		if ((ret = UnionReadPackFldAndFunVarRelationFromStr(lineStr+strlen(conPackAndFunRelatioTagFunAndPack),lineLen-strlen(conPackAndFunRelatioTagFunAndPack),&(pdef->relationGrp[pdef->relationNum]))) < 0)
		{
			UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: UnionReadPackFldAndFunVarRelationFromStr [%s]! lineNum = [%04d]\n",lineStr,lineNum);
			goto errExit;
		}
		pdef->relationNum += 1;
	}
	ret = 0;
errExit:
	fclose(fp);
	return(ret);
}

/*
功能	
	打印一个报文域与函数参数转换关系
输入参数
	pdef		要打印的报文域与函数参数对应关系定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintPackFldAndFunVarRelationToFp(PUnionPackFldAndFunVarRelation pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	fprintf(outFp,"%d %d %30s %30s %10s %s %s\n",pdef->isPackToFun,pdef->isOptional,pdef->packFldID,pdef->varValueTag,pdef->fldType,
			pdef->remark,pdef->varNameOfRealLen);
	return(0);
}

/*
功能	
	打印一个报文与函数的转换关系
输入参数
	pdef		要打印的报文定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintPackAndFunRelationToFp(PUnionPackAndFunRelation pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	relationNum;
	int	num = 0;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"报文代号::%s\n",pdef->serviceID);
	fprintf(outFp,"请求域定义::\n");
	for (relationNum = 0,num = 0; relationNum < pdef->relationNum; relationNum++)
	{
		if (pdef->relationGrp[relationNum].isPackToFun)
			continue;
		UnionPrintPackFldAndFunVarRelationToFp(&(pdef->relationGrp[relationNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"无\n");
	fprintf(outFp,"响应域定义::\n");
	for (relationNum = 0,num = 0; relationNum < pdef->relationNum; relationNum++)
	{
		if (!pdef->relationGrp[relationNum].isPackToFun)
			continue;
		UnionPrintPackFldAndFunVarRelationToFp(&(pdef->relationGrp[relationNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"无\n");	
	return(0);
}

/*
功能	
	打印一个报文与函数的转换关系
输入参数
	funName		函数名称
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecPackAndFunRelationToFp(char *funName,FILE *fp)
{
	TUnionPackAndFunRelation	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecPackAndFunRelationToFp:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	return(UnionPrintPackAndFunRelationToFp(&def,fp));
}

/*
功能	
	打印一个报文与函数的转换关系到指定文件
输入参数
	funName		函数名称
	fileName	文件名称
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecPackAndFunRelationToSpecFile(char *funName,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintSpecPackAndFunRelationToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionPrintSpecPackAndFunRelationToFp(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecPackAndFunRelationToSpecFile:: UnionPrintSpecPackAndFunRelationToFp!\n");
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);	
}

