
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <math.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "clusterSyn.h"
#include "unionREC.h"
#include "unionLisence.h"
#include "baseUIService.h"

/*********************************
服务代码:	0309
服务名:		修改授权码
功能描述:	修改授权码
**********************************/

int UnionDealServiceCode0309(PUnionHsmGroupRec phsmGroupRec)
{
	int	i,j;
	int	ret;
	char	nameOfMyself[128+1];
	char	serialNumber[128+1];
	char	liscenceCode[16+1];
	char	maintainDate[8+1];
	char	endDate[8+1];
	char	sql[512+1];
	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;

	// 客户名称
	memset(nameOfMyself,0,sizeof(nameOfMyself));
	if ((ret = UnionReadRequestXMLPackageValue("body/nameOfMyself",nameOfMyself,sizeof(nameOfMyself))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionReadRequestXMLPackageValue[%s]!\n","body/nameOfMyself");
		return(ret);
	}

	// 序列号
	memset(serialNumber,0,sizeof(serialNumber));
	if ((ret = UnionReadRequestXMLPackageValue("body/serialNumber",serialNumber,sizeof(serialNumber))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionReadRequestXMLPackageValue[%s]!\n","body/serialNumber");
		return(ret);
	}

	// 授权码
	memset(liscenceCode,0,sizeof(liscenceCode));
	if ((ret = UnionReadRequestXMLPackageValue("body/liscenceCode",liscenceCode,sizeof(liscenceCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionReadRequestXMLPackageValue[%s]!\n","body/liscenceCode");
		return(ret);
	}
	
	/*
	=2		校验成功
	=1		到维护期
	=0		校验失败
	=-1		到截止使用日期
	<-1		出错代码
	*/
	memset(maintainDate,0,sizeof(maintainDate));
	memset(endDate,0,sizeof(endDate));
	if ((ret = UnionVerifyFinalLisenceKeyUseSpecInputData(nameOfMyself,serialNumber,maintainDate,endDate,liscenceCode)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionVerifyFinalLisenceKeyUseSpecInputData[%s][%s][%s]!\n",nameOfMyself,serialNumber,liscenceCode);
		return(errCodeLiscenceCodeInvalid);
	}
	
	// 更新授权码
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update unionREC set varValue='%s' where varSysName='nameOfMyself'",nameOfMyself);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
/*	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}*/
	
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update unionREC set varValue='%s' where varSysName='serialNumber'",serialNumber);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	/*else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}*/

	memset(sql,0,sizeof(sql));
	sprintf(sql,"update unionREC set varValue='%s' where varSysName='liscenceCode'",liscenceCode);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	/*else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0309:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}*/

	// 更新共享内存
	memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionInitClusterDef!\n");
		return(ret);
	}

	for (i = 0; i < clusterDefTBL.realNum; i++)
	{
		if (strcasecmp("unionREC",clusterDefTBL.rec[i].tableName) == 0)
		{
			for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
			{
				memset(&clusterSyn,0,sizeof(clusterSyn));
				clusterSyn.clusterNo = j + 1;
				sprintf(clusterSyn.cmd,"%s",clusterDefTBL.rec[i].cmd);
				UnionGetFullSystemDateTime(clusterSyn.regTime);
				if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0309:: UnionInsertClusterSyn!\n");
					return(ret);
				}
			}
		}
	}

	// 设置响应数据
	if ((ret = UnionSetResponseXMLPackageValue("body/maintainDate",maintainDate)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: UnionSetResponseXMLPackageValue[%s]!\n","body/maintainDate");
		return(ret);
	}
	if ((ret = UnionSetResponseXMLPackageValue("body/endDate",endDate)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: UnionSetResponseXMLPackageValue[%s]!\n","body/endDate");
		return(ret);
	}

	return(0);
}
