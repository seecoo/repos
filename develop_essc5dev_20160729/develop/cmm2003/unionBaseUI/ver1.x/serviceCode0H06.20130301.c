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
#include "baseUIService.h"

/*********************************
�������:	0H06
������:		��ʼ��˽Կ�ռ�
��������:	��ʼ��˽Կ�ռ�
**********************************/

int UnionDealServiceCode0H06(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		i,j;
	char		hsmGroupID[16+1];
	char		tmpBuf[128+1];
	char		sql[256+1];
	int		isFailed = 0;
	char		status[5+1];
	int		vknum = 21;
	int		algorithmNum = 0;
	char		algorithmIDList[10+1];
	char		algorithmName[16+1];
	
	// �������ID 
	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(hsmGroupID);
	if (strlen(hsmGroupID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: hsmGroupID can not be null!\n");
		UnionSetResponseRemark("������鲻��Ϊ��");
		return(errCodeParameter);
	}

	// �㷨��ʶ�б� 
	memset(algorithmIDList,0,sizeof(algorithmIDList));
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmIDList",algorithmIDList,sizeof(algorithmIDList))) <= 0)
	{
		UnionLog("in UnionDealServiceCode0H06:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmIDList");
		strcpy(algorithmIDList,"0");
	}
	algorithmNum = strlen(algorithmIDList);

	// ��ʾ������
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","head/displayBody");	
		return(ret);
	}

	// �����ֶ��嵥
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"algorithmID,vkIndex,status");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");	
		return(ret);
	}

	//�������ֶ��嵥������
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"�㷨��ʶ,˽Կ����,״̬");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}

	// ��������
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",algorithmNum * vknum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	// ����������¼
	isFailed = 0;
	for (i = 0; i < algorithmNum; i++)
	{
		if (algorithmIDList[i] == '0')
			strcpy(algorithmName,"RSA�㷨");
		else if (algorithmIDList[i] == '1')
			strcpy(algorithmName,"SM2�㷨");
		else
		{
			algorithmName[0] = algorithmIDList[i];
			algorithmName[1] = 0;
		}
		
		for (j = 0; j < vknum; j++)
		{
			memset(sql,0,sizeof(sql));
			sprintf(sql,"insert into vkKeyIndex (hsmGroupID,algorithmID,vkIndex,keyType,keyName,status) values ('%s',%c,'%02d',2,'default',0)",hsmGroupID,algorithmIDList[i],j);
	
			memset(status,0,sizeof(status));
			if ((ret = UnionExecRealDBSql(sql)) <= 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H06:: UnionExecRealDBSql[%s]!\n",sql);
				strcpy(status,"ʧ��");	
				isFailed ++;
			}
			else
			{
				strcpy(status,"�ɹ�");
			}
	
			if ((ret = UnionLocateResponseNewXMLPackage("body/detail",i * vknum + j + 1)) < 0)	
			{
				UnionUserErrLog("in UnionDealServiceCode0H06:: UnionLocateResponseNewXMLPackage[%s]!\n","body/detail");
				return(ret);
			}
		
			if ((ret = UnionSetResponseXMLPackageValue("algorithmID",algorithmName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","algorithmID");
				return(ret);
			}

			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%02d",j);
			if ((ret = UnionSetResponseXMLPackageValue("vkIndex",tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","vkIndex");
				return(ret);
			}
	
			if ((ret = UnionSetResponseXMLPackageValue("status",status)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","status");
				return(ret);
			}
		}
	}

	if (isFailed == algorithmNum * vknum)
	{
		UnionSetResponseRemark("��ʼ��˽Կ�ռ�ʧ�ܣ������ظ���ʼ����");
		return(errCodeParameter);
	}
	return(0);
}
