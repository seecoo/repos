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

// ���״���׶Σ�0���״���ǰ��1���״����У�2���״����
int	gunionStageOfTransProcessing = defStageOfTransing;
int	gunionRegisterNodeID = 1;

// ���ý��״���׶�
void UnionSetStageOfTransProcessing(int stage)
{
	gunionStageOfTransProcessing = stage;
	return;
}

// ��ʼ���Ǽǽڵ�ID
void UnionInitRegisterNodeID()
{
	gunionRegisterNodeID = 1;
	return;
}

// �Ǽǽڵ�����ʱ��
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
	
	// ����ʹ��ʱ��
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

// ���������ģ�����ʱ��
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

	// ���õ�ǰʱ��
	UnionSetUserSetTime(&userTime);	
	
	gunionRegisterNodeID = 1;
	
	return;
}

// ����ʱ�䵽������
void UnionSetTimeToRequestPackage()
{
	char	tmpBuf[32];
	struct timeval	userTime;

	if (UnionReadIntTypeRECVar("isRegisterRunningTime") <= 0)
		return;

	// ��ȡ��ǰ����ʱ��
	UnionGetUserSetTime(&userTime);
	
	snprintf(tmpBuf,sizeof(tmpBuf),"%ld",userTime.tv_sec);
	UnionSetRequestXMLPackageValue("time/tv_sec",tmpBuf);
	snprintf(tmpBuf,sizeof(tmpBuf),"%ld",userTime.tv_usec);
	UnionSetRequestXMLPackageValue("time/tv_usec",tmpBuf);
	
	gunionRegisterNodeID = 1;
	return;
}
