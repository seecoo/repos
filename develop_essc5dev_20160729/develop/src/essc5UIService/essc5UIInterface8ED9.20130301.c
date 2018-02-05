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
  服务代码:	8ED9
  服务名:	生成证书请求文件
  功能描述:	生成证书请求文件
 ***************************************/
int UnionDealServiceCode8ED9(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			tmpBuf[128];
	char			hsmGrpID[128];
	char			countryName[128];
	char			stateOrProvinceName[128];
	char			localityName[128];
	char			organizationName[128];
	char			organizationalUnitName[128];
	char			email[128];
	char			commonName[128];
	char			p10buf[8192*4];
	int			algID;
	int			vkIndex;
	int			pkcsLen;

	char			fileName[128];
	char			filePath[128];
	char			fileStoreDir[512];
	char			fullTime[20];
	char			sql[1024];
	FILE			*fp;

	TUnionHsmGroupRec	pSpecHsmGroup;


	// 证书类型
	if ((ret = UnionReadRequestXMLPackageValue("body/algID", tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/algID");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algID = atoi(tmpBuf);

	// 私钥索引号
	if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex", tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
		return(ret);
	}
	tmpBuf[ret] = 0;
	vkIndex = atoi(tmpBuf);

	// 国家名
	if ((ret = UnionReadRequestXMLPackageValue("body/countryName", countryName,sizeof(countryName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/countryName");
		return(ret);
	}
	countryName[ret] = 0;
	
	// 省名
	if ((ret = UnionReadRequestXMLPackageValue("body/stateOrProvinceName", stateOrProvinceName,sizeof(stateOrProvinceName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/stateOrProvinceName");
		return(ret);
	}
	stateOrProvinceName[ret] = 0;

	// 城市名
	if ((ret = UnionReadRequestXMLPackageValue("body/localityName", localityName,sizeof(localityName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/localityName");
		return(ret);
	}
	localityName[ret] = 0;

	// 单位名
	if ((ret = UnionReadRequestXMLPackageValue("body/organizationName", organizationName,sizeof(organizationName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/organizationName");
		return(ret);
	}
	organizationName[ret] = 0;

	// 部门名
	if ((ret = UnionReadRequestXMLPackageValue("body/organizationalUnitName", organizationalUnitName,sizeof(organizationalUnitName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/organizationalUnitName");
		return(ret);
	}
	organizationalUnitName[ret] = 0;

	// 用户名
	if ((ret = UnionReadRequestXMLPackageValue("body/commonName", commonName,sizeof(commonName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/commonName");
		return(ret);
	}
	commonName[ret] = 0;

	// Email地址
	if ((ret = UnionReadRequestXMLPackageValue("body/email", email,sizeof(email))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/email");
		return(ret);
	}
	email[ret] = 0;
	
	// 加密机组ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// 获取并设置使用的密码机组
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED9:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("加密机指令类型不匹配");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	memset(p10buf, 0, sizeof(p10buf));
	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((pkcsLen = UnionSRJ1401CmdES824(vkIndex, algID, countryName, stateOrProvinceName, localityName, organizationName,
                        		organizationalUnitName, commonName, email, p10buf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionSRJ1401CmdES824,ret = [%d]!\n",pkcsLen);
			return(pkcsLen);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8ED9:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("非法的加密机指令类型");
		return(errCodeParameter);
	}

	//查询文件存储路径
        memset(sql,0,sizeof(sql));
        sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'","CERT");
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionSelectRealDBRecord[%s]!\n",sql);
                return(ret);
        }
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }

        // 读取文件存储目录
        memset(fileStoreDir,0,sizeof(fileStoreDir));
        if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
                return(ret);
        }

	// 设置下载证书名称
	memset(fullTime, 0, sizeof(fullTime));
	UnionGetFullSystemDateTime(fullTime);
	snprintf(fileName, sizeof(fileName), "%s.req", fullTime);

	memset(filePath,0,sizeof(filePath));
        UnionFormFullFileName(fileStoreDir,fileName,filePath);

	// 将证书内容写入文件中
	if ((fp = fopen(filePath, "wb")) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED9:: fopen[%s]!\n",filePath);
                return(errCodeUseOSErrCode);
	}
	if ((ret = fwrite(p10buf, 1, pkcsLen, fp)) != pkcsLen)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED9:: fwrite[%s]!\n",filePath);
		fclose(fp);
                return(errCodeUseOSErrCode);
	}
	fclose(fp);

	// 设置证书名称
	if ((ret = UnionSetResponseXMLPackageValue("body/fileName",fileName)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileName");
                return(ret);
        }

	// 设置文件类型
	if ((ret = UnionSetResponseXMLPackageValue("body/fileTypeID","CERT")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED9:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileTypeID");
                return(ret);
        }

	return(0);
}

