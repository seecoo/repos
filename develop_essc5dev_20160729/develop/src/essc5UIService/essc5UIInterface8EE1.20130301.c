//      Author:         zhouxw
//      Copyright:      Union Tech. Guangzhou
//      Date:           20160713

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "essc5UIService.h"
#include "UnionLog.h"
#include "unionXMLPackage.h"
#include "unionErrCode.h"
#include "unionRealBaseDB.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"
#include "asymmetricKeyDB.h"
#include "unionCertFunSM2.h"
#include "unionREC.h"

/***************************************
  服务代码:     8EE0
  服务名:       生成SM2证书请求文件,通过每个域填充
  功能描述:     生成SM2证书请求文件,通过每个域填充
		不支持索引,密钥必须存放密钥库
 ***************************************/
//static int setVKKeyIndexUnused(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB);
//int UnionGenerateSM2Key(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB);

int UnionDealServiceCode8EE1(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		hashID;
	char		pk[512];
	char		remark[128];
	char		fileName[128];
	char		fileFullName[512];
	char		path[512];
	char		tmpBuf[128];
	int		len = 0;
	char		sql[2048];
	char		sign[2048];
	char		certDN[1024] = {0};
	char		vkValue[1024];	
	int		vkLen = 0;
	
	char		*ptr = NULL;
	char		reqContent[8192*2];
	FILE		*fp = NULL;

	TUnionX509UserInfoSM2		reqUserInfo;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&reqUserInfo, 0, sizeof(reqUserInfo));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));

	// 读取密钥名
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName", asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyName");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: keyName 不能为空\n");
		UnionSetResponseRemark("密钥名不能为空");
		return(errCodeParameter);
	}
	asymmetricKeyDB.keyName[ret] = 0;
	
	// 读取密钥信息
	if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 1, &asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n", asymmetricKeyDB.keyName);
		return(ret);
	}
	
	// 检测私钥类型
	if (asymmetricKeyDB.keyType != 1 && asymmetricKeyDB.keyType != 2)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: keyType[%d] is not 1 or 2!\n", asymmetricKeyDB.keyType);
		UnionSetResponseRemark("私钥类型[%d]不支持加密,必须为[1或2]", asymmetricKeyDB.keyType);
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	
	snprintf(pk, sizeof(pk), "04%s", asymmetricKeyDB.pkValue);
	vkLen = aschex_to_bcdhex(asymmetricKeyDB.vkValue, strlen(asymmetricKeyDB.vkValue), vkValue);
	
	// 请求文件名
	if((ret = UnionReadRequestXMLPackageValue("body/fileName", fileName, sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionReadRequestXMLPackageValue[%s]!\n", "body/fileName");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionReadRequestXMLPackageValue[%s]!\n", "body/fileName");
		UnionSetResponseRemark("请求文件名不能为空");
		return(errCodeParameter);
	}
	else
	{
		UnionFilterHeadAndTailBlank(fileName);
		fileName[ret] = 0;
		strcat(fileName, ".csr");
	}
		
	//读取目录
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}
	UnionReadDirFromStr(ptr,-1,path);

	//证书请求文件名(全路径)
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", path, fileName);
	
	// EMail地址
	if((ret = UnionReadRequestXMLPackageValue("body/email", reqUserInfo.email, sizeof(reqUserInfo.email))) > 0)
	{
		reqUserInfo.email[ret] = 0;
		len += snprintf(certDN, sizeof(certDN), "E=%s,", reqUserInfo.email);
	}
	
	// 用户名
	if((ret = UnionReadRequestXMLPackageValue("body/commonName", reqUserInfo.commonName, sizeof(reqUserInfo.commonName))) > 0)
	{
		reqUserInfo.commonName[ret] =0;
		len += snprintf(certDN+len, sizeof(certDN)-len, "CN=%s,", reqUserInfo.commonName);
	}
	
	// 部门名
	if((ret = UnionReadRequestXMLPackageValue("body/organizationalUnitName", reqUserInfo.organizationalUnitName, sizeof(reqUserInfo.organizationalUnitName))) > 0)
	{
		reqUserInfo.organizationalUnitName[ret] = 0;
		len += snprintf(certDN+len, sizeof(certDN)-len, "OU=%s,", reqUserInfo.organizationalUnitName);
	}
	
	// 单位名
	if((ret = UnionReadRequestXMLPackageValue("body/organizationName", reqUserInfo.organizationName, sizeof(reqUserInfo.organizationName))) > 0)
	{
		reqUserInfo.organizationName[ret] = 0;
		len += snprintf(certDN+len, sizeof(certDN)-len, "O=%s,", reqUserInfo.organizationName);
	}
	
	// 城市名
	if((ret = UnionReadRequestXMLPackageValue("body/localityName", reqUserInfo.localityName, sizeof(reqUserInfo.localityName))) > 0)
	{
		reqUserInfo.localityName[ret] = 0;
		len += snprintf(certDN+len, sizeof(certDN)-len, "L=%s,", reqUserInfo.localityName);
	}
	
	// 省名
	if((ret = UnionReadRequestXMLPackageValue("body/stateOrProvinceName", reqUserInfo.stateOrProvinceName, sizeof(reqUserInfo.stateOrProvinceName))) > 0)
	{
		reqUserInfo.stateOrProvinceName[ret] = 0;
		len += snprintf(certDN+len, sizeof(certDN)-len, "ST=%s,", reqUserInfo.stateOrProvinceName);
	}

	// 国家名
	if((ret = UnionReadRequestXMLPackageValue("body/countryName", reqUserInfo.countryName, sizeof(reqUserInfo.countryName))) > 0)
	{
		reqUserInfo.countryName[ret] = 0;
		len = snprintf(certDN+len, sizeof(certDN)-len, "C=%s", reqUserInfo.countryName);
	}
	
	//摘要算法标识
	if((ret = UnionReadRequestXMLPackageValue("body/hashID", tmpBuf, sizeof(tmpBuf))) <= 0)
		hashID = 2;
	else
	{
		tmpBuf[ret] = 0;
		hashID = atoi(tmpBuf);
	}
	
	//备注
	if((ret = UnionReadRequestXMLPackageValue("body/remark", remark, sizeof(remark))) <= 0)
		memset(remark, 0, sizeof(remark));
	else
		remark[ret] = 0;
	
	//生成证书请求文件
	if ((ret = UnionGenerateSM2PKCS10Ex(-1, vkValue, vkLen, pk, &reqUserInfo, 0, fileFullName, sign)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionGenerateSM2PKCS10Ex vkValue=[%s] vkLen[%d]!\n", vkValue, vkLen);
                return(ret);
        }
	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: fopen fileName[%s]!\n", fileFullName);
		return(errCodeParameter);
	}
	len = fread(reqContent, 1, 8192*2, fp);
	fclose(fp);
	reqContent[len] = 0;
	
	//将数据插入数据库
	len = snprintf(sql, sizeof(sql), "insert into cert(keyName, cerFlag, hashID, userInfo, reqContent, fileName, remark) values('%s', %d, %d, '%s', '%s', '%s', '%s')", asymmetricKeyDB.keyName, 2, hashID, certDN, reqContent, fileName, remark);
	sql[len] = 0;
	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	if((ret = UnionSetResponseXMLPackageValue("file/filename", fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionSetResponseXMLPackageValue[%s]\n", "file/filename");
		return(ret);
	}
	return(0);
}
