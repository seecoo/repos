// Automatically generated codes
// Generated Date and Time 20100208094822
// Generated by ud090401

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionComplexDBRecord.h"
#include "unionDataTBLList.h"
#include "unionRecFile.h"

#include "unionSecurityServiceDef.h"
#include "simuMngSvrLocally.h"


/*
功能：
	结构的缺省赋值函数
输入参数:
	prec	记录指针
输出参数:
	无
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormDefaultValueOfSecurityServiceDefRec(PUnionSecurityServiceDef prec)
{
	if (prec == NULL)
	{
		UnionUserErrLog("in UnionFormDefaultValueOfSecurityServiceDefRec:: prec is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	memset(prec,0,sizeof(*prec));
	return(0);
}


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	prec	记录指针
	sizeOfBuf	接收记录字串的缓冲的大小
输出参数:
	recStr	记录字串
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormRecStrFromSecurityServiceDefRec(PUnionSecurityServiceDef prec,char *recStr,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormRecStrFromSecurityServiceDefRec:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	if ((ret = UnionFormPrimaryKeyRecStrFromSecurityServiceDefRec(prec,recStr,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormRecStrFromSecurityServiceDefRec:: UnionFormPrimaryKeyRecStrFromSecurityServiceDefRec!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset = ret;
	if ((ret = UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec(prec,recStr+offset,sizeOfBuf-offset)) < 0)
	{
		UnionUserErrLog("in UnionFormRecStrFromSecurityServiceDefRec:: UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	return(offset);
}


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	prec	记录指针
	sizeOfBuf	接收记录字串的缓冲的大小
输出参数:
	recStr	记录字串
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormPrimaryKeyRecStrFromSecurityServiceDefRec(PUnionSecurityServiceDef prec,char *recStr,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormPrimaryKeyRecStrFromSecurityServiceDefRec:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	ret = UnionPutIntTypeRecFldIntoRecStr(conSecurityServiceDefFldNameServiceID,prec->serviceID,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutIntTypeRecFldIntoRecStr [serviceID] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	return(offset);
}


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	prec	记录指针
	sizeOfBuf	接收记录字串的缓冲的大小
输出参数:
	recStr	记录字串
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec(PUnionSecurityServiceDef prec,char *recStr,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameServiceFunction,prec->serviceFunction,strlen(prec->serviceFunction),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [serviceFunction] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameRequestRequiredField,prec->requestRequiredField,strlen(prec->requestRequiredField),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [requestRequiredField] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameRequestOptionalField,prec->requestOptionalField,strlen(prec->requestOptionalField),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [requestOptionalField] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameResponseRequiredField,prec->responseRequiredField,strlen(prec->responseRequiredField),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [responseRequiredField] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameResponseOptionalField,prec->responseOptionalField,strlen(prec->responseOptionalField),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [responseOptionalField] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameRemark,prec->remark,strlen(prec->remark),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [remark] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameInputTeller,prec->inputTeller,strlen(prec->inputTeller),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [inputTeller] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameInputDate,prec->inputDate,strlen(prec->inputDate),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [inputDate] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameInputTime,prec->inputTime,strlen(prec->inputTime),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [inputTime] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameUpdateTeller,prec->updateTeller,strlen(prec->updateTeller),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [updateTeller] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameUpdateDate,prec->updateDate,strlen(prec->updateDate),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [updateDate] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conSecurityServiceDefFldNameUpdateTime,prec->updateTime,strlen(prec->updateTime),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [updateTime] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	return(offset);
}


/*
功能：
	将记录字串转换为记录结构的关键字的函数
输入参数:
	recStr	记录字串
	lenOfRecStr	记录串长度
输出参数:
	prec	记录指针
返回值
	>=0		读出的域数目
	<0		失败,错误码
*/
int UnionFormPrimaryKeyOfSecurityServiceDefRecFromRecStr(char *recStr,int lenOfRecStr,PUnionSecurityServiceDef prec)
{
	int	ret;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormPrimaryKeyOfSecurityServiceDefRecFromRecStr:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	UnionFormDefaultValueOfSecurityServiceDefRec(prec);
	ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameServiceID,&(prec->serviceID));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyOfSecurityServiceDefRecFromRecStr:: UnionReadIntTypeRecFldFromRecStr fld [serviceID] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	return(0);
}


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	recStr	记录字串
	lenOfRecStr	记录串长度
输出参数:
	prec	记录指针
返回值
	>=0		读出的域数目
	<0		失败,错误码
*/
int UnionFormSecurityServiceDefRecFromRecStr(char *recStr,int lenOfRecStr,PUnionSecurityServiceDef prec)
{
	int	ret;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	UnionFormDefaultValueOfSecurityServiceDefRec(prec);
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameServiceFunction,prec->serviceFunction,sizeof(prec->serviceFunction));
	if (ret < 0)
	{
		if (!0)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [serviceFunction] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameServiceID,&(prec->serviceID));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadIntTypeRecFldFromRecStr fld [serviceID] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameRequestRequiredField,prec->requestRequiredField,sizeof(prec->requestRequiredField));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [requestRequiredField] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameRequestOptionalField,prec->requestOptionalField,sizeof(prec->requestOptionalField));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [requestOptionalField] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameResponseRequiredField,prec->responseRequiredField,sizeof(prec->responseRequiredField));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [responseRequiredField] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameResponseOptionalField,prec->responseOptionalField,sizeof(prec->responseOptionalField));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [responseOptionalField] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameRemark,prec->remark,sizeof(prec->remark));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [remark] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameInputTeller,prec->inputTeller,sizeof(prec->inputTeller));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [inputTeller] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameInputDate,prec->inputDate,sizeof(prec->inputDate));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [inputDate] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameInputTime,prec->inputTime,sizeof(prec->inputTime));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [inputTime] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameUpdateTeller,prec->updateTeller,sizeof(prec->updateTeller));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [updateTeller] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameUpdateDate,prec->updateDate,sizeof(prec->updateDate));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [updateDate] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conSecurityServiceDefFldNameUpdateTime,prec->updateTime,sizeof(prec->updateTime));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormSecurityServiceDefRecFromRecStr:: UnionReadRecFldFromRecStr fld [updateTime] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	return(0);
}


/*
功能：
	插入一个记录
输入参数:
	prec	记录指针
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionInsertSecurityServiceDefRec(PUnionSecurityServiceDef prec)
{
	int	ret;
	int	lenOfRecStr;
	char	recStr[8192*4+1];

	//从记录拼装串
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormRecStrFromSecurityServiceDefRec(prec,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertSecurityServiceDefRec:: UnionFormRecStrFromSecurityServiceDefRec!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//赋加自动插入域
	if ((lenOfRecStr = UnionAutoAppendDBRecInputAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertSecurityServiceDefRec:: UnionAutoAppendDBRecInputAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//插入记录
	if ((ret = UnionInsertObjectRecord(conTBLNameSecurityServiceDef,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertSecurityServiceDefRec:: UnionInsertObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}


/*
功能：
	删除一个记录
输入参数:
	serviceID 服务代码
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionDeleteSecurityServiceDefRec(int serviceID)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];

	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutIntTypeRecFldIntoRecStr("serviceID",serviceID,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDeleteSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [serviceID] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	//删除记录
	if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameSecurityServiceDef,recStr)) < 0)
	{
		UnionUserErrLog("in UnionDeleteSecurityServiceDefRec:: UnionDeleteUniqueObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}


/*
功能：
	修改一个记录
输入参数:
	prec	记录指针
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionUpdateSecurityServiceDefRec(PUnionSecurityServiceDef prec)
{
	int	ret;
	int	lenOfRecStr;
	char	recStr[8192*4+1];
	char	primaryKey[1024+1];

	//从记录拼关键字
	memset(primaryKey,0,sizeof(primaryKey));
	if ((ret = UnionFormPrimaryKeyRecStrFromSecurityServiceDefRec(prec,primaryKey,sizeof(primaryKey))) < 0)
	{
		UnionUserErrLog("in UnionUpdateSecurityServiceDefRec:: UnionFormPrimaryKeyRecStrFromSecurityServiceDefRec!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	//从记录接记录串
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec(prec,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateSecurityServiceDefRec:: UnionFormNonPrimaryKeyRecStrFromSecurityServiceDefRec!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//赋加自动修改域
	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateSecurityServiceDefRec:: UnionAutoAppendDBRecUpdateAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//修改记录
	if ((ret = UnionUpdateUniqueObjectRecord(conTBLNameSecurityServiceDef,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSecurityServiceDefRec:: UnionUpdateUniqueObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}
/*
函数功能:
	修改指定的表域:
输入参数:
	serviceID 服务代码
	fldName 域名称
	fldValue 域值
	fldValueLen 域值长度
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionUpdateSpecFldOfSecurityServiceDefRec(int serviceID,char *fldName,char *fldValue,int lenOfFldValue)
{
	int	ret;
	char	recStr[8192*2+1];
	char	primaryKey[1024+1];
	int	lenOfRecStr = 0;

	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutIntTypeRecFldIntoRecStr("serviceID",serviceID,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [serviceID] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	strcpy(primaryKey,recStr);

	// 组记录
	memset(recStr,0,sizeof(recStr));
	lenOfRecStr = 0;
	if ((ret = UnionPutRecFldIntoRecStr(fldName,fldValue,lenOfFldValue,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [%s] failure!\n",fldName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;

	//赋加自动修改域
	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfSecurityServiceDefRec:: UnionAutoAppendDBRecUpdateAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	// 修改记录
	if ((ret = UnionUpdateUniqueObjectRecord(conTBLNameSecurityServiceDef,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfSecurityServiceDefRec:: UnionUpdateUniqueObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}

/*
函数功能:
	修改指定的表域:
输入参数:
	serviceID 服务代码
	fldName 域名称
	fldValue 域值
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionUpdateIntTypeSpecFldOfSecurityServiceDefRec(int serviceID,char *fldName,int fldValue)
{
	int	ret;
	char	recStr[8192*2+1];
	char	primaryKey[1024+1];
	int	lenOfRecStr = 0;

	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutIntTypeRecFldIntoRecStr("serviceID",serviceID,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [serviceID] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	strcpy(primaryKey,recStr);

	// 组记录
	memset(recStr,0,sizeof(recStr));
	lenOfRecStr = 0;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr(fldName,fldValue,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfSecurityServiceDefRec:: UnionPutIntTypeRecFldIntoRecStr [%s] failure!\n",fldName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;

	//赋加自动修改域
	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfSecurityServiceDefRec:: UnionAutoAppendDBRecUpdateAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	// 修改记录
	if ((ret = UnionUpdateUniqueObjectRecord(conTBLNameSecurityServiceDef,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfSecurityServiceDefRec:: UnionUpdateUniqueObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}



/*
功能：
	读取一个记录
输入参数:
	serviceID 服务代码
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionReadSecurityServiceDefRec(int serviceID,PUnionSecurityServiceDef prec)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	//从记录拼装串
	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutIntTypeRecFldIntoRecStr("serviceID",serviceID,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRec:: UnionPutRecFldIntoRecStr [serviceID] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	if (lenOfRecStr >= (int)sizeof(condition))
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRec:: lenOfCondition = [%d] too long!\n",lenOfRecStr);
		return(ret);
	}
	memset(condition,0,sizeof(condition));
	memcpy(condition,recStr,lenOfRecStr);

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameSecurityServiceDef,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRec:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//从记录串拼装结构
	if ((ret = UnionFormSecurityServiceDefRecFromRecStr(recStr,lenOfRecStr,prec)) < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRec:: UnionFormSecurityServiceDefRecFromRecStr!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(sizeof(*prec));
}


/*
功能：
	读取一个记录
输入参数:
	serviceID 服务代码
	fldName		域名
输出参数:
	fldValue	域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadSecurityServiceDefRecIntTypeFld(int serviceID,char *fldName,int *fldValue)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	//从记录拼装串
	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutIntTypeRecFldIntoRecStr("serviceID",serviceID,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecIntTypeFld:: UnionPutRecFldIntoRecStr [serviceID] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	if (lenOfRecStr >= (int)sizeof(condition))
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecIntTypeFld:: lenOfCondition = [%d] too long!\n",lenOfRecStr);
		return(ret);
	}
	memset(condition,0,sizeof(condition));
	memcpy(condition,recStr,lenOfRecStr);

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameSecurityServiceDef,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecIntTypeFld:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//读指定域
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue)) < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecIntTypeFld:: UnionReadIntTypeRecFldFromRecStr!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}


/*
功能：
	读取一个记录
输入参数:
	serviceID 服务代码
	fldName		域名
	sizeOfBuf	域值缓冲大小
输出参数:
	fldValue	域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadSecurityServiceDefRecFld(int serviceID,char *fldName,char *fldValue,int sizeOfBuf)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	//从记录拼装串
	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutIntTypeRecFldIntoRecStr("serviceID",serviceID,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecFld:: UnionPutRecFldIntoRecStr [serviceID] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	if (lenOfRecStr >= (int)sizeof(condition))
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecFld:: lenOfCondition = [%d] too long!\n",lenOfRecStr);
		return(ret);
	}
	memset(condition,0,sizeof(condition));
	memcpy(condition,recStr,lenOfRecStr);

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameSecurityServiceDef,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecFld:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//读指定域
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadSecurityServiceDefRecFld:: UnionReadRecFldFromRecStr!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}


/*
功能：
	根据条件，批量读取数据
输入参数:
	condition	条件字串
	maxRecNum		读取的最大记录数量
输出参数:
	recGrp		记录数组
返回值
	>=0		成功,读取的记录数量
	<0		失败,错误码
*/
int UnionBatchReadSecurityServiceDefRec(char *condition,TUnionSecurityServiceDef recGrp[],int maxRecNum)
{
	int	ret;
	char	fileName[512+1];
	int	recNum = 0;
	FILE	*recFileFp = NULL;
	char	recStr[8192*2+1];
	int	lenOfRecStr;
	TUnionSecurityServiceDef	rec;

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

// 读所有记录
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameSecurityServiceDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionBatchReadSecurityServiceDefRec:: UnionBatchSelectObjectRecord from [%s] on [%s]!\n",conTBLNameSecurityServiceDef,condition);
		return(ret);
	}
	// 打开文件
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionBatchReadSecurityServiceDefRec:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		if (recNum >= maxRecNum)
		{
			UnionLog("in UnionBatchReadSecurityServiceDefRec:: still exists records satisfy the condition [%s]!\n",condition);
			ret = recNum;
			goto errExit;
		}
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(&rec,0,sizeof(rec));
		if ((ret = UnionFormSecurityServiceDefRecFromRecStr(recStr,lenOfRecStr,&rec)) < 0)
		{
			UnionUserErrLog("in UnionBatchReadSecurityServiceDefRec:: UnionFormSecurityServiceDefRecFromRecStr from [%04d] [%s]\n",lenOfRecStr,recStr);
			goto errExit;
		}
		memcpy(&(recGrp[recNum]),&rec,sizeof(rec));
		recNum++;
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);

}
