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
#include "unionREC.h"
#include "unionCertFunSM2.h"

/***************************************
�������:	E401
������:		���ɱ��б���
��������:	���ɱ��б���
***************************************/

int UnionGenerateRSAPKCS7Sign();
int UnionGenerateSM2PKCS7Sign();

int UnionDealServiceCodeE401(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret=0;
	char				data[8192*2+1];
	//char				dataBcd[8192+1];
	int				lenOfData = 0;
	char				pix[8+1];
	char				bankID[40+1];
	char				certVersion[2+1];
	//char				keyName[136];
	int				algID = 0;
	char				tmpBuf[16];
	int				algIDOfMyBankCert = 0;
	
	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

	// Ӧ��ID
	if ((ret = UnionReadRequestXMLPackageValue("body/pix",pix,sizeof(pix))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE401:: UnionReadRequestXMLPackageValue[%s]!\n","body/pix");
		UnionSetResponseRemark("��ȡӦ��IDʧ��");
		return(ret);
	}
	else
	{
		pix[ret] = 0;
		UnionFilterHeadAndTailBlank(pix);
		if (strlen(pix) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE401:: pix can not be null!\n");
                        UnionSetResponseRemark("Ӧ��ID����Ϊ��!");
                        return(errCodeParameter);
                }
		if (!UnionIsBCDStr(pix))
		{
			UnionUserErrLog("in UnionDealServiceCodeE401:: pix[%s] is error!\n",pix);
			UnionSetResponseRemark("Ӧ��ID[%s]�Ƿ�,����Ϊʮ��������",pix);
			return(errCodeParameter);
		}
	}
	
	// �����л�����
	if ((ret = UnionReadRequestXMLPackageValue("body/bankID",bankID,sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE401:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankID");
		UnionSetResponseRemark("��ȡ�����л�����ʧ��");
		return(ret);
	}
	bankID[ret] = 0;
	UnionFilterHeadAndTailBlank(bankID);
	if (strlen(bankID)  == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE401:: bankID can not be null!\n");
		UnionSetResponseRemark("�����л����Ų���Ϊ��!");
		return(errCodeParameter);
	}

	// ֤��汾��
	if ((ret = UnionReadRequestXMLPackageValue("body/certVersion",certVersion,sizeof(certVersion))) <= 0)
		snprintf(certVersion,sizeof(certVersion),"%s","01");
	else
	{
		certVersion[ret] = 0;
		UnionFilterHeadAndTailBlank(certVersion);
	}
	

	// ǩ������
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE401:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		UnionSetResponseRemark("��ȡǩ������ʧ��");
                return(lenOfData);
        }
	else
        {
                if (lenOfData == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE401:: data can not be null!\n");
                        UnionSetResponseRemark("ǩ�����ݲ���Ϊ��!");
                        return(errCodeParameter);
                }
        }

	// ����ʹ�õ�֤����㷨��ʶ
	if((algIDOfMyBankCert = UnionReadIntTypeRECVar("algFlagOfMyBankCert")) < 0)
	{
		algIDOfMyBankCert = 2;
	}
	switch(algIDOfMyBankCert)
	{
		case	2:	
			//RSA AND SM2
			// ��ȡ�㷨��ʶ
			if((ret = UnionReadRequestXMLPackageValue("body/algFlag", tmpBuf, sizeof(tmpBuf))) <= 0)
			{
				// ƴ����Կ���ƣ� RSA
				algID = 0;
				sprintf(asymmetricKeyDB.keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
			}
			else
			{
				tmpBuf[ret] = 0;
				algID = atoi(tmpBuf);
				switch(algID)
				{
					case	0:
						// ƴ����Կ���ƣ� RSA
						sprintf(asymmetricKeyDB.keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
						break;
					case	1:
						// ƴ����Կ���ƣ� SM2
						sprintf(asymmetricKeyDB.keyName,"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE401:: algID[%d] error!\n", algID);
						UnionSetResponseRemark("algID[%d]ӦΪ0��1", algID);
						return(errCodeParameter);
				}
			}
			break;
		case	0:	
			// RSA
			algID = 0;
			sprintf(asymmetricKeyDB.keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
			break;
		case	1:
			// SM2	
			algID = 1;
			sprintf(asymmetricKeyDB.keyName,"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE401:: algIDOfMyBankCert[%d] error!\n", algIDOfMyBankCert);
			UnionSetResponseRemark("�ͻ������õ�\"����ʹ�õ�֤����㷨��ʶ\"�����д�ӦΪ0�,1��2");
			return(errCodeParameter);
	}
	
	switch(algID)
	{
		case	0:
			// ���ù��ʽӿ����ɱ��б���
			if((ret = UnionGenerateRSAPKCS7Sign(&asymmetricKeyDB, data, lenOfData)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE401:: UnionGenerateRSAPKCS7Sign!\n");
				return(ret);
			}
			break;
		case	1:
			//���ù��ܽӿ����ɱ��б���
			if((ret = UnionGenerateSM2PKCS7Sign(&asymmetricKeyDB, data, lenOfData)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE401:: UnionGenerateSM2PKCS7Sign!\n");
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE401:: algID[%d] error!\n", algID);
			UnionSetResponseRemark("algID[%d]ӦΪ0��1", algID);
			return(errCodeParameter);
	}
	
	return 0;
}

int UnionGenerateRSAPKCS7Sign(PUnionAsymmetricKeyDB pasymmetricKeyDB, char *data, int lenOfData)
{
	int			ret = 0;
	char			sql[512];
	char			x509Buf[4000+1];
	char			sign[4096+1];
	char			dataBcd[8192+1];
	
	int			vkIndex = 0;

	data[lenOfData] = 0;
	lenOfData = aschex_to_bcdhex(data,lenOfData,dataBcd);
	memcpy(data,dataBcd,lenOfData);
	data[lenOfData] = 0;

	if(pasymmetricKeyDB == NULL || data == NULL || lenOfData == 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: parameter error!\n");
		return(errCodeParameter);
	}
	
	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(pasymmetricKeyDB->keyName,0,pasymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionReadAsymmetricKeyDBRec[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}	

	snprintf(sql,sizeof(sql),"select bankCert from certOfCnaps2 where keyName = '%s'",pasymmetricKeyDB->keyName);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
                UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: keyName[%s] not found!\n",pasymmetricKeyDB->keyName);
		UnionSetResponseRemark("��Կ[%s]������",pasymmetricKeyDB->keyName);
		return(errCodeParameter);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("bankCert",x509Buf,sizeof(x509Buf))) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionReadXMLPackageValue[%s]!\n","bankCert");
		return(ret);
	}
	x509Buf[ret] = 0;

	if(!pasymmetricKeyDB->vkStoreLocation)
		vkIndex = 99;
	else
		vkIndex = atoi(pasymmetricKeyDB->vkIndex);
		
	// ����PKCS#7��ʽ��ǩ��
	if ((ret = UnionJHSignWithPkcs7Hsm(vkIndex,pasymmetricKeyDB->vkValue,(unsigned char *)data,lenOfData,0,x509Buf,NULL,sign,sizeof(sign))) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionJHSignWithPkcs7Hsm!\n");
		return(ret);
	}

	// ������Ӧǩ��
	if ((ret = UnionSetResponseXMLPackageValue("body/sign",sign)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE401:: UnionSetResponseXMLPackageValue!\n");
		return(ret);
	}
	
	return 0;
}

int UnionGenerateSM2PKCS7Sign(PUnionAsymmetricKeyDB pasymmetricKeyDB, char *data, int lenOfData)
{
	int			ret = 0;
	char			signature[128];
	//unsigned char		rout[64];
	//unsigned char		sout[64];

	char			sql[512];
	char			sign[4096+1];
	char			x509Buf[4000+1];
	int			lenOfX509Buf = 0;
	char			tmpData[1024];
	int			lenOfVkValue = 0;
	char			bcdSign[2048];
	int			lenOfSign = 0;
	int			vkIndex = 0;

	if(pasymmetricKeyDB == NULL || data == NULL || lenOfData == 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: parameter error!\n");
		return(errCodeParameter);
	}
	
	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(pasymmetricKeyDB->keyName,0,pasymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionReadAsymmetricKeyDBRec[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}	
	
	lenOfVkValue = aschex_to_bcdhex(pasymmetricKeyDB->vkValue, strlen(pasymmetricKeyDB->vkValue), tmpData);
	memcpy(pasymmetricKeyDB->vkValue, tmpData, lenOfVkValue);
	pasymmetricKeyDB->vkValue[lenOfVkValue] = 0;

	if(pasymmetricKeyDB->vkStoreLocation == 0)
		vkIndex = -1;
	else
		vkIndex = atoi(pasymmetricKeyDB->vkIndex);

	if((ret = UnionHsmCmdK3("02", strlen(SM2DefaultUserID), SM2DefaultUserID, lenOfData, data, vkIndex, lenOfVkValue, pasymmetricKeyDB->vkValue, signature, sizeof(signature))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE401:: UnionHsmCmdK3 error!");
		return(ret);
	}
	lenOfSign = aschex_to_bcdhex(signature, strlen(signature), bcdSign);
	bcdSign[lenOfSign] = 0;

	snprintf(sql,sizeof(sql),"select bankCert from certOfCnaps2 where keyName = '%s'",pasymmetricKeyDB->keyName);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
                UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: keyName[%s] not found!\n",pasymmetricKeyDB->keyName);
		UnionSetResponseRemark("��Կ[%s]������",pasymmetricKeyDB->keyName);
		return(errCodeParameter);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	if ((lenOfX509Buf = UnionReadXMLPackageValue("bankCert",x509Buf,sizeof(x509Buf))) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionReadXMLPackageValue[%s]!\n","bankCert");
		return(lenOfX509Buf);
	}
	x509Buf[lenOfX509Buf] = 0;
	
	// ����PKCS#7��ʽ��ǩ��
	if ((ret = UnionFormSM2Pkcs7SignDetach((unsigned char*)bcdSign, x509Buf, lenOfX509Buf, NULL, 0, sign,sizeof(sign), 0)) < 0)
	{
		UnionUserErrLog("in UnionGenerateRSAPKCS7Sign:: UnionFormSM2Pkcs7SignDetach!\n");
		return(ret);
	}

	// ������Ӧǩ��
	if ((ret = UnionSetResponseXMLPackageValue("body/sign",sign)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE401:: UnionSetResponseXMLPackageValue!\n");
		return(ret);
	}
	
	return 0;
}
