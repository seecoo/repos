//	Author:		周修伟
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
服务代码:	8E0B
服务名:		批量增加密钥节点
功能描述:	批量增加密钥节点(兰州银行添加范围内节点)
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
	
	// 密钥应用编号
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
		UnionSetResponseRemark("appID不能为空");
		return(errCodeParameter);
	}
	
	// 起始节点
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
		UnionSetResponseRemark("起始节点不能为空");
		return(errCodeParameter);
	}
	if('S' == firstNode[0] || 's' == firstNode[0])
	{
		isSCharBegin = 1;
		if(!UnionIsDigitString(firstNode + 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", firstNode+1);
			UnionSetResponseRemark("起始节点开头S/s后的字符不为数字");
			return(errCodeParameter);
		}
		firstNodeID = atoi(firstNode+1);
		if(firstNodeID > 9999999)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNode[%d]\n", firstNodeID);
			UnionSetResponseRemark("初始节点位数过长，S/s+7位有效数字");
			return(errCodeParameter);
		}
	}
	else
	{
		if(!UnionIsDigitString(firstNode))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", firstNode);
                        UnionSetResponseRemark("起始节点不全为数字");
                        return(errCodeParameter);
                }
		firstNodeID = atoi(firstNode);
		if(firstNodeID > 99999999)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNode[%d]\n", firstNodeID);
                        UnionSetResponseRemark("初始节点位数过长，8位有效数字");
                        return(errCodeParameter);
                }
	}

	//终止节点
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
                UnionSetResponseRemark("终止节点不能为空");
                return(errCodeParameter);
        }
	/*
	if((isSCharBegin == 1) && ((lastNode[0] != 'S' && firstNode[0] != 'S') && (lastNode[0] != 's' && firstNode[0] != 's')))
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNodeID[%s] lastNodeID[%s]\n", firstNode, lastNode);
		UnionSetResponseRemark("起始节点和终止节点不以相同的S/s开头");
		return(errCodeParameter);
	}
	*/
	if(('S' == firstNode[0] && 'S' == lastNode[0]) || ('s' == firstNode[0] && 's' == lastNode[0]))
        {
                isSCharBegin = 1;
                if(!UnionIsDigitString(lastNode + 1))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", lastNode+1);
                        UnionSetResponseRemark("终止节点开头S后的字符不为数字");
                        return(errCodeParameter);
                }
                lastNodeID = atoi(lastNode+1);
		if(lastNodeID > 9999999)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: lastNode[%d]\n", lastNodeID);
                        UnionSetResponseRemark("终止节点位数过长，S+7位有效数字");
                        return(errCodeParameter);
                }
        }
	else if(isSCharBegin)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: firstNodeID[%s] lastNodeID[%s]\n", firstNode, lastNode);
		UnionSetResponseRemark("起始节点和终止节点不以相同的S/s开头");
                return(errCodeParameter);
	}
        else
        {
                if(!UnionIsDigitString(lastNode))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionIsDigitString[%s]\n", lastNode);
                        UnionSetResponseRemark("终止节点不全为数字");
                        return(errCodeParameter);
                }
                lastNodeID = atoi(lastNode);
		if(lastNodeID > 99999999)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E0B:: lastNode[%d]\n", lastNodeID);
                        UnionSetResponseRemark("终止节点位数过长，8位有效数字");
                        return(errCodeParameter);
                }
        }
	
	if(lastNodeID < firstNodeID)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: lastNodeID[%d] lt firstNodeID[%d]\n", lastNodeID,firstNodeID);
		UnionSetResponseRemark("终止节点不应小于起始节点");
		return(errCodeParameter);
	}
	
	nodeIDNum = lastNodeID - firstNodeID+1;
	if(nodeIDNum > maxGrpNum)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: nodeIDNum[%d] gt %d\n", nodeIDNum, maxGrpNum);
		UnionSetResponseRemark("节点总数不应超过10000");
		return(errCodeParameter);
	}
	
	// 状态
	if ((ret = UnionReadRequestXMLPackageValue("body/status",tmpBuf,sizeof(tmpBuf))) <= 0)
		status = 0;
	else
		status = atoi(tmpBuf);

	// 备注
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
		remark[0] = 0;

	// 设置显示报文体
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s]\n","body/displayBody");
		return(ret);
	}

	// 字段清单
	snprintf(fieldList,sizeof(fieldList),"nodeID,status");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldList");
		return(ret);
	}

	// 字段清单中文名
	snprintf(fieldListChnName,sizeof(fieldListChnName),"节点ID,状态");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldListChnName");
		return(ret);
	}

	// 设置总数
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

		// 优先设置失败的
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

		// 节点ID
		if ((ret = UnionSetResponseXMLPackageValue("nodeID",nodeIDList[offset])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s][%d]\n","nodeID",offset+1);
			return(ret);
		}

		// 节点状态
		snprintf(tmpBuf,sizeof(tmpBuf),"%s",statusList[offset] == 1 ? "成功":"失败");
		if ((ret = UnionSetResponseXMLPackageValue("status",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0B:: UnionSetResponseXMLPackageValue[%s][%d]\n","status",offset+1);
			return(ret);
		}

		offset ++;
	}
	if (failNum == totalNum)
	{
		UnionSetResponseRemark("批量增加密钥节点失败");	
		return(errCodeParameter);
	}

	return(0);
}


