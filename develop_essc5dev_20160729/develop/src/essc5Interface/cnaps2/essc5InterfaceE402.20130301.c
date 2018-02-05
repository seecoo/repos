//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionCertFun.h"
#include "unionVKForm.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
#include "UnionSHA1.h"
#include "unionRSA.h"
#include "unionCertFunSM2.h"
#include "base64.h"
#include "unionCertFun.h"

/***************************************
�������:	E402
������:		�������а󶨱���
��������:	�������а󶨱���
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

int UnionDealServiceCodeE402(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret=0;
	char				shapadStr[] = "3021300906052B0E03021A05000414";
	char				t_shapadStr[64];
	char 				filePath[512];
	char				x509FileName[512];
	char				cerBuf[4096];
	int				cerBufLen = 0;
	char				data[8192*2];
	char				dataBcd[8192];
	char				newData[4096];
	int				lenOfData = 0;
	char				pkcs7Sign[8192];
	char				pix[16];
	char				bankID[64];
	char				certVersion[16];
	char				mode[16];
	char				oriData[4096];
	int				lenOfOriData = 0;
	char				evalue[16];
	char				sql[8192];
	int				len = 0;
	
	char				sign[4096];
	char				hashID[16] = "01";
	int				h_id = 0;
	FILE				*fp = NULL;
	char				dateTime[16];
	char				t_sign[4096];
	int				lenOfSign = 0;
	time_t				begin_t,end_t;
	
	int				signlen = 0;
	int				signType = 0;
	int				algID = 0;
	char				derPK[1024];
	char				base64CertBuf[8192];
	char				sysDate[16];
	char				caBcdDerPK[4096];
	char				caDerPK[4096];
	int				pkExponent = 0;
	char				buffer[8192*2];
	
	//TUnionX509Cer			x509Cer;
	TUnionX509CerSM2		x509CerSM2;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionAsymmetricKeyDB		caAsymmetricKeyDB;

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	snprintf(filePath,sizeof(filePath),"%s/fileDir/bankCert",getenv("UNIONREC"));
	
	UnionGetFullSystemDate(sysDate);

	// Ӧ��ID
	if ((ret = UnionReadRequestXMLPackageValue("body/pix",pix,sizeof(pix))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE402:: UnionReadRequestXMLPackageValue[%s]!\n","body/pix");
                return(ret);
        }
	else
        {
		pix[ret] = 0;
		UnionFilterHeadAndTailBlank(pix);
		if (strlen(pix) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE402:: pix can not be null!\n");
                        UnionSetResponseRemark("Ӧ��ID����Ϊ��!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(pix))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE402:: pix[%s] is error!\n",pix);
                        UnionSetResponseRemark("Ӧ��ID[%s]�Ƿ�,����Ϊʮ��������",pix);
                        return(errCodeParameter);
                }
        }

	// �����л�����
	if ((ret = UnionReadRequestXMLPackageValue("body/bankID",bankID,sizeof(bankID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE402:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankID");
                return(ret);
        }
	bankID[ret] = 0;
	UnionFilterHeadAndTailBlank(bankID);
	if (strlen(bankID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE402:: bankID can not be null!\n");
		UnionSetResponseRemark("�����л�����");
		return(errCodeParameter);
	}


	// ֤��汾��
	if ((ret = UnionReadRequestXMLPackageValue("body/certVersion",certVersion,sizeof(certVersion))) <= 0)
		strcpy(certVersion,"01");
	else
	{
		certVersion[ret] = 0;
		UnionFilterHeadAndTailBlank(certVersion);
	}

	// ģʽ
	// 0:����
	// 1:���
	// 2:����
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE402:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	mode[ret] = 0;
	if (mode[0] != '0' && mode[0] != '1' && mode[0] != '2')
	{
		UnionUserErrLog("in UnionDealServiceCodeE402:: mode[%s] error!!\n",mode);
		UnionSetResponseRemark("ģʽ[%s]������[0,1,2]֮��",mode);
		return(errCodeParameter);
	}
	
	if (mode[0] == '2')
	{
		snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer", pix, bankID, certVersion);
		if ((ret =  UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,1,&asymmetricKeyDB)) < 0)
		{
			// ���ʵĲ����ڶ�����
			UnionLog("in UnionDealServiceCodeE404:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n", asymmetricKeyDB.keyName);
			snprintf(asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName),"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
			if ((ret =  UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,1,&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE404:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
		}
		// ɾ����Կֵ
		if ((ret = UnionDropAsymmetricKeyDB(&asymmetricKeyDB)) < 0)	
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionDropAsymmetricKeyDB[%s]!\n",asymmetricKeyDB.keyName);
			return(ret);
		}

		// ɾ����Կ֤���¼
		if ((ret = UnionDropCertOfCnaps2(asymmetricKeyDB.keyName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionDropCertOfCnaps2[%s]!\n",asymmetricKeyDB.keyName);
			return(ret);
		}
		return 0;
	}
	
	if (mode[0] != '2')
	{
		// ǩ������
		if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
			return(lenOfData);
		}
		else
		{
       			if (lenOfData == 0)
       			{
        	       		UnionUserErrLog("in UnionDealServiceCodeE402:: data can not be null!\n");
        	       		UnionSetResponseRemark("ǩ�����ݲ���Ϊ��!");
        	       		return(errCodeParameter);
       			}
		}

		// ����ǩ��
		if ((lenOfSign = UnionReadRequestXMLPackageValue("body/sign",pkcs7Sign,sizeof(pkcs7Sign))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionReadRequestXMLPackageValue[%s]!\n","body/sign");
			return(lenOfSign);
		}
		pkcs7Sign[lenOfSign] = 0;
		if (lenOfSign == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: sign can not be null!\n");
			return(errCodeParameter);
		}
	
		// ����pkcs7����,��֤��ȡ����Կ֤��
		// ȡ��ǩ�����ݡ�ǩ����Ԥ��hsm��֤ǩ��
		// ����,p7b�ļ�����ǩ������,��ca��֤��
		//memset(cerBuf,0,sizeof(cerBuf));
		if((ret = UnionParasePKcs7SignEx((unsigned char *)pkcs7Sign,lenOfSign, (unsigned char*)cerBuf, &cerBufLen,(unsigned char*)sign,&signlen,(unsigned char*)oriData,&lenOfOriData,&signType,&algID,&h_id)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionParaseJHSignWithPkcs7 [%s] [%d]!\n",pkcs7Sign,ret);
			return(ret);
		}
	}

	if(!algID)
	{
		snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer", pix, bankID, certVersion);
		if (mode[0] != '0')
		{
			if ((ret =  UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionReadAsymmetricKeyDBRec asymmetricKeyDB.keyName[%s]!\n",asymmetricKeyDB.keyName);
				UnionSetResponseRemark("��Կ[%s]������",asymmetricKeyDB.keyName);
				return(ret);
			}
		}
	
		// ��ȡRSA֤����Ϣ����
		memset(&x509CerSM2,0,sizeof(x509CerSM2));
		if ((ret = UnionGetCertificateInfoFromBufEx(cerBuf,cerBufLen,&x509CerSM2)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionGetCertificateInfoFromBuf[%s] [%d]!\n",cerBuf,ret);
			return(ret);
		}
		if((strncmp(sysDate, x509CerSM2.startDateTime, 8) < 0) || (strncmp(sysDate, x509CerSM2.endDataTime, 8) > 0))
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: certificate out of date!\n");
			return(errCodeOffsetOfCertMDL_OutOfDate);
		}
		len = snprintf(sql, sizeof(sql), "select keyName from certOfCNAPS2 where cerFlag=3 and userInfo='%s'", x509CerSM2.issuserDN);
		if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionSelectRealDBRecord[%s]!\n", sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: rootCert[%s] record not found!\n", x509CerSM2.issuserDN);
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
		// �㹫Կתder����
		// ����֤����㹫Կ�����DER��ʽ
		// ��ȡ��Կָ��
		pkExponent = UnionGetCurrentRsaExponent();
		// ���ù�Կָ��
		if(pkExponent != caAsymmetricKeyDB.pkExponent);
			UnionSetCurrentRsaExponent(caAsymmetricKeyDB.pkExponent);
		if((ret = UnionFormANSIDERRSAPK(caAsymmetricKeyDB.pkValue, strlen(caAsymmetricKeyDB.pkValue), caBcdDerPK, sizeof(caBcdDerPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E18:: UnionFormANSIDERRSAPK!\n");
			UnionSetResponseRemark("�㹫Կת��DER��ʽʧ��");
			return(ret);
		}
		bcdhex_to_aschex(caBcdDerPK, ret, caDerPK);
		caDerPK[ret * 2] = 0;
		
		//��ԭ��Կָ��
		if(pkExponent != caAsymmetricKeyDB.pkExponent)
			UnionSetCurrentRsaExponent(pkExponent);

		//��֤��Ĺ�Կ��֤֤��
		if ((ret = UnionVerifyCertificateWithPKEx(caDerPK, (char *)cerBuf,cerBufLen)) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret = [%d]!\n",caDerPK, cerBufLen, cerBuf, ret);
        	        return(errCodeOffsetOfCertMDL_VerifyCert);
        	}
		// ��ȡ֤����
		snprintf(x509FileName,sizeof(x509FileName),"%s/%s.%s%02d.cer",filePath,pix,bankID,x509CerSM2.version);
		
		// �湫Կ֤��
		// ��תΪbase64����
		to64frombits((unsigned char*)base64CertBuf, (unsigned char*)cerBuf, cerBufLen);
		UnionPemStrPreHandle((char*)base64CertBuf, (char*)buffer, "CERTIFICATE", 8192);
		if ((fp = fopen(x509FileName,"wb")) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: create file [%s] failed!\n",x509FileName);
			UnionSetResponseRemark("�����ļ�[%s]ʧ��",x509FileName);
			return(errCodeOffsetOfCertMDL_WriteFile);
		}
	
		fwrite(cerBuf,cerBufLen,1,fp);
		fflush(fp);
		fclose(fp);
		
		//��DER��ʽת�����㹫Կ
		if ((ret = UnionPKCSCerASN1DER_Decode(x509CerSM2.derPK,asymmetricKeyDB.pkValue,evalue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionPKCSCerASN1DER_Decode[%s] [%d]!\n",x509CerSM2.derPK,ret);
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
			UnionUserErrLog("in UnionDealServiceCodeE402:: evalue[%s] != 010001 or 3!\n",evalue);
			return(ret);
		}
	
		// ��Կ״̬
		asymmetricKeyDB.status = 1;
		
		// ֤����ʼʱ��
		memcpy(asymmetricKeyDB.activeDate,x509CerSM2.startDateTime,8);
		snprintf(dateTime, sizeof(dateTime), "%s000000", sysDate);
	
		if ((begin_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
			return(errCodeOffsetOfCertificate_InvalidDate);
		}
	
		// ֤�����ʱ��
		memcpy(dateTime,x509CerSM2.endDataTime,8);
		sprintf(dateTime+8,"%s","000000");
		dateTime[14] = 0;
	
		if ((end_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
			return(errCodeOffsetOfCertificate_InvalidDate);
		}
	
		// ��Ч����
		asymmetricKeyDB.effectiveDays = (end_t - begin_t)/60/60/24; 
		
		//lenOfSign = aschex_to_bcdhex(sign,strlen(sign),t_sign);
		//t_sign[lenOfSign] = 0;
		
		data[lenOfData] = 0;
		lenOfData = aschex_to_bcdhex(data,lenOfData,dataBcd);
		memcpy(data,dataBcd,lenOfData);
		data[lenOfData] = 0;

		// ����hashֵ
		if (strcmp(hashID,"01") == 0)
		{
			len = strlen(shapadStr);
			UnionSHA1((unsigned char *)data,lenOfData,(unsigned char *)newData+len/2);
			aschex_to_bcdhex(shapadStr,strlen(shapadStr),t_shapadStr);
			memcpy(newData,t_shapadStr,len/2);
			lenOfData = 20+len/2;
			newData[lenOfData] = 0;
		}
	/*	else if (strcmp(hashID,"02") == 0)
		{
			UnionMD5((unsigned char *)data,strlen(data),(unsigned char *)newData);
			lenOfData = 16;
			newData[lenOfData] = 0;
		}
	*/
	
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case conHsmCmdVerRacalStandardHsmCmd:
			case conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmd38('1',NULL,signlen,sign,lenOfData,newData,asymmetricKeyDB.pkValue)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE402:: UnionHsmCmd38!\n");
					return(ret);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE402:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				return(errCodeParameter);
		}
		/*
		// p7b֤����֤
		if ((ret = UnionVerifyCertByP7bCAlinks(x509FileName,p7bFullName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionVerifyCertByP7bCAlinks  x509FileName[%s]  p7bFullName[%s]!\n",x509FileName,p7bFileName);
			return(ret);
		}
		*/
	
		// ������Կ
		if (mode[0] == '0')	// ������Կ
		{
			if ((ret = UnionInitAsymmetricKeyDB(&asymmetricKeyDB, algID, conAsymmetricCreatorTypeOfApp)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionInitAsymmetricKeyDB!\n");
				return(ret);
			}
			if ((ret = UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionGenerateAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
			/*
			// ����֤���ߵ��û���Ϣ
			len = sprintf(issuerInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",x509CerSM2.issuerInfo.countryName,x509CerSM2.issuerInfo.stateOrProvinceName,x509CerSM2.issuerInfo.localityName,x509CerSM2.issuerInfo.organizationName,x509CerSM2.issuerInfo.organizationalUnitName,x509CerSM2.issuerInfo.commonName,x509CerSM2.issuerInfo.email,x509CerSM2.issuerInfo.alias);
			issuerInfo[len] = 0;
	
			// ֤��ӵ������Ϣ
			len = sprintf(userInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",x509CerSM2.userInfo.countryName,x509CerSM2.userInfo.stateOrProvinceName,x509CerSM2.userInfo.localityName,x509CerSM2.userInfo.organizationName,x509CerSM2.userInfo.organizationalUnitName,x509CerSM2.userInfo.commonName,x509CerSM2.userInfo.email,x509CerSM2.userInfo.alias);
			userInfo[len] = 0;
			*/
			// ����֤��
			len = sprintf(sql,"insert into certOfCnaps2 (keyName,cerFlag,serialNumber,version,hashID,startDateTime,endDataTime,issuerInfo,userInfo,bankCert) "
					" values ('%s',1,'%s',%d,%d,'%s','%s','%s','%s','%s')"
					,asymmetricKeyDB.keyName,x509CerSM2.serialNumberAscBuf,x509CerSM2.version,x509CerSM2.hashID,x509CerSM2.startDateTime,x509CerSM2.endDataTime,x509CerSM2.issuserDN,x509CerSM2.userDN,buffer);
			sql[len] = 0;
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionExecRealDBSql[%s]!\n",sql);
				return(ret);
			}
		}
		else if (mode[0] == '1')	// ������Կ
		{
			if ((ret = UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionUpdateAsymmetricKeyDBKeyValue[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
			// ����֤����Ϣ
			snprintf(sql, sizeof(sql), "update certOfCnaps2 set serialNumber = '%s', version = %d, hashID = %d, startDateTime = '%s', endDataTime = '%s', issuerInfo = '%s', userInfo = '%s', bankCert = '%s' where keyName = '%s'", x509CerSM2.serialNumberAscBuf, x509CerSM2.version, x509CerSM2.hashID, x509CerSM2.startDateTime, x509CerSM2.endDataTime, x509CerSM2.issuserDN, x509CerSM2.userDN, buffer, asymmetricKeyDB.keyName);
			//snprintf(sql, sizeof(sql), "update certOfCnaps2 set serialNumber = '%s', version = %d, startDateTime = '%s', endDataTime = '%s', bankCert = '%s' where keyName = '%s'", x509CerSM2.serialNumberAscBuf, x509CerSM2.version, x509CerSM2.startDateTime, x509CerSM2.endDataTime, buffer, asymmetricKeyDB.keyName);
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionExecRealDBSql[%s]!\n", sql);
				return(ret);
			}
		}
		
	}
	else
	{
		snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer_sm2", pix, bankID, certVersion);
		if (mode[0] != '0')
		{
			if ((ret =  UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionReadAsymmetricKeyDBRec asymmetricKeyDB.keyName[%s]!\n",asymmetricKeyDB.keyName);
				UnionSetResponseRemark("��Կ[%s]������",asymmetricKeyDB.keyName);
				return(ret);
			}
		}
	
		// ��ȡSM2֤����Ϣ
		memset(&x509CerSM2, 0, sizeof(x509CerSM2));
		if ((ret = UnionGetCertificateInfoFromBufEx(cerBuf, cerBufLen, &x509CerSM2)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionGetCertificateInfoFromBufEx[%s] [%d]!\n", cerBuf, ret);
			return(ret);
		}
		if((strncmp(sysDate, x509CerSM2.startDateTime, 8) < 0) || (strncmp(sysDate, x509CerSM2.endDataTime, 8) > 0))
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: certificate out of date!\n");
			return(errCodeOffsetOfCertMDL_OutOfDate);
		}
		/*
		// ����֤���ߵ��û���Ϣ
		len = sprintf(issuerInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",x509CerSM2.issuerInfo.countryName,x509CerSM2.issuerInfo.stateOrProvinceName,x509CerSM2.issuerInfo.localityName,x509CerSM2.issuerInfo.organizationName,x509CerSM2.issuerInfo.organizationalUnitName,x509CerSM2.issuerInfo.commonName,x509CerSM2.issuerInfo.email,x509CerSM2.issuerInfo.alias);
		issuerInfo[len] = 0;
		*/
		// ��֤���鹫Կ֤��
		//len = snprintf(sql, sizeof(sql), "select keyName from certOfCNAPS2 where cerFlag=3 and userInfo='%s'", issuerInfo);	
		len = snprintf(sql, sizeof(sql), "select keyName from certOfCNAPS2 where cerFlag=3 and userInfo='%s'", x509CerSM2.issuserDN);
		if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionSelectRealDBRecord[%s]!\n", sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: rootCert[%s] record not found!\n", x509CerSM2.issuserDN);
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
		if ((ret = UnionVerifyCertificateWithPKEx(derPK, (char *)cerBuf,cerBufLen)) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E1F:: UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret = [%d]!\n",derPK, cerBufLen, cerBuf, ret);
        	        return(errCodeOffsetOfCertMDL_VerifyCert);
        	}

		// ��ȡ֤����
		snprintf(x509FileName,sizeof(x509FileName),"%s/%s.%s%02dSM2.cer",filePath,pix,bankID,x509CerSM2.version);
		
		// �湫Կ֤��
		to64frombits((unsigned char*)base64CertBuf, (unsigned char*)cerBuf, cerBufLen);
		UnionPemStrPreHandle((char*)base64CertBuf, (char*)buffer, "CERTIFICATE", 8192);
		if ((fp = fopen(x509FileName,"wb")) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: create file [%s] failed!\n",x509FileName);
			UnionSetResponseRemark("�����ļ�[%s]ʧ��",x509FileName);
			return(errCodeOffsetOfCertMDL_WriteFile);
		}
	
		fwrite(cerBuf,cerBufLen,1,fp);
		fflush(fp);
		fclose(fp);
		
		//��DER��ʽת�����㹫Կ
		memcpy(asymmetricKeyDB.pkValue, x509CerSM2.derPK+2, 128);
		asymmetricKeyDB.pkValue[128] = 0;
		
		// ��Կ����
		asymmetricKeyDB.keyLen = 256;
	
		// ��Կָ��
		asymmetricKeyDB.pkExponent = 0;
	
		// ��Կ״̬
		asymmetricKeyDB.status = 1;
	
		// ֤����ʼʱ��
		memcpy(asymmetricKeyDB.activeDate,x509CerSM2.startDateTime,8);
		snprintf(dateTime, sizeof(dateTime), "%s000000", sysDate);
	
	
		if ((begin_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
			return(errCodeOffsetOfCertificate_InvalidDate);
		}
	
		// ֤�����ʱ��
		memcpy(dateTime,x509CerSM2.endDataTime,8);
		sprintf(dateTime+8,"%s","000000");
		dateTime[14] = 0;
	
		if ((end_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE402:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
			return(errCodeOffsetOfCertificate_InvalidDate);
		}
	
		// ��Ч����
		asymmetricKeyDB.effectiveDays = (end_t - begin_t)/60/60/24; 
		
		lenOfSign = bcdhex_to_aschex(sign,signlen,t_sign);
		t_sign[lenOfSign] = 0;
	
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case conHsmCmdVerRacalStandardHsmCmd:
			case conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdK4(-1, "02",strlen(SM2DefaultUserID),SM2DefaultUserID,strlen(asymmetricKeyDB.pkValue),asymmetricKeyDB.pkValue, lenOfSign,t_sign,lenOfData,data)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE402:: UnionHsmCmdK4!\n");
					return(ret);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE402:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				return(errCodeParameter);
		}
	
		// ������Կ
		if (mode[0] == '0')	// ������Կ
		{
			if ((ret = UnionInitAsymmetricKeyDB(&asymmetricKeyDB, algID, conAsymmetricCreatorTypeOfApp)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionInitAsymmetricKeyDB!\n");
				return(ret);
			}
			if ((ret = UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionGenerateAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
			
			/*
			// ����֤���ߵ��û���Ϣ
			len = sprintf(issuerInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",x509CerSM2.issuerInfo.countryName,x509CerSM2.issuerInfo.stateOrProvinceName,x509CerSM2.issuerInfo.localityName,x509CerSM2.issuerInfo.organizationName,x509CerSM2.issuerInfo.organizationalUnitName,x509CerSM2.issuerInfo.commonName,x509CerSM2.issuerInfo.email,x509CerSM2.issuerInfo.alias);
			issuerInfo[len] = 0;
	
			// ֤��ӵ������Ϣ
			len = sprintf(userInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",x509CerSM2.userInfo.countryName,x509CerSM2.userInfo.stateOrProvinceName,x509CerSM2.userInfo.localityName,x509CerSM2.userInfo.organizationName,x509CerSM2.userInfo.organizationalUnitName,x509CerSM2.userInfo.commonName,x509CerSM2.userInfo.email,x509CerSM2.userInfo.alias);
			userInfo[len] = 0;
			*/

			// ֤�����к�
			//len = sprintf(serialNumber,"%ld",x509CerSM2.serialNumberAscBuf);
			//serialNumber[len] = 0;
	
			// ����֤��
			len = sprintf(sql,"insert into certOfCnaps2 (keyName,cerFlag,serialNumber,version,hashID,startDateTime,endDataTime,issuerInfo,userInfo,bankCert) "
					" values ('%s',1,'%s',%d,%d,'%s','%s','%s','%s','%s')"
					,asymmetricKeyDB.keyName,x509CerSM2.serialNumberAscBuf,x509CerSM2.version,x509CerSM2.hashID,x509CerSM2.startDateTime,x509CerSM2.endDataTime,x509CerSM2.issuserDN,x509CerSM2.userDN,buffer);
			sql[len] = 0;
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionExecRealDBSql[%s]!\n",sql);
				return(ret);
			}
		}
		else if (mode[0] == '1')	// ������Կ
		{	
			if ((ret = UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionUpdateAsymmetricKeyDBKeyValue[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
			// ����֤����Ϣ
			snprintf(sql, sizeof(sql), "update certOfCnaps2 set serialNumber = '%s', version = %d, hashID = %d, startDateTime = '%s', endDataTime = '%s', issuerInfo = '%s', userInfo = '%s', bankCert = '%s' where keyName = '%s'", x509CerSM2.serialNumberAscBuf, x509CerSM2.version, x509CerSM2.hashID, x509CerSM2.startDateTime, x509CerSM2.endDataTime, x509CerSM2.issuserDN, x509CerSM2.userDN, buffer, asymmetricKeyDB.keyName);
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE402:: UnionExecRealDBSql[%s]!\n", sql);
				return(ret);
			}
		}
	}


	return 0;
}

