#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "unionXMLPackage.h"
#include "registerRunningTime.h"

// 交易处理阶段，0交易处理前，1交易处理中，2交易处理后
int	gunionStageOfTransProcessing = defStageOfTransing;
int	gunionRegisterNodeID = 1;

// 设置交易处理阶段
void UnionSetStageOfTransProcessing(int stage)
{
	gunionStageOfTransProcessing = stage;
	return;
}

// 初始化登记节点ID
void UnionInitRegisterNodeID()
{
	gunionRegisterNodeID = 1;
	return;
}

// 登记节点运行时间
void UnionRegisterRunningTime(char *nodeName)
{
	unsigned long times;
	char	uTime[32];;
	char	fieldName[64];
	
	if (UnionReadIntTypeRECVar("isRegisterRunningTime") <= 0)
		return;
	
	if ((nodeName == NULL) || (strlen(nodeName) == 0))
	{
		if (gunionStageOfTransProcessing == defStageOfTransBefore)
			snprintf(fieldName,sizeof(fieldName),"time/B%02d",gunionRegisterNodeID++);
		else if (gunionStageOfTransProcessing == defStageOfTransAfter)
			snprintf(fieldName,sizeof(fieldName),"time/A%02d",gunionRegisterNodeID++);
		else
			snprintf(fieldName,sizeof(fieldName),"time/P%02d",gunionRegisterNodeID++);
	}
	else
		snprintf(fieldName,sizeof(fieldName),"time/%s",nodeName);
	
	// 计算使用时间
	times = UnionGetRunningTimeOffsetInMacroSeconds();
	snprintf(uTime,sizeof(uTime),"%ld",times);
	
	if (gunionStageOfTransProcessing == defStageOfTransBefore)
	{
		UnionLocateRequestXMLPackage("",0);
		UnionSetRequestXMLPackageValue(fieldName,uTime);
	}
	else
	{
		UnionLocateResponseXMLPackage("",0);
		UnionSetResponseXMLPackageValue(fieldName,uTime);
	}
	return;
}

// 根据请求报文，设置时间
void UnionSetTimeByRequestPackage()
{
	int	i;
	int	ret;
	char	tmpBuf[32];
	char	fieldName[64];
	struct timeval	userTime;
	
	if (UnionReadIntTypeRECVar("isRegisterRunningTime") <= 0)
		return;

	if ((ret = UnionReadRequestXMLPackageValue("time/tv_sec",tmpBuf,sizeof(tmpBuf))) < 0)
		userTime.tv_sec = 0;
	else
		userTime.tv_sec = atol(tmpBuf);

	if ((ret = UnionReadRequestXMLPackageValue("time/tv_usec",tmpBuf,sizeof(tmpBuf))) < 0)
		userTime.tv_usec = 0;
	else
		userTime.tv_usec = atol(tmpBuf);
	
	for (i = 1; ;i++)
	{
		snprintf(fieldName,sizeof(fieldName),"time/B%02d",i);
		if (UnionReadRequestXMLPackageValue(fieldName,tmpBuf,sizeof(tmpBuf)) < 0)
			break;
		UnionSetResponseXMLPackageValue(fieldName,tmpBuf);
	}

	// 设置当前时间
	UnionSetUserSetTime(&userTime);	
	
	gunionRegisterNodeID = 1;
	
	return;
}

// 设置时间到请求报文
void UnionSetTimeToRequestPackage()
{
	char	tmpBuf[32];
	struct timeval	userTime;

	if (UnionReadIntTypeRECVar("isRegisterRunningTime") <= 0)
		return;

	// 读取当前设置时间
	UnionGetUserSetTime(&userTime);
	
	snprintf(tmpBuf,sizeof(tmpBuf),"%ld",userTime.tv_sec);
	UnionSetRequestXMLPackageValue("time/tv_sec",tmpBuf);
	snprintf(tmpBuf,sizeof(tmpBuf),"%ld",userTime.tv_usec);
	UnionSetRequestXMLPackageValue("time/tv_usec",tmpBuf);
	
	gunionRegisterNodeID = 1;
	return;
}
