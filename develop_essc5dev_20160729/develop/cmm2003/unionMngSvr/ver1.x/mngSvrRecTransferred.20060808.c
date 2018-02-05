// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#include <stdio.h>
#include <string.h>

#include "mngSvrRecTransferred.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

// 从字符串中读取记录域
// 返回域值的长度
int UnionReadMngSvrRecFldFromStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf)
{
	char	*ptr;
	int	lenOfValue;
	char	fldTag[100];
	
	if ((recStr == NULL) || (lenOfRecStr < 0) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionReadMngSvrRecFldFromStr:: null pointer or lenOfRecStr = [%d]!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	if (lenOfRecStr == 0)
	{
		UnionAuditLog("in UnionReadMngSvrRecFldFromStr:: fld [%s] not exists!\n",fldName);
		return(errCodeEsscMDL_FldNotExists);
	}
	recStr[lenOfRecStr] = 0;
	sprintf(fldTag,"%s=",fldName);
	if ((ptr = strstr(recStr,fldTag)) == NULL)
	{
		UnionAuditLog("in UnionReadMngSvrRecFldFromStr:: fld [%s] not exists!\n",fldName);
		return(errCodeEsscMDL_FldNotExists);
	}
	if (value == NULL)	// 不需要读数据
		return(0);
	ptr = ptr + strlen(fldTag);
	for (lenOfValue = 0; lenOfValue < lenOfRecStr; lenOfValue++)
	{
		if (ptr[lenOfValue] != '|')
			continue;
		else
			break;
	}
	if (sizeOfBuf <= lenOfValue)
	{
		UnionUserErrLog("in UnionReadMngSvrRecFldFromStr:: sizeOfBuf [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfBuf,lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}
	memcpy(value,ptr,lenOfValue);
	value[lenOfValue] = 0;
	UnionFilterHeadAndTailBlank(value);
	return(strlen(value));
}

// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutMngSvrRecFldIntoStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	
	if ((recStr == NULL) || (lenOfValue < 0) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutMngSvrRecFldIntoStr:: null pointer or lenOfValue = [%d]!\n",lenOfValue);
		return(errCodeParameter);
	}
	value[lenOfValue] = 0;
	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutMngSvrRecFldIntoStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}
	sprintf(recStr,"%s=%s|",fldName,value);
	return(fldTagLen+lenOfValue);
}
