//	Author:		lusj
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-12-09

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "asymmetricKeyDB.h"
#include "symmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"

/***************************************
  �������:	E209
  ������:��Կ����PINת��DES���ܣ�֧���ַ��������㷨��
  ��������:	��Կ����PINת��DES���ܣ�֧���ַ��������㷨��

 ***************************************/

int UnionDealServiceCodeE209(PUnionHsmGroupRec phsmGroupRec)
{
	int 				ret;
	int					lenOfVK=0;
	int					encrypMode=-1;
	int					fillMode=0;
	int					format=-1;
	int					dataPrefixLen;
	int					lenOfdataPrefix;
	int					dataSuffixLen;
	int					lenOfdataSuffix;
	int 				algorithmID=0;	//�㷨ģʽ 1��RSA	   2��SM2
	int 				lenOfPinByPK;
	int					algorithmID_ZEK=0;	//�㷨ģʽ 1��DES	   2��SM4	
	char				pkKeyName[128];
	char				zekKeyName[128];
	char				vkIndex[8];
	char				vkValue[4096+1];
	char				encrypMode_buf[2+1];
	char				fillMode_buf[2+1];
	char				format_buf[2+1];
	char				dataPrefixLen_buf[2+1];
	char				dataPrefix[128+1];
	char				dataSuffixLen_buf[2+1];
	char				dataSuffix[128+1];
	char				iv[32+1];
	char				pinByPK[2048+1];
	char				pinByZEK[128+1];
	char				specialAlg[8];
	char				complexityOfPin[3];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionSymmetricKeyDB		zekKeyDB;
	PUnionSymmetricKeyValue		pzekKeyValue = NULL;

	

	//��ȡ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName",pkKeyName,sizeof(pkKeyName))) <=0)
	{
			UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkKeyName");
			UnionSetResponseRemark("��Կ���Ʋ���Ϊ��");
			return(errCodeParameter);
	}
	else
	{
		pkKeyName[ret]=0;
		
		UnionFilterHeadAndTailBlank(pkKeyName);

		// ��ȡ��Կ��Կ��Ϣ     
		memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
		if ((ret =  UnionReadAsymmetricKeyDBRec(pkKeyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadAsymmetricKeyDBRec pkKeyName[%s]!\n",pkKeyName);
			return(ret);
		}

		// ���˽Կ����
		if (asymmetricKeyDB.keyType != 1 && asymmetricKeyDB.keyType != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE209:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
			UnionSetResponseRemark("˽Կ����[%d]��֧�ּ���,����Ϊ[1��2]");
			return(errCodeHsmCmdMDL_InvalidKeyType);
		}

		// ����Ƿ��������
		if (asymmetricKeyDB.vkStoreLocation != 0)
		{
			ret=sprintf(vkIndex,"%s",asymmetricKeyDB.vkIndex);
			vkIndex[ret]=0;
		}
		else
		{
			ret=sprintf(vkIndex,"%s","99");
			vkIndex[ret]=0;
			
			lenOfVK = strlen(asymmetricKeyDB.vkValue);
			memcpy(vkValue,asymmetricKeyDB.vkValue,lenOfVK);
			vkValue[lenOfVK] = 0;	
		}
	}

	//�ù�Կ����ʱ�����õ���䷽ʽ
	if(asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
	{
		//��䷽ʽ
		if ((ret = UnionReadRequestXMLPackageValue("body/fillMode",fillMode_buf,sizeof(fillMode_buf))) <=0)
		{
				UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/fillMode");
				return(errCodeParameter);
		}
		fillMode_buf[ret] = 0;
		fillMode=atoi(fillMode_buf);

		algorithmID=1;//�㷨ģʽ	
	}
	else if(asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfSM2)
		algorithmID=2;

	//��ȡ��Կ���ܵ�PIN
	if ((lenOfPinByPK=ret = UnionReadRequestXMLPackageValue("body/pinByPK",pinByPK,sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(ret);
	}
	pinByPK[lenOfPinByPK]=0;

	//ר���㷨��ʾspecialAlg
	memset(specialAlg,0,sizeof(specialAlg));
	if ((ret = UnionReadRequestXMLPackageValue("body/specialAlg",specialAlg,sizeof(specialAlg))) >0)
	{
		specialAlg[ret]=0;
	}


	//UnionDebugLog("in UnionDealServiceCodeE209:: specialAlg[%s]!\n",specialAlg);

	if(specialAlg[0] != 'Q')
	{
		
		//��ȡzek��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/zekKeyName",zekKeyName,sizeof(zekKeyName))) < 0)
		{
				UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/zekKeyName");
				return(ret);
		}
		zekKeyName[ret] = 0;
		
		memset(&zekKeyDB,0,sizeof(zekKeyDB));
		if ((ret =	UnionReadSymmetricKeyDBRec(zekKeyName,1,&zekKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadSymmetricKeyDBRec zekKeyName[%s]!\n",zekKeyName);
			return(ret);
		}
		
		if (zekKeyDB.keyType != conZEK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE209:: is not zek [%s]!\n",zekKeyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		
		// ��ȡ��Կֵ
		if ((pzekKeyValue = UnionGetSymmetricKeyValue(&zekKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE209::UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ[%s]��Կֵʧ��",zekKeyDB);
			return(errCodeParameter);
		}	
		
		if(zekKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
			algorithmID_ZEK=1;
		else if (zekKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4)
			algorithmID_ZEK=2;
		UnionLog("algorithmID_ZEK[%d]\n",algorithmID_ZEK);
		

		//��ȡ�㷨ģʽ
		if ((ret = UnionReadRequestXMLPackageValue("body/encrypMode",encrypMode_buf,sizeof(encrypMode_buf))) <=0)
		{
				UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/encrypMode");
				return(errCodeParameter);
		}
		encrypMode_buf[ret] = 0;	
		encrypMode=atoi(encrypMode_buf);
		
		if((encrypMode !=1) && (encrypMode !=2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE209:: encrypMode is not 1 or 2!\n");
			return(errCodeParameter);
		}
		
		if(encrypMode == 2)
		{
			//��ȡIV��ʼ����
			if ((ret = UnionReadRequestXMLPackageValue("body/iv",iv,sizeof(iv))) <=0)
			{
					UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv");
					return(errCodeParameter);
			}
			iv[ret] = 0;		
		}
		
	
		// ������䷽ʽ
		if ((ret = UnionReadRequestXMLPackageValue("body/format",format_buf,sizeof(format_buf))) < 0)
		{
				UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/format");
				return(ret);
		}
		format_buf[ret] = 0;
		format=atoi(format_buf);	
		
		//��ȡǰ׺���ݳ���
		if ((ret = UnionReadRequestXMLPackageValue("body/dataPrefixLen",dataPrefixLen_buf,sizeof(dataPrefixLen_buf))) <=0)
		{
			dataPrefixLen=0;
		}
		else
		{
			dataPrefixLen_buf[ret]=0;
			dataPrefixLen=atoi(dataPrefixLen_buf);
			
			if ((lenOfdataPrefix = ret = UnionReadRequestXMLPackageValue("body/dataPrefix",dataPrefix,sizeof(dataPrefix))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/dataPrefix");
				return(ret);
			}
			dataPrefix[ret]=0;
		
			if(lenOfdataPrefix != dataPrefixLen)
			{
				UnionUserErrLog("in UnionDealServiceCodeE209:: strlen(dataPrefix) != dataPrefixLen[%d]!\n",dataPrefixLen);
				return(errCodeParameter);
			}
		}
		
		//��ȡ������ݺ�׺����
		if ((ret = UnionReadRequestXMLPackageValue("body/dataSuffixLen",dataSuffixLen_buf,sizeof(dataSuffixLen_buf))) <=0)
		{
			dataSuffixLen=0;
		}
		else
		{
			dataSuffixLen_buf[ret]=0;
			dataSuffixLen=atoi(dataSuffixLen_buf);
			
			if (( lenOfdataSuffix = ret = UnionReadRequestXMLPackageValue("body/dataSuffix",dataSuffix,sizeof(dataSuffix))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE209:: UnionReadRequestXMLPackageValue[%s]!\n","body/dataSuffix");
				return(ret);
			}
			dataSuffix[ret]=0;
		
			if(lenOfdataSuffix != dataSuffixLen)
			{
				UnionUserErrLog("in UnionDealServiceCodeE209:: strlen(dataSuffix)!= dataSuffixLen[%d]!\n",dataSuffixLen);
				return(errCodeParameter);
			}
		}
	}
	

	memset(pinByZEK, 0, sizeof(pinByZEK));
	memset(complexityOfPin, 0, sizeof(complexityOfPin));

	//���ü��ܻ�ָ���
	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerRacalStandardHsmCmd:
		case conHsmCmdVerSJL06StandardHsmCmd:
			if(specialAlg[0] != 'Q')
			{
				if ((ret = UnionHsmCmd6A(algorithmID,algorithmID_ZEK,atoi(vkIndex),lenOfVK,vkValue,fillMode,specialAlg,pzekKeyValue->keyValue,format,encrypMode,strlen(iv),iv,dataPrefixLen,dataPrefix,dataSuffixLen,dataSuffix,lenOfPinByPK,pinByPK,pinByZEK,complexityOfPin)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE209:: UnionHsmCmd6A!\n");
					return(ret);
				}
			}
			else 
			{
				if ((ret = UnionHsmCmd6A(algorithmID,algorithmID_ZEK,atoi(vkIndex),lenOfVK,vkValue,fillMode,specialAlg,NULL,0,0,0,NULL,0,NULL,0,NULL,lenOfPinByPK,pinByPK,pinByZEK,complexityOfPin)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE209:: UnionHsmCmd6A!\n");
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE209:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}
	
	// ������Ӧ����
	if(specialAlg[0] == 'Q')
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/complexityOfPin",complexityOfPin)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE209:: UnionSetResponseXMLPackageValue[%s]!\n","body/complexityOfPin");
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/pinByZEK",pinByZEK)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE209:: UnionSetResponseXMLPackageValue[%s]!\n","body/pinByZEK");
			return(ret);
		}
	}
	
	return(0);
}

