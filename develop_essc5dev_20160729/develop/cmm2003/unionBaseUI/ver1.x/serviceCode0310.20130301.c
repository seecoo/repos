//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "baseUIService.h"
#include "unionCommBetweenMDL.h"
#include "unionREC.h"

/***************************************
�������:	0310
������:		ִ��
��������:	ִ��
***************************************/
int UnionDealServiceCode0310(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			recordID = 0;
	char			tmpBuf[16];
	char			status[8];
	char			reqStatus[8];
	char			sql[256];
	char			reqDateTime[16];
	char			opinion[136];
	char			expirationTime[16];

	// ��ȡ��¼��
	if ((ret = UnionReadRequestXMLPackageValue("body/recordID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: UnionReadRequestXMLPackageValue[%s]!\n","body/reocrdID");
		return(ret);
	}
	else
	{
		tmpBuf[ret] = 0;
		recordID = atoi(tmpBuf);
	}

	// ��ȡ�������
	if ((ret = UnionReadRequestXMLPackageValue("body/opinion",opinion,sizeof(opinion))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: UnionReadRequestXMLPackageValue[%s]!\n","body/opinion");
		return(ret);
	}
	opinion[ret] = 0;

	// ��ȡ״̬
	memset(reqStatus,0,sizeof(reqStatus));
	if ((ret = UnionReadRequestXMLPackageValue("body/status",reqStatus,sizeof(reqStatus))) < 0)
	{
		UnionLog("in UnionDealServiceCode0310:: UnionReadRequestXMLPackageValue[%s] is null!\n","body/status");
		return(ret);
	}

	// ʧЧʱ��
	if ((ret = UnionReadRequestXMLPackageValue("body/expirationTime",expirationTime,sizeof(expirationTime))) < 0)
	{
		UnionLog("in UnionDealServiceCode0310:: UnionReadRequestXMLPackageValue[%s] is null!\n","body/expirationTime");
		return(ret);
	}
	expirationTime[ret] = 0;

	// ���״̬
	if (reqStatus[0] != '1' && reqStatus[0] != '0')
	{
		UnionLog("in UnionDealServiceCode0310:: status[%s] != 1 or != 2  !\n",reqStatus);
		return(ret);
	}

	if (reqStatus[0] == '0')
	{
		strcpy(reqStatus,"2");
		goto end;
	}

	snprintf(sql,sizeof(sql),"select * from approval where recordID = %d",recordID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	
	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: UnionLocateXMLPackage!\n");
		return(ret);
	}

	memset(status,0,sizeof(status));
	if ((ret = UnionReadXMLPackageValue("status", status, sizeof(status))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: UnionReadXMLPackageValue[%s]!\n","status");
		return(ret);
	}

	if (atoi(status) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: status[%s] != 0!\n","status");
		UnionSetResponseRemark("��¼����Ϊ����״̬�ſ����");
		return(ret);
	}

end:
	memset(reqDateTime,0,sizeof(reqDateTime));
	UnionGetFullSystemDateTime(reqDateTime);
	snprintf(sql,sizeof(sql),"update approval set expirationTime = '%s',status = %s,opinion = '%s',approvalTime = '%s' where recordID = %d",expirationTime,reqStatus,opinion,reqDateTime,recordID);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0310:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);	
	}
	
	return(ret);
}
