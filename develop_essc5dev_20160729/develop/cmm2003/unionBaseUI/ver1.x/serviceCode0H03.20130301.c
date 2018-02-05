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

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "UnionTask.h"
#include "unionErrCode.h"
#include "UnionSocket.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "errCodeTranslater.h"
#include "highCached.h"
#include "unionVersion.h"
#include "clusterSyn.h"
#include "baseUIService.h"
#include "unionDesKey.h"
#include "unionHsmCmdVersion.h"
#include "unionHsmCmd.h"

/*********************************
服务代码:	0H03
服务名:		增加密码机
功能描述:	增加密码机
**********************************/

static int getCheckValueByHsmIP(char *ipAddr,int port,int lenOfCommHead,int lenOfMsgHead,char *checkValue);
static int setHsmGroupResCmdOfTestByHsm(char *ipAddr,int port,char *hsmGroupID);
int UnionDealServiceCode0H03(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret = 0;
	char			hsmID[16+1];
	char			hsmName[40+1];
	char			hsmGroupID[8+1];
	char			ipAddr[32+1];
	int			taskNum = 0;
	char			logFileName[100+1];
	int			port = 0;
	int			uses = 0;
	int			enabled = 0;
	char			remark[128+1];
	char			tmpBuf[512+1];
	char			sql[1024+1];
	int			len = 0;
	int			j = 0;
	char			oriCheckValue[48];
	char			dstVersion[48];
	int			lenOfMsgHead = 0;
	int			lenOfCommHead = 0;
/*	int			i = 0;
	int			totalNum = 0;
	char			checkValue[16+1];
	char			keyValue[48+1];
	char			modeID[16+1];
	char			protectKey[2][16+1]={"desProtectKey","sm4ProtectKey"};
	char			protectCV[2][16+1]={"desProtectCV","sm4ProtectCV"};
	TUnionDesKeyLength	desKeyLen;
*/
	TUnionClusterSyn	clusterSyn;

	// 密码机ID 
	memset(hsmID,0,sizeof(hsmID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmID",hsmID,sizeof(hsmID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 密码机名称
	memset(hsmName,0,sizeof(hsmName));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmName",hsmName,sizeof(hsmName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 密码机组ID
	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/hsmGroupID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// IP地址 
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadRequestXMLPackageValue("body/ipAddr",ipAddr,sizeof(ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s]!\n","body/ipAddr");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/ipAddr");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// modify 20150409 by leipp 为了支持域名解析，去除检查ip地址格式
	// 验证IP合法性
	/*if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: ipAddr[%s] error!\n",ipAddr);
		return(errCodeInvalidIPAddr);
	}*/

	// 端口 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/port",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s]!\n","body/port");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/port");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		port = atoi(tmpBuf);

	// 用途
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/uses",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s]!\n","body/uses");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/uses");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
		uses = atoi(tmpBuf);

	// 激活
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s]!\n","body/enabled");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/enabled");
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
	
	// 任务数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/taskNum",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		taskNum = 5;
	} 
	else
		taskNum = atoi(tmpBuf);

	if (enabled == 0)
		taskNum = 0;

	// 日志名
	memset(logFileName,0,sizeof(logFileName));
	if ((ret = UnionReadRequestXMLPackageValue("body/logFileName",logFileName,sizeof(logFileName))) <= 0)
	{
		strcpy(logFileName,"hsmSvr");
	}

	// 检查加密机与所属组其他加密机主密钥是否一致
	/*if (!UnionIsValidIPAddrStr(ipAddr))
	{
		if ((ret = UnionGetHostByName(ipAddr,ip)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionGetHostByName[%s]!\n", ip);
			return(ret);
		}	
	} */

	// add by leipp 20151016
	// 检查加密机组内是否有加密机
	len = sprintf(sql,"select hsm.ipAddr,hsmGroup.lenOfCommHead,hsmGroup.lenOfMsgHead from hsm left join hsmGroup on hsm.hsmGroupID = hsmGroup.hsmGroupID where hsm.hsmGroupID = '%s' and hsm.status = 1 and hsm.enabled = 1",hsmGroupID);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionLocateXMLPackage [%s]!\n","detail");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("lenOfCommHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
			return(ret);
		}
		tmpBuf[ret] = 0;
		lenOfCommHead = atoi(tmpBuf);

		if ((ret = UnionReadXMLPackageValue("lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
			return(ret);
		}
		tmpBuf[ret] = 0;
		lenOfMsgHead = atoi(tmpBuf);

		UnionSetHsmGroupIDForHsmSvr(hsmGroupID);
		if ((ret = UnionHsmCmdNC(dstVersion)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionHsmCmdNC!\n");
			return(ret);
		}

		memset(oriCheckValue,0,sizeof(oriCheckValue));
		if ((ret = getCheckValueByHsmIP(ipAddr,port,lenOfCommHead,lenOfMsgHead,oriCheckValue)) < 0)
                {               
			UnionUserErrLog("in UnionDealServiceCode0H03:: getCheckValueByHsmIP ipAddr[%s] port[%d]!\n",ipAddr,port);
			return(ret);
                }

		if (memcmp(oriCheckValue,dstVersion,16) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionHsmCmdNC!\n");
			UnionSetResponseRemark("该加密机与组内加密机主密钥不相同");
			return(errCodeParameter);
		}
	}
	else if (ret == 0)
	{
		// add by leipp 20151104
		if ((ret = setHsmGroupResCmdOfTestByHsm(ipAddr,port,hsmGroupID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: setHsmGroupResCmdOfTestByHsm ipAddr[%s] port[%d] hsmGroupID[%s]!\n",ipAddr,port,hsmGroupID);
			return(ret);
		}
		// add end
	}
	// add by leipp end
	
	// 建立sql插入语句
	len = sprintf(sql,"insert into hsm (hsmID,hsmName,hsmGroupID,ipAddr,port,uses,enabled,remark) "
		"values('%s','%s','%s','%s',%d,%d,%d,'%s')",
		hsmID,hsmName,hsmGroupID,ipAddr,port,uses,enabled,remark);
	sql[len] = 0;

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	
	// unionTask增加记录，默认5个
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"hsmSvr %s",hsmID);
	len = sprintf(sql,"insert into unionTask (taskName,startCmd,taskNum,logFileName) "
		"values('%s','%s',%d,'%s') ",tmpBuf,tmpBuf,taskNum,logFileName);
	sql[len] = 0;

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 更新共享内存
	for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
	{
		memset(&clusterSyn,0,sizeof(clusterSyn));
		strcpy(clusterSyn.cmd,"mngTask -reloadanyway");
		clusterSyn.clusterNo = j + 1;
		UnionGetFullSystemDateTime(clusterSyn.regTime);
		if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionInsertClusterSyn!\n");
			return(ret);
		}
	}

/*
	// 验证LMK保护方式中保护密钥与校验值
	UnionSetUseSpecHsmGroupForOneCmd(hsmGroupID);
	
	len = sprintf(sql,"select * from lmkProtectMode left join hsmGroup on lmkProtectMode.modeID = hsmGroup.lmkProtectMode where hsmGroupID = '%s'",hsmGroupID);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadXMLPackageValue [%s]!\n","totalNum");
		return(ret);
	}
	else
		totalNum = atoi(tmpBuf);

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionLocateXMLPackage [%s]!\n","detail");
			return(ret);
		}

		memset(modeID,0,sizeof(modeID));
		if ((ret = UnionReadXMLPackageValue("modeID",modeID,sizeof(modeID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadXMLPackageValue[%s]!\n","modeID");
			return(ret);
		}

		for (j = 0; j < 2; j++)
		{
			memset(keyValue,0,sizeof(keyValue));
			if ((ret = UnionReadXMLPackageValue(protectKey[j],keyValue,sizeof(keyValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadXMLPackageValue[%s]!\n",protectKey[j]);
				return(ret);
			}

			memset(checkValue,0,sizeof(checkValue));
			if ((ret = UnionReadXMLPackageValue(protectCV[j],checkValue,sizeof(checkValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadXMLPackageValue[%s]!\n",protectCV[j]);
				return(ret);
			}

			 switch (strlen(keyValue))
			{
				case    16:
				case    64:
						desKeyLen = con64BitsDesKey;
						break;
				case    32:
				case    128:
						desKeyLen = con128BitsDesKey;
						break;
				case    48:
				case    192:
						desKeyLen = con192BitsDesKey;
						break;
				default:
					UnionUserErrLog("in UnionDealServiceCode0H03:: keyLen[%d][%s] is error!\n",strlen(keyValue),keyValue);
					return(errCodeEssc_KeyLength);
			}

			UnionSetIsUseNormalZmkType();
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					memset(tmpBuf,0,sizeof(tmpBuf));
					if ((ret = UnionHsmCmdBU(i,conZMK,desKeyLen,keyValue,tmpBuf)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0H03:: UnionHsmCmdBU!\n");
						return(ret);
					}
					if (strcmp(checkValue,tmpBuf) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0H03:: %s[%s] != checkValue[%s]\n",protectCV[j],checkValue,tmpBuf);
						UnionSetResponseRemark("保护方式[%s],%s保护密钥校验值错误",protectCV[j],modeID);
						return(errCodeEssc_CheckValue);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode0H03:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			}
		}

		UnionSetResponseRemark("非法的加密机指令类型");
		return(errCodeParameter);
	}

*/
	return(0);
}

static int getCheckValueByHsmIP(char *ipAddr,int port,int lenOfCommHead,int lenOfMsgHead,char *checkValue)
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
			UnionUserErrLog("in getCheckValueByHsmIP:: UnionGetHostByName[%s]!\n", ip);
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
		UnionUserErrLog("in getCheckValueByHsmIP:: UnionCreateSocketClient[%s][%d]!\n",ipAddr,port);
		return(hsmSck);
	}

	UnionLog("in getCheckValueByHsmIP:: req[%s:%d][%04d][%s]!\n",ipAddr,port,len-lenOfCommHead-lenOfMsgHead,reqBuf+lenOfCommHead+lenOfMsgHead);
	if ((ret = UnionSendToSocket(hsmSck,reqBuf,len)) < 0)
	{
		UnionUserErrLog("in getCheckValueByHsmIP:: UnionSendToSocket[%s][%d]!\n",ipAddr,port);
		UnionCloseSocket(hsmSck);
		return(ret);
	}
	
	if ((ret = UnionReceiveFromSocketUntilLen(hsmSck,resBuf,lenOfCommHead)) != lenOfCommHead)
	{
		UnionUserErrLog("in getCheckValueByHsmIP:: UnionReceiveFromSocketUntilLen!\n");
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
		UnionUserErrLog("in getCheckValueByHsmIP:: UnionReceiveFromSocketUntilLen ret[%d] len[%d]!\n",ret,len);
		UnionCloseSocket(hsmSck);
		return(errCodeSocketMDL_ReceiveLen);
	}
	resBuf[len] = 0;

	UnionLog("in getCheckValueByHsmIP:: res[%s:%d][%04d][%s]!\n",ipAddr,port,len-lenOfMsgHead,resBuf+lenOfMsgHead);
	UnionCloseSocket(hsmSck);
	memcpy(checkValue,resBuf+lenOfMsgHead+4,16);
	checkValue[16] = 0;
	return 0;
}

// add by leipp 20151104
static int setHsmGroupResCmdOfTestByHsm(char *ipAddr,int port,char *hsmGroupID)
{
	int	ret = 0;
	int	i = 0,j = 0;
	int	len = 0;
	char	sql[128];
	int	lenOfMsgHead = 0;
	int	lenOfCommHead = 0;
	char	tmpBuf[128];
	char	checkValue[32];

	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;

	len = sprintf(sql,"select lenOfCommHead,lenOfMsgHead from hsmGroup where hsmGroupID = '%s' and reqCmdOfTest = 'NC'",hsmGroupID);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: UnionLocateXMLPackage [%s]!\n","detail");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("lenOfCommHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
			return(ret);
		}
		tmpBuf[ret] = 0;
		lenOfCommHead = atoi(tmpBuf);

		if ((ret = UnionReadXMLPackageValue("lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
			return(ret);
		}
		tmpBuf[ret] = 0;
		lenOfMsgHead = atoi(tmpBuf);

		if ((ret = getCheckValueByHsmIP(ipAddr,port,lenOfCommHead,lenOfMsgHead,checkValue)) < 0)
		{               
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: getCheckValueByHsmIP ipAddr[%s] port[%d]!\n",ipAddr,port);
			return(ret);
		}

		len = sprintf(sql,"update hsmGroup set resCmdOfTest = 'ND00%s' where hsmGroupID = '%s' and reqCmdOfTest = 'NC'",checkValue,hsmGroupID);
		sql[len] = 0;
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
		// 更新共享内存
		memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
		if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
		{
			UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: UnionInitClusterDef!\n");
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
						UnionUserErrLog("in setHsmGroupResCmdOfTestByHsm:: UnionInsertClusterSyn!\n");
						return(ret);
					}
				}
			}
		}
	}

	return 0;
}
// add by leipp 20151104 end
