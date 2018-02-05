

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"
#include "unionCertFunSM2.h"
#include "asymmetricKeyDB.h"
#include "UnionSHA1.h"
#include "UnionSHA256.h"
#include "UnionMD5.h"
#include "unionCertErrCode.h"

/***************************************
�������:	E211
������:		����������ǩ����ǩ
��������:	����������ǩ����ǩ
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
	
	if(pX509Cer->issuerInfo.commonName != NULL && strlen(pX509Cer->issuerInfo.commonName) != 0)
	{
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.commonName, strlen(pX509Cer->issuerInfo.commonName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN, sizeOfUserDN, "%s", pX509Cer->issuerInfo.commonName);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.email != NULL && strlen(pX509Cer->issuerInfo.email) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.email, strlen(pX509Cer->issuerInfo.email), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.email);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.organizationalUnitName != NULL && strlen(pX509Cer->issuerInfo.organizationalUnitName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.organizationalUnitName, strlen(pX509Cer->issuerInfo.organizationalUnitName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.organizationalUnitName);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.organizationalUnitName2 != NULL && strlen(pX509Cer->issuerInfo.organizationalUnitName2) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.organizationalUnitName2, strlen(pX509Cer->issuerInfo.organizationalUnitName2), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.organizationalUnitName2);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.organizationName != NULL && strlen(pX509Cer->issuerInfo.organizationName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.organizationName, strlen(pX509Cer->issuerInfo.organizationName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.organizationName);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.organizationName2 != NULL && strlen(pX509Cer->issuerInfo.organizationName2) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.organizationName2, strlen(pX509Cer->issuerInfo.organizationName2), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.organizationName2);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.localityName != NULL && strlen(pX509Cer->issuerInfo.localityName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.localityName, strlen(pX509Cer->issuerInfo.localityName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.localityName);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.stateOrProvinceName != NULL && strlen(pX509Cer->issuerInfo.stateOrProvinceName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.stateOrProvinceName, strlen(pX509Cer->issuerInfo.stateOrProvinceName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.stateOrProvinceName);
		lenOfUserDN += len;
	}

	if(pX509Cer->issuerInfo.countryName != NULL && strlen(pX509Cer->issuerInfo.countryName) != 0)
	{
		userDN[lenOfUserDN] = 0x2D;
		lenOfUserDN += 1;
		if((len = UnionAlterAllBlankcharsWithUnderline(pX509Cer->issuerInfo.countryName, strlen(pX509Cer->issuerInfo.countryName), userDN+lenOfUserDN, sizeOfUserDN-lenOfUserDN)) < 0)
		{
			UnionUserErrLog("in UnionGetUserDNInSpecFormFromX509Cer:: UnionAlterAllBlankcharsWithUnderline!\n");
			return(len);
		}
		//len = snprintf(userDN + lenOfUserDN, sizeOfUserDN - lenOfUserDN, "-%s", pX509Cer->issuerInfo.countryName);
		lenOfUserDN += len;
	}
	
	return(lenOfUserDN);
}


int UnionDealServiceCodeE211(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret = 0;
	char		data[8192] = {0};
	char		pkcs7Str[4096] = {0};
	int		pkcs7StrLen = 0;
//	char		caName[128] = {0};
	char		userID[] = "1234567812345678";
	int		userIDLen = 16;
	char		tmpBuf[256];
	char		hashData[128];
	int		sendFlag = 0;

	//PKCS7Str��������������
	char		certData[8192] = {0};
	int		certLen = 0;
	char		certSign[8192] = {0};
	int		certSignLen = 0;
	char		oriData[8192*2] = {0};
	int		oriDataLen = 0;
	int		signType = 0;
	int		algorithmID = 0;
	int		hashID = 0;
//	char		certIssuerInfo[1024];
	
	char		bcdCertInfoPK[2048];
	char		ascCertSign[8192];

//	char		sql[2048];
	int		len = 0;
//	char		caDerPK[2048];
	char		DerPK[2048];
	char		certPK[2048];
	int		lenOfCertPK = 0;
	
	char 		sha1pre[] = "3021300906052B0E03021A05000414";
	//char		md5pre[] = "3020300C06082A864886F70D020505000410";
	char 		sha256pre[] = "3031300d060960864801650304020105000420";
	int		lenOfHash = 0;
	char		specissuerDN[512];

	char		sysDateTime[16];
	char		derpkValue[4096];
	int                             pkExponent = 0;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionX509CerSM2                certInfo;
	
	memset(&certInfo, 0, sizeof(certInfo));

	//������ʶ
	if((ret = UnionReadRequestXMLPackageValue("body/sendFlag", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/sendFlag");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/sendFlag");
		UnionSetResponseRemark("sendFlagΪ��");
		return(errCodeParameter);
	}
	else
	{
		tmpBuf[ret] = 0;
		sendFlag = atoi(tmpBuf);
		if(sendFlag != 1 && sendFlag != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE211:: sendFlag err\n");
			UnionSetResponseRemark("sendFlag����Ϊ1��2");
			return(errCodeParameter);
		}
	}

	switch(sendFlag)
	{
		//�״���֤�鲢���ع�ϣ��ʶ����Կ��ǩ������
		case	1:
			//��ȡpkcs7ǩ��
			if((ret = UnionReadRequestXMLPackageValue("body/pkcs7Str", pkcs7Str, sizeof(pkcs7Str))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/pkcs7Str");
				return(ret);
			}
			else if(ret == 0)
			{	
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/pkcs7Str");
				UnionSetResponseRemark("ǩ������Ϊ��");
				return(errCodeParameter);
			}	
			pkcs7StrLen = ret;
			pkcs7Str[ret] = 0;
			
			//����ǩ������ȡ֤����ǩ������
			if((ret = UnionParasePKcs7SignEx((unsigned char*)pkcs7Str, pkcs7StrLen, (unsigned char*)certData, &certLen, (unsigned char*)certSign, &certSignLen, (unsigned char*)oriData, &oriDataLen, &signType, &algorithmID, &hashID)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionParasePKcs7SignEx[%s] ret[%d]\n", pkcs7Str, ret);
				return(ret);
			}
			
			//��չ֤��ǩ��
			bcdhex_to_aschex(certSign, certSignLen, ascCertSign);
			ascCertSign[certSignLen*2] = 0;
			
			//����֤��
			if((ret = UnionGetCertificateInfoFromBufEx(certData, certLen, &certInfo)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionGetCertificateInfoFromBufEx cert[%d][%s], ret[%d]\n", certLen, certData, ret);
				return(ret);
			}
			
			//ƴװ��֤����Կ��
			if((ret = UnionGetUserDNInSpecFormFromX509Cer(&certInfo, specissuerDN, sizeof(specissuerDN))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E1G:: UnionGetUserDNInSpecFormFromX509Cer error!\n");
				return(ret);
			}
			switch (certInfo.algID)
			{
				case	0:
					snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CERT.%s.RSARoot", specissuerDN);                          	        	                                                                                          
					break;
				case	1:
					snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CERT.%s.SM2Root", specissuerDN);
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE211:: algorithmID[%d] error!\n", certInfo.algID);

					return(errCodeParameter);			
			}
			if ((ret =  UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,1,&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
			//��ȡ��֤����
			/*if((ret = UnionReadRequestXMLPackageValue("body/caName", caName, sizeof(caName))) <= 0)
			{
				//ͨ������������֤���issuerDN���㷨ID���Ҹ�֤��Ĺ�Կ
				len = sprintf(certIssuerInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",certInfo.issuerInfo.countryName,certInfo.issuerInfo.stateOrProvinceName,certInfo.issuerInfo.localityName,certInfo.issuerInfo.organizationName,certInfo.issuerInfo.organizationalUnitName,certInfo.issuerInfo.commonName,certInfo.issuerInfo.email,certInfo.issuerInfo.alias);
        			certIssuerInfo[len] = 0;
				
				//��ȡ��֤�鹫Կ
				len = snprintf(sql, sizeof(sql), "select derPK from rootCert where userInfo = '%s' and algID = %d", certIssuerInfo, algorithmID);
				sql[len] = 0;
				if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSelectRealDBRecord[%s]\n", sql);
                                        return(ret);
                                }
                                else if(ret == 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE211:: record[%s] not found\n", caName);
                                        return(errCodeDatabaseMDL_RecordNotFound);
                                }

                                if((ret = UnionLocateXMLPackage("detail",1)) < 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE211:: UnionLocateXMLPackage[%s]\n", "detail");
                                        return(ret);
                                }

                                if((ret = UnionReadXMLPackageValue("derPK", caDerPK, sizeof(caDerPK))) < 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadXMLPackageValue[%s]\n", "derPK");
                                        return(ret);
                                }
                                caDerPK[ret] = 0;
			} 
			else 
			{
				caName[ret] = 0;
			
				//��ȡ��֤�鹫Կ
				len = snprintf(sql, sizeof(sql), "select derPK from rootCert where rootCertName = '%s'", caName);
				sql[len] = 0;
				if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSelectRealDBRecord[%s]\n", sql);
					return(ret);
				}
				else if(ret == 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE211:: record[%s] not found\n", caName);
					return(errCodeDatabaseMDL_RecordNotFound);
				}	
			
				if((ret = UnionLocateXMLPackage("detail",1)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE211:: UnionLocateXMLPackage[%s]\n", "detail");
					return(ret);
				}
				
				if((ret = UnionReadXMLPackageValue("derPK", caDerPK, sizeof(caDerPK))) < 0)
				{	
					UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadXMLPackageValue[%s]\n", "derPK");
					return(ret);
				}
				caDerPK[ret] = 0;
			}*/
			//��֤֤���Ƿ�����Ч����
			UnionGetFullSystemDateTime(sysDateTime);
			if((strcmp(sysDateTime, certInfo.endDataTime) > 0) || (strcmp(sysDateTime, certInfo.startDateTime) < 0))
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: sysTime[%s] is not in certStartTime[%s] and certEndTime[%s]\n", sysDateTime, certInfo.startDateTime, certInfo.endDataTime);
				UnionSetResponseRemark("֤�鲻����Ч����");
				return(errCodeParameter);
			}
			switch(algorithmID)
			{	
				case 0:
					// ��ȡ��Կָ��
		                       	pkExponent = UnionGetCurrentRsaExponent();

					// ���ù�Կָ��
		                        if (pkExponent != asymmetricKeyDB.pkExponent)
		                                UnionSetCurrentRsaExponent(asymmetricKeyDB.pkExponent);
					
					//�㹫ԿתΪDER��Կ
					if ((ret = UnionFormANSIDERRSAPK(asymmetricKeyDB.pkValue,strlen(asymmetricKeyDB.pkValue),DerPK,sizeof(DerPK))) < 0)
		        		{
		                		UnionUserErrLog("in UnionDealServiceCodeE211:: UnionFormANSIDERRSAPK ERR!\n");
		                		return(ret);
		        		}
					bcdhex_to_aschex(DerPK,ret,derpkValue);
		                        derpkValue[ret*2] = 0;
					
		                        // ��ԭ��Կָ
		                        if (pkExponent != asymmetricKeyDB.pkExponent)
		                                UnionSetCurrentRsaExponent(pkExponent);
					break;
				case 1:					
					snprintf(derpkValue, sizeof(derpkValue), "04%s", asymmetricKeyDB.pkValue); 
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE211:: algorithmID invalid\n");
					UnionSetResponseRemark("ǩ���㷨��ΪRSA��SM2");
					return(errCodeParameter);
			}
			//��֤֤����Ч��
			if((ret = UnionVerifyCertificateWithPKEx(derpkValue, certData, certLen)) < 0)	
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret[%d]", derpkValue, certLen, certData, ret);
				return(errCodeOffsetOfCertMDL_VerifyCert);
			}
			//��ȡ�㹫Կ
			switch(algorithmID)
			{
				case 0:
					//��ȡ֤���㹫Կ
					len = aschex_to_bcdhex(certInfo.derPK, strlen(certInfo.derPK), bcdCertInfoPK);
					if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)bcdCertInfoPK, len, certPK, &lenOfCertPK, sizeof(certPK))) < 0)
        		        	{
        		        	        UnionUserErrLog("in UnionDealServiceCodeE211:: UnionGetPKOutOfRacalHsmCmdReturnStr derPK[%s]\n", certInfo.derPK);
        		        	        return(ret);
        		        	}
					break;
				case 1:
					lenOfCertPK = 128;
                			memcpy(certPK, certInfo.derPK+2, lenOfCertPK);
                			certPK[lenOfCertPK] = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE211:: algorithmID invalid\n");
					UnionSetResponseRemark("ǩ���㷨��ΪRSA��SM2");
					return(errCodeParameter);
			}
				
			//���ù�Կ��ǩ��
			if((ret = UnionSetResponseXMLPackageValue("body/pkValue", certPK)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSetResponseXMLPackageValue[%s]\n", "body/pkValue");
				return(ret);
			}
			
			if((ret = UnionSetResponseXMLPackageValue("body/sign", ascCertSign)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSetResponseXMLPackageValue[%s]\n", "body/sign");
				return(ret);
			}
			
			snprintf(tmpBuf, sizeof(tmpBuf), "%d", hashID);
			//���ù�ϣ��ʶ
			if((ret = UnionSetResponseXMLPackageValue("body/hashID", tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSetResponseXMLPackageValue[%s]\n", "body/hashID");
				return(ret);
			}
			
			snprintf(tmpBuf, sizeof(tmpBuf), "%d", algorithmID);
			//�����㷨��ʶ
			if((ret = UnionSetResponseXMLPackageValue("body/algorithmID", tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSetResponseXMLPackageValue[%s]\n", "body/algorithmID");
				return(ret);
			}
	
			// ���ð䷢����Ϣ
			if((ret = UnionSetResponseXMLPackageValue("body/issuserDN", certInfo.issuserDN)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSetResponseXMLPackageValue[%s]\n", "body/issuserDN");
				return(ret);
			}
			
			// ����֤������
			if((ret = UnionSetResponseXMLPackageValue("body/userDN", certInfo.userDN)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSetResponseXMLPackageValue[%s]\n", "body/certDN");
				return(ret);
			}
			
			// ����֤�����к�
			if((ret = UnionSetResponseXMLPackageValue("body/serialNumber", certInfo.serialNumberAscBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionSetResponseXMLPackageValue[%s]\n", "body/serialNumber");
				return(ret);
			}
	
			break;
		//�ڶ��α�����ǩ
		case	2:
			//��ȡ�㷨��ʶ
			if((ret = UnionReadRequestXMLPackageValue("body/algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/algorithmID");
				return(ret);
			}
			else if(ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/algorithmID");
				UnionSetResponseRemark("algorithmIDΪ��");
				return(errCodeParameter);
			}
			else
			{
				tmpBuf[ret] = 0;
				algorithmID = atoi(tmpBuf);
			}
		
			//��ȡ��ϣ��ʶ
			if((ret = UnionReadRequestXMLPackageValue("body/hashID", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/hashID");
				return(ret);
			}
			else if(ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/hashID");
				UnionSetResponseRemark("hashIDΪ��");
				return(errCodeParameter);
			}
			else
			{
				tmpBuf[ret] = 0;
				hashID = atoi(tmpBuf);
			}

			//��ȡԭʼǩ������ժҪ,��չ����ǰ׺
			if((ret = UnionReadRequestXMLPackageValue("body/data", data, sizeof(data))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/data");
				return(ret);
			}
			else if(ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/data");
				UnionSetResponseRemark("ԭ�Ĳ���Ϊ��");
				return(errCodeParameter);
			}
			data[ret] = 0;

			//��ȡ��Կֵ
			if((ret = UnionReadRequestXMLPackageValue("body/pkValue", certPK, sizeof(certPK))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/pkValue");
				return(ret);
			}
			else if(ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/pkValue");
				UnionSetResponseRemark("pkValueΪ��");
				return(errCodeParameter);
			}
			certPK[ret] = 0;
			lenOfCertPK = ret;
			
			//��ȡǩ��
			if((ret = UnionReadRequestXMLPackageValue("body/sign", ascCertSign, sizeof(ascCertSign))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/sign");
				return(ret);
			}
			else if(ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE211:: UnionReadRequestXMLPackageValue[%s]\n", "body/sign");
				UnionSetResponseRemark("signΪ��");
				return(errCodeParameter);
			}
			ascCertSign[ret] = 0;
			certSignLen = ret;
			
			//ժҪ����, �����㷨��Ҫ���ǰ׺
			if(!algorithmID)
			{
				switch(hashID)
				{
					case	UNION_SHA1_Digest_ALG:
						len = snprintf(tmpBuf, sizeof(tmpBuf), "%s%s", sha1pre, data);
						lenOfHash = aschex_to_bcdhex(tmpBuf, len, hashData);
						hashData[lenOfHash] = 0;
						break;
					case	UNION_SHA256_Digest_ALG:
						len = snprintf(tmpBuf, sizeof(tmpBuf), "%s%s", sha256pre, data);
						lenOfHash = aschex_to_bcdhex(tmpBuf, len, hashData);
						hashData[lenOfHash] = 0;
						break;
					/*
					case:	UNION_MD5_Digest_ALG
						len = snprintf(tmpBuf, sizeof(tmpBuf), "%s%s", md5pre, data);
						//UnionMD5((unsigned char*)bcdData, len, (unsigned char*)tmpBuf);
						lenOfHash = aschex_to_bcdhex(tmpBuf, len, hashData);
						hashData[lenOfHash] = 0;
						break;
					*/
					default:
						UnionUserErrLog("in UnionDealServiceCodeE211:: hashID error\n");
						UnionSetResponseRemark("hashID��ΪSHA1��SHA256");
						return(errCodeParameter);
				}
			}

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					switch(algorithmID)
					{
						case	UNION_RSA_ALG:
							//ǩ��������ѹ��
							len = aschex_to_bcdhex(ascCertSign, certSignLen, certSign);
							certSign[len] = 0;
							if((ret = UnionHsmCmd38('1', NULL, len, certSign, lenOfHash, hashData, certPK)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE211:: UnionHsmCmd38 error\n");
								return(ret);
							}
							break;
						case	UNION_SM2_ALG:
							//ָ�����ϣ��API�����ϣ���ֵ
							if((ret = UnionHsmCmdK4(-1,"01",userIDLen,userID,lenOfCertPK,certPK,certSignLen,ascCertSign,strlen(data),data)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE211:: UnionHsmCmdK4 error\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE211:: algorithmID[%d] error\n", algorithmID);
							return(errCodeEsscMDL_InvalidAlgorithmID);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE211:: phsmGroupRec->hsmCmdVersionID[%d]\n", phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE211:: sendFlag error\n");
			UnionSetResponseRemark("sendFlag��Ϊ1��2");
			return(errCodeParameter);
	}
	
	return 0;
}
