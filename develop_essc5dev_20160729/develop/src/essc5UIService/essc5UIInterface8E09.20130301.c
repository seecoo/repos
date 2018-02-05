//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#define	maxGrpNum 100

/***************************************
�������:	8E09
������:		����������Կ�ڵ�
��������:	����������Կ�ڵ�
***************************************/
int UnionDealServiceCode8E09(PUnionHsmGroupRec phsmGroupRec)
{
	int				i = 0;
	int				ret;
	char				appID[32];
	char				nodeIDList[4096];
	int				status = 1;
	char				nodeNameList[4096];
	char				nodeIDGrp[maxGrpNum][128];
	int				statusList[maxGrpNum];
	char				nodeNameGrp[maxGrpNum][128];
	int				nodeIDNum = 0;
	int				nodeNameNum = 0;
	char				sql[512];
	char				tmpBuf[128];
	char				remark[128];
	char				fieldList[1024];
	char				fieldListChnName[4096];
	int				totalNum = 0;
	int				failNum = 0;
	
	// ��ԿӦ�ñ��
	if ((ret = UnionReadRequestXMLPackageValue("body/appID",appID,sizeof(appID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: UnionReadRequestXMLPackageValue[%s]!\n","body/appID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(appID);
	if (strlen(appID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: appID can not be null!\n");
		UnionSetResponseRemark("appID����Ϊ��");
		return(errCodeParameter);
	}
	
	// ��Կ�ڵ�
	if ((ret = UnionReadRequestXMLPackageValue("body/nodeIDList",nodeIDList,sizeof(nodeIDList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: UnionReadRequestXMLPackageValue[%s]!\n","body/nodeIDList");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(nodeIDList);
	if (strlen(nodeIDList) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: nodeIDList can not be null!\n");
		UnionSetResponseRemark("nodeIDList����Ϊ��");
		return(errCodeParameter);
	}
	
	// ƴ�����崮
	if ((nodeIDNum = UnionSeprateVarStrIntoVarGrp(nodeIDList,strlen(nodeIDList),',',nodeIDGrp,maxGrpNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSeprateVarStrIntoVarGrp[%s]!\n",nodeIDList);
		return(nodeIDNum);
	}
	
	// ��Կ�ڵ���
	if ((ret = UnionReadRequestXMLPackageValue("body/nodeNameList",nodeNameList,sizeof(nodeNameList))) > 0)
	{
		UnionFilterHeadAndTailBlank(nodeNameList);
		if (strlen(nodeNameList) != 0)
		{
			// ƴ�����崮
			if ((nodeNameNum = UnionSeprateVarStrIntoVarGrp(nodeNameList,strlen(nodeNameList),',',nodeNameGrp,maxGrpNum)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSeprateVarStrIntoVarGrp[%s]!\n",nodeNameList);
				return(nodeNameNum);
			}
			if (nodeNameNum != nodeIDNum)
			{
				UnionUserErrLog("in UnionDealServiceCode8E09:: nodeNameNum[%d] != nodeIDNum[%d]\n",nodeNameNum,nodeIDNum);
				UnionSetResponseRemark("�ڵ����Ƶ������ͽڵ��������ƥ��");
				return(errCodeParameter);
			}
		}
	}
	
	// ״̬
	if ((ret = UnionReadRequestXMLPackageValue("body/status",tmpBuf,sizeof(tmpBuf))) <= 0)
		status = 1;
	else
		status = atoi(tmpBuf);

	// ��ע
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
		remark[0] = 0;

	// ������ʾ������
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s]\n","body/displayBody");
		return(ret);
	}

	// �ֶ��嵥
	snprintf(fieldList,sizeof(fieldList),"nodeID,nodeName,status");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldList");
		return(ret);
	}

	// �ֶ��嵥������
	snprintf(fieldListChnName,sizeof(fieldListChnName),"�ڵ�ID,�ڵ�����,״̬");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldListChnName");
		return(ret);
	}

	// ��������
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",nodeIDNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s]\n","body/totalNum");
		return(ret);
	}

	for (i = 0; i < nodeIDNum; i++)
	{
		if (nodeNameNum > 0)
			snprintf(sql,sizeof(sql),"insert into keyNode(appID,nodeID,nodeName,status,remark) values('%s','%s','%s',%d,'%s')",appID,nodeIDGrp[i],nodeNameGrp[i],status,remark);
		else
			snprintf(sql,sizeof(sql),"insert into keyNode(appID,nodeID,status,remark) values('%s','%s',%d,'%s')",appID,nodeIDGrp[i],status,remark);
		
		if ((ret = UnionExecRealDBSql(sql)) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E09:: UnionExecRealDBSql nodeID[%d][%s] sql[%s]!\n",i,nodeIDGrp[i],sql);
			statusList[i] = 0;
			failNum ++;
			//return(ret);
		}
		else
			statusList[i] = 1;
	}

	
	status = 1;
	i = 0;
	while(totalNum != nodeIDNum)
	{
		if (i == nodeIDNum)
		{
			i = 0;
			status = 0;
		}

		// ��������ʧ�ܵ�
		if (statusList[i] == status)
		{
			i ++;
			continue;
		}
		
		totalNum ++;
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)	
		{
			UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s][%d]\n","body/detail",totalNum);
			return(ret);
		}

		// �ڵ�ID
		if ((ret = UnionSetResponseXMLPackageValue("nodeID",nodeIDGrp[i])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s][%d]\n","nodeID",i+1);
			return(ret);
		}

		// �ڵ�ID
		if (nodeNameNum <= 0)
			nodeNameGrp[i][0] = 0;
		if ((ret = UnionSetResponseXMLPackageValue("nodeName",nodeNameGrp[i])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s][%d]\n","nodeID",i+1);
			return(ret);
		}

		// �ڵ�״̬
		snprintf(tmpBuf,sizeof(tmpBuf),"%s",statusList[i] == 1 ? "�ɹ�":"ʧ��");
		if ((ret = UnionSetResponseXMLPackageValue("status",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E09:: UnionSetResponseXMLPackageValue[%s][%d]\n","status",i+1);
			return(ret);
		}

		i ++;
	}
	if (failNum == totalNum)
	{
		UnionSetResponseRemark("����������Կ�ڵ�ʧ��");	
		return(errCodeParameter);
	}

	return(0);
}


