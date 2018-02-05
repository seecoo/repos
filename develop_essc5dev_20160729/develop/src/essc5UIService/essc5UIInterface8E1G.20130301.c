//      Author:         zhouxw
//      Copyright:      Union Tech. Guangzhou
//      Date:           20151202

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
#include "unionCertFunSM2.h"
#include "unionREC.h"
#include "asymmetricKeyDB.h"
#include "unionRSA.h"
#include "base64.h"

/***************************************
  服务代码:     8E1G
  服务名:    	导入根证书
  功能描述:     导入根证书
 ***************************************/

/*
 * 将字符串空格转为下划线
 *
 */
int UnionAlterAllBlankcharsWithUnderline(char *inStr, int lenOfInStr, char *outStr, int sizeOfOutStr)
{
	int	i = 0;
	outStr[0] = 0;
		
	if(lenOfInStr >= sizeOfOutStr)
	{	
		UnionUserErrLog("in UnionAlterAllBlankcharsWithUnderline:: buffer too small!\n");
		return(errCodeParameter);
	}
	
	for(i = 0; i < lenOfInStr; i ++)
	{
		if(*(inStr + i) == 0x20)
			*(outStr+i) = 0x5f;
		else
			*(outStr+i) = *(inStr+i);
	}
	outStr[i] = 0;
	return(i);
}

/*
 * 获取证书DN内容
 * 以'-'连接有值部分
 */
int UnionGetUserDNInSpecFormFromX509Cer(PUnionX509CerSM2 pX509Cer, char *userDN, int sizeOfUserDN)
{
	int	len = 0;
	int	lenOfUserDN = 0;

	if (pX509Cer == NULL || userDN == NULL || sizeOfUserDN == 0)
	{
		UnionUserErrLog("in UnionGetSpecFormDNFromX509Cer:: parameter error!\n");
		return(errCodeParameter);
	}
	
	if(pX509Cer->userInfo.commonName != NULL && strlen(pX509Cer->userInfo.commonName) != 0)
	{
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.commonName, strlen(pX509Cer->userInfo.commonName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN, sizeOfUserDN, "%s", pX509Cer->userInfo.commonName);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.email != NULL && strlen(pX509Cer->userInfo.email) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.email, strlen(pX509Cer->userInfo.email), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.email);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.organizationalUnitName != NULL && strlen(pX509Cer->userInfo.organizationalUnitName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.organizationalUnitName, strlen(pX509Cer->userInfo.organizationalUnitName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.organizationalUnitName);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.organizationalUnitName2 != NULL && strlen(pX509Cer->userInfo.organizationalUnitName2) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.organizationalUnitName2, strlen(pX509Cer->userInfo.organizationalUnitName2), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.organizationalUnitName2);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.organizationName != NULL && strlen(pX509Cer->userInfo.organizationName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.organizationName, strlen(pX509Cer->userInfo.organizationName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.organizationName);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.organizationName2 != NULL && strlen(pX509Cer->userInfo.organizationName2) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.organizationName2, strlen(pX509Cer->userInfo.organizationName2), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.organizationName2);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.localityName != NULL && strlen(pX509Cer->userInfo.localityName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.localityName, strlen(pX509Cer->userInfo.localityName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.localityName);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.stateOrProvinceName != NULL && strlen(pX509Cer->userInfo.stateOrProvinceName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.stateOrProvinceName, strlen(pX509Cer->userInfo.stateOrProvinceName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.stateOrProvinceName);
		lenOfUserDN += len;
	}

	if(pX509Cer->userInfo.countryName != NULL && strlen(pX509Cer->userInfo.countryName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->userInfo.countryName, strlen(pX509Cer->userInfo.countryName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->userInfo.countryName);
		lenOfUserDN += len;
	}
	
	return(lenOfUserDN);
}

/*
* 初始化密钥属性
*/
int UnionInitAsymmetricKeyDB(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB, int algID, int creatorType)
{
	int			ret = 0;
	if(pUnionAsymmetricKeyDB == NULL || (algID != conAsymmetricAlgorithmIDOfRSA && algID != conAsymmetricAlgorithmIDOfSM2))
	{
		UnionUserErrLog("in UnionInitAsymmetricKeyDB:: parameter error!\n");
		return(errCodeParameter);
	}
	
	// 密钥组
	strcpy(pUnionAsymmetricKeyDB->keyGroup, "default");
	// 算法标识
	pUnionAsymmetricKeyDB->algorithmID = algID;
	// 密钥类型
	pUnionAsymmetricKeyDB->keyType = 0;
	// 私钥存储位置
	pUnionAsymmetricKeyDB->vkStoreLocation = 0;
	// 允许使用旧密钥
	pUnionAsymmetricKeyDB->oldVersionKeyIsUsed = 1;
	// 创建者类型
	pUnionAsymmetricKeyDB->creatorType = creatorType;
	// 创建者
	if(creatorType == conAsymmetricCreatorTypeOfUser)
	{
		if((ret = UnionReadRequestXMLPackageValue("head/userID", pUnionAsymmetricKeyDB->creator, sizeof(pUnionAsymmetricKeyDB->creator))) < 0)
		{
			UnionUserErrLog("in UnionInitAsymmetricKeyDB:: UnionReadRequestXMLPackageValue[%s]!\n", "head/userID");
			return(ret);
		}
	}	
	else if (creatorType == conAsymmetricCreatorTypeOfApp)
	{
		if((ret = UnionReadRequestXMLPackageValue("head/appID", pUnionAsymmetricKeyDB->creator, sizeof(pUnionAsymmetricKeyDB->creator))) < 0)
		{
			UnionUserErrLog("in UnionInitAsymmetricKeyDB:: UnionReadRequestXMLPackageValue[%s]!\n", "head/appID");
			return(ret);
		}
	}
	pUnionAsymmetricKeyDB->status = conAsymmetricKeyStatusOfEnabled;

	return 0;
}

int UnionDealServiceCode8E1G(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		len = 0;
	char		*ptr = NULL;
	char		rootCertName[512];
	char		filePath[512];
	char		fileFullName[512];
	FILE		*fp;
	char		cert[8192] = {0};
	int		certLen = 0;
	char		sql[8192];
	char		evalue[16];		
	char		baseCert[8192*2];
	char		dateTime[16];
	time_t		begin_t, end_t;
	char		sysFullDate[16];
	char		specUserDN[512];
	
	TUnionX509CerSM2		caCertInfo;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&caCertInfo, 0, sizeof(caCertInfo));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));

	UnionGetFullSystemDate(sysFullDate);
	
	//读取根证书文件路径
	memset(filePath,0,sizeof(filePath));
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}

	UnionReadDirFromStr(ptr,-1,filePath);
	
	//根证书文件名
	if((ret = UnionReadRequestXMLPackageValue("body/rootCertName", rootCertName, sizeof(rootCertName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionReadRequestXMLPackageValue[%s]\n", "body/rootCertName");
		return(ret);
	}
	rootCertName[ret] = 0;
	UnionFilterHeadAndTailBlank(rootCertName);
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", filePath, rootCertName);

	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: fopen [%s] failed\n", fileFullName);
		return(errCodeParameter);
	}
	
	//读根证书文件内容
	if((certLen = fread(cert, 1, sizeof(cert), fp)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: read file[%s] failed", "fileFullName");
		return(certLen);
	}
	else if(certLen == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: read file[%s] failed len=[%d]", "fileFullName", certLen);
		UnionSetResponseRemark("文件为空文件");
		return(errCodeParameter);
	}
	
	to64frombits((unsigned char*)baseCert, (unsigned char*)cert, certLen);
	
	UnionTrimPemHeadTail(cert, cert, "certificate");
	//获取根证书信息
	if ((ret = UnionGetCertificateInfoFromBufEx(cert, certLen, &caCertInfo)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionGetCertificateInfoFromBufEx cert[%s] ret[%d]", cert, ret);
		return(ret);
	}
	
	// 根证书有效期
	if(memcmp(sysFullDate, caCertInfo.startDateTime, 8) < 0 || memcmp(sysFullDate, caCertInfo.endDataTime, 8) > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: certificate out of date!\n");
		return(errCodeOffsetOfCertMDL_OutOfDate);
	}

	// 证书起始时间
	memcpy(asymmetricKeyDB.activeDate, caCertInfo.startDateTime, 8);
	snprintf(dateTime, sizeof(dateTime), "%s000000", asymmetricKeyDB.activeDate);
	
	if ((begin_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}

	// 证书结束时间
	memcpy(dateTime,caCertInfo.endDataTime,8);
	sprintf(dateTime+8,"%s","000000");
	dateTime[14] = 0;

	if ((end_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}
	
	if(memcmp(sysFullDate, caCertInfo.startDateTime, 8) < 0 || memcmp(sysFullDate, caCertInfo.endDataTime, 8) > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: certtificate out of date!\n");
		return(errCodeOffsetOfCertMDL_OutOfDate);
	}

	// 有效天数
	asymmetricKeyDB.effectiveDays = (end_t - begin_t)/60/60/24; 
	
	// 拼装根证书密钥名
	if((ret = UnionGetUserDNInSpecFormFromX509Cer(&caCertInfo, specUserDN, sizeof(specUserDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionGetUserDNInSpecFormFromX509Cer error!\n");
		return(ret);
	}
	
	if (!caCertInfo.algID)
	{
		snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s.RSARoot", specUserDN);
		//将DER格式转换成裸公钥                                                                   
        	if ((ret = UnionPKCSCerASN1DER_Decode(caCertInfo.derPK,asymmetricKeyDB.pkValue,evalue)) < 0) 
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionPKCSCerASN1DER_Decode[%s] [%d]!\n",caCertInfo.derPK,ret);

      		        return(ret);
        	}                                                                                         
        	                                                                                          
        	// 密钥长度                                                                               
        	asymmetricKeyDB.keyLen = strlen(asymmetricKeyDB.pkValue) * 4;                             
        	                                                                                          
        	// 公钥指数                                                                               
        	if (strcmp(evalue,"010001") == 0)                                                         
        	        asymmetricKeyDB.pkExponent = 65537;                                     
        	else if (atoi(evalue) == 3)
        	        asymmetricKeyDB.pkExponent = 3;                                                   
        	else                                                                                      
        	{                                                                                         
                	UnionUserErrLog("in UnionDealServiceCode8E1G:: evalue[%s] != 010001 or 3!\n",evalue);
                	return(ret);                                                                      
        	}
	}
	else
	{
		snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s.SM2Root", specUserDN);
		memcpy(asymmetricKeyDB.pkValue, caCertInfo.derPK+2, strlen(caCertInfo.derPK)-2);
		asymmetricKeyDB.keyLen = 256;
		asymmetricKeyDB.pkExponent = 0;
	}
	// 生成根证书对应的非对称密钥
	if((ret = UnionInitAsymmetricKeyDB(&asymmetricKeyDB, caCertInfo.algID, conAsymmetricCreatorTypeOfUser)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionInitAsymmetricKeyDB");
		return(ret);
	}
	if((ret = UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionGenerateAsymmetricKeyDBRec!\n");
		return(ret);
	}
	/*
	len = sprintf(issuerInfo,"国家名[%s]  省名[%s]  城市名[%s]  单位名[%s]  部门[%s]  用户名[%s]  EMail地址[%s]  别名[%s]",caCertInfo.issuerInfo.countryName,caCertInfo.issuerInfo.stateOrProvinceName,caCertInfo.issuerInfo.localityName,caCertInfo.issuerInfo.organizationName,caCertInfo.issuerInfo.organizationalUnitName,caCertInfo.issuerInfo.commonName,caCertInfo.issuerInfo.email,caCertInfo.issuerInfo.alias);
	issuerInfo[len] = 0;
	
	len = sprintf(userInfo,"国家名[%s]  省名[%s]  城市名[%s]  单位名[%s]  部门[%s]  用户名[%s]  EMail地址[%s]  别名[%s]",caCertInfo.userInfo.countryName,caCertInfo.userInfo.stateOrProvinceName,caCertInfo.userInfo.localityName,caCertInfo.userInfo.organizationName,caCertInfo.userInfo.organizationalUnitName,caCertInfo.userInfo.commonName,caCertInfo.userInfo.email,caCertInfo.userInfo.alias);
	userInfo[len] = 0;
	*/

	//将数据插入数据库
	len = snprintf(sql, sizeof(sql), "insert into certOfCnaps2(keyName, cerFlag, serialNumber, version, hashID, startDateTime, endDataTime, issuerInfo, userInfo, bankCert) values('%s', %d, '%s', %d, %d, '%s', '%s', '%s', '%s', '%s')", asymmetricKeyDB.keyName, 3, caCertInfo.serialNumberAscBuf, caCertInfo.version, caCertInfo.hashID, caCertInfo.startDateTime, caCertInfo.endDataTime, caCertInfo.issuserDN, caCertInfo.userDN, baseCert);
	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	
	return(0);
}
