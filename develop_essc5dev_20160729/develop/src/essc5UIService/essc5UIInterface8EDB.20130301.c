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
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

#include "unionSRJ1401Cmd.h"
#include "base64.h"

/***************************************
  服务代码:	8EDB
  服务名:	证书同步设备查看(843)
  功能描述:	证书同步设备查看(843)
 ***************************************/
int UnionDealServiceCode8EDB(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			totalNum= 0;
	char			tmpBuf[128];
	char                    fieldList[1024+1];
        char                    fieldListChnName[4096+1];
	int			i;
	char			hsmGrpID[128];

	TUnionTargetDevWithCount	targetDevList;
	TUnionHsmGroupRec	pSpecHsmGroup;

	// 设置字段清单
        memset(fieldList,0,sizeof(fieldList));
        strcpy(fieldList,"ipAddr,port,hsmGrpID");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
                return(ret);
        }

        // 设置字段清单中文名
        memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"IP地址,端口号,加密机组ID");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
        {       
                UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
                return(ret);
        }

	// 读取加密机组ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// 获取并设置使用密码机组
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDB:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("加密机指令类型不匹配");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);
	
	memset(&targetDevList, 0, sizeof(targetDevList));
	if ((ret = UnionSRJ1401CmdES843(&targetDevList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionSRJ1401CmdES843,ret = [%d]!\n",ret);
		return(ret);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");
	for (i = 0; i < targetDevList.count; ++i)
	{
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum + 1)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",totalNum+1);
                        return(ret);
                }

		if ((ret = UnionSetResponseXMLPackageValue("ipAddr", targetDevList.certinfo[i].ip)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionSetResponseXMLPackageValue[%s][%d]!\n","ipAddr",totalNum+1);
                        return(ret);
                }

		snprintf(tmpBuf, sizeof(tmpBuf), "%d", targetDevList.certinfo[i].port);
		if ((ret = UnionSetResponseXMLPackageValue("port",tmpBuf)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionSetResponseXMLPackageValue[%s][%d]!\n","port",totalNum+1);
                        return(ret);
                }

		if ((ret = UnionSetResponseXMLPackageValue("hsmGrpID",hsmGrpID)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionSetResponseXMLPackageValue[%s][%d]!\n","hsmGrpID",totalNum+1);
                        return(ret);
                }

		totalNum++;
	}
	UnionLocateResponseXMLPackage("",0);

	//赋值总记录数
        snprintf(tmpBuf, sizeof(tmpBuf), "%d",totalNum);
        if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EDB:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
                return(ret);
        }

	return(0);
}

