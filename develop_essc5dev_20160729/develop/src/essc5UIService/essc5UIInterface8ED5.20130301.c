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
#include "unionREC.h"
#include "unionRecFile.h"
#include "base64.h"

/***************************************
  服务代码:	8ED5
  服务名:	上传PFX证书(630)目前只支持RSA证书
  功能描述:	上传PFX证书(630)目前只支持RSA证书
 ***************************************/
int UnionDealServiceCode8ED5(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			tmpBuf[128];
	char			appName[128];
	char			bankName[128];
	char			cert[4096];
	char			certDer[4096];
	char			hsmGrpID[128];
	char			fileTypeID[16];
	int			certLen;
	char			fileName[128];
	char			fileStoreDir[512];
	char			sql[1024];
	char			fullFileName[128];
	char			vkPassword[128];
	int			upload;
	int			vkIndex;
	int			keyType;
	FILE			*fp;

	TUnionHsmGroupRec	pSpecHsmGroup;

	// 文件名
        memset(fileName,0,sizeof(fileName));
        if ((ret = UnionReadRequestXMLPackageValue("body/fileName",fileName,sizeof(fileName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileName");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(fileName);
        if (strlen(fileName) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: fileName can not be null!\n");
                UnionSetResponseRemark("上传文件名不能为空");
                return(errCodeFileTransSvrNoFileName);
        }

	// 读取文件类型
        memset(fileTypeID,0,sizeof(fileTypeID));
        if ((ret = UnionReadRequestXMLPackageValue("body/fileTypeID",fileTypeID,sizeof(fileTypeID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(fileTypeID);
        if (strlen(fileTypeID) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: fileTypeID can not be null!\n");
                UnionSetResponseRemark("上传文件类型不能为空");
                return(errCodeParameter);
        }

        //查询文件存储路径
        memset(sql,0,sizeof(sql));
        sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'",fileTypeID);
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionSelectRealDBRecord[%s]!\n",sql);
                return(ret);
        }
        else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: fileTypeID[%s]!\n",fileTypeID);
                return(errCodeDatabaseMDL_RecordNotFound);
        }
	
        if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }

        // 读取文件存储目录
        memset(fileStoreDir,0,sizeof(fileStoreDir));
        if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) <= 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
                return(ret);
        }

	memset(fullFileName,0,sizeof(fullFileName));
        UnionFormFullFileName(fileStoreDir,fileName,fullFileName);

	// 检查文件是否存在
        if (access(fullFileName,0) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: fullFileName[%s] not found!\n",fullFileName);
                return(errCodeMngSvrFileNotExists);
        }

	// 读取证书内容(DER编码)
	if ((fp = fopen(fullFileName, "rb")) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED5:: fopen[%s]!\n",fullFileName);
                return(errCodeUseOSErrCode);
	}
	if ((certLen = fread(certDer, 1, sizeof(certDer) - 1, fp)) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED5:: fread[%s]!\n",fullFileName);
                return(errCodeUseOSErrCode);
	}
	fclose(fp);
	certDer[certLen] = 0;

	// 证书数据(base64)
	to64frombits((unsigned char *)cert, (unsigned char *)certDer, certLen);
	certLen = strlen(cert);

	// 证书应用名
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/appName");
		return(ret);
	}
	appName[ret] = 0;

	// 机构名
	if ((ret = UnionReadRequestXMLPackageValue("body/bankName",bankName,sizeof(bankName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankName");
		return(ret);
	}
	bankName[ret] = 0;

	// 私钥索引
	if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
		return(ret);
	}
	tmpBuf[ret] = 0;
	vkIndex = atoi(tmpBuf);
	if (vkIndex <= 0 || vkIndex > 1024)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: vkIndex[%d] error!\n",vkIndex);
		return(errCodeParameter);
	}

	// 密钥类型  1-SM2  0-RSA
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	keyType = atoi(tmpBuf);
	if (keyType != 0 && keyType != 1)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: keyType[%d] error!\n",keyType);
		return(errCodeParameter);
	}

	// 私钥访问码
	if ((ret = UnionReadRequestXMLPackageValue("body/vkPassword",vkPassword,sizeof(vkPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkPassword");
		return(ret);
	}
	vkPassword[ret] = 0;

	// 读取密码机组ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// 获取并设置使用的密码机组
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED5:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("加密机指令类型不匹配");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((ret = UnionSRJ1401CmdES630(appName,bankName,vkIndex,keyType,vkPassword,(unsigned char *)cert, certLen, &upload)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionSRJ1401CmdES630,ret = [%d]!\n",ret);
			return(ret);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8ED5:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("非法的加密机指令类型");
		return(errCodeParameter);
	}

	// 设置上传结果 1-证书已存在 2-私钥索引无效或被占用 0-上传成功
        snprintf(tmpBuf, sizeof(tmpBuf), "%d",upload);
        if ((ret = UnionSetResponseXMLPackageValue("body/upload",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED5:: UnionSetResponseXMLPackageValue[%s]!\n","body/upload");
                return(ret);
        }

	return(0);
}

