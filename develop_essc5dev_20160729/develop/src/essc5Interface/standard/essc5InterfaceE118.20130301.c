//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

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
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"

/***************************************
�������:	E118
������:		��ӡ��Կ
��������:	��ӡ��Կ
***************************************/
int UnionDealServiceCodeE118(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	char				tmpNum[32];
	char				hsmIP[64];
	char				keyPrintFormat[2048];
	char				formatName[64];
	int				numOfComponent = 0;
	char				sql[128];
	int				keyLen = 0;
	char				algorithmID[32];
	int				appendPrintNum = 0;
	int				mode = 0;
	int				maxParamNum = 20;
	char				appendPrintParam[maxParamNum][80];
	int				i = 0;
	char				tmpBuf[128];
	char				isCheckAlonePrint[32];
	char				keyType[32];
	char				keyName[160];
	

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s",keyName);

	// ��ӡģʽ
	// 1����Կֵ���ڣ���ӡ���洢
	// 2����Կ���ƴ���,��Կֵ�����ڣ���ӡ���洢
	// 3����Կ���Ʋ����ڣ���ӡ���洢
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	else
		mode = atoi(tmpNum);
	
	switch (mode)
	{
		case 	1:
		case	2:
			// ��ȡ�Գ���Կ
			if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
			{
				if (ret == errCodeKeyCacheMDL_WrongKeyName)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: �Գ���Կ[%s]������!\n",keyName);
					UnionSetResponseRemark("�Գ���Կ[%s]������",keyName);
					return(errCodeKeyCacheMDL_KeyNonExists);
				}
				UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadSymmetricKeyDBRec symmetricKeyDB.keyName[%s] ret[%d]!\n",keyName,ret);
				return(ret);
			}

			if (mode == 2)
			{
				// �����Կֵ�Ƿ����
				if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: mode[%d] keyName[%s] is find keyValue!\n",mode,keyName);
					UnionSetResponseRemark("�Գ���Կ[%s],��Կֵ����",keyName);
					return(errCodeParameter);
				}
			}
			break;

		case	0:
		case	3:
			// �㷨��ʶ
			if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
				return(ret);
			}

			// ��Կ����
			if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpNum,sizeof(tmpNum))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
				return(ret);
			}
			else
				keyLen = atoi(tmpNum);
			
			// �����Կ���ȼ��㷨��ʶ
			if (strcmp(algorithmID,"DES") == 0)
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
				switch (keyLen)
				{
					case	64:
					case	16:
						symmetricKeyDB.keyLen = con64BitsSymmetricKey;
						break;
					case	128:
					case	32:
						symmetricKeyDB.keyLen = con128BitsSymmetricKey;
						break;
					case	192:
					case	48:
						symmetricKeyDB.keyLen = con192BitsSymmetricKey;
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE110:: keyLen[%d] is invalid!\n",keyLen);
						return(errCodeEssc_KeyLength);
				}
			}
			else if (strcmp(algorithmID,"SM4") == 0)
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;

				if (keyLen != 128)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: keyLen[%d] != 128!\n",keyLen);
					UnionSetResponseRemark("��Կ����[%d],����Ϊ128",keyLen);
					return(errCodeEssc_KeyLength);
				}
				symmetricKeyDB.keyLen = con128BitsSymmetricKey;
			}
			else
			{
				UnionUserErrLog("in UnionDealServiceCodeE118:: algorithmID[%s] != DES or SM4 !\n",algorithmID);
				UnionSetResponseRemark("�㷨��ʶ[%s],����Ϊ[DES��SM4]",algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
			}

			//if (mode == 0)
			{
				// ��Կ����
				if ((ret = UnionReadRequestXMLPackageValue("body/keyType",keyType,sizeof(keyType))) <= 0)
				{
					//UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
					//return(ret);    
					strcpy(keyType,"ZMK");
					if ((ret = UnionSetRequestXMLPackageValue("body/keyType",keyType)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSetRequestXMLPackageValue[%s]!\n","body/keyType");
						return(ret);

					}
					if ((ret = UnionSetRequestXMLPackageValue("body/enabled","1")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSetRequestXMLPackageValue[%s]!\n","body/enabled");
						return(ret);
					}
				}
				if ((symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(keyType)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: keyType[%s] error!\n",keyType);
					return(symmetricKeyDB.keyType); 
				}

				// add by liwj 20150706
				// mode = 3 ʱ��E110 mode��û��3�ģ���Ҫ��mode ����Ϊ0
				if (mode == 3)
				{
					snprintf(tmpBuf, sizeof(tmpBuf), "%d", 0);
					if ((ret = UnionSetRequestXMLPackageValue("body/mode",tmpBuf)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSetRequestXMLPackageValue[%s]!\n","body/mode");
						return(ret);
					}
				}
				// end

				if ((ret = UnionDealServiceCodeE110(phsmGroupRec)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: UnionDealServiceCodeE110!\n");
					UnionSetResponseRemark("�洢��Կ��Ϣʧ��");
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE118:: mode[%d] != 1 or 2 or 3!\n",mode);
			UnionSetResponseRemark("�Ƿ���ӡ��Կģʽ[%d]",mode);
			return(errCodeParameter);
	}

	// ��ӡ��ʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/formatName",formatName,sizeof(formatName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/formatName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(formatName);

	// ���ݴ�ӡ���Ʋ��Ҵ�ӡ��ʽ
	len = sprintf(sql,"select * from keyPrintFormat where formatName = '%s'",formatName);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: formatName[%s] not found, sql[%s]!\n",formatName,sql);
		UnionSetResponseRemark("��ӡ����[%s]û���ҵ�",formatName);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// ��ȡУ��ֵ��ӡ��ʽ
	if ((ret = UnionReadXMLPackageValue("isCheckAlonePrint",isCheckAlonePrint,sizeof(isCheckAlonePrint))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadXMLPackageValue[%s]!\n","isCheckAlonePrint");
		return(ret);
	}

	UnionFilterHeadAndTailBlank(isCheckAlonePrint);
	if (atoi(isCheckAlonePrint) != 0 && atoi(isCheckAlonePrint) != 1)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: isCheckAlonePrint[%s] != 0 or != 1!\n",isCheckAlonePrint);
		return(errCodeParameter);
	}

	// ��ȡ��ӡ��ʽ
	if ((ret = UnionReadXMLPackageValue("format",keyPrintFormat,sizeof(keyPrintFormat))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadXMLPackageValue[%s]!\n","format");
		return(ret);
	}
	keyPrintFormat[ret] = 0;

	// ��������
	if ((ret = UnionReadRequestXMLPackageValue("body/numOfComponent",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/numOfComponent");
		return(ret);
	}
	else
		numOfComponent = atoi(tmpNum);

	// ����������
	if (numOfComponent < 2 || numOfComponent > 9)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: numOfComponent[%d] < 2 or numOfComponent[%d] > 9!\n",numOfComponent,numOfComponent);
		return(errCodeParameter);
	}

	// �����IP
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmIP",hsmIP,sizeof(hsmIP))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmIP");
		return(ret);
	}
	
	// ���IP��ַ
	if (!UnionIsValidIPAddrStr(hsmIP))
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: hsmIP[%s] is invalid!\n",hsmIP);
		UnionSetResponseRemark("�����IP[%s]�Ƿ�",hsmIP);
		return(errCodeInvalidIPAddr);
	}

	// ���Ӵ�ӡ����
	if ((ret = UnionReadRequestXMLPackageValue("body/appendPrintNum",tmpNum,sizeof(tmpNum))) < 0)
		appendPrintNum = 0;
	else
		appendPrintNum = atoi(tmpNum);

	if (appendPrintNum > maxParamNum)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: appendPrintNum[%d] > maxParamNum[%d]!\n",appendPrintNum,maxParamNum);
		UnionSetResponseRemark("������ӡ����[%d]�������ֵ[%d]",appendPrintNum,maxParamNum);
		return(errCodeParameter);
	}

	for (i = 0; i < appendPrintNum; i++)
	{
		// ���Ӵ�ӡ����
		sprintf(tmpBuf,"body/appendPrintParam%d",i+1);
		
		// ��ӡ����
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,appendPrintParam[i],sizeof(appendPrintParam[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n",tmpBuf);
			return(ret);
		}
	}

	// ���ɲ���ӡ��Կ
	if ((ret = UnionGenerateAndPrintSymmetricKey(phsmGroupRec,&symmetricKeyDB,keyPrintFormat,atoi(isCheckAlonePrint),numOfComponent,hsmIP,appendPrintNum,appendPrintParam)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionGenerateAndPrintSymmetricKey!\n");
		UnionSetResponseRemark("��ӡ��Կʧ��",symmetricKeyDB.keyName);
		return(ret);
	}

	if (mode != 3)
	{
		// ���¶Գ���Կ
		if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE118:: UnionUpdateSymmetricKeyDBKeyValue keyName[%s]!\n",symmetricKeyDB.keyName);
			return(ret);
		}
	}

	return(0);
}


