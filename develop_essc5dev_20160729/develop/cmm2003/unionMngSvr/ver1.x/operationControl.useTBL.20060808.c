// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operationControl.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionOperationAuthorization.h"
#include "mngSvrAutoAppOnlineLog.h"

char    gunionResOperationRemark[128+1]="";

/*
功能	
	根据关键字读记录
输入参数
	resID		资源号
	operationID	操作号
输入出数
	prec		读出的记录
返回值
	>=0		成功
	<0	错误码
*/
int UnionReadOperationAuthorizationRecByResIDAndOperationID(int resID,int operationID,PUnionOperationAuthorization prec)
{
	int	ret;
	
	if (prec == NULL)
		return(errCodeNullPointer);
	memset(prec,0,sizeof(*prec));
	//prec->resID = resID;
	//prec->serviceID = operationID;
	if ((ret = UnionReadOperationAuthorizationRec(resID,operationID,prec)) < 0)
	{
		UnionUserErrLog("in UnionReadOperationAuthorizationRecByResIDAndOperationID:: UnionReadOperationAuthorizationRec!\n");
		return(ret);
	}
	return(ret);
}

// 读取资源操作的说明
char *UnionGetOperationRemark(int resID,int operationID)
{
        TUnionOperationAuthorization    rec;
        int                             ret;

	if ((ret = UnionReadOperationAuthorizationRecByResIDAndOperationID(resID,operationID,&rec)) < 0)
        {
                UnionAuditLog("in UnionGetOperationRemark:: operation not defined for resID [%d] operationID [%d]\n",resID,operationID);
                sprintf(gunionResOperationRemark,"对资源%d进行%d操作未定义",resID,operationID);
        }
        else
                strcpy(gunionResOperationRemark,rec.remark);
        return(gunionResOperationRemark);
}

// 验证角色级别能否进行这个操作,返回值是命令的类型
int UnionVerifyOperation(int resID,int resCmd,char roleLevel)
{
        TUnionOperationAuthorization    rec;
        int                             ret;
	char				fldName[10];
	
	if ((ret = UnionReadOperationAuthorizationRecByResIDAndOperationID(resID,resCmd,&rec)) < 0)
        {
                UnionUserErrLog("in UnionVerifyOperation:: UnionReadOperationAuthorizationRecByResIDAndOperationID resID [%d] operationID [%d]\n",resID,resCmd);
		return(errCodeYLQZMDL_YLQZAuthorizedTrans);
        }
	/***del by xusj 20100607 begin ***
        // 2010-6-2,王纯军增加
        if ((ret = UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr(&rec)) < 0)
        {
                UnionUserErrLog("in UnionVerifyOperation:: UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr resID [%d] operationID [%d]\n",resID,resCmd);
        }        	
        // 2010-6-2,王纯军增加结束
	***del by xusj 20100607 end ***/
        sprintf(fldName,"%c",roleLevel);
	if ((ret = UnionIsFldStrInUnionFldListStr(rec.listOfOperatorLevel,strlen(rec.listOfOperatorLevel),',',fldName)) <= 0)
        {
                UnionUserErrLog("in UnionVerifyOperation:: UnionReadOperationAuthorizationRecByResIDAndOperationID resID [%d] operationID [%d] not permitted for role [%c]! ret = [%d]\n",
                	resID,resCmd,roleLevel,ret);
                /*
		if (ret == 0)
                	return(errCodeEsscMDL_InvalidService);
                else
                	return(ret);
		*/
		return(errCodeYLQZMDL_YLQZAuthorizedTrans);
        }
        return(rec.level);
}

// 获得命令的类型
int UnionGetMngSvrOperationType(int resID,int resCmd)
{
        TUnionOperationAuthorization    rec;
        int                             ret;
	
	if ((ret = UnionReadOperationAuthorizationRecByResIDAndOperationID(resID,resCmd,&rec)) < 0)
        {
                UnionUserErrLog("in UnionGetMngSvrOperationType:: UnionReadOperationAuthorizationRecByResIDAndOperationID resID [%d] operationID [%d]\n",resID,resCmd);
                return(ret);
        }
        return(rec.level);
}

