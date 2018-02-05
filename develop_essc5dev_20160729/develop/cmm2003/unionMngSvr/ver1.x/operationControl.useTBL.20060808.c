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
����	
	���ݹؼ��ֶ���¼
�������
	resID		��Դ��
	operationID	������
�������
	prec		�����ļ�¼
����ֵ
	>=0		�ɹ�
	<0	������
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

// ��ȡ��Դ������˵��
char *UnionGetOperationRemark(int resID,int operationID)
{
        TUnionOperationAuthorization    rec;
        int                             ret;

	if ((ret = UnionReadOperationAuthorizationRecByResIDAndOperationID(resID,operationID,&rec)) < 0)
        {
                UnionAuditLog("in UnionGetOperationRemark:: operation not defined for resID [%d] operationID [%d]\n",resID,operationID);
                sprintf(gunionResOperationRemark,"����Դ%d����%d����δ����",resID,operationID);
        }
        else
                strcpy(gunionResOperationRemark,rec.remark);
        return(gunionResOperationRemark);
}

// ��֤��ɫ�����ܷ�����������,����ֵ�����������
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
        // 2010-6-2,����������
        if ((ret = UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr(&rec)) < 0)
        {
                UnionUserErrLog("in UnionVerifyOperation:: UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr resID [%d] operationID [%d]\n",resID,resCmd);
        }        	
        // 2010-6-2,���������ӽ���
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

// ������������
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

