
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
服务代码:	0303
服务名:		删除记录
功能描述:	删除记录
**********************************/

int UnionDealServiceCode0303(PUnionHsmGroupRec phsmGroupRec)
{
	int	i,j;
	int	len;
	int	ret;
	char	tableName[128];
	char	sql[4096+512];
	char	condition[4096];
	char	taskName[128];
	int	port = -1;
	char	*ptr = NULL;
	
	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;

	// 表名
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0303:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tableName);
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0303:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 只取第一个表
	if ((ptr = strchr(tableName,',')))
		*ptr = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/condition",condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0303:: UnionReadRequestXMLPackageValue[%s]!\n","body/condition");
		return(ret);
		// UnionLog("in UnionDealServiceCode0303:: UnionReadRequestXMLPackageValue[%s]!\n","body/condition");
		// strcpy(condition,"1 = 1");
	}

	UnionFilterHeadAndTailBlank(condition);

	// added 2015-07-13
	if(strlen(condition)  == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0303:: condition = '%s' 需要指定删除条件删除!\n", condition);
		UnionSetResponseRemark("删除条件为'', 删除服务需要指定条件");
		return(errCodeSQLInvalidSQLStr);
	}
	// end of addition 2015-07-13
	
	if (strcasecmp(tableName,"unionTask") == 0)
	{
		memset(taskName,0,sizeof(taskName));
		if (memcmp(condition,"taskName='",10) == 0)
		{
			if ((ptr = strstr(condition + 10,"'")) != NULL)
			{
				memcpy(taskName,condition + 10, ptr - condition - 10);
				if ((strcmp(taskName,"dbSvr") == 0) || (strcmp(taskName,"mngClusterSyn") == 0))
				{
					UnionUserErrLog("in UnionDealServiceCode0303:: taskName[%s]不允许删除!\n",taskName);
					UnionSetResponseRemark("此记录不允许删除");
					return(ret);
				}
			}
		}
	}

	// 建立sql删除语句
	if (strlen(condition) > 0)
		snprintf(sql,sizeof(sql), "delete from %s where %s ",tableName,condition);
	else
		snprintf(sql, sizeof(sql), "delete from %s",tableName);
		
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0303:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 更新共享内存
	memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionInitClusterDef!\n");
		return(ret);
	}

	for (i = 0; i < clusterDefTBL.realNum; i++)
	{
		if (strcasecmp(tableName,clusterDefTBL.rec[i].tableName) == 0)
		{
			for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
			{
				memset(&clusterSyn,0,sizeof(clusterSyn));
				len = 0;
				if (strcasecmp(tableName,"unionTask") == 0)
				{
					memset(taskName,0,sizeof(taskName));
					if (memcmp(condition,"taskName='",10) == 0)
					{
						if ((ptr = strstr(condition + 10,"'")) != NULL)
						{
							memcpy(taskName,condition + 10, ptr - condition - 10);
							if (memcmp(taskName,"appTask",7) == 0)
							{
								if ((ptr = strstr(taskName," ")) != NULL)
								{
									*ptr = 0;
									port = atoi(ptr+1);
								}
								len = sprintf(clusterSyn.cmd,"mngTask -clnanyway \" %d\";",port);
							}
							else
								len = sprintf(clusterSyn.cmd,"mngTask -clnanyway \"%s\";",taskName);
						}
					}
				}
				clusterSyn.clusterNo = j + 1;
				sprintf(clusterSyn.cmd+len,"%s",clusterDefTBL.rec[i].cmd);
				UnionGetFullSystemDateTime(clusterSyn.regTime);
				if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0302:: UnionInsertClusterSyn!\n");
					return(ret);
				}
			}
		}
	}

	return(0);
}
