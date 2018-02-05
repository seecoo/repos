
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
#include "unionREC.h"
#include "UnionTask.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "errCodeTranslater.h"
#include "highCached.h"
#include "clusterSyn.h"
#include "baseUIService.h"

/*********************************
服务代码:	0H02
服务名:		删除密码机组
功能描述:	删除密码机组
**********************************/

int UnionDealServiceCode0H02(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	i = 0;
	int	j = 0;
	int	totalNum = 0;
	char	hsmGroupID[8+1];
	char	hsmID[16+1];
	char	sql[1024+1];
	char	tmpBuf[128+1];

	TUnionClusterSyn	clusterSyn;
	
	// 读取密码机ID 
	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H02:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H02:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmGroupID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 查询密码机组下的密码机
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select hsmID from hsm where hsmGroupID = '%s' ",hsmGroupID);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H02:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		goto deleteHsmGroup;		
	}
	
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H02:: UnionReadXMLPackageValue[%s]!\n","totalNum");	
		return(ret);
	}
	else
		totalNum = atoi(tmpBuf);

	for (i = 0; i < totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H02:: UnionLocateXMLPackage!\n");
			return(ret);
		}

		// 读取密码机组名 
		memset(hsmID,0,sizeof(hsmID));
		if ((ret = UnionReadXMLPackageValue("hsmID",hsmID,sizeof(hsmID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H02:: UnionReadXMLPackageValue[%s][%d]!\n","hsmID",i+1);
			return(ret);
		}

		// 删除加密机
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from hsm where hsmID = '%s'",hsmID);

		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H02:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	
		// 删除任务表中密码机
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from unionTask where taskName = 'hsmSvr %s'",hsmID);

		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H02:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}

		// 更新共享内存
		for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
		{
			memset(&clusterSyn,0,sizeof(clusterSyn));
			sprintf(clusterSyn.cmd,"mngTask -clnanyway \"hsmSvr %s\";mngTask -reloadanyway",hsmID);
			clusterSyn.clusterNo = j + 1;
			UnionGetFullSystemDateTime(clusterSyn.regTime);
			if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H02:: UnionInsertClusterSyn!\n");
				return(ret);
			}
		}
	}

deleteHsmGroup:
	// 删除加密机组
	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from hsmGroup where hsmGroupID = '%s' ",hsmGroupID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H02:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 删除私钥索引
	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from vkKeyIndex where hsmGroupID = '%s' ",hsmGroupID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H02:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	
	/*// 更新共享内存
	if (isReload)
	{
		usleep(200);
		UnionReloadTaskTBL();
	}*/
	return(0);
}
