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
#include "unionRecFile.h"

/***************************************
  服务代码:	8ED7
  服务名:	证书下载(622)by userDn
  功能描述:	证书下载(622)by userDn
 ***************************************/
int UnionDealServiceCode8ED7(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			userDN[128];
	char			cert[4096];
	char			hsmGrpID[128];
	char			fileName[128];
	char			filePath[128];
	char			fileStoreDir[512];
	char			fullTime[20];
	char			sql[1024];
	int			certLen;
	FILE			*fp;

	TUnionHsmGroupRec	pSpecHsmGroup;

	// 证书DN
	if ((ret = UnionReadRequestXMLPackageValue("body/userDN",userDN,sizeof(userDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionReadRequestXMLPackageValue[%s]!\n","body/userDN");
		return(ret);
	}
	userDN[ret] = 0;

	// 读取密码机组ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// 获取并设置使用的密码机组
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("加密机指令类型不匹配");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((ret = UnionSRJ1401CmdES622(userDN, (unsigned char *)cert, &certLen)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionSRJ1401CmdES622,ret = [%d]!\n",ret);
			return(ret);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8ED7:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("非法的加密机指令类型");
		return(errCodeParameter);
	}

	//查询文件存储路径
        memset(sql,0,sizeof(sql));
        sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'","CERT");
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionSelectRealDBRecord[%s]!\n",sql);
                return(ret);
        }
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }

        // 读取文件存储目录
        memset(fileStoreDir,0,sizeof(fileStoreDir));
        if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
                return(ret);
        }

	// 设置下载证书名称
	memset(fullTime, 0, sizeof(fullTime));
	UnionGetFullSystemDateTime(fullTime);
	snprintf(fileName, sizeof(fileName), "%s.cer", fullTime);

	memset(filePath,0,sizeof(filePath));
        UnionFormFullFileName(fileStoreDir,fileName,filePath);

	// 将证书内容写入文件中
	if ((fp = fopen(filePath, "wb")) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED7:: fopen[%s]!\n",filePath);
                return(errCodeUseOSErrCode);
	}
	if ((ret = fwrite(cert, 1, certLen, fp)) != certLen)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED7:: fwrite[%s]!\n",filePath);
		fclose(fp);
                return(errCodeUseOSErrCode);
	}
	fclose(fp);

	// 设置证书名称
	if ((ret = UnionSetResponseXMLPackageValue("body/fileName",fileName)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED3:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileName");
                return(ret);
        }

	// 设置文件类型
	if ((ret = UnionSetResponseXMLPackageValue("body/fileTypeID","CERT")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED3:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileTypeID");
                return(ret);
        }
	
	return(0);
}

