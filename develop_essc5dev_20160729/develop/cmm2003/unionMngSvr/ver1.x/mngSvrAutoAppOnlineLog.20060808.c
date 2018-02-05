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
#include "unionTableList.h"

#include "mngSvrAutoAppOnlineLogWriter.h"
#include "mngSvrAutoAppOnlineLog.h"
#include "mngSvrOnlineSSN.h"
#include "unionComplexDBRecord.h"

TUnionAutoAppOnlineLogWriter	gunionAutoAppOnlineLogWriter;
int				gunionIsAutoAppOnlineLogForThisService = 0;

/*
功能
	根据授权记录初始化自动日志生成器
输入参数：
	serviceID	操作代码
输出参数
	无
返回值
	>=0	成功,返回域的数目
	<0	错误码
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr(PUnionOperationAuthorization pauthRec)
{
	int	ret;
	
	if (pauthRec == NULL)
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr:: null pointer!\n");
		return(errCodeNullPointer);
	}
	// 判断是否对服务作流水处理
	if (UnionReadIntTypeRECVar("isAppSpecOnlineLogOpened") <= 0)	// 没有打开自动流水登记
	{
		gunionIsAutoAppOnlineLogForThisService = 0;
		return(0);
	}
	if (gunionIsAutoAppOnlineLogForThisService)	// 上一个服务的自动流水已打开
	{
		// 判断是否同一个自动流水操作
		if ((strcmp(pauthRec->tableName,gunionAutoAppOnlineLogWriter.oriTableName) == 0) && (strcmp(pauthRec->onlineLogTableName,gunionAutoAppOnlineLogWriter.desTableName) == 0) &&
			(pauthRec->serviceID = gunionAutoAppOnlineLogWriter.serviceID) && (pauthRec->resID = gunionAutoAppOnlineLogWriter.resID))
			return(gunionAutoAppOnlineLogWriter.fldNum);
	}
	// 初始化自动流水生成器
	if ((ret = UnionInitAutoAppOnlineLogWriterOnAuthRec(pauthRec,&gunionAutoAppOnlineLogWriter)) <= 0)
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr:: err! [%d][%d]\n", pauthRec->resID,pauthRec->serviceID);
		gunionIsAutoAppOnlineLogForThisService = 0;
		return(ret);
	}
	gunionIsAutoAppOnlineLogForThisService = 1;
	return(ret);
}

/*
功能
	在自动生成流水表中插入请求
输入参数：
	无
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInsertAutoAppOnlineLogRequestRec()
{
	int	ret;
	char	recStr[8192+1];
	int	lenOfRecStr;

	// add by xusj 20100607 begin
	TUnionOperationAuthorization    rec;
	memset(&rec, 0, sizeof rec);
        if ((ret = UnionReadOperationAuthorizationRecByResIDAndOperationID(UnionGetCurrentResID(),UnionGetCurrentServiceID(),&rec)) < 0)
        {
                UnionUserErrLog("in UnionVerifyOperation:: UnionReadOperationAuthorizationRecByResIDAndOperationID resID [%d] operationID [%d]\n",UnionGetCurrentResID(), UnionGetCurrentServiceID());
                return(ret);
        }

	if (strlen(rec.onlineLogTableName) == 0)
		return 0;

        if ((ret = UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr(&rec)) < 0)
        {
                UnionUserErrLog("in UnionVerifyOperation:: UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr resID [%d] operationID [%d]\n",UnionGetCurrentResID(),UnionGetCurrentServiceID());
		return(ret);
        }
	// add by xusj 20100607 end

	if (!gunionIsAutoAppOnlineLogForThisService)	// 不生成自动生成流水
		return(0);
	
	// 生成自动生成域
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormAutoAppOnlineLogRecStrForRequest(&gunionAutoAppOnlineLogWriter,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogRequestRec:: UnionFormAutoAppOnlineLogRecStrForRequest!\n");
		return(lenOfRecStr);
	}
	// 附加mngSvr流水的域
	if ((ret = UnionFormHeaderStrOfCurrentMngSvrOnlineRec(recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogRequestRec:: UnionFormHeaderStrOfCurrentMngSvrOnlineRec!\n");
		return(ret);
	}
	lenOfRecStr += ret;
	// 插入记录
	if ((ret = UnionInsertObjectRecord(gunionAutoAppOnlineLogWriter.desTableName,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogRequestRec:: UnionInsertObjectRecord! [%04d][%s] into table [%s]\n",lenOfRecStr,recStr,gunionAutoAppOnlineLogWriter.desTableName);
		return(ret);
	}
	// UnionLog("***** auto on line log request ******\n");
	// UnionLog("[%s][%04d][%s]\n",gunionAutoAppOnlineLogWriter.desTableName,lenOfRecStr,recStr);
	return(ret);
}

/*****del by xusj 20100607 begin ****
*
功能
	在自动生成流水表中插入响应
输入参数：
	无
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*
int UnionInsertAutoAppOnlineLogResponseRec()
{
	int	ret;
	char	recStr[8192+1];
	int	lenOfRecStr;
	
	if (!gunionIsAutoAppOnlineLogForThisService)	// 不生成自动生成流水
		return(0);
	
	// 生成自动生成域
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormAutoAppOnlineLogRecStrForResponse(&gunionAutoAppOnlineLogWriter,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormAutoAppOnlineLogRecStrForResponse!\n");
		return(lenOfRecStr);
	}
	// 附加mngSvr流水的域
	if ((ret = UnionFormHeaderStrOfCurrentMngSvrOnlineRec(recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormHeaderStrOfCurrentMngSvrOnlineRec!\n");
		return(ret);
	}
	lenOfRecStr += ret;
	// 插入记录
	if ((ret = UnionUpdateUniqueObjectRecord(gunionAutoAppOnlineLogWriter.desTableName,recStr,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionUpdateUniqueObjectRecord! [%04d][%s] into table [%s]\n",lenOfRecStr,recStr,gunionAutoAppOnlineLogWriter.desTableName);
		return(ret);
	}
	//UnionLog("***** auto on line log response ******\n");
	//UnionLog("[%s][%04d][%s]\n",gunionAutoAppOnlineLogWriter.desTableName,lenOfRecStr,recStr);
	return(ret);
}
*****del by xusj 20100607 end ****/

/*
功能
	在自动生成流水表中插入响应
输入参数：
	无
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInsertAutoAppOnlineLogResponseRec()
{
	int	ret;
	char	condition[2048+1];
	char	primaryKey[512+1];
	char	recStr[2048+1];
	int	lenOfRecStr;
	
	if (!gunionIsAutoAppOnlineLogForThisService)	// 不生成自动生成流水
		return(0);
	
	// 生成更新域
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormAutoAppOnlineLogRecStrForResponse(&gunionAutoAppOnlineLogWriter,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormAutoAppOnlineLogRecStrForResponse!\n");
		return(lenOfRecStr);
	}

	// 生成条件域
	memset(condition, 0, sizeof condition);
	if ((ret = UnionFormHeaderStrOfCurrentMngSvrOnlineRec(condition,sizeof(condition)) < 0))
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormHeaderStrOfCurrentMngSvrOnlineRec!\n");
		return(ret);
	}
	// 根据条件串获得记录关键字
	memset(primaryKey, 0, sizeof primaryKey);
	if ((ret = UnionFormPrimaryKeyRecStr(gunionAutoAppOnlineLogWriter.desTableName, condition, strlen(condition), primaryKey, sizeof(primaryKey)) < 0))
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormPrimaryKeyStrOfSpecObjectRec condition=[%s]!\n", condition);
		return(ret);
	}

	lenOfRecStr = sprintf(recStr,"%s%s",recStr,condition);
	// 插入记录
	if ((ret = UnionUpdateUniqueObjectRecord(gunionAutoAppOnlineLogWriter.desTableName,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionUpdateUniqueObjectRecord! [%04d][%s] into table [%s], condition=[%s]\n",lenOfRecStr,recStr,gunionAutoAppOnlineLogWriter.desTableName, condition);
		return(ret);
	}
	//UnionLog("***** auto on line log response ******\n");
	//UnionLog("[%s][%04d][%s]\n",gunionAutoAppOnlineLogWriter.desTableName,lenOfRecStr,recStr);
	return(ret);
}
