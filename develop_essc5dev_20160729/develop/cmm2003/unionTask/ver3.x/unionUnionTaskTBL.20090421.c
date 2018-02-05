// Automatically generated codes
// Generated Date and Time 20091110172947
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

#include "unionUnionTaskTBL.h"
#include "unionRecFile.h"
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
int UnionFormDefaultValueOfUnionTaskTBLRec(PUnionUnionTaskTBL prec)
{
	if (prec == NULL)
	{
		UnionUserErrLog("in UnionFormDefaultValueOfUnionTaskTBLRec:: prec is null!\n");
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
int UnionFormRecStrFromUnionTaskTBLRec(PUnionUnionTaskTBL prec,char *recStr,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormRecStrFromUnionTaskTBLRec:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	if ((ret = UnionFormPrimaryKeyRecStrFromUnionTaskTBLRec(prec,recStr,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormRecStrFromUnionTaskTBLRec:: UnionFormPrimaryKeyRecStrFromUnionTaskTBLRec!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset = ret;
	if ((ret = UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec(prec,recStr+offset,sizeOfBuf-offset)) < 0)
	{
		UnionUserErrLog("in UnionFormRecStrFromUnionTaskTBLRec:: UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec!\n");
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
int UnionFormPrimaryKeyRecStrFromUnionTaskTBLRec(PUnionUnionTaskTBL prec,char *recStr,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormPrimaryKeyRecStrFromUnionTaskTBLRec:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameName,prec->name,strlen(prec->name),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [name] error!\n");
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
int UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec(PUnionUnionTaskTBL prec,char *recStr,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameStartCmd,prec->startCmd,strlen(prec->startCmd),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [startCmd] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutIntTypeRecFldIntoRecStr(conUnionTaskTBLFldNameNum,prec->num,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutIntTypeRecFldIntoRecStr [num] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameLogFileName,prec->logFileName,strlen(prec->logFileName),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [logFileName] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameRemark,prec->remark,strlen(prec->remark),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [remark] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameInputTeller,prec->inputTeller,strlen(prec->inputTeller),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [inputTeller] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameInputDate,prec->inputDate,strlen(prec->inputDate),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [inputDate] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameInputTime,prec->inputTime,strlen(prec->inputTime),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [inputTime] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameUpdateTeller,prec->updateTeller,strlen(prec->updateTeller),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [updateTeller] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameUpdateDate,prec->updateDate,strlen(prec->updateDate),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [updateDate] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conUnionTaskTBLFldNameUpdateTime,prec->updateTime,strlen(prec->updateTime),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [updateTime] error!\n");
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
int UnionFormPrimaryKeyOfUnionTaskTBLRecFromRecStr(char *recStr,int lenOfRecStr,PUnionUnionTaskTBL prec)
{
	int	ret;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormPrimaryKeyOfUnionTaskTBLRecFromRecStr:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	UnionFormDefaultValueOfUnionTaskTBLRec(prec);
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameName,prec->name,sizeof(prec->name));
	if (ret < 0)
	{
		if (!0)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyOfUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [name] error!\n");
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
int UnionFormUnionTaskTBLRecFromRecStr(char *recStr,int lenOfRecStr,PUnionUnionTaskTBL prec)
{
	int	ret;

	if ((prec == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: prec or recStr is null!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	UnionFormDefaultValueOfUnionTaskTBLRec(prec);
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameName,prec->name,sizeof(prec->name));
	if (ret < 0)
	{
		if (!0)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [name] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameStartCmd,prec->startCmd,sizeof(prec->startCmd));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [startCmd] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameNum,&(prec->num));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadIntTypeRecFldFromRecStr fld [num] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameLogFileName,prec->logFileName,sizeof(prec->logFileName));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [logFileName] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameRemark,prec->remark,sizeof(prec->remark));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [remark] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameInputTeller,prec->inputTeller,sizeof(prec->inputTeller));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [inputTeller] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameInputDate,prec->inputDate,sizeof(prec->inputDate));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [inputDate] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameInputTime,prec->inputTime,sizeof(prec->inputTime));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [inputTime] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameUpdateTeller,prec->updateTeller,sizeof(prec->updateTeller));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [updateTeller] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameUpdateDate,prec->updateDate,sizeof(prec->updateDate));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [updateDate] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
	}
	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conUnionTaskTBLFldNameUpdateTime,prec->updateTime,sizeof(prec->updateTime));
	if (ret < 0)
	{
		if (!1)
		{
			UnionUserErrLog("in UnionFormUnionTaskTBLRecFromRecStr:: UnionReadRecFldFromRecStr fld [updateTime] error!\n");
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
int UnionInsertUnionTaskTBLRec(PUnionUnionTaskTBL prec)
{
	int	ret;
	int	lenOfRecStr;
	char	recStr[8192*4+1];

	//从记录拼装串
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormRecStrFromUnionTaskTBLRec(prec,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertUnionTaskTBLRec:: UnionFormRecStrFromUnionTaskTBLRec!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//赋加自动插入域
	if ((lenOfRecStr = UnionAutoAppendDBRecInputAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertUnionTaskTBLRec:: UnionAutoAppendDBRecInputAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//插入记录
	if ((ret = UnionInsertObjectRecord(conTBLNameUnionTaskTBL,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertUnionTaskTBLRec:: UnionInsertObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}


/*
功能：
	删除一个记录
输入参数:
	name 关键字
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionDeleteUnionTaskTBLRec(char *name)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];

	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutRecFldIntoRecStr("name",name,strlen(name),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDeleteUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [name] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	//删除记录
	if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameUnionTaskTBL,recStr)) < 0)
	{
		UnionUserErrLog("in UnionDeleteUnionTaskTBLRec:: UnionDeleteUniqueObjectRecord!\n");
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
int UnionUpdateUnionTaskTBLRec(PUnionUnionTaskTBL prec)
{
	int	ret;
	int	lenOfRecStr;
	char	recStr[8192*4+1];
	char	primaryKey[1024+1];

	//从记录拼关键字
	memset(primaryKey,0,sizeof(primaryKey));
	if ((ret = UnionFormPrimaryKeyRecStrFromUnionTaskTBLRec(prec,primaryKey,sizeof(primaryKey))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUnionTaskTBLRec:: UnionFormPrimaryKeyRecStrFromUnionTaskTBLRec!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	//从记录接记录串
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec(prec,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUnionTaskTBLRec:: UnionFormNonPrimaryKeyRecStrFromUnionTaskTBLRec!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//赋加自动修改域
	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUnionTaskTBLRec:: UnionAutoAppendDBRecUpdateAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//修改记录
	if ((ret = UnionUpdateUniqueObjectRecord(conTBLNameUnionTaskTBL,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUnionTaskTBLRec:: UnionUpdateUniqueObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}
/*
函数功能:
	修改指定的表域:
输入参数:
	name 关键字
	fldName 域名称
	fldValue 域值
	fldValueLen 域值长度
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionUpdateSpecFldOfUnionTaskTBLRec(char *name,char *fldName,char *fldValue,int lenOfFldValue)
{
	int	ret;
	char	recStr[8192*2+1];
	char	primaryKey[1024+1];
	int	lenOfRecStr = 0;

	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutRecFldIntoRecStr("name",name,strlen(name),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [name] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	strcpy(primaryKey,recStr);

	// 组记录
	memset(recStr,0,sizeof(recStr));
	lenOfRecStr = 0;
	if ((ret = UnionPutRecFldIntoRecStr(fldName,fldValue,lenOfFldValue,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [%s] failure!\n",fldName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;

	//赋加自动修改域
	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfUnionTaskTBLRec:: UnionAutoAppendDBRecUpdateAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	// 修改记录
	if ((ret = UnionUpdateUniqueObjectRecord(conTBLNameUnionTaskTBL,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSpecFldOfUnionTaskTBLRec:: UnionUpdateUniqueObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}

/*
函数功能:
	修改指定的表域:
输入参数:
	name 关键字
	fldName 域名称
	fldValue 域值
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionUpdateIntTypeSpecFldOfUnionTaskTBLRec(char *name,char *fldName,int fldValue)
{
	int	ret;
	char	recStr[8192*2+1];
	char	primaryKey[1024+1];
	int	lenOfRecStr = 0;

	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutRecFldIntoRecStr("name",name,strlen(name),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [name] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	strcpy(primaryKey,recStr);

	// 组记录
	memset(recStr,0,sizeof(recStr));
	lenOfRecStr = 0;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr(fldName,fldValue,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfUnionTaskTBLRec:: UnionPutIntTypeRecFldIntoRecStr [%s] failure!\n",fldName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;

	//赋加自动修改域
	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfUnionTaskTBLRec:: UnionAutoAppendDBRecUpdateAttr!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	// 修改记录
	if ((ret = UnionUpdateUniqueObjectRecord(conTBLNameUnionTaskTBL,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateIntTypeSpecFldOfUnionTaskTBLRec:: UnionUpdateUniqueObjectRecord!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}



/*
功能：
	读取一个记录
输入参数:
	name 关键字
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionReadUnionTaskTBLRec(char *name,PUnionUnionTaskTBL prec)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	//从记录拼装串
	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutRecFldIntoRecStr("name",name,strlen(name),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRec:: UnionPutRecFldIntoRecStr [name] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	if (lenOfRecStr >= sizeof(condition))
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRec:: lenOfCondition = [%d] too long!\n",lenOfRecStr);
		return(ret);
	}
	memset(condition,0,sizeof(condition));
	memcpy(condition,recStr,lenOfRecStr);

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameUnionTaskTBL,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRec:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//从记录串拼装结构
	if ((ret = UnionFormUnionTaskTBLRecFromRecStr(recStr,lenOfRecStr,prec)) < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRec:: UnionFormUnionTaskTBLRecFromRecStr!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(sizeof(*prec));
}


/*
功能：
	读取一个记录
输入参数:
	name 关键字
	fldName		域名
输出参数:
	fldValue	域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadUnionTaskTBLRecIntTypeFld(char *name,char *fldName,int *fldValue)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	//从记录拼装串
	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutRecFldIntoRecStr("name",name,strlen(name),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecIntTypeFld:: UnionPutRecFldIntoRecStr [name] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	if (lenOfRecStr >= sizeof(condition))
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecIntTypeFld:: lenOfCondition = [%d] too long!\n",lenOfRecStr);
		return(ret);
	}
	memset(condition,0,sizeof(condition));
	memcpy(condition,recStr,lenOfRecStr);

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameUnionTaskTBL,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecIntTypeFld:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//读指定域
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue)) < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecIntTypeFld:: UnionReadIntTypeRecFldFromRecStr!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}


/*
功能：
	读取一个记录
输入参数:
	name 关键字
	fldName		域名
	sizeOfBuf	域值缓冲大小
输出参数:
	fldValue	域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadUnionTaskTBLRecFld(char *name,char *fldName,char *fldValue,int sizeOfBuf)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	//从记录拼装串
	// 拼关键字
	memset(recStr,0,sizeof(recStr));
	ret = UnionPutRecFldIntoRecStr("name",name,strlen(name),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecFld:: UnionPutRecFldIntoRecStr [name] failure!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;
	if (lenOfRecStr >= sizeof(condition))
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecFld:: lenOfCondition = [%d] too long!\n",lenOfRecStr);
		return(ret);
	}
	memset(condition,0,sizeof(condition));
	memcpy(condition,recStr,lenOfRecStr);

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameUnionTaskTBL,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecFld:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//读指定域
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadUnionTaskTBLRecFld:: UnionReadRecFldFromRecStr!\n");
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
int UnionBatchReadUnionTaskTBLRec(char *condition,TUnionUnionTaskTBL recGrp[],int maxRecNum)
{
	int	ret;
	char	fileName[512+1];
	int	recNum = 0;
	FILE	*recFileFp = NULL;
	char	recStr[8192*2+1];
	int	lenOfRecStr;
	TUnionUnionTaskTBL	rec;

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

// 读所有记录
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameUnionTaskTBL,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionBatchReadUnionTaskTBLRec:: UnionBatchSelectObjectRecord from [%s] on [%s]!\n",conTBLNameUnionTaskTBL,condition);
		return(ret);
	}
	// 打开文件
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionBatchReadUnionTaskTBLRec:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		if (recNum >= maxRecNum)
		{
			UnionLog("in UnionBatchReadUnionTaskTBLRec:: still exists records satisfy the condition [%s]!\n",condition);
			ret = recNum;
			goto errExit;
		}
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(&rec,0,sizeof(rec));
		if ((ret = UnionFormUnionTaskTBLRecFromRecStr(recStr,lenOfRecStr,&rec)) < 0)
		{
			UnionUserErrLog("in UnionBatchReadUnionTaskTBLRec:: UnionFormUnionTaskTBLRecFromRecStr from [%04d] [%s]\n",lenOfRecStr,recStr);
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
