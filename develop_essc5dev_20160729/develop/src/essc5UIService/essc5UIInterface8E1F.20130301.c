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
#include "unionREC.h"

/***************************************
  �������:     8E1F
  ������:    	�ϴ�֤���ļ�
  ��������:     �ϴ�֤���ļ�
 ***************************************/
int UnionDealServiceCode8E1F(PUnionHsmGroupRec phsmGroupRec)
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
	char		remark[128];
	char		derPK[144];
	char		*ptr = NULL;
	//char		issuerInfo[1024];
	//char		userInfo[1024];
	
	char		dateTime[16];
	time_t		begin_t, end_t;
	char		sysFullDate[16];
	
	//TUnionX509CerSM2		caCertInfo;
	TUnionX509CerSM2		tCertInfo;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionAsymmetricKeyDB		caAsymmetricKeyDB;
	
	//memset(&caCertInfo, 0, sizeof(caCertInfo));
	memset(&tCertInfo, 0, sizeof(tCertInfo));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	memset(&caAsymmetricKeyDB, 0, sizeof(caAsymmetricKeyDB));
	
	UnionGetFullSystemDate(sysFullDate);
	
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
	snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer_sm2", pix, bankID, certVersion);
	
	if((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 0, &asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n", asymmetricKeyDB.keyName);
		return(ret);
	}
	
	//��ȡ֤��洢·��
	if((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionReadStringTypeRECVar[%s]!\n", "defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}
	UnionReadDirFromStr(ptr, -1, path);
	
	//�ļ���
	if((ret = UnionReadRequestXMLPackageValue("body/pfxFileName", fileName, sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionReadRequestXMLPackageValue[%s]\n", "body/pfxFileName`");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", path, fileName);

	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: fopen [%s] failed\n", fileFullName);
		return(errCodeParameter);
	}
	
	//��֤���ļ�����
	if((certLen = fread(cert, 1, sizeof(cert), fp)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: read file[%s] failed", "fileFullName");
		return(certLen);
	}
	else if(certLen == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: read file[%s] failed len=[%d]", "fileFullName", certLen);
		UnionSetResponseRemark("�ļ�Ϊ���ļ�");
		return(errCodeParameter);
	}
	
	UnionTrimPemHeadTail((char *)cert, (char *)cert, "CERTIFICATE");
	//����֤��
        if ((ret = UnionGetCertificateInfoFromBufEx((char *)cert,certLen,&tCertInfo)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1F:: cert[%d][%s], ret = [%d]!\n", certLen, cert, ret);
                return(ret);
        }
	/*
	len = sprintf(issuerInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]", tCertInfo.issuerInfo.countryName, tCertInfo.issuerInfo.stateOrProvinceName, tCertInfo.issuerInfo.localityName, tCertInfo.issuerInfo.organizationName, tCertInfo.issuerInfo.organizationalUnitName, tCertInfo.issuerInfo.commonName, tCertInfo.issuerInfo.email, tCertInfo.issuerInfo.alias);
	issuerInfo[len] = 0;
	len = sprintf(userInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]", tCertInfo.userInfo.countryName, tCertInfo.userInfo.stateOrProvinceName, tCertInfo.userInfo.localityName, tCertInfo.userInfo.organizationName, tCertInfo.userInfo.organizationalUnitName, tCertInfo.userInfo.commonName, tCertInfo.userInfo.email, tCertInfo.userInfo.alias);
	userInfo[len] = 0;
	*/
	
	// �ж�֤����Ч��
	if(memcmp(sysFullDate, tCertInfo.startDateTime, 8) < 0 || memcmp(sysFullDate, tCertInfo.endDataTime, 8) > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: certificate expired!\n");
		return(errCodeOffsetOfCertMDL_OutOfDate);
	}
	
	// ֤�鹫Կ�����������ļ�ʱ�Ĺ�Կһ��
	if(memcmp(tCertInfo.derPK+2, asymmetricKeyDB.pkValue, 128) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: certPK[%s] and localKeyPK[%s] are not the same!\n", tCertInfo.derPK+2, asymmetricKeyDB.pkValue);
		UnionSetResponseRemark("֤�鹫Կ�뱾�ع�Կ��һ��");
		return(errCodeOffsetOfCertMDL_VerifyCert);
	}

	// �ø�֤��Ĺ�Կ��֤��
	len = snprintf(sql, sizeof(sql), "select keyName from certOfCnaps2 where hashID = 2 and cerFlag = 3 and userInfo = '%s'", tCertInfo.issuserDN);
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionSelectRealDBRecord[%s]!\n", sql);
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: rootCert[%s] record not found!\n", tCertInfo.issuserDN);
		return(errCodeOffsetOfCertMDL_CaNotExist);
	}
	UnionLocateXMLPackage("detail", 1);
	if((ret = UnionReadXMLPackageValue("keyName", caAsymmetricKeyDB.keyName, sizeof(caAsymmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionReadXMLPackageValue[%s]!\n", "keyName");
		return(ret);
	}
	
	if((ret = UnionReadAsymmetricKeyDBRec(caAsymmetricKeyDB.keyName, 0, &caAsymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionReadAsymmetricKeyDBRec[%s]!\n", caAsymmetricKeyDB.keyName);	
		return(ret);
	}
	snprintf(derPK, sizeof(derPK), "04%s", caAsymmetricKeyDB.pkValue);
	//��֤��Ĺ�Կ��֤֤��
	if ((ret = UnionVerifyCertificateWithPKEx(derPK, (char *)cert, certLen)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret = [%d]!\n",derPK, certLen, cert, ret);
                return(errCodeOffsetOfCertMDL_VerifyCert);
	}
        

	if((ret = UnionReadRequestXMLPackageValue("body/remark", remark, sizeof(remark))) <= 0)
		memset(remark, 0, sizeof(remark));
	else
		remark[ret] = 0;

	// ������Կ����
	// ֤����ʼʱ��
	memcpy(asymmetricKeyDB.activeDate, tCertInfo.startDateTime, 8);
	snprintf(dateTime, sizeof(dateTime), "%s000000", asymmetricKeyDB.activeDate);
	
	if ((begin_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE402:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}

	// ֤�����ʱ��
	memcpy(dateTime,tCertInfo.endDataTime,8);
	sprintf(dateTime+8,"%s","000000");
	dateTime[14] = 0;

	if ((end_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE402:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}

	// ��Ч����
	asymmetricKeyDB.effectiveDays = (end_t - begin_t)/60/60/24; 
	
	// ��Կ״̬
	asymmetricKeyDB.status = conAsymmetricKeyStatusOfEnabled;
	
	len = snprintf(sql, sizeof(sql), "update asymmetricKeyDB set activeDate = '%s', effectiveDays = '%d', status = %d where keyName = '%s'", asymmetricKeyDB.activeDate, asymmetricKeyDB.effectiveDays, asymmetricKeyDB.status, asymmetricKeyDB.keyName);
	sql[len] = 0;
	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionExecRealDBSql sql[%s] ret[%d]!\n", sql, ret);
		return(ret);
	}

	//�����ݲ������ݿ�
	len = snprintf(sql, sizeof(sql), "insert into certOfCNAPS2(keyName, cerFlag, serialNumber, version, hashID, startDateTime, endDataTime, issuerInfo, userInfo, bankCert, remark) values('%s', 0, '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', '%s')", asymmetricKeyDB.keyName, tCertInfo.serialNumberAscBuf, tCertInfo.version, tCertInfo.hashID, tCertInfo.startDateTime, tCertInfo.endDataTime, tCertInfo.issuserDN, tCertInfo.userDN, cert, remark);

	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	
	return(0);
}
