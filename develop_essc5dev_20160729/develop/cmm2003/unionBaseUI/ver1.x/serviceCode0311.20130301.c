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
#include "UnionTask.h"
#include "unionLisence.h"
#include "baseUIService.h"

/*********************************
服务代码:       0311
服务名:         停止守护进程子进程
功能描述:       停止守护进程子进程
**********************************/

int UnionDealServiceCode0311(PUnionHsmGroupRec phsmGroupRec)
{      
        int     ret;
        char    taskID[40+1];
        //char    tmpBuf[512+1];
        char    sql[512+1];
	int     j = 0;

        TUnionClusterSyn        clusterSyn;
	

        // 任务标识
        memset(taskID,0,sizeof(taskID));
        if ((ret = UnionReadRequestXMLPackageValue("body/taskID",taskID,sizeof(taskID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode0311:: UnionReadRequestXMLPackageValue[%s]!\n","body/taskID");
                return(ret);
        }
	
	memset(sql,0,sizeof(sql));
        sprintf(sql,"update daemonTask set enabled=0 where taskID='%s'",taskID);
        if ((ret = UnionExecRealDBSql(sql)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode0311:: UnionExecRealDBSql[%s]!\n",sql);
                return(ret);
        }
	
	//memset(tmpBuf,0,sizeof(tmpBuf));
	//sprintf(tmpBuf,"mngTask -cln %s",taskID);

	if ((ret = UnionKillTaskInstanceByAlikeName(taskID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0311:: UnionKillTaskInstanceByAlikeName[%s]!\n",taskID);
                return(ret);
	}

	for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
        {
                memset(&clusterSyn,0,sizeof(clusterSyn));
                sprintf(clusterSyn.cmd,"mngTask -clnanyway  %s",taskID);
                clusterSyn.clusterNo = j + 1;
                UnionGetFullSystemDateTime(clusterSyn.regTime);
                if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0311:: UnionInsertClusterSyn!\n");
                        return(ret);
                }
        }
        return(0);
}
