// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionDatagramDef.h"
#include "UnionLog.h"

/*
功能	
	从一个数据串中读取一个报文的头定义
输入参数
	str		域定义串
	lenOfStr	串长度
输出参数
	serviceID	报文头中的服务代码
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadDatagramHeaderFromStr(char *str,int lenOfStr,char *serviceID)
{
	int	ret;
	
	// 服务代码
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramTagServiceID,serviceID,100)) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramHeaderFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramTagServiceID,str);
		return(ret);
	}
	return(ret);
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
void UnionGetFileNameOfDatagramDef(char *appID,char *datagramID,char *fileName)
{
	sprintf(fileName,"%s/datagram/%s/%s.def",getenv("UNIONDESIGNDIR"),appID,datagramID);
}

/*
功能	
	从一个数据串中读取一个报文域定义
输入参数
	str		域定义串
	lenOfStr	串长度
输出参数
	pdef		读出的报文域定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadDatagramFldDefFromStr(char *str,int lenOfStr,PUnionDatagramFldDef pdef)
{
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	memset(pdef,0,sizeof(*pdef));
	// 域标识
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagFldID,pdef->fldID,sizeof(pdef->fldID))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramFldDefTagFldID,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: conDatagramFldDefTagFldID [%s]! not defined!\n",conDatagramFldDefTagFldID);
		return(errCodeCDPMDL_DatagramFldTagNotDefined);
	}
	// 别名
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagAlais,pdef->alais,sizeof(pdef->alais))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: UnionReadRecFldFromRecStr [%s]  from [%s]!\n",conDatagramFldDefTagAlais,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: conDatagramFldDefTagFldID [%s]! not defined!\n",conDatagramFldDefTagFldID);
		return(errCodeCDPMDL_DatagramFldAlaisNotDefined);
	}
	// 长度
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagLen,&(pdef->len))) < 0)
		pdef->len = 2048;
	if (pdef->len == 0)
		pdef->len = 2048;
	// 域值方法
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagFldValueID,pdef->valueID,sizeof(pdef->valueID));
	// 域说明
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagRemark,pdef->remark,sizeof(pdef->remark));
	// 域请求标识
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagIsRequest,&(pdef->isRequest))) < 0)
		pdef->isRequest = 1;
	UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagOptional,&(pdef->optional));
	return(0);
}

/*
功能	
	从表中读取一个报文的定义
输入参数
	appID		报文的类别
	datagramID	报文的标识
输出参数
	pdef		读出的报文定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadSpecDatagramDefFromFile(char *appID,char *datagramID,PUnionDatagramDef pdef)
{
	char			fileName[256+1];
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			lineNum = 0;
	int			ret;
	
	if ((appID == NULL) || (datagramID == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// 打开定义文件
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfDatagramDef(appID,datagramID,fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSpecDatagramDefFromFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// 从文件中读取定义
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		if (pdef->fldNum >= conMaxNumOfFldPerDatagram)
		{
			UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: too many datagram fld [%d] defined!\n",pdef->fldNum);
			ret = errCodeCDPMDL_TooManyRequestDatagramFld;
			goto errExit;
		}
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeFileEnd)
				continue;
			UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d] ret = [%d]\n",lineNum,lineLen);
			ret = lineLen;
			goto errExit;
		}
		lineNum++;
		if (lineLen == 0)	// 空行
			continue;
		if (strncmp(lineStr,conDatagramTagHeader,strlen(conDatagramTagHeader)) == 0)
		{
			// 报文头
			if ((ret = UnionReadDatagramHeaderFromStr(lineStr+strlen(conDatagramTagHeader),lineLen-strlen(conDatagramTagHeader),pdef->serviceID)) < 0)
			{
				UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: UnionReadDatagramHeaderFromStr from [%s]!\n",lineStr+strlen(conDatagramTagHeader));
				goto errExit;
			}
			continue;
		}			
		// 读域
		if ((ret = UnionReadDatagramFldDefFromStr(lineStr,lineLen,&(pdef->fldGrp[pdef->fldNum]))) < 0)
		{
			UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: UnionReadDatagramFldDefFromStr [%s]! lineNum = [%04d]\n",lineStr,lineNum);
			goto errExit;
		}
		pdef->fldNum += 1;
	}
	ret = 0;
	strcpy(pdef->appID,appID);
	strcpy(pdef->datagramID,datagramID);
	if (strlen(pdef->serviceID) == 0)
	{
		UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: header defined error!\n");
		ret = errCodeCDPMDL_DatagramHeaderDefinedError;
	}
errExit:
	fclose(fp);
	return(ret);
}

/*
功能	
	打印一个报文的域定义
输入参数
	pdef		要打印的报文域定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintDatagramFldDefToFp(PUnionDatagramFldDef pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	//fprintf(outFp,"isRequest=%d|fldID=%s|valueID=%s|remark=%s|\n",pdef->isRequest,pdef->fldID,pdef->valueID,pdef->remark);
	fprintf(outFp,"%24s %16s %4d %16s",pdef->fldID,pdef->alais,pdef->len,pdef->valueID);
	if (pdef->optional)
		fprintf(outFp," 可选");
	else
		fprintf(outFp," 必须");
	fprintf(outFp," %s\n",pdef->remark);
	return(0);
}

/*
功能	
	打印一个报文的定义
输入参数
	pdef		要打印的报文定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintDatagramDefToFp(PUnionDatagramDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	fldNum;
	int	num = 0;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	//fprintf(outFp,"appID=%s|datagramID=%s|\n",pdef->appID,pdef->datagramID);
	fprintf(outFp,"报文用途::%s 报文功能::%s\n",pdef->appID,pdef->datagramID);
	//fprintf(outFp,"idOfApp=%s|serviceID=%s|\n",pdef->idOfAppInDatagram,pdef->serviceID);
	fprintf(outFp,"服务代码::%s\n",pdef->serviceID);
	//fprintf(outFp,"fldNum=%d\n",pdef->fldNum);
	fprintf(outFp,"%24s %16s %4s %16s %4s %s\n","域标识","别名","长度","取值方法","可选","说明");
	fprintf(outFp,"请求域定义::\n");
	for (fldNum = 0,num = 0; fldNum < pdef->fldNum; fldNum++)
	{
		if (!pdef->fldGrp[fldNum].isRequest)
			continue;
		UnionPrintDatagramFldDefToFp(&(pdef->fldGrp[fldNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"无\n");
	fprintf(outFp,"响应域定义::\n");
	for (fldNum = 0,num = 0; fldNum < pdef->fldNum; fldNum++)
	{
		if (pdef->fldGrp[fldNum].isRequest)
			continue;
		UnionPrintDatagramFldDefToFp(&(pdef->fldGrp[fldNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"无\n");	
	return(0);
}

/*
功能	
	打印一个报文的定义
输入参数
	appID		报文用途标识
	datagramID	报文的类别
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramDefToFp(char *appID,char *datagramID,FILE *fp)
{
	TUnionDatagramDef	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecDatagramDefFromFile(appID,datagramID,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramDefToFp:: UnionReadSpecDatagramDefFromFile!\n");
		return(ret);
	}
	return(UnionPrintDatagramDefToFp(&def,fp));
}

/*
功能	
	打印一个报文的定义到指定文件
输入参数
	appID		报文用途标识
	datagramID	报文的类别
	fileName	文件名称
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramDefToSpecFile(char *appID,char *datagramID,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintSpecDatagramDefToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionPrintSpecDatagramDefToFp(appID,datagramID,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramDefToSpecFile:: UnionPrintSpecDatagramDefToFp!\n");
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);	
}

