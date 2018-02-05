//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-02-22

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
#include "unionHsmCmd.h"
#include "UnionSocket.h"

/*********************************
服务代码:	0H04
服务名:		修改密码机
功能描述:	修改密码机
**********************************/
static int setHsmGroupResCmdOfTestByHsm0H04(char *ipAddr,int port,char *hsmGroupID);
static int getCheckValueByHsmIP0H04(char *ipAddr,int port,int lenOfCommHead,int lenOfMsgHead,char *checkValue);
int UnionDealServiceCode0H04(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0;
	int	taskNum = 0;
	int	reloadFlag = 0;
	char	hsmID[16+1];
	char	hsmName[40+1];
	char	hsmGroupID[8+1];
	char	ipAddr[32+1];
	int	port = 0;
	int	uses = 0;
	int	enabled = 0;
	char	remark[128+1];
	char	tmpBuf[512+1];
	char	sql[2048+1];
	int	j = 0;

	TUnionClusterSyn	clusterSyn;
	
	// 密码机ID 
	memset(hsmID,0,sizeof(hsmID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmID",hsmID,sizeof(hsmID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 密码机名称
	memset(hsmName,0,sizeof(hsmName));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmName",hsmName,sizeof(hsmName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 密码机组ID
	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmGroupID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// IP地址 
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadRequestXMLPackageValue("body/ipAddr",ipAddr,sizeof(ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s]!\n","body/ipAddr");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/ipAddr");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// modify  by leipp 20150409 ,为了支持域名解析，去除检查IP地址合法性
	// 验证IP合法性
	/*if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: ipAddr[%s] error!\n",ipAddr);
		return(errCodeInvalidIPAddr);
	}*/

	// 端口 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/port",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s]!\n","body/port");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/port");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		port = atoi(tmpBuf);

	// 用途
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/uses",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s]!\n","body/uses");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/uses");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		uses = atoi(tmpBuf);

	// 激活
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s]!\n","body/enabled");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/enabled");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		enabled = atoi(tmpBuf);

	// 备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) < 0)
	{
		strcpy(remark,"");
	}

	// 任务数
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/taskNum",tmpBuf,sizeof(tmpBuf))) < 0)
		taskNum = 5;
	else
		taskNum = atoi(tmpBuf);
	
	// 查询加密机数据
	snprintf(sql,sizeof(sql),"select * from hsm where hsmID = '%s'",hsmID);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: hsmID[%s] not found!\n",hsmID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// 密码机名称
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("hsmName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadXMLPackageValue[%s]!\n","hsmName");
		return(ret);
	}

	if (strcmp(tmpBuf,hsmName) != 0)
		reloadFlag = 1;	

	// 密码机组ID
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("hsmGroupID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
		return(ret);
	}

	if (strcmp(tmpBuf,hsmGroupID) != 0)
		reloadFlag = 1;

	// IP地址 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("ipAddr",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
		return(ret);
	}

	if (strcmp(tmpBuf,ipAddr) != 0)
		reloadFlag = 1;

	// 端口 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("port",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadXMLPackageValue[%s]!\n","port");
		return(ret);
	}

	if (atoi(tmpBuf) != port)
		reloadFlag = 1;

	// 用途
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("uses",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadXMLPackageValue[%s]!\n","uses");
		return(ret);
	}

	if (atoi(tmpBuf) != uses)
		reloadFlag = 1;
	
	// 激活
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("enabled",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionReadXMLPackageValue[%s]!\n","enabled");
		return(ret);
	}
	
	if (atoi(tmpBuf) != enabled)
		reloadFlag = 1;

	if (enabled)
	{
		if ((ret = setHsmGroupResCmdOfTestByHsm0H04(ipAddr, port,hsmGroupID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H04:: setHsmGroupResCmdOfTestByHsm0H04[%s][%d][%s]!\n",ipAddr,port,hsmGroupID);
			return(ret);
		}
	}
	
	// 建立sql插入语句
	if (reloadFlag)
	{
		snprintf(sql,sizeof(sql),"update hsm set hsmName = '%s', "
			"hsmGroupID = '%s', "
			"ipAddr = '%s', "
			"port = %d, "
			"uses = %d, "
			"status = 0, "
			"enabled = %d, "
			"remark = '%s' "
			"where hsmID = '%s'",
			hsmName,hsmGroupID,ipAddr,port,uses,enabled,remark,hsmID);
	}
	else
	{
		snprintf(sql,sizeof(sql),"update hsm set hsmName = '%s', "
			"hsmGroupID = '%s', "
			"ipAddr = '%s', "
			"port = %d, "
			"uses = %d, "
			"enabled = %d, "
			"remark = '%s' "
			"where hsmID = '%s'",
			hsmName,hsmGroupID,ipAddr,port,uses,enabled,remark,hsmID);
	}
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 更新密码机任务
	if (enabled == 0)
		taskNum = 0;

	snprintf(sql,sizeof(sql),"update unionTask set taskNum = %d where taskName = 'hsmSvr %s'",taskNum,hsmID);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H04:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 更新共享内存
	if (reloadFlag)
	{
		for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
		{
			memset(&clusterSyn,0,sizeof(clusterSyn));
			sprintf(clusterSyn.cmd,"mngTask -clnanyway \"hsmSvr %s\";mngTask -reloadanyway",hsmID);
			clusterSyn.clusterNo = j + 1;
			UnionGetFullSystemDateTime(clusterSyn.regTime);
			if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H04:: UnionInsertClusterSyn!\n");
				return(ret);
			}
		}
	}

	return(0);
}

// add by leipp 20151209
static int getCheckValueByHsmIP0H04(char *ipAddr,int port,int lenOfCommHead,int lenOfMsgHead,char *checkValue)
{
	int		ret = 0;	
	int		hsmSck = 0;
	unsigned char	reqBuf[64];
	unsigned char	resBuf[128];
	char		lenBuf[16];
	int		len = 0;
	char		ip[32];

	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		memset(ip,0,sizeof(ip));
		if ((ret = UnionGetHostByName(ipAddr,ip)) < 0)
		{
			UnionUserErrLog("in getCheckValueByHsmIP0H04:: UnionGetHostByName[%s]!\n", ip);
			return(ret);
		}
	}

	if (lenOfCommHead == 2)
	{
		reqBuf[0] = (lenOfMsgHead + 2) / 256;
		reqBuf[1] = (lenOfMsgHead + 2) % 256;
	}
	else if (lenOfCommHead > 2)
	{
		snprintf((char *)reqBuf, sizeof(reqBuf), "%0*d", lenOfCommHead,lenOfCommHead + lenOfMsgHead + 2);
	}
	memset(reqBuf + lenOfCommHead,'0', lenOfMsgHead);
	memcpy(reqBuf + lenOfCommHead + lenOfMsgHead,"NC",2);
	len = lenOfCommHead + lenOfMsgHead + 2;
	reqBuf[len] = 0;

	if ((hsmSck = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in getCheckValueByHsmIP0H04:: UnionCreateSocketClient[%s][%d]!\n",ipAddr,port);
		return(hsmSck);
	}

	UnionLog("in getCheckValueByHsmIP0H04:: req[%s:%d][%04d][%s]!\n",ipAddr,port,len-lenOfCommHead-lenOfMsgHead,reqBuf+lenOfCommHead+lenOfMsgHead);
	if ((ret = UnionSendToSocket(hsmSck,reqBuf,len)) < 0)
	{
		UnionUserErrLog("in getCheckValueByHsmIP0H04:: UnionSendToSocket[%s][%d]!\n",ipAddr,port);
		UnionCloseSocket(hsmSck);
		return(ret);
	}
	
	if ((ret = UnionReceiveFromSocketUntilLen(hsmSck,resBuf,lenOfCommHead)) != lenOfCommHead)
	{
		UnionUserErrLog("in getCheckValueByHsmIP0H04:: UnionReceiveFromSocketUntilLen!\n");
		UnionCloseSocket(hsmSck);
		return(errCodeSocketMDL_ReceiveLen);
	}
	if (lenOfCommHead == 2)
		len = resBuf[0] * 256 + resBuf[1];
	else if (lenOfCommHead > 0)
	{
		memcpy(lenBuf,resBuf,lenOfCommHead);
		lenBuf[lenOfCommHead] = 0;
		len = atoi(lenBuf);
	}

	if ((ret = UnionReceiveFromSocketUntilLen(hsmSck,resBuf,len)) != len)
	{
		UnionUserErrLog("in getCheckValueByHsmIP0H04:: UnionReceiveFromSocketUntilLen ret[%d] len[%d]!\n",ret,len);
		UnionCloseSocket(hsmSck);
		return(errCodeSocketMDL_ReceiveLen);
	}
	resBuf[len] = 0;

	UnionLog("in getCheckValueByHsmIP0H04:: res[%s:%d][%04d][%s]!\n",ipAddr,port,len-lenOfMsgHead,resBuf+lenOfMsgHead);
	UnionCloseSocket(hsmSck);
	memcpy(checkValue,resBuf+lenOfMsgHead+4,16);
	checkValue[16] = 0;
	return 0;
}

static int setHsmGroupResCmdOfTestByHsm0H04(char *ipAddr,int port,char *hsmGroupID)
{
	int	ret = 0;
	int	i = 0,j = 0;
	int	len = 0;
	char	sql[128];
	int	lenOfMsgHead = 0;
	int	lenOfCommHead = 0;
	int	hsmNum = 0;
	char	tmpBuf[128];
	char	checkValue[32];
	char	condition[64];
	char	dstVersion[32];

	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;

	snprintf(condition,sizeof(condition),"hsmGroupID = '%s' and enabled = 1",hsmGroupID);
	if ((hsmNum = UnionSelectRealDBRecordCounts("hsm",NULL,condition)) < 0)
	{
		UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionSelectRealDBRecordCounts[hsm][NULL][%s]!\n",condition);
		return(hsmNum);
	}

	len = sprintf(sql,"select lenOfCommHead,lenOfMsgHead from hsmGroup where hsmGroupID = '%s' and reqCmdOfTest = 'NC'",hsmGroupID);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionLocateXMLPackage [%s]!\n","detail");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("lenOfCommHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
			return(ret);
		}
		tmpBuf[ret] = 0;
		lenOfCommHead = atoi(tmpBuf);

		if ((ret = UnionReadXMLPackageValue("lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
			return(ret);
		}
		tmpBuf[ret] = 0;
		lenOfMsgHead = atoi(tmpBuf);

		if ((ret = getCheckValueByHsmIP0H04(ipAddr,port,lenOfCommHead,lenOfMsgHead,checkValue)) < 0)
		{               
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: getCheckValueByHsmIP0H04 ipAddr[%s] port[%d]!\n",ipAddr,port);
			return(ret);
		}

		if (hsmNum > 0)
		{
			UnionSetHsmGroupIDForHsmSvr(hsmGroupID);
			if ((ret = UnionHsmCmdNC(dstVersion)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H04:: UnionHsmCmdNC!\n");
				return(ret);
			}

			if (memcmp(checkValue,dstVersion,16) != 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H03:: UnionHsmCmdNC new checkValue[%s] != old checkValue[%s]!\n",checkValue,dstVersion);
				UnionSetResponseRemark("该加密机与组内加密机主密钥不相同");
				return(errCodeParameter);
			}
		}
		else
		{
			len = sprintf(sql,"update hsmGroup set resCmdOfTest = 'ND00%s' where hsmGroupID = '%s' and reqCmdOfTest = 'NC'",checkValue,hsmGroupID);
			sql[len] = 0;
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionExecRealDBSql[%s]!\n",sql);
				return(ret);
			}
		}
		// 更新共享内存
		memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
		if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionInitClusterDef!\n");
			return(ret);
		}

		for (i = 0; i < clusterDefTBL.realNum; i++)
		{
			if (strcasecmp("hsmGroup",clusterDefTBL.rec[i].tableName) == 0)
			{
				for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
				{
					memset(&clusterSyn,0,sizeof(clusterSyn));
					len = 0;
					clusterSyn.clusterNo = j + 1;
					sprintf(clusterSyn.cmd+len,"%s",clusterDefTBL.rec[i].cmd);
					UnionGetFullSystemDateTime(clusterSyn.regTime);
					if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
					{
						UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm0H04:: UnionInsertClusterSyn!\n");
						return(ret);
					}
				}
			}
		}
	}

	return 0;
}
// add by leipp 20151209 end
