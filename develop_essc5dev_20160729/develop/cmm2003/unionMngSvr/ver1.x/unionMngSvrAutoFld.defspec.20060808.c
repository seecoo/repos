// 2008/7/26
// Wolfang Wang

#ifdef _WIN32
#include <vcl.h>
#include <windows.h>
#include "dir.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "operationControl.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "UnionLog.h"			// ʹ��3.x�汾
#include "unionWorkingDir.h"
#include "mngSvrCommProtocol.h"
#include "unionMngSvrOperationType.h"
#include "mngSvrFileSvr.h"
#include "unionResID.h"
#include "unionREC.h"
#include "mngSvrTeller.h"
#include "simuMngSvrLocally.h"

#ifdef _WIN32
#include "unionTBLQueryInterface.h"
#include "unionDBConf.h"
#endif

char    gunionTellerNo[48+1] = "";

char    gunionCurrentMngSvrClientReqStr[8192+1] = ""; 
int     gunionLenOfCurrentMngSvrClientReqStr;

char *UnionGetTellerNo()
{
	strcpy(gunionTellerNo,getenv("LOGNAME"));
	return(gunionTellerNo);
}

char *UnionGetCurrentOperationTellerNo()
{
	return(UnionGetTellerNo());
}

void UnionGetAutoAppendDBRecInputAttr(char *inputTeller,char *inputDate,char *inputTime)
{
	strcpy(inputTeller,UnionGetTellerNo());
	strcpy(inputDate,UnionGetCurrentFullSystemDate());
	strcpy(inputTime,UnionGetCurrentSystemTime());
	//UnionLog("in UnionGetAutoAppendDBRecInputAttr:: [%s][%s][%s]\n",inputTeller,inputDate,inputTime);
}

void UnionGetAutoAppendDBRecUpdateAttr(char *updateTeller,char *updateDate,char *updateTime)
{
	strcpy(updateTeller,UnionGetTellerNo());
	strcpy(updateDate,UnionGetCurrentFullSystemDate());
	strcpy(updateTime,UnionGetCurrentSystemTime());
	//UnionLog("in UnionGetAutoAppendDBRecUpdateAttr:: [%s][%s][%s]\n",updateTeller,updateDate,updateTime);
}

/*
����
	�Զ��ڼ�¼�󸽼Ӽ�¼��������
�������
	recStr		��¼��
	lenOfRecStr	��¼����
	sizeOfRecStr	��¼�������С
�������
	recStr	��¼��
����ֵ
	>=0	�������´���ֵ����
	<0	������
*/

int UnionAutoAppendDBRecInputAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr)
{
	int	ret;
	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "inputDate", UnionGetCurrentFullSystemDate())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecInputAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}
	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "inputTime", UnionGetCurrentSystemTime())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecInputAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}
	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "inputTeller", UnionGetCurrentOperationTellerNo())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecInputAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}
	return (ret);
}

/*
����
	�Զ��ڼ�¼�󸽼Ӽ�¼�޸�����
�������
	recStr		��¼��
	lenOfRecStr	��¼����
	sizeOfRecStr	��¼�������С
�������
	recStr	��¼��
����ֵ
	>=0	�������´���ֵ����
	<0	������
*/
int UnionAutoAppendDBRecUpdateAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr)
{
	int	ret;
	ret = lenOfRecStr;


	if ( (ret = UnionDeleteSpecFldOnUnionRec(recStr, ret, "inputDate")) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionDeleteSpecFldOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionDeleteSpecFldOnUnionRec(recStr, ret, "inputTime")) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionDeleteSpecFldOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionDeleteSpecFldOnUnionRec(recStr, ret, "inputTeller")) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionDeleteSpecFldOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "updateDate", UnionGetCurrentFullSystemDate())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "updateTime", UnionGetCurrentSystemTime())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "updateTeller", UnionGetCurrentOperationTellerNo())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}

	return (ret);
}

// 2010-3-20 wolfgang added
int UnionReadFldFromCurrentMngSvrClientReqStr(char *fldName,char *buf,int sizeOfBuf)
{
        return(UnionReadRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,buf,sizeOfBuf)
);
}
