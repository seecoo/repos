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

/***************************************
  �������:     8E31
  ������:    	�ϴ�֤���ļ�
  ��������:     �ϴ�֤���ļ�
 ***************************************/
int UnionDealServiceCode8E31(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	char		bankID[16];
	char		version[16];
	char		fileName[512];
	char		fileStoreDir[512];
	char		path[512];
	char		fileFullName[512];
	char		sql[19200];
	int		len;
	FILE		*fp;
	char		cert[8192] = {0};
	int		certLen = 0;
	char		dataBuf[8192] = {0};
	char		remark[128];
	char		derPK[144];
	
	TUnionX509CerSM2		caCertInfo;
	TUnionX509CerSM2		tCertInfo;
	
	memset(&caCertInfo, 0, sizeof(caCertInfo));
	memset(&tCertInfo, 0, sizeof(tCertInfo));
	
	//�����к�
	if((ret = UnionReadRequestXMLPackageValue("body/bankID", bankID, sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionReadRequestXMLPackageValue[%s]\n", "body/bankID");
		return(ret);
	}
	bankID[ret] = 0;
	
	//��ȡ֤��洢·��
	len = snprintf(sql, sizeof(sql), "select * from fileType where fileTypeID='CIP2'");
	sql[len] = 0;
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionSelectRealDBRecord sql[%s]\n", sql);
		return(ret);
	}
	else if(ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E31:: fileTypeID[CIP2]\n");
                return(errCodeDatabaseMDL_RecordNotFound);
        }
	
	if((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionLocateXMLPackage[%s]\n", "detail");
		return(ret);
	}
	
	if((ret = UnionReadXMLPackageValue("fileStoreDir", fileStoreDir, sizeof(fileStoreDir))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionReadXMLPackageValue[%s]\n", "fileStoreDir");
		return(ret);
	}
	fileStoreDir[ret] = 0;
	UnionReadDirFromStr(fileStoreDir, -1, path);
	
	//�ļ���
	if((ret = UnionReadRequestXMLPackageValue("body/fileName", fileName, sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionReadRequestXMLPackageValue[%s]\n", "body/fileName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", path, fileName);

	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: fopen [%s] failed\n", fileFullName);
		return(errCodeParameter);
	}
	
	//��֤���ļ�����
	if((certLen = fread(cert, 1, sizeof(cert), fp)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: read file[%s] failed", "fileFullName");
		return(certLen);
	}
	else if(certLen == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: read file[%s] failed len=[%d]", "fileFullName", certLen);
		UnionSetResponseRemark("�ļ�Ϊ���ļ�");
		return(errCodeParameter);
	}
	
	UnionTrimPemHeadTail((char *)cert, (char *)cert, "CERTIFICATE");
	
	//��ȡ��֤��洢·��
        len = snprintf(sql, sizeof(sql), "select * from fileType where fileTypeID='CIP3'");
        sql[len] = 0;
        if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E31:: UnionSelectRealDBRecord sql[%s]\n", sql);
                return(ret);
        }
	else if(ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E31:: fileTypeID[CIP3]\n");
                return(errCodeDatabaseMDL_RecordNotFound);
        }

        if((ret = UnionLocateXMLPackage("detail", 1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E31:: UnionLocateXMLPackage[%s]\n", "detail");
                return(ret);
        }

        if((ret = UnionReadXMLPackageValue("fileStoreDir", fileStoreDir, sizeof(fileStoreDir))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E31:: UnionReadXMLPackageValue[%s]\n", "fileStoreDir");
                return(ret);
        }
        fileStoreDir[ret] = 0;
        UnionReadDirFromStr(fileStoreDir, -1, path);	

	//��ȡ��֤���ļ�
	len = snprintf(fileFullName, sizeof(fileFullName), "%s/oca1sm2.cer", path);
	
	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCode8E31:: fopen [%s] failed\n", fileFullName);
                return(errCodeParameter);
        }
		
	len = fread(dataBuf, 1, 8192, fp);
	
	//��ȡ��֤����Ϣ
	if ((ret = UnionGetCertificateInfoFromBufEx(dataBuf,len,&caCertInfo)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionGetCertificateInfoFromBufEx cert[%s] ret[%d]", dataBuf, ret);
		return(ret);
	}
	//��֤��Ĺ�Կ��֤֤��
	if ((ret = UnionVerifyCertificateWithPKEx(caCertInfo.derPK, (char *)cert, certLen)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E31:: UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret = [%d]!\n",caCertInfo.derPK, len, dataBuf, ret);
                return(ret);
        }

	//����֤��
        if ((ret = UnionGetCertificateInfoFromBufEx((char *)cert,certLen,&tCertInfo)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E31:: cert[%d][%s], ret = [%d]!\n", certLen, cert, ret);
                return(ret);
        }
	snprintf(version, sizeof(version), "%03d", tCertInfo.version);
	version[3] = 0;

	len = snprintf(sql, sizeof(sql), "select derPK from reqCertOfCIPS where version ='%s'", version);
	sql[len] = 0;
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionSelectRealDBRecord[%s]\n", sql);
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: record[version=%s] not found\n", version);
		return(errCodeDatabaseMDL_RecordNotFound);
	}
	
	if((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionLocateXMLPackage[%s]\n", "detail");
		return(ret);
	}
	if((ret = UnionReadXMLPackageValue("derPK", derPK, sizeof(derPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionReadXMLPackageValue[%s]\n", "derPK");
		return(ret);
	}
	derPK[ret] = 0;
	if(strncmp(derPK, tCertInfo.derPK, ret) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: verion[%s] certPK[%s] is diff from derPK[%s]\n", version, tCertInfo.derPK, derPK);
		UnionSetResponseRemark("֤�鹫Կ�������ļ��Ĺ�Կ��ͬ");
		return(errCodeParameter);
	}
	
	if((ret = UnionReadRequestXMLPackageValue("body/remark", remark, sizeof(remark))) <= 0)
		memset(remark, 0, sizeof(remark));
	else
		remark[ret] = 0;

	//�����ݲ������ݿ�
	len = snprintf(sql, sizeof(sql), "insert into certOfCIPS values('%s', '%s', %d, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", bankID, version, 1, tCertInfo.hashID, tCertInfo.algID, tCertInfo.issuserDN, tCertInfo.userDN, tCertInfo.derPK, tCertInfo.startDateTime, tCertInfo.endDataTime, tCertInfo.serialNumberAscBuf, fileName, cert, remark);
	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E31:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	
	return(0);
}
