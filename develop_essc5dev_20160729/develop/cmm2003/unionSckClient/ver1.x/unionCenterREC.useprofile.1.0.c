// Author:	Wolfgang Wang
// Date:	2006/07/26

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionCenterREC.h"

int     gunionNumOfEssc = 0;
int     gunionIsCenterRECConnected = 0;
char    gunionIDOfEsscAPI[10+1] = "TE";
char    gunionIPAddrOfEssc[conMaxNumOfEssc][15+1];
int     gunionPortOfEssc[conMaxNumOfEssc];
int     gunionTimeoutOfEssc = 5;
int     gunionIsDebug = 0;
int     gunionIsShortConnUsed = 0;
int     gunionLenOfMsgHeader = 8;
int     gunionTypeOfAutoSign = 100;

int UnionConnectCenterREC()
{
	int	ret;
	char	*ptr;
	int	index;
	char	varName[100];
	
	if (gunionIsCenterRECConnected)
		return(0);
	
	// ��DEBUG��־
	if ((ptr = getenv("openDebug")) != NULL)
		gunionIsDebug = atoi(ptr);
	
	// �������������
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
		// �����ĵĶ˿ں�
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
		// �����ĵĶ˿ں�
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

	// ��APIʹ�õ�Ӧ�ñ��
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
	
	// ��������ͨѶ�ĳ�ʱֵ
	if ((ptr = getenv("timeoutOfCenterSecuSvr")) != NULL)
	{
		if ((gunionTimeoutOfEssc = atoi(ptr)) <= 0)
		{
			UnionUserErrLog("in UnionConnectCenterREC:: %s is not timeout!\n",ptr);
			return(errCodeAPIEsscSvrTimeoutInvalid);
		}
	}

	// ���Ƿ�ʹ�ö�����
	if ((ptr = getenv("isShortConnUsed")) != NULL)
		gunionIsShortConnUsed = atoi(ptr);

	// ��ʹ�õ��Զ�ǩ������
	if ((ptr = getenv("typeOfAutoSign")) != NULL)
		gunionTypeOfAutoSign = atoi(ptr);
	if (gunionTypeOfAutoSign < 0)
		gunionTypeOfAutoSign = 100;

	gunionIsCenterRECConnected = 1;
	return(0);
}

int UnionIsDebug()
{
	return(gunionIsDebug);
}

char *UnionGetIPAddrOfCenterSecuSvr(int index)
{
	if (index <= 0)
		index = 1;
	return(gunionIPAddrOfEssc[(index-1)%gunionNumOfEssc]);
}

int UnionGetPortOfCenterSecuSvr(int index)
{
	if (index <= 0)
		index = 1;
	return(gunionPortOfEssc[(index-1)%gunionNumOfEssc]);
}

int UnionGetTimeoutOfCenterSecuSvr()
{
	//return(gunionTimeoutOfEssc[(index-1)%gunionNumOfEssc]);
	return gunionTimeoutOfEssc;
}

char *UnionGetIDOfEsscAPI()
{
	return(gunionIDOfEsscAPI);
}

int UnionIsShortConnectionUsed()
{
	return(gunionIsShortConnUsed);
}

int UnionGetLenOfMsgHeaer()
{
	return(gunionLenOfMsgHeader);
}

int UnionGetAutoAppendSignType()
{
	return(gunionTypeOfAutoSign);
}



