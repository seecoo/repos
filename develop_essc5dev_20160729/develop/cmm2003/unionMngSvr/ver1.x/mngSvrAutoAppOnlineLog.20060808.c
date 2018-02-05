// Author:	������
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
����
	������Ȩ��¼��ʼ���Զ���־������
���������
	serviceID	��������
�������
	��
����ֵ
	>=0	�ɹ�,���������Ŀ
	<0	������
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr(PUnionOperationAuthorization pauthRec)
{
	int	ret;
	
	if (pauthRec == NULL)
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr:: null pointer!\n");
		return(errCodeNullPointer);
	}
	// �ж��Ƿ�Է�������ˮ����
	if (UnionReadIntTypeRECVar("isAppSpecOnlineLogOpened") <= 0)	// û�д��Զ���ˮ�Ǽ�
	{
		gunionIsAutoAppOnlineLogForThisService = 0;
		return(0);
	}
	if (gunionIsAutoAppOnlineLogForThisService)	// ��һ��������Զ���ˮ�Ѵ�
	{
		// �ж��Ƿ�ͬһ���Զ���ˮ����
		if ((strcmp(pauthRec->tableName,gunionAutoAppOnlineLogWriter.oriTableName) == 0) && (strcmp(pauthRec->onlineLogTableName,gunionAutoAppOnlineLogWriter.desTableName) == 0) &&
			(pauthRec->serviceID = gunionAutoAppOnlineLogWriter.serviceID) && (pauthRec->resID = gunionAutoAppOnlineLogWriter.resID))
			return(gunionAutoAppOnlineLogWriter.fldNum);
	}
	// ��ʼ���Զ���ˮ������
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
����
	���Զ�������ˮ���в�������
���������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
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

	if (!gunionIsAutoAppOnlineLogForThisService)	// �������Զ�������ˮ
		return(0);
	
	// �����Զ�������
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormAutoAppOnlineLogRecStrForRequest(&gunionAutoAppOnlineLogWriter,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogRequestRec:: UnionFormAutoAppOnlineLogRecStrForRequest!\n");
		return(lenOfRecStr);
	}
	// ����mngSvr��ˮ����
	if ((ret = UnionFormHeaderStrOfCurrentMngSvrOnlineRec(recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogRequestRec:: UnionFormHeaderStrOfCurrentMngSvrOnlineRec!\n");
		return(ret);
	}
	lenOfRecStr += ret;
	// �����¼
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
����
	���Զ�������ˮ���в�����Ӧ
���������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*
int UnionInsertAutoAppOnlineLogResponseRec()
{
	int	ret;
	char	recStr[8192+1];
	int	lenOfRecStr;
	
	if (!gunionIsAutoAppOnlineLogForThisService)	// �������Զ�������ˮ
		return(0);
	
	// �����Զ�������
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormAutoAppOnlineLogRecStrForResponse(&gunionAutoAppOnlineLogWriter,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormAutoAppOnlineLogRecStrForResponse!\n");
		return(lenOfRecStr);
	}
	// ����mngSvr��ˮ����
	if ((ret = UnionFormHeaderStrOfCurrentMngSvrOnlineRec(recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormHeaderStrOfCurrentMngSvrOnlineRec!\n");
		return(ret);
	}
	lenOfRecStr += ret;
	// �����¼
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
����
	���Զ�������ˮ���в�����Ӧ
���������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInsertAutoAppOnlineLogResponseRec()
{
	int	ret;
	char	condition[2048+1];
	char	primaryKey[512+1];
	char	recStr[2048+1];
	int	lenOfRecStr;
	
	if (!gunionIsAutoAppOnlineLogForThisService)	// �������Զ�������ˮ
		return(0);
	
	// ���ɸ�����
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionFormAutoAppOnlineLogRecStrForResponse(&gunionAutoAppOnlineLogWriter,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormAutoAppOnlineLogRecStrForResponse!\n");
		return(lenOfRecStr);
	}

	// ����������
	memset(condition, 0, sizeof condition);
	if ((ret = UnionFormHeaderStrOfCurrentMngSvrOnlineRec(condition,sizeof(condition)) < 0))
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormHeaderStrOfCurrentMngSvrOnlineRec!\n");
		return(ret);
	}
	// ������������ü�¼�ؼ���
	memset(primaryKey, 0, sizeof primaryKey);
	if ((ret = UnionFormPrimaryKeyRecStr(gunionAutoAppOnlineLogWriter.desTableName, condition, strlen(condition), primaryKey, sizeof(primaryKey)) < 0))
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionFormPrimaryKeyStrOfSpecObjectRec condition=[%s]!\n", condition);
		return(ret);
	}

	lenOfRecStr = sprintf(recStr,"%s%s",recStr,condition);
	// �����¼
	if ((ret = UnionUpdateUniqueObjectRecord(gunionAutoAppOnlineLogWriter.desTableName,primaryKey,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionInsertAutoAppOnlineLogResponseRec:: UnionUpdateUniqueObjectRecord! [%04d][%s] into table [%s], condition=[%s]\n",lenOfRecStr,recStr,gunionAutoAppOnlineLogWriter.desTableName, condition);
		return(ret);
	}
	//UnionLog("***** auto on line log response ******\n");
	//UnionLog("[%s][%04d][%s]\n",gunionAutoAppOnlineLogWriter.desTableName,lenOfRecStr,recStr);
	return(ret);
}
