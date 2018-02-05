//      Author:         zhouxw
//      Copyright:      Union Tech. Guangzhou
//      Date:           20150713

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
#include "unionCertFun.h"
#include "unionREC.h"
#include "unionRSA.h"
#include "base64.h"

/***************************************
  �������:     8E1H
  ������:    	�ϴ�����֤���ļ�
  ��������:     �ϴ�����֤���ļ�
 ***************************************/

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

int UnionDealServiceCode8E1H(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	char		bankID[64];
	char		pix[32];
	char		certVersion[32];
	char		fileName[512];
	//char		fileStoreDir[512];
	char		path[512];
	char		fileFullName[512];
	char		sql[19200];
	int		len;
	FILE		*fp;
	char		cert[8192] = {0};
	int		certLen = 0;
	//char		dataBuf[8192] = {0};
	char		remark[128] = {0};
	//char		derPK[8192];
	char		*ptr = NULL;
	//char		issuerInfo[1024];
	//char		userInfo[1024];

	char		evalue[16];
	char		baseCert[8192];
	char		buffer[8192];
	
	char		dateTime[16];
	time_t		begin_t, end_t;
	char		sysFullDate[16];
	//char		keyName[128];
	
	TUnionX509CerSM2	tCertInfo;
	TUnionAsymmetricKeyDB	asymmetricKeyDB;
	TUnionAsymmetricKeyDB	caAsymmetricKeyDB;
	
	UnionGetFullSystemDate(sysFullDate);
	
	//memset(&caCertInfo, 0, sizeof(caCertInfo));
	memset(&tCertInfo, 0, sizeof(tCertInfo));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	memset(&caAsymmetricKeyDB, 0, sizeof(caAsymmetricKeyDB));
	
	// ��ȡ������
	if((ret = UnionReadRequestXMLPackageValue("body/bankID", bankID, sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/bankID");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/bankID");
		UnionSetResponseRemark("�����Ų���Ϊ��");
		return(errCodeParameter);
	}
	else
		bankID[ret] = 0;
	
	// ��ȡӦ��ID
	if((ret = UnionReadRequestXMLPackageValue("body/pix", pix, sizeof(pix))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/pix");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/pix");
		UnionSetResponseRemark("Ӧ��ID����Ϊ��");
		return(errCodeParameter);
	}
	else
		pix[ret] = 0;
	
	// ��ȡ�汾��
	if((ret = UnionReadRequestXMLPackageValue("body/certVersion", certVersion, sizeof(certVersion))) <= 0)
	{
		memcpy(certVersion, "01", 2);
	}
	else
		certVersion[ret] = 0;

	//��ȡ֤��洢·��
	if((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionReadStringTypeRECVar[%s]!\n", "defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}
	UnionReadDirFromStr(ptr, -1, path);
	
	//�ļ���
	if((ret = UnionReadRequestXMLPackageValue("body/cerFileName", fileName, sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionReadRequestXMLPackageValue[%s]\n", "body/cerFileName`");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", path, fileName);

	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: fopen [%s] failed\n", fileFullName);
		return(errCodeParameter);
	}
	
	//��֤���ļ�����
	if((certLen = fread(cert, 1, sizeof(cert), fp)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: read file[%s] failed", "fileFullName");
		return(certLen);
	}
	else if(certLen == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: read file[%s] failed len=[%d]", "fileFullName", certLen);
		UnionSetResponseRemark("�ļ�Ϊ���ļ�");
		return(errCodeParameter);
	}
	
	//����֤��
        if ((ret = UnionGetCertificateInfoFromBufEx((char *)cert,certLen,&tCertInfo)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1H:: cert[%d][%s], ret = [%d]!\n", certLen, cert, ret);
                return(ret);
        }
	// �ж�֤����Ч��
	if(memcmp(sysFullDate, tCertInfo.startDateTime, 8) < 0 || memcmp(sysFullDate, tCertInfo.endDataTime, 8) > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: certtificate out of date!\n");
		return(errCodeOffsetOfCertMDL_OutOfDate);
	}
	
	// ��֤����֤��Կ֤��
	
	// ƴװ��Կ��
	switch(tCertInfo.algID)
	{
		case	0:
			snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer", pix, bankID, certVersion);
		
			if((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 0, &asymmetricKeyDB)) > 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionReadAsymmetricKeyDBRec keyName[%s] already exist!\n", asymmetricKeyDB.keyName);
				return(errCodePKDBMDL_KeyAlreadyExists);
			}
		
			//��Կ
			if ((ret = UnionPKCSCerASN1DER_Decode(tCertInfo.derPK,asymmetricKeyDB.pkValue,evalue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionPKCSCerASN1DER_Decode[%s] [%d]!\n",tCertInfo.derPK,ret);
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
				UnionUserErrLog("in UnionDealServiceCode8E1H:: evalue[%s] != 010001 or 3!\n",evalue);
				return(ret);
			}
			break;
		case	1:
			snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer_sm2", pix, bankID, certVersion);
		
			if((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 0, &asymmetricKeyDB)) > 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionReadAsymmetricKeyDBRec keyName[%s] already exist!\n", asymmetricKeyDB.keyName);
				return(errCodePKDBMDL_KeyAlreadyExists);
			}
			
			// ��Կ
			memcpy(asymmetricKeyDB.pkValue, tCertInfo.derPK+2, 128);
			
			// ��Կ����
			asymmetricKeyDB.keyLen = 256;
			
			// ��Կָ��
			asymmetricKeyDB.pkExponent = 0;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCode8E1H:: cert algorithmID[%d] error\n", tCertInfo.algID);
			return(errCodeOffsetOfCertMDL_AlgorithmFlag);
	}		
	
	// ֤����ʼʱ��
	memcpy(asymmetricKeyDB.activeDate, tCertInfo.startDateTime, 8);
	snprintf(dateTime, sizeof(dateTime), "%s000000", asymmetricKeyDB.activeDate);
	
	if ((begin_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}

	// ֤�����ʱ��
	memcpy(dateTime,tCertInfo.endDataTime,8);
	sprintf(dateTime+8,"%s","000000");
	dateTime[14] = 0;

	if ((end_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}
	
	// ��Ч����
	asymmetricKeyDB.effectiveDays = (end_t - begin_t)/60/60/24; 
	
	// ��Կ״̬
	asymmetricKeyDB.status = conAsymmetricKeyStatusOfEnabled;
	
	// ������Կ
	if((ret = UnionInitAsymmetricKeyDB(&asymmetricKeyDB, tCertInfo.algID, conAsymmetricCreatorTypeOfUser)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionInitAsymmetricKeyDB!\n");
		return(ret);
	}
	
	if((ret = UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionGenerateAsymmetricKeyDBRec[%s]!\n", asymmetricKeyDB.keyName);
		return(ret);
	}

	to64frombits((unsigned char*)baseCert, (unsigned char*)cert, certLen);
	UnionPemStrPreHandle((char*)baseCert, (char*)buffer, "CERTIFICATE", 8192);
	//�����ݲ������ݿ�
	len = snprintf(sql, sizeof(sql), "insert into certOfCNAPS2(keyName, cerFlag, serialNumber, version, hashID, startDateTime, endDataTime, issuerInfo, userInfo, bankCert, remark) values('%s', 1, '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', '%s')", asymmetricKeyDB.keyName, tCertInfo.serialNumberAscBuf, tCertInfo.version, tCertInfo.hashID, tCertInfo.startDateTime, tCertInfo.endDataTime, tCertInfo.issuserDN, tCertInfo.userDN, buffer, remark);

	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1H:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	
	return(0);
}
