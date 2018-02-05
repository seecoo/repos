//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-09-12

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <setjmp.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#include "unionREC.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionSocket.h"
#include "UnionSHA1.h"

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "remoteKeyPlatform.h"

#define defRemoteFlagOfRemoteKeyPlatform	"remote/flag"
#define defSysIDOfRemoteKeyPlatform		"remote/sysID"
#define defAppIDOfRemoteKeyPlatform		"remote/appID"

jmp_buf	gunionRemoteKeyPlatformJmpEnv;
void	UnionDealRemoteKeyPlatformResponseTimeout();

/* 
功能：	读取一个远程密钥平台记录
参数：	keyPlatformID[in]	密钥平台ID
	keyDirection[in]	密钥方向
	premoteKeyPlatform[out]	远程密钥平台信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadRemoteKeyPlatformRec(char *keyPlatformID,TUnionRemoteKeyPlatformKeyDirection keyDirection,PUnionRemoteKeyPlatform premoteKeyPlatform)
{
	int		ret;
	char		sql[1024];
	char		tmpBuf[128];
	
	// 从远程密钥平台表中读取
	snprintf(sql,sizeof(sql),"select * from remoteKeyPlatform where keyPlatformID = '%s' and keyDirection = %d",keyPlatformID,keyDirection);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: keyPlatformID[%s] not find!\n",keyPlatformID);
		UnionSetResponseRemark("密钥平台ID[%s]不存在",keyPlatformID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	UnionLocateXMLPackage("detail", 1);
	
	snprintf(premoteKeyPlatform->keyPlatformID,sizeof(premoteKeyPlatform->keyPlatformID),"%s",keyPlatformID);
	premoteKeyPlatform->keyDirection = keyDirection;
	
	if ((ret = UnionReadXMLPackageValue("keyPlatformName", premoteKeyPlatform->keyPlatformName, sizeof(premoteKeyPlatform->keyPlatformName))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","keyPlatformName");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","status");
		return(ret);
	}
	premoteKeyPlatform->status = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","algorithmID");
		return(ret);
	}
	premoteKeyPlatform->algorithmID = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("type", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","type");
		return(ret);
	}
	premoteKeyPlatform->type = atoi(tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("protectKey", premoteKeyPlatform->protectKey, sizeof(premoteKeyPlatform->protectKey))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","protectKey");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("checkValue", premoteKeyPlatform->checkValue, sizeof(premoteKeyPlatform->checkValue))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","checkValue");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("ipAddr", premoteKeyPlatform->ipAddr, sizeof(premoteKeyPlatform->ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("port", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","port");
		return(ret);
	}
	premoteKeyPlatform->port = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("timeout", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","timeout");
		return(ret);
	}
	premoteKeyPlatform->timeout = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("packageType", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","packageType");
		return(ret);
	}
	premoteKeyPlatform->packageType = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("permitSysID", premoteKeyPlatform->permitSysID, sizeof(premoteKeyPlatform->permitSysID))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","permitSysID");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("permitAppID", premoteKeyPlatform->permitAppID, sizeof(premoteKeyPlatform->permitAppID))) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","permitAppID");
		return(ret);
	}
	return(0);
}

/* 
功能：	初始化远程密钥操作请求报文
参数：	premoteKeyPlatform[in]	远程密钥平台信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionInitRemoteRequestPackageToRemoteKeyPlatform(PUnionRemoteKeyPlatform premoteKeyPlatform)
{
	int	ret;
	char	buf[81920];
	char	hostname[128];
	char	ipAddr[64];
	PUnionXMLPackageHead	pxmlPackageHead;

	/*
	if (UnionGetPackageType() == PACKAGE_TYPE_V001)
	{
		if ((ret = UnionInitRequestRemoteXMLPackage(NULL,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionInitRequestRemoteXMLPackage!\n");
			return(ret);
		}
	}
	else if (UnionGetPackageType() == PACKAGE_TYPE_XML)
	{
	*/
		if ((ret = UnionRequestXMLPackageToBuf(buf,sizeof(buf))) < 0)
		{
			UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionRequestXMLPackageToBuf!\n");
			return(ret);
		}
		buf[ret] = 0;
		
		if ((ret = UnionInitRequestRemoteXMLPackage(NULL,buf,ret)) < 0)
		{
			UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionInitRequestRemoteXMLPackage[%d][%s]!\n",ret,buf);
			return(ret);
		}
	//}
	
	if ((pxmlPackageHead = UnionGetXMLPackageHead()) == NULL)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionGetXMLPackageHead!\n");
		return(ret);
	}

	// 设置远程标识
	if ((ret = UnionSetRequestRemoteXMLPackageValue(defRemoteFlagOfRemoteKeyPlatform,"1")) < 0)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n",defRemoteFlagOfRemoteKeyPlatform,"1");
		return(ret);
	}

	// 设置服务码
	/*
	if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode",pxmlPackageHead->serviceCode)) < 0)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","head/serviceCode");
		return(ret);	
	}
	*/
	
	// 设置原始系统ID
	if ((ret = UnionSetRequestRemoteXMLPackageValue(defSysIDOfRemoteKeyPlatform,pxmlPackageHead->sysID)) < 0)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n",defSysIDOfRemoteKeyPlatform,pxmlPackageHead->sysID);
		return(ret);
	}

	// 设置原始应用ID
	if (pxmlPackageHead->isUI)
	{
		if ((ret = UnionSetRequestRemoteXMLPackageValue(defAppIDOfRemoteKeyPlatform,pxmlPackageHead->userID)) < 0)
		{
			UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n",defAppIDOfRemoteKeyPlatform,pxmlPackageHead->userID);
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionSetRequestRemoteXMLPackageValue(defAppIDOfRemoteKeyPlatform,pxmlPackageHead->appID)) < 0)
		{
			UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n",defAppIDOfRemoteKeyPlatform,pxmlPackageHead->appID);
			return(ret);
		}
	}
	// 设置系统ID
	if ((ret = UnionSetRequestRemoteXMLPackageValue("head/sysID",premoteKeyPlatform->permitSysID)) < 0)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/sysID",premoteKeyPlatform->permitSysID);
		return(ret);
	}
	// 设置应用ID
	if ((ret = UnionSetRequestRemoteXMLPackageValue("head/appID",premoteKeyPlatform->permitAppID)) < 0)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/sysID",premoteKeyPlatform->permitAppID);
		return(ret);
	}
	
	memset(hostname,0,sizeof(hostname));
	gethostname(hostname, sizeof(hostname));
	memset(ipAddr,0,sizeof(ipAddr));
	UnionGetHostByName(hostname,ipAddr);
	// 设置IP地址
	if ((ret = UnionSetRequestRemoteXMLPackageValue("head/clientIPAddr",ipAddr)) < 0)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/clientIPAddr",ipAddr);
		return(ret);
	}
	
	// 设置交易时间
	/*
	if ((ret = UnionSetRequestRemoteXMLPackageValue("head/transTime",pxmlPackageHead->transTime)) < 0)
	{
		UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/transTime",pxmlPackageHead->transTime);
		return(ret);
	}
	
	// 设置用户信息
	if (strlen(pxmlPackageHead->userInfo) > 0)
	{
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/userInfo",pxmlPackageHead->userInfo)) < 0)
		{
			UnionUserErrLog("in UnionInitRemoteRequestPackageToRemoteKeyPlatform:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/userInfo",pxmlPackageHead->userInfo);
			return(ret);
		}
	}
	*/
	return(0);
}
/* 
功能：	转发密钥操作到远程平台
参数：	premoteKeyPlatform[in]	远程密钥平台信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionTransferKeyOperateToRemoteKeyPlatform(PUnionRemoteKeyPlatform premoteKeyPlatform)
{
	int		ret = 0;
	int		len = 0;
	int		socketHDL = -1;
	unsigned char	lenBuf[32];
	unsigned char	buf[81920];
	char		responseCode[32];
	char		responseRemark[128];
	char		serviceCode[32];
	//unsigned char	tmpBuf[128+1];
	//unsigned char	hash[40+1];
	
	if (premoteKeyPlatform == NULL)
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: premoteKeyPlatform is null!\n");
		return(errCodeParameter);
	}
	
	UnionNullLog("***** START 远程密钥平台[%s:%s][%s:%d] *****\n",premoteKeyPlatform->keyPlatformID,premoteKeyPlatform->keyPlatformName,premoteKeyPlatform->ipAddr,premoteKeyPlatform->port);
	
	if (premoteKeyPlatform->packageType == 3)
	{
		// 设置3.x报文
		if ((ret = UnionInitEssc3RemoteRequestPackage(premoteKeyPlatform->permitAppID,(char *)buf+2,serviceCode)) < 0)
		{
			UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionInitEssc3RemoteRequestPackage!\n");
			goto remoteEnd;
		}
		UnionNullLog("****3.x远程请求报文: Request Pacakge[%s]!\n",buf+2);
	}
	else if (premoteKeyPlatform->packageType == 5)
	{
		UnionLogRequestRemoteXMLPackage();
		if ((ret = UnionRequestRemoteXMLPackageToBuf((char *)buf+2,sizeof(buf)-2)) < 0)
		{
			UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionRequestRemoteXMLPackageToBuf!\n");
			goto remoteEnd;
		}
	}
	else
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: packageType[%d] is invalid!\n",premoteKeyPlatform->packageType);
		goto remoteEnd;
	}

	buf[0] = ret / 256;
	buf[1] = ret % 256;
	len = ret + 2;
	buf[len] = 0;
	
	if ((socketHDL = UnionCreateSocketClient(premoteKeyPlatform->ipAddr,premoteKeyPlatform->port)) < 0)
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionCreateSocketClient[%s][%d]!\n",premoteKeyPlatform->ipAddr,premoteKeyPlatform->port);
		ret = errCodeUseOSErrCode;
		goto remoteEnd;
	}

	alarm(0);
#ifdef _LINUX_
	if (sigsetjmp(gunionRemoteKeyPlatformJmpEnv,1) != 0)
#else
	if (setjmp(gunionRemoteKeyPlatformJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: Timeout!\n");
		UnionSetResponseRemark("远程密钥平台[%s],地址[%s:%d]响应超时",premoteKeyPlatform->keyPlatformID,premoteKeyPlatform->ipAddr,premoteKeyPlatform->port);
		ret = errCodeSocketMDL_Timeout;
		goto remoteEnd;
	}
	alarm(premoteKeyPlatform->timeout);
	signal(SIGALRM,UnionDealRemoteKeyPlatformResponseTimeout);

	if ((ret = UnionSendToSocket(socketHDL,buf,len)) < 0)
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionSendToSocket buf[%d][%s]!\n",len,buf);
		goto remoteEnd;
	}
	
	if ((ret = UnionReceiveFromSocketUntilLen(socketHDL,lenBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionReceiveFromSocketUntilLen!\n");
		goto remoteEnd;
	}
	len = lenBuf[0] * 256 + lenBuf[1];
	
	if (len >= sizeof(buf))
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionReceiveFromSocketUntilLen len [%d] larger than expected [%d]!\n",len,(int)sizeof(buf));
		ret = errCodeSmallBuffer;
		goto remoteEnd;
	}

	if ((ret = UnionReceiveFromSocketUntilLen(socketHDL,buf,len)) != len)
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionReceiveFromSocketUntilLen!\n");
		ret = errCodeSocketMDL_DataLen;
		goto remoteEnd;
	}
	buf[ret] = 0;

	if (premoteKeyPlatform->packageType == 3)
	{
		// 读3.x响应报文,并设置
		UnionNullLog("****3.x远程响应报文: Response Pacakge[%s]!\n",buf);
		if ((ret = UnionReadEssc3RemoteResponsePackage(premoteKeyPlatform->permitAppID,(char *)buf,serviceCode)) < 0)
		{
			UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionReadEssc3RemoteResponsePackage!\n");
		}
		goto remoteEnd;
	}

	if ((ret = UnionInitResponseRemoteXMLPackage(NULL,(char *)buf,ret)) < 0)
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionInitResponseRemoteXMLPackage[%d][%s]!\n",ret,buf);
		goto remoteEnd;
	}
	UnionLogResponseRemoteXMLPackage();
	
	// 读取响应码
	if ((ret = UnionReadResponseRemoteXMLPackageValue("head/responseCode",responseCode,sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","head/responseCode");
		goto remoteEnd;
	}

	if (strcmp(responseCode,"000000") != 0) 	// 响应非成功
	{
		if ((ret = UnionReadResponseRemoteXMLPackageValue("head/responseRemark",responseRemark,sizeof(responseRemark))) < 0)
		{
			UnionUserErrLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","head/responseRemark");
			return(ret);
		}
		UnionSetResponseRemark("远程::%s",responseRemark);
		ret = 0 - atoi(responseCode);
		goto remoteEnd;
	}
remoteEnd:
	if (socketHDL > 0)
		UnionCloseSocket(socketHDL);
	if (premoteKeyPlatform->keyDirection == conRemoteKeyPlatformKeyDirectionOfDistribute)	// 分发
	{
		UnionLog("in UnionTransferKeyOperateToRemoteKeyPlatform:: 分发密钥失败,不要求强制成功\n");
		ret = 0;
	}

	UnionNullLog("***** END 远程密钥平台[%s:%s][%s:%d] *****\n",premoteKeyPlatform->keyPlatformID,premoteKeyPlatform->keyPlatformName,premoteKeyPlatform->ipAddr,premoteKeyPlatform->port);
	return(ret);
}

// 是远程密钥操作
int UnionIsRemoteKeyOperate()
{
	int	ret;
	char	tmpBuf[32];
	
	if ((ret = UnionReadRequestXMLPackageValue(defRemoteFlagOfRemoteKeyPlatform, tmpBuf, sizeof(tmpBuf))) <= 0)
		return(0);
	
	if (tmpBuf[0] == '1')
		return(1);
	else
		return(0);
}

// 读取远程系统编号和应用编号
int UnionReadRemoteSysIDAndAppID(char *sysID,int sizeofSysID,char *appID,int sizeofAppID)
{
	int	ret;
	
	if ((ret = UnionReadRequestXMLPackageValue(defSysIDOfRemoteKeyPlatform,sysID,sizeofSysID)) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteSysIDAndAppID:: UnionReadRequestXMLPackageValue[%s]!\n",defSysIDOfRemoteKeyPlatform);
		return(ret);	
	}
	
	if ((ret = UnionReadRequestXMLPackageValue(defAppIDOfRemoteKeyPlatform,appID,sizeofAppID)) < 0)
	{
		UnionUserErrLog("in UnionReadRemoteSysIDAndAppID:: UnionReadRequestXMLPackageValue[%s]!\n",defAppIDOfRemoteKeyPlatform);
		return(ret);	
	}
	return(0);
}

void UnionDealRemoteKeyPlatformResponseTimeout()
{
	UnionUserErrLog("in UnionDealRemoteKeyPlatformResponseTimeout:: remote key platform response time out!\n");
#ifdef _LINUX_
	siglongjmp(gunionRemoteKeyPlatformJmpEnv,10);
#else
	longjmp(gunionRemoteKeyPlatformJmpEnv,10);
#endif
}

int UnionInitEssc3RemoteRequestPackage(char *appID,char *buf,char *serviceCode)
{
	int	ret = 0;
	int	len = 0;
	char	keyName[128];
	char	zmkName[128];
	char	keyValue[64];
	char	checkValue[32];

	if ((ret = UnionReadRequestRemoteXMLPackageValue("head/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionInitEssc3RemoteRequestPackage:: UnionReadRequestRemoteXMLPackageValue[head/serviceCode]!\n");
		return(ret);
	}

	if ((ret = UnionReadRequestRemoteXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionInitEssc3RemoteRequestPackage:: UnionReadRequestRemoteXMLPackageValue [keyName]!\n");
		return(ret);
	}
	
	snprintf(zmkName,sizeof(zmkName),"%.*s.zmk",(int)strlen(keyName)-4,keyName);

	if (strcmp(serviceCode,"293") == 0 || strcmp(serviceCode,"294") == 0)
	{
		if ((ret = UnionReadRequestRemoteXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionInitEssc3RemoteRequestPackage:: UnionReadRequestRemoteXMLPackageValue [keyValue]!\n");
			return(ret);
		}
		keyValue[ret] = 0;

		if ((ret = UnionReadRequestRemoteXMLPackageValue("body/checkValue",checkValue,sizeof(checkValue))) < 0)
		{
			UnionUserErrLog("in UnionInitEssc3RemoteRequestPackage:: UnionReadRequestRemoteXMLPackageValue [checkValue]!\n");
			return(ret);
		}
		checkValue[ret] = 0;
	}

	if (strcmp(serviceCode,"291") == 0)
		len = sprintf(buf,"%s2911002001%04d%s011%04d%s",appID,(int)strlen(keyName),keyName,(int)strlen(zmkName),zmkName);
	else if (strcmp(serviceCode,"292") == 0)
		len = sprintf(buf,"%s2921002001%04d%s011%04d%s",appID,(int)strlen(keyName),keyName,(int)strlen(zmkName),zmkName);
	else if (strcmp(serviceCode,"293") == 0)
		len = sprintf(buf,"%s2931004001%04d%s011%04d%s061%04d%s051%04d%s",appID,(int)strlen(keyName),keyName,(int)strlen(zmkName),zmkName,(int)strlen(keyValue),keyValue,(int)strlen(checkValue),checkValue);
	else if (strcmp(serviceCode,"294") == 0)
		len = sprintf(buf,"%s2941004001%04d%s011%04d%s061%04d%s051%04d%s",appID,(int)strlen(keyName),keyName,(int)strlen(zmkName),zmkName,(int)strlen(keyValue),keyValue,(int)strlen(checkValue),checkValue);
	else
	{
		UnionUserErrLog("in UnionInitEssc3RemoteRequestPackage:: serviceCode[%s] is invalid!\n",serviceCode);
		return (errCodeParameter);
	}
	buf[len+2] = 0;
	return(len);
}

int UnionReadEssc3RemoteResponsePackage(char *appID,char *buf,char *serviceCode)
{
	int		ret = 0;
	char		tmpBuf[32];
	char		keyValue[64];
	char		checkValue[32];
	int		flagNum = 0;
	int		offset = 0;
	char		responseCode[32];
	int		i = 0;

	if (strlen(buf) < 12)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Read Response Package is failed!  Recv buf[%s]!\n",buf);
		return(errCodeEsscMDL_NotEsscResponsePackage);
	}

	if (memcmp(appID,buf,2) != 0)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Right appID[%s], Recv appID[%.2s] is error!  Recv buf[%s]!\n",appID,buf,buf);
		return(errCodeEsscMDL_ReqAndResNotIsIndentified);
	}

	offset = 2;

	if (memcmp(serviceCode,buf+offset,3) != 0)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Right serviceCode[%s],Recv serviceCode[%.3s] is error!  Recv buf[%s]!\n",serviceCode,buf+offset,buf);
		return(errCodeEsscMDL_ReqAndResNotIsIndentified);
	}
	offset += 3;
	
	if (memcmp("0",buf+offset,1) != 0)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Right responseFlag[0],Recv responseFlag[%.1s] is error!  Recv buf[%s]!\n",buf+offset,buf);
		return(errCodeEsscMDL_NotEsscResponsePackage);
	}
	offset += 1;

	memcpy(responseCode,buf+offset,6);
	responseCode[6] = 0;
	if (strcmp(responseCode,"000000") != 0)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Right responseCode[000000],Recv responseFlag[%s] is error!  Recv buf[%s]!\n",responseCode,buf);
		if (atoi(responseCode) < 0)
			return(atoi(responseCode));
		else
			return(errCodeEsscMDL_ResponseCodeNotSuccess);
	}
	offset += 6;

	if ((ret = UnionInitResponseRemoteXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: UnionInitResponseRemoteXMLPackage!\n");
		return(ret);
	}

	if (strcmp(serviceCode,"291") == 0 || strcmp(serviceCode,"292") == 0)
	{
		if (strlen(buf+offset) <= 6)
		{
			UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Recv buf[%s]!\n",buf);
			return(errCodeEsscMDL_ResponseToShort);
		}

		memcpy(tmpBuf,buf+offset,3);	
		tmpBuf[3] = 0;
		flagNum = atoi(tmpBuf);
		offset += 3;
		for (i = 0; i < flagNum; i++)
		{
				// 读取密钥值
			if (memcmp("061",buf+offset,3) == 0)
			{
				if ((ret = UnionReadEssc3PackageValueByField(buf+offset,keyValue,sizeof(keyValue))) < 0)
				{
					UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: UnionReadEssc3PackageValueByField  read keyValue is failed!\n");
					return(ret);
				}
				offset += ret;
				keyValue[ret] = 0;

				if ((ret = UnionSetResponseRemoteXMLPackageValue("body/keyValue",keyValue)) < 0)
				{
					UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: UnionSetRequestRemoteXMLPackageValue[body/keyValue]!\n");
					return(ret);
				}
			}	// 读取校验值
			else if (memcmp("051",buf+offset,3) == 0)
			{
				if ((ret = UnionReadEssc3PackageValueByField(buf+offset,checkValue,sizeof(checkValue))) < 0)
				{
					UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: UnionReadEssc3PackageValueByField  read checkValue is failed!\n");
					return(ret);
				}
				offset += ret;
				checkValue[ret] = 0;
				
				if ((ret = UnionSetResponseRemoteXMLPackageValue("body/checkValue",checkValue)) < 0)
				{
					UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: UnionSetRequestRemoteXMLPackageValue[body/checkValue]!\n");
					return(ret);
				}
			}
			else
			{
				UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Right flagValue[061 or 051],Recv falgValue[%.3s] is error!  Recv buf[%s]!\n",buf+offset,buf);
				return(errCodeParameter);
			}
		}
	}

	return 0;
}

int UnionReadEssc3PackageValueByField(char *buf,char *value,int sizeofValue)
{
	int	offset = 0;
	char	tmpBuf[32];
	int	len = 0;

	offset += 3;

	if (strlen(buf+offset) <= 4)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Read lenOfValue is failed! Recv buf[%s]!\n",buf);
		return(errCodeEsscMDL_ResponseToShort);
	}
	memcpy(tmpBuf,buf+offset,4);
	tmpBuf[4] = 0;
	offset += 4;
	len = atoi(tmpBuf);

	if (strlen(buf+offset) < len)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: Read value is failed! Recv buf[%s]!\n",buf);
		return(errCodeEsscMDL_ResponseToShort);
	}

	if (len >= sizeofValue)
	{
		UnionUserErrLog("in UnionReadEssc3RemoteResponsePackage:: value len[%d] >= sizeofValue[%d]!\n",len,sizeofValue);
		return(errCodeSmallBuffer);
	}

	memcpy(value,buf+offset,len);
	value[len] = 0;
	offset += len;

	return (offset);
}

/* 
功能：	检查远程密钥平台
参数：	keyName[in]			密钥名称
	keyApplyPlatform[in]		密钥申请平台
	keyDistributePlatform[in]	密钥分发平台
	pkeyApplyPlatform[out]		远程密钥申请平台信息
	pkeyDistributePlatform[out]	远程密钥分发平台信息
	isRemoteApplyKey[out]		是远程申请密钥
	isRemoteDistributeKey[out]	是远程分发密钥
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckRemoteKeyPlatform(char *keyName,char *keyApplyPlatform, char *keyDistributePlatform,PUnionRemoteKeyPlatform pkeyApplyPlatform,PUnionRemoteKeyPlatform pkeyDistributePlatform,int *isRemoteApplyKey,int *isRemoteDistributeKey)
{
	int	ret;
	char	localIP[40];

	if (keyName == NULL)
	{
		UnionUserErrLog("in UnionCheckRemoteKeyPlatform:: keyName is null!\n");
		return(errCodeParameter);
	}
	
	if (isRemoteApplyKey != NULL)
		*isRemoteApplyKey = 0;
		
	// 密钥申请平台
	if ((keyApplyPlatform != NULL) && (strlen(keyApplyPlatform) > 0) && (pkeyApplyPlatform != NULL) && (isRemoteApplyKey != NULL))
	{
		if ((ret =  UnionReadRemoteKeyPlatformRec(keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,pkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionCheckRemoteKeyPlatform:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",keyApplyPlatform);
			return(ret);
		}
		*isRemoteApplyKey = pkeyApplyPlatform->status;

		if (isRemoteApplyKey)
		{
			// 获取本地ip地址
			if ((ret = UnionGetLocalIpAddr(pkeyApplyPlatform->ipAddr,pkeyApplyPlatform->port,localIP,sizeof(localIP))) >= 0)
			{
				// 判断远程申请平台是否是本地平台,避免配置申请平台为自身而进入死循环
				if ((strcmp(pkeyApplyPlatform->ipAddr, localIP) == 0) || (strcmp(pkeyApplyPlatform->ipAddr, "127.0.0.1")) == 0)
					isRemoteApplyKey = 0;
			}
		}
	}
	
	if (isRemoteDistributeKey != NULL)
		*isRemoteDistributeKey = 0;

	// 密钥分发平台
	if ((keyDistributePlatform != NULL) && (strlen(keyDistributePlatform) > 0) && (pkeyDistributePlatform != NULL) && (isRemoteDistributeKey != NULL))
	{
		if ((ret =  UnionReadRemoteKeyPlatformRec(keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,pkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionCheckRemoteKeyPlatform:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",keyDistributePlatform);
			return(ret);
		}
		*isRemoteDistributeKey = pkeyDistributePlatform->status;

		if (isRemoteDistributeKey)
		{
			// 获取本地ip地址
			if ((ret = UnionGetLocalIpAddr(pkeyDistributePlatform->ipAddr,pkeyDistributePlatform->port,localIP,sizeof(localIP))) >= 0)
			{
				// 判断远程分发平台是否是本地平台,避免配置分发平台为自身而进入死循环
				if ((strcmp(pkeyDistributePlatform->ipAddr, localIP) == 0) || (strcmp(pkeyDistributePlatform->ipAddr, "127.0.0.1")) == 0)
					isRemoteDistributeKey = 0;
			}
		}
	}
	
	if ((isRemoteApplyKey != NULL) && (isRemoteDistributeKey != NULL) &&
		(*isRemoteApplyKey && *isRemoteDistributeKey))
	{
		if ((strcmp(pkeyApplyPlatform->keyPlatformID,pkeyDistributePlatform->keyPlatformID) == 0) ||
			((strcmp(pkeyApplyPlatform->ipAddr,pkeyDistributePlatform->ipAddr) == 0) &&
			(pkeyApplyPlatform->port == pkeyDistributePlatform->port)))
		{
			UnionUserErrLog("in UnionCheckRemoteKeyPlatform:: keyName[%s] applyPlatform[%s:%s:%d] == distributePlatform[%s:%s|%d]!\n",
				keyName,
				pkeyApplyPlatform->keyPlatformID,pkeyApplyPlatform->ipAddr,pkeyApplyPlatform->port,
				pkeyDistributePlatform->keyPlatformID,pkeyDistributePlatform->ipAddr,pkeyDistributePlatform->port);
			UnionSetResponseRemark("密钥[%s]的申请平台和分发平台不能相同",keyName);
			return(errCodeParameter);
		}
	}	

	return(0);
}

