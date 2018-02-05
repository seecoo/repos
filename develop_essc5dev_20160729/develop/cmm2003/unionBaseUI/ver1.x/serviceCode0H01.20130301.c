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
#include "unionVersion.h"
#include "clusterSyn.h"
#include "baseUIService.h"

/*********************************
服务代码:	0H01
服务名:		修改密码机组
功能描述:	修改密码机组
**********************************/

int UnionDealServiceCode0H01(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			i = 0;
	int			j = 0;
	int			reloadFlag = 0;
	int			localEnabled = 0;
	int			totalNum = 0;
	char			hsmGroupID[8+1];
	char			hsmGroupName[40+1];
	int			hsmCmdVersionID = 0;
	int			lenOfCommHead = 0;
	int			lenOfMsgHead = 0;
	char			reqCmdOfTest[128+1];
	char			resCmdOfTest[128+1];
	char			sensitiveCmd[512+1];
	char			remark[128+1];
	char			tmpBuf[512+1];
	char			hsmID[16+1];
	char			sql[2048+1];
	int			enabled;
	int			numOfCluster = 0;

	TUnionClusterSyn	clusterSyn;
	
	// 密码机组ID 
	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmGroupID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 密码机组名 
	memset(hsmGroupName,0,sizeof(hsmGroupName));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupName",hsmGroupName,sizeof(hsmGroupName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmGroupName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 密码机指令类型
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmCmdVersionID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmCmdVersionID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmCmdVersionID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		hsmCmdVersionID = atoi(tmpBuf);

	// 通讯头长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/lenOfCommHead",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/lenOfCommHead");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/lenOfCommHead");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		lenOfCommHead = atoi(tmpBuf);

	// 消息头长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/lenOfMsgHead");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/lenOfMsgHead");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		lenOfMsgHead = atoi(tmpBuf);

	// 状态
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/enabled");
		return(ret);
	}
	enabled = atoi(tmpBuf);

	// 探测请求指令
	memset(reqCmdOfTest,0,sizeof(reqCmdOfTest));
	if ((ret = UnionReadRequestXMLPackageValue("body/reqCmdOfTest",reqCmdOfTest,sizeof(reqCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/reqCmdOfTest");
		return(ret);
	}

	// 探测响应指令
	memset(resCmdOfTest,0,sizeof(resCmdOfTest));
	if ((ret = UnionReadRequestXMLPackageValue("body/resCmdOfTest",resCmdOfTest,sizeof(resCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadRequestXMLPackageValue[%s]!\n","body/resCmdOfTest");
		return(ret);
	}

	// 敏感指令
	memset(sensitiveCmd,0,sizeof(sensitiveCmd));
	if ((ret = UnionReadRequestXMLPackageValue("body/sensitiveCmd",sensitiveCmd,sizeof(sensitiveCmd))) < 0)
	{
		strcpy(sensitiveCmd,"");
	}
	
	// 备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) < 0)
	{
		strcpy(remark,"");
	}

	// 查询数据库中密码机组
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from hsmGroup where hsmGroupID = '%s'",hsmGroupID);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: hsmGroupID[%s]\n",hsmGroupID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionLocateXMLPackage!\n");
		return(ret);
	}

	// 读取密码机组名 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("hsmGroupName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","hsmGroupName");
		return(ret);
	}

	if (strcmp(tmpBuf,hsmGroupName) != 0)
		reloadFlag = 1;
	
	// 读取密码机指令类型
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("hsmCmdVersionID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","hsmCmdVersionID");
		return(ret);
	}
	if (atoi(tmpBuf) != hsmCmdVersionID)
		reloadFlag = 1;

	// 读取通讯头长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("lenOfCommHead",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
		return(ret);
	}
	if (atoi(tmpBuf) != lenOfCommHead)
		reloadFlag = 1;

	// 读取消息头长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","lenOfMsgHead");
		return(ret);
	}

	if (atoi(tmpBuf) != lenOfMsgHead)
		reloadFlag = 1;

	// 读取状态
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("enabled",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","enabled");
		return(ret);
	}
	localEnabled = atoi(tmpBuf);
	if (localEnabled != enabled)
		reloadFlag = 1;
	
	// 读取探测请求指令
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("reqCmdOfTest",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","reqCmdOfTest");
		return(ret);
	}
	if (strcmp(tmpBuf,reqCmdOfTest) != 0)
		reloadFlag = 1;

	// 读取探测响应指令
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("resCmdOfTest",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","resCmdOfTest");
		return(ret);
	}

	if (strcmp(tmpBuf,resCmdOfTest) != 0)
		reloadFlag = 1;

	// 读取敏感指令
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("sensitiveCmd",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		strcpy(tmpBuf,"");
	}

	if (strcmp(tmpBuf,sensitiveCmd) != 0)
		reloadFlag = 1;

	// 建立sql修改语句
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update hsmGroup set hsmGroupName = '%s', "
		"hsmCmdVersionID = %d, "
		"lenOfCommHead = %d, "
		"lenOfMsgHead = %d, "
		"reqCmdOfTest = '%s', "
		"resCmdOfTest = '%s', "
		"sensitiveCmd = '%s', "
		"enabled = %d, "
		"remark = '%s' "
		"where hsmGroupID = '%s'",
		hsmGroupName,hsmCmdVersionID,lenOfCommHead,lenOfMsgHead,reqCmdOfTest,resCmdOfTest,sensitiveCmd,enabled,remark,hsmGroupID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}

	if (reloadFlag == 0)
		return 0;

	numOfCluster = UnionReadIntTypeRECVar("numOfCluster");

	// 查找加密机
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select hsmID from hsm where hsmGroupID = '%s' order by hsmID",hsmGroupID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		return(ret);	
	}

	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s]!\n","totalNum");	
		return(ret);
	}
	else
		totalNum = atoi(tmpBuf);
	
	for (i = 0; i < totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H01:: UnionLocateXMLPackage [%d]!\n",i+1);
			return(ret);
		}

		// 读取密码机组名 
		memset(hsmID,0,sizeof(hsmID));
		if ((ret = UnionReadXMLPackageValue("hsmID",hsmID,sizeof(hsmID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H01:: UnionReadXMLPackageValue[%s][%d]!\n","hsmID",i+1);
			return(ret);
		}

		if (localEnabled != enabled)	// 状态变更
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"hsmSvr %s",hsmID);

			// 密码机组状态变为锁定状态
			if (enabled == 0)
			{
				memset(sql,0,sizeof(sql));
				sprintf(sql,"delete from unionTask where taskName = '%s'",tmpBuf);
				if ((ret = UnionExecRealDBSql(sql)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0H01:: UnionExecRealDBSql sql = [%s]!\n",sql);
					return(ret);
				}
				// 密码机状态置为异常、锁定
				memset(sql,0,sizeof(sql));
				sprintf(sql,"update hsm set status = 0,enabled = 0 where hsmID = '%s'",hsmID);
				if ((ret = UnionExecRealDBSql(sql)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0H01:: UnionExecRealDBSql sql = [%s]!\n",sql);
					return(ret);
				}	
			}
			else
			{
				sprintf(sql,"insert into unionTask (taskName,startCmd,taskNum,logFileName) "
					"values('%s','%s',%d,'%s') ",tmpBuf,tmpBuf,5,"hsmSvr");
				if ((ret = UnionExecRealDBSql(sql)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0H01:: UnionExecRealDBSql sql = [%s]!\n",sql);
					return(ret);
				}
				// 密码机状态置为异常、激活
				memset(sql,0,sizeof(sql));
				sprintf(sql,"update hsm set status = 0,enabled = 1 where hsmID = '%s'",hsmID);
				if ((ret = UnionExecRealDBSql(sql)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0H01:: UnionExecRealDBSql sql = [%s]!\n",sql);
					return(ret);
				}	
			}
			
		}
		
		for (j = 0; j < numOfCluster; j++)
		{
			memset(&clusterSyn,0,sizeof(clusterSyn));
			sprintf(clusterSyn.cmd,"mngTask -clnanyway \"hsmSvr %s\";mngTask -reloadanyway",hsmID);
			clusterSyn.clusterNo = j + 1;
			UnionGetFullSystemDateTime(clusterSyn.regTime);
			if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H01:: UnionInsertClusterSyn!\n");
				return(ret);
			}
		}	

		// 查找加密机
		memset(sql,0,sizeof(sql));
		sprintf(sql,"select hsmID from hsm where hsmGroupID = '%s' order by hsmID",hsmGroupID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H01:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			return(ret);	
		}
	}
	return(0);
}
