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
#include "base64.h"
#include "unionRSA.h"

/***************************************
  �������:     8EE2
  ������:    	�ϴ�֤���ļ�
  ��������:     �ϴ�֤���ļ�
 ***************************************/

/*
 * ���ַ����ո�תΪ�»���
 *
 */
static int UnionAlterAllBlankcharsWithUnderline(char *inStr, int lenOfInStr, char *outStr, int sizeOfOutStr)
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
static int UnionGetUserDNInSpecFormFromX509Cer(PUnionX509CerSM2 pX509Cer, char *userDN, int sizeOfUserDN)
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

int UnionDealServiceCode8EE2(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	char		tmpBuf[32];
	int		cerFlag = 0;
	char		fileName[512];
	char		path[512];
	char		fileFullName[512];
	char		sql[19200];
	int		len;
	FILE		*fp;
	char		cert[8192] = {0};
	int		certLen = 0;
	char		remark[128];
	char		derPK[144];
	char		*ptr = NULL;
	
	char		dateTime[16];
	time_t		begin_t, end_t;
	char		sysFullDate[16];
	char		evalue[16];
	char		specUserDN[512];
	char		baseCert[8192*2];
	char		buffer[8192*2];
	
	TUnionX509CerSM2		tCertInfo;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionAsymmetricKeyDB		caAsymmetricKeyDB;
	
	memset(&tCertInfo, 0, sizeof(tCertInfo));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	memset(&caAsymmetricKeyDB, 0, sizeof(caAsymmetricKeyDB));
	
	UnionGetFullSystemDate(sysFullDate);
	
	// ��ȡ֤������
	if((ret = UnionReadRequestXMLPackageValue("body/cerFlag", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/cerFlag");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE2:: cerFlag should not be null!\n");
		UnionSetResponseRemark("cerFlag����Ϊ��");
		return(ret);
	}
	else
	{
		tmpBuf[ret] = 0;
		cerFlag = atoi(tmpBuf);
	}

	//��ȡ֤��洢·��
	if((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionReadStringTypeRECVar[%s]!\n", "defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}
	UnionReadDirFromStr(ptr, -1, path);
	
	//�ļ���
	if((ret = UnionReadRequestXMLPackageValue("body/fileName", fileName, sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionReadRequestXMLPackageValue[%s]\n", "body/fileName`");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", path, fileName);

	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE2:: fopen [%s] failed\n", fileFullName);
		return(errCodeParameter);
	}
	
	//��֤���ļ�����
	if((certLen = fread(cert, 1, sizeof(cert), fp)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE2:: read file[%s] failed", "fileFullName");
		return(certLen);
	}
	else if(certLen == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE2:: read file[%s] failed len=[%d]", "fileFullName", certLen);
		UnionSetResponseRemark("�ļ�Ϊ���ļ�");
		return(errCodeParameter);
	}
	
	if(!UnionTrimPemHeadTail((char *)cert, (char *)cert, "CERTIFICATE"))
	{
		to64frombits((unsigned char*)baseCert, (unsigned char*)cert, certLen);
		UnionPemStrPreHandle((char*)baseCert, (char*)buffer, "CERTIFICATE", 8192);
	}
	else
	{
		memcpy(baseCert, cert, certLen);
		baseCert[certLen] = 0;
		UnionPemStrPreHandle((char*)baseCert, (char*)buffer, "CERTIFICATE", 8192);
	}
	//����֤��
        if ((ret = UnionGetCertificateInfoFromBufEx((char *)cert,certLen,&tCertInfo)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EE2:: cert[%d][%s], ret = [%d]!\n", certLen, cert, ret);
                return(ret);
        }
	
	// �ж�֤����Ч��
	if(memcmp(sysFullDate, tCertInfo.startDateTime, 8) < 0 || memcmp(sysFullDate, tCertInfo.endDataTime, 8) > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE2:: certificate expired!\n");
		return(errCodeOffsetOfCertMDL_OutOfDate);
	}
	
	switch(cerFlag)		// �ڲ�֤��
	{
		case	0:
			// ��Կ��
			if((ret = UnionReadRequestXMLPackageValue("body/keyName", asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyName");
				return(ret);
			}
			else if(ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: keyName����Ϊ��\n");
				UnionSetResponseRemark("keyName����Ϊ��");
				return(errCodeParameter);
			}
			else
				asymmetricKeyDB.keyName[ret] = 0;
			break;
		case	1:
			// ��Կ��
			if((ret = UnionReadRequestXMLPackageValue("body/virKeyName", asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/virKeyName");
				return(ret);
			}
			else if(ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: keyName����Ϊ��\n");
				UnionSetResponseRemark("keyName����Ϊ��");
				return(errCodeParameter);
			}
			else
				asymmetricKeyDB.keyName[ret] = 0;
			break;
		case	3:
			if ((ret = UnionGetUserDNInSpecFormFromX509Cer(&tCertInfo, specUserDN, sizeof(specUserDN))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionGetUserDNInSpecFormFromX509Cer error!\n");
				return(ret);
			}
			switch(tCertInfo.algID)
			{
				case	0:
					snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CERT.%s.RSARoot", specUserDN);
					break;
				case	1:
					snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CERT.%s.SM2Root", specUserDN);
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode8EE2:: algorithmID[%d] error!\n", tCertInfo.algID);
					UnionSetResponseRemark("֤���㷨��֧��");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCode8EE2:: cerFlag[%d] error\n", cerFlag);
			UnionSetResponseRemark("֤���ʶ��");
			return(errCodeParameter);
	}
	
	if(cerFlag == 0)
	{
		if((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 0, &asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n", asymmetricKeyDB.keyName);
			return(ret);
		}
	
		// ֤�鹫Կ�����������ļ�ʱ�Ĺ�Կһ��
		if(memcmp(tCertInfo.derPK+2, asymmetricKeyDB.pkValue, 128) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE2:: certPK[%s] and localKeyPK[%s] are not the same!\n", tCertInfo.derPK+2, asymmetricKeyDB.pkValue);
			UnionSetResponseRemark("֤�鹫Կ�뱾�ع�Կ��һ��");
			return(errCodeOffsetOfCertMDL_VerifyCert);
		}
	}
	else
	{
		// ��ѯ��Կ�Ƿ����
		if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 0, &asymmetricKeyDB)) > 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE2:: keyName[%s] already exist!\n", asymmetricKeyDB.keyName);
			UnionSetResponseRemark("��Կ�Ѵ���");
			return(errCodeParameter);
		}	
	}
	if(cerFlag != 3)
	{
		// �ø�֤��Ĺ�Կ��֤��
		len = snprintf(sql, sizeof(sql), "select keyName from cert where cerFlag = 3 and userInfo = '%s'", tCertInfo.issuserDN);
		if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionSelectRealDBRecord[%s]!\n", sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE2:: rootCert[%s] record not found!\n", tCertInfo.issuserDN);
			return(errCodeOffsetOfCertMDL_CaNotExist);
		}
		UnionLocateXMLPackage("detail", 1);
		if((ret = UnionReadXMLPackageValue("keyName", caAsymmetricKeyDB.keyName, sizeof(caAsymmetricKeyDB.keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionReadXMLPackageValue[%s]!\n", "keyName");
			return(ret);
		}
		
		if((ret = UnionReadAsymmetricKeyDBRec(caAsymmetricKeyDB.keyName, 0, &caAsymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionReadAsymmetricKeyDBRec[%s]!\n", caAsymmetricKeyDB.keyName);	
			return(ret);
		}
		snprintf(derPK, sizeof(derPK), "04%s", caAsymmetricKeyDB.pkValue);
		//��֤��Ĺ�Կ��֤֤��
		if ((ret = UnionVerifyCertificateWithPKEx(derPK, (char *)cert, certLen)) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret = [%d]!\n",derPK, certLen, cert, ret);
        	        return(errCodeOffsetOfCertMDL_VerifyCert);
		}
        }	

	if((ret = UnionReadRequestXMLPackageValue("body/remark", remark, sizeof(remark))) <= 0)
		memset(remark, 0, sizeof(remark));
	else
		remark[ret] = 0;

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
	
	switch(cerFlag)
	{
		case	0:			//����֤��
			len = snprintf(sql, sizeof(sql), "update asymmetricKeyDB set activeDate = '%s', effectiveDays = '%d', status = %d where keyName = '%s'", asymmetricKeyDB.activeDate, asymmetricKeyDB.effectiveDays, asymmetricKeyDB.status, asymmetricKeyDB.keyName);
			sql[len] = 0;
			if((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionExecRealDBSql sql[%s] ret[%d]!\n", sql, ret);
				return(ret);
			}
			break;
		case	1:			//����֤��
		case	3:			//��֤��
			switch(tCertInfo.algID)
			{
				case	0:
					// ��DER����Ĺ�Կת�����㹫Կ
					if((ret = UnionPKCSCerASN1DER_Decode(tCertInfo.derPK, asymmetricKeyDB.pkValue, evalue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionPKCSCerASN1DER_Decod[%s] [%d]!\n", tCertInfo.derPK, ret);
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
					break;
				case	1:
					memcpy(asymmetricKeyDB.pkValue, tCertInfo.derPK+2, strlen(tCertInfo.derPK)-2);
					asymmetricKeyDB.keyLen = 256;
					asymmetricKeyDB.pkExponent = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode8EE2:: algorithmID[%d] error!\n", tCertInfo.algID);
					UnionSetResponseRemark("֤���㷨��֧��");
					return(errCodeParameter);
			}
			// ������Կ
			if((ret = UnionInitAsymmetricKeyDB(&asymmetricKeyDB, tCertInfo.algID, conAsymmetricCreatorTypeOfUser)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionInitAsymmetricKeyDB!\n");
				return(ret);
			}
			
			if((ret = UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionGenerateAsymmetricKeyDBRec[%s]!\n", asymmetricKeyDB.keyName);
				return(ret);
			}
			
			//to64frombits((unsigned char*)baseCert, (unsigned char*)cert, certLen);
			//UnionPemStrPreHandle((char*)baseCert, (char*)buffer, "CERTIFICATE", 8192);
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCode8EE2:: cerFlag[%d] error!\n", cerFlag);
			UnionSetResponseRemark("֤���ʶ����");
			return(errCodeParameter);
	}

	//�����ݲ������ݿ�
	len = snprintf(sql, sizeof(sql), "insert into cert(keyName, serialNumber, cerFlag, version, algID, hashID, startDateTime, endDateTime, issuerInfo, userInfo, bankCert, fileName, remark) values('%s', '%s', %d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s')", asymmetricKeyDB.keyName, tCertInfo.serialNumberAscBuf, cerFlag, tCertInfo.version, tCertInfo.algID, tCertInfo.hashID, tCertInfo.startDateTime, tCertInfo.endDataTime, tCertInfo.issuserDN, tCertInfo.userDN, buffer, fileName, remark);

	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		if(cerFlag != 0)
			UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8EE2:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	
	return(0);
}
