//	Author:		����ΰ
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-07-01

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

#define maxGrpNum 10000

/***************************************
�������:	8E0B
������:		����������Կ�ڵ�
��������:	����������Կ�ڵ�(����������ӷ�Χ�ڽڵ�)
***************************************/
int UnionDealServiceCode8E0B(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				appID[32];
	int				firstNodeID = 0;
	int				lastNodeID = 0;
	int				status = 1;
	char                            remark[128];
	
	char				firstNode[16];
	char				lastNode[16];
	char				tmpBuf[128];
	int				offset = 0;
	int				nodeIDNum = 0;
	int				statusList[maxGrpNum];
	char				nodeIDList[maxGrpNum][16];
	char                            fieldList[1024];
        char                            fieldListChnName[4096];
	int				isSCharBegin = 0;
	
	char				sql[512];
	int				totalNum = 0;
	int				failNum = 0;
	
	// ��ԿӦ�ñ��
	if ((ret = UnionReadRequestXMLPackageValue("body/appID",appID,sizeof(appID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/appID");
		return(ret);
	}
	appID[ret] = 0;
	UnionFilterHeadAndTailBlank(appID);
	if (strlen(appID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: appID can not be null!\n");
		UnionSetResponseRemark("appID����Ϊ��");
		return(errCodeParameter);
	}
	
	// ��ʼ�ڵ�
	if ((ret = UnionReadRequestXMLPackageValue("body/firstNodeID",firstNode,sizeof(firstNode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/firstNode");
		return(ret);
	}
	firstNode[ret] = 0;
	UnionFilterHeadAndTailBlank(firstNode);
	if (strlen(firstNode) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNode can not be null!\n");
		UnionSetResponseRemark("��ʼ�ڵ㲻��Ϊ��");
		return(errCodeParameter);
	}
	if('S' == firstNode[0] || 's' == firstNode[0])
	{
		isSCharBegin = 1;
		if(!UnionIsDigitString(firstNode + 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", firstNode+1);
			UnionSetResponseRemark("��ʼ�ڵ㿪ͷS/s����ַ���Ϊ����");
			return(errCodeParameter);
		}
		firstNodeID = atoi(firstNode+1);
		if(firstNodeID > 9999999)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNode[%d]\n", firstNodeID);
			UnionSetResponseRemark("��ʼ�ڵ�λ��������S/s+7λ��Ч����");
			return(errCodeParameter);
		}
	}
	else
	{
		if(!UnionIsDigitString(firstNode))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", firstNode);
                        UnionSetResponseRemark("��ʼ�ڵ㲻ȫΪ����");
                        return(errCodeParameter);
                }
		firstNodeID = atoi(firstNode);
		if(firstNodeID > 99999999)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNode[%d]\n", firstNodeID);
                        UnionSetResponseRemark("��ʼ�ڵ�λ��������8λ��Ч����");
                        return(errCodeParameter);
                }
	}

	//��ֹ�ڵ�
	if ((ret = UnionReadRequestXMLPackageValue("body/lastNodeID",lastNode,sizeof(lastNode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/lastNode");
                return(ret);
        }
	lastNode[ret] = 0;
        UnionFilterHeadAndTailBlank(lastNode);
        if (strlen(lastNode) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0B:: lastNode can not be null!\n");
                UnionSetResponseRemark("��ֹ�ڵ㲻��Ϊ��");
                return(errCodeParameter);
        }
	/*
	if((isSCharBegin == 1) && ((lastNode[0] != 'S' && firstNode[0] != 'S') && (lastNode[0] != 's' && firstNode[0] != 's')))
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNodeID[%s] lastNodeID[%s]\n", firstNode, lastNode);
		UnionSetResponseRemark("��ʼ�ڵ����ֹ�ڵ㲻����ͬ��S/s��ͷ");
		return(errCodeParameter);
	}
	*/
	if(('S' == firstNode[0] && 'S' == lastNode[0]) || ('s' == firstNode[0] && 's' == lastNode[0]))
        {
                isSCharBegin = 1;
                if(!UnionIsDigitString(lastNode + 1))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", lastNode+1);
                        UnionSetResponseRemark("��ֹ�ڵ㿪ͷS����ַ���Ϊ����");
                        return(errCodeParameter);
                }
                lastNodeID = atoi(lastNode+1);
		if(lastNodeID > 9999999)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: lastNode[%d]\n", lastNodeID);
                        UnionSetResponseRemark("��ֹ�ڵ�λ��������S+7λ��Ч����");
                        return(errCodeParameter);
                }
        }
	else if(isSCharBegin)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNodeID[%s] lastNodeID[%s]\n", firstNode, lastNode);
		UnionSetResponseRemark("��ʼ�ڵ����ֹ�ڵ㲻����ͬ��S/s��ͷ");
                return(errCodeParameter);
	}
        else
        {
                if(!UnionIsDigitString(lastNode))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", lastNode);
                        UnionSetResponseRemark("��ֹ�ڵ㲻ȫΪ����");
                        return(errCodeParameter);
                }
                lastNodeID = atoi(lastNode);
		if(lastNodeID > 99999999)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: lastNode[%d]\n", lastNodeID);
                        UnionSetResponseRemark("��ֹ�ڵ�λ��������8λ��Ч����");
                        return(errCodeParameter);
                }
        }
	
	if(lastNodeID < firstNodeID)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: lastNodeID[%d] lt firstNodeID[%d]\n", lastNodeID,firstNodeID);
		UnionSetResponseRemark("��ֹ�ڵ㲻ӦС����ʼ�ڵ�");
		return(errCodeParameter);
	}
	
	nodeIDNum = lastNodeID - firstNodeID+1;
	if(nodeIDNum > maxGrpNum)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: nodeIDNum[%d] gt %d\n", nodeIDNum, maxGrpNum);
		UnionSetResponseRemark("�ڵ�������Ӧ����10000");
		return(errCodeParameter);
	}
	
	// ״̬
	if ((ret = UnionReadRequestXMLPackageValue("body/status",tmpBuf,sizeof(tmpBuf))) <= 0)
		status = 0;
	else
		status = atoi(tmpBuf);

	// ��ע
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
		remark[0] = 0;

	// ������ʾ������
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s]\n","body/displayBody");
		return(ret);
	}

	// �ֶ��嵥
	snprintf(fieldList,sizeof(fieldList),"nodeID,status");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldList");
		return(ret);
	}

	// �ֶ��嵥������
	snprintf(fieldListChnName,sizeof(fieldListChnName),"�ڵ�ID,״̬");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldListChnName");
		return(ret);
	}

	// ��������
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",nodeIDNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s]\n","body/totalNum");
		return(ret);
	}

	for (offset = 0; offset < nodeIDNum; offset++)
	{
		if(isSCharBegin)
			snprintf(nodeIDList[offset], sizeof(nodeIDList[offset]), "%c%07d", firstNode[0], firstNodeID+offset);
		else
			snprintf(nodeIDList[offset], sizeof(nodeIDList[offset]), "%08d", firstNodeID+offset);
		snprintf(sql,sizeof(sql),"insert into keyNode(appID,nodeID,status,remark) values('%s','%s',%d,'%s')",appID,nodeIDList[offset],status,remark);
		
		if ((ret = UnionExecRealDBSql(sql)) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionExecRealDBSql nodeID[%d][%s] sql[%s]!\n",offset,nodeIDList[offset],sql);
			statusList[offset] = 0;
			failNum ++;
			//return(ret);
		}
		else
			statusList[offset] = 1;
	}

	status = 1;
	offset = 0;
	while(totalNum != nodeIDNum)
	{
		if (offset == nodeIDNum)
		{
			offset = 0;
			status = 0;
		}

		// ��������ʧ�ܵ�
		if (statusList[offset] == status)
		{
			offset ++;
			continue;
		}
		
		totalNum ++;
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)	
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s][%d]\n","body/detail",totalNum);
			return(ret);
		}

		// �ڵ�ID
		if ((ret = UnionSetResponseXMLPackageValue("nodeID",nodeIDList[offset])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s][%d]\n","nodeID",offset+1);
			return(ret);
		}

		// �ڵ�״̬
		snprintf(tmpBuf,sizeof(tmpBuf),"%s",statusList[offset] == 1 ? "�ɹ�":"ʧ��");
		if ((ret = UnionSetResponseXMLPackageValue("status",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s][%d]\n","status",offset+1);
			return(ret);
		}

		offset ++;
	}
	if (failNum == totalNum)
	{
		UnionSetResponseRemark("����������Կ�ڵ�ʧ��");	
		return(errCodeParameter);
	}

	return(0);
}


