
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
服务代码:	0H05
服务名:		删除密码机
功能描述:	删除密码机
**********************************/

static int getHsmGroupIDByHsmID(char *hsmID,char *hsmGroupID,int sizeofHsmGroupID);
static int delHsmGroupCheckValueOfResCmdOfTest(char *hsmGroupID);
int UnionDealServiceCode0H05(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	hsmID[32];
	char	sql[256];
	char	tmpBuf[132];
	int	j = 0;
	char	hsmGroupID[16];
	
	TUnionClusterSyn	clusterSyn;
	// 表名
	memset(hsmID,0,sizeof(hsmID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmID",hsmID,sizeof(hsmID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H05:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H05:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// add by leipp 20151104
	// 获取密码机组ID
	if ((ret = getHsmGroupIDByHsmID(hsmID,hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H05:: getHsmGroupIDByHsmID hsmID[%s]!\n",hsmID);
		return(ret);
	}
	// add by leipp 20151104 end


	// 建立sql删除语句
	snprintf(sql,sizeof(sql),"delete from hsm where hsmID = '%s'",hsmID);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H05:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// add by leipp 20151104
	// 去除密码机组中的校验值
	if ((ret = delHsmGroupCheckValueOfResCmdOfTest(hsmGroupID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H05:: delHsmGroupCheckValueOfResCmdOfTest[%s]!\n",hsmGroupID);
		return(ret);
	}
	// add by leipp 20151104 end
	
	// 删除任务表中密码机
	snprintf(tmpBuf,sizeof(tmpBuf),"hsmSvr %s",hsmID);
	snprintf(sql,sizeof(sql),"delete from unionTask where taskName = '%s'",tmpBuf);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H05:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
	{
		memset(&clusterSyn,0,sizeof(clusterSyn));
		sprintf(clusterSyn.cmd,"mngTask -clnanyway \"hsmSvr %s\";mngTask -reloadanyway",hsmID);
		clusterSyn.clusterNo = j + 1;
		UnionGetFullSystemDateTime(clusterSyn.regTime);
		if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H05:: UnionInsertClusterSyn!\n");
			return(ret);
		}
	}
	
	return(0);
}

static int getHsmGroupIDByHsmID(char *hsmID,char *hsmGroupID,int sizeofHsmGroupID)
{
	int	ret = 0;
	char	sql[256];

	snprintf(sql,sizeof(sql),"select hsm.hsmGroupID from hsm left join hsmGroup on hsm.hsmGroupID = hsmGroup.hsmGroupID where hsm.hsmID = '%s' and hsmGroup.reqCmdOfTest = 'NC'",hsmID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in getHsmGroupIDByHsmID:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
		{
			UnionUserErrLog("in getHsmGroupIDByHsmID:: UnionLocateXMLPackage [%s]!\n","detail");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("hsmGroupID",hsmGroupID,sizeofHsmGroupID)) < 0)
		{
			UnionUserErrLog("in getHsmGroupIDByHsmID:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
			return(ret);
		}
		hsmGroupID[ret] = 0;
	}
	else
		hsmGroupID[0] = 0;
	return 0;
}

static int delHsmGroupCheckValueOfResCmdOfTest(char *hsmGroupID)
{
	int			ret = 0;
	int			i = 0,j = 0;
	char			sql[256];
	int			allTotalNum = 0;
	
	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;

	snprintf(sql,sizeof(sql),"hsmGroupID = '%s'",hsmGroupID);
	if ((allTotalNum = UnionSelectRealDBRecordCounts("hsm",NULL,sql)) < 0)
	{
		UnionUserErrLog("in delHsmGroupCheckValueOfResCmdOfTest:: UnionSelectRealDBRecordCounts[hsm][][%s]!\n",sql);
		return(allTotalNum);
	}
	if (allTotalNum > 0)
		return 0;

	snprintf(sql,sizeof(sql),"update hsmGroup set resCmdOfTest = 'ND00' where hsmGroupID = '%s' and reqCmdOfTest = 'NC'",hsmGroupID);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in delHsmGroupCheckValueOfResCmdOfTest:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	// 更新共享内存
	memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in delHsmGroupCheckValueOfResCmdOfTest:: UnionInitClusterDef!\n");
		return(ret);
	}

	for (i = 0; i < clusterDefTBL.realNum; i++)
	{
		if (strcasecmp("hsmGroup",clusterDefTBL.rec[i].tableName) == 0)
		{
			for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
			{
				memset(&clusterSyn,0,sizeof(clusterSyn));
				clusterSyn.clusterNo = j + 1;
				sprintf(clusterSyn.cmd,"%s",clusterDefTBL.rec[i].cmd);
				UnionGetFullSystemDateTime(clusterSyn.regTime);
				if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
				{
					UnionUserErrLog("in delHsmGroupCheckValueOfResCmdOfTest:: UnionInsertClusterSyn!\n");
					return(ret);
				}
			}
		}
	}

	return 0;
}
