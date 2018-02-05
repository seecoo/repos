// Author:	王纯军
// Date:	2010-6-2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "unionErrCode.h"

#include "mngSvrAutoAppOnlineLogWriter.h"

/*
功能
	拼装由指定方法形成的记录串
输入参数：
	pwriterDef	方法定义
	methodTag	方法标识
	sizeOfBuf	缓冲区大小
输出参数
	recStr		记录串
返回值
	>=0	成功，记录串的长度
	<0	错误码
*/
int UnionFormAutoAppOnlineLogRecStr(PUnionAutoAppOnlineLogWriter pwriterDef,int methodTag,char *recStr,int sizeOfBuf)
{
	int				index;
	int				ret;
	char				tmpBuf[4196+1];
	PUnionAutoAppOnlineLogFldDef	pfldDef;
	int				lenOfRecStr = 0;
	char				method[128+1];
	int				fldLen;
	
	if (pwriterDef == NULL)
		return(errCodeParameter);
	
	for (index = 0; index < pwriterDef->fldNum; index++)
	{
		pfldDef = &(pwriterDef->desTableFldGrp[index]);
		if (pfldDef->methodTag != methodTag)
			continue;
		// 读取域值
		memset(tmpBuf,0,sizeof(tmpBuf));
		switch (pfldDef->methodTag)
		{
			case	conAutoAppOnlineLogFldMethodReadReqStrFld:
				fldLen = UnionReadFldFromCurrentMngSvrClientReqStr(pfldDef->oriFldName,tmpBuf,sizeof(tmpBuf));
				break;
			case	conAutoAppOnlineLogFldMethodReadResStrFld:
				fldLen = UnionReadFldFromCurrentMngSvrClientResStr(pfldDef->oriFldName,tmpBuf,sizeof(tmpBuf));
				break;
			default:
				continue;
		}				
		if (fldLen < 0)
		{
			UnionUserErrLog("in UnionFormAutoAppOnlineLogRecStr:: UnionReadValueUsingSpecMethod method = [%d] fldName = [%s]\n",pfldDef->methodTag,pfldDef->oriFldName);
			if (methodTag == conAutoAppOnlineLogFldMethodReadReqStrFld)
				return(fldLen);
			else
				continue;
		}
		if ((ret = UnionPutRecFldIntoRecStr(pfldDef->desFldName,tmpBuf,fldLen,recStr+lenOfRecStr,sizeOfBuf-lenOfRecStr)) < 0)
		{
			UnionUserErrLog("in UnionFormAutoAppOnlineLogRecStr:: UnionPutRecFldIntoRecStr fldName = [%s] [%04d][%s]\n",pfldDef->desFldName,fldLen,tmpBuf);
			return(ret);
		}
		lenOfRecStr += ret;
	}
	return(lenOfRecStr);
}

/*
功能
	拼装记录请求串
输入参数：
	pwriterDef	方法定义
	sizeOfBuf	缓冲区大小
输出参数
	recStr		记录串
返回值
	>=0	成功，记录串的长度
	<0	错误码
*/
int UnionFormAutoAppOnlineLogRecStrForRequest(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf)
{
	return(UnionFormAutoAppOnlineLogRecStr(pwriterDef,conAutoAppOnlineLogFldMethodReadReqStrFld,recStr,sizeOfBuf));
}

/*
功能
	拼装记录响应串
输入参数：
	pwriterDef	方法定义
	sizeOfBuf	缓冲区大小
输出参数
	recStr		记录串
返回值
	>=0	成功，记录串的长度
	<0	错误码
*/
int UnionFormAutoAppOnlineLogRecStrForResponse(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf)
{
	return(UnionFormAutoAppOnlineLogRecStr(pwriterDef,conAutoAppOnlineLogFldMethodReadResStrFld,recStr,sizeOfBuf));
}

/*
功能
	从一个串，读取域赋值方法
输入参数：
	fldDefStr	域赋值串
	lenOfFldDefStr	域赋值串长度
输出参数
	pfldDef		域赋值定义
返回值
	>=0	成功,返回域定义的数量
	<0	错误码
*/
int UnionInitAutoAppOnlineLogFldWriterFromDefStr(char *fldDefStr,int lenOfFldDefStr,PUnionAutoAppOnlineLogFldDef pfldDef)
{
	int	ret;
	char	fldGrp[3][128];
	int	fldNum;
	int	fldIndex = 0;
	
	if (pfldDef == NULL)
		return(errCodeNullPointer);

	memset(fldGrp,0,sizeof(fldGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(fldDefStr,lenOfFldDefStr,'.',fldGrp,3)) < 0)
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogFldWriterFromDefStr:: UnionSeprateVarStrIntoVarGrp [%04d][%s]!\n",lenOfFldDefStr,fldDefStr);
		return(fldNum);
	}
	if (fldNum == 0)
		return(0);
	fldGrp[0][40] = 0;
	strcpy(pfldDef->desFldName,fldGrp[0]);
	if (fldNum == 1)	// 只定义了流水表的域名
	{
		pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadReqStrFld;
		strcpy(pfldDef->oriFldName,pfldDef->desFldName);
		return(1);
	}
	
	if (strlen(fldGrp[1]) == 0)	// 采用缺省的取值方法
		strcpy(fldGrp[1],"0");
	if (!UnionIsDigitStr(fldGrp[1]))	// 第二个域是源域的定义
	{
		if ((strcmp(fldGrp[1],"reqFld") == 0) || (strcmp(fldGrp[1],"mngSvrReqFld") == 0))
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadReqStrFld;
		else if ((strcmp(fldGrp[1],"resFld") == 0) || (strcmp(fldGrp[1],"mngSvrResFld") == 0))
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadResStrFld;
		else if ((strcmp(fldGrp[1],"tblFld") == 0) || (strcmp(fldGrp[1],"tableFld") == 0))
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadTableFld;
		else
		{
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadReqStrFld;
			fldGrp[1][40] = 0;
			strcpy(pfldDef->oriFldName,fldGrp[1]);
			return(1);
		}
	}
	else
	{
		// 第二个域是取值方法
		pfldDef->methodTag = atoi(fldGrp[1]);
	}
	if (fldNum == 2)	// 未定义源域
	{
		strcpy(pfldDef->oriFldName,pfldDef->desFldName);
		return(1);
	}
	fldGrp[2][40] = 0;
	strcpy(pfldDef->oriFldName,fldGrp[2]);
	return(1);
}

/*
功能
	根据授权记录初始化自动日志生成器
输入参数：
	serviceID	操作代码
输出参数
	pwriterDef	日志生成器定义
返回值
	>=0	成功,返回域的数目
	<0	错误码
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRec(PUnionOperationAuthorization pauthRec,PUnionAutoAppOnlineLogWriter pwriterDef)
{
	int	ret;
	int	index;
	char	fldGrp[conMaxFldNumOfAutoAppOnlineLogRec][128];
	int	fldNum;
	
	if ((pauthRec == NULL) || (pwriterDef == NULL))
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRec:: null pointer!\n");
		return(errCodeNullPointer);
	}
	if ((strlen(pauthRec->onlineLogTableName) == 0) || (strlen(pauthRec->fldAssignDef) == 0))	// 不对这个服务进行自动流水登记
	{
		return(0);
	}
	memset(pwriterDef,0,sizeof(*pwriterDef));
	strcpy(pwriterDef->serviceName,pauthRec->serviceName);
	strcpy(pwriterDef->oriTableName,pauthRec->tableName);
	strcpy(pwriterDef->desTableName,pauthRec->onlineLogTableName);
	// 折分流水表的域定义
	memset(fldGrp,0,sizeof(fldGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(pauthRec->fldAssignDef,strlen(pauthRec->fldAssignDef),',',fldGrp,conMaxFldNumOfAutoAppOnlineLogRec)) < 0)
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRec:: UnionSeprateVarStrIntoVarGrp [%s]!\n",pauthRec->fldAssignDef);
		return(fldNum);
	}
	if (fldNum > conMaxFldNumOfAutoAppOnlineLogRec)
		fldNum = conMaxFldNumOfAutoAppOnlineLogRec;
	// 逐一解析域的赋值方法
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionInitAutoAppOnlineLogFldWriterFromDefStr(fldGrp[index],strlen(fldGrp[index]),&(pwriterDef->desTableFldGrp[index]))) < 0)
		{
			UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRec:: UnionInitAutoAppOnlineLogFldWriterFromDefStr [%s] of index [%d] of [%s]!\n",fldGrp[index],index,pauthRec->fldAssignDef);
			return(ret);
		}
	}
	pwriterDef->fldNum = fldNum;
	return(fldNum);
}

