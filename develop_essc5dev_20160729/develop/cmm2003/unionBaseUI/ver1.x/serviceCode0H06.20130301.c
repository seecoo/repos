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
服务代码:	0H06
服务名:		初始化私钥空间
功能描述:	初始化私钥空间
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
	
	// 密码机组ID 
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
		UnionSetResponseRemark("密码机组不能为空");
		return(errCodeParameter);
	}

	// 算法标识列表 
	memset(algorithmIDList,0,sizeof(algorithmIDList));
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmIDList",algorithmIDList,sizeof(algorithmIDList))) <= 0)
	{
		UnionLog("in UnionDealServiceCode0H06:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmIDList");
		strcpy(algorithmIDList,"0");
	}
	algorithmNum = strlen(algorithmIDList);

	// 显示报文体
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","head/displayBody");	
		return(ret);
	}

	// 设置字段清单
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"algorithmID,vkIndex,status");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");	
		return(ret);
	}

	//　设置字段清单中文名
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"算法标识,私钥索引,状态");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}

	// 设置总数
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",algorithmNum * vknum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H06:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	// 增加索引记录
	isFailed = 0;
	for (i = 0; i < algorithmNum; i++)
	{
		if (algorithmIDList[i] == '0')
			strcpy(algorithmName,"RSA算法");
		else if (algorithmIDList[i] == '1')
			strcpy(algorithmName,"SM2算法");
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
				strcpy(status,"失败");	
				isFailed ++;
			}
			else
			{
				strcpy(status,"成功");
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
		UnionSetResponseRemark("初始化私钥空间失败（不能重复初始化）");
		return(errCodeParameter);
	}
	return(0);
}
