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
  �������:     8E1G
  ������:    	�����֤��
  ��������:     �����֤��
 ***************************************/

/*
 * ���ַ����ո�תΪ�»���
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
 * ��ȡ֤��DN����
 * ��'-'������ֵ����
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
* ��ʼ����Կ����
*/
int UnionInitAsymmetricKeyDB(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB, int algID, int creatorType)
{
	int			ret = 0;
	if(pUnionAsymmetricKeyDB == NULL || (algID != conAsymmetricAlgorithmIDOfRSA && algID != conAsymmetricAlgorithmIDOfSM2))
	{
		UnionUserErrLog("in UnionInitAsymmetricKeyDB:: parameter error!\n");
		return(errCodeParameter);
	}
	
	// ��Կ��
	strcpy(pUnionAsymmetricKeyDB->keyGroup, "default");
	// �㷨��ʶ
	pUnionAsymmetricKeyDB->algorithmID = algID;
	// ��Կ����
	pUnionAsymmetricKeyDB->keyType = 0;
	// ˽Կ�洢λ��
	pUnionAsymmetricKeyDB->vkStoreLocation = 0;
	// ����ʹ�þ���Կ
	pUnionAsymmetricKeyDB->oldVersionKeyIsUsed = 1;
	// ����������
	pUnionAsymmetricKeyDB->creatorType = creatorType;
	// ������
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
	
	//��ȡ��֤���ļ�·��
	memset(filePath,0,sizeof(filePath));
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}

	UnionReadDirFromStr(ptr,-1,filePath);
	
	//��֤���ļ���
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
	
	//����֤���ļ�����
	if((certLen = fread(cert, 1, sizeof(cert), fp)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: read file[%s] failed", "fileFullName");
		return(certLen);
	}
	else if(certLen == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: read file[%s] failed len=[%d]", "fileFullName", certLen);
		UnionSetResponseRemark("�ļ�Ϊ���ļ�");
		return(errCodeParameter);
	}
	
	to64frombits((unsigned char*)baseCert, (unsigned char*)cert, certLen);
	
	UnionTrimPemHeadTail(cert, cert, "certificate");
	//��ȡ��֤����Ϣ
	if ((ret = UnionGetCertificateInfoFromBufEx(cert, certLen, &caCertInfo)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionGetCertificateInfoFromBufEx cert[%s] ret[%d]", cert, ret);
		return(ret);
	}
	
	// ��֤����Ч��
	if(memcmp(sysFullDate, caCertInfo.startDateTime, 8) < 0 || memcmp(sysFullDate, caCertInfo.endDataTime, 8) > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: certificate out of date!\n");
		return(errCodeOffsetOfCertMDL_OutOfDate);
	}

	// ֤����ʼʱ��
	memcpy(asymmetricKeyDB.activeDate, caCertInfo.startDateTime, 8);
	snprintf(dateTime, sizeof(dateTime), "%s000000", asymmetricKeyDB.activeDate);
	
	if ((begin_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}

	// ֤�����ʱ��
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

	// ��Ч����
	asymmetricKeyDB.effectiveDays = (end_t - begin_t)/60/60/24; 
	
	// ƴװ��֤����Կ��
	if((ret = UnionGetUserDNInSpecFormFromX509Cer(&caCertInfo, specUserDN, sizeof(specUserDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionGetUserDNInSpecFormFromX509Cer error!\n");
		return(ret);
	}
	
	if (!caCertInfo.algID)
	{
		snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s.RSARoot", specUserDN);
		//��DER��ʽת�����㹫Կ                                                                   
        	if ((ret = UnionPKCSCerASN1DER_Decode(caCertInfo.derPK,asymmetricKeyDB.pkValue,evalue)) < 0) 
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionPKCSCerASN1DER_Decode[%s] [%d]!\n",caCertInfo.derPK,ret);

      		        return(ret);
        	}                                                                                         
        	                                                                                          
        	// ��Կ����                                                                               
        	asymmetricKeyDB.keyLen = strlen(asymmetricKeyDB.pkValue) * 4;                             
        	                                                                                          
        	// ��Կָ��                                                                               
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
	// ���ɸ�֤���Ӧ�ķǶԳ���Կ
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
	len = sprintf(issuerInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",caCertInfo.issuerInfo.countryName,caCertInfo.issuerInfo.stateOrProvinceName,caCertInfo.issuerInfo.localityName,caCertInfo.issuerInfo.organizationName,caCertInfo.issuerInfo.organizationalUnitName,caCertInfo.issuerInfo.commonName,caCertInfo.issuerInfo.email,caCertInfo.issuerInfo.alias);
	issuerInfo[len] = 0;
	
	len = sprintf(userInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",caCertInfo.userInfo.countryName,caCertInfo.userInfo.stateOrProvinceName,caCertInfo.userInfo.localityName,caCertInfo.userInfo.organizationName,caCertInfo.userInfo.organizationalUnitName,caCertInfo.userInfo.commonName,caCertInfo.userInfo.email,caCertInfo.userInfo.alias);
	userInfo[len] = 0;
	*/

	//�����ݲ������ݿ�
	len = snprintf(sql, sizeof(sql), "insert into certOfCnaps2(keyName, cerFlag, serialNumber, version, hashID, startDateTime, endDataTime, issuerInfo, userInfo, bankCert) values('%s', %d, '%s', %d, %d, '%s', '%s', '%s', '%s', '%s')", asymmetricKeyDB.keyName, 3, caCertInfo.serialNumberAscBuf, caCertInfo.version, caCertInfo.hashID, caCertInfo.startDateTime, caCertInfo.endDataTime, caCertInfo.issuserDN, caCertInfo.userDN, baseCert);
	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	
	return(0);
}
