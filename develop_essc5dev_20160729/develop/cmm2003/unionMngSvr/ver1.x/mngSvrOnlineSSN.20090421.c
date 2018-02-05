// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2009/5/23
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "mngSvrServicePackage.h"
#include "mngSvrOnlineSSN.h"
#include "unionMngServiceOnline.h"
#include "UnionStr.h"
#include "unionResID.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionOperationAuthorization.h"
#include "unionDatabaseCmd.h"
#include "mngSvrAutoAppOnlineLog.h"

int			gunionMyCurrentMngSvrSSN = 1;
int			gunionMngServiceRecInited = 0;
TUnionMngServiceOnline	gunionMngServiceRec;

int UnionApplyMyMngSvrSSN()
{
	if (++gunionMyCurrentMngSvrSSN <= 0)
		gunionMyCurrentMngSvrSSN = 1;
	return(gunionMyCurrentMngSvrSSN);
}

void UnionRegisterMngSvrRequestService(int resID,int serviceID,char *tellerNo,int lenOfData,char *data)
{
	int				sizeOfDataBuf;
	int				ret;
	TUnionOperationAuthorization	rec;
	char	szTmpBuf[8192+1];
	int 	iLenOfBuf = 0;

	// 2010-1-22,wolfgang wang added
	if (UnionIsBackuperMngSvr() || UnionIsNonMngSvr())
		return;
	// end of addition of 2010-1-22
	
	if ((serviceID == conDatabaseCmdRead) || (serviceID == conDatabaseCmdQueryAllRec) || (UnionIsNonAuthorizationMngSvrService(resID,serviceID)))
		return;

	if (!gunionMngServiceRecInited)
		memset(&gunionMngServiceRec,0,sizeof(gunionMngServiceRec));
	if ((ret = UnionReadOperationAuthorizationRec(resID,serviceID,&rec)) < 0)	
	{
		UnionUserErrLog("in UnionRegisterMngSvrRequestService:: UnionReadOperationAuthorizationRec! resID = [%03d] serviceID = [%03d] ret = [%d]\n",resID,serviceID,ret);
		memset(gunionMngServiceRec.tableName,0,sizeof(gunionMngServiceRec.tableName));
		memset(gunionMngServiceRec.serviceName,0,sizeof(gunionMngServiceRec.serviceName));
	}
	else
	{
		strcpy(gunionMngServiceRec.tableName,rec.tableName);
		strcpy(gunionMngServiceRec.serviceName,rec.serviceName);
	}

	memset(gunionMngServiceRec.data,0,sizeof(gunionMngServiceRec.data));
	gunionMngServiceRec.directionTag = '1';
	UnionGetFullSystemDate(gunionMngServiceRec.date);
	UnionGetSystemTime(gunionMngServiceRec.time);
	strcpy(gunionMngServiceRec.tellerNo,tellerNo);
	strcpy(gunionMngServiceRec.clientIPAddr,UnionReadResMngClientIPAddr());
	gunionMngServiceRec.port = UnionReadResMngClientPort();
	gunionMngServiceRec.ssn = UnionApplyMyMngSvrSSN();
	gunionMngServiceRec.pid = UnionGetPID();
	gunionMngServiceRec.resID = resID;
	gunionMngServiceRec.serviceID = serviceID;
	gunionMngServiceRec.lenOfData = lenOfData;

	// add by wangk 2010-3-23
	// ���ַ������ҳ������ţ����ڵ�����ǰ�ټ�һ��������
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	if( (iLenOfBuf = UnionTranslateSpecCharInDBStr(data, lenOfData, szTmpBuf, sizeof(szTmpBuf))) < 0 )
	{
		UnionUserErrLog("in UnionRegisterMngSvrRequestService:: UnionTranslateSpecCharInDBStr! ret = [%d]\n",ret);
		return;
	}
	// add end wangk 2010-3-23

	if ((ret = UnionTranslateSpecCharInStr('|',szTmpBuf,iLenOfBuf,gunionMngServiceRec.data,sizeof(gunionMngServiceRec.data))) < 0)
	{
		UnionUserErrLog("in UnionRegisterMngSvrRequestService:: UnionTranslateSpecCharInStr! ret = [%d]\n",ret);
		return;
	}

        // 2010-6-2,����������
        if ((ret = UnionInsertAutoAppOnlineLogRequestRec()) < 0)
        {
                UnionUserErrLog("in UnionRegisterMngSvrRequestService:: UnionInsertAutoAppOnlineLogRequestRec\n");
        }        	
        // 2010-6-2,���������ӽ���

	if ((ret = UnionInsertMngServiceOnlineRec(&gunionMngServiceRec)) < 0)
	{
		UnionUserErrLog("in UnionRegisterMngSvrRequestService:: UnionInsertMngServiceOnlineRec! ret = [%d]\n",ret);
		return;
	}
	gunionMngServiceRecInited = 1;
	return;
}

void UnionRegisterMngSvrResponseService(int resID,int serviceID, int responseCode,int lenOfData,char *data,int fileRecved)
{
	int			sizeOfDataBuf;
	int			ret;
	char	szTmpBuf[8192*2+1];
	int 	iLenOfBuf = 0;
	
	// 2010-1-22,wolfgang wang added
	if (UnionIsBackuperMngSvr() || UnionIsNonMngSvr())
		return;
	// end of addition of 2010-1-22
	
	if ((serviceID == conDatabaseCmdRead) || (serviceID == conDatabaseCmdQueryAllRec) || (UnionIsNonAuthorizationMngSvrService(resID,serviceID)))
		return;

	UnionSetCurrentMngSvrClientResStr(data,lenOfData);	// 2010-6-2,����������
	memset(gunionMngServiceRec.data,0,sizeof(gunionMngServiceRec.data));
	gunionMngServiceRec.directionTag = '0';
	gunionMngServiceRec.responseCode = responseCode;
	sizeOfDataBuf = sizeof(gunionMngServiceRec.data)-1;
	gunionMngServiceRec.fileTransferred = fileRecved;
	if (lenOfData > 0)
	{
		// add by wangk 2010-3-23
		// ���ַ������ҳ������ţ����ڵ�����ǰ�ټ�һ��������
		memset(szTmpBuf, 0, sizeof(szTmpBuf));
		if( (iLenOfBuf = UnionTranslateSpecCharInDBStr(data, lenOfData, szTmpBuf, sizeof(szTmpBuf))) < 0 )
		{
			UnionUserErrLog("in UnionRegisterMngSvrResponseService:: UnionTranslateSpecCharInDBStr! ret = [%d]\n",ret);
			return;
		}
		// add end wangk 2010-3-23

		if ((ret = UnionTranslateSpecCharInStr('|',szTmpBuf,iLenOfBuf,gunionMngServiceRec.data,sizeof(gunionMngServiceRec.data))) < 0)
		{
			UnionUserErrLog("in UnionRegisterMngSvrResponseService:: UnionTranslateSpecCharInStr! ret = [%d]\n",ret);
			return;
		}
	}

        // 2010-6-2,����������
        if ((ret = UnionInsertAutoAppOnlineLogResponseRec()) < 0)
        {
                UnionUserErrLog("in UnionRegisterMngSvrResponseService:: UnionInsertAutoAppOnlineLogResponseRec\n");
        }        	
        // 2010-6-2,���������ӽ���
	if ((ret = UnionInsertMngServiceOnlineRec(&gunionMngServiceRec)) < 0)
	{
		UnionUserErrLog("in UnionRegisterMngSvrResponseService:: UnionInsertMngServiceOnlineRec! ret = [%d]\n",ret);
		//return;
	}
	return;
}

/*
���ܣ�
	ƴװ��ǰ��ˮ��ͷ
�������:
	sizeOfBuf	���ռ�¼�ִ��Ļ���Ĵ�С
�������:
	recStr	��¼�ִ�
����ֵ
	>=0		�ɹ�,ƴװ�ļ�¼���ĳ���
	<0		ʧ��,������
*/
int UnionFormHeaderStrOfCurrentMngSvrOnlineRec(char *recStr,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;

	// ƴ�ؼ���
	if ((ret = UnionFormPrimaryKeyRecStrFromMngServiceOnlineRec(&gunionMngServiceRec,recStr,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionFormPrimaryKeyRecStrFromMngServiceOnlineRec!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset = ret;
	
	// ƴ������
	// ����Ӧ
	if (gunionMngServiceRec.directionTag == '0')	
	{
		ret = UnionPutIntTypeRecFldIntoRecStr(conMngServiceOnlineFldNameResponseCode,gunionMngServiceRec.responseCode,recStr+offset,sizeOfBuf-offset);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionPutIntTypeRecFldIntoRecStr [responseCode] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
		offset += ret;
		ret = UnionPutIntTypeRecFldIntoRecStr(conMngServiceOnlineFldNameFileTransferred,gunionMngServiceRec.fileTransferred,recStr+offset,sizeOfBuf-offset);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionPutIntTypeRecFldIntoRecStr [fileTransferred] error!\n");
			return(UnionSetUserDefinedErrorCode(ret));
		}
		offset += ret;
		return(offset);
	}
	// ������
	ret = UnionPutRecFldIntoRecStr(conMngServiceOnlineFldNameTellerNo,gunionMngServiceRec.tellerNo,strlen(gunionMngServiceRec.tellerNo),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionPutRecFldIntoRecStr [tellerNo] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutIntTypeRecFldIntoRecStr(conMngServiceOnlineFldNameResID,gunionMngServiceRec.resID,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionPutIntTypeRecFldIntoRecStr [resID] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conMngServiceOnlineFldNameTableName,gunionMngServiceRec.tableName,strlen(gunionMngServiceRec.tableName),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionPutRecFldIntoRecStr [tableName] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutIntTypeRecFldIntoRecStr(conMngServiceOnlineFldNameServiceID,gunionMngServiceRec.serviceID,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionPutIntTypeRecFldIntoRecStr [serviceID] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutRecFldIntoRecStr(conMngServiceOnlineFldNameServiceName,gunionMngServiceRec.serviceName,strlen(gunionMngServiceRec.serviceName),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionFormHeaderStrOfCurrentMngSvrOnlineRec:: UnionPutRecFldIntoRecStr [serviceName] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	return(offset);
}
