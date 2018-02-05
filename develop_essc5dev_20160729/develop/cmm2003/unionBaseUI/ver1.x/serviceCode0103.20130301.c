
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionREC.h"
#include "baseUIService.h"

/***************************************
�������:	0103
������:		����û�״̬
��������:	����û�״̬
***************************************/
int UnionDealServiceCode0103(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	userID[40+1];
	char	loginFlag[1+1];
	char	lastLoginLocation[32+1];
	char	lastLoginTime[14+1];
	int	maxFreeTimes;
	long	freeTimes;
	char	sql[512+1];
	char	lastOperationTime[14+1];
	char	passwdUpdateTime[14+1];
	int	len = 0;
	char	tmpNum[10+1];
	int	totalNum = 0;
	int	i = 0;
	char	passwdWarn[128+1];
	char	passwdUpdateDate[8+1];
	int     maxEffictiveDays;	// ����������Ч������
	int     usedDays;		// �����ʹ������
	
	// ��ȡ�û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// �����û���
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select loginFlag,lastLoginLocation,lastLoginTime,lastOperationTime,passwdUpdateTime from sysUser where userID = '%s'",userID);
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: �û�ID[%s]������!\n",sql);
		return(errCodeOperatorMDL_OperatorNotExists);		
	}
	
	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionLocateXMLPackage!\n");
		return(ret);
	}

	// ��¼״̬
	memset(loginFlag,0,sizeof(loginFlag));
	if ((ret = UnionReadXMLPackageValue("loginFlag", loginFlag, sizeof(loginFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s]!\n","loginFlag");
		return(ret);
	}

	// �����¼�ص�
	memset(lastLoginLocation,0,sizeof(lastLoginLocation));
	if ((ret = UnionReadXMLPackageValue("lastLoginLocation", lastLoginLocation, sizeof(lastLoginLocation))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s]!\n","lastLoginLocation");
		return(ret);
	}

	// �����¼ʱ��
	memset(lastLoginTime,0,sizeof(lastLoginTime));
	if ((ret = UnionReadXMLPackageValue("lastLoginTime", lastLoginTime, sizeof(lastLoginTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s]!\n","lastLoginTime");
		return(ret);
	}

	if ( (maxFreeTimes = UnionReadIntTypeRECVar("freeTimesOfUser"))<=0)
		maxFreeTimes = 1800;  // Ĭ��1800��
		
	if (atoi(loginFlag))
	{
		// ��ȡ�������ʱ��
		memset(lastOperationTime,0,sizeof(lastOperationTime));
		if ((ret = UnionReadXMLPackageValue("lastOperationTime", lastOperationTime, sizeof(lastOperationTime))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s]!\n","lastOperationTime");
			return(ret);
		}
		UnionCalcuSecondsPassedAfterSpecTime(lastOperationTime, &freeTimes);
		if ((freeTimes - maxFreeTimes) > 0)
		{
			memset(sql,0,sizeof(sql));	
			sprintf(sql,"update sysUser set loginFlag = 0 where userID = '%s'",userID);
			if ((ret = UnionExecRealDBSql(sql)) < 0) 
			{
				UnionUserErrLog("in UnionDealServiceCode0103:: UnionExecRealDBSql[%s]!\n",sql);
			}
			else
			{
				memset(loginFlag,0,sizeof(loginFlag));
				strcpy(loginFlag,"0");
			}
		}
	}

	// �����޸�ʱ��
	memset(passwdUpdateTime,0,sizeof(passwdUpdateTime));
	if ((ret = UnionReadXMLPackageValue("passwdUpdateTime", passwdUpdateTime, sizeof(passwdUpdateTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s]!\n","passwdUpdateTime");
		return(ret);
	}

	memset(passwdUpdateDate,0,sizeof(passwdUpdateDate));
	strncpy(passwdUpdateDate,passwdUpdateTime,8);
	
	if ((maxEffictiveDays = UnionReadIntTypeRECVar("effictiveDaysOfPwd")) <= 0)
		maxEffictiveDays = 36500;  // Ĭ��36500��

	memset(passwdWarn,0,sizeof(passwdWarn));
	if (strlen(passwdUpdateDate) == 0)
		strcpy(passwdWarn, "ʹ�õ��ǳ�ʼ���룬���޸�����");
	else
	{
		// ȡ������ʹ�õ�����
		usedDays = UnionDecideDaysBeforeToday(passwdUpdateDate);
		if (usedDays < 0)
		{
			UnionUserErrLog("in UnionIsOverduePasswd:: UnionDecideDaysBeforeToday [%s] \n", passwdUpdateDate);
			return(usedDays);
		}
	
		if ((maxEffictiveDays-usedDays) <= 7 && (maxEffictiveDays-usedDays) >= 0)
			sprintf(passwdWarn, "%s%d%s", "���뻹��", (maxEffictiveDays-usedDays), "����ڣ����޸�����");
	}
	
	// ������Ӧ����
	if ((ret = UnionSetResponseXMLPackageValue("body/loginFlag",loginFlag)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionSetResponseXMLPackageValue[%s]!\n","body/loginFlag");
		return(ret);
	}

	// ���������¼�ص�
	if ((ret = UnionSetResponseXMLPackageValue("body/lastLoginLocation",lastLoginLocation)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionSetResponseXMLPackageValue[%s]!\n","body/lastLoginLocation");
		return(ret);
	}
	
	// ���������¼ʱ��
	if ((ret = UnionSetResponseXMLPackageValue("body/lastLoginTime",lastLoginTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionSetResponseXMLPackageValue[%s]!\n","body/lastLoginTime");
		return(ret);
	}

	// ���������¼ʱ��
	if (strlen(passwdWarn) > 0)
	{
		// ���������¼ʱ��
		if ((ret = UnionSetResponseXMLPackageValue("body/passwdWarn",passwdWarn)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0103:: UnionSetResponseXMLPackageValue[%s]!\n","body/passwdWarn");
			return(ret);
		}
	}

	// ��ѯ��½��ʱ�û����޸���״̬
	len = sprintf(sql,"select userID,lastOperationTime from sysUser where loginFlag = 1");
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionSetResponseXMLPackageValue[%s]!\n","body/lastLoginLocation");
		return(ret);
	}
	else if (ret == 0)
		return(0);	

	memset(tmpNum,0,sizeof(tmpNum));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	else
		totalNum = atoi(tmpNum);

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0103:: UnionLocateXMLPackage[%s][%d]!\n","detail",i);
			continue;
		}

		// ��ȡ�û�ID
		memset(userID,0,sizeof(userID));
		if ((ret = UnionReadXMLPackageValue("userID",userID,sizeof(userID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s][%d]!\n","userID",i);
			continue;
		}

		// ��ȡ�������ʱ��
		memset(lastOperationTime,0,sizeof(lastOperationTime));
		if ((ret = UnionReadXMLPackageValue("lastOperationTime",lastOperationTime,sizeof(lastOperationTime))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0103:: UnionReadXMLPackageValue[%s][%d]!\n","lastOperationTime",i);
			continue;
		}

		UnionCalcuSecondsPassedAfterSpecTime(lastOperationTime, &freeTimes);
		if ((freeTimes - maxFreeTimes) > 0)
		{
			len = sprintf(sql,"update sysUser set loginFlag = 0 where userID = '%s'",userID);
			sql[len] = 0;
			if ((ret = UnionExecRealDBSql(sql)) < 0) 
			{
				UnionUserErrLog("in UnionDealServiceCode0103:: UnionExecRealDBSql[%s][%d]!\n",sql,i);
			}
		}
	}
	
	return(0);
}
