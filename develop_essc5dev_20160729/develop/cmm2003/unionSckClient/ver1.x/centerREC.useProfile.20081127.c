// Author:	Wolfgang Wang
// Date:	2006/07/26

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "centerREC.h"

// 密码机的实际数据
extern int	gunionNumOfEssc;
// 客户端API使用的应用编号
extern char	gunionIDOfEsscAPI[];
// 中心的IP地址
extern char	gunionIPAddrOfEssc[][15+1];
// 本机的IP地址
extern char	gunionIPAddrOfMyself[15+1];
// 中心的端口号
extern int	gunionPortOfEssc[];
// 与中心通讯使用的超时
extern int	gunionTimeoutOfEssc;
// 日志是否打开
extern int	gunionIsDebug;
// 是否使用短连接
extern int	gunionIsShortConnUsed;
// 自动签名的类型
extern int	gunionTypeOfAutoSign;

extern int	gunionIsCenterRECConnected;

int UnionConnectCenterREC()
{
	int	ret;
	char	*ptr;
	int	index;
	char	varName[100];
	
	if (gunionIsCenterRECConnected)
		return(0);
	
	// 读DEBUG标志
	if ((ptr = getenv("openDebug")) != NULL)
		gunionIsDebug = atoi(ptr);
	
	// 读密码机的配置
	gunionNumOfEssc = 0;
	for (index = 0; (index < conMaxNumOfEssc) && (gunionNumOfEssc < conMaxNumOfEssc); index++)
	{
		sprintf(varName,"ipAddrOfCenterSecuSvr%02d",index+1);
		if ((ptr = getenv(varName)) == NULL)
			continue;
		if (!UnionIsValidIPAddrStr(ptr))
		{
			UnionUserErrLog("in UnionConnectCenterREC:: %s is not valid ipAddr!\n",ptr);
			return(errCodeEsscMDL_InvalidEsscIPAddr);
		}
		strcpy(gunionIPAddrOfEssc[gunionNumOfEssc],ptr);
		// 读中心的端口号
		sprintf(varName,"portOfCenterSecuSvr%02d",index+1);
		if ((ptr = getenv(varName)) == NULL)
		{
			gunionPortOfEssc[gunionNumOfEssc] = 8;
		}
		else
		{
			if ((gunionPortOfEssc[gunionNumOfEssc] = atoi(ptr)) <= 0)
			{
				UnionUserErrLog("in UnionConnectCenterREC:: %s is not valid port!\n",ptr);
				return(errCodeAPIEsscSvrPortInvalid);
			}
		}
		gunionNumOfEssc++;
	}
	if (gunionNumOfEssc == 0)
	{
		sprintf(varName,"ipAddrOfCenterSecuSvr");
		if ((ptr = getenv(varName)) == NULL)
		{
			UnionUserErrLog("in UnionConnectCenterREC:: ipAddrOfCenterSecuSvr not defined in .profile!\n");
			return(errCodeEsscMDL_InvalidEsscIPAddr);
		}
		if (!UnionIsValidIPAddrStr(ptr))
		{
			UnionUserErrLog("in UnionConnectCenterREC:: %s is not valid ipAddr!\n",ptr);
			return(errCodeEsscMDL_InvalidEsscIPAddr);
		}
		strcpy(gunionIPAddrOfEssc[gunionNumOfEssc],ptr);
		// 读中心的端口号
		sprintf(varName,"portOfCenterSecuSvr");
		if ((ptr = getenv(varName)) == NULL)
		{
			gunionPortOfEssc[gunionNumOfEssc] = 8;
		}
		else
		{
			if ((gunionPortOfEssc[gunionNumOfEssc] = atoi(ptr)) <= 0)
			{
				UnionUserErrLog("in UnionConnectCenterREC:: %s is not valid port!\n",ptr);
				return(errCodeAPIEsscSvrPortInvalid);
			}
		}
		gunionNumOfEssc++;
	}

	// 读API使用的应用编号
	if ((ptr = getenv("idOfEsscAPI")) != NULL)
	{
		if (strlen(ptr) >= 10)
		{
			UnionUserErrLog("in UnionConnectCenterREC:: %s is not valid idOfEsscAPI!\n",ptr);
			return(errCodeAPIEsscSvrIDOfAppApiInvalid);
		}
		else
			strcpy(gunionIDOfEsscAPI,ptr);
	}
	
	// 读本机的ip地址
	if ((ptr = getenv("ipAddrOfMyself")) != NULL)
	{
		if (!UnionIsValidIPAddrStr(ptr) >= 10)
		{
			UnionUserErrLog("in UnionConnectCenterREC:: %s is not valid ipAddr!\n",ptr);
			return(errCodeInvalidIPAddr);
		}
		else
			strcpy(gunionIPAddrOfMyself,ptr);
	}
	
	// 读与中心通讯的超时值
	if ((ptr = getenv("timeoutOfCenterSecuSvr")) != NULL)
	{
		if ((gunionTimeoutOfEssc = atoi(ptr)) <= 0)
		{
			UnionUserErrLog("in UnionConnectCenterREC:: %s is not timeout!\n",ptr);
			return(errCodeAPIEsscSvrTimeoutInvalid);
		}
	}

	// 读是否使用短连接
	if ((ptr = getenv("isShortConnUsed")) != NULL)
		gunionIsShortConnUsed = atoi(ptr);

	// 读使用的自动签名类型
	if ((ptr = getenv("typeOfAutoSign")) != NULL)
		gunionTypeOfAutoSign = atoi(ptr);
	if (gunionTypeOfAutoSign < 0)
		gunionTypeOfAutoSign = 100;

	gunionIsCenterRECConnected = 1;
	return(0);
}
