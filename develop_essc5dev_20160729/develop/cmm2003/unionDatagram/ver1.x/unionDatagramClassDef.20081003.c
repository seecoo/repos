// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionDatagramClassDef.h"

#include "UnionLog.h"
/*
功能	
	获得应用报文类型定义的文件名
输入参数
	无
输出参数
	fileName	获得的应用报文类型定义的文件名称
返回值
	无
*/
void UnionGetFileNameOfDatagramClassDef(char *fileName)
{
	sprintf(fileName,"%s/datagram/datagramClass.def",getenv("UNIONDESIGNDIR"));
}

/*
功能	
	获得应用报文类型对应的服务端模块标识
输入参数
	appID			应用标识
输出参数
	nameOfModuleOfSvr	服务端模块标识
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetNameOfModuleSvrOfSpecDatagramClass(char *appID,char *nameOfModuleOfSvr)
{
	int	ret;
	TUnionDatagramClassDef	def;
	
	if ((ret = UnionReadDatagramClassDefFromFile(appID,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetNameOfModuleSvrOfSpecDatagramClass:: UnionReadDatagramClassDefFromFile!\n");
		return(ret);
	}
	if (nameOfModuleOfSvr != NULL)
		strcpy(nameOfModuleOfSvr,def.nameOfModuleOfSvr);
	return(0);
}

/*
功能	
	从一个定义口串中读取一个报文类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadDatagramClassDefFromStr(char *str,int lenOfStr,PUnionDatagramClassDef pdef)
{
	int	ret;
	char	classID[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// 应用标识
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagAppID,pdef->appID,sizeof(pdef->appID))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramClassDefTagAppID,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromStr:: appID must be defined!\n");
		return(errCodeCDPMDL_DatagramClassNotDefined);
	}
	memset(classID,0,sizeof(classID));
	// 报文类型
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagClassID,pdef->classID,sizeof(pdef->classID))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramClassDefTagClassID,str);
		return(ret);
	}
	// 说明
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagRemark,pdef->remark,sizeof(pdef->remark));
	// svr端模块
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagNameOfModuleOfSvr,pdef->nameOfModuleOfSvr,sizeof(pdef->nameOfModuleOfSvr));
	return(0);
}


/* 指定名称的报文类型标识的定义
输入参数
	appID	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadDatagramClassDefFromFile(char *appID,PUnionDatagramClassDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	fileName[256+1];
	
	
	if (pdef == NULL)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromFile:: parameter!\n");
		return(errCodeParameter);
	}
	UnionGetFileNameOfDatagramClassDef(fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadDatagramClassDefFromFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionReadDatagramClassDefFromFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if ((ret = UnionReadDatagramClassDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		if (strcmp(appID,pdef->appID) == 0)
		{
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	UnionUserErrLog("in UnionReadDatagramClassDefFromFile:: [%s] not defined in [%s]\n",appID,fileName);
	return(errCodeCDPMDL_DatagramClassNotDefined);
}

/*
功能	
	打印一个报文的类型定义
输入参数
	pdef		要打印的报文类型定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintDatagramClassDefToFp(PUnionDatagramClassDef pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	//fprintf(outFp,"appID=%s|classID=%s|remark=%s|\n",pdef->appID,pdef->classID,pdef->remark);
	fprintf(outFp,"%24s %20s %30s %s \n",pdef->appID,pdef->classID,pdef->nameOfModuleOfSvr,pdef->remark);
	return(0);
}

/*
功能	
	打印一个报文类型的定义
输入参数
	appID		报文用途标识
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramClassDefToFp(char *appID,FILE *fp)
{
	TUnionDatagramClassDef	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadDatagramClassDefFromFile(appID,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramClassDefToFp:: UnionReadDatagramClassDefFromFile!\n");
		return(ret);
	}
	return(UnionPrintDatagramClassDefToFp(&def,fp));
}

/*
功能	
	打印一个报文类型的定义
输入参数
	appID		报文用途标识
	fileName	输出到的文件
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramClassDef(char *appID,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintSpecDatagramClassDef:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionPrintSpecDatagramClassDefToFp(appID,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramClassDef:: UnionPrintSpecDatagramClassDefToFp!\n");
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);	
}
