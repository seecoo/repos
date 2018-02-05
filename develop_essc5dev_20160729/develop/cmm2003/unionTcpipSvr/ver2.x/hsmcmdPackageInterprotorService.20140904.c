//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-10-31

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionStr.h"
#include "UnionLog.h"
#include "unionCommBetweenMDL.h"
#include "unionRealBaseDB.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"
#include "unionXMLPackage.h"
#include "unionTableData.h"
#include "accessCheck.h"

int		gunionLenOfMsgHeader = -1;

typedef struct
{
	char		hsmGroupID[8+1];
	char		hsmGroupName[40+1];
	unsigned int	hsmCmdVersionID;
	unsigned int	lenOfCommHead;
	unsigned int	lenOfMsgHead;
	char		reqCmdOfTest[128+1];
	char		resCmdOfTest[128+1];
	char		sensitiveCmd[512+1];
} TUnionHsmGroup;
typedef TUnionHsmGroup	*PUnionHsmGroup;

TUnionHsmGroup	gunionHsmGroupRec;

int UnionReadHsmGroupRecFromHsmGroupID(char *hsmGroupID,PUnionHsmGroup pHsmGroup)
{
	int	ret;
	char	sql[1024];
	char	tmpBuf[128];
	char	*precStr = NULL;
	
	if ((precStr = UnionFindTableValue("hsmGroup", hsmGroupID)) == NULL)
	{
		snprintf(sql,sizeof(sql),"select hsmGroupName,hsmCmdVersionID,lenOfCommHead,lenOfMsgHead,reqCmdOfTest,resCmdOfTest,sensitiveCmd from hsmGroup where hsmGroupID='%s'",hsmGroupID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) <= 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		UnionLocateXMLPackage("detail", 1);

		// 读取密码机组名
		if ((ret = UnionReadXMLPackageValue("hsmGroupName", pHsmGroup->hsmGroupName, sizeof(pHsmGroup->hsmGroupName))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","hsmGroupName");
			return(ret);
		}
		// 读取密码机指令类型
		if ((ret = UnionReadXMLPackageValue("hsmCmdVersionID", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","hsmCmdVersionID");
			return(ret);
		}
		pHsmGroup->hsmCmdVersionID = atoi(tmpBuf);
		// 读取通讯头长度
		if ((ret = UnionReadXMLPackageValue("lenOfCommHead", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
			return(ret);
		}
		pHsmGroup->lenOfCommHead = atoi(tmpBuf);
		// 读取消息头长度
		if ((ret = UnionReadXMLPackageValue("lenOfMsgHead", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","lenOfMsgHead");
			return(ret);
		}
		pHsmGroup->lenOfMsgHead = atoi(tmpBuf);
		// 读取探测请求指令
		if ((ret = UnionReadXMLPackageValue("reqCmdOfTest", pHsmGroup->reqCmdOfTest, sizeof(pHsmGroup->reqCmdOfTest))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","reqCmdOfTest");
			return(ret);
		}
		// 读取探测响应指令
		if ((ret = UnionReadXMLPackageValue("resCmdOfTest", pHsmGroup->resCmdOfTest, sizeof(pHsmGroup->resCmdOfTest))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","resCmdOfTest");
			return(ret);
		}
		// 读取敏感指令
		if ((ret = UnionReadXMLPackageValue("sensitiveCmd", pHsmGroup->sensitiveCmd, sizeof(pHsmGroup->sensitiveCmd))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","sensitiveCmd");
			return(ret);
		}
	}
	else
	{
		if (strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: hsmGroupID[%s] not found!\n",hsmGroupID);
			return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
		}

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"hsmGroupName",pHsmGroup->hsmGroupName,sizeof(pHsmGroup->hsmGroupName))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadRecFldFromRecStr[hsmGroupName][%s]!\n",precStr);
			return(ret);
		}
		pHsmGroup->hsmGroupName[ret] = 0;

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"hsmCmdVersionID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadRecFldFromRecStr[hsmCmdVersionID][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		pHsmGroup->hsmCmdVersionID = atoi(tmpBuf);

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"lenOfCommHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadRecFldFromRecStr[lenOfCommHead][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		pHsmGroup->lenOfCommHead = atoi(tmpBuf);

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadRecFldFromRecStr[lenOfMsgHead][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		pHsmGroup->lenOfMsgHead = atoi(tmpBuf);

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"reqCmdOfTest",pHsmGroup->reqCmdOfTest,sizeof(pHsmGroup->reqCmdOfTest))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadRecFldFromRecStr[reqCmdOfTest][%s]!\n",precStr);
			return(ret);
		}
		pHsmGroup->reqCmdOfTest[ret] = 0;

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"resCmdOfTest",pHsmGroup->resCmdOfTest,sizeof(pHsmGroup->resCmdOfTest))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadRecFldFromRecStr[resCmdOfTest][%s]!\n",precStr);
			return(ret);
		}
		pHsmGroup->resCmdOfTest[ret] = 0;

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"sensitiveCmd",pHsmGroup->sensitiveCmd,sizeof(pHsmGroup->sensitiveCmd))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupRecFromHsmGroupID:: UnionReadRecFldFromRecStr[sensitiveCmd][%s]!\n",precStr);
			return(ret);
		}
		pHsmGroup->sensitiveCmd[ret] = 0;
	}
	
	return(1);
}

// 与指定的模块进行交互
/* 输入参数：
	recvMDLID,指定的模块
	reqStr,lenOfReqStr,分别是请求和请求长度
	sizeOfBuf,是接收缓冲的大小
	timeout,是读响应的超时值
   输出参数：
	resStr,为接收到的消息
   返回值：
	>=0，消息的长度
	负值，错误码
*/
int UnionFunSvrInterprotor(char *cliIPAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr)
{
	int		ret = 0;

	if ((ret = UnionCheckExternalSystemDef(1,cliIPAddr,NULL,cliIPAddr,port,NULL)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCheckExternalSystemDef[%s:%d]!\n",cliIPAddr,port);
		
		// add begin by lusj 20151117
		snprintf((char *)resStr,11,(const char *)reqStr);
		resStr[9]=resStr[9]+1;
		sprintf((char *)resStr+10 ,"99SocketStop");
		resStr[10+12] =0;
		// add end by lusj 20151117	

		return(ret);
	}
	
	if (gunionLenOfMsgHeader < 0)
	{
		// 读取加密机组
		memset(&gunionHsmGroupRec,0,sizeof(gunionHsmGroupRec));
		if ((ret = UnionReadHsmGroupRecFromHsmGroupID(UnionGetHsmGroupIDForHsmSvr(),&gunionHsmGroupRec)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadHsmGroupRecFromHsmGroupID[%s]!\n",UnionGetHsmGroupIDForHsmSvr());
			return(ret);
		}
		if ((gunionLenOfMsgHeader = gunionHsmGroupRec.lenOfMsgHead) < 0)
			gunionLenOfMsgHeader = 0;
	}

	if (lenOfReqStr < gunionLenOfMsgHeader)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: lenOfReqStr[%d] < gunionLenOfMsgHeader[%d]!\n",lenOfReqStr,gunionLenOfMsgHeader);
		memcpy(resStr,reqStr,lenOfReqStr);
		return(lenOfReqStr);
	}
	
	memcpy(resStr,reqStr,gunionLenOfMsgHeader);

	switch(gunionHsmGroupRec.hsmCmdVersionID)
	{
		case conHsmCmdVerSJL05StandardHsmCmd:
			UnionSetLenOfHsmCmdHeader(0);
			UnionSetBCDPrintTypeForHSMCmd();
			break;
		default:
			break;
	}
	UnionSetIsNotCheckHsmResCode();
	
	ret = UnionDirectHsmCmd((char *)reqStr + gunionLenOfMsgHeader,lenOfReqStr - gunionLenOfMsgHeader,(char *)resStr + gunionLenOfMsgHeader,sizeOfResStr - gunionLenOfMsgHeader);
	if (ret < 0)
	{
		switch(gunionHsmGroupRec.hsmCmdVersionID)
		{
			case conHsmCmdVerSJL05StandardHsmCmd:
				resStr[gunionLenOfMsgHeader] = 'E';
				resStr[gunionLenOfMsgHeader+1] = 0xFF;
				return(gunionLenOfMsgHeader+2);
			default:
				resStr[gunionLenOfMsgHeader] = reqStr[gunionLenOfMsgHeader];
				resStr[gunionLenOfMsgHeader+1] = reqStr[gunionLenOfMsgHeader+1] + 1;
				sprintf((char *)resStr+gunionLenOfMsgHeader+2,"15");
				return(strlen((char *)resStr));
		}
	}
	else
		return(ret + gunionLenOfMsgHeader);
}
