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
  服务名:       生成SM2证书请求文件,通过证书DN生成
  功能描述:     生成SM2证书请求文件,通过证书DN生成
		不支持索引,密钥必须存放密钥库
 ***************************************/
//static int setVKKeyIndexUnused(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB);
//int UnionGenerateSM2Key(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB);

int UnionDealServiceCode8EE0(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		hashID;
	char		pk[512];
	char		remark[128];
	char		fileName[128];
	char		fileFullName[512];
	char		path[512];
	char		tmpBuf[128];
	int		len;
	char		sql[2048];
	char		sign[2048];
	char		certDN[1024];
	char		fieldSeperate;
	char		tmpVarStr[32][128];
	int		varNum;
	int		i;
	int		isFstOU = 0;
	int		isFstO = 0;
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
		UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyName");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: keyName 不能为空\n");
		UnionSetResponseRemark("密钥名不能为空");
		return(errCodeParameter);
	}
	asymmetricKeyDB.keyName[ret] = 0;
	
	// 读取密钥信息
	if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 1, &asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n", asymmetricKeyDB.keyName);
		return(ret);
	}
	
	// 检测私钥类型
	if (asymmetricKeyDB.keyType != 1 && asymmetricKeyDB.keyType != 2)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: keyType[%d] is not 1 or 2!\n", asymmetricKeyDB.keyType);
		UnionSetResponseRemark("私钥类型[%d]不支持加密,必须为[1或2]", asymmetricKeyDB.keyType);
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	
	snprintf(pk, sizeof(pk), "04%s", asymmetricKeyDB.pkValue);
	vkLen = aschex_to_bcdhex(asymmetricKeyDB.vkValue, strlen(asymmetricKeyDB.vkValue), vkValue);
	
	// 请求文件名
	if((ret = UnionReadRequestXMLPackageValue("body/reqFileName", fileName, sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionReadRequestXMLPackageValue[%s]!\n", "body/reqFileName");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE1:: UnionReadRequestXMLPackageValue[%s]!\n", "body/reqFileName");
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
		UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}
	UnionReadDirFromStr(ptr,-1,path);

	//证书请求文件名(全路径)
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", path, fileName);

	//证书DN
	if((ret = UnionReadRequestXMLPackageValue("body/certDN", certDN, sizeof(certDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionReadRequestXMLPackageValue[%s]\n", "body/certDN");
		return(ret);
	}
	certDN[ret] = 0;
	
	if((varNum = UnionSeprateVarStrIntoVarGrp(certDN, ret, fieldSeperate, tmpVarStr, 32)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionSeprateVarStrIntoVarGrp[%s]\n", certDN);
		return(varNum);
	}
	if(varNum == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: no var in certDN\n");
		UnionSetResponseRemark("证书DN有错");
		return(errCodeParameter);
	}
	
	for(i = 0; i < varNum; i++)
	{
		if((ptr = strchr(tmpVarStr[i], '=')) != NULL)
		{
			*ptr = 0;
			if(strcasecmp(tmpVarStr[i], "CN") == 0)
			{
				strcpy(reqUserInfo.commonName, ptr+1);
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "E") == 0)
			{
				strcpy(reqUserInfo.email, ptr+1);
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "OU") == 0)
			{
				if(isFstOU)
					strcpy(reqUserInfo.organizationalUnitName, ptr+1);
				else
				{
					strcpy(reqUserInfo.organizationalUnitName2, ptr+1);
					isFstOU = 1;
				}
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "O") == 0)
			{
				if(isFstO)
					strcpy(reqUserInfo.organizationName, ptr+1);
				else
				{
					strcpy(reqUserInfo.organizationName2, ptr+1);
					isFstO = 1;
				}
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "L") == 0)
			{
				strcpy(reqUserInfo.localityName, ptr+1);
				continue;
			}
		
			if(strcasecmp(tmpVarStr[i], "S") == 0)
			{
				strcpy(reqUserInfo.stateOrProvinceName, ptr+1);
				continue;
			}	

			if(strcasecmp(tmpVarStr[i], "C") == 0)
			{
				strcpy(reqUserInfo.countryName, ptr+1);
				continue;
			}
		}
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
                UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionGenerateSM2PKCS10Ex vkValue=[%s] vkLen[%d]!\n", vkValue, vkLen);
                return(ret);
        }
	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: fopen fileName[%s]!\n", fileFullName);
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
		UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	if((ret = UnionSetResponseXMLPackageValue("file/filename", fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE0:: UnionSetResponseXMLPackageValue[%s]\n", "file/filename");
		return(ret);
	}
	return(0);
}
