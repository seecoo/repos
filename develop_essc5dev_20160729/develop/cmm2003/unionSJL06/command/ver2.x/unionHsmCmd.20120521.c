//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionDesKey.h"

#include "emv2000AndPbocSyntaxRules.h"
#include "3DesRacalSyntaxRules.h"
#include "unionHsmCmd.h"
#include "commWithHsmSvr.h"

// ʹ�������ZMK����
int gunionIsUseSpecZmkType = 1;


// ����ʹ��������ZMK����
int UnionSetIsUseNormalZmkType()
{
	gunionIsUseSpecZmkType = 0;
	return(0);
}

// add by leipp 20150419
// PVKIֵ
int gunionUseSpecPVKI = 0;

// ����PVKI
int UnionSetUseSpecPVKI(int pvki)
{
	gunionUseSpecPVKI = pvki;
	return(0);
}

int UnionGetUseSpecPVKI()
{
	return gunionUseSpecPVKI;
}
// end

int UnionTranslateDesKeyTypeTo3CharFormat(TUnionDesKeyType desKeyType, char *keyType)
{
	switch(desKeyType)
	{
		case conZMK:
			sprintf(keyType, "%s", "000");
			break;
		case conZEK:
			sprintf(keyType, "%s", "00A");
			break;
		case conZPK:
			sprintf(keyType, "%s", "001");
			break;
		case conZAK:
			sprintf(keyType, "%s", "008");
			break;
		case conPVK:
		case conTPK:
		case conTMK:
			sprintf(keyType, "%s", "002");
			break;
		case conTAK:
			sprintf(keyType, "%s", "003");
			break;
		case conCVK:
			sprintf(keyType, "%s", "402");
			break;
		case conWWK:
			sprintf(keyType, "%s", "006");
			break;
		case conEDK://20120827����
			sprintf(keyType, "%s", "007");
			break;
		case conBDK:
			sprintf(keyType, "%s", "009");
			break;
		case conMKSMC://20131106
			sprintf(keyType, "%s", "309");
			break;
		case conMKAC:	//20140416 ������ add
			sprintf(keyType, "%s", "109");
			break;
		case conMKSMI:	// add by leipp 20151023
			sprintf(keyType, "%s", "209");
			break;	// add by leipp end
		default:
			UnionUserErrLog("in UnionTranslateDesKeyTypeTo3CharFormat:: desKeyType = [%d] not supported!\n", desKeyType);
			return(errCodeEsscMDL_KeyObjectTypeNotSupported);
	}
	return 0;
}

// ����һ��RSA��Կ
/* �������
   type������	0��ֻ����ǩ��
   1��ֻ������Կ����
   2��ǩ���͹���
   length��ģ��	0320/512/1024/2048
   pkEncoding	��Կ�ı��뷽ʽ
   lenOfPKExponent	��Կexponent�ĳ���	��ѡ����
   pkExponent	��Կ��pkExponent	��ѡ����
   exportNullPK	1�������PK��0�����뷽ʽ��PK
   sizeOfPK	���չ�Կ�Ļ����С
   sizeOfVK	����˽Կ�Ļ����С
   �������
   pk		��Կ
   lenOfVK		˽Կ���ĳ���
   vk		˽Կ
 */
int UnionHsmCmdEI(char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		lenOfPK;
	int		lenOfPKStr;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"EI",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",length);
	hsmCmdLen += 4;
	if ((pkEncoding == NULL) || (strlen(pkEncoding) == 0))
		memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
	else
		memcpy(hsmCmdBuf+hsmCmdLen,pkEncoding,2);
	hsmCmdLen += 2;
	if ((pkExponent != NULL) && (lenOfPKExponent > 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPKExponent);
		hsmCmdLen += 4;
		if (lenOfPKExponent + hsmCmdLen >= sizeof(hsmCmdBuf))
		{
			UnionUserErrLog("in UnionHsmCmdEI:: lenOfPKExponent [%d] too long !\n",lenOfPKExponent);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,pkExponent,lenOfPKExponent);
		hsmCmdLen += lenOfPKExponent;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if ((lenOfPKStr = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)hsmCmdBuf+4,ret-4,pk,&lenOfPK,sizeOfPK)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEI:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
		UnionMemLog("in UnionHsmCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(lenOfPKStr);
	}
	if (lenOfPKStr + 4 > ret - 4)
	{
		UnionUserErrLog("in UnionHsmCmdEI:: lenOfPKStr + 4 = [%d] longer than [%d]!\n",lenOfPKStr+4,ret-4);
		UnionMemLog("in UnionHsmCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	if (!exportNullPK)
	{
		if (lenOfPKStr > sizeOfPK)
		{
			UnionUserErrLog("in UnionHsmCmdEI:: sizeOfPK [%04d] < expected [%04d]!\n",sizeOfPK,lenOfPKStr);
			return(errCodeSmallBuffer);
		}
		memcpy(pk,hsmCmdBuf+4,lenOfPKStr);
		pk[lenOfPKStr] = 0;
		lenOfPK = lenOfPKStr;
	}		

	*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+lenOfPKStr+4,4);
	if ((*lenOfVK <= 0) || (*lenOfVK + 4 + 4 + lenOfPKStr > ret) || (*lenOfVK > sizeOfVK))
	{
		UnionUserErrLog("in UnionHsmCmdEI:: lenOfVK [%d] error!\n",*lenOfVK);
		UnionMemLog("in UnionHsmCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	memcpy(vk,hsmCmdBuf+4+lenOfPKStr+4,*lenOfVK);
	vk[*lenOfVK] = 0;
	return(lenOfPK);
}

// �洢˽Կ
/* �������
   vkIndex		˽Կ��������
   lenOfVK		˽Կ����
   vk		˽Կ
   �������
 */
int UnionHsmCmdEK(int vkIndex,int lenOfVK,unsigned char *vk)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		offset = 0;

	memcpy(hsmCmdBuf,"EK",2);
	offset = 2;
	sprintf(hsmCmdBuf+offset,"%02d",vkIndex%100);
	offset += 2;
	sprintf(hsmCmdBuf+offset,"%04d",lenOfVK);
	offset += 4;
	if (offset + lenOfVK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEK:: lenOfVK = [%04d] too long\n",lenOfVK);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+offset,vk,lenOfVK);
	offset += lenOfVK;
	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[offset] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

// ���ɹ�Կ��MACֵ
int UnionHsmCmdEO(char *pkEncoding,int isNullPK,int lenOfPK,char *pk,char *authData,char *mac)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		lenOfPKStr;

	memcpy(hsmCmdBuf,"EO",2);
	hsmCmdLen = 2;
	if ((pkEncoding == NULL) || (strlen(pkEncoding) == 0))
		memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
	else
		memcpy(hsmCmdBuf+hsmCmdLen,pkEncoding,2);
	hsmCmdLen += 2;
	if (isNullPK)
	{
		if ((lenOfPKStr = UnionFormANSIDERRSAPK(pk,lenOfPK,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdEO:: UnionFormANSIDERRSAPK for [%s]\n",pk);
			return(lenOfPKStr);
		}
		hsmCmdLen += lenOfPKStr;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,pk,lenOfPK);
		hsmCmdLen += lenOfPK;
	}

	if ((authData != NULL) && (strlen(authData) != 0))
	{
		strcpy(hsmCmdBuf+hsmCmdLen,authData);
		hsmCmdLen += strlen(authData);
	}
	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEO:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(mac,hsmCmdBuf+4,4);
	mac[4] = 0;
	return(4);
}

// ��֤��Կ��MACֵ
int UnionHsmCmdEQ(char *pk,char *authData,char *mac)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		lenOfPKStr;

	memcpy(hsmCmdBuf,"EQ",2);
	hsmCmdLen = 2;
	aschex_to_bcdhex(mac,8,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 4;
	if ((lenOfPKStr = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEQ:: UnionFormANSIDERRSAPK for [%s]\n",pk);
		return(lenOfPKStr);
	}
	hsmCmdLen += lenOfPKStr;
	if ((authData != NULL) && (strlen(authData) != 0))
	{
		strcpy(hsmCmdBuf+hsmCmdLen,authData);
		hsmCmdLen += strlen(authData);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEQ:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

// ��һ��LMK���ܵ���Կת��Ϊ��Կ����
int UnionHsmCmdGK(char *encyFlag,char *padMode,char *lmkType,char *desKeyByLMK,
		char *checkValue,char *pkMac,char *pk,char *authData,
		char *initValue,char *desKeyByPK,int sizeOfBuf)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		lenOfPKStr;
	int		desKeyLen;

	if ((encyFlag == NULL) || (padMode == NULL) || (lmkType == NULL) || (desKeyByLMK == NULL) ||
			(checkValue == NULL) || (pkMac == NULL) || (pk == NULL) || (initValue == NULL) || (desKeyByPK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdGK:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"GK",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,encyFlag,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,padMode,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lmkType,4);
	hsmCmdLen += 4;
	switch (desKeyLen = strlen(desKeyByLMK))
	{
		case	16:
			hsmCmdBuf[hsmCmdLen] = '0';
			hsmCmdLen++;
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"1X",2);
			hsmCmdLen += 2;
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"2Y",2);
			hsmCmdLen += 2;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdGK:: desKeyByLMK = [%s] error!\n",desKeyByLMK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,desKeyByLMK,desKeyLen);
	hsmCmdLen += desKeyLen;
	memcpy(hsmCmdBuf+hsmCmdLen,checkValue,16);
	hsmCmdLen += 16;
	aschex_to_bcdhex(pkMac,8,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 4;
	if ((lenOfPKStr = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdGK:: UnionFormANSIDERRSAPK for [%s]\n",pk);
		return(lenOfPKStr);
	}
	hsmCmdLen += lenOfPKStr;
	if ((authData != NULL) && (strlen(authData) != 0))
	{
		strcpy(hsmCmdBuf+hsmCmdLen,authData);
		hsmCmdLen += strlen(authData);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdGK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(initValue,hsmCmdBuf+4,16);
	initValue[16] = 0;
	if (((desKeyLen = UnionConvertIntoLen(hsmCmdBuf+4+16,4)) < 0) || (desKeyLen * 2 > sizeOfBuf))
	{
		UnionUserErrLog("in UnionHsmCmdGK:: desKeyLen [%d] error!\n",desKeyLen);
		return(errCodeParameter);
	}

	memcpy(desKeyByPK,hsmCmdBuf+4+16+4,desKeyLen);
	desKeyByPK[desKeyLen] = 0;
	return(desKeyLen);
}
/* 
   ʮһ��	����ժҪ

 */
int UnionHsmCmdGM(char *method,int lenOfData,char *data,
		char *hashResult,int sizeOfBuf)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		retDataLen;

	if ((method == NULL) || (data == NULL) || (hashResult == NULL) || (lenOfData <= 0))
	{
		UnionUserErrLog("in UnionHsmCmdGM:: null pointer or lenOfData Error [%d]\n",lenOfData);
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"GM",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+2,method,2);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%05d",lenOfData);
	hsmCmdLen += 5;
	if (lenOfData + hsmCmdLen >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in in UnionHsmCmdGM:: lenOfData too long [%d]\n",lenOfData);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdGM:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if (((retDataLen = ret - 4) < 0) || (retDataLen * 2 >= sizeOfBuf))
	{
		UnionUserErrLog("in UnionHsmCmdGM:: retDataLen [%d] error!\n",retDataLen);
		return(errCodeParameter);
	}
	memcpy(hashResult,hsmCmdBuf+4,retDataLen);
	hashResult[retDataLen] = 0;
	return(retDataLen);
}

// ����ǩ����ָ��
int UnionHsmCmdEW(char *hashID,char *signID,char *padID,int lenOfData,char *data,
		int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign)
{
	int		ret;
	char		hsmCmdBuf[8192*2];
	int		offset = 0;
	int		lenOfSign;

	memcpy(hsmCmdBuf,"EW",2);
	offset = 2;
	// ժҪ�㷨��ʶ
	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;
	// ǩ���㷨��ʶ		
	if ((signID == NULL) || (strlen(signID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,signID,2);
	offset += 2;
	// ��λ��־
	if ((padID == NULL) || (strlen(padID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,padID,2);
	offset += 2;
	// ���ݳ���
	sprintf(hsmCmdBuf+offset,"%04d",lenOfData);
	offset += 4;
	// ����
	if (lenOfData + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEW:: lenOfData [%04d] too long!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,data,lenOfData);
	offset += lenOfData;
	// �ָ���
	hsmCmdBuf[offset] = ';';
	offset++;
	// ˽Կ����
	if (vkIndex < 0)
		vkIndex = 99;
	sprintf(hsmCmdBuf+offset,"%02d",vkIndex);
	offset += 2;
	if (vkIndex == 99)
	{
		// ˽Կ����
		sprintf(hsmCmdBuf+offset,"%04d",lenOfVK);
		offset += 4;
		if ((lenOfVK + offset >= sizeof(hsmCmdBuf)) || (lenOfVK <= 0) || (vk == NULL))
		{
			UnionUserErrLog("in UnionHsmCmdEW:: lenOfVK [%04d] error!\n",lenOfVK);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+offset,vk,lenOfVK);
		offset += lenOfVK;
	}
	hsmCmdBuf[offset] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEW:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;
	if ((lenOfSign = UnionConvertIntStringToInt(hsmCmdBuf+offset,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEW:: lenOfSign = [%04d]!\n",lenOfSign);
		return(errCodeHsmCmdMDL_ReturnLen);
	}
	offset += 4;
	//if (lenOfSign >= sizeOfSign)
	if (lenOfSign * 2 >= sizeOfSign)
	{
		UnionUserErrLog("in UnionHsmCmdEW:: lenOfSign = [%04d] too long!\n",lenOfSign);
		return(errCodeSmallBuffer);
	}
	//memcpy(sign,hsmCmdBuf+offset,lenOfSign);
	bcdhex_to_aschex(hsmCmdBuf+offset,lenOfSign,sign);
	//return(lenOfSign);
	return(lenOfSign*2);
}

// ��֤ǩ����ָ��
int UnionHsmCmdEY(char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData)
{
	int		ret;
	int		lenOfPKStr;
	char		hsmCmdBuf[8192*2];
	int		offset = 0;

	memcpy(hsmCmdBuf,"EY",2);
	offset = 2;
	// ժҪ�㷨��ʶ
	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;
	// ǩ���㷨��ʶ		
	if ((signID == NULL) || (strlen(signID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,signID,2);
	offset += 2;
	// ��λ��־
	if ((padID == NULL) || (strlen(padID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,padID,2);
	offset += 2;
	// ǩ������
	sprintf(hsmCmdBuf+offset,"%04d",lenOfSign);
	offset += 4;
	// ǩ��
	if (lenOfSign + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEY:: lenOfSign [%04d] too long!\n",lenOfSign);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,sign,lenOfSign);
	offset += lenOfSign;
	// �ָ���
	hsmCmdBuf[offset] = ';';
	offset++;
	// ���ݳ���
	sprintf(hsmCmdBuf+offset,"%04d",lenOfData);
	offset += 4;
	// ����
	if (lenOfData + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEY:: lenOfData [%04d] too long!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,data,lenOfData);
	offset += lenOfData;
	// �ָ���
	hsmCmdBuf[offset] = ';';
	offset++;
	// MAC
	memcpy(hsmCmdBuf+offset,mac,4);
	offset += 4;
	// ��Կ
	if (isNullPK)
	{
		if ((lenOfPKStr = UnionFormANSIDERRSAPK(PK,lenOfPK,hsmCmdBuf+offset,sizeof(hsmCmdBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdEY:: UnionFormANSIDERRSAPK for [%s]\n",PK);
			return(lenOfPKStr);
		}
		offset += lenOfPKStr;
	}
	else
	{
		if (lenOfPK + offset >= sizeof(hsmCmdBuf))
		{
			UnionUserErrLog("in UnionHsmCmdEY:: lenOfPK [%04d] too long!\n",lenOfPK);
			return(errCodeSmallBuffer);
		}
		memcpy(hsmCmdBuf+offset,PK,lenOfPK);
		offset += lenOfPK;
	}
	// ��֤����
	if (lenOfAuthData + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEY:: lenOfAuthData [%04d] too long!\n",lenOfAuthData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,authData,lenOfAuthData);
	offset += lenOfAuthData;

	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[offset] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEY:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

/*
   ��һ����Կ���ܵ���Կת��ΪLMK����
 */
int UnionHsmCmdGI(char *encyFlag, char *padMode, 
		char *lmkType, int keyLength, int lenOfDesKeyByPK,
		char *desKeyByPK, int vkIndex, int lenOfVK, unsigned char *vk,
		char *initValue, char *desKeyByLMK, char *checkValue)
{
	int		ret=-1;
	int		offset;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	char 		tmpBuf[2048];

	if ((encyFlag == NULL) || (padMode == NULL) || (lmkType == NULL) || (desKeyByPK == NULL) ||
			(desKeyByLMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdGI:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "GI", 2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen, encyFlag, 2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, padMode, 2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, lmkType, 4);
	hsmCmdLen += 4;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", lenOfDesKeyByPK/2);
	hsmCmdLen += 4;

	// desKeyByPK
	if (lenOfDesKeyByPK/2 + hsmCmdLen >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdGI:: lenOfDesKeyByPK [%04d] too long!\n",lenOfDesKeyByPK);
		return(errCodeSmallBuffer);
	}

	aschex_to_bcdhex(desKeyByPK, lenOfDesKeyByPK, tmpBuf);
	tmpBuf[lenOfDesKeyByPK/2] = 0;
	memcpy(hsmCmdBuf+hsmCmdLen, tmpBuf, lenOfDesKeyByPK/2);
	hsmCmdLen += lenOfDesKeyByPK/2;

	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;

	// ˽Կ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	hsmCmdLen += 2;

	if (vkIndex == 99)
	{
		// ˽Կ����
		sprintf(hsmCmdBuf+hsmCmdLen, "%04d", lenOfVK);
		hsmCmdLen += 4;
		if ((lenOfVK + hsmCmdLen >= sizeof(hsmCmdBuf)) || (lenOfVK <= 0) || (vk == NULL))
		{
			UnionUserErrLog("in UnionHsmCmdGI:: lenOfVK [%04d] error!\n",lenOfVK);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen, vk, lenOfVK);
		hsmCmdLen += lenOfVK;
	}

	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;

	// ZMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdGI:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// LMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdGI:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen++;

	UnionSetBCDPrintTypeForHSMCmd();	
	UnionMemLog("in UnionHsmCmdGI:: req =",(unsigned char *)hsmCmdBuf,hsmCmdLen);
	hsmCmdBuf[hsmCmdLen] = 0;


	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdGI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	offset = 4;

	if (initValue != NULL)
		memcpy(initValue,hsmCmdBuf+offset,16);
	offset += 16;

	switch(keyLength)
	{
		case con64BitsDesKey:
			memcpy(desKeyByLMK,hsmCmdBuf+offset,16);
			offset += 16;
			memcpy(checkValue,hsmCmdBuf+offset,16);
			return (16);
		case con128BitsDesKey:
			offset += 1;
			memcpy(desKeyByLMK,hsmCmdBuf+offset,32);
			offset += 32;
			memcpy(checkValue,hsmCmdBuf+offset,16);
			return (32);
		case con192BitsDesKey:
			offset += 1;
			memcpy(desKeyByLMK,hsmCmdBuf+offset,48);
			offset += 48;
			memcpy(checkValue,hsmCmdBuf+offset,16);
			return (48);
		default:
			UnionUserErrLog("in UnionHsmCmdGI:: des key length %d error!\n",keyLength);
			return errCodeKeyCacheMDL_InvalidKeyLength;
	}
	return 0;
}

// ����һ��SM2��Կ
/* �������
   type������
   0,Э��	
   1��ǩ��
   2, ����
   3, ǩ�������ܺ�Э��
   length������ �̶�256
   sizeOfPK	���չ�Կ�Ļ����С
   sizeOfVK	����˽Կ�Ļ����С
   �������
   pk		��Կ
   lenOfVK		˽Կ���ĳ���
   vk		˽Կ
 */
int UnionHsmCmdK1(char type,int length,	char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK)
{
	int			ret;
	char			hsmCmdBuf[4096];
	int			hsmCmdLen = 0;
	char			bcdPK[4096];

	UnionSetBCDPrintTypeForHSMResCmd();	

	memcpy(hsmCmdBuf,"K1",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",length);
	hsmCmdLen += 4;
	type = '3';
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%2d",99);
	hsmCmdLen += 2;	

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK1:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+4,4);
	if ((*lenOfVK <= 0) || (*lenOfVK + 4 + 4 > ret) || (*lenOfVK > sizeOfVK))
	{
		UnionUserErrLog("in UnionHsmCmdK1:: lenOfVK [%d] error!\n",*lenOfVK);
		UnionMemLog("in UnionHsmCmdK1:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	memcpy(vk,hsmCmdBuf+4+4,*lenOfVK);
	vk[*lenOfVK] = 0;
	memcpy(bcdPK,hsmCmdBuf+4+4+*lenOfVK,64);
	bcdPK[64] = 0;
	bcdhex_to_aschex(bcdPK,64,pk);
	pk[128] = 0;
	return(128);
}

// ʹ��SM3�㷨����HASH����
/* �������
   algorithmID	�㷨��ʶ��3-SM3
   lenOfData	���ݳ���
   hashData	��Hash������
   sizeOfBuf	����Hash����Ļ����С
   �������
   hashValue	Hash���
 */
int UnionHsmCmdM7(char *algorithmID,int lenOfData,char *hashData,char *hashValue,int sizeOfBuf)
{
	int			ret;
	char			hsmCmdBuf[8096];
	int			hsmCmdLen = 0;

	// ָ�����
	memcpy(hsmCmdBuf,"M7",2);
	hsmCmdLen = 2;

	//ģʽ��־
	memcpy(hsmCmdBuf + hsmCmdLen,"1",1);
	hsmCmdLen += 1;

	// �㷨��ʶ
	memcpy(hsmCmdBuf + hsmCmdLen,algorithmID,1);
	hsmCmdLen += 1;

	// ���ݳ���
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfData/2);
	hsmCmdLen += 4;	

	// ����
	aschex_to_bcdhex(hashData,lenOfData,hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfData/2;

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM7:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (sizeOfBuf < 64+1)
	{
		UnionUserErrLog("in UnionHsmCmdM7:: sizeOfBuf [%d] < 64 + 1!\n",sizeOfBuf);
		return(errCodeParameter);
	}

	bcdhex_to_aschex(hsmCmdBuf+4,32,hashValue);
	return(64);
}

// ����ǩ����ָ��
int UnionHsmCmdK3(char *hashID, int lenOfUsrID, char *usrID, int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	offset = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf + offset,"K3",2);
	offset += 2;

	if (vkIndex < 0)
	{
		// ��Կ����
		memcpy(hsmCmdBuf + offset,"99",2);
		offset += 2;
		// �����Կ����
		sprintf(hsmCmdBuf + offset,"%04d",lenOfVK);
		offset += 4;
		// �����Կ
		memcpy(hsmCmdBuf + offset,vk,lenOfVK);
		offset += lenOfVK;
	}
	else
	{
		// ��Կ����
		sprintf(hsmCmdBuf + offset,"%02d",vkIndex);
		offset += 2;
	}

	// HASH�㷨
	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;

	// �û���ʶ
	if (strcmp(hashID, "02") == 0)
	{
		sprintf(hsmCmdBuf + offset,"%04d",lenOfUsrID);
		offset += 4;
		memcpy(hsmCmdBuf + offset, usrID, lenOfUsrID);
		offset += lenOfUsrID;
	}

	// ���ݳ���
	sprintf(hsmCmdBuf+offset,"%04d",lenOfData/2);
	offset += 4;

	// ����
	aschex_to_bcdhex(data,lenOfData,hsmCmdBuf + offset);
	offset += lenOfData/2;

	hsmCmdBuf[offset] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK3:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ǩ�������R����
	bcdhex_to_aschex(hsmCmdBuf + 4,32,sign);
	// ǩ�������S����
	bcdhex_to_aschex(hsmCmdBuf + 4 + 32,32,sign+64);
	sign[128] = 0;
	//return(lenOfSign);
	return(128);
}

//������ǩ��ָ��
int UnionHsmCmdK4(int vkIndex, char *hashID, int lenOfUsrID, char *usrID, int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal)
{
	int	ret;
	char	hsmCmdBuf[8192*2];
	int	offset = 0;

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf + offset,"K4",2);
	offset += 2;
	if (vkIndex < 0)
	{
		// ��Կ����
		memcpy(hsmCmdBuf + offset,"99",2);
		offset += 2;
		// �����Կ
		aschex_to_bcdhex(rCaPK,lengRCaPK,hsmCmdBuf + offset);
		offset += lengRCaPK/2;
		//memcpy(hsmCmdBuf + offset,rCaPK,lengRCaPK);
		//offset += lengRCaPK;
	}
	else
	{
		// ��Կ����
		sprintf(hsmCmdBuf + offset,"%02d",vkIndex);
		offset += 2;
	}

	// ǩ�����
	aschex_to_bcdhex(caCertDataSign,lenCaCertDataSign,hsmCmdBuf + offset);
	offset += lenCaCertDataSign/2;
	//memcpy(hsmCmdBuf + offset,caCertDataSign,lenCaCertDataSign);
	//offset += lenCaCertDataSign;

	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;

	// �û���ʶ
	if (strcmp(hashID, "02") == 0)
	{
		sprintf(hsmCmdBuf + offset,"%04d",lenOfUsrID);
		offset += 4;
		memcpy(hsmCmdBuf + offset, usrID, lenOfUsrID);
		offset += lenOfUsrID;
	}

	// ���ݳ���
	sprintf(hsmCmdBuf+offset,"%04d",lenHashVal/2);
	//sprintf(hsmCmdBuf+offset,"0032",4);
	//UnionLog("in UnionHsmCmdK4:: UnionDirectHsmCmd lenHashVal = [%d]!\n",lenHashVal/2);
	offset += 4;

	// ����
	aschex_to_bcdhex(hashVal,lenHashVal,hsmCmdBuf + offset);
	offset += lenHashVal/2;
	//memcpy(hsmCmdBuf + offset,hashVal,lenHashVal);
	//offset += lenHashVal;

	hsmCmdBuf[offset] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK4:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

//��SM2��Կ��SCE����
/*�������
  pkIndex			��Կ����
  lenOfPK			��Կ���ĳ���
  PK			��Կ����
  lenOfData		���ݳ���
  data			����
  �������
  lenOfCiphertext		���ĳ���
  ciphertext		����
 */
int UnionHsmCmdK5(int pkIndex,int lenOfPK,char *PK,int lenOfData,char *data,int *lenOfCiphertext,unsigned char *ciphertext)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;

	memcpy(hsmCmdBuf + hsmCmdLen,"K5",2);
	hsmCmdLen = 2;
	if(pkIndex < 0)
	{
		//��Կ����
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		//��Կ����
		aschex_to_bcdhex(PK,lenOfPK,hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += lenOfPK/2;
	}
	else
	{
		//��Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",pkIndex);
		hsmCmdLen += 2;
	}
	//���ݳ���
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	//����
	memcpy(hsmCmdBuf + hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMReqCmd();
	// end 

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//���ĳ���
	// modify by zhangyd 20150305 
	if ((*lenOfCiphertext = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK5:: len[%d] error!\n",*lenOfCiphertext);
		return(errCodeParameter);
	}
	//����
	memcpy(ciphertext,hsmCmdBuf+4+4,*lenOfCiphertext);
	ciphertext[*lenOfCiphertext] = 0;
	return(*lenOfCiphertext);
}

//��SM2��Կ��SCE����
/*�������
  pkIndex			��Կ����
  lenOfPK			��Կ���ĳ���
  PK			��Կ����
  lenOfData		���ݳ���
  data			����
  �������
  lenOfCiphertext		���ĳ���
  ciphertext		����
 */
int UnionHsmCmdKE(int pkIndex,int lenOfPK,char *PK,int lenOfData,char *data,int *lenOfCiphertext,unsigned char *ciphertext)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;

	memcpy(hsmCmdBuf + hsmCmdLen,"KE",2);
	hsmCmdLen = 2;
	if(pkIndex < 0)
	{
		//��Կ����
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		//��Կ����
		aschex_to_bcdhex(PK,lenOfPK,hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += lenOfPK/2;
	}
	else
	{
		//��Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",pkIndex);
		hsmCmdLen += 2;
	}
	//���ݳ���
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	//����
	memcpy(hsmCmdBuf + hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMReqCmd();
	// end 

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKE:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//���ĳ���
	// modify by zhangyd 20150305 
	if ((*lenOfCiphertext = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKE:: len[%d] error!\n",*lenOfCiphertext);
		return(errCodeParameter);
	}
	//����
	memcpy(ciphertext,hsmCmdBuf+4+4,*lenOfCiphertext);
	ciphertext[*lenOfCiphertext] = 0;
	return(*lenOfCiphertext);
}

//��SM2˽Կ��SM2����
/*�������
  vkIndex			��Կ����
  lenOfVK			�����Կ����
  VK			�����Կ
  lenOfCiphertext		���ĳ���
  ciphertext		����
  �������
  lenOfData		���ݳ���
  data			����
 */
int UnionHsmCmdK6(int vkIndex,int lenOfVK,char *VK,int lenOfCiphertext,unsigned char *ciphertext,int *lenOfData,char *data)
{
	int		ret;
	char            hsmCmdBuf[8096];
	int             hsmCmdLen = 0;

	memcpy(hsmCmdBuf + hsmCmdLen,"K6",2);
	hsmCmdLen = 2;
	if(vkIndex < 0)
	{
		//��Կ����
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// �����Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfVK);
		hsmCmdLen += 4;
		// �����Կ
		memcpy(hsmCmdBuf + hsmCmdLen,VK,lenOfVK);
		hsmCmdLen += lenOfVK;
	}
	else
	{
		//��Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",vkIndex);
		hsmCmdLen += 2;
	}
	//���ĳ���
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfCiphertext);
	hsmCmdLen += 4;
	//����
	memcpy(hsmCmdBuf + hsmCmdLen,ciphertext,lenOfCiphertext);
	//hsmCmdLen += *lenOfData;
	hsmCmdLen += lenOfCiphertext;
	hsmCmdBuf[hsmCmdLen] = 0;

	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMResCmd();
	// end

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK6:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//���ݳ���
	if ((*lenOfData = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK6:: UnionDirectHsmCmd!\n");
		return(errCodeParameter);
	}
	//����
	memcpy(data,hsmCmdBuf+4+4,*lenOfData);
	return(*lenOfData);
}

//��SM2˽Կ��SM2����
/*�������
  vkIndex			��Կ����
  lenOfVK			�����Կ����
  VK			�����Կ
  lenOfCiphertext		���ĳ���
  ciphertext		����
  �������
  lenOfData		���ݳ���
  data			����
 */
int UnionHsmCmdKF(int vkIndex,int lenOfVK,char *VK,int lenOfCiphertext,unsigned char *ciphertext,int *lenOfData,char *data)
{
	int		ret;
	char            hsmCmdBuf[8096];
	int             hsmCmdLen = 0;

	memcpy(hsmCmdBuf + hsmCmdLen,"KF",2);
	hsmCmdLen = 2;
	if(vkIndex < 0)
	{
		//��Կ����
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// �����Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfVK);
		hsmCmdLen += 4;
		// �����Կ
		memcpy(hsmCmdBuf + hsmCmdLen,VK,lenOfVK);
		hsmCmdLen += lenOfVK;
	}
	else
	{
		//��Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",vkIndex);
		hsmCmdLen += 2;
	}
	//���ĳ���
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfCiphertext);
	hsmCmdLen += 4;
	//����
	memcpy(hsmCmdBuf + hsmCmdLen,ciphertext,lenOfCiphertext);
	//hsmCmdLen += *lenOfData;
	hsmCmdLen += lenOfCiphertext;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();
	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMResCmd();
	// end

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKF:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//���ݳ���
	if ((*lenOfData = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKF:: UnionDirectHsmCmd!\n");
		return(errCodeParameter);
	}
	//����
	memcpy(data,hsmCmdBuf+4+4,*lenOfData);
	return(*lenOfData);
}

int UnionHsmCmd38(char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	char		ascBuf[8192];

	if ((signLen <= 0) || (pk == NULL) || (dataLen <= 0) || (data == NULL) || (signature == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in UnionHsmCmd38:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"38",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signLen);
	hsmCmdLen += 4;
	if (hsmCmdLen + signLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd38:: signLen [%d] too long!\n",signLen);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,signature,signLen);
	hsmCmdLen += signLen;
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",dataLen);
	hsmCmdLen += 4;
	if (hsmCmdLen + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd38:: dataLen [%d] too long!\n",dataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	if ((ret = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd38:: UnionFormANSIDERRSAPK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		bcdhex_to_aschex(hsmCmdBuf,hsmCmdLen,ascBuf);
		ascBuf[hsmCmdLen*2] = 0;
		UnionUserErrLog("in UnionHsmCmd38:: UnionDirectHsmCmd[%s] !\n",ascBuf);
		return(ret);
	}
	return(0);
}

int UnionHsmCmd30(char flag,char *pkIndex,char *pk,int dataLen,char *data,char *encData)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		len;

	if ((pk == NULL) || (dataLen <= 0) || (data == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in UnionHsmCmd30:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"30",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	if ((pkIndex == NULL) || (strlen(pkIndex) ==0))
		memcpy(hsmCmdBuf+hsmCmdLen,"99",2);
	else
		memcpy(hsmCmdBuf+hsmCmdLen,pkIndex,2);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",dataLen/2);
	hsmCmdLen += 4;
	if (hsmCmdLen + dataLen/2 > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd30:: dataLen [%d] too long!\n",dataLen/2);
		return(errCodeSmallBuffer);
	}
	aschex_to_bcdhex(data,dataLen,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += dataLen/2;
	if ((pkIndex == NULL) || (strlen(pkIndex) ==0))
	{
		aschex_to_bcdhex(pk,strlen(pk),hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += strlen(pk)/2;
	}
	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[hsmCmdLen] = 0;

	// add by leipp 20150116
	//UnionSetMaskPrintTypeForHSMReqCmd();
	// end

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd30:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if ((len = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd30:: signLen error!\n");
		return(errCodeParameter);
	}

	bcdhex_to_aschex(hsmCmdBuf+4+4,len,encData);
	encData[len*2] = 0;
	return(len*2);
}

int UnionHsmCmdNC(char *version)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"NC",2);
	hsmCmdLen = 2;

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNC:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(version,hsmCmdBuf+4,ret - 4);
	version[ret-4] = 0;
	return(ret - 4);
}


int UnionHsmCmd31(char *bmkIndex,char *bmkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((bmkIndex == NULL) || (bmkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd31:: null pointer!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"31",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd31:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	switch (ret - 4)
	{
		case    32:
		case    48:
			memcpy(bmkValue,hsmCmdBuf+4,ret-4);
			return(ret-4);
		default:
			UnionUserErrLog("in UnionHsmCmd31:: ret = [%d]\n",ret);
			return(errCodeSmallBuffer);
	}
	return(ret - 4);
}


int UnionHsmCmd11(TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	lenOfKey;

	if ((bmkIndex == NULL) || (bmkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd11:: null pointer!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"11",2);
	hsmCmdLen = 2;
	switch (keyLen)
	{
		case    con64BitsDesKey:
			lenOfKey = 16;
			break;
		case    con128BitsDesKey:
			//hsmCmdBuf[hsmCmdLen] = 'Y';
			//hsmCmdLen++;
			lenOfKey = 32;
			break;
		case    con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmd11:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd11:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(bmkValue,hsmCmdBuf+4,lenOfKey);
	return(lenOfKey);
}


int UnionHsmCmd2A(TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	lenOfKey;

	if ((bmkIndex == NULL) || (bmkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd2A:: null pointer!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"2A",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	switch (keyLen)
	{
		case    con64BitsDesKey:
			lenOfKey = 16;
			break;
		case    con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case    con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmd2A:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,bmkValue,lenOfKey);
	hsmCmdLen += lenOfKey;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd2A:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


int UnionHsmCmd35(char *vkIndex,char *vkByMK,int lenOfVKByMK)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if ((vkIndex == NULL) || (vkByMK == NULL) || (lenOfVKByMK < 0))
	{
		UnionUserErrLog("in UnionHsmCmd35:: null pointer!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"35",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	if (lenOfVKByMK > sizeof(hsmCmdBuf) - hsmCmdLen)
	{
		UnionUserErrLog("in UnionHsmCmd35:: too long vk length [%d]!\n",lenOfVKByMK);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVKByMK);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf+hsmCmdLen,vkByMK,lenOfVKByMK);
	hsmCmdLen += lenOfVKByMK;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd35:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}

int UnionHsmCmd36(char *vkIndex,char *vkByMK,int sizeOfVKByMK)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if ((vkIndex == NULL) || (vkByMK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd36:: null pointer!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"36",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd36:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(vkByMK,hsmCmdBuf+4+4,ret - 8);

	return(ret - 8);
}


/*
   ת��DES��Կ��������Կ���ܵ���Կ����
   ���ڷַ���Կ��
   �������
   pk ��Կ
   keyByMK ��Կ����
   �������
   checkValue ���ֵ 
   sizeOfBuf ��Կ����
   keyByPK ��Կ���ģ��ù�Կ���ܵ�DES��Կ��
 */
int UnionHsmCmd3B(char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[8192+100];
	char	tmpBuf[8];
	int	hsmCmdLen = 0;
	int	len;

	if ((keyByPK == NULL) || (keyByMK == NULL) || (pk == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd3B:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"3B",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,keyByMK,32);
	hsmCmdLen += 32;
	if ((ret = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd3B:: UnionFormANSIDERRSAPK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd3B:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(tmpBuf, hsmCmdBuf + 4 + 16, 4);
	tmpBuf[4] = 0;
	len = atoi(tmpBuf);
	memcpy(checkValue,hsmCmdBuf+4,16);
	checkValue[16] = 0;
	memcpy(keyByPK,hsmCmdBuf+4+16+4,len);
	keyByPK[len] = 0;
	return(len);
}


/*
   ת��DES��Կ���ӹ�Կ���ܵ�����Կ����
   ���ڽ�����Կ��
   �������
   vkIndex ˽Կ������"00"��"20"����������ڵ�˽Կ
   lenOfKeyByPK ��Կ���ȣ�DES��Կ���ĵ��ֽ���
   keyByPK �ù�Կ���ܵ�DES��Կ
   �������
   keyByMK ������Կ���ܵ�DES��Կ
   checkValue ���ֵ
 */
int UnionHsmCmd3A(int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[8192+100];
	int	hsmCmdLen = 0;

	if ((keyByPK == NULL) || (keyByMK == NULL) || (lenOfKeyByPK <= 0) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd3A:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"3A",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfKeyByPK);
	hsmCmdLen += 4;
	if (hsmCmdLen + lenOfKeyByPK >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd3A:: lenOfKeyByPK [%d] too long!\n",lenOfKeyByPK);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyByPK,lenOfKeyByPK);
	hsmCmdLen += lenOfKeyByPK;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd3A:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(keyByMK,hsmCmdBuf+4,32);
	keyByMK[32] = 0;
	memcpy(checkValue,hsmCmdBuf+4+32,16);
	checkValue[16] = 0;
	return(32);
}


int UnionHsmCmd3E(char *bmk,char *pk,char *keyByBMK,char *checkValue,char *keyByPK)
{
	int	ret;
	char	hsmCmdBuf[4096];
	int	hsmCmdLen = 0;
	char	tmpBuf[10];

	if ((bmk == NULL) || (pk == NULL) || (keyByBMK == NULL) || (checkValue == NULL) || (keyByPK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd3E:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"3E",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmk,3);
	hsmCmdLen += 3;
	if ((ret = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd3E:: UnionFormANSIDERRSAPK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd3E:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(keyByBMK,hsmCmdBuf+4,16);
	memcpy(checkValue,hsmCmdBuf+4+16,16);
	memcpy(tmpBuf,hsmCmdBuf+4+16+16,4);
	tmpBuf[4] = 0;

	bcdhex_to_aschex(hsmCmdBuf+4+16+16+4,128,keyByPK);
	return(0);
}


int UnionHsmCmd3C(char flag,int lenOfData,char *data,char *hash)
{
	int	ret;
	char	hsmCmdBuf[4096];
	int	hsmCmdLen = 0;

	if ((lenOfData <= 0) || (data == NULL) || (hash == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in UnionHsmCmd3C:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"3C",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	if (hsmCmdLen + lenOfData > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd3C:: lenOfData [%d] too long!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd3C:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if (flag == '1')
	{
		bcdhex_to_aschex(hsmCmdBuf+4,20,hash);
		return(40);
	}
	else
	{
		bcdhex_to_aschex(hsmCmdBuf+4,16,hash);
		return(32);
	}
}

int UnionHsmCmd3CForEEMG(char flag,int lenOfData,char *data,char *hash)
{
	int	ret;
	char	hsmCmdBuf[4096];
	int	hsmCmdLen = 0;
	int	hashLen = 0;

	if ((lenOfData <= 0) || (data == NULL) || (hash == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd3C:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"3C",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	if (hsmCmdLen + lenOfData > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd3C:: lenOfData [%d] too long!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd3C:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hashLen = ret - 4;
	memcpy(hash, hsmCmdBuf+4, hashLen);
	return hashLen;
}


int UnionHsmCmd37(char flag,char *vkIndex,int lenOfVK,char *vk,int dataLen,char *data,char *signature,int sizeOfSignature)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	signLen;

	if ((vkIndex == NULL) || (dataLen <= 0) || (data == NULL) || (signature == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in UnionHsmCmd37:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"37",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	if (strcmp(vkIndex,"99") == 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen,vk,lenOfVK);
		hsmCmdLen += lenOfVK;
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",dataLen);
	hsmCmdLen += 4;
	if (hsmCmdLen + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd37:: dataLen [%d] too long!\n",dataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;

	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd37:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if ((signLen = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd37:: signLen error!\n");
		return(errCodeSmallBuffer);
	}

	if (signLen > sizeOfSignature - 1)
	{
		UnionUserErrLog("in UnionHsmCmd37:: sizeOfSignature[%d] is to small!\n",sizeOfSignature);
		return(errCodeParameter);
	}
	memcpy(signature,hsmCmdBuf+4+4,signLen);
	signature[signLen] = 0;
	return(signLen);
}


//add 20130719,34ָ�����ͬʱ�����Կ˽Կ
int UnionHsmCmd34(char *vkIndex,int lenOfVK,char *pk,int sizeOfPK, char *vk, int sizeOfVk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	char	tmpBuf[2048];
	int	vkLen;
	int	lenOfPK = 0;  //add in 2012.01.13

	if ((vkIndex == NULL) || ((lenOfVK != 256) && (lenOfVK != 512) && (lenOfVK != 1024) && (lenOfVK != 2048) && (lenOfVK != 4096)))
	{
		UnionUserErrLog("in UnionHsmCmd34:: lenOfVK = [%d]\n",lenOfVK);
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"34",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd34:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	// Get VK
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	tmpBuf[4] = 0;
	vkLen = atoi(tmpBuf);
	if (vkLen * 2 > sizeOfVk)
	{
		UnionUserErrLog("in UnionHsmCmd34::sizeOf VK is to small!\n");
		return(errCodeParameter);
	}
	bcdhex_to_aschex(hsmCmdBuf+4+4, vkLen, vk);
	//UnionMemLog("in SJL06Cmd34::",(unsigned char *)hsmCmdBuf+4+4,vkLen);
	//("in SJL06Cmd34:: vkLen = [%04d] derPKLen = [%04d]\n",vkLen,ret-4-4-vkLen);
	//UnionMemLog("in SJL06Cmd34::",(unsigned char *)hsmCmdBuf+4+4+vkLen,ret-4-4-vkLen);
	//modify by hzh in 2012.01.13, ������ܻ������󣬹�Կȡ��ƫ�Ƶ�����. (��30818902818100..., ��߶���00,�ұ�ĩβ�ֽڶ�ʧ)
	if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)hsmCmdBuf+4+4+vkLen,ret-4-4-vkLen,pk,&lenOfPK,sizeOfPK)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd34:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
		UnionMemLog("in UnionHsmCmd34:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(ret);
	}
	return lenOfPK;
	//modify end
}


int UnionHsmCmd40(char *vkIndex,char *bmk,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;

	if ((vkIndex == NULL) || (bmk == NULL) || (pvk == NULL) || (pinByPK == NULL) || (pinByPVK == NULL))
		return(errCodeParameter);

	memcpy(hsmCmdBuf,"40",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmk,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,pvk,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + lenOfPinByPK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd40:: lenOfPinByPK [%d] too long!\n",lenOfPinByPK);
		return(errCodeSmallBuffer);
	}

	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd40:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if (ret == 4+16)
	{
		memcpy(pinByPVK,hsmCmdBuf+4,16);
		return(16);
	}
	else
		return(0);
}


int UnionHsmCmd41(char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;

	if ((vkIndex == NULL) || (bmk == NULL) || (zpk == NULL) || (pan == NULL) || (pinByPK == NULL) || (pinByZPK == NULL))
		return(errCodeParameter);

	memcpy(hsmCmdBuf,"41",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd41:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + lenOfPinByPK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd41:: lenOfPinByPK [%d] too long!\n",lenOfPinByPK);
		return(errCodeSmallBuffer);
	}

	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;
	hsmCmdBuf[hsmCmdLen] = 0;
	UnionMemLog("in UnionHsmCmd41::",(unsigned char *)hsmCmdBuf,hsmCmdLen);

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd41:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if (ret == 4+16)
	{
		memcpy(pinByZPK,hsmCmdBuf+4,16);
		return(16);
	}
	else
		return(0);
}


int UnionHsmCmd42(char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;

	if ((vkIndex == NULL) || (encryptedData == NULL) || (plainText == NULL))
		return(errCodeParameter);

	memcpy(hsmCmdBuf,"42",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	if (hsmCmdLen + lenOfEncryptedData > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd42:: lenOfEncryptedData [%d] too long!\n",lenOfEncryptedData);
		return(errCodeSmallBuffer);
	}

	memcpy(hsmCmdBuf+hsmCmdLen,encryptedData,lenOfEncryptedData);
	hsmCmdLen += lenOfEncryptedData;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd42:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if (ret == 4+lenOfEncryptedData)
	{
		memcpy(plainText,hsmCmdBuf+4,lenOfEncryptedData);
		return(lenOfEncryptedData);
	}
	else
		return(0);
}


/*
   ���ܣ����ɹ�Կ���ܵ�PINת����ANSI9.8��׼
   �������
   vkIndex ˽Կ������ָ����˽Կ�����ڽ���PIN��������
   type ��Կ���ͣ�1��TPK  2��ZPK
   zpk ���ڼ���PIN����Կ
   pinType PIN���ͣ�1��ANSI9.8
   pan �û����ʺ�
   pinByPK ����Կ���ܵ�PIN��������
   lenOfPinByPK ���ĳ���
   �������
   pinByZPK ���ص�PIN����--ANSI9.8

�޸ģ�20160125 lusj
�޸����ݣ�ƽ������43ָ����չ��֧�����˽Կ���漰�ķ�����E201	

 */
int UnionHsmCmd43(char *vkIndex,int lenOfVK,char *vk,char type,char *zpk,char pinType,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK)
{
	int	ret;
	//char	hsmCmdBuf[1024]; 2016-05-19
	char	hsmCmdBuf[4096];
	int	hsmCmdLen = 0;
	int	panLen;
	char		tmpBuf[8192];

	if ((vkIndex == NULL) || (zpk == NULL) || (pan == NULL) || (pinByPK == NULL) || (pinByZPK == NULL))
		return(errCodeParameter);

	memcpy(hsmCmdBuf,"43",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
		
	//add begin  by lusj 20160125 ƽ������43ָ���޸ĺ�֧�����˽Կ
	if(atoi(vkIndex)==99)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmd43:: vk is null!\n");
			return(errCodeParameter);
		}

		//˽Կ����
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//˽Կ
		aschex_to_bcdhex(vk,lenOfVK,tmpBuf);
		tmpBuf[lenOfVK/2]=0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;	
	}
	//add end 20160125

	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd43:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = pinType;
	hsmCmdLen++;
	memset(hsmCmdBuf+hsmCmdLen,'0',16);
	if ((panLen = strlen(pan)) >= 13)
		memcpy(hsmCmdBuf+hsmCmdLen+4,pan+panLen-13,12);
	else
		memcpy(hsmCmdBuf+hsmCmdLen+16-panLen,pan,panLen);
	hsmCmdLen += 16;
	if (hsmCmdLen + lenOfPinByPK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd43:: lenOfPinByPK [%d] too long!\n",lenOfPinByPK);
		return(errCodeSmallBuffer);
	}

	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;
	//UnionMemLog("in UnionHsmCmd43::",(unsigned char *)hsmCmdBuf,hsmCmdLen); 20160531

	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd43:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (ret == 4+16)
	{
		memcpy(pinByZPK,hsmCmdBuf+4,16);
		return(16);
	}
	else
		return(0);
}

/*
   ���ܣ����ɹ�Կ���ܵ�PINת���ɾ����������㷨����
   �������
   vkIndex ˽Կ������ָ����˽Կ�����ڽ���PIN��������
   pinByPK ����Կ���ܵ�PIN��������
   lenOfPinByPK ���ĳ���
   �������
   hash ���ص�PIN����--ANSI9.8
 */
// modify by zhouxw 20160415 �������˽Կ
int UnionHsmCmd44(char *vkIndex,char *vkValue, int lenOfVKValue, unsigned char *pinByPK,int lenOfPinByPK,char *hash)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	pinLen;

	if ((vkIndex == NULL) || (pinByPK == NULL) || (hash == NULL))
		return(errCodeParameter);

	memcpy(hsmCmdBuf,"44",2);
	hsmCmdLen = 2;
	
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;

	if(strcmp(vkIndex, "99") == 0)
	{
		snprintf(hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen, "%04d", lenOfVKValue/2);
		hsmCmdLen += 4;
		aschex_to_bcdhex(vkValue, lenOfVKValue, hsmCmdBuf+hsmCmdLen);
		//memcpy(hsmCmdBuf+hsmCmdLen, vkValue, lenOfPinByPK);
		hsmCmdLen += lenOfVKValue/2;
	}

	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;

	UnionMemLog("in UnionHsmCmd44::",(unsigned char *)hsmCmdBuf,hsmCmdLen);

	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd44:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	pinLen = ret - 4;
	memcpy(hash, hsmCmdBuf+4, pinLen);
	hash[pinLen] = 0;

	return pinLen;
}

/*
   ���ܣ���EDK��Կ�ӽ������ݣ�����ǽ���״̬�����������Ȩ�²��ܴ������򱨴�
   �������
   flag 0������
   edk LMK24-25����
   lenOfData ���������ֽ�����8�ı�������Χ��0008-4096
   indata �����ܻ���ܵ����ݣ���BCD���ʾ��
   �������
   outdata ������
 */
int UnionHsmCmd50(char flag,char *edk,int lenOfData,char *indata,char *outdata,int sizeOfOutData)
{
	int	ret;
	char	hsmCmdBuf[8096+40];
	int	hsmCmdLen = 0;
	int	retLen;
	int	keyLen;

	memcpy(hsmCmdBuf,"50",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;

	// add  by leipp 20150116
	if (flag == '0')
		UnionSetMaskPrintTypeForHSMReqCmd();
	else
		UnionSetMaskPrintTypeForHSMResCmd();
	// end

	hsmCmdLen++;
	switch (keyLen = strlen(edk))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmd50:: edk [%s] length error!\n",edk);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,edk,keyLen);
	hsmCmdLen += keyLen;
	if ((lenOfData % 8 != 0) || (lenOfData <= 0) || (lenOfData > 8096))
	{
		UnionUserErrLog("in UnionHsmCmd50:: lenOfData [%04d] error!\n",lenOfData);
		return(errCodeParameter);
	}
	switch (flag)
	{
		case    '0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData);
			hsmCmdLen += 4;
			bcdhex_to_aschex(indata,lenOfData,hsmCmdBuf+hsmCmdLen);
			hsmCmdLen += (lenOfData * 2);
			break;
		case    '1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData/2);
			hsmCmdLen += 4;
			memcpy(hsmCmdBuf+hsmCmdLen,indata,lenOfData);
			hsmCmdLen += lenOfData;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmd50:: flag [%c] error!\n",flag);
			return(errCodeParameter);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd50:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	retLen = ret - 4;
	switch (flag)
	{
		case    '0':
			if (retLen > sizeOfOutData)
			{
				UnionUserErrLog("in UnionHsmCmd50:: sizeOfOutData [%04d] < retLen [%04d]\n",sizeOfOutData,retLen);
				return(errCodeSmallBuffer);
			}
			memcpy(outdata,hsmCmdBuf+4,retLen);
			outdata[retLen] = 0;
			return(retLen);
		case    '1':
			if (retLen / 2 > sizeOfOutData)
			{
				UnionUserErrLog("in UnionHsmCmd50:: sizeOfOutData [%04d] < retLen [%04d]\n",sizeOfOutData,retLen/2);
				return(errCodeSmallBuffer);
			}
			aschex_to_bcdhex(hsmCmdBuf+4,retLen,outdata);
			outdata[retLen/2] = 0;
			return(retLen/2);
		default:
			return(errCodeParameter);
	}
}

/*
   ���ܣ���RC4�㷨�������ݼ�/���ܡ�����RC4�㷨�����ԣ����ܺͽ��ܶ��ǵ���ͬһָ�
   ������
   desKeyType	��Կ���ͣ�֧��edk��zek
   inData		�������ݣ�binary
   inLen		�������ݳ���
   outData		�������
   sizeOfOutData	outDataָ��Ļ������ĳ���
   ���أ�
   <0 ����
   >0 �������ݳ���
   add by zhangsb 20140627
 */
int UnionHsmCmdRD(TUnionDesKeyType desKeyType, char *keyValue, char *inData, int inLen, char *outData, int sizeOfOutData)
{
	int	ret, hsmCmdLen;
	char	hsmCmdBuf[10240];

	hsmCmdBuf[0] = 'R';
	hsmCmdBuf[1] = 'D';
	hsmCmdLen = 2;

	if ((ret = UnionTranslateDesKeyTypeTo3CharFormat(desKeyType, hsmCmdBuf + hsmCmdLen)) <0)
	{
		UnionUserErrLog("in UnionHsmCmdRD::UnionTranslateDesKeyTypeTo3CharFormat!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 3;

	switch (ret = strlen(keyValue))
	{
		case	16:
			strcpy(hsmCmdBuf + hsmCmdLen, keyValue);
			hsmCmdLen += 16;
			break;
		case	32:
			hsmCmdBuf[hsmCmdLen++] = 'X';
			strcpy(hsmCmdBuf + hsmCmdLen, keyValue);
			hsmCmdLen += 32;
			break;
		case	48:
			hsmCmdBuf[hsmCmdLen++] = 'Y';	
			strcpy(hsmCmdBuf + hsmCmdLen, keyValue);
			hsmCmdLen += 48;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdRD::len of keyValue [%d]  error!\n", ret);
			return(errCodeParameter);
	}

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", inLen);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf + hsmCmdLen, inData, inLen);
	hsmCmdLen += inLen;

	// add by leip 20150116
	UnionSetMaskPrintTypeForHSMReqCmd();
	// end

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRD:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(outData, hsmCmdBuf + 4, ret -4);
	outData[ret-4] = 0;
	return(ret - 4);
}


// ��ZMK���ܵ���Կת��ΪLMK���ܵ�ZAK/ZEK
int UnionHsmCmdFK(char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	keyLen;
	int	offset;

	memcpy(hsmCmdBuf,"FK",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	switch (keyLen = strlen(zmk))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdFK:: zmk [%s] length error!\n",zmk);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,zmk,keyLen);
	hsmCmdLen += keyLen;
	switch (keyLen = strlen(keyByZMK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdFK:: keyByZMK [%s] length error!\n",keyByZMK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyByZMK,keyLen);
	hsmCmdLen += keyLen;
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	switch (keyLen)
	{
		case    16:
			memcpy(hsmCmdBuf+hsmCmdLen,"ZZ0",3);
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"XX0",3);
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"YY0",3);
			break;
	}
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	switch (keyLen)
	{
		case    16:
			offset = 0;
			break;
		case    32:
		case    48:
			offset = 1;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdFK:: keyByZMK [%s] length error!\n",keyByZMK);
			return(errCodeParameter);
	}
	memcpy(keyByLMK,hsmCmdBuf+4+offset,keyLen);
	keyByLMK[keyLen] = 0;
	memcpy(checkValue,hsmCmdBuf+4+offset+keyLen,ret-4-offset-keyLen);
	checkValue[ret-4-offset-keyLen] = 0;
	return(keyLen);
}

/*
   ��������:
   TIָ���Կ����
   �������:
encryMode: 1H �㷨��ʶ 1���������� 2���������� 3����֤ǩ�� 4��������Կ 5��������Կ
pkLength: ���빫Կ����
pk: nB/1A+3H DER����Ĺ�Կ��Կ��HSM��ȫ�洢���ڵ�λ��
signLength: ����֤ǩ��ֵ���ȣ�encryMode = 3ʱ�������
sign: nB ����֤ǩ��ֵ��encryMode = 3ʱ�������
keyLength: 1H ����/������Կ���ȣ�0 = ��������Կ��1 = ˫������Կ��encryMode = 4 �� encryMode = 5ʱ�������
inputDataLength: �ӽ������ݳ���
inputData: nB/1A+3H ���ӽ������ݻ���ӽ�����Կ��HSM��ȫ�洢���ڵ�����
�������:
outputData: nB �ӽ���/�ӽ�����Կ����
����ֵ:
>0	ouputData�ĳ���
<0	ʧ��
 */

int UnionHsmCmdTI(char encryMode, int pkLength, char *pk, int signLength,
		char *sign, char keyLength, int inputDataLength, char *inputData, char *outputData)
{
	int		ret;
	char		tmpBuf[32];
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;

	if ((pk == NULL) || (inputDataLength <= 0))
	{
		UnionUserErrLog("in UnionHsmCmdTI:: parameters error!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf, "TI", 2);
	hsmCmdLen += 2;

	hsmCmdBuf[hsmCmdLen] = encryMode;
	hsmCmdLen++;

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", pkLength);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf + hsmCmdLen, pk, pkLength);
	hsmCmdLen += pkLength;

	memcpy(hsmCmdBuf + hsmCmdLen, "01", 2);
	hsmCmdLen += 2;

	if (encryMode == '3')
	{
		if (sign == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdTI:: parameters error!\n");
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", signLength);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf + hsmCmdLen, sign, signLength);
		hsmCmdLen += signLength;
	}

	if ((encryMode == '4') || (encryMode == '5'))
	{
		memcpy(hsmCmdBuf + hsmCmdLen, "000", 3);
		hsmCmdLen += 3;
		hsmCmdBuf[hsmCmdLen] = keyLength;
		hsmCmdLen++;
	}

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", inputDataLength);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf + hsmCmdLen, inputData, inputDataLength);
	hsmCmdLen += inputDataLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdTI:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;

	memcpy(tmpBuf, hsmCmdBuf + 4, 4);
	tmpBuf[4] = 0;
	ret = atoi(tmpBuf);
	memcpy(outputData, hsmCmdBuf + 4 + 4, ret);
	outputData[ret] = 0;

	return(ret);
}


// ��һ��ZMK���ܵ���Կת��ΪLMK����,֧�ֹ����㷨SM1��SFF33
int UnionHsmCmdA6(char *algFlag, TUnionDesKeyType keyType,char *zmk,char *keyByZmk, char *keyByLmk,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	keyByLmkStrLen;

	memcpy(hsmCmdBuf,"A6",2);
	hsmCmdLen = 2;

	if (algFlag != NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, algFlag, 1);
		hsmCmdLen += 1;
	}

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA6:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if (strlen(zmk) == 4)	// ������
	{
		memcpy(hsmCmdBuf+hsmCmdLen, zmk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",zmk);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	if ((ret = UnionPutKeyIntoRacalKeyString(keyByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionCaculateHsmKeyKeyScheme(strlen(keyByZmk),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA6:: UnionCaculateHsmKeyKeyScheme [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA6:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if ((keyByLmkStrLen = UnionReadKeyFromRacalKeyString(hsmCmdBuf+4,ret-4,keyByLmk)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA6:: UnionReadKeyFromRacalKeyString! hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(keyByLmkStrLen);
	}
	if (keyByLmkStrLen >= ret)      // û��У��ֵ
		return(0);
	if (ret - keyByLmkStrLen > 16)
	{
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,16);
		checkValue[16] = 0;
	}
	else
	{
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,ret-keyByLmkStrLen-4);
		checkValue[ret-keyByLmkStrLen-4] = 0;
	}
	return(0);
}


// ��һ��ZMK���ܵ���Կת��ΪLMK����,SM4�㷨
int UnionHsmCmdSV(TUnionDesKeyType keyType, char *zmk, char *keyByZmk, char *keyByLmk, char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	offset = 0;

	memcpy(hsmCmdBuf,"SV",2);
	hsmCmdLen = 2;

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSV:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//zmk��Կ
	if (strlen(zmk) == 4)	// ������
	{
		memcpy(hsmCmdBuf+hsmCmdLen, zmk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zmk,32);
		hsmCmdLen += 32;
	}

	//ZMK�¼��ܵ���Կ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,keyByZmk,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSV:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (strcmp(hsmCmdBuf+offset, "S") == 0)
	{
		UnionUserErrLog("in UnionHsmCmdSV:: Key LENGTH ERROR!\n");
		return(ret);
	}
	offset += 1;
	memcpy(keyByLmk,hsmCmdBuf+offset,32);
	keyByLmk[32] = 0;
	offset += 32;
	memcpy(checkValue,hsmCmdBuf+offset,16);
	checkValue[16] = 0;

	return(0);
}


// ��������MAC��ָ��
int UnionHsmCmdMU(char msgNo,int keyType,TUnionDesKeyLength keyLength,char *key,char msgType,char *iv,int msgLen,char *msg,char *mac)
{
	int	ret;
	char	hsmCmdBuf[8096];
	int	hsmCmdLen = 0;

	if ((key == NULL) || (msg == NULL) || (mac == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdMU:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"MU",2);
	hsmCmdLen = 2;

	// ��Ϣ���
	hsmCmdBuf[hsmCmdLen] = msgNo;
	hsmCmdLen += 1;

	// ��Կ����	
	sprintf(hsmCmdBuf + hsmCmdLen,"%d",keyType);
	hsmCmdLen += 1;

	// ��Կ���ȱ�־
	switch (keyLength)
	{
		case    con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case    con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case    con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '2';
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdMU:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;

	hsmCmdBuf[hsmCmdLen] = msgType;
	++hsmCmdLen;
	switch (keyLength)
	{
		case    con64BitsDesKey:
			memcpy(hsmCmdBuf+hsmCmdLen,key,16);
			hsmCmdLen += 16;
			break;
		case    con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'X';
			hsmCmdLen++;
			// 2008/07/11,���ӽ���
			memcpy(hsmCmdBuf+hsmCmdLen,key,32);
			hsmCmdLen += 32;
			break;
		case    con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			memcpy(hsmCmdBuf+hsmCmdLen,key,48);
			hsmCmdLen += 48;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdMU:: wrong keyLength flag [%c]!\n",keyLength);
			return(errCodeParameter);
	}

	switch (msgNo)
	{
		case	'2':
		case	'3':
			if (iv == NULL)
			{
				UnionUserErrLog("in UnionHsmCmdMU:: iv is NULL!\n");
				return(errCodeParameter);
			}
			memcpy(hsmCmdBuf+hsmCmdLen,iv,16);
			hsmCmdLen += 16;
			break;
		default:
			break;
	}

	switch (msgType)
	{
		case    '0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen);
			break;
		case    '1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen/2);
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdMU:: wrong msgtype flag [%c]!\n",msgType);
			return(errCodeParameter);
	}
	hsmCmdLen += 4;
	if (hsmCmdLen + msgLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdMU:: msg length [%d] too long!\n",msgLen);
		return(-1);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,msg,msgLen);
	hsmCmdLen += msgLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdMU:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	mac[16] = 0;
	return(16);
}

// ��ZMK���ܵ�ZPKת��ΪLMK����
int UnionHsmCmdFA(char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	offset;
	int	len;

	memcpy(hsmCmdBuf,"FA",2);
	hsmCmdLen = 2;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFA:: UnionPutKeyIntoRacalKeyString zmk [%s]error!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(zpkByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFA:: UnionPutKeyIntoRacalKeyString zpkByZmk [%s]error!\n",zpkByZmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if (variant > 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",variant % 100);
		hsmCmdLen += 2;
	}
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,zpkByLmk)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFA:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	UnionDebugLog("in UnionHsmCmdFA:: zpkByLmk = [%s]\n",zpkByLmk);
	offset += len;
	if (offset + 16 > ret)
		ret = ret - offset;
	else
		ret = 16;
	if (ret < 0)
		ret = 0;
	memcpy(checkValue,hsmCmdBuf+offset,ret);
	return(strlen(zpkByZmk));
}

// ����ZAK/ZEK
int UnionHsmCmdFI(char keyFlag,char *zmk,int variant,char keyLenFlag,char *keyByZmk,char *keyByLmk,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	offset;
	int	len;

	memcpy(hsmCmdBuf,"FI",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = keyFlag;
	hsmCmdLen++;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFI:: UnionPutKeyIntoRacalKeyString zmk [%s]error!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if (variant > 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",variant % 100);
		hsmCmdLen += 2;
	}
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,keyByZmk)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFI:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	offset += len;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,keyByLmk)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFI:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	offset += len;
	if (offset + 16 > ret)
		ret = ret - offset;
	else
		ret = 16;
	if (ret < 0)
		ret = 0;
	memcpy(checkValue,hsmCmdBuf+offset,ret);
	return(strlen(keyByZmk));
}


int UnionHsmCmdA0(int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	zmkLength,keyLen;
	int	offsetOfKeyByLMK,offsetOfKeyByZMK,offsetOfCheckValue;
	int	cvLen;

	memcpy(hsmCmdBuf,"A0",2);
	hsmCmdLen = 2;
	if (outputByZMK)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
		hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen += 1;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA0:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}

	/*
	   if (gunionIsUseSpecZmkType)
	   {
	   if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
	   memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
	   }
	   else
	   gunionIsUseSpecZmkType = 1;
	 */
	hsmCmdLen += ret;
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA0:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if (outputByZMK)
	{
		/*
		   if ((ret = UnionCaculateHsmKeyKeyScheme(zmkLength=strlen(zmk),hsmCmdBuf+hsmCmdLen)) < 0)
		   {
		   UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme! zmk = [%s] zmkLen = [%d]\n",zmk,zmkLength);
		   return(ret);
		   }
		 */
		switch (zmkLength=strlen(zmk))
		{
			case    16:
				ret = 0;
				break;
			case    32:
				memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
				ret = 1;
				break;
			case    48:
				memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
				ret = 1;
				break;
			default:
				UnionUserErrLog("in UnionHsmCmdA0:: UnionCaculateHsmKeyKeyScheme! zmk = [%s] zmkLen = [%d]\n",zmk,zmkLength);
				return(errCodeParameter);
		}
		hsmCmdLen += ret;
		memcpy(hsmCmdBuf+hsmCmdLen,zmk,zmkLength);
		hsmCmdLen += zmkLength;
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
			//if ((ret = UnionCaculateHsmKeyKeyScheme(zmkLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdA0:: UnionTranslateHsmKeyKeyScheme keyLength = [%d]!\n",keyLength);
			//UnionUserErrLog("in UnionHsmCmdA0:: UnionCaculateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA0:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offsetOfKeyByZMK = -1;
	switch (keyLength)
	{
		case    con64BitsDesKey:
			offsetOfKeyByLMK = 4;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 16;
				offsetOfCheckValue = 4 + 16 + 16;
			}
			else
				offsetOfCheckValue = 4 + 16;
			keyLen = 16;
			break;
			/*if (outputByZMK)
			  {
			  memcpy(keyByZMK,hsmCmdBuf+4,16);
			  memcpy(keyByLMK,hsmCmdBuf+4+16,16);
			  memcpy(checkValue,hsmCmdBuf+4+16+16,6);
			  }
			  else
			  {
			  memcpy(keyByLMK,hsmCmdBuf+4,16);
			  memcpy(checkValue,hsmCmdBuf+4+16,6);
			  }
			  return(16);
			 */
		case    con128BitsDesKey:
			offsetOfKeyByLMK = 4 + 1;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 1 + 32 + 1;
				offsetOfCheckValue = 4 + 1 + 32 + 1 + 32;
			}
			else
				offsetOfCheckValue = 4 + 1 + 32;
			keyLen = 32;
			break;
			/*
			   if (outputByZMK)
			   {
			   memcpy(keyByZMK,hsmCmdBuf+4+1,32);
			   memcpy(keyByLMK,hsmCmdBuf+4+1+32+1,32);
			   memcpy(checkValue,hsmCmdBuf+4+1+32+1+32,6);
			   }
			   else
			   {
			   memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			   memcpy(checkValue,hsmCmdBuf+4+1+32,6);
			   }
			   return(32);
			 */
		case    con192BitsDesKey:
			offsetOfKeyByLMK = 4 + 1;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 1 + 48 + 1;
				offsetOfCheckValue = 4 + 1 + 48 + 1 + 48;
			}
			else
				offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;	 // Mary add, 2004-4-12
			/*
			   if (outputByZMK)
			   {
			   memcpy(keyByZMK,hsmCmdBuf+4+1,48);
			   memcpy(keyByLMK,hsmCmdBuf+4+1+48+1,48);
			   memcpy(checkValue,hsmCmdBuf+4+1+48+1+48,6);
			   }
			   else
			   {
			   memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			   memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			   }
			   return(48);
			 */
		default:
			UnionUserErrLog("in UnionHsmCmdA0:: invalid keyLength type [%d]\n",keyLength);
			return(errCodeParameter);
	}
	memcpy(keyByLMK,hsmCmdBuf+offsetOfKeyByLMK,keyLen);
	keyByLMK[keyLen] = 0;
	if (outputByZMK)
	{
		memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
		keyByZMK[keyLen] = 0;
	}
	else 
	{
		// modify by leipp 20150402
		if ((keyByZMK != NULL) && (strlen(keyByZMK) > 0))
			keyByZMK[0] = 0;
	}
	if ((cvLen = ret - offsetOfKeyByZMK - keyLen) > 0)
	{
		if (cvLen >= 16)
			cvLen = 16;
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,cvLen);
		checkValue[cvLen] = 0;
	}
	return(keyLen);
}


int UnionHsmCmd0A()
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"0A",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd0A:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}


int UnionHsmCmdRA()
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"RA",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRA:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}


int UnionHsmCmdPA(char *format)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"PA",2);
	hsmCmdLen = 2;
	strcpy(hsmCmdBuf+hsmCmdLen,format);
	hsmCmdLen += strlen(format);
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


int UnionHsmCmdA2(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;
	int	i;

	memcpy(hsmCmdBuf,"A2",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA2:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	if (gunionIsUseSpecZmkType)
	{
		if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
			memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
	}
	else
		gunionIsUseSpecZmkType = 1;
	hsmCmdLen += ret;

	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA2:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	for (i = 0; i < fldNum; i++)
	{
		if (i > 0)
		{
			hsmCmdBuf[hsmCmdLen] = ';';
			hsmCmdLen++;
		}
		//printf("fld[%d] = [%s]\n",i,fld[i]);
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",fld[i]);
		hsmCmdLen += strlen(fld[i]);
	}

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA2:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	switch (keyLength)
	{
		case    con64BitsDesKey:
			memcpy(component,hsmCmdBuf+4,16);
			component[16] = 0;
			return(16);
		case    con128BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,32);
			component[32] = 0;
			return(32);
		case    con192BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,48);
			component[48] = 0;
			return(48);
		default:
			UnionUserErrLog("in UnionHsmCmdA2:: unknown key length!\n");
			return(errCodeParameter);
	}
}


int UnionHsmCmdCC(int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2, int *pinLen)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	pinLenBuf[4];

	if ((zpk1 == NULL) || (zpk2 == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || (accNo == NULL) ||
			(pinBlockByZPK1 == NULL) || (pinBlockByZPK2 == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdCC:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"CC",2);
	hsmCmdLen = 2;

	if ((ret = UnionGenerateX917RacalKeyString(zpk1Length,zpk1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk1);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(zpk2Length,zpk2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk2);
		return(ret);
	}
	hsmCmdLen += ret;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",maxPinLen);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK1,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,2);
	hsmCmdLen += 2;
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCC:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCC:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(pinLenBuf, hsmCmdBuf+4, 2);
	pinLenBuf[2] = 0;
	*pinLen = atoi(pinLenBuf);

	memcpy(pinBlockByZPK2,hsmCmdBuf+4+2,16);
	pinBlockByZPK2[16] = 0;
	return(16);
}

int UnionHsmCmdMS(char msgNo,int keyType,TUnionDesKeyLength keyLength,char *key,char msgType,char *iv,int msgLen,char *msg,char *mac)
{
	int	ret;
	char	hsmCmdBuf[8096];
	int	hsmCmdLen = 0;

	if ((key == NULL) || (msg == NULL) || (mac == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdMS:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"MS",2);
	hsmCmdLen = 2;

	// ��Ϣ���
	hsmCmdBuf[hsmCmdLen] = msgNo;
	hsmCmdLen += 1;
	//UnionProgramerLog("in UnionHsmCmdMS:: after msgNo hsmCmdBuf [%s]!\n", hsmCmdBuf);

	// ��Կ����	
	hsmCmdBuf[hsmCmdLen] = keyType + '0';
	hsmCmdLen += 1;
	//UnionProgramerLog("in UnionHsmCmdMS:: after keyType hsmCmdBuf [%s]!\n", hsmCmdBuf);

	// ��Կ���ȱ�־
	switch (keyLength)
	{
		case    con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case    con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case    con192BitsDesKey:
			/*
			   hsmCmdBuf[hsmCmdLen] = '2';
			   break;
			 */
		default:
			UnionUserErrLog("in UnionHsmCmdMS:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	//UnionProgramerLog("in UnionHsmCmdMS:: after keyLength hsmCmdBuf [%s]!\n", hsmCmdBuf);

	hsmCmdBuf[hsmCmdLen] = msgType;
	++hsmCmdLen;
	//UnionProgramerLog("in UnionHsmCmdMS:: after msgType hsmCmdBuf [%s]!\n", hsmCmdBuf);
	switch (keyLength)
	{
		case    con64BitsDesKey:
			memcpy(hsmCmdBuf+hsmCmdLen,key,16);
			hsmCmdLen += 16;
			break;
		case    con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'X';
			hsmCmdLen++;
			// 2008/07/11,���ӽ���
			memcpy(hsmCmdBuf+hsmCmdLen,key,32);
			hsmCmdLen += 32;
			break;
		case    con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			memcpy(hsmCmdBuf+hsmCmdLen,key,48);
			hsmCmdLen += 48;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdMS:: wrong keyLength flag [%c]!\n",keyLength);
			return(errCodeParameter);
	}
	//UnionProgramerLog("in UnionHsmCmdMS:: after keyValue hsmCmdBuf [%s]!\n", hsmCmdBuf);

	switch (msgNo)
	{
		case	'2':
		case	'3':
			if (iv == NULL)
			{
				UnionUserErrLog("in UnionHsmCmdMS:: iv is NULL!\n");
				return(errCodeParameter);
			}
			memcpy(hsmCmdBuf+hsmCmdLen,iv,16);
			hsmCmdLen += 16;
			break;
		default:
			break;	
	}

	switch (msgType)
	{
		case    '0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen);
			break;
		case    '1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen/2);
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdMS:: wrong msgtype flag [%c]!\n",msgType);
			return(errCodeParameter);
	}
	hsmCmdLen += 4;
	if (hsmCmdLen + msgLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdMS:: msg length [%d] too long!\n",msgLen);
		return(-1);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,msg,msgLen);
	hsmCmdLen += msgLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdMS:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	mac[16] = 0;
	return(16);
}


/*
   �������ܣ�
   DGָ���PVK����PIN��PVV(PIN Verification Value)��
   ���õļ��ܱ�׼ΪVisa Method
   ���������
   pinLength��LMK���ܵ�PIN���ĳ���
   pin��LMK���ܵ�PIN����
   pvkLength��LMK���ܵ�PVK�Գ���
   pvk��LMK���ܵ�PVK��
   lenOfAccNo���ͻ��ʺų���
   accNo���ͻ��ʺ�
   ���������
   pvv��������PVV

 */
int UnionHsmCmdDG(int pinLength,char *pin,int pvkLength,char *pvk,int lenOfAccNo,char *accNo,char *pvv)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((pin == NULL) || (pvk == NULL) || (accNo == NULL) || (pvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdDG:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"DG",2);
	hsmCmdLen = 2;
	// LMK���ܵ�PVK��
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PIN
	memcpy(hsmCmdBuf+hsmCmdLen,pin,pinLength);
	hsmCmdLen += pinLength;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;

	// modify by leipp 20150418 begin
	// PVKI��0��6֮�䣩
	//memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%d",UnionGetUseSpecPVKI());
	// modify end
	hsmCmdLen += 1;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDG:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ������PVV
	memcpy(pvv,hsmCmdBuf+4,4);
	pvv[4] = 0;
	return(4);
}

/*
   �������ܣ�
   CAָ���һ��TPK���ܵ�PINת��Ϊ��ZPK����
   ���������
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinFormat1��ת��ǰ��PIN��ʽ
   pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   pinFormat2��ת�����PIN��ʽ
   ���������
   pinBlockByZPK��ת������ZPK���ܵ�PIN����
 */
int UnionHsmCmdCA(int tpkLength,char *tpk,int zpkLength,char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((tpk == NULL) || (zpk == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || \
			(accNo == NULL) || (pinBlockByTPK == NULL) || (pinBlockByZPK == NULL))
	{
		UnionUserErrLog("in RacalCmdCA:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"CA",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN��󳤶�
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// ��TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// ת��ǰ��PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,2);
	hsmCmdLen += 2;
	// ת�����PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ת����ZPK���ܵ�PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4+2,16);
	pinBlockByZPK[16] = 0;
	return(16);
}



/*
   �������ܣ�
   JEָ���һ��ZPK���ܵ�PINת��Ϊ��LMK����
   ���������
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinFormat��PIN��ʽ
   pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK��ת������LMK���ܵ�PIN����

 */
int UnionHsmCmdJE(int zpkLength,char *zpk,char *pinFormat,char *pinBlockByZPK,char *accNo,int lenOfAccNo,char *pinBlockByLMK)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((zpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
			(pinBlockByZPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJE:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"JE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ��ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;

	if (strcmp(pinFormat,"04") == 0)
	{
		strcpy(hsmCmdBuf+hsmCmdLen,"FFFFFF");	
		hsmCmdLen += 6;
	}

	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJE:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ת����LMK���ܵ�PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   �������ܣ�
   JCָ���һ��TPK���ܵ�PINת��Ϊ��LMK����
   ���������
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   pinFormat��PIN��ʽ
   pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK��ת������LMK���ܵ�PIN����

 */
int UnionHsmCmdJC(int tpkLength,char *tpk,char *pinFormat,char *pinBlockByTPK,char *accNo,int lenOfAccNo,char *pinBlockByLMK)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((tpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
			(pinBlockByTPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJC:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"JC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ��TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJC:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ת����LMK���ܵ�PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   �������ܣ�
   JGָ���һ��LMK���ܵ�PINת��Ϊ��ZPK����
   ���������
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinFormat��PIN��ʽ
   pinLength��LMK���ܵ�PIN���ĳ���
   pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByZPK��ת������LMK���ܵ�PIN����

 */
int UnionHsmCmdJG(int zpkLength,char *zpk,char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,int lenOfAccNo,char *pinBlockByZPK)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((zpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
			(pinBlockByZPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJG:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"JG",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;

	if (strcmp(pinFormat,"04") == 0)
	{
		strcpy(hsmCmdBuf+hsmCmdLen,"FFFFFF");	
		hsmCmdLen += 6;
	}

	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// ��LMK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJG:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ת����ZPK���ܵ�PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4,16);
	pinBlockByZPK[16] = 0;
	return(16);
}


/*
   �������ܣ�
   JAָ��������һ��PIN
   ���������
   pinLength��Ҫ��������ɵ�PIN���ĵĳ���
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK�����������PIN�����ģ���LMK����

 */
int UnionHsmCmdJA(int pinLength,char *accNo,int lenOfAccNo,char *pinBlockByLMK)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((accNo == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// ������
	memcpy(hsmCmdBuf,"JA",2);
	hsmCmdLen = 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pinLength);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ���������PIN�����ģ���LMK����
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   �������ܣ�
   EEָ���IBM��ʽ����һ��PIN
   ���������
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinOffset��PIN Offset������룬�Ҳ�'F'
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK�����������PIN�����ģ���LMK����

 */
int UnionHsmCmdEE(int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinBlockByLMK)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	errCode[32];

	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
			(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdEE:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"EE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// ���������PIN�����ģ���LMK����
		memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
		pinBlockByLMK[ret-4] = 0;
		return(ret-4);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}


/*
   �������ܣ�
   DEָ���IBM��ʽ����һ��PIN��Offset
   ���������
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinLength��LMK���ܵ�PIN���ĳ���
   pinBlockByLMK����LMK���ܵ�PIN����
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinOffset��PIN Offset������룬�Ҳ�'F'

 */
int UnionHsmCmdDE(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	errCode[32];

	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
			(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdDE:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"DE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// ������PIN��Offset
		memcpy(pinOffset,hsmCmdBuf+4,12);
		pinOffset[12] = 0;
		return(12);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}


/*
   �������ܣ�
   BAָ�ʹ�ñ�������Կ����һ��PIN����
   ���������
   pinCryptogramLen�����ܺ��PIN���ĵĳ���
   pinTextLength��PIN���ĵĳ���
   pinText��PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK��LMK���ܵ�PIN����

 */
int UnionHsmCmdBA(int pinCryptogramLen,int pinTextLength,char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK)
{
	int	ret;
	char	hsmCmdBuf[512],tmpBuf[128];
	int	hsmCmdLen = 0;

	if ((pinText == NULL) || (accNo == NULL) || (pinBlockByLMK == NULL) || (pinTextLength < 0) || (pinCryptogramLen < 0))
	{
		UnionUserErrLog("in UnionHsmCmdBA:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"BA",2);
	hsmCmdLen = 2;

	// ���PIN����
	if ((pinCryptogramLen >= sizeof(tmpBuf)) || (pinTextLength >= sizeof(tmpBuf)))
	{
		UnionUserErrLog("in UnionHsmCmdBA:: pinCryptogramLen = [%d] or pinTextLength = [%d] too long!\n",pinCryptogramLen,pinTextLength);
		return(errCodeParameter);
	}
	memset(tmpBuf,'F',pinCryptogramLen);
	tmpBuf[pinCryptogramLen] = 0;
	memcpy(tmpBuf,pinText,pinTextLength);

	// �����PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetMaskPrintTypeForHSMReqCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// LMK���ܵ�PIN����
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   �������ܣ�
   NGָ�ʹ�ñ�������Կ����һ��PIN����
   ���������
   pinCryptogramLen��LMK���ܵ�PIN���ĵĳ���
   pinCryptogram��LMK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   referenceNumber����LMK18-19�����ʺŵõ���ƫ��ֵ
   pinText��PIN����

 */
int UnionHsmCmdNG(int pinCryptogramLen,char *pinCryptogram,char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText)
{
	int	ret;
	char	hsmCmdBuf[512],tmpBuf[128];
	int	hsmCmdLen = 0,i;

	if ((pinCryptogram == NULL) || (accNo == NULL) || (referenceNumber == NULL) || (pinText == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdNG:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"NG",2);
	hsmCmdLen = 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinCryptogram,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetMaskPrintTypeForHSMResCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNG:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ��LMK18-19�����ʺŵõ���ƫ��ֵ
	memcpy(referenceNumber,hsmCmdBuf+4+pinCryptogramLen,12);
	referenceNumber[12] = 0;

	// ����PIN����
	memcpy(tmpBuf,hsmCmdBuf+4,pinCryptogramLen);
	for (i=0;i<pinCryptogramLen;i++)
	{
		if (tmpBuf[i] == 'F')
		{
			tmpBuf[i] = '\0';
			break;
		}
	}
	// PIN����
	memcpy(pinText,tmpBuf,i);
	pinText[i] = 0;
	return(i);
}


/*
   �������ܣ�
   DAָ���IBM��ʽ��֤�ն˵�PIN
   ���������
   pinBlockByTPK��TPK���ܵ�PIN����
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinOffset��IBM Offset������룬�Ҳ�'F'
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdDA(char *pinBlockByTPK,int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((pinBlockByTPK == NULL) || (pinValidData == NULL) || (pinOffset == NULL) || \
			(decimalizationTable == NULL) || (tpk == NULL) || (pvk == NULL) || \
			(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdDA:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"DA",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ���PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	// IBM Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}

/*
   �������ܣ�
   EAָ���IBM��ʽ��֤�������ĵ�PIN
   ���������
   pinBlockByZPK��ZPK���ܵ�PIN����
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinOffset��IBM Offset������룬�Ҳ�'F'
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pvkLength��LMK���ܵ�TPK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdEA(char *pinBlockByZPK,int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	errCode[32];
	int	pinLen = 0;
	char	*ptr = NULL;

	if ((pinBlockByZPK == NULL) || (pinValidData == NULL) || (pinOffset == NULL) || \
			(zpk == NULL) || (pvk == NULL) || (decimalizationTable == NULL) || \
			(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdEA:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"EA",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ���PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	// IBM Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// add by leipp 20160306
	if ((ptr = strchr(pinOffset,'F')) == NULL)
		pinLen = 12;
	else
		pinLen = ptr - pinOffset;
	// add by leipp end  20160306

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	// modify by leipp 20160306 ��������Ϊ88ʱ,���pinOffset��pin���ĳ���,������ȵ���4,������ȷ,���򱨴���
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0) || (memcmp(errCode,"88",2) == 0 && pinLen == 4))
		return(0);
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
	// modify end 20160306
}


/*
   �������ܣ�
   DCָ���VISA��ʽ��֤�ն˵�PIN
   ���������
   pinBlockByTPK��TPK���ܵ�PIN����
   pvv���ն�PIN��4λVISA PVV
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */

int UnionHsmCmdDC(char *pinBlockByTPK,char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((pinBlockByTPK == NULL) || (pvv == NULL) || (tpk == NULL) || \
			(pvk == NULL) || (pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdDC:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"DC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	hsmCmdLen += 1;
	// �ն�PIN��4λVISA PVV
	memcpy(hsmCmdBuf+hsmCmdLen,pvv,4);
	hsmCmdLen += 4;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDC:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}

/*
   �������ܣ�
   ECָ���VISA��ʽ��֤�������ĵ�PIN
   ���������
   pinBlockByZPK��ZPK���ܵ�PIN����
   pvv���ն�PIN��4λVISA PVV
   zpkLength��LMK���ܵ�ZPK
   zpk��LMK���ܵ�ZPK
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */

int UnionHsmCmdEC(char *pinBlockByZPK,char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((pinBlockByZPK == NULL) || (pvv == NULL) || (zpk == NULL) || \
			(pvk == NULL) || (pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdEC:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"EC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	hsmCmdLen += 1;
	// �ն�PIN��4λVISA PVV
	memcpy(hsmCmdBuf+hsmCmdLen,pvv,4);
	hsmCmdLen += 4;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEC:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}

/*
   �������ܣ�
   BCָ��ñȽϷ�ʽ��֤�ն˵�PIN
   ���������
   pinBlockByTPK��TPK���ܵ�PIN����
   tpkLength��LMK���ܵ�TPK
   tpk��LMK���ܵ�TPK
   pinByLMKLength������PIN���ĳ���
   pinByLMK������PIN���ģ���LMK02-03����
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdBC(char *pinBlockByTPK,int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,char *pinFormat,char *accNo,int lenOfAccNo)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((pinBlockByTPK == NULL) || (pinByLMK == NULL) || (tpk == NULL) || \
			(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdBC:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"BC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// ����PIN���ģ���LMK02-03����
	memcpy(hsmCmdBuf+hsmCmdLen,pinByLMK,pinByLMKLength);
	hsmCmdLen += pinByLMKLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBC:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}


/*
   �������ܣ�
   BEָ��ñȽϷ�ʽ��֤�������ĵ�PIN
   ���������
   pinBlockByZPK��ZPK���ܵ�PIN����
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinByLMKLength������PIN���ĳ���
   pinByLMK������PIN���ģ���LMK02-03����
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdBE(char *pinBlockByZPK,int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,char *pinFormat,char *accNo,int lenOfAccNo)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((pinBlockByZPK == NULL) || (pinByLMK == NULL) || (zpk == NULL) || \
			(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdBE:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"BE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// ����PIN���ģ���LMK02-03����
	memcpy(hsmCmdBuf+hsmCmdLen,pinByLMK,pinByLMKLength);
	hsmCmdLen += pinByLMKLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBE:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}


/*
   �������ܣ�
   CWָ�����VISA��У��ֵCVV
   ���������
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   cvv�����ɵ�VISA����CVV

 */
int UnionHsmCmdCW(char *cardValidDate,int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *cvv)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdCW:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"CW",2);
	hsmCmdLen = 2;
	// LMK���ܵ�CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCW:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// �ͻ��ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA������Ч��
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA�������̴���
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCW:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// ���ɵ�VISA����CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	cvv[3] = 0;
	return(3);
}


/*
   �������ܣ�
   CYָ���֤VISA����CVV
   ���������
   cvv������֤��VISA����CVV
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdCY(char *cvv,char *cardValidDate,int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdCY:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"CY",2);
	hsmCmdLen = 2;
	// LMK���ܵ�CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCY:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ����֤��VISA����CVV
	memcpy(hsmCmdBuf+hsmCmdLen,cvv,3);
	hsmCmdLen += 3;
	// �ͻ��ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA������Ч��
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA�������̴���
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCY:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}


int UnionHsmCmdBU(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	char		tmpBuf[128];
	char		tmpBuf1[128];

	if ((key == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdBU:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;

	// ������Կ��LMK��ָʾ��������Կ���ʹ�������2λ
	memset(tmpBuf,0,sizeof(tmpBuf));
	memset(tmpBuf1,0,sizeof(tmpBuf1));
	if ((ret = UnionTranslateHsmKeyTypeStringForBU(keyType,tmpBuf,tmpBuf1)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBU:: UnionTranslateHsmKeyTypeStringForBU!\n");
		return(ret);
	}

	/*
	   if ((ret = UnionTranslateHsmKeyTypeString(keyType,tmpBuf)) < 0)
	   {
	   UnionUserErrLog("in UnionHsmCmdBU:: UnionTranslateHsmKeyTypeString!\n");
	   return(ret);
	   }*/
	if (gunionIsUseSpecZmkType)
	{
		if (memcmp(tmpBuf,"000",3) == 0)
			memcpy(tmpBuf,"010",3);	
	}
	else
		gunionIsUseSpecZmkType = 1;

	memcpy(hsmCmdBuf+hsmCmdLen,&tmpBuf[1],2);
	hsmCmdLen += 2;
	// ��Կ���ȱ�־
	switch (keyLength)
	{
		case    con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case    con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case    con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '2';
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdBU:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	// LMK���ܵ���Կ����
	if (SM4Mode == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, key, strlen(key));
		hsmCmdLen += strlen(key);
	}
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdBU:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	if (strcmp(tmpBuf,"FFF") == 0)
	{
		// �ָ���
		hsmCmdBuf[hsmCmdLen] = ';';
		hsmCmdLen += 1;
		// ��Կ���ʹ���
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf1,3);
		hsmCmdLen += 3;
	}
	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBU:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(checkValue,hsmCmdBuf+4,16);
	checkValue[16] = 0;
	return(16);
}


/*
   �������ܣ�
   A4ָ��ü�����Կ�����ĳɷֺϳ�һ����Կ��������У��ֵ
   ���������
   keyType����Կ������
   keyLength����Կ�ĳ���
   partKeyNum����Կ�ɷֵ�����
   partKey����Ÿ�����Կ�ɷֵ����黺�壬ΪLMK���ܵ���Կ����
   ���������
   keyByLMK���ϳɵ���Կ���ģ���LMK����
   checkValue���ϳɵ���Կ��У��ֵ

 */
int UnionHsmCmdA4(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;
	int	i;

	if ((keyByLMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdA4:: wrong parameters!\n");
		return(errCodeParameter);
	}
	if (partKeyNum < 2 || partKeyNum > 9)
	{
		UnionUserErrLog("in UnionHsmCmdA4:: partKeyNum [%d] must between 2 and 9!\n",partKeyNum);
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"A4",2);
	hsmCmdLen = 2;
	// ��Կ�ɷ�����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// ��Կ���ʹ���
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA4:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	if (SM4Mode != 1)
	{
		if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
		}
	}
	// Mary add end, 2004-4-15
	hsmCmdLen += ret;
	// ��Կ���ȱ�־
	if (SM4Mode == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
	}
	else
	{
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdA4:: UnionTranslateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
	}

	for (i = 0; i < partKeyNum; i++)
	{
		// LMK���ܵ���Կ����
		if (SM4Mode == 1)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
			hsmCmdLen += 1;
			memcpy(hsmCmdBuf+hsmCmdLen,partKey[i],strlen(partKey[i]));
			hsmCmdLen += strlen(partKey[i]);
		}
		else
		{
			if ((ret = UnionGenerateX917RacalKeyString(keyLength,partKey[i],hsmCmdBuf+hsmCmdLen)) < 0)
			{
				UnionUserErrLog("in UnionHsmCmdA4:: UnionGenerateX917RacalKeyString for [%s][%d]\n",partKey[i],keyLength);
				return(ret);
			}
			hsmCmdLen += ret;
		}
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA4:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	switch (keyLength)
	{
		case    con64BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			keyByLMK[16] = 0;
			// �ϳɵ���Կ��У��ֵ
			memcpy(checkValue,hsmCmdBuf+4+16,6);
			checkValue[6] = 0;
			return(16+6);
		case    con128BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			keyByLMK[32] = 0;
			// �ϳɵ���Կ��У��ֵ
			if (SM4Mode == 1)
			{
				memcpy(checkValue,hsmCmdBuf+4+1+32,16);
				checkValue[16] = 0;
				return(32+16);
			}
			else
			{
				memcpy(checkValue,hsmCmdBuf+4+1+32,6);
				checkValue[6] = 0;
				return(32+6);
			}
		case    con192BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			keyByLMK[48] = 0;
			// �ϳɵ���Կ��У��ֵ
			memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			checkValue[6] = 0;
			return(48+6);
		default:
			UnionUserErrLog("in UnionHsmCmdA4:: unknown key length!\n");
			return(errCodeParameter);
	}
}



/*
   ����    ��Դ�ʺ����ɵ�PINBLOCKת��Ϊ��Ŀ���ʺ����ɵ�PINBLOCK������ָ������Կ���ܱ���
   ˵��    ������Կ����ȷ���㷨
   �������
   zpk �� LMK 06-07 ���ܵ�ZPK
   pvk �� LMK 14-15���ܵ�ZPK
   maxPinLen ���PIN����
   decimalizationTable ʮ��������
   pinValidationData 
   pinBlock1 ��ZPK����ANSI X9.8��׼����
   accNo1 Դ�˺�
   accNo2 Ŀ���˺�

 */

int UnionHsmCmdDF(char *zpk,char *pvk,int maxPinLen,char *decimalizationTable,char *pinValidationData,char *pinBlock1,char *accNo1,char *accNo2,char *pinBlock2,char *pinOffset)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	errCode[32];

	if ((zpk == NULL) || (pvk == NULL) || (decimalizationTable == NULL) ||
			(pinValidationData == NULL) || (pinBlock1 == NULL) || (accNo1 == NULL) || (accNo2 == NULL) ||
			(pinBlock2 == NULL) || (pinOffset == NULL))
	{
		UnionUserErrLog("in RacalCmdDF:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// ������
	memcpy(hsmCmdBuf,"DF",2);
	hsmCmdLen = 2;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionGenerateX917RacalKeyString for [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(pvk),pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionGenerateX917RacalKeyString for [%s]\n",pvk);
		return(ret);
	}
	hsmCmdLen += ret;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",maxPinLen);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidationData,12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock1,16);
	hsmCmdLen += 16;

	// modified 2014-04-25
	/*
	   memcpy(hsmCmdBuf+hsmCmdLen,accNo1,12);
	   hsmCmdLen += 12;
	   memcpy(hsmCmdBuf+hsmCmdLen,accNo2,12);
	   hsmCmdLen += 12;
	 */
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo1,strlen(accNo1),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo1,(int)strlen(accNo1));
		return(ret);
	}
	hsmCmdLen += ret;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo2,strlen(accNo2),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo2,(int)strlen(accNo2));
		return(ret);
	}
	hsmCmdLen += ret;
	// end of modification 2014-04-25
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		memcpy(pinBlock2,hsmCmdBuf+4,16);
		memcpy(pinOffset,hsmCmdBuf+4+16,12);
		return(16);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}


int UnionHsmCmdA8(TUnionDesKeyType keyType,char *key,char *zmk,char *keyByZMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	keyLen;
	int	offsetOfKeyByZMK;
	int	offsetOfCheckValue;

	memcpy(hsmCmdBuf,"A8",2);
	hsmCmdLen = 2;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(zmk),zmk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(key),key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	switch (strlen(key))
	{
		case    16:
			memcpy(hsmCmdBuf+hsmCmdLen,"Z",1);
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdA8:: keylen [%d] error!\n",(int)strlen(key));
			return(errCodeParameter);
	}
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA8:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	switch (strlen(key))
	{
		case    16:
			offsetOfKeyByZMK = 4;
			offsetOfCheckValue = 4 + 16;
			keyLen = 16;
			break;
		case    32:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 32;
			keyLen = 32;
			break;
		case    48:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;	 // Mary add, 2004-4-12
		default:
			UnionUserErrLog("in UnionHsmCmdA8:: invalid keyLen[%d]\n",(int)strlen(key));
			return(errCodeParameter);
	}
	memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	keyByZMK[keyLen] = 0;

	if (ret - offsetOfCheckValue > 16)
	{
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,16);
		checkValue[16] = 0;
	}
	else
	{
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,ret - offsetOfCheckValue);
		checkValue[ret - offsetOfCheckValue] = 0;
	}
	return(keyLen);
}


// ��һ��LMK���ܵ���Կת��ΪZMK����,SM4�㷨
int UnionHsmCmdST(TUnionDesKeyType keyType, char *key, char *zmk, char *keyByZMK, char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	offset = 0;

	memcpy(hsmCmdBuf,"ST",2);
	hsmCmdLen = 2;

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdST:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//ZMK��Կ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,zmk,32);
	hsmCmdLen += 32;

	//LMK�¼��ܵ���Կ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,key,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdST:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (strcmp(hsmCmdBuf+offset, "S") == 0)
	{
		UnionUserErrLog("in UnionHsmCmdST:: Key LENGTH ERROR!\n");
		return(ret);
	}
	offset += 1;
	memcpy(keyByZMK,hsmCmdBuf+offset,32);
	keyByZMK[32] = 0;
	offset += 32;
	memcpy(checkValue,hsmCmdBuf+offset,16);
	checkValue[16] = 0;

	return(0);
}


/*
   �������ܣ�
   33ָ�˽Կ����
   ���������
   flag���ù�Կ����ʱ�����õ���䷽ʽ��
   '0'��������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
   '1'��PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   vkIndex��˽Կ������"00"��"20"
   lenOfVK��˽Կ����
   vk��˽Կ����
   cipherDataLen���������ݵ��ֽ���
   cipherData�����ڽ��ܵ���������
   sizeOfPlainData��plainData���ڵĴ洢�ռ��С
   ���������
   plainData�����ܵõ�����������

 */
int UnionHsmCmd33(char flag,char *vkIndex,int lenOfVK,char *vk,int cipherDataLen,char *cipherData,char *plainData,int sizeOfPlainData)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;

	if (vkIndex==NULL || cipherDataLen<=0 || cipherData==NULL || plainData==NULL || (flag!='0' && flag!='1') || sizeOfPlainData<=0)
	{
		UnionUserErrLog("in UnionHsmCmd33:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"33",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;

	if ((vkIndex != NULL) && (strcmp(vkIndex,"99") != 0) && (atoi(vkIndex) >= 0))
	{
		memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
		hsmCmdLen += 2;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen,vk,lenOfVK);
		hsmCmdLen += lenOfVK;
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",cipherDataLen);
	hsmCmdLen += 4;
	if (hsmCmdLen + cipherDataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmd33:: cipherDataLen [%d] too long!\n",cipherDataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,cipherData,cipherDataLen);
	hsmCmdLen += cipherDataLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();
	UnionSetMaskPrintTypeForHSMResCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd33:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if ((len = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd33:: signLen error!\n");
		return(errCodeSmallBuffer);
	}

	memcpy(plainData,hsmCmdBuf+4+4,len);
	return(len);
}


/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   pvkIndex��      ָ����˽Կ�����ڽ���PIN��������
   fillMode��      ��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
   ��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   ZPK��	  ���ڼ���PIN����Կ��
   accNo��	�û����ʺ�
lenOfPinBlock:  ��Կ���ܵ�PIN���ĵĳ���
pinBlock��      ����Կ���ܵ�PIN��������
�������
lenOfPin��      ��Կ����
pinBlock1��     ����ZPK�����µ�PIN����
pinBlock2��     ����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
lenOfUniqueID�� 01-20
UniqueID��      ���صľ�ASCII��չ��ID������

 */
int UnionHsmCmdH2 (int pvkIndex, char fillMode, char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;
	int	offset;
	int	lenOfID;

	if ((fillMode!='0' && fillMode!='1') || ZPK==NULL || accNo==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdH2:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"H2",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pvkIndex);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = fillMode;
	hsmCmdLen++;

	switch (len = strlen(ZPK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH2:: invalid [%s]\n",ZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZPK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,strlen(accNo));
	hsmCmdLen += strlen(accNo);

	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,lenOfPinBlock);
	hsmCmdLen += lenOfPinBlock;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH2:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(lenOfPin,hsmCmdBuf+4,2);
	if ((offset = UnionConvertIntoLen(lenOfPin,2)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH2:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	memcpy(pinBlock1,hsmCmdBuf+4+2,32);
	memcpy(pinBlock2,hsmCmdBuf+4+2+32,48);
	memcpy(lenOfUniqueID,hsmCmdBuf+4+2+32+48,2);

	if ((offset = UnionConvertIntoLen(lenOfUniqueID,2)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH2:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	lenOfID = atoi(lenOfUniqueID)*2;
	if(lenOfID > strlen(hsmCmdBuf+4+2+32+48+2))
	{
		UnionUserErrLog("in UnionHsmCmdH2:: lenOfUniqueID [%d] too small!\n",lenOfID);
		return(errCodeSmallBuffer);
	}
	memcpy(UniqueID,hsmCmdBuf+4+2+32+48+2,lenOfID);
	return(lenOfID);
}


/*
����
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
   srcAlgorithmID  �ǶԳ���Կ�㷨��ʶ��0:�����㷨,  1:�����㷨
   pvkIndex��   ָ����˽Կ�����ڽ���PIN��������
   lenOfVK	˽Կ����
   vkValue	˽Կֵ
   fillMode��   ��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
		��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   dstAlgorithmID  ZPK��Կ�㷨��ʶ,0:�����㷨,  1:�����㷨
   ZPK��	���ڼ���PIN����Կ��
   accNo��	�û����ʺ�
   lenOfPinBlock:  ��Կ���ܵ�PIN���ĵĳ���
   pinBlock��      ����Կ���ܵ�PIN��������
�������
   lenOfPin��      ��Կ����
   pinBlock1��     ����ZPK�����µ�PIN����
   pinBlock2��     ����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
   lenOfUniqueID�� 01-20
   UniqueID��      ���صľ�ASCII��չ��ID������

 */
int UnionHsmCmdN6(int srcAlgorithmID,int pvkIndex, int lenOfVK,char *vkValue,char fillMode, int dstAlgorithmID,char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	offset;
	int	lenOfID;

	if ((fillMode!='0' && fillMode!='1') || ZPK==NULL || accNo==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdN6:: parameter error!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"N6",2);
	hsmCmdLen = 2;

	if (srcAlgorithmID == 1)	// �����㷨
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
	}

	if (dstAlgorithmID == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"H",1);
		hsmCmdLen += 1;
	}

	//˽Կ����
	if (pvkIndex == 99)
	{
		if (lenOfVK <= 0 || vkValue == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdN6:: lenOfVK[%d] <= 0 or vkValue == null parameter error!\n",lenOfVK);
			return(errCodeParameter);
		}

		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pvkIndex);
		hsmCmdLen += 2;

		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue,lenOfVK,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += lenOfVK/2;
	}
	else
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pvkIndex);
		hsmCmdLen += 2;
	}

	// �����㷨���д���
	if (srcAlgorithmID == 0)
	{
		//�ù�Կ����ʱ�����õ���䷽ʽ
		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%c",fillMode);
		hsmCmdLen++;
	}

	if (dstAlgorithmID == 0)
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(ZPK,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdN6:: UnionPutKeyIntoRacalKeyString [%s]!\n",ZPK);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen,ZPK,32);
		hsmCmdLen += 32;
	}

	if ((ret = UnionForm12LenAccountNumber (accNo, strlen(accNo), hsmCmdBuf + hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdN6:: UnionForm12LenAccountNumber!\n");
		return (ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,lenOfPinBlock);
	hsmCmdLen += lenOfPinBlock;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdN6:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(lenOfPin,hsmCmdBuf+offset,2);
	lenOfPin[2] = 0;
	offset += 2;
	if ((ret = UnionConvertIntoLen(lenOfPin,2)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdN6:: signLen[%d] error!\n",ret);
		return(errCodeSmallBuffer);
	}

	if (dstAlgorithmID == 0)
	{
		memcpy(pinBlock1,hsmCmdBuf+offset,32);
		pinBlock1[32] = 0;
		offset += 32;
		memcpy(pinBlock2,hsmCmdBuf+offset,48);
		pinBlock2[48] = 0;
		offset += 48;
	}
	else
	{
		memcpy(pinBlock1,hsmCmdBuf+offset,64);
		pinBlock1[64] = 0;
		offset += 64;
		memcpy(pinBlock2,hsmCmdBuf+offset,64);
		pinBlock2[64] = 0;
		offset += 64;
	}
	memcpy(lenOfUniqueID,hsmCmdBuf+offset,2);
	lenOfUniqueID[2] = 0;
	offset += 2;

	if ((ret = UnionConvertIntoLen(lenOfUniqueID,2)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdN6:: offset[%d] error!\n",ret);
		return(errCodeSmallBuffer);
	}
	lenOfID = atoi(lenOfUniqueID)*2;
	if(lenOfID > strlen(hsmCmdBuf+offset))
	{
		UnionUserErrLog("in UnionHsmCmdN6:: lenOfUniqueID [%d] too small!\n",lenOfID);
		return(errCodeSmallBuffer);
	}
	memcpy(UniqueID,hsmCmdBuf+offset,lenOfID);
	UniqueID[lenOfID] = 0;
	return(lenOfID);
}

/*
   ���ܣ�
   ��PIN��X9.8תΪ�����㷨����
   ����:
   srcZPK ��ǰ����PIN���ZPK��LMK�ԣ�06-07���¼���
   dstZPK ��Ҫ����PIN���ZPK��LMK�ԣ�06-07���¼��ܡ�
   srcPin ԴZPK���ܵ��ַ���������
   srcPan �û���Ч���ʺ�
   dstPan �û���Ч���ʺ�
   ���:
   lenOfPin	pin���ĳ���
   dstPinBlock ����PIN���� 
 */
int UnionHsmCmdN7(char* srcZPK, char* dstZPK, char *srcPinBlock, char* srcPan, char* dstPan, int* lenOfPin, char* dstPinBlock)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;
	int 	offset = 0;	
	char 	tmpBuf[32];

	if ((srcZPK == NULL) &&	(dstZPK == NULL) && (srcPinBlock == NULL) && (srcPan == NULL) && (dstPan == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdN7:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "N7", 2);
	hsmCmdLen += 2;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(srcZPK), srcZPK, hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdN7:: UnionGenerateX917RacalKeyString [%s]\n", srcZPK);
		return(ret);
	}

	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(dstZPK), dstZPK, hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdN7:: UnionGenerateX917RacalKeyString [%s]\n", srcZPK);
		return(ret);
	}

	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen, srcPinBlock, strlen(srcPinBlock));
	hsmCmdLen += strlen(srcPinBlock);

	if ((ret = UnionForm12LenAccountNumber (srcPan, strlen(srcPan), hsmCmdBuf + hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdN7:: UnionForm12LenAccountNumber!\n");
		return (ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionForm12LenAccountNumber (dstPan, strlen(dstPan), hsmCmdBuf + hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdN7:: UnionForm12LenAccountNumber!\n");
		return (ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdN7:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;

	memcpy(tmpBuf, hsmCmdBuf + offset, 2);
	tmpBuf[2] = 0;

	*lenOfPin = atoi(tmpBuf);

	offset += 2;

	memcpy(dstPinBlock, hsmCmdBuf+offset, 48);
	dstPinBlock[48] = 0;
	return (*lenOfPin);
}

/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   OriKeyType��    ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
   OriKey��	ԴKEY�����ڽ���PIN����Կ��
   lenOfAcc��      �ʺų��ȣ�4~20��
   accNo��	�û����ʺš�
   DesKeyType��    ������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
   DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
   pinBlock��      ����Կ���ܵ�PIN�������ġ�
   �������
   lenOfPinBlock1�����볤�ȡ�
   pinBlock1��     ����ZPK2/PVK�����µ�PIN���ġ�

 */
int UnionHsmCmdH5(char *OriKey, int lenOfAcc, char *AccNo, char DesKeyType, char *DesKey, char *pinBlock, char *lenOfPinBlock1,char *pinBlock1)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;

	if (OriKey==NULL || AccNo==NULL || (DesKeyType!='0' && DesKeyType!='1') || DesKey==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdH5:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"H5",2);
	hsmCmdLen = 2;

	switch (len = strlen(OriKey))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH5:: invalid [%s]\n",OriKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,OriKey,len);
	hsmCmdLen += len;

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAcc);
	hsmCmdLen += 2;

	//accLen = atoi(lenOfAcc);
	memcpy(hsmCmdBuf+hsmCmdLen,AccNo, lenOfAcc);
	hsmCmdLen += lenOfAcc;
	hsmCmdBuf[hsmCmdLen] = DesKeyType;
	hsmCmdLen++;

	switch (len = strlen(DesKey))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH5:: invalid [%s]\n",DesKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,DesKey,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH5:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(pinBlock1,hsmCmdBuf+4,32);
	return(ret);
}


/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   OriKeyType��    ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
   OriKey��	ԴKEY�����ڽ���PIN����Կ��
   lenOfAcc��      �ʺų��ȣ�4~20��
   accNo��	�û����ʺš�
   DesKeyType��    ������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
   DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
   pinBlock��      ����Կ���ܵ�PIN�������ġ�
   �������
   lenOfPinBlock1�����볤�ȡ�
   pinBlock1��     ����ZPK2/PVK�����µ�PIN���ġ�

 */
int UnionHsmCmdH7(char *OriKey, int lenOfAcc,char *AccNo, char *DesKey, char *pinBlock, char *lenOfPinBlock1,char *pinBlock1)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;

	if (OriKey==NULL || AccNo==NULL || DesKey==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdH7:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"H7",2);
	hsmCmdLen = 2;

	switch (len = strlen(OriKey))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH7:: invalid [%s]\n",OriKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,OriKey,len);
	hsmCmdLen += len;

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAcc);
	hsmCmdLen += 2;

	//accLen = atoi(lenOfAcc);
	memcpy(hsmCmdBuf+hsmCmdLen, AccNo, lenOfAcc);
	hsmCmdLen += lenOfAcc;

	switch (len = strlen(DesKey))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH7:: invalid [%s]\n",DesKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,DesKey,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH7:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(pinBlock1,hsmCmdBuf+4,32);
	return(ret);
}



/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   ZPK��	  ���ڽ���PIN����Կ��
   lenOfAcc��      �ʺų��ȣ�4~20��
   accNo��	�û����ʺš�
   pinBlockByZPK�� ��ZPK���ܵ�PIN�������ġ�
   PVK��	  PVK��
   pinBlockByPVK�� ��PVK���ܵ�PIN�������ġ�
   �������


 */
int UnionHsmCmdH9(char *ZPK, int lenOfAcc, char *AccNo, char *pinBlockByZPK, char *PVK, char *pinBlockByPVK)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;

	if (ZPK==NULL || AccNo==NULL ||pinBlockByZPK==NULL || PVK==NULL || pinBlockByPVK==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdH9:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"H9",2);
	hsmCmdLen = 2;

	switch (len = strlen(ZPK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH9:: invalid [%s]\n",ZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZPK,len);
	hsmCmdLen += len;

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAcc);
	hsmCmdLen += 2;

	//accLen = atoi(lenOfAcc);
	memcpy(hsmCmdBuf+hsmCmdLen, AccNo, lenOfAcc);
	hsmCmdLen += lenOfAcc;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,32);
	hsmCmdLen += 32;

	switch (len = strlen(PVK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH9:: invalid [%s]\n",PVK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,PVK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByPVK,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH9:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(ret);
}

/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   ZPK��	  ���ڼ���PIN����Կ��
   accNo��	�û����ʺš�
   pinBlockByZPK�� ��ZPK���ܵ�PIN�������ġ�
   dataOfZAK��     ����ZAK��Կ������֮һ��
   �������
   lenOfPin��      ���볤��
   ZAK��	  ����LMK26-27�ԶԼ����µ�ZAK��Կ��


 */
int UnionHsmCmdH4 (char *ZPK, char *AccNo, char *pinBlockByZPK, char *dataOfZAK, char *lenOfPin, char *ZAK)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;

	if (ZPK==NULL || AccNo==NULL || pinBlockByZPK==NULL || dataOfZAK==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdH4:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"H4",2);
	hsmCmdLen = 2;
	switch (len = strlen(ZPK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdH4:: invalid [%s]\n",ZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZPK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,AccNo,12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,48);
	hsmCmdLen += 48;
	memcpy(hsmCmdBuf+hsmCmdLen,dataOfZAK,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH4:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(lenOfPin,hsmCmdBuf+4,2);
	memcpy(ZAK,hsmCmdBuf+4+2+1,32);

	return(ret-4-2);

}

/*
   ���ܣ���PinOffsetת��Ϊר���㷨��FINSE�㷨�����ܵ�����
   ������� 
   PVK LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset
   pinOffSet Offset��ֵ�����������ķ�ʽ���ұ�����ַ���F��
   minPinLen ��С��PIN����
   accNo �˺�
   decimalizationTable ʮ����ת����
   pinValidationData PINУ������
   �������
   pinBlock PIN����

 */
int UnionHsmCmdS1(char *PVK, char *pinOffSet,int minPinLen,char *accNo,char *decimalizationTable,char *pinValidationData, char *pinBlock)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;

	if (PVK==NULL || pinOffSet==NULL || minPinLen < 0|| accNo==NULL || decimalizationTable==NULL || pinValidationData==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdS1:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"S1",2);
	hsmCmdLen = 2;
	switch (len = strlen(PVK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdS1:: invalid [%s]\n",PVK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,PVK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffSet,12);
	hsmCmdLen += 12;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPinLen);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,12);
	hsmCmdLen += 12;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// pinValidationdata
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidationData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdS1:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(pinBlock,hsmCmdBuf+4,6);
	return(ret-4);
}


/*
   ���ܣ���PinBlockת��Ϊר���㷨��FINSE�㷨�����ܵ�����
   �������
   ZPK LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset
   minPinLen ��С��PIN����
   accNo �˺�
   OriPinBlock ԴPIN��
   �������
   pinBlock PIN���� 

 */
int UnionHsmCmdS2(char *ZPK, int minPinLen,char *accNo,char *OriPinBlock, char *pinBlock)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;

	if (ZPK==NULL || OriPinBlock==NULL || minPinLen < 0|| accNo==NULL )
	{
		UnionUserErrLog("in UnionHsmCmdS2:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"S2",2);
	hsmCmdLen = 2;
	switch (len = strlen(ZPK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdS2:: invalid [%s]\n",ZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZPK,len);
	hsmCmdLen += len;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPinLen);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf+hsmCmdLen,OriPinBlock,16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdS2:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(pinBlock,hsmCmdBuf+4,6);
	return(ret-4);
}


/*
   ���ܣ�������ɢMAK����MAC
   �������
   ZAK ��LMK26-27�Լ���
   scatData ���ڼ�����ɢ��Կ������
   lenOfmac MAC���ݳ���
   macData MAC����
   �������
   mac ���ص�MACֵ

   �������̣�
   1.      ��ZAK��Կ����ɢ���ݲ���IC����׼��ɢ�㷨������ɢ�����ZAKΪ64bit������ֱ�Ӷ���ɢ���ݽ���DES���ܼ��ɡ�
   2.      ����ɢ��Կ��MAC���ݲ���Ansi9.19��ZAK˫�������ϣ���9.9��ZAK���������㷨����MAC��
 */
int UnionHsmCmdG1(char *ZAK, char *scatData,char *lenOfmac,char *macData, char *mac)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	int	len;
	int	lenmac;

	if (ZAK==NULL || scatData==NULL || lenOfmac == NULL|| macData==NULL )
	{
		UnionUserErrLog("in UnionHsmCmdG1:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"G1",2);
	hsmCmdLen = 2;
	switch (len = strlen(ZAK))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdG1:: invalid [%s]\n",ZAK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZAK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,scatData,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,lenOfmac,3);
	hsmCmdLen += 3;

	lenmac = UnionOxToD(lenOfmac);

	memcpy(hsmCmdBuf+hsmCmdLen,macData,lenmac);
	hsmCmdLen += lenmac;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdG1:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	return(ret-4);
}


/*
   ���������ָ��
 */
int UnionHsmCmdY3(int min,int max,int Cnt,int isRepeat,char *RandNo)
{
	int     ret = -1;
	char	hsmCmdBuf[1024] = {0};
	int     hsmCmdLen = 0;

	hsmCmdLen = sprintf(hsmCmdBuf,"%s%03d%03d%03d%d","Y3",min,max,Cnt,isRepeat);
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY3:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(RandNo,hsmCmdBuf+4,3);
	RandNo[3] = 0;
	return( ret -4 );

}



#ifndef MAXMSGBLOCK
#define MAXMSGBLOCK 4000 
//#define MAXMSGBLOCK 512 
#endif
/*һ���Է��ͼ��ܻ����ݿ���󳤶�*/
int UnionHsmCmdE0(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData)
{
	int     nLoop,i;
	int     ret;
	int     p_len =0;
	int     lastDataLen;

	UnionSetBCDPrintTypeForHSMCmd();

	*encryDataLen = 0;
	ret = MsgLen % 8 ;

	if ( ret != 0 )
		ret = MsgLen + 8 - ret;
	else
		ret = MsgLen;
	if ( encrypData == NULL )
	{
		*encryDataLen = ret;
		return ret;
	}
	else
		if ( ret > sizeOfEncryptedData )
		{
			UnionUserErrLog("in UnionHsmCmdE0 : SizeOfBuffer[%d] > sizeOfData[%d]\n",ret,sizeOfEncryptedData);
			return (errCodeSmallBuffer);
		}


	if ( MsgLen <= MAXMSGBLOCK )
	{
		return UnionHsmCmdE0_f(crytoFlag,blockFlag,encrypMode,zekLen,\
				Zek,datatype,IV,MsgLen,Msg,\
				encrypData,encryDataLen,sizeOfEncryptedData);
	}

	if ( MsgLen > MAXMSGBLOCK )
	{
		nLoop = MsgLen / MAXMSGBLOCK;

		for ( i = 0; i < nLoop; i++ )
		{
			ret = UnionHsmCmdE0_f(crytoFlag,blockFlag,encrypMode,zekLen,\
					Zek,datatype,IV,MAXMSGBLOCK,Msg + i *MAXMSGBLOCK,\
					encrypData + i*MAXMSGBLOCK,&p_len,sizeOfEncryptedData);
			if ( ret < 0  )
			{
				UnionUserErrLog("in UnionHsmCmdE0 : RacalCmdE0_f error !!\n");
				return ( ret );
			}
			*encryDataLen += p_len;
		}

		lastDataLen = MsgLen % MAXMSGBLOCK;
		if ( lastDataLen != 0 )
		{
			ret = UnionHsmCmdE0_f(crytoFlag,blockFlag,encrypMode,zekLen,\
					Zek,datatype,IV,lastDataLen,Msg + i *MAXMSGBLOCK,\
					encrypData + i*MAXMSGBLOCK,&p_len,sizeOfEncryptedData);

			if ( ret < 0  )
			{
				UnionUserErrLog("in UnionHsmCmdE0 : UnionHsmCmdE0_f error !!\n");
				return ( ret );
			}

			*encryDataLen += p_len;
		}
	}
	return *encryDataLen;
}


int UnionHsmCmdE0_f(int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData)
{
	int     offset,ret;
	char    iMsg_Len[32];
	char    hsmCmd[32];
	char	hsmCmdBuf[MAXMSGBLOCK+ 256];
	char    v_zek[64];

	strcpy(hsmCmd,"E0");

	if( blockFlag < 0 ||  blockFlag > 3 )
	{
		UnionUserErrLog("in UnionHsmCmdE0_f:: parameter error! blockFlag[%d]\n",blockFlag);
		return(errCodeParameter);
	}
	offset = 2;
	if( encrypMode < 1 ||encrypMode > 5 )
	{
		UnionUserErrLog("in UnionHsmCmdE0_f:: parameter  encrypMode=[%d]error!\n",encrypMode);
		return(errCodeParameter);
	}

	// add by leipp 20150116
	if (crytoFlag == 0 || crytoFlag == 2 || crytoFlag == 4)
		UnionSetMaskPrintTypeForHSMReqCmd();
	else
		UnionSetMaskPrintTypeForHSMResCmd();
	// end

	sprintf(hsmCmdBuf ,"%s%.1d%.1d%.1d%.1d",hsmCmd,blockFlag,crytoFlag,encrypMode,0);
	offset += 4;

	ret = UnionPutKeyIntoRacalKeyString(Zek,v_zek,sizeof(v_zek));
	if ( ret < 0 )
	{
		UnionUserErrLog("in UnionHsmCmdE0_f:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	memcpy(hsmCmdBuf+ offset,v_zek,ret);
	offset += ret;

	if(datatype==0)
		memcpy(hsmCmdBuf+offset,"00",2);/***Binary****/
	else if(datatype==1)
		memcpy(hsmCmdBuf+offset,"11",2);/****expanded Hex****/
	else
	{
		UnionUserErrLog("in UnionHsmCmdE0_f:: parameter  datatype=[%d]error!\n",datatype);
		return(errCodeParameter);
	}

	offset += 2;
	if (blockFlag == 0 || blockFlag == 3)
	{
		memcpy(hsmCmdBuf+offset,"000000",6);
		offset += 6;
	}

	if( encrypMode > 1 )
	{
		memcpy(hsmCmdBuf+offset, IV, 16);
		offset += 16;
	}

	if( datatype == 1 )
		sprintf(iMsg_Len,"%03X",MsgLen/2);
	else
		sprintf(iMsg_Len,"%03X",MsgLen);

	memcpy(hsmCmdBuf+offset,iMsg_Len,3);
	offset += 3;
	memcpy(hsmCmdBuf+offset,Msg,MsgLen);
	offset+= MsgLen;
	hsmCmdBuf[offset] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdE0_f:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(iMsg_Len,hsmCmdBuf + 4 + 1,3);
	iMsg_Len[3] = 0;

	*encryDataLen = UnionOxToD(iMsg_Len);
	if ( datatype == 1 )
		*encryDataLen *= 2;

	if ( encrypData != NULL && ret-8 <= sizeOfEncryptedData )
	{
		memcpy(encrypData,hsmCmdBuf + 8,*encryDataLen);
		encrypData[*encryDataLen] = 0;
	}
	else 
		return(errCodeSmallBuffer);

	return *encryDataLen;
}



/* Y4ָ��,����:������Կ.ʹ�ø���Կ����ɢ����1������ɢ,�õ���ɢ��Կ;ʹ�ñ�����Կ(ZMK)���������LMK06-07�Լ������.
   �������:
   kek��������ԿKEK
   rootKey��Ӧ������Կ
   keyType����Կ���ͣ�001-ZPK��008-ZAK��
   discreteNum����ɢ����
   discreteData1����ɢ����1
   discreteData2����ɢ����2
   discreteData3����ɢ����3
   �������:
   keyByKek��ʹ��KEK���ܵ�key
checkValue:У��ֵ

 */

int UnionHsmCmdY4 (char *kek, char *rootKey, char *keyType, int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char *keyByKek, char *keyByLmk, char *checkValue)
{
	char hsmCmdBuf[2048];
	int hsmCmdLen = 0;
	int ret;
	if ((kek == NULL) || (rootKey == NULL) || (discreteNum <= 0) || (discreteNum > 3) || (discreteData1 == NULL) || (keyByKek == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdY4:: parameter error!\n");
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "Y4", 2);      /*�������*/
	hsmCmdLen = 2;

	if ((ret = UnionPutKeyIntoRacalKeyString(kek, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*������Կ*/
	{
		UnionUserErrLog("in UnionHsmCmdY4:: kek UnionPutKeyIntoRacalKeyString [%s]!\n", kek);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(rootKey, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*����Կ*/
	{
		UnionUserErrLog("in UnionHsmCmdY4:: rootKey UnionPutKeyIntoRacalKeyString [%s]!\n", rootKey);
		return(ret);
	}
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", discreteNum); /*��ɢ����*/
	hsmCmdLen += 1;
	/*��ɢ����*/
	memcpy(hsmCmdBuf + hsmCmdLen, discreteData1, 16);
	hsmCmdLen += 16;
	if (discreteNum >= 2)
	{
		if (discreteData2 == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdY4:: parameter error!\n");
			return (errCodeParameter);
		}
		memcpy(hsmCmdBuf + hsmCmdLen, discreteData2, 16);
		hsmCmdLen += 16;
		if (discreteNum == 3)
		{
			if (discreteData3 == NULL)
			{
				UnionUserErrLog("in UnionHsmCmdY4:: parameter error!\n");
				return (errCodeParameter);
			}
			memcpy(hsmCmdBuf + hsmCmdLen, discreteData3, 16);
			hsmCmdLen += 16;
		}
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	/*�����������������*/
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY4:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(keyByLmk, hsmCmdBuf + 4, 32);  /*����keyByLmk*/
	memcpy(keyByKek, hsmCmdBuf + 36, 32);  /*����keyByKek*/
	memcpy(checkValue,hsmCmdBuf+36+32,16);

	return (0);
}



/*
   �������ܣ�
   A5ָ�������Կ���ĵļ����ɷݺϳ����յ���Կ��ͬʱ����Ӧ��LMK��Կ�Լ���
   ���������
   keyType����Կ������
   keyLength����Կ�ĳ���
   partKeyNum����Կ�ɷֵ�����
   partKey����Ÿ�����Կ�ɷֵ����黺��
   ���������
   keyByLMK���ϳɵ���Կ���ģ���LMK����
   checkValue���ϳɵ���Կ��У��ֵ

 */
int UnionHsmCmdA5(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;
	int	i;

	if ((keyByLMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdA5:: wrong parameters!\n");
		return(errCodeParameter);
	}
	if (partKeyNum < 2 || partKeyNum > 9)
	{
		UnionUserErrLog("in UnionHsmCmdA5:: partKeyNum [%d] must between 2 and 9!\n",partKeyNum);
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"A5",2);
	hsmCmdLen = 2;
	// ��Կ�ɷ�����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// ��Կ���ʹ���
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA5:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// ��Կ���ȱ�־
	if (SM4Mode == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
	}
	else
	{
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdA5:: UnionTranslateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
	}

	for (i = 0; i < partKeyNum; i++)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,partKey[i],strlen(partKey[i]));
		hsmCmdLen += strlen(partKey[i]);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// add by leipp 20150731
	UnionSetMaskPrintTypeForHSMReqCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;			// add by leipp 20150731

	switch (keyLength)
	{
		case    con64BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			keyByLMK[16] = 0;
			// �ϳɵ���Կ��У��ֵ
			//memcpy(checkValue,hsmCmdBuf+4+16,6);
			ret = sprintf(checkValue,"%s",hsmCmdBuf+4+16);		// modify by leipp 20150731 Ŀǰ�������ʷ���У��ֵΪ16
			checkValue[ret] = 0;
			return(16+ret);
		case    con128BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			keyByLMK[32] = 0;
			// �ϳɵ���Կ��У��ֵ
			if (SM4Mode == 1)
			{
				memcpy(checkValue,hsmCmdBuf+4+1+32,16);
				return(32+16);
			}
			else
			{
				//memcpy(checkValue,hsmCmdBuf+4+1+32,6);
				ret = sprintf(checkValue,"%s",hsmCmdBuf+4+1+32); // modify by leipp 20150731 Ŀǰ�������ʷ���У��ֵΪ16
				checkValue[ret] = 0;
				return(32+ret);
			}
		case    con192BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			keyByLMK[48] = 0;
			// �ϳɵ���Կ��У��ֵ
			//memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			ret = sprintf(checkValue,"%s",hsmCmdBuf+4+1+48);	// modify by leipp 20150731 Ŀǰ�������ʷ���У��ֵΪ16
			checkValue[ret] = 0;
			return(48+ret);
		default:
			UnionUserErrLog("in UnionHsmCmdA5:: unknown key length!\n");
			return(errCodeParameter);
	}
}

/*
   �������ܣ�
   N4ָ�� ʹ����ɢ��Կת����PIN
   ���������
importProgram: ���뷽��		0-��ʹ�ù�����Կ��������
1-ʹ�ù�����Կ(ͬ��ɢ�㷨)
2-ʹ�ù�����Կ����׼�㷨��
3��ʹ�ù�����Կ����Կ���Ҳ��ֱַ���DES��
importKeyFlag: ������Կ��־	0 �C ʹ�ü��ܻ��ڲ���Կ
1 �C ʹ��LMK��������Կ
importKey:	  ���ܻ��ڲ�������Կ	1A+3H	����Կ��־Ϊ0�����и���
1A+3H��ʾʹ��K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ��
mkDvsNum1:      ��ɢ����		ָ���Ը���Կ��ɢ�Ĵ�����0-3�Σ�
DvsData1:       ��ɢ����	n*16H	��ɢ��Ƭ��Կ�����ݣ�����n������ɢ����
ProcessData1:   ��������	N*16H	���ڲ���������Կ�����ݣ���������IDΪ1,2,3ʱ�д���
KeyLengthFlag1:������Կ���ȱ�ʶ	1N	����Կ��־Ϊ1�����и���  ��Կ���ȣ�0=��������DES��Կ
1=˫������DES��Կ����ʱֻ֧��˫������Կ��
2=��������DES��Կ
importKeyByLMK:	  LMK�����ĵ�����Կ	16H/32H/48H	����Կ��־Ϊ1�����и�����Կ����
exportProgram: ��������	1N	0-��ʹ�ù�����Կ��������
1-ʹ�ù�����Կ(ͬ��ɢ�㷨)
2-ʹ�ù�����Կ����׼�㷨��
3��ʹ�ù�����Կ����Կ���Ҳ��ֱַ���DES��
exportKeyFlag: ������Կ��־	1N	0 �C ʹ�ü��ܻ��ڲ���Կ
1 �C ʹ��LMK��������Կ
exportKey:	  ���ܻ��ڲ�������Կ	1A+3H	����Կ��־Ϊ0�����и���
1A+3H��ʾʹ��K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ��
mkDvsNum2:   ��ɢ����	1N	ָ���Ը���Կ��ɢ�Ĵ�����0-3�Σ�
DvsData2:	  ��ɢ����	n*16H	��ɢ��Ƭ��Կ�����ݣ�����n������ɢ����
ProcessData2:��������	N*16H	���ڲ���������Կ�����ݣ���������IDΪ1,2,3ʱ�д���
KeyLengthFlag2:������Կ���ȱ�ʶ	1N	����Կ��־Ϊ1�����и��� ��Կ���ȣ�0=��������DES��Կ
1=˫������DES��Կ����ʱֻ֧��˫������Կ��
2=��������DES��Կ
exportKeyByLMK:	LMK�����ĵ�����Կ	16H/32H/48H	����Կ��־Ϊ1�����и��� ��Կ����
maxPasswordLength:	������볤��	2N	��12��
accNo1:			ԭ�ʺ�	12N	�˺���ȥ��У��λ������12λ��ԭPINBLOCK�����ʺ�
accNo2:			Ŀ���ʺ�	12N	�˺���ȥ��У��λ������12λ��Ŀ��PINBLOCK�����ʺ�
format1:		ԴPIN���ʽ	2N	PIN��ĸ�ʽ���롣��01��ΪANSI X9.8��ʽ��Ŀǰ����Ҫ֧�֡�01������03��,��13������14����ʽ��
format2:		Ŀ��PIN���ʽ	2N	PIN��ĸ�ʽ���롣��01��ΪANSI X9.8��ʽ��Ŀǰ����Ҫ֧�֡�01������03��,��13������15������16����ʽ��
accNo:			Ŀ��ȫ�ʺ�	16N	����Ŀ�ĸ�ʽΪ16ʱ�д���
pinBlockByZPK:		ԴPINBLOCK	16H	ԴZPK�¼��ܵ�ԴPIN�顣


���������
pinBlock:		Ŀ��PIN 	16H	Ŀ��ZPK��Ŀ���ʺż��ܵ�PIN����
 */
int UnionHsmCmdN4(int importProgram, int importKeyFlag, char *importKey, int mkDvsNum1, char *DvsData1, char *ProcessData1, 
		int KeyLengthFlag1, char *importKeyByLMK, int exportProgram, int exportKeyFlag, char *exportKey, int mkDvsNum2, 
		char *DvsData2, char *ProcessData2, int KeyLengthFlag2,char *exportKeyByLMK, int maxPasswordLength, char *accNo1, char *accNo2, 
		int format1, int format2, char *accNo, char *pinBlockByZPK, char *pinBlock)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;

	// �������
	memcpy(hsmCmdBuf,"N4",2);
	hsmCmdLen += 2;

	// ���뷽��
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",importProgram);
	hsmCmdLen += 1;

	// ������Կ��־
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",importKeyFlag);
	hsmCmdLen += 1;

	// ���ܻ��ڲ�������Կ
	if (importKeyFlag == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, importKey, 4);
		hsmCmdLen += 4;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mkDvsNum1);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, DvsData1, mkDvsNum1*16);
	hsmCmdLen += mkDvsNum1*16;

	// ��������
	if (importProgram == 1 || importProgram == 2 || importProgram == 3)
	{
		// modify by chenqy 20151227 mkDvsNum1*16 �ĳ� 16
		memcpy(hsmCmdBuf+hsmCmdLen, ProcessData1, 16);
		hsmCmdLen += 16;
		// modify end
	}

	if (importKeyFlag == 1)
	{
		// ������Կ���ȱ�ʶ
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",KeyLengthFlag1);
		hsmCmdLen += 1;

		// LMK�����ĵ�����Կ
		memcpy(hsmCmdBuf+hsmCmdLen, importKeyByLMK, strlen(importKeyByLMK));
		hsmCmdLen += strlen(importKeyByLMK);
	}

	// ��������
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",exportProgram);
	hsmCmdLen += 1;

	// ������Կ��־
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",exportKeyFlag);
	hsmCmdLen += 1;

	// ���ܻ��ڲ�������Կ
	if (exportKeyFlag == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, exportKey, 4);
		hsmCmdLen += 4;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mkDvsNum2);
	hsmCmdLen += 1;

	if (mkDvsNum2 != 0)
	{	
		// ��ɢ����
		memcpy(hsmCmdBuf+hsmCmdLen, DvsData2, mkDvsNum2*16);
		hsmCmdLen += mkDvsNum2*16; // modify by chenqy 20151225 "mkDvsNum1 �ĳ� mkDvsNum2"
	}

	// ��������
	if (exportProgram == 1 || exportProgram == 2 || exportProgram == 3)
	{
		// modify by chenqy 20151227 mkDvsNum2*16 �ĳ� 16
		memcpy(hsmCmdBuf+hsmCmdLen, ProcessData2, 16);
		hsmCmdLen += 16;
		// modify end
	}

	if (exportKeyFlag == 1)
	{
		// ������Կ���ȱ�ʶ
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",KeyLengthFlag2);
		hsmCmdLen += 1;

		// LMK�����ĵ�����Կ
		memcpy(hsmCmdBuf+hsmCmdLen, exportKeyByLMK, strlen(exportKeyByLMK));
		hsmCmdLen += strlen(exportKeyByLMK);
	}

	// ������볤��
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",maxPasswordLength);
	hsmCmdLen += 2;

	// Դ�ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen, accNo1, 12);
	hsmCmdLen += 12;

	// Ŀ���ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen, accNo2, 12);
	hsmCmdLen += 12;

	// ԴPIN���ʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",format1);
	hsmCmdLen += 2;

	// Ŀ��PIN���ʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",format2);
	hsmCmdLen += 2;

	// Ŀ��ȫ�ʺ�	
	if (format2 == 16)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, accNo, 16);
		hsmCmdLen += 16;
	}

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, pinBlockByZPK, 16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdN4:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(pinBlock, hsmCmdBuf+offset, 16);
	pinBlock[16] = 0;
	offset += 16;

	return(0);
}


/*
   �������ܣ�
   18ָ�� ʹ����ɢ��Կ�ӽ������ݻ����MAC
   ���������
keyIndex: ��Կ���� 3H:  001-57F����֧�֣�
K+3H: K001-K57F ��������
T109(����)
ע��SJL06��֧��T109��ʽ��������Կ��ʽ�ɲ���֧��

withKey: �����Կ ������һ����ΪTXXXʱ�д���
mkDvsNum: ��ɢ���� ָ���Ը���Կ��ɢ�Ĵ�����0-2�Σ�
DvsData: ��ɢ���� n*16H ��ɢ��Ƭ��Կ�����ݣ�����n������ɢ����
proKeyAlgorithmFlag: ������Կ�㷨��־			0����׼�㷨��3DES(Kc),64bit��
1��DES(Kcl+Kcr,128bit)
2��AES(Kc,128bit)
3��3DES��Kc��128bit��
4�������������Կ ��֧����ɽũ��������
GC_Data: ���ڲ���������Կ�����ݣ�AES�㷨Ҫ���������Ϊ128bit��
����������Կ�㷨��־��Ϊ4ʱ���޴���
encryptFlag: ���ܱ�־ 0������/����
1������
2������
encryptAlgorithmSelect: �����㷨ѡ�� 0����׼�㷨(1-DES-CBC(Kg))
1��3-DES-CBC(Kg)
2��AES-ECB(Kg)
3��1-DES-ECB(Kgl)
4�����-ECB(Kg)
(���ܱ�־Ϊ1��2 ʱ����)
macFlag:  MAC ��־ 0������MAC
1����ԭ������MAC
2���Դ����ı�����MAC
macAlgorithmSelect: MAC �㷨ѡ�� 0����׼�㷨��1-DES-CBC(Kg)��
1��1-DES-CBC(Kgl)
2��1-DES-CBC-Lite(Kgl)
3��SHA1(Kg)
4�����MAC����ɽũ��������
(MAC ��־Ϊ1��2 ʱ����)
lengthOfMessage: ���ĳ��� ���ĵ��ֽ���
Message: ���� ����N

���������
mac: MACֵ ����MAC ��־Ϊ1/2 ʱ�д���
criperMessage: �������� �������ܱ�־Ϊ1/2 ʱ�д��򳤶�N
 */
int UnionHsmCmd18(char *keyIndex, char *withKey, int mkDvsNum, char *DvsData, int proKeyAlgorithmFlag,
		char *GC_Data, int encryptFlag, int encryptAlgorithmSelect, int macFlag, int macAlgorithmSelect, 
		int lengthOfMessage, char *Message, char *mac, char *criperMessage)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;

	if ((keyIndex == NULL) || (DvsData == NULL) || (Message == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd18:: parameters err!\n");
		return(errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf,"18",2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, keyIndex, strlen(keyIndex));
	hsmCmdLen += strlen(keyIndex);

	// �����Կ
	if (keyIndex[0] == 'T')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, withKey, 32);
		hsmCmdLen += 32;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, DvsData, (mkDvsNum * 16));
	hsmCmdLen += (mkDvsNum * 16);

	// ������Կ�㷨��־
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", proKeyAlgorithmFlag);
	hsmCmdLen += 1;

	// ���ڲ���������Կ������
	if (proKeyAlgorithmFlag != 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, GC_Data, strlen(GC_Data));
		hsmCmdLen += strlen(GC_Data);
	}

	// ���ܱ�־
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", encryptFlag);
	hsmCmdLen += 1;

	// �����㷨ѡ��
	if (encryptFlag == 1 || encryptFlag == 2)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", encryptAlgorithmSelect);
		hsmCmdLen += 1;
	}

	// MAC ��־
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", macFlag);
	hsmCmdLen += 1;

	// MAC �㷨ѡ��
	if (macFlag == 1 || macFlag == 2)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", macAlgorithmSelect);
		hsmCmdLen += 1;
	}

	// ���IV��ʼ����
	if (encryptAlgorithmSelect == 0 || encryptAlgorithmSelect == 1)
	{
		memmove(Message+16,Message,lengthOfMessage);
		sprintf(Message,"%016d%s",0,Message+16);
		lengthOfMessage += 16;
	}

	// ���ĳ���
	sprintf(hsmCmdBuf+hsmCmdLen, "%03d", lengthOfMessage/2);
	hsmCmdLen += 3;

	// ��������
	memcpy(hsmCmdBuf+hsmCmdLen, Message, lengthOfMessage);
	hsmCmdLen += lengthOfMessage;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd18:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;

	if (macFlag == 1 || macFlag == 2)
	{
		memcpy(mac, hsmCmdBuf+offset, 8);
		mac[8] = 0;
		offset += 8;
	}

	if (encryptFlag == 1 || encryptFlag == 2)
	{
		memcpy(criperMessage, hsmCmdBuf+offset, strlen(hsmCmdBuf)-offset);
		offset += strlen(hsmCmdBuf)-offset;
	}

	return(0);
}


/*
   �������ܣ�
   U2ָ�ʹ��ָ����Ӧ������Կ����2����ɢ�õ���ƬӦ������Կ��
   ʹ��ָ��������Կ����Կ���м��ܱ������������MAC���㡣
   ���������
securityMech: ��ȫ����(S����DES���ܺ�MAC, T����DES���ܺ�MAC)
mode: ģʽ��־, 0-������ 1-���ܲ�����MAC
id: ����ID, 0=M/Chip4(CBCģʽ��ǿ�����X80) 1=VISA/PBOC(������ָ����ECB) 2=PBOC1.0ģʽ(ECBģʽ������������)

mk: ����Կ
mkType: ����Կ����109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
��ʹ�ü��ܻ�����Կʱ����������
mkIndex: ����Կ����, K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
mkDvsNum: ����Կ��ɢ����, 1-3��
mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

pkType: ������Կ���� 0=TK(������Կ)1=DK-SMC(ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ)
pk: ������Կ
pkIndex: ������Կ����
pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������

proKeyFlag:������Կ��ʶ��Y:���������Կ��N:�����������Կ����ѡ��:��û�и���ʱȱʡΪN
proFactor: ��������(16H),��ѡ��:����������Կ��־ΪYʱ��

ivCbc: IV-CBC,8H ����������ID��Ϊ0ʱ��

encryptFillDataLen: ����������ݳ���(4H),����������ID��Ϊ2ʱ�У�������1024��
����Կ����һ����м��ܵ����ݳ���
encryptFillData: ����������� nB ����������ID��Ϊ2ʱ��,����Կ����һ����м���
encryptFillOffset: �����������ƫ���� 4H ����������ID��Ϊ2ʱ��
����Կ���Ĳ��뵽����������ݵ�λ��, ��ֵ������0������������ݳ���֮��

ivMac: IV-MAC,16H ������ģʽ��־��Ϊ1ʱ��
macDataLen: MAC������ݳ��� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC��������ݳ���
macData: MAC������� nB ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC���������
macOffset: ƫ���� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ���Ĳ��뵽MAC������ݵ�λ��
��ֵ������0��MAC������ݳ���֮��
���������
mac: MACֵ 8B ������ģʽ��־��Ϊ1ʱ��
criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
criperData: nB �������������
checkValue: У��ֵ

 */
int UnionHsmCmdU2(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex,
		int pkDvsNum, char *pkDvsData, char *proKeyFlag, char *proFactor, char *ivCbc,
		int encryptFillDataLen, char *encryptFillData, int encryptFillOffset,
		char *ivMac, int macDataLen, char *macData, int macOffset,
		char *mac, int *criperDataLen, char *criperData, char *checkValue)
{
	int     ret;
	int     lenOfDvsData = 16;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    szMkBuff[64];
	char    szPkBuff[64];
	// �������
	memcpy(hsmCmdBuf,"U2",2);
	hsmCmdLen += 2;

	// ��ȫ����
	memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
	hsmCmdLen += 1;
	if ((securityMech[0] == 'Q') || (securityMech[0] == 'U'))
		lenOfDvsData = 32;
	else
		lenOfDvsData = 16;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
		memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
		hsmCmdLen += ret;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 1);
	hsmCmdLen += 1;

	// ������Կ
	if ((pk == NULL) || (strlen(pk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", pkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(pk) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, pk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(pk), pk, szPkBuff);
		memcpy(hsmCmdBuf+hsmCmdLen, szPkBuff, ret);
		hsmCmdLen += ret;
	}

	// ������Կ��ɢ����, ������Կ��ɢ����
	if (pkType[0] == '1')
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
		hsmCmdLen += pkDvsNum*lenOfDvsData;
	}

	// modify by lix,20090730
	// ������Կ��ʶ
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// ��������
		if (proKeyFlag[0] == 'Y')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, proFactor, lenOfDvsData);
			hsmCmdLen += lenOfDvsData;
		}
	}
	// modify end;

	// IV-CBC
	if (id[0] == '0')
	{
		//lizh��20140328�޸ģ��޸�����Ϊ��ivCbc�ɸ���8����16��
		//	memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 8);
		//	hsmCmdLen += 8;
		memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 16);
		hsmCmdLen += 16;
	}

	// ����������ݳ��ȡ�����������ݡ������������ƫ����
	if (id[0] == '2')
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, encryptFillData, encryptFillDataLen);
		hsmCmdLen += encryptFillDataLen;

		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillOffset/2);
		hsmCmdLen += 4;
	}

	// IV-MAC, MAC������ݳ���, MAC�������, ƫ����
	if (mode[0] == '1')
	{
		if (strlen(ivMac) > 0)
		{
			if (securityMech[0] == 'W' || securityMech[0] == 'U')
			{
				memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 32);
				hsmCmdLen += 32;
			}
			else
			{
				memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 16);
				hsmCmdLen += 16;
			}
		}

		if (macDataLen > 8000)
		{
			UnionUserErrLog("in UnionHsmCmdU2::macDataLen[%d] too large\n", macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, macData, macDataLen);
		hsmCmdLen += macDataLen;

		if ((macOffset < 0) || (macOffset > macDataLen))
		{
			UnionUserErrLog("in UnionHsmCmdU2::macOffset[%d] <0 or >macDataLen[%d]\n", macOffset, macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macOffset/2);
		hsmCmdLen += 4;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU2:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (mode[0] == '1') // MACֵ
	{
		if (securityMech[0] == 'U')
		{
			memcpy(mac, hsmCmdBuf+offset, 32);
			mac[32] = 0;
			offset += 32;
		}
		else
		{
			memcpy(mac, hsmCmdBuf+offset, 16);
			mac[16] = 0;
			offset += 16;
		}
	}
	else if (mode[0] == '2') // ģʽ2ʱ,����У��ֵ
	{
		memcpy(checkValue, hsmCmdBuf + ret - 16, 16);
		checkValue[16] = 0;
		offset += 16;
	}


	// �������ݳ���
	offset += 4;
	*criperDataLen = ret - offset;

	//edit by chenwd 20140719
	// ��������
	// memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
	// UnionLog("in UnionHsmCmdU2::*criperDataLen=[%d]\n",*criperDataLen);

	if (mode[0] == '1')
	{
		if (securityMech[0] == 'U')
		{
			// ��������
			memcpy(criperData, hsmCmdBuf + 40, *criperDataLen);
			criperData[*criperDataLen] = 0;
		}
		else
		{
			// ��������
			memcpy(criperData, hsmCmdBuf + 24, *criperDataLen);
			criperData[*criperDataLen] = 0;
		}
	}
	else
	{
		// ��������
		memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
		criperData[*criperDataLen] = 0;
	}

	//edit end by chenwd 20140719

	return(0);
}


int UnionHsmCmdKW(char *mode, char *id, int mkIndex, char *mk, char *iv, int divNum, char *divData,
		int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,
		char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,
		char *data1,char *ARPC)
{
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	char keyString[64];
	int offset = 0;
	char    ARPCBuf[64];
	char	tmpPan[32];

	// �������
	memcpy(hsmCmdBuf,"KW",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk),mk,keyString);
		keyString[ret] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen, keyString, ret);
		hsmCmdLen += ret;
	}

	// IV-AC
	if ( (id[0]=='0') || (id[0]=='1') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	// add by lisq 20150120 ΢�����пͻ�����չ
	if (id[0] == '7')
	{
		// divNum
		if (divNum < 1 || divNum > 5)
		{
			UnionUserErrLog("in UnionHsmCmdKW::divNum [%d] must between 1 and 5!\n", divNum);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", divNum);
		hsmCmdLen++;

		// divData
		ret = aschex_to_bcdhex(divData, strlen(divData), hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += ret;
	}	
	// add by lisq 20150120 end ΢�����пͻ�����չ

	// PAN����
	if ( id[0]=='1' )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", lenOfPan/2);
		hsmCmdLen += 2;
	}

	// PAN
	if ( id[0]=='1' )
	{
		ret = aschex_to_bcdhex(pan, lenOfPan, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += ret;
	}
	else
	{
		if (id[0] != '7')
		{
			UnionForm16BytesDisperseDataOfV41A(strlen(pan), pan, tmpPan);
			ret = aschex_to_bcdhex(tmpPan, 16, hsmCmdBuf+hsmCmdLen);
			hsmCmdLen += ret;
		}
	}

	if ( id[0]=='1' )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
		hsmCmdLen += 1;
	}

	// B/H
	if ( (id[0]=='0') || (id[0]=='1') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, bh, 1);
		hsmCmdLen += 1;
	}

	// ATC
	ret = aschex_to_bcdhex(atc, 4, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += ret;

	// �������ݳ���
	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6') )
	{
		// modified 2011-11-08
		//sprintf(hsmCmdBuf+hsmCmdLen, "%2x", lenOfData/2);
		sprintf(hsmCmdBuf+hsmCmdLen, "%02X", lenOfData/2);
		hsmCmdLen += 2;
	}

	// ��������
	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6')  )
	{
		ret = aschex_to_bcdhex(data, lenOfData, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += ret;
	}

	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6')  )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
		hsmCmdLen += 1;
	}

	// ARQC
	ret = aschex_to_bcdhex(ARQC, 16, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += ret;

	// ARC
	if ( (mode[0]=='1') || (mode[0]=='2') || (mode[0]=='6') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ARC, 2);
		hsmCmdLen += 2;
	}

	// CSU
	if ( (mode[0]=='3') || (mode[0]=='4') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, csu, 4);
		hsmCmdLen += 4;
	}

	// lenOfData1
	if ( (mode[0]=='3') || (mode[0]=='4') )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04d", lenOfData1);
		hsmCmdLen += 4;
	}

	// data1
	if ( (mode[0]=='3') || (mode[0]=='4') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, data1, lenOfData1);
		hsmCmdLen += lenOfData1;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKW:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if ((mode[0] == '1')||(mode[0] == '2')||(mode[0] == '3')||(mode[0] == '4')) // ARPC
	{
		memcpy(ARPCBuf,hsmCmdBuf + offset, 8);
		ARPCBuf[8] = 0;
		bcdhex_to_aschex(ARPCBuf, 8, ARPC);
		return 16;
	}

	return(0);
}


// added 2011-11-08 �������ã���09KWָ����ͬ
int UnionHsmCmdKX(char *mode, char *id, int mkIndex, char *mk, char *iv,
		int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,
		char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,
		char *data1,char *ARPC)
{
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	char keyString[64];
	int offset = 0;
	char    ARPCBuf[64];

	// �������
	memcpy(hsmCmdBuf,"KX",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk),mk,keyString);
		memcpy(hsmCmdBuf+hsmCmdLen, keyString, ret);
		hsmCmdLen += ret;
	}

	// IV-AC
	if ( (id[0]=='0') || (id[0]=='1') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	// PAN����
	if ( id[0]=='1' )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", lenOfPan/2);
		hsmCmdLen += 2;
	}

	// PAN
	if ( id[0]=='1' )
	{
		aschex_to_bcdhex(pan, lenOfPan, pan);
		memcpy(hsmCmdBuf+hsmCmdLen, pan, lenOfPan/2);
		hsmCmdLen += lenOfPan/2;
	}
	else
	{
		UnionForm16BytesDisperseDataOfV41A(strlen(pan), pan, pan);
		aschex_to_bcdhex(pan, 16, pan);
		memcpy(hsmCmdBuf+hsmCmdLen, pan, 8);
		hsmCmdLen += 8;
	}

	if ( id[0]=='1' )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
		hsmCmdLen += 1;
	}

	// B/H
	if ( (id[0]=='0') || (id[0]=='1') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, bh, 1);
		hsmCmdLen += 1;
	}

	// ATC
	aschex_to_bcdhex(atc, 4, atc);
	memcpy(hsmCmdBuf+hsmCmdLen, atc, 2);
	hsmCmdLen += 2;

	// �������ݳ���
	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6') )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02X", lenOfData/2);
		hsmCmdLen += 2;
	}

	// ��������
	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6')  )
	{
		aschex_to_bcdhex(data, lenOfData, data);
		memcpy(hsmCmdBuf+hsmCmdLen, data, lenOfData/2);
		hsmCmdLen += lenOfData/2;
	}

	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6')  )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
		hsmCmdLen += 1;
	}

	// ARQC
	aschex_to_bcdhex(ARQC, 16, ARQC);
	memcpy(hsmCmdBuf+hsmCmdLen, ARQC, 8);
	hsmCmdLen += 8;

	// ARC
	if ( (mode[0]=='1') || (mode[0]=='2') || (mode[0]=='6') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ARC, 2);
		hsmCmdLen += 2;
	}

	// CSU
	if ( (mode[0]=='3') || (mode[0]=='4') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, csu, 4);
		hsmCmdLen += 4;
	}

	// lenOfData1
	if ( (mode[0]=='3') || (mode[0]=='4') )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04d", lenOfData1);
		hsmCmdLen += 4;
	}

	// data1
	if ( (mode[0]=='3') || (mode[0]=='4') )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, data1, lenOfData1);
		hsmCmdLen += lenOfData1;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();	

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKX:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if ((mode[0] == '1')||(mode[0] == '2')||(mode[0] == '3')||(mode[0] == '4')||(mode[0] == '6')) // ARPC
	{
		memcpy(ARPCBuf,hsmCmdBuf + offset, 8);
		ARPCBuf[8] = 0;
		bcdhex_to_aschex(ARPCBuf, 8, ARPC);
		return 16;
	}

	return(0);
}


int UnionHsmCmdU0(char *mode, char *id, int mkIndex, char *mk, char *iv,
		char *pan, char *bh, char *atc, int lenOfPlainData,
		char *plainData, int *lenOfCiperData, char *ciperData)
{
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	char tmpBuf[32];
	char keyString[64];
	int offset = 0;

	// �������
	memcpy(hsmCmdBuf,"U0",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk),mk,keyString);
		keyString[ret] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen, keyString, ret);
		hsmCmdLen += ret;
	}

	// IV-AC
	if ( id[0]=='0' )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	// PAN
	UnionForm16BytesDisperseDataOfV41A(strlen(pan), pan, pan);
	aschex_to_bcdhex(pan, 16, pan);
	memcpy(hsmCmdBuf+hsmCmdLen, pan, 8);
	hsmCmdLen += 8;

	// B/H
	if (id[0]=='0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, bh, 1);
		hsmCmdLen += 1;
	}

	// ATC
	aschex_to_bcdhex(atc, 4, atc);
	memcpy(hsmCmdBuf+hsmCmdLen, atc, 2);
	hsmCmdLen += 2;

	// ������������
	aschex_to_bcdhex(plainData, lenOfPlainData, plainData);
	ret = UnionPBOCEMVFormPlainDataBlock(lenOfPlainData/2,(unsigned char *)plainData,(unsigned char *)plainData);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU0:: UnionPBOCEMVFormPlainDataBlock!\n");
		return(ret);
	}

	// �������ݳ���
	sprintf(hsmCmdBuf+hsmCmdLen, "%04x", ret);
	hsmCmdLen += 4;

	// ��������
	memcpy(hsmCmdBuf+hsmCmdLen, plainData, ret);
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU0:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(tmpBuf, hsmCmdBuf+offset, 4);
	aschex_to_bcdhex(tmpBuf, 4, tmpBuf);
	*lenOfCiperData = tmpBuf[0]*256+tmpBuf[1];
	offset += 4;
	memcpy(ciperData, hsmCmdBuf+offset, *lenOfCiperData);
	return (*lenOfCiperData);
}


/*
   �������ܣ�
   UGָ�����ת��RSA˽Կ��LMK��KEK,LMK������RSA˽ԿΪREF�ṹ,KEKʹ��Mode������㷨����REF�ṹ��RSA˽Կ��ÿ��Ԫ�ء�
   ���������
mode: ����˽Կÿ��Ԫ�صļ����㷨 "00"-DES_ECB, "01"-DES_ECB_LP, "02"-DES_ECB_P,
"10"-DES_CBC, "11"-DES_CBC_LP, "12"-DES_CBC_P
keyType����Կ����
key: ����Կ����HSM�ڰ�ȫ����ʧ�洢������Կ�Լ���LMK���ܵ���������
iv: ��ʼ����, modeΪCBC����ʱ��������ڡ�
vkLength: ˽Կ���ݵ����ݳ���
vk: nB ��LMK������RSA˽Կ��REF�ṹ��
���������
vkByKey: key���ܵ�vk���ݴ�

 */

int UnionHsmCmdUG(char *mode, TUnionDesKeyType keyType,char *key, char *iv,
		int vkLength, char *vk, char *vkByKey)
{
	int     ret;
	int     hsmCmdLen = 0;
	char    lenOfVK[32];
	char    hsmCmdBuf[8192];
	char    tmpBuf[8192];

	UnionSetBCDPrintTypeForHSMCmd();

	if ( (mode == NULL) || (key == NULL) || (vk == NULL) || (vkByKey == NULL) )
	{
		UnionUserErrLog("in UnionHsmCmdUG:: parameters error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "UG", 2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf + hsmCmdLen, mode , 2);
	hsmCmdLen += 2;

	UnionTranslateDesKeyTypeTo3CharFormat(keyType,tmpBuf);
	memcpy(hsmCmdBuf + hsmCmdLen, tmpBuf, 3);
	hsmCmdLen += 3;

	if (strlen(key) == 16)
		hsmCmdBuf[hsmCmdLen] = '0';
	else if(strlen(key) == 32)
		hsmCmdBuf[hsmCmdLen] = '1';
	else if(strlen(key) != 4)
	{
		UnionUserErrLog("in UnionHsmCmdUG:: parameters error!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf + hsmCmdLen, key, strlen(key));
	hsmCmdLen += strlen(key);

	if ( (strncmp(mode, "10", 2) == 0) || (strncmp(mode, "11", 2) == 0) || (strncmp(mode, "12", 2) == 0))
	{
		if (iv == NULL)
			UnionUserErrLog("in UnionHsmCmdUG:: parameters error!\n");
		return(errCodeParameter);
		memcpy(hsmCmdBuf + hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	sprintf(lenOfVK, "%04d", vkLength);
	memcpy(hsmCmdBuf + hsmCmdLen, lenOfVK, 4);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf + hsmCmdLen, vk, vkLength);
	hsmCmdLen += vkLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdUG:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(vkByKey, hsmCmdBuf + 4, ret - 4);
	vkByKey[ret - 4] = 0;

	return ret - 4;
}


/*
   ��������:
   TKָ�˽Կ����
   �������:
encryMode: 1H �㷨��ʶ 1���������� 2���������� 3��ǩ�� 4��������Կ 5��������Կ
vkLength: ����˽Կ����
vk: nB/1A+3H DER�����˽Կ��˽Կ��HSM��ȫ�洢���ڵ�����
keyLength: 1H ����/������Կ���ȣ�0 = ��������Կ��1 = ˫������Կ��encryMode = 4 �� encryMode = 5ʱ�������
inputDataLength: �ӽ������ݳ���
inputData: nB/1A+3H ���ӽ������ݻ���ӽ�����Կ��HSM��ȫ�洢���ڵ�����
�������:
outputData: nB �ӽ���/�ӽ�����Կ����

 */

int UnionHsmCmdTK(char encryMode, int vkLength, char *vk, char keyLength,
		int inputDataLength, char *inputData, char *outputData)
{
	int	ret;
	char	tmpBuf[128];
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;

	if ((vk == NULL) || (inputDataLength <= 0))
	{
		UnionUserErrLog("in UnionHsmCmdTK:: parameters error!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf, "TK", 2);
	hsmCmdLen += 2;

	hsmCmdBuf[hsmCmdLen] = encryMode;
	hsmCmdLen++;

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLength);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf + hsmCmdLen, vk, vkLength);
	hsmCmdLen += vkLength;

	if ((encryMode == '4') || (encryMode == '5'))
	{
		memcpy(hsmCmdBuf + hsmCmdLen, "000", 3);
		hsmCmdLen += 3;
		hsmCmdBuf[hsmCmdLen] = keyLength;
		hsmCmdLen++;
	}

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", inputDataLength);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf + hsmCmdLen, inputData, inputDataLength);
	hsmCmdLen += inputDataLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdTK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(tmpBuf, hsmCmdBuf + 4, 4);
	tmpBuf[4] = 0;
	memcpy(outputData, hsmCmdBuf + 4 + 4, atoi(tmpBuf));

	return(atoi(tmpBuf));
}


/*
   ��������:
   ���������
   �������:
   lenOfRandomData ���������
   �������:
   randomData �����

 */

int UnionHsmCmdTE(int lenOfRandomData, char *randomData)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if ((randomData == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdTE:: parameters err!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "TE", 2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d", lenOfRandomData);
	hsmCmdLen += 4;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdTE:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(randomData, hsmCmdBuf + 4, lenOfRandomData);
	return lenOfRandomData;
}


/*
   ��������:
   ������ɢ/������Կ
   �������:
   mode �㷨��־:
   "00" - EMV2000
   "01" - DES_CBC
   "02" - DES_ECB
   "10" - VISA
   "11" - EMV2000
   masterKeyType	������Կ����
   masterKey	������Կ
   desKeyType	����Կ����
   keyLengthFlag	����ɢ����Կ����	
   0 = ��������Կ
   1 = ˫������Կ
   data		��ɢ����
   masterKey	������Կ
   iv		����
   �������:
   key	����Կ����

 */

int UnionHsmCmdX1(char *mode, TUnionDesKeyType masterKeyType, char *masterKey,
		TUnionDesKeyType desKeyType, char *keyLengthFlag,char *data, char *iv, char *key)
{
	int	ret;
	char	hsmCmdBuf[2048];
	char	keyType[32];
	int	hsmCmdLen = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	if ((mode == NULL) || (data == NULL) || (masterKey == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdX1:: parameters err!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "X1", 2);
	hsmCmdLen = 2;

	// ģʽ
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 2);
	hsmCmdLen += 2;

	// ������Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(masterKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// ����Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(desKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// ��Կ����
	memcpy(hsmCmdBuf + hsmCmdLen, keyLengthFlag, 1);
	hsmCmdLen += 1;

	// ��ɢ���ݳ���
	sprintf(hsmCmdBuf + hsmCmdLen,"%03d", (int)strlen(data)/2);
	hsmCmdLen += 3;

	// ��ɢ����
	aschex_to_bcdhex(data,strlen(data),hsmCmdBuf + hsmCmdLen);	
	hsmCmdLen += strlen(data)/2;

	memcpy(hsmCmdBuf + hsmCmdLen, masterKey, strlen(masterKey));
	hsmCmdLen += strlen(masterKey);

	if ((iv != NULL) && (strlen(iv) != 0))
	{
		memcpy(hsmCmdBuf + hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdX1:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(key, hsmCmdBuf + 4, ret - 4);
	return(ret - 4);
}



/*
   ��������:
   ��һ����Կ���ܵ�����ת��Ϊ��һ����Կ����
   �������:
   srcKeyType			 Դ��Կ������
   srcKey	 1A+3H/16H/32H/48H       Դ��Կ
   srcModel				Դ�����㷨
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4��CBCģʽ��ǿ�����X80��
   "21"=VISA/PBOC��������ָ����ECB��
   dstKeyType			 Ŀ����Կ����
   dstKey	 1A+3H/16H/32H/48H       Ŀ����Կ
   dstModel				Ŀ�ļ����㷨
   "00"=DES_ECB
   "02"=DES_ECB_P
   lenOfSrcCiphertext		 Դ��Կ���ܵ��������ݳ���
   srcCiphertext   nB		 Դ��Կ���ܵ���������
   iv_cbc	 16H		��ʼ������Դ�����㷨ΪCBC����ʱ���������
   �������:
   dstCiphertext   nB		 Ŀ����Կ���ܵ���������

 */
int UnionHsmCmdY1(TUnionDesKeyType srcKeyType,char *srcKey,int srcKeyIdx,char *srcModel,
		TUnionDesKeyType dstKeyType,char *dstKey,int dstKeyIdx,char *dstModel,int lenOfSrcCiphertext,char *srcCiphertext,
		char *iv_cbc,char *dstCiphertext)
{
	int				ret;
	char				hsmCmdBuf[2048];
	int				hsmCmdLen;
	char				tmpBuf[32];
	int				lenOfDstCiphertext;
	char				keyType[32];

	ret		=	  -1;
	lenOfDstCiphertext      =	  0;
	hsmCmdLen	  =	  0;

	if ((srcModel == NULL) || (dstModel == NULL) || (srcCiphertext == NULL) || (dstCiphertext == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdY1:: parameters err!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"Y1",2);
	hsmCmdLen += 2;

	UnionTranslateDesKeyTypeTo3CharFormat(srcKeyType, keyType);
	strncpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	if(srcKey == NULL || strlen(srcKey) <= 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X",srcKeyIdx);
		hsmCmdLen += 4;
	}
	else if(strlen(srcKey) == 4)
	{
		strncpy(hsmCmdBuf+hsmCmdLen, srcKey, 4);
		hsmCmdLen += 4;
	}
	else
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(srcKey,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdY1:: UnionPutKeyIntoRacalKeyString [%s]!\n",srcKey);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	memcpy(hsmCmdBuf+hsmCmdLen,srcModel,2);
	hsmCmdLen += 2;

	UnionTranslateDesKeyTypeTo3CharFormat(dstKeyType, keyType);
	strncpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	if (dstKey == NULL || strlen(dstKey) <= 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X",dstKeyIdx);
		hsmCmdLen += 4;
	}
	else if(strlen(dstKey) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,dstKey,4);
		hsmCmdLen += 4;
	}
	else
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(dstKey,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdY1:: UnionPutKeyIntoRacalKeyString [%s]!\n",dstKey);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	memcpy(hsmCmdBuf+hsmCmdLen,dstModel,2);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfSrcCiphertext/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(srcCiphertext,lenOfSrcCiphertext,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfSrcCiphertext/2;

	if ((strncmp(srcModel,"10",2) == 0) || (strncmp(srcModel,"11",2) == 0)
			|| (strncmp(srcModel,"12",2) == 0) || (strncmp(srcModel,"20",2) == 0))
	{
		if (iv_cbc == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdY1:: parameters err!\n");
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,iv_cbc,16);
		hsmCmdLen += 16;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	/* �������ͨѶ */
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY1:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(tmpBuf,hsmCmdBuf+4,4);
	tmpBuf[4] = 0;
	lenOfDstCiphertext = bcdhex_to_aschex(hsmCmdBuf+8,atoi(tmpBuf),dstCiphertext);

	return(lenOfDstCiphertext);
}


/*
   �������ܣ�
   UDָ������ն˿����������Ա����Ȩ״̬�²�����ʵ�ֶ���Կ�Ļָ��򱸷ݹ��ܡ�
   ���������
   algorithmFlag��	[1A]	�㷨��ʶ����D��-des�㷨����S��-SM4�㷨��Ĭ��Ϊ'D��
modeFlag:       [1N]		ģʽ��ʶ��
1-�ָ���Կ 2-������Կ
mkIndex:	[1A+3H] ��Կλ�ã�      ����������Ƭ��Կ�ķ���������Կ��
ʹ��LMK��28-29��Ӧ���ּ��ܡ�
1A+3H��ʾʹ��K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ��
����ʱ��ʵ��1A+3Hȡ��Կ��
keyLocale       [32H/1A+32H]
key:	[32H]		  ��Կ
����ģʽ��־Ϊ1ʱ�У�LMK04_05����


���������
encryKey:       [N*8B/N*16B/N*24B]      LMK�Լ��ܵ���Կ����(lmk04_05����)
encryKeyLen:			��Կ����
checkValue:     [16H]		  ����Կ����0�Ľ��

 */

int UnionHsmCmdUD(char *algorithmFlag,char *modeFlag,char *mkIndex,char *keyLocale,
		char *key,char *encryKey,int *pencryKeyLen,char *checkValue)
{
	int     ret;
	int     hsmCmdLen = 0;
	int     offset,encryKeyLen;
	char    szMkBuff[128];
	char    hsmCmdBuf[1024];

	// �������
	memcpy(hsmCmdBuf,"UD",2);
	hsmCmdLen += 2;

	UnionSetBCDPrintTypeForHSMCmd();

	// �㷨��ʶ
	if ((algorithmFlag != NULL) && (algorithmFlag[0] != 'D'))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, algorithmFlag, 1);
		hsmCmdLen += 1;
	}

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, modeFlag, 1);
	hsmCmdLen += 1;

	if ((keyLocale == NULL) || (strlen(keyLocale)== 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(keyLocale),keyLocale,szMkBuff);
		szMkBuff[ret] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
		hsmCmdLen += ret;

	}
	if (modeFlag[0] == '1')
	{
		if ((key == NULL) || (strlen(key) != 32))
		{
			UnionUserErrLog("in UnionHsmCmdUD:: key is error!\n");
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",key);
		hsmCmdLen += 32;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdUD:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;

	// ��������
	// encryKeyLen = strlen(hsmCmdBuf) - offset - 16;
	encryKeyLen = ret - offset - 16;
	if(encryKeyLen > 0)
	{
		if ((encryKey == NULL) || (pencryKeyLen == NULL) || (checkValue == NULL))
		{
			UnionUserErrLog("in UnionHsmCmdUD:: encryKey or pencryKeyLen or checkValue is null!\n");
			return(errCodeParameter);
		}

		(*pencryKeyLen) = encryKeyLen;
		memcpy(encryKey,hsmCmdBuf+offset,encryKeyLen);
		encryKey[encryKeyLen] = 0;
		offset += encryKeyLen;
		memcpy(checkValue, hsmCmdBuf+offset, 16);
		checkValue[16] = 0;
	}
	return(0);
}


/*
   �������
   mode ����ģʽ��ʶ 0=��ɢ��Կ3DES����
   1=��ɢ��Կ3DES����
   2=��1DES_MAC����Կ����8�ֽڣ�(ANSI X9.19 MAC)
   3=��3DES_MAC(ANSI X9.19 MAC)
   4=��ɢ��ԿDES����
   5=��ɢ��ԿDES����
   6=������ԿDES����
   7=������ԿDES����
   id ����ID �ӽ����㷨ģʽ��
   01=ECB
   02=CBC
   03=CFB
   04=OFB
   mkType ����Կ���ͣ�������Ƭ��Կ�ķ�������Կ���ͣ�
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   lenOfData ���ݳ���
   data ����
   �������
   criperDataLen ���ݳ���  
   criperData ����   
 */
int UnionHsmCmdU1(char *mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, char *proFactor, int lenOfData,
		char *data, int *criperDataLen, char *criperData)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	char    tmpBuf[32];
	int     offset = 0;
	char    szMkBuff[128];
	char    lenBuf[32];
	int     len;

	// �������
	memcpy(hsmCmdBuf,"U1",2);
	hsmCmdLen += 2;

	// ����ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 2);
	hsmCmdLen += 2;

	// ����Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
		szMkBuff[ret] = 0;
		memcpy(hsmCmdBuf + hsmCmdLen, szMkBuff, ret);
		hsmCmdLen += ret;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	if ('A' == mode[0] || 'B' == mode[0] || 'C' == mode[0] || 'D' == mode[0])
	{
		// ��ɢ����
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*32);
		hsmCmdLen += mkDvsNum*32;
	}
	else
	{
		// ��ɢ����
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
		hsmCmdLen += mkDvsNum*16;
	}

	// ��������
	if ('6' == mode[0] || '7' == mode[0] || '8' == mode[0] || '9' == mode[0] || 'C' == mode[0] || 'D' == mode[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 16);
		hsmCmdLen += 16;
	}
	// ��������ʶ
	if ('2' == mode[0] || '3' == mode[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, "02", 2);
		hsmCmdLen += 2;
	}
	else
	{
		memcpy(hsmCmdBuf + hsmCmdLen, "01", 2);
		hsmCmdLen += 2;
	}

	// ���ݳ���
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	// ����
	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	// ��żУ���ʶ add by wuhy at 20140903
	if ('A' == mode[0] || 'B' == mode[0] || 'C' == mode[0] || 'D' == mode[0])
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "N", 1);
		hsmCmdLen += 1;
	}

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU1:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(tmpBuf,hsmCmdBuf + offset, 3);
	tmpBuf[3] = 0;
	len = atoi(tmpBuf) * 2;
	*criperDataLen = len;

	offset += 3;
	memcpy(criperData, hsmCmdBuf + offset, *criperDataLen);
	return(*criperDataLen);
}


/*
   �������
   mode ģʽ��ʶ 1=����MAC
   2=У��MAC
   1=����C-MAC
   id ����ID
   0=3DESMAC��ʹ������Կ����3DESMAC��
   1=DESMAC��ʹ�ù�����Կ����DESMAC��
   2=TAC(ʹ������Կ����DESMAC)
   3=3DESMAC��ʹ��2����������Կ����3DESMAC��
   mkType ����Կ���� 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   macFillFlagMAC ��������ʶ
   1=ǿ�����0X80
   0=��ǿ�����0X80
   ivMac IV-MAC 
   lenOfData MAC�������ݳ���
   data MAC��������
   macFlagMAC ���ȱ�ʶ   
   1=4 BYTE
   2=8 BYTE
   �������
   checkMac
   mac MACֵ

 */
int UnionHsmCmdUB(char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, char *checkMac, char *mac)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    lenBuf[32];
	//char    ivBuf[32];
	char    proBuf[32];

	// �������
	memcpy(hsmCmdBuf,"UB",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf + hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ����
	memcpy(hsmCmdBuf + hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if(strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdUB:: UnionGenerateX917RacalKeyString [%s]\n", mk);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
	hsmCmdLen += mkDvsNum*16;

	// ��������
	if ('1' == id[0])
	{

		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 16);
		hsmCmdLen += 16;
	}
	else if ('3' == id[0])
	{
		memset(proBuf,'0',sizeof(proBuf));
		memcpy(proBuf+16-strlen(proFactor),proFactor,strlen(proFactor));
		memcpy(hsmCmdBuf + hsmCmdLen, proBuf, 16);
		hsmCmdLen += 16;
	}
	// MAC��������ʶ
	memcpy(hsmCmdBuf + hsmCmdLen, macFillFlag, 1);
	hsmCmdLen += 1;
	// IV-MAC
	if ( ivMac != NULL && strlen(ivMac) == 16)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, ivMac, 16);
		hsmCmdLen += 16;
	}
	else
	{
		//memset(ivBuf,'0',sizeof(ivBuf));
		//memcpy(hsmCmdBuf + hsmCmdLen, ivBuf, 16);
		memset(hsmCmdBuf + hsmCmdLen, '0', 16);
		hsmCmdLen += 16;
	}
	// MAC�������ݳ���
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;

	/*
	   if (macFlag[0] != '1' && macFlag[0] !='2')
	   {
	   UnionUserErrLog("in UnionHsmCmdUB::macFlag is error!macFlag = [%s]\n",macFlag);
	   return(errCodeParameter);
	   }
	   memcpy(hsmCmdBuf + hsmCmdLen, macFlag, 1);
	   hsmCmdLen += 1;
	 */
	if ((strcmp(macFlag, "1") == 0) || (strcmp(macFlag, "2") == 0))
	{
		memcpy(hsmCmdBuf + hsmCmdLen, macFlag, 1);
		hsmCmdLen += 1;
	}
	else if(strlen(macFlag) == 2)
	{
		// add by chenwd 20150911 ���ݽ�ͨ��ָ��
		memcpy(hsmCmdBuf + hsmCmdLen, macFlag, 2);
		hsmCmdLen += 2;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdUB::macFlag is error!macFlag = [%s]\n",macFlag);
		return(errCodeParameter);
	}

	if ('2' == mode[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, checkMac, 8*atoi(macFlag));
		hsmCmdLen += 8*atoi(macFlag);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdUB:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;
	if (mode[0] == '1')
	{
		memcpy(mac,hsmCmdBuf + offset, 8*atoi(macFlag));
		mac[8*atoi(macFlag)] = 0;
		return(8*atoi(macFlag));
	}
	return(0);
}


/*
   �������
   mode ģʽ��ʶ 
   2=У��MAC
   1=����C-MAC
   id ����ID
   0=3DESMAC��ʹ������Կ����3DESMAC��
   1=DESMAC��ʹ�ù�����Կ����DESMAC��
   2=TAC(ʹ������Կ����DESMAC)
   mk ����Կ
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor �������� 
   macFillFlag MAC��������ʶ
   1=ǿ�����0X80
   0=��ǿ�����0X80
   ivMac IV-MAC 
   lenOfData MAC�������ݳ���
   data MAC��������
   macFlag MAC���ȱ�ʶ
   1=4 BYTE
   2=8 BYTE
   �������
   checkMac
   mac MACֵ

 */
int UnionHsmCmdU3(char *mode, char *id, char *mk, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, char *checkMac, char *mac)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    szMkBuff[128];
	char    lenBuf[32];
	//char    ivBuf[32];

	// �������
	memcpy(hsmCmdBuf,"U3",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
		szMkBuff[ret] = 0;
		memcpy(hsmCmdBuf + hsmCmdLen, szMkBuff, ret);
		hsmCmdLen += ret;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	if ('4' == id[0] || '5' == id[0])
	{
		// ��ɢ����
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*32);
		hsmCmdLen += mkDvsNum*32;
	}
	else
	{
		// ��ɢ����
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
		hsmCmdLen += mkDvsNum*16;
	}

	// ��������
	if ('1' == id[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 16);
		hsmCmdLen += 16;
	}
	else if ('4' == id[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 32);
		hsmCmdLen += 32;
	}

	// MAC��������ʶ
	memcpy(hsmCmdBuf + hsmCmdLen, macFillFlag, 1);
	hsmCmdLen += 1;
	// IV-MAC
	if ( ivMac != NULL && strlen(ivMac) == 16)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, ivMac, 16);
		hsmCmdLen += 16;
	}
	else if(ivMac != NULL && strlen(ivMac) == 32)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, ivMac, 32);
		hsmCmdLen += 32;
	}
	else
	{
		//memset(ivBuf,'0',sizeof(ivBuf));
		//memcpy(hsmCmdBuf + hsmCmdLen, ivBuf, 16);
		memset(hsmCmdBuf + hsmCmdLen, '0', 16);
		hsmCmdLen += 16;
	}
	// MAC�������ݳ���
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;

	if (macFlag[0] != '1' && macFlag[0] !='2' && macFlag[0] != '4')
	{
		UnionUserErrLog("in UnionHsmCmdU3::macFlag is error!macFlag = [%s]\n",macFlag);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf + hsmCmdLen, macFlag, 1);
	hsmCmdLen += 1;
	if ('2' == mode[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, checkMac, 8*atoi(macFlag));
		hsmCmdLen += 8*atoi(macFlag);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU3:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;
	if (mode[0] == '1')
	{
		memcpy(mac,hsmCmdBuf + offset, 8*atoi(macFlag));
		return(8*atoi(macFlag));
	}
	return(0);
}


/*
   �������
   mode ģʽ��־
   1-���ܲ�������ԿУ��ֵ(checkvalue)
   id ����ID
   0=DES_ECB(ֱ�Ӽ��ܣ��������)
   1=DES_CBC(ֱ�Ӽ��ܣ��������)
   mkType ����Կ����
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   pkType ������Կ����
   0=TK(ZEK)��������Կ��
   1=ZMK
   pk ������Կ 
   ivCbc IV-CBC������������ID��Ϊ1ʱ��
   �������
   keyCheckValue ��ԿУ��ֵ
   keyValue ��������

 */
int UnionHsmCmdUK(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex,
		char *ivCbc,char *keyValue, char *keyCheckValue)
{
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	int offset = 0;
	char szMkBuff[128];
	char szPkBuff[128];

	// �������
	memcpy(hsmCmdBuf,"UK",2);
	hsmCmdLen += 2;

	// ��ȫ����
	if (securityMech != NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
		hsmCmdLen += 1;
	}
	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
		szMkBuff[ret] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
		hsmCmdLen += ret;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*32);
	hsmCmdLen += mkDvsNum*32;

	// ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 1);
	hsmCmdLen += 1;

	// ������Կ
	if ((pk == NULL) || (strlen(pk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", pkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(pk), pk, szPkBuff);
		szPkBuff[ret] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen, szPkBuff, ret);
		hsmCmdLen += ret;
	}

	// IV-CBC
	if (id[0] == '1')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 16);
		hsmCmdLen += 16;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdUK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;
	memcpy(keyValue, hsmCmdBuf+offset, 32);
	offset += 32;
	if (mode[0] == '1')
	{
		memcpy(keyCheckValue, hsmCmdBuf + offset, 6);
	}

	return(strlen(keyValue));
}


/*
   ���ܣ�����ȫ���ı�������Կ���뵽���ܻ��У�EMV 4.1/PBOC��
   �������
   mode ģʽ��־ 
   1-��֤MAC������
   id ����ID 
   0=M/Chip4��CBCģʽ��ǿ�����X80��
   1=VISA/PBOC��������ָ����ECB��
   mkFlag �洢��־ 
   1-����Կ�洢�����ܻ���
   mkIndex �洢����
   mkLengthFlag ��Կ������ָ��������Կ���ͣ���ȷ�����ܵ�LMK��
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN

   pkType ������Կ����
   0=TK��������Կ��
   1=DK-SMC��ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ��
   pk ������Կ 
   pkDvsNum ������Կ��ɢ����
   pkDvsData ������Կ��ɢ���� 
   ivCbc IV-CBC
   criperDataLen ���ĳ��� 
   criperData ��������
   mac MAC
   ivMac IV-MAC 
   macDataLen MAC������ݳ���
   macData MAC������� 
   macOffset ƫ����
   �������
   keyValue LMK��������Կ��ԿУ��ֵ
   checkValue ��ԿУ��ֵ

 */
int UnionHsmCmdU4(char *mode,char *id,char *mkFlag,int mkIndex,char *mkType,char *mkLengthFlag,
		char *pkType,char *pk,int pkIndex,int pkDvsNum,char *pkDvsData,char *proKeyFlag,char *ivCbc,int criperDataLen,
		char *criperData,char *mac,char *ivMac,int macDataLen,char *macData, int macOffset,char *keyValue,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	offset = 0;
	char	szPkBuff[128];

	// �������
	memcpy(hsmCmdBuf,"U4",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen,mode,1);
	hsmCmdLen += 1;
	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen,id,1);
	hsmCmdLen += 1;
	// �洢��־
	memcpy(hsmCmdBuf+hsmCmdLen,mkFlag,1);
	hsmCmdLen += 1;
	// �洢����
	sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
	hsmCmdLen += 4;
	// ��Կ����
	memcpy(hsmCmdBuf+hsmCmdLen,mkType,3);
	hsmCmdLen += 3;
	// �����Կģʽ
	memcpy(hsmCmdBuf+hsmCmdLen,mkLengthFlag,1);
	hsmCmdLen += 1;

	// ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen,pkType,1);
	hsmCmdLen += 1;

	// ������Կ
	if ((pk == NULL) || (strlen(pk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", pkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(pk),pk,szPkBuff);
		szPkBuff[ret] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen,szPkBuff,ret);
		hsmCmdLen += ret;
	}
	// ������Կ��ɢ����, ������Կ��ɢ����
	if (pkType[0] == '1')
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*16);
		hsmCmdLen += pkDvsNum*16;
	}
	// IV-CBC
	if (id[0] == '0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 8);
		hsmCmdLen += 8;
	}
	// ���ĳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%02X",criperDataLen);
	hsmCmdLen += 2;
	// ��������
	memcpy(hsmCmdBuf+hsmCmdLen,criperData,criperDataLen);
	hsmCmdLen += criperDataLen;
	// MAC
	memcpy(hsmCmdBuf+hsmCmdLen,mac,8);
	hsmCmdLen += 8;

	// IV-MAC, MAC������ݳ���, MAC�������, ƫ����
	if (mode[0] == '1')
	{
		if (strlen(ivMac) > 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 16);
			hsmCmdLen += 16;
		}

		if (macDataLen > 8000)
		{
			UnionUserErrLog("in UnionHsmCmdU4::macDataLen[%d] too large\n", macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, macData, macDataLen);
		hsmCmdLen += macDataLen;

		if ((macOffset < 0) || (macOffset > macDataLen))
		{
			UnionUserErrLog("in UnionHsmCmdU4::macOffset[%d] <0 or >macDataLen[%d]\n", macOffset, macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macOffset/2);
		hsmCmdLen += 4;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU4:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;
	if (strncmp(mkLengthFlag,"Z",1) == 0)
	{
		memcpy(keyValue,hsmCmdBuf+offset,16);
		offset += 16;
	}
	else if ((strncmp(mkLengthFlag,"X",1) == 0)||(strncmp(mkLengthFlag,"U",1) == 0))
	{
		memcpy(keyValue,hsmCmdBuf+offset,32);
		offset += 32;
	}
	else if ((strncmp(mkLengthFlag,"Y",1) == 0)||(strncmp(mkLengthFlag,"T",1) == 0))
	{
		memcpy(keyValue,hsmCmdBuf+offset,48);
		offset += 48;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdU4:: mkLengthFlag=[%s] is error!\n",mkLengthFlag);
		return(errCodeParameter);
	}

	memcpy(checkValue,hsmCmdBuf+offset,16);

	return(0);
}


/*
   ����
   �ӽ�������
   ���������
   encryFlag��1�����ܣ�2������
   mode��01��DES_ECB��02��DES_CBC
   keyType����Կ���ͣ�000��ZMK��00A��ZAK
   keyLen����Կ���ȣ�0 = ��������Կ��1 = ˫������Կ
   key���ӽ�����Կ
   iv��CBCģʽʱʹ�õĳ�ʼ������
   lenOfData����Ҫ��ժҪ�����ݳ���
   pData����Ҫ��ժҪ������
   �������
   pCipherDataLen���������ݳ���
   pCipherData����������

 */
int UnionHsmCmdTG(char encryFlag,char *mode,char *keyType,
		char keyLen,char *key,char *iv,int lenOfData,char *pData,
		int *pCipherDataLen,char *pCipherData)
{
	int				ret;
	char				hsmCmdReq[2048];
	int				hsmCmdReqLen;
	char				hsmCmdRsp[2048];
	char				tmpBuf[32];
	char				t_data[2048];
	int				padDataLen=0;
	ret		=	  -1;
	hsmCmdReqLen	=	  0;

	strncpy(hsmCmdReq,"TG",2);
	hsmCmdReqLen += 2;

	hsmCmdReq[hsmCmdReqLen] = encryFlag;
	hsmCmdReqLen++;

	strncpy(hsmCmdReq+hsmCmdReqLen,mode,2);
	hsmCmdReqLen += 2;

	strncpy(hsmCmdReq+hsmCmdReqLen,keyType,3);
	hsmCmdReqLen += 3;

	hsmCmdReq[hsmCmdReqLen] = keyLen;
	hsmCmdReqLen++;

	if (keyLen == '0')
	{
		strncpy(hsmCmdReq+hsmCmdReqLen,key,16);
		hsmCmdReqLen += 16;
	}
	else if (keyLen == '1')
	{
		strncpy(hsmCmdReq+hsmCmdReqLen,key,32);
		hsmCmdReqLen += 32;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdTG:: unknow keyLen = [%d] error,it must be 1 or 0\n",keyLen);
		return(errCodeParameter);
	}

	if (strncmp(mode,"02",2) == 0)
	{
		strncpy(hsmCmdReq+hsmCmdReqLen,iv,strlen(iv));
		hsmCmdReqLen += strlen(iv);
	}else
	{
		//ECB ģʽ �����ݽ������
		if (lenOfData % 16 != 0)
			padDataLen = 16 - lenOfData % 16;

		memcpy(t_data,pData, lenOfData);

		memset(t_data+lenOfData,'0',padDataLen);

		lenOfData += padDataLen;
		t_data[lenOfData] = 0;
		memcpy(pData,t_data,lenOfData);
	}


	sprintf(hsmCmdReq+hsmCmdReqLen,"%04d",lenOfData/2);
	hsmCmdReqLen += 4;

	hsmCmdReqLen += aschex_to_bcdhex(pData,lenOfData,hsmCmdReq+hsmCmdReqLen);
	hsmCmdReq[hsmCmdReqLen] = 0;

	/* �������ͨѶ */
	if ((ret = UnionDirectHsmCmd(hsmCmdReq,hsmCmdReqLen,hsmCmdRsp,sizeof(hsmCmdRsp))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdTG:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdRsp[ret] = 0;

	strncpy(tmpBuf,hsmCmdRsp+4,4);
	tmpBuf[4] = 0;
	*pCipherDataLen=bcdhex_to_aschex(hsmCmdRsp+8,atoi(tmpBuf),pCipherData);

	return(*pCipherDataLen);
}


/*
   ��������:
   ��������keyA����ת����ΪkeyB����
   �������:
   srcKeyType			 Դ��Կ������
   srcKey	 1A+3H/16H/32H/48H       Դ��Կ
   srcModel				Դ�����㷨
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4��CBCģʽ��ǿ�����X80��
   "21"=VISA/PBOC��������ָ����ECB��
   srcIV	  16H		��ʼ������Դ�����㷨ΪCBC����ʱ���������
   dstKeyType			 Ŀ����Կ����
   dstKey	 1A+3H/16H/32H/48H       Ŀ����Կ
   dstModel				Ŀ�ļ����㷨
   "00"=DES_ECB
   "02"=DES_ECB_P
   dstIV	  16H		��ʼ������Ŀ�ļ����㷨ΪCBC����ʱ���������
   lenOfSrcCiphertext		 Դ��Կ���ܵ��������ݳ���
   srcCiphertext   nH		 Դ��Կ���ܵ���������

   �������:
   dstCiphertext   nH		 Ŀ����Կ���ܵ���������

 */
int UnionHsmCmdUE(char *srcKeyType,char *srcKey,int srcKeyIdx,char *srcModel,char *srcIV,
		char *dstKeyType,char *dstKey,int dstKeyIdx,char *dstModel,char *dstIV,
		int lenOfSrcCiphertext,char *srcCiphertext,char *dstCiphertext)
{
	int				ret;
	char				hsmCmdReq[2048];
	int				hsmCmdReqLen;
	char				hsmCmdRsp[2048];
	char				tmpBuf[32];
	int				lenOfDstCiphertext;

	ret		=	  -1;
	lenOfDstCiphertext      =	  0;
	hsmCmdReqLen	=	  0;

	if ((srcModel == NULL) || (dstModel == NULL) || (srcCiphertext == NULL) || (dstCiphertext == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdUE:: parameters err!\n");
		return(errCodeParameter);
	}

	strncpy(hsmCmdReq,"UE",2);
	hsmCmdReqLen += 2;

	strncpy(hsmCmdReq+hsmCmdReqLen,srcModel,strlen(srcModel));
	hsmCmdReqLen += strlen(srcModel);

	strncpy(hsmCmdReq+hsmCmdReqLen,srcKeyType,strlen(srcKeyType));
	hsmCmdReqLen += strlen(srcKeyType);

	strncpy(hsmCmdReq+hsmCmdReqLen,"1",1);
	hsmCmdReqLen += 1;

	strncpy(hsmCmdReq+hsmCmdReqLen,srcKey,strlen(srcKey));
	hsmCmdReqLen += strlen(srcKey);

	if (strncmp(srcModel,"1",1) == 0)
	{
		if (srcIV == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdUE:: parameters err srcIV == NULL!\n");
			return(errCodeParameter);
		}
		strncpy(hsmCmdReq+hsmCmdReqLen,srcIV,strlen(srcIV));
		hsmCmdReqLen += strlen(srcIV);
	}

	strncpy(hsmCmdReq+hsmCmdReqLen,dstModel,strlen(dstModel));
	hsmCmdReqLen += strlen(dstModel);

	strncpy(hsmCmdReq+hsmCmdReqLen,dstKeyType,strlen(dstKeyType));
	hsmCmdReqLen += strlen(dstKeyType);

	strncpy(hsmCmdReq+hsmCmdReqLen,"1",1);
	hsmCmdReqLen += 1;

	strncpy(hsmCmdReq+hsmCmdReqLen,dstKey,strlen(dstKey));
	hsmCmdReqLen += strlen(dstKey);

	if (strncmp(dstModel,"1",1) == 0)
	{
		if (dstIV == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdUE:: parameters err dstIV == NULL!\n");
			return(errCodeParameter);
		}
		strncpy(hsmCmdReq+hsmCmdReqLen,dstIV,strlen(dstIV));
		hsmCmdReqLen += strlen(dstIV);
	}

	sprintf(hsmCmdReq+hsmCmdReqLen,"%04d",lenOfSrcCiphertext/2);
	hsmCmdReqLen += 4;

	aschex_to_bcdhex(srcCiphertext,lenOfSrcCiphertext,hsmCmdReq+hsmCmdReqLen);
	hsmCmdReqLen += lenOfSrcCiphertext/2;
	hsmCmdReq[hsmCmdReqLen] = 0;

	// modify by leipp 20150428
	UnionSetBCDPrintTypeForHSMCmd();
	// end

	/* �������ͨѶ */
	if ((ret = UnionDirectHsmCmd(hsmCmdReq,hsmCmdReqLen,hsmCmdRsp,sizeof(hsmCmdRsp))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdUE:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdRsp[ret] = 0;

	memcpy(tmpBuf,hsmCmdRsp+4,4);
	tmpBuf[4] = 0;
	lenOfDstCiphertext = bcdhex_to_aschex(hsmCmdRsp+4+4,atoi(tmpBuf),dstCiphertext);

	return(lenOfDstCiphertext);
}


/*
   ����
   ����Կ��KEK����תΪLMK����
   ���������
   mode���㷨��ʶ
   00��DES_ECB
   01��DES_ECB_LP
   02��DES_ECB_P
   10��DES_CBC
   11��DES_CBC_LP
   12��DES_CBC_P
   kekType����Կ���ͣ�000��ZMK��
kekLen:��Կ���ȣ�0 = ��������Կ��1 = ˫������Կ
kek����Կ������Կ
iv��CBCģʽʱʹ�õĳ�ʼ������
keyType����Կ���ͣ�000��ZMK��
lenOfKeyCipherTextByKek����Ҫ��ժҪ�����ݳ���
keyCipherTextByKek����Ҫ��ժҪ������
�������
keyCipherTextByLmk����������

 */
int UnionHsmCmdTU(char *mode,char *kekType,char kekLen,
		char *kek,char *iv,char *keyType,char keyLen,int lenOfKeyCipherTextByKek,char *keyCipherTextByKek,
		int *lenOfKeyCipherTextByLmk,char *keyCipherTextByLmk)
{
	int				ret;
	char				hsmCmdReq[2048];
	int				hsmCmdReqLen;
	char				hsmCmdRsp[2048];

	ret		=	  -1;
	hsmCmdReqLen	=	  0;

	strncpy(hsmCmdReq,"TU",2);
	hsmCmdReqLen += 2;

	strncpy(hsmCmdReq+hsmCmdReqLen,mode,2);
	hsmCmdReqLen += 2;

	strncpy(hsmCmdReq+hsmCmdReqLen,kekType,3);
	hsmCmdReqLen += 3;

	hsmCmdReq[hsmCmdReqLen] = kekLen;
	hsmCmdReqLen += 1;

	if (kekLen == '0')
	{
		strncpy(hsmCmdReq+hsmCmdReqLen,kek,16);
		hsmCmdReqLen += 16;
	}
	else if (kekLen == '1')
	{
		strncpy(hsmCmdReq+hsmCmdReqLen,kek,32);
		hsmCmdReqLen += 32;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdTU:: unknow kekLen = [%d] error,it must be 1 or 0\n",kekLen);
		return(errCodeParameter);
	}


	strncpy(hsmCmdReq+hsmCmdReqLen,keyType,3);
	hsmCmdReqLen += 3;

	hsmCmdReq[hsmCmdReqLen] = keyLen;
	hsmCmdReqLen += 1;

	sprintf(hsmCmdReq+hsmCmdReqLen,"%04d",lenOfKeyCipherTextByKek/2);
	hsmCmdReqLen += 4;

	hsmCmdReqLen += aschex_to_bcdhex(keyCipherTextByKek,lenOfKeyCipherTextByKek,hsmCmdReq+hsmCmdReqLen);

	if (strncmp(mode,"1",1) == 0)
	{
		if (iv == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdTU:: parameters err iv == NULL!\n");
			return(errCodeParameter);
		}
		strncpy(hsmCmdReq+hsmCmdReqLen,iv,strlen(iv));
		hsmCmdReqLen += strlen(iv);
	}

	strncpy(hsmCmdReq+hsmCmdReqLen,"00000000",8);
	hsmCmdReqLen += 8;
	hsmCmdReq[hsmCmdReqLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	/* �������ͨѶ */
	if ((ret = UnionDirectHsmCmd(hsmCmdReq,hsmCmdReqLen,hsmCmdRsp,sizeof(hsmCmdRsp))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdTU:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdRsp[ret] = 0;

	// modify by leipp 20150326 begin
	//strncpy(tmpBuf,hsmCmdRsp,4);
	//tmpBuf[4] = 0;
	//
	//*lenOfKeyCipherTextByLmk = bcdhex_to_aschex(hsmCmdRsp+4,atoi(tmpBuf),keyCipherTextByLmk);
	*lenOfKeyCipherTextByLmk = sprintf(keyCipherTextByLmk,"%s",hsmCmdRsp+4);
	// modify by leipp 20150326 end

	return(*lenOfKeyCipherTextByLmk);
}

int UnionHsmCmdVY(char *k1, int lenOfK1, char *k2, int lenOfK2,int isDis, char *disFac, int lenOfDisFac, int pinMaxLen, char *pinByZPK1, int lenOfPinByZPK1, char *pinFormatZPK1, int lenOfPinFormatZPK1, char *pinFormatMDKENC, int lenOfPinFormatMDKENC, char *accNo1, int lenOfAccNo1, char *accNo2, int lenOfAccNo2,
		char *pinSec, char *pinFormat, char *isWeaKey)
{
	int	ret;
	char	hsmCmdBuf[512];
	char	tmpbuf[128];
	int	hsmCmdLen = 0;
	int	lenOfPin;

	if ((k1 == NULL) || (k2 == NULL) || (pinByZPK1 == NULL) || (pinFormatZPK1 == NULL) || (pinFormatMDKENC == NULL) || (accNo1 == NULL) || (accNo2 == NULL))
	{
		UnionUserErrLog("in RacalCmdVY:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"VY",2);
	hsmCmdLen = 2;
	// LMK^[$)A^N<SC\5D^Opik
	if ((ret = UnionGenerateX917RacalKeyString(lenOfK1,k1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in %s :: UnionGenerateX917RacalKeyString for [%s][%d]\n",__func__,k1,lenOfK1);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK^[$)A^N<SC\5D^OMKD_ENC
	if ((ret = UnionGenerateX917RacalKeyString(lenOfK2,k2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdVY:: UnionGenerateX917RacalKeyString [%s][%d!\n",k2,lenOfK2);
		return(ret);
	}
	hsmCmdLen += ret;

	//add key type
	sprintf (hsmCmdBuf + hsmCmdLen, "%s", "00100A");
	hsmCmdLen += 6;

	//int isDis, char *disFac, int lenOfDisFac,
	//modified by zhangjl 0831
	sprintf (hsmCmdBuf + hsmCmdLen, "%d", isDis);
	hsmCmdLen += 1;

	if (isDis == 1)
	{
		snprintf (hsmCmdBuf + hsmCmdLen, lenOfDisFac,"%s", disFac);
		hsmCmdLen += lenOfDisFac;
	}

	//add pinlength
	sprintf (hsmCmdBuf + hsmCmdLen, "%02d", pinMaxLen);
	hsmCmdLen += 2;

	//add pinBlock
	sprintf (hsmCmdBuf+hsmCmdLen,"%s",pinByZPK1);
	hsmCmdLen += lenOfPinByZPK1;

	//add PINBLOCK TYPE
	sprintf (hsmCmdBuf + hsmCmdLen,"%s%s",pinFormatZPK1, pinFormatMDKENC);
	hsmCmdLen = hsmCmdLen + lenOfPinFormatZPK1 + lenOfPinFormatMDKENC;

	//add accno1
	if ((ret = UnionForm12LenAccountNumber (accNo1, lenOfAccNo1, hsmCmdBuf + hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdVY:: UnionForm12LenAccountNumber!\n");
		return (ret);
	}
	hsmCmdLen += ret;

	//add accno2
	if ((ret = UnionForm12LenAccountNumber (accNo2, lenOfAccNo2, hsmCmdBuf + hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdVY:: UnionForm12LenAccountNumber!\n");
		return (ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	//communicate with sjl
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdVY:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	ret = 4;
	memcpy(tmpbuf,hsmCmdBuf+ret,2);
	tmpbuf[2] = 0;
	ret += 2;
	lenOfPin = UnionConvertIntStringToInt (tmpbuf, 2);
	memcpy (pinSec, hsmCmdBuf + ret, 16);
	ret += 16;
	memcpy (pinFormat, hsmCmdBuf + ret, 2);
	ret += 2;
	memcpy (isWeaKey, hsmCmdBuf + ret, 1);

	return(lenOfPin);
}


int UnionHsmCmdPE(char *pinByLmk,char *accNo,int fldNum,char fldGrp[][128+1],char *retValue,int sizeOfBuf)
{
	int	ret;
	char	hsmCmdBuf[4096];
	int	hsmCmdLen = 0;
	char	strdate[32];
	int	i;
	int	len;

	if ((accNo == NULL) || (pinByLmk == NULL) || strlen(accNo) < 12)
	{
		UnionUserErrLog("in UnionHsmCmdPE:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"PE",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,"C",1);
	hsmCmdLen++;
	// 12λ�ͻ��ʺ�

	memcpy(hsmCmdBuf+hsmCmdLen,accNo,12);
	hsmCmdLen += 12;

	// PIN����
	strcpy(hsmCmdBuf+hsmCmdLen,pinByLmk);
	hsmCmdLen += strlen(pinByLmk);

	// ��
	for (i = 0; i < fldNum; i++)
	{       
		len = strlen(fldGrp[i]);
		if (len + hsmCmdLen >= sizeof(hsmCmdBuf) - 1)
		{
			UnionUserErrLog("in UnionHsmCmdPE:: fldGrp length too long!\n");
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,fldGrp[i],len);
		hsmCmdLen += len;
		hsmCmdBuf[hsmCmdLen] = ';';
		hsmCmdLen++;
	}
	//����
	if ((ret =  UnionGetFullSystemDate(strdate)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPE:: UnionGetFullSystemDate err!\n");
		return(ret);
	}

	memcpy(hsmCmdBuf+hsmCmdLen,strdate,4);
	hsmCmdLen+=4;
	hsmCmdBuf[hsmCmdLen] = '/';
	hsmCmdLen++;
	memcpy(hsmCmdBuf+hsmCmdLen,strdate+4,2);
	hsmCmdLen+=2;
	hsmCmdBuf[hsmCmdLen] = '/';
	hsmCmdLen++;
	memcpy(hsmCmdBuf+hsmCmdLen,strdate+6,2);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPE:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if (retValue != NULL)
	{
		if (sizeOfBuf > ret)
			memcpy(retValue,hsmCmdBuf,ret);
		else
		{
			memcpy(retValue,hsmCmdBuf,sizeOfBuf - 1);
			ret = sizeOfBuf - 1;
		}
	}
	return(ret);
}

/* �������
   algorithmID	�㷨��ʶ��0:ECB	1:CBC
   keyType		��Կ����
   zek		������Կ
   iv		iv
   keyByZek	��Կ����
   keyFlag		��Կ����X/U
   sizeOfBuf	����keyByLmk����Ļ����С
   �������
   keyByLmk	��Կ����
   checkValue	����ֵ
 */
int UnionHsmCmdM8(char *algorithmID,TUnionDesKeyType keyType,char *zek,char *iv,char *keyByZek,char *keyFlag,char *keyByLmk,char *checkValue,int sizeOfBuf)
{
	int			ret;
	char			hsmCmdBuf[8192];
	int			hsmCmdLen = 0;

	// ָ�����
	memcpy(hsmCmdBuf,"M8",2);
	hsmCmdLen = 2;

	// �㷨��ʶ
	memcpy(hsmCmdBuf + hsmCmdLen,algorithmID,1);
	hsmCmdLen += 1;

	// ��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// ��Կ
	if (strlen(zek) == 3)
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%s",zek);
		hsmCmdLen += 4;	
	}
	else if (strlen(zek) == 4)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, zek, 4);
		hsmCmdLen += 4;
	}
	else
	{
		memcpy(hsmCmdBuf + hsmCmdLen,zek,32);
		hsmCmdLen += 32;	
	}

	// iv
	if (algorithmID[0] == '1')
	{
		memcpy(hsmCmdBuf + hsmCmdLen,iv,16);
		hsmCmdLen += 16;	
	}

	// ��Կ����
	memcpy(hsmCmdBuf + hsmCmdLen,keyByZek,32);
	hsmCmdLen += 32;

	// LMK������Կ����
	if (keyFlag == NULL)
		memcpy(hsmCmdBuf + hsmCmdLen,"X",1);
	else
		memcpy(hsmCmdBuf + hsmCmdLen,keyFlag,1);

	hsmCmdLen += 1;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdLen = 4 + 1;
	memcpy(keyByLmk,hsmCmdBuf+hsmCmdLen,32);
	hsmCmdLen += 32;

	memcpy(checkValue,hsmCmdBuf+hsmCmdLen,16);
	hsmCmdLen += 16;

	return(0);
}

/*
   ���ã�M1�ӽ�������
����: 
 */

int UnionHsmCmdM1(char *algorithmID,TUnionDesKeyType keyType, char *zek, char *iv, char *inData, int dataLen, char * encryptData, int sizefEncryptDaga)
{
	int     ret;
	char    hsmCmdBuf[8192];
	char		tmpBuf[8];
	int     hsmCmdLen = 0;
	int			dataLenTmp=0;

	// ָ�����
	memcpy(hsmCmdBuf,"M1",2);
	hsmCmdLen = 2;

	// �㷨��ʶ
	memcpy(hsmCmdBuf + hsmCmdLen,algorithmID,1);
	hsmCmdLen += 1;

	// ��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// ��Կ
	if (strlen(zek) == 3)
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%s",zek);
		hsmCmdLen += 4;	
	}
	else
	{
		memcpy(hsmCmdBuf + hsmCmdLen,zek,32);
		hsmCmdLen += 32;	
	}

	// iv
	if (algorithmID[0] == '2' || algorithmID[0] == '3')
	{
		memcpy(hsmCmdBuf + hsmCmdLen,iv,16);
		hsmCmdLen += 16;	
	}

	//���ݳ��ȣ�Ϊ16�ı���
	if (dataLen % 16 || dataLen > 4000)
	{
		UnionUserErrLog("in UnionHsmCmdM1::dataLen illegal [%d]!\n", dataLen);
		return(errCodeParameter);
	}


	// ���ݼ���
	if (algorithmID[0] == '0' || algorithmID[0] == '2')
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%04d", dataLen);
		dataLenTmp = dataLen *2;
		memcpy(hsmCmdBuf + hsmCmdLen, inData, dataLen);
		hsmCmdLen += dataLen;
	}
	else//��������ΪBCD���룬��Ҫѹ��
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%04d", dataLen/2);
		dataLenTmp= dataLen;
		hsmCmdLen += aschex_to_bcdhex(inData, dataLen, hsmCmdBuf + hsmCmdLen);
	}

	if (sizefEncryptDaga < dataLenTmp)
	{
		UnionUserErrLog("in UnionHsmCmdM1::sizefEncryptDaga is too small,sizefEncryptDaga= [%d],dataLenTmp=[%d]!\n", sizefEncryptDaga,dataLenTmp);
		return(errCodeParameter);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM1:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(tmpBuf, hsmCmdBuf + 4, 4);
	tmpBuf[4] = 0;
	if (algorithmID[0] == '0' || algorithmID[0] == '2')
		ret = bcdhex_to_aschex(hsmCmdBuf + 8, atoi(tmpBuf), encryptData);
	else
	{
		memcpy(encryptData, hsmCmdBuf + 8, atoi(tmpBuf));
		ret = atoi(tmpBuf);
	}
	return(ret);
}

////////////////////����Ϊpboc30���ӵ�ָ��////////////////////

// ����SM2��Կ��
/* �������
   index		��Կ����
   lenOfVK	��Կ�����ֽ���
   VK		��Կ����
   �������
   ��
 */
int UnionHsmCmdK2(int index, int lenOfVK, unsigned char *VK)
{
	int			ret;
	char			hsmCmdBuf[8192];
	int			hsmCmdLen = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"K2",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen ,"%02d",index);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen ,"%04d",lenOfVK);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf+hsmCmdLen,VK,lenOfVK);
	hsmCmdLen += lenOfVK;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK2:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

//����SM2˽Կ
/*�������
  index		��Կ����
  isOutPutPK	�Ƿ���Ҫ������Կ, 0�����޸��򣺱�ʾ����Ҫ������Կ, 1����ʾ��Ҫ������Կ
  �������
  lenOfVK		��Կ�����ֽ���
  VK		��Կ����
  PK		��Կֵ
 */
int UnionHsmCmdK8(int index,char *isOutPutPK,int *lenOfVK,unsigned char *VK,unsigned char *PK)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"K8",2);
	hsmCmdLen = 2;
	//��Կ����
	sprintf(hsmCmdBuf+hsmCmdLen ,"%02d",index);
	hsmCmdLen += 2;

	// �Ƿ���Ҫ������Կ
	if (strcmp(isOutPutPK, "1") == 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen ,"%s",isOutPutPK);
		hsmCmdLen += 1;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if ((*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK8:: lenOfVK[%d] error!\n",*lenOfVK);
		return(errCodeParameter);
	}
	memcpy(VK,hsmCmdBuf+4+4,*lenOfVK);

	if (strcmp(isOutPutPK, "1") == 0)
	{
		memcpy(PK,hsmCmdBuf+4+4+(*lenOfVK),64);
	}

	return(*lenOfVK);
}

//����SM2˽Կ��TK������
/*�������
  SM2Index	��Կ����
  lenOfSM2	�����Կ���ĳ���
  SM2		�����Կ����
  keyType		��Կ����
  Key		����Կ�����µĴ���(SM4)��Կ
  checkValue	������ԿУ��ֵ
  �������
  lenOfVK		��Կ�����ֽ���
  VK		TK�����µ�SM2˽Կ����

 */
int UnionHsmCmdK9(int SM2Index,int lenOfSM2,char *SM2,TUnionDesKeyType keyType,char *Key,char *checkValue,int *lenOfVK,unsigned char *VK)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;
	char    tmpBuf[32];

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"K9",2);
	hsmCmdLen = 2;
	if(SM2Index < 0)
	{
		//��Կ����
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// �����Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfSM2);
		hsmCmdLen += 4;
		// �����Կ
		memcpy(hsmCmdBuf + hsmCmdLen,SM2,lenOfSM2);
		hsmCmdLen += lenOfSM2;
	}
	else
	{
		//��Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",SM2Index);
		hsmCmdLen += 2;
	}
	//��Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(keyType,tmpBuf);
	tmpBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, tmpBuf, 3);
	hsmCmdLen += 3;
	//LMK���ܵĴ��䣨SM4����Կ
	memcpy(hsmCmdBuf+hsmCmdLen,Key,32);
	hsmCmdLen += 32;
	//��ԿУ��ֵ
	memcpy(hsmCmdBuf+hsmCmdLen,checkValue,16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = 0;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK9:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if ((*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK9:: lenOfVK[%d] error!\n",*lenOfVK);
		return(errCodeParameter);
	}
	memcpy(VK,hsmCmdBuf+4+4,*lenOfVK);
	VK[*lenOfVK] = 0;
	return(*lenOfVK);
}	

int UnionHsmCmdK9ForInput(char mode,int SM2Index,int lenOfSM2,char *SM2,int lenOfPK,char *pk,TUnionDesKeyType keyType,char *Key,char *checkValue,int *lenOfVK,unsigned char *VK)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;
	char	tmpBuf[32];

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"K9",2);
	hsmCmdLen = 2;

	// ģʽ��ʶ
	if (mode == 'N')
	{
		hsmCmdBuf[hsmCmdLen] = mode;
		hsmCmdLen++;
	}

	if(SM2Index < 0)
	{
		//��Կ����
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// �����Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfSM2);
		hsmCmdLen += 4;
		// �����Կ
		memcpy(hsmCmdBuf + hsmCmdLen,SM2,lenOfSM2);
		hsmCmdLen += lenOfSM2;
	}
	else
	{
		//��Կ����
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",SM2Index);
		hsmCmdLen += 2;
	}

	if (mode == 'N')
	{
		//��ԿX
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPK/2);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf + hsmCmdLen, pk, lenOfPK/2);
		hsmCmdLen += lenOfPK/2;

		//��ԿY
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPK/2);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf + hsmCmdLen, pk + lenOfPK/2, lenOfPK/2);
		hsmCmdLen += lenOfPK/2;

		hsmCmdBuf[hsmCmdLen] = '3';
		hsmCmdLen++;
	}

	//��Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(keyType,tmpBuf);
	tmpBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, tmpBuf, 3);
	hsmCmdLen += 3;
	//LMK���ܵĴ��䣨SM4����Կ
	memcpy(hsmCmdBuf+hsmCmdLen,Key,32);
	hsmCmdLen += 32;
	//��ԿУ��ֵ
	memcpy(hsmCmdBuf+hsmCmdLen,checkValue,16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = 0;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK9:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if ((*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK9:: lenOfVK[%d] error!\n",*lenOfVK);
		return(errCodeParameter);
	}
	memcpy(VK,hsmCmdBuf+4+4,*lenOfVK);
	return(*lenOfVK);
}


// SM4�㷨�ӽ�������
/* �������
   encryptFlag		�ӽ��ܱ�ʶ
   1��������
   2��������
mode:
01 �CSM4-ECB
02 --SM4-CBC
03 --SM4-CFB
04 --SM4-OFB
keyType			֧�ֵ���Կ����ΪZEK/ZMK
lenOfKey			1 = ˫������Կ
Key		��S����ʾSM4��Կ,����Կ��LMK���ܵ���������
checkValue			KeyУ��ֵ
iv			��ʼ������ Mode=02�ͣ�������ڡ�
lenOfDate			�ӽ������ݳ��ȣ����ȱ�����16�ı���
data			���ӽ�������
�������
value			�ӽ��ܺ������
 */
int UnionHsmCmdWA(int encryptFlag, char *mode, char *keyType, int lenOfKey, char *Key, char *checkValue, char *iv, int lenOfDate, unsigned char *data, char *value)
{
	int		ret;
	char		hsmCmdBuf[8192];
	char		tmpBuf[512];
	int		hsmCmdLen = 0;
	int 		lenOfValue;
	int		offset;

	memcpy(hsmCmdBuf,"WA",2);
	hsmCmdLen = 2;

	//��ȡ�ӽ��ܱ�ʶ
	sprintf(hsmCmdBuf+hsmCmdLen ,"%01d",encryptFlag);
	hsmCmdLen += 1;

	// add by leipp 20150116
	UnionSetBCDPrintTypeForHSMCmd();
	if (encryptFlag == 2)
		UnionSetMaskPrintTypeForHSMReqCmd();
	else
		UnionSetMaskPrintTypeForHSMResCmd();
	// end

	//�ӽ���ģʽ
	memcpy(hsmCmdBuf+hsmCmdLen,mode,2);
	hsmCmdLen += 2;

	//��Կ����
	memcpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	//1 = ˫������Կ
	sprintf(hsmCmdBuf+hsmCmdLen ,"%01d",lenOfKey);
	hsmCmdLen += 1;

	//LMK���ܵ���Կ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,Key,32);
	hsmCmdLen += 32;

	//KeyУ��ֵ
	memcpy(hsmCmdBuf+hsmCmdLen,checkValue,16);
	hsmCmdLen += 16;

	//	if(strcmp(mode, "02") == 0)
	if((strcmp(mode, "02") == 0)||(strcmp(mode, "03") == 0)||(strcmp(mode, "04") == 0))//changed by lusj 20151031

	{
		memcpy(hsmCmdBuf+hsmCmdLen,iv,32);
		hsmCmdLen += 32;
	}

	//���ӡ���������
	sprintf(hsmCmdBuf+hsmCmdLen ,"%04d",lenOfDate);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfDate);
	hsmCmdLen += lenOfDate;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWA:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(tmpBuf,hsmCmdBuf+offset,4);
	tmpBuf[4] = 0;
	offset += 4;
	lenOfValue = atoi(tmpBuf);
	memcpy(value,hsmCmdBuf+offset,lenOfValue);
	value[lenOfValue] = 0;

	return(lenOfValue);
}


//�ű����ݼ������EMV 4.1/PBOC��
/*�������
  mode			ģʽ
  id			����ID
  mkIndex			MK-SMC����
  mk			MK-SMC
  checkMk			MK-SMCУ��ֵ
  pan			PAN/PAN���к�
  atc			ATC
  iv			ģʽ��־λ0ʱ�д���
  lenOfPlainData		�������ݳ���
  plainData		��������
  �������
  lenOfCipherData		�������ݳ���
  cipherData		��������
 */
int UnionHsmCmdWB(char *mode,char *id,int mkIndex,char *mk,char *checkMk,char *pan,char *atc,char *iv,int lenOfPlainData,char *plainData,int *lenOfCipherData,char *cipherData)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	char	tmpBuf[32];
	char	keyString[64];
	int	offset = 0;
	char    tmpPan[32];
	char    atcTmp[32];
	char    plainDataTmp[1024];

	// �������
	memcpy(hsmCmdBuf,"WB",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	//MK-SMC 
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		if (strlen(mk) == 32)
		{
			keyString[0] = 'S';
			memcpy(&keyString[1], mk, 32);
			memcpy(hsmCmdBuf+hsmCmdLen, keyString, 33);
			hsmCmdLen += 33;
		}
		else if(strlen(mk) == 33)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, mk, 33);
			hsmCmdLen += 33;
		}
		else
		{
			UnionUserErrLog("in UnionHsmCmdWB:: strlen(mk)=%d\n", (int)strlen(mk));
			return -1;
		}

		memcpy(hsmCmdBuf+hsmCmdLen, checkMk, 16);
		hsmCmdLen += 16;

	}

	// PAN
	UnionForm16BytesDisperseDataOfV41A(strlen(pan), pan, tmpPan);
	aschex_to_bcdhex(tmpPan, 16, tmpPan);
	memcpy(hsmCmdBuf+hsmCmdLen, tmpPan, 8);
	hsmCmdLen += 8;

	// ATC
	aschex_to_bcdhex(atc, 4, atcTmp);
	memcpy(hsmCmdBuf+hsmCmdLen, atcTmp, 2);
	hsmCmdLen += 2;

	// IV
	if (mode[0]=='0' )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, 32);
		hsmCmdLen += 32;
	}

	// ������������
	aschex_to_bcdhex(plainData, lenOfPlainData, plainDataTmp);
	ret = UnionPBOCEMVFormPlainDataBlockForSM4(lenOfPlainData/2, (unsigned char *)plainDataTmp, (unsigned char *)plainDataTmp);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWB:: UnionPBOCEMVFormPlainDataBlockForSM4!\n");
		return(ret);
	}

	// �������ݳ���
	sprintf(hsmCmdBuf+hsmCmdLen, "%04x", ret);
	hsmCmdLen += 4;

	// ��������
	memcpy(hsmCmdBuf+hsmCmdLen, plainDataTmp, ret);
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();
	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWB:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(tmpBuf, hsmCmdBuf+offset, 4);
	aschex_to_bcdhex(tmpBuf, 4, tmpBuf);
	*lenOfCipherData = tmpBuf[0]*256+tmpBuf[1];
	offset += 4;
	memcpy(cipherData, hsmCmdBuf+offset, *lenOfCipherData);
	return (*lenOfCipherData);
}


// ������ԿSM4��Կ
/* �������
mode:
0��������Կ
1��������Կ����ZMK�¼���
keyType		��Կ����
zmk			����ģʽΪ1ʱ����ʾ��ZMK��
�������
keyByLMK		LMK��������Կ����ֵ
keyByZMK		ZMK��������Կ����ֵ
checkValue		��ԿУ��ֵ
 */
int UnionHsmCmdWI(char *mode, TUnionDesKeyType keyType, char *zmk, char *keyByLMK,char *keyByZMK, char *checkValue)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		offset;

	memcpy(hsmCmdBuf,"WI",2);
	hsmCmdLen = 2;

	//ģʽ
	memcpy(hsmCmdBuf+hsmCmdLen,mode,1);
	hsmCmdLen += 1;

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWI:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if (mode[0] == '1')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zmk,32);
		hsmCmdLen += 32;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWI:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (strcmp(hsmCmdBuf+offset, "S") == 0)
	{
		UnionUserErrLog("in UnionHsmCmdWI:: Key LENGTH ERROR!\n");
		return(ret);
	}
	offset += 1;
	memcpy(keyByLMK,hsmCmdBuf+offset,32);
	keyByLMK[32] = 0;
	offset += 32;
	if (mode[0] == '1')
	{
		offset += 1;
		memcpy(keyByZMK,hsmCmdBuf+offset,32);
		keyByZMK[32] = 0;
		offset += 32;
	}
	else
	{
		// modify by leipp 20150414
		if ((keyByZMK != NULL) && (strlen(keyByZMK) > 0))
			keyByZMK[0] = 0;
	}

	memcpy(checkValue,hsmCmdBuf+offset,16);
	checkValue[16] = 0;

	return(0);
}


//����SM4��Կ����ӡһ���ɷ� 
/* �������
   keyType		��Կ����
   fldNumi		��ӡ�����
   fld		��ӡ��
   �������
   component	�ɷ�
 */
int UnionHsmCmdWJ(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component)
{
	int     ret;
	char    hsmCmdBuf[1024];
	int     hsmCmdLen = 0;
	int     i;

	memcpy(hsmCmdBuf,"WJ",2);
	hsmCmdLen = 2;
	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWJ:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// add by liwj 20150707
	if (gunionIsUseSpecZmkType)
	{
		if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
			memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
	}
	else
		gunionIsUseSpecZmkType = 1;
	// end
	hsmCmdLen += ret;
	//��ӡ��
	for (i = 0; i < fldNum; i++)
	{
		if (i > 0)
		{
			hsmCmdBuf[hsmCmdLen] = ';';
			hsmCmdLen++;
		}
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",fld[i]);
		hsmCmdLen += strlen(fld[i]);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWJ:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//��ȡ�ɷ�
	memcpy(component,hsmCmdBuf+4+1,32);
	component[32] = 0;
	return(32);
}

//����SM4��Կ���Էֿ��ĳɷ���ʽ��ӡ
/* �������          
   keyType         ��Կ����
   fldNumi         ��ӡ�����
   fld             ��ӡ��
   �������
   component       �ɷ�
   checkValue	��ԿУ��ֵ
 */              
int UnionHsmCmdWK(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component,char *checkValue)
{       
	int     ret;
	char    hsmCmdBuf[1024];
	int     hsmCmdLen = 0;
	int     i; 

	memcpy(hsmCmdBuf,"WK",2);
	hsmCmdLen = 2;
	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWK:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	//��ӡ��
	for (i = 0; i < fldNum; i++)
	{
		if (i > 0)
		{
			hsmCmdBuf[hsmCmdLen] = ';';
			hsmCmdLen++;
		} 
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",fld[i]);
		hsmCmdLen += strlen(fld[i]);
	}       
	hsmCmdBuf[hsmCmdLen] = 0;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdLen = 4+1;
	//��ȡ�ɷ�
	memcpy(component,hsmCmdBuf+hsmCmdLen,32);
	hsmCmdLen += 32;
	memcpy(checkValue,hsmCmdBuf+hsmCmdLen,16);
	return(16);
}


/*
   �������
   mode ����ģʽ��ʶ
   G=��ɢ��ԿSM4����
   H=��ɢ��ԿSM4����
   I=������ԿSM4����
   J=������ԿSM4����
   id ����ID
   01=ECB
   02=CBC
   03=CFB
   mkType ����Կ����
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkCheckValue ����ԿУ��ֵ
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   lenOfData ���ݳ���
   data ����
   �������
   criperDataLen ���ݳ���  
   criperData ����   
 */
int UnionHsmCmdWC(char *mode, char *id, char *mkType, char *mk, char *mkCheckValue, int mkIndex, int mkDvsNum, char *mkDvsData, char *proFactor, int lenOfData, char *data, int *criperDataLen, char *criperData)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	char    tmpBuf[32];
	int     offset = 0;
	char    szMkBuff[128];
	char    lenBuf[32];
	int     len;

	// �������
	memcpy(hsmCmdBuf,"WC",2);
	hsmCmdLen += 2;

	// ����ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 2);
	hsmCmdLen += 2;

	// ����Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		/*
		   memcpy(hsmCmdBuf + hsmCmdLen, szMkBuff, ret);
		   hsmCmdLen += ret;
		 */
		if (strlen(mk) == 32)
		{
			szMkBuff[0] = 'S';
			memcpy(&szMkBuff[1], mk, 32);
			//ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
			memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, 33);
			hsmCmdLen += 33;
		}
		else if(strlen(mk) == 33)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, mk, 33);
			hsmCmdLen += 33;
		}
		else
		{
			UnionUserErrLog("in UnionHsmCmdWC:: strlen(mk)=%d err\n", (int)strlen(mk));
			return -1;
		} 

		memcpy(hsmCmdBuf+hsmCmdLen, mkCheckValue, 16);
		hsmCmdLen += 16;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
	hsmCmdLen += mkDvsNum*16;

	// ��������
	if ('I' == mode[0] || 'J' == mode[0])
	{
		aschex_to_bcdhex(proFactor,strlen(proFactor),hsmCmdBuf + hsmCmdLen);	
		hsmCmdLen += strlen(proFactor)/2;

	}

	// ���ݳ���
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	// ����
	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;

	hsmCmdBuf[hsmCmdLen] = 0;
	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWC:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(tmpBuf,hsmCmdBuf + offset, 3);
	tmpBuf[3] = 0;
	len = atoi(tmpBuf) * 2;
	*criperDataLen = len;

	offset += 3;
	memcpy(criperData, hsmCmdBuf + offset, *criperDataLen);
	return(*criperDataLen);
}

/*
   �������ܣ�
   WH��ɢ��Ƭ��Կ���԰�ȫ���ķ�ʽ������PBOC3.0��
   ���������
securityMech: ��ȫ����(R��SM4���ܺ�MAC)
mode: ģʽ��־, 0-������ 1-���ܲ�����MAC
id: ����ID, 0=PBOC3.0ģʽ��CBCģʽ��ǿ�����X80�� 1= PBOC3.0ģʽ��������ָ����ECB�� 2=PBOC3.0ģʽ(ECBģʽ������������)

mk: ����Կ
mkType: ����Կ����109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
��ʹ�ü��ܻ�����Կʱ����������
mkIndex: ����Կ����, K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
mkCheckValue: ����ԿУ��ֵ
mkDvsNum: ����Կ��ɢ����, 1-3��
mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

pkType: ������Կ���� 0=TK(������Կ)1=DK-SMC(ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ)
pk: ������Կ
pkIndex: ������Կ����
pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������
pkCheckValue: ������ԿУ��ֵ

proKeyFlag:������Կ��ʶ��Y:���������Կ��N:�����������Կ����ѡ��:��û�и���ʱȱʡΪN
proFactor: ��������(32H),��ѡ��:����������Կ��־ΪYʱ��

ivCbc: IV-CBC,32H ����������ID��Ϊ0ʱ��

encryptFillDataLen: ����������ݳ���(4H),����������ID��Ϊ2ʱ�У�������1024��
����Կ����һ����м��ܵ����ݳ���
encryptFillData: ����������� nB ����������ID��Ϊ2ʱ��,����Կ����һ����м���
encryptFillOffset: �����������ƫ���� 4H ����������ID��Ϊ2ʱ��
����Կ���Ĳ��뵽����������ݵ�λ��, ��ֵ������0������������ݳ���֮��

ivMac: IV-MAC,32H ������ģʽ��־��Ϊ1ʱ��
macDataLen: MAC������ݳ��� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC��������ݳ���
macData: MAC������� nB ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC���������
macOffset: ƫ���� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ���Ĳ��뵽MAC������ݵ�λ��
��ֵ������0��MAC������ݳ���֮��
���������
mac: MACֵ 16H ������ģʽ��־��Ϊ1ʱ��
criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
criperData: nB �������������
checkValue: 16H У��ֵ

 */
int UnionHsmCmdWH(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex, char *mkCheckValue, int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex, int pkDvsNum, char *pkDvsData, char *pkCheckValue, char *proKeyFlag, char *proFactor, char *ivCbc, int encryptFillDataLen, char *encryptFillData, int encryptFillOffset, char *ivMac, int macDataLen, char *macData, int macOffset, char *mac, int *criperDataLen, char *criperData, char *checkValue)
{
	int     ret;
	int     lenOfDvsData = 16;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    szMkBuff[128];
	char    szPkBuff[128];


	// �������
	memcpy(hsmCmdBuf,"WH",2);
	hsmCmdLen += 2;

	// ��ȫ����
	memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
	hsmCmdLen += 1;
	if (securityMech[0] == 'Q')
		lenOfDvsData = 32;
	else
		lenOfDvsData = 16;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		szMkBuff[0] = 'S';
		memcpy(&szMkBuff[1], mk, 32);	
		memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, 33);
		hsmCmdLen += 33;

		memcpy(hsmCmdBuf+hsmCmdLen, mkCheckValue, 16);
		hsmCmdLen += 16;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 1);
	hsmCmdLen += 1;

	// ������Կ
	if ((pk == NULL) || (strlen(pk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", pkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(pk) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, pk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		szPkBuff[0] = 'S';
		memcpy(&szPkBuff[1], pk, 32);
		memcpy(hsmCmdBuf+hsmCmdLen, szPkBuff, 33);
		hsmCmdLen += 33;

		memcpy(hsmCmdBuf+hsmCmdLen, pkCheckValue, 16);
		hsmCmdLen += 16;
	}
	// ������Կ��ɢ����, ������Կ��ɢ����
	if (pkType[0] == '1')
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
		hsmCmdLen += pkDvsNum*lenOfDvsData;
	}

	// modify by lix,20090730
	// ������Կ��ʶ
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// ��������
		if (proKeyFlag[0] == 'Y')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, proFactor, 32);
			hsmCmdLen += 32;
		}
	}
	// modify end;

	// IV-CBC
	if (id[0] == '0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 32);
		hsmCmdLen += 32;
	}

	// ����������ݳ��ȡ�����������ݡ������������ƫ����
	if (id[0] == '2')
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, encryptFillData, encryptFillDataLen);
		hsmCmdLen += encryptFillDataLen;

		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillOffset/2);
		hsmCmdLen += 4;
	}

	// IV-MAC, MAC������ݳ���, MAC�������, ƫ����
	if (mode[0] == '1')
	{
		if (strlen(ivMac) > 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 32);
			hsmCmdLen += 32;
		}

		if (macDataLen > 8000)
		{
			UnionUserErrLog("in UnionHsmCmdWH::macDataLen[%d] too large\n", macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, macData, macDataLen);
		hsmCmdLen += macDataLen;

		if ((macOffset < 0) || (macOffset > macDataLen))
		{
			UnionUserErrLog("in UnionHsmCmdWH::macOffset[%d] <0 or >macDataLen[%d]\n", macOffset, macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macOffset/2);
		hsmCmdLen += 4;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWH:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (mode[0] == '1') // MACֵ
	{
		memcpy(mac, hsmCmdBuf+offset, 16);
		mac[16] = 0;
		offset += 16;
	}
	else if (mode[0] == '2') //ģʽ2,����У��ֵ
	{
		offset += 32;
		memcpy(checkValue, hsmCmdBuf + ret - 32, 16);
		checkValue[16] = 0;
	}

	// �������ݳ���
	offset += 4;
	*criperDataLen = ret - offset;

	// ��������
	//memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
	// add by wuhy at 20140826
	if (mode[0] == '1')
	{
		// ��������
		memcpy(criperData, hsmCmdBuf + 24, *criperDataLen);
		criperData[*criperDataLen] = 0;
		UnionLog("in UnionHsmCmdWH::*criperDataLen=[%d]\n",*criperDataLen);
	}
	else
	{
		// ��������
		memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
		criperData[*criperDataLen] = 0;
		UnionLog("in UnionHsmCmdWH::*criperDataLen=[%d]\n",*criperDataLen);
	}
	// end add

	return(0);
}
/*
   �������ܣ�
   WD:: ���㼰У��MAC/TAC��ԭUB��
   �������
   mode ģʽ��ʶ 1=����MAC 2=У��MAC
   id ����ID
   4=SM4MAC��ʹ��2����������Կ����SM4��
   mkType ����Կ���� 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkCheckValue ����ԿУ��ֵ 
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   macFillFlagMAC ��������ʶ
   1=ǿ�����0X80
   0=��ǿ�����0X80
   ivMac IV-MAC 
   lenOfData MAC�������ݳ���
   data MAC��������
   macFlagMAC ���ȱ�ʶ   
   1=4 BYTE
   2=8 BYTE
   �������
   checkMac
   mac MACֵ
   ����ֵ��
   �ɹ���>=0

 */

int UnionHsmCmdWD(char *mode, char *id, char *mkType,char *mk, char* mkCheckValue, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, char *checkMac, char *mac)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    szMkBuff[64];
	char    lenBuf[32];
	char    proBuf[32];

	// �������
	memcpy(hsmCmdBuf,"WD",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf + hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ����
	memcpy(hsmCmdBuf + hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		if(strlen(mk) == 32)
		{
			szMkBuff[0] = 'S';
			memcpy(&szMkBuff[1], mk, 32);			
			//ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
			memcpy(hsmCmdBuf + hsmCmdLen, szMkBuff, 33);
			hsmCmdLen += 33;
		}
		else if(strlen(mk) == 33)
		{
			memcpy(hsmCmdBuf + hsmCmdLen, mk, 33);
			hsmCmdLen += 33;
		}
		else
		{
			UnionUserErrLog("in UnionHsmCmdWD:: strlen(mk)=%d err\n", (int)strlen(mk));
			return -1;
		}

		//����ԿУ��ֵ
		memcpy(hsmCmdBuf + hsmCmdLen, mkCheckValue, 16);
		hsmCmdLen += 16;	
	}


	// ��ɢ����
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
	hsmCmdLen += mkDvsNum*16;

	// �������� 
	if(id[0] != '1') // ����ID��1û�и��� 2016-05-06
	{
		if (strlen(proFactor) == 4)
		{
			memset(proBuf,'0',sizeof(proBuf));
			memcpy(proBuf+12, proFactor, 4);
			memcpy(hsmCmdBuf + hsmCmdLen, proBuf, 16);
		}
		else
		{
			memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 16);
		}
		hsmCmdLen += 16;
	}

	// MAC��������ʶ
	memcpy(hsmCmdBuf + hsmCmdLen, macFillFlag, 1);
	hsmCmdLen += 1;

	// IV-MAC
	memcpy(hsmCmdBuf+ hsmCmdLen, ivMac, 32);	
	hsmCmdLen += 32;

	// MAC�������ݳ���
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	//MAC��������
	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;

	//MAC���ȱ�ʶ
	if (macFlag[0] != '1' && macFlag[0] !='2')
	{
		UnionUserErrLog("in UnionHsmCmdWD::macFlag is error!macFlag = [%s]\n",macFlag);
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf + hsmCmdLen, macFlag, 1);
	hsmCmdLen += 1;

	//��У���MACֵ
	if ('2' == mode[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, checkMac, 16*atoi(macFlag));
		hsmCmdLen += 16*atoi(macFlag);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWD:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;

	if (mode[0] == '1')
	{
		memcpy(mac,hsmCmdBuf + offset, 16*atoi(macFlag));
		mac[16*atoi(macFlag)] = 0;
		return(16*atoi(macFlag));
	}

	return(0);
}


/*
   ��������:
   WE::  ������ɢ/������Կ
   �������:
   mode �㷨��־:
   "01" - SM4_CBC
   "02" - SM4_ECB
   masterKeyType   ������Կ����
   masterKey       ������Կ
   desKeyType      ����Կ����
   keyLengthFlag   ����ɢ����Կ����   1 = ˫������Կ
   data            ��ɢ����
   masterKey      	������Կ 
   masterKeyIndex  K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
   masterKeyCheckValue ������ԿУ��ֵ
   iv              ����
   �������:
   key     ����Կ����
   ����ֵ��
   �ɹ���>=0

 */
int UnionHsmCmdWE(char *mode, TUnionDesKeyType masterKeyType, char *masterKey, char* masterKeyCheckValue, int masterKeyIndex, TUnionDesKeyType desKeyType, char *keyLengthFlag,char *data, char *iv, char *key)
{
	int	ret;
	char	hsmCmdBuf[2048];
	char	keyType[32];
	char    szMkBuff[64];

	int	hsmCmdLen = 0;

	if ((mode == NULL) || (data == NULL) || (masterKey == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdWE:: parameters err!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();
	//������
	memcpy(hsmCmdBuf, "WE", 2);
	hsmCmdLen = 2;

	// ģʽ
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 2);
	hsmCmdLen += 2;

	// ������Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(masterKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// ����Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(desKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// ��Կ����
	memcpy(hsmCmdBuf + hsmCmdLen, keyLengthFlag, 1);
	hsmCmdLen += 1;

	// ��ɢ���ݳ���
	sprintf(hsmCmdBuf + hsmCmdLen,"%03d", (int)strlen(data)/2);
	hsmCmdLen += 3;

	// ��ɢ����
	aschex_to_bcdhex(data,strlen(data),hsmCmdBuf + hsmCmdLen);	
	hsmCmdLen += strlen(data)/2;

	// ����Կ
	if (masterKey == NULL || strlen(masterKey) == 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", masterKeyIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(masterKey) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, masterKey, 33);
		hsmCmdLen += 4;
	}
	else
	{
		if(strlen(masterKey) == 32)
		{
			szMkBuff[0]='S';
			memcpy(szMkBuff+1, masterKey, 32);
			memcpy(hsmCmdBuf + hsmCmdLen, szMkBuff, 33);
			hsmCmdLen += 33;
		}	
		else if(strlen(masterKey) == 33)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, masterKey, 33);
			hsmCmdLen += 33;
		}
		else
		{
			UnionUserErrLog("in UnionHsmCmdWE:: strlen(masterKey)=%d err\n", (int)strlen(masterKey));
			return -1;
		}	

		//����ԿУ��ֵ 
		memcpy(hsmCmdBuf + hsmCmdLen, masterKeyCheckValue, 16);
		hsmCmdLen += 16;	
	}

	//iv 
	if ((iv != NULL) && (strlen(iv) != 0))
	{
		memcpy(hsmCmdBuf + hsmCmdLen, iv, 32);
		hsmCmdLen += 32;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWE:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(key, hsmCmdBuf + 4, ret - 4);

	return(ret - 4);
}

/*
   ��������:
   WF::  ��һ����Կ���ܵ�����ת��Ϊ��һ����Կ����
   �������:
   algType	�㷨ת������
01:3DES�㷨תSM4
02:SM4�㷨ת3DES
03:SM4�㷨תSM4
srcKeyType             Դ��Կ������
srcKey                 Դ��Կ
srcKeyCheckValue       Դ��ԿУ��ֵ
srcModel               Դ�����㷨
"00"=ECB
"01"=ECB_LP
"02"=ECB_P
"10"=CBC
"11"=CBC_LP
"12"=CBC_P
dstKeyType                            Ŀ����Կ����
dstKey    1A+3H/16H/32H/48H       	Ŀ����Կ
dstKeyCheckValue              		Ŀ����ԿУ��ֵ 
dstModel                                Ŀ�ļ����㷨
"00"=ECB
"02"=ECB_P
"10"=CBC
"12"=ECB_P
lenOfSrcCiphertext                    Դ��Կ���ܵ��������ݳ���
srcCiphertext   nB                    Դ��Կ���ܵ���������
iv_cbc    16H                ��ʼ������Դ�����㷨ΪCBC����ʱ���������
iv_cbcDstKey 16H                     Ŀ�ļ����㷨λcbc�����ʱ�������Ԅ1�7

�������:
dstCiphertext   nB                    Ŀ����Կ���ܵ���������
����ֵ��
�ɹ���>=0

 */
int UnionHsmCmdWF(char *algType, TUnionDesKeyType srcKeyType,char *srcKey, char* srcKeyCheckValue, int srcKeyIdx,char *srcModel, TUnionDesKeyType dstKeyType,char *dstKey, char* dstKeyCheckValue, int dstKeyIdx,char *dstModel,int lenOfSrcCiphertext,char *srcCiphertext, char *iv_cbc, char *iv_cbcDstKey, char *dstCiphertext)
{
	int				ret;
	int				keyLen = 0;
	char				hsmCmdBuf[2048];
	int				hsmCmdLen;
	char				tmpBuf[32];
	int				lenOfDstCiphertext;
	char				keyType[32];
	char 				keyTmp[1024];

	ret			= 		-1;
	lenOfDstCiphertext      =	  	0;
	hsmCmdLen	  	=	  	0;

	if ((srcModel == NULL) || (dstModel == NULL) || (srcCiphertext == NULL) || (dstCiphertext == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdWF:: parameters err!\n");
		return(errCodeParameter);
	}
	UnionSetBCDPrintTypeForHSMCmd();

	//������
	memcpy(hsmCmdBuf, "WF", 2);
	hsmCmdLen += 2;

	//�㷨ת������	
	memcpy(hsmCmdBuf+hsmCmdLen, algType, 2);
	hsmCmdLen += 2;

	//Դ��Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(srcKeyType, keyType);
	strncpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	//Դ��Կ
	if(srcKey == NULL || strlen(srcKey) <= 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X",srcKeyIdx);
		hsmCmdLen += 4;
	}
	else if(strlen(srcKey) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, srcKey, 4);
		hsmCmdLen += 4;
	}
	else
	{
		keyLen = strlen(srcKey);
		switch(keyLen)	
		{
			case 16:	
				memcpy(hsmCmdBuf+hsmCmdLen, srcKey, 16);
				hsmCmdLen += 16;
				break;
			case 32:
				if (strcmp(algType, "01") == 0)
				{
					keyTmp[0] = 'X';
				}
				else
				{
					keyTmp[0] = 'S';
				}
				memcpy(keyTmp + 1, srcKey, 32);
				memcpy(hsmCmdBuf+hsmCmdLen, keyTmp, 33);
				hsmCmdLen += 33;
				break;
			case 33:
				memcpy(hsmCmdBuf+hsmCmdLen, srcKey, 33);
				hsmCmdLen += 33;
				break;
			case 48:
				keyTmp[0] = 'Y';
				memcpy(keyTmp + 1, srcKey, 48);
				memcpy(hsmCmdBuf+hsmCmdLen, keyTmp, 49);
				hsmCmdLen += 49;
				break;
			case 49:
				memcpy(hsmCmdBuf+hsmCmdLen, srcKey, 49);
				hsmCmdLen += 49;
				break;
			default:
				UnionUserErrLog("in UnionHsmCmdWF:: srcKey len err! len=[%d]!\n", keyLen);
				break;

		}

		//Դ��ԿУ��ֵ 
		if (strcmp(algType, "01") != 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, srcKeyCheckValue, 16);
			hsmCmdLen += 16;
		}	
	}

	//Դ����ģʽ
	memcpy(hsmCmdBuf+hsmCmdLen,srcModel,2);
	hsmCmdLen += 2;

	//Ŀ����Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(dstKeyType, keyType);
	strncpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	//Ŀ����Կ
	if (dstKey == NULL || strlen(dstKey) <= 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X",dstKeyIdx);
		hsmCmdLen += 4;
	}
	else if(strlen(dstKey) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, dstKey, 4);
		hsmCmdLen += 4;
	}
	else
	{
		keyLen = strlen(dstKey);

		switch(keyLen)	
		{
			case 16:	
				memcpy(hsmCmdBuf+hsmCmdLen, dstKey, 16);
				hsmCmdLen += 16;
				break;
			case 32:
				if (strcmp(algType, "02") == 0)
				{
					keyTmp[0] = 'X';
				}
				else
				{
					keyTmp[0] = 'S';
				}
				memcpy(keyTmp + 1, dstKey, 32);
				memcpy(hsmCmdBuf+hsmCmdLen, keyTmp, 33);
				hsmCmdLen += 33;
				break;
			case 33:
				memcpy(hsmCmdBuf+hsmCmdLen, dstKey, 33);
				hsmCmdLen += 33;
				break;
			case 48:
				keyTmp[0] = 'Y';
				memcpy(keyTmp + 1, dstKey, 48);
				memcpy(hsmCmdBuf+hsmCmdLen, keyTmp, 49);
				hsmCmdLen += 49;
				break;
			case 49:
				memcpy(hsmCmdBuf+hsmCmdLen, dstKey, 49);
				hsmCmdLen += 49;
				break;
			default:
				UnionUserErrLog("in UnionHsmCmdWF:: dstKey len err! len=[%d]!\n", keyLen);
				break;

		}

		//Ŀ����ԿУ��ֵ
		if (strcmp(algType, "02") != 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, dstKeyCheckValue, 16);
			hsmCmdLen += 16;
		}
	}

	//Ŀ�ļ����㷨
	memcpy(hsmCmdBuf+hsmCmdLen,dstModel,2);
	hsmCmdLen += 2;

	//�������ݳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfSrcCiphertext/2);
	hsmCmdLen += 4;

	//��������
	aschex_to_bcdhex(srcCiphertext,lenOfSrcCiphertext,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfSrcCiphertext/2;

	//iv_cbc srckey
	if ((strncmp(srcModel, "10", 2)==0) || (strncmp(srcModel,"11",2) == 0) || (strncmp(srcModel,"12",2) == 0))
	{
		if (iv_cbc == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdWF:: parameters err!\n");
			return(errCodeParameter);
		}
		// modified by lizh at 20151130
		if (strcmp(algType, "01") == 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,iv_cbc,16);
			hsmCmdLen += 16;
		}
		else
		{
			memcpy(hsmCmdBuf+hsmCmdLen,iv_cbc,32);
			hsmCmdLen += 32;
		}
		// modified end
	}

	//iv_cbc dstkey
	if ((strncmp(dstModel,"10",2) == 0)
			|| (strncmp(dstModel,"12",2) == 0))
	{
		if (iv_cbcDstKey == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdWF:: parameters err!\n");
			return(errCodeParameter);
		}
		// modified by lizh at 20151130
		if (strcmp(algType, "02") == 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,iv_cbcDstKey,16);
			hsmCmdLen += 16;
		}
		else
		{
			memcpy(hsmCmdBuf+hsmCmdLen,iv_cbcDstKey,32);
			hsmCmdLen += 32;
		}
		// modified end
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	/* �������ͨѶ */
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWF:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(tmpBuf,hsmCmdBuf+4,4);
	tmpBuf[4] = 0;
	lenOfDstCiphertext = bcdhex_to_aschex(hsmCmdBuf+8,atoi(tmpBuf),dstCiphertext);

	return(lenOfDstCiphertext);
}

/*
   RQCУ��
   1=ִ��ARQCУ���ARPC����
   2=����ִ��ARPC����
   id        ����ID   0=PBOC3.0(����SM4�㷨����)
   mkIndex           mk���������mkֵΪ����ʹ��������ʽ
   mk                ���ܼ�������Կ
   mkCheckValue      ���ܼ�������ԿУ��ֵ
   pan               ��ɢ��Ƭ��Կʹ�õ��ʺŻ����ʺ����к�
   atc               ʹ��ATC�����������Կ
   lenOfData         �������ݳ���
   data             Ҫ���ܵ�����
   ARQC             ARQC
   ARC              ARC
   �������:
   ARPC             ARPC
   ����ֵ��
   �ɹ���>=0
 */
int UnionHsmCmdWG(char *mode, char *id, int mkIndex, char *mk, char* mkCheckValue, int lenOfPan, char *pan, char *atc, int lenOfData, char *data, char *ARQC, char *ARC, char *ARPC)
{
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	char keyString[64];
	int offset = 0;
	char    ARPCBuf[64];

	// �������
	memcpy(hsmCmdBuf,"WG",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		if (strlen(mk) == 32)	
		{
			keyString[0] = 'S';	
			memcpy(&keyString[1], mk, 32);
			memcpy(hsmCmdBuf+hsmCmdLen, keyString, 33);
			hsmCmdLen += 33;
		}
		else if(strlen(mk) == 33)	
		{
			memcpy(hsmCmdBuf+hsmCmdLen, mk, 33);
			hsmCmdLen += 33;
		}
		else
		{
			UnionUserErrLog("in UnionHsmCmdWG:: strlen(mk) err! len=[%d]\n",(int)strlen(mk));
			return -1;
		}

		//����Կ У��ֵ
		memcpy(hsmCmdBuf+hsmCmdLen, mkCheckValue, 16);
		hsmCmdLen += 16;
	}

	//PAN/PAN ���к�
	aschex_to_bcdhex(pan, 16, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 8;

	//ATC
	aschex_to_bcdhex(atc, 4, hsmCmdBuf + hsmCmdLen);	
	hsmCmdLen += 2;

	// �������ݳ���
	if ( (mode[0]=='0') || (mode[0]=='1') )
	{
		// modified 2011-11-08
		//sprintf(hsmCmdBuf+hsmCmdLen, "%2x", lenOfData/2);
		sprintf(hsmCmdBuf+hsmCmdLen, "%02X", lenOfData/2);
		hsmCmdLen += 2;

		// ��������
		ret = aschex_to_bcdhex(data, lenOfData, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += ret;
		//�ָ���
		memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
		hsmCmdLen += 1;
	}

	// ARQC
	ret = aschex_to_bcdhex(ARQC, 16, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += ret;

	// ARC
	if ( (mode[0]=='1') || (mode[0]=='2')  )
	{
		ret = aschex_to_bcdhex(ARC, 4, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += ret;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWG:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if ((mode[0] == '1')||(mode[0] == '2')||(mode[0] == '3')||(mode[0] == '4')) // ARPC
	{
		memcpy(ARPCBuf,hsmCmdBuf + offset, 8);
		ARPCBuf[8] = 0;
		bcdhex_to_aschex(ARPCBuf, 8, ARPC);
		return 16;
	}
	return(0);
}

/*
   ���ܣ� 
   RSA˽Կ��������תZPK��������
   ����:   
   vkeyLen	˽Կ���� ˽Կ�洢�ڼ��ܻ��ڲ�ʱ����˽Կ����ȡ0
   vkey	˽Կֻ��Ϊ�ⲿ�������ΪHSM�ڰ�ȫ�洢������Կ��
   dataPaddingMode 	�1�7 00=����� 01=PKCS#1 v1.5 02=OAEP
   dataLength  �ӽ������ݳ���	
   data �ӽ�������
   keyType	ZMKTYPE=0;ZPKTYPE=1;TPKTYPE=2 
   Key  �ö�Ӧ��LMK��Կ�Խ���
   Specialalgorithm     ר���㷨��ʶ
   ���:
   pin ����
 */
int UnionHsmCmd70(int vkeyLen, char *vkey, int vkIndex, int dataPaddingMode, int dataLength, char* data,  int keyType, char* key, char* Specialalgorithm, char *pin)
{
	/*δ����*/
	char	hsmCmdBuf[8192];

	int	hsmCmdLen = 0;
	int	ret = 0;
	int	offset = 0;
	char	tmpBuf[32];
	int	lenOfPIN = 0;

	memcpy(hsmCmdBuf, "70", 2);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", vkeyLen);	
	hsmCmdLen += 4;

	if(vkeyLen == 0) //ʹ��˽Կ����
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);	
		hsmCmdLen += 2;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, vkey, vkeyLen);
		hsmCmdLen += vkeyLen;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", dataPaddingMode);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", dataLength/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(data, dataLength,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += dataLength/2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", keyType);
	hsmCmdLen += 1;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(key), key, hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd70:: UnionGenerateX917RacalKeyString [%s]\n", key);
		return(ret);
	}
	hsmCmdLen += ret;

	if(strlen(Specialalgorithm) != 0)//motify by yansq 20150410
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%s", Specialalgorithm);
		hsmCmdLen += 1;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd70:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	offset = 4;

	memcpy(tmpBuf,hsmCmdBuf+offset,4);
	tmpBuf[4] = 0;
	offset += 4;

	lenOfPIN = atoi(tmpBuf);
	memcpy(pin,hsmCmdBuf+offset,lenOfPIN);

	bcdhex_to_aschex(pin,lenOfPIN,tmpBuf);
	strcpy(pin,tmpBuf);
	pin[lenOfPIN*2] = 0;

	return (lenOfPIN*2);
}

/*
   ���ܣ�
   ��zpk���ܵ��ַ���������ת��Ϊ��һzpk���ܵ��ַ���������
   ����:
   srcAlgorithmID Դ��Կ�㷨��ʶ��0�������,1�������
   srcZPK ��ǰ����PIN���ZPK��LMK�ԣ�06-07���¼���
   dstAlgorithmID Ŀ����Կ�㷨��ʶ��0�������,1�������
   srcPin ԴZPK���ܵ��ַ���������
   srcFormat ԴPIN��ʽ	,01-ANSI9.8
			 09-ũ���и�ʽ
   dstFormat Ŀ��PIN��ʽ,01-ANSI9.8
			 09-ũ���и�ʽ
   srcPan Դ12λ��Ч���˺�
   dstPan Ŀ��12λ ��Ч���˺�
   ���:
   pin ����PIN���� 
 */
int UnionHsmCmdH8(int srcAlgorithmID,char* srcZPK,int dstAlgorithmID, char* dstZPK, char *srcPin,char *srcFormat,char *dstFormat, char* srcPan, char* dstPan, int* lenOfPin, char* pin)
{
	int	ret;
	char	hsmCmdBuf[1024];
	int	hsmCmdLen = 0;
	int 	offset = 0;	
	char 	tmpBuf[32];

	if ((srcZPK == NULL) &&	(dstZPK == NULL) && (srcPin == NULL) && (srcPin == NULL) && (dstPan == NULL))
	{
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "H8", 2);
	hsmCmdLen += 2;

	if (srcAlgorithmID == 0)
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(srcZPK), srcZPK, hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdH8:: UnionGenerateX917RacalKeyString [%s]\n", srcZPK);
			return(ret);
		}

		hsmCmdLen += ret;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);	
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,srcZPK,32);	
		hsmCmdLen += 32;
	}

	if (dstAlgorithmID == 0)
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(dstZPK), dstZPK, hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdH8:: UnionGenerateX917RacalKeyString [%s]\n", srcZPK);
			return(ret);
		}

		hsmCmdLen += ret;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);	
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,dstZPK,32);	
		hsmCmdLen += 32;
	}

	if ((srcFormat != NULL && strlen(srcFormat) > 0) && 
		(dstFormat != NULL && strlen(dstFormat) > 0))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "Y", 1);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen, srcFormat, 2);
		hsmCmdLen += 2;

		memcpy(hsmCmdBuf+hsmCmdLen, dstFormat, 2);
		hsmCmdLen += 2;
	}

	memcpy(hsmCmdBuf+hsmCmdLen, srcPin, strlen(srcPin));
	hsmCmdLen += strlen(srcPin);

	if ((ret = UnionForm12LenAccountNumber (srcPan, strlen(srcPan), hsmCmdBuf + hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdH8:: UnionForm12LenAccountNumber!\n");
		return (ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionForm12LenAccountNumber (dstPan, strlen(dstPan), hsmCmdBuf + hsmCmdLen)) < 0)
	{
		UnionUserErrLog ("in UnionHsmCmdH8:: UnionForm12LenAccountNumber!\n");
		return (ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	//UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd43:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;

	memcpy(tmpBuf, hsmCmdBuf + offset, 2);
	tmpBuf[2] = 0;

	*lenOfPin = atoi(tmpBuf);

	offset += 2;

	memcpy(pin, hsmCmdBuf+offset, ret-offset);
	pin[ret-offset] = 0;

	return (ret-offset);
}


/*
   ���ܣ��R8/R9���������

   �������: 
   genKeyLen 	ָ����������ĳ���
   genKeyType ����ĸ��Ӷ�		1�������� 2������ĸ 3������+��ĸ

   ���������
   outPutKey  �����Ŀ��� 

   ����ֵ��
   �����Ŀ���ĳ���
 */

int UnionHsmCmdR8(int genKeyLen,  char genKeyType, char* outPutkey)
{
	char  hsmCmdBuf[1024];	
	int hsmCmdLen = 0;
	int ret = 0;
	int offset = 0;

	memcpy(hsmCmdBuf, "R8", 2);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%03d", genKeyLen);
	hsmCmdLen += 3;

	hsmCmdBuf[hsmCmdLen] = genKeyType;
	hsmCmdLen += 1;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdR8:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;

	strncpy(outPutkey, hsmCmdBuf + offset, genKeyLen);	

	return(genKeyLen);
}

/*
   Mode    1       H       0��������Կ
   2��������Կ
   Key type        3       H       ��000��
   Key length      1       H       0 = ��������Կ
   1 = ˫������Կ
   Key Plain       16H/32H H       ������Կ��Mode��2ʱ��������ڡ�
 */
int UnionHsmCmdTA(int mode ,int keyLen,char *keyType,char *keyPlain,char *keybyLmk)
{
	int ret;
	int hsmCmdLen = 0;
	char hsmCmdBuf[8192];

	memcpy(hsmCmdBuf, "TA", 2);
	hsmCmdLen += 2;

	if (mode != 0 && mode != 2)
	{
		UnionUserErrLog("in RacalCmdTA:: parameters error!\n");
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;


	if (keyType == NULL)
		memcpy(hsmCmdBuf+hsmCmdLen,"000",3);
	else
		memcpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;
	if (keyLen == 16)
		memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	else if (keyLen == 32)
		memcpy(hsmCmdBuf+hsmCmdLen,"1",1);
	else
	{
		UnionUserErrLog("in RacalCmdTA:: parameters  key Length error!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 1;
	if(mode == 2)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,keyPlain,strlen(keyPlain));
		hsmCmdLen += strlen(keyPlain);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdTA:: UnionLongConnSJL06Cmd [%s]!\n",hsmCmdBuf);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	memcpy(keybyLmk, hsmCmdBuf + 4 , keyLen);
	keybyLmk[keyLen] = 0;
	return keyLen;
}

int UnionHsmCmdUI(
		char *mode, char *iv,
		int kekLen, char *Kek,char *keyType,
		int n ,
		int mLen, char *m,
		int eLen, char *e,
		int dLen, char *d,
		int pLen, char *p,
		int qLen, char *q,
		int dmp1Len, char *dmp1,
		int dmq1Len, char *dmq1,
		int coefLen, char *coef,
		unsigned char *keyByLmk,int *keyByLmkLen)
{
	//	UnionLog("UnionHsmCmdUI Para:mode=[%s]\niv=[%s]\nkekLen=[%d]\nkek=[%s]\nkeyType=[%s]\nn=[%d]\nm:[%d][%s]\ne:[%d][%s]\nd:[%d][%s]\np:[%d][%s]\nq:[%d][%s]\ndp:[%d][%s]\ndq:[%d][%s]\ncoef:[%d][%s]\n",mode,iv,kekLen,Kek,keyType,n,mLen,m,eLen,e,dLen,d,pLen,p,qLen,q,dmp1Len,dmp1,dmq1Len,dmq1,coefLen,coef);
	int ret;
	int hsmCmdLen = 0;
	char hsmCmdBuf[8192];
	char ascBuf[8192];
	int offset;
	char oDatalen[32];
	char nLen[32];

	if ( (mode == NULL) || (Kek == NULL) ||(kekLen <=0))
	{
		UnionUserErrLog("in UnionHsmCmdUI:: parameters error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "UI", 2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf + hsmCmdLen, mode , 2);
	hsmCmdLen += 2;

	if ( keyType == NULL )
		memcpy(hsmCmdBuf + hsmCmdLen, "000", 3);
	else
		memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);

	hsmCmdLen += 3;

	if (kekLen == 16)
		hsmCmdBuf[hsmCmdLen] = '0';
	else if(kekLen == 32)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
	{
		UnionUserErrLog("in UnionHsmCmdUI:: parameters error!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf + hsmCmdLen, Kek, kekLen);
	hsmCmdLen += kekLen;

	if ( (strncmp(mode, "12", 2) == 0)
			||(strncmp(mode, "10", 2) == 0)
			|| (strncmp(mode, "11", 2) == 0))
	{
		if (iv == NULL)
		{
			UnionUserErrLog("in RacalCmdUI:: parameters error!\n");
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf + hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	//˽Կ����
	offset = 0;

	sprintf(nLen, "%08X", n);
	//memcpy(ascBuf + offset,nLen,8);
	aschex_to_bcdhex(nLen,8,ascBuf);
	offset += 4;

	sprintf(ascBuf + offset, "%04d", mLen/2);
	offset += 4;
	aschex_to_bcdhex(m,mLen,ascBuf+offset);
	offset += mLen/2;

	sprintf(ascBuf + offset, "%04d", eLen/2);
	offset += 4;
	aschex_to_bcdhex(e,eLen,ascBuf+offset);
	offset += eLen/2;


	sprintf(ascBuf + offset, "%04d", dLen/2);
	offset += 4;
	aschex_to_bcdhex(d,dLen,ascBuf+offset);
	offset +=dLen/2;

	sprintf(ascBuf + offset, "%04d", pLen/2);
	offset += 4;
	aschex_to_bcdhex(p,pLen,ascBuf+offset);
	offset +=pLen/2;

	sprintf(ascBuf + offset, "%04d", qLen/2);
	offset += 4;
	aschex_to_bcdhex(q,qLen,ascBuf+offset);
	offset +=qLen/2;

	sprintf(ascBuf + offset, "%04d", dmp1Len/2);
	offset += 4;
	aschex_to_bcdhex(dmp1,dmp1Len,ascBuf+offset);
	offset +=dmp1Len/2;

	sprintf(ascBuf + offset, "%04d", dmq1Len/2);
	offset += 4;
	aschex_to_bcdhex(dmq1,dmq1Len,ascBuf+offset);
	offset +=dmq1Len/2;

	sprintf(ascBuf + offset, "%04d", coefLen/2);
	offset += 4;
	aschex_to_bcdhex(coef,coefLen,ascBuf+offset);
	offset += coefLen/2;

	memcpy(hsmCmdBuf+hsmCmdLen,ascBuf,offset);
	hsmCmdLen += offset;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdUI:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(oDatalen, hsmCmdBuf + 4, 4);
	oDatalen[4] = 0;
	*keyByLmkLen = atoi(oDatalen)*2;
	bcdhex_to_aschex(hsmCmdBuf+8,*keyByLmkLen/2,(char*)keyByLmk);
	keyByLmk[*keyByLmkLen] = 0;

	return *keyByLmkLen;
}

int UnionHsmCmdUI2(char *mode,char *keyType,int keyLen,char *keyValue,char *iv,char *vkByKek,int vkByKekLen,unsigned char *keyByLmk)
{
	int ret;
	int hsmCmdLen = 0;
	char hsmCmdBuf[8192];
	int offset;
	char oDatalen[32];

	if ( (mode == NULL) || (keyValue == NULL) ||(keyLen <=0))
	{
		UnionUserErrLog("in UnionHsmCmdUI2:: parameters error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "UI", 2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf + hsmCmdLen, mode , 2);
	hsmCmdLen += 2;

	if ( keyType == NULL )
		memcpy(hsmCmdBuf + hsmCmdLen, "000", 3);
	else
		memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);

	hsmCmdLen += 3;

	if (keyLen == 16)
		hsmCmdBuf[hsmCmdLen] = '0';
	else if(keyLen == 32)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
	{
		UnionUserErrLog("in UnionHsmCmdUI2:: parameters error!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf + hsmCmdLen, keyValue, keyLen);
	hsmCmdLen += keyLen;

	if ( (strncmp(mode, "12", 2) == 0)
			||(strncmp(mode, "10", 2) == 0)
			|| (strncmp(mode, "11", 2) == 0))
	{
		if (iv == NULL)
		{
			UnionUserErrLog("in RacalCmdUI2:: parameters error!\n");
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf + hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	memcpy(hsmCmdBuf+hsmCmdLen,vkByKek,vkByKekLen);
	hsmCmdLen += vkByKekLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdUI2:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(oDatalen, hsmCmdBuf + 4, 4);
	oDatalen[4] = 0;
	offset = atoi(oDatalen)*2;
	bcdhex_to_aschex(hsmCmdBuf+8,offset/2,(char*)keyByLmk);
	keyByLmk[offset] = 0;

	return offset;
}

int UnionHsmCmdTY(int vkIdx,int keyLen,unsigned char *vkByLmk )
{
	int ret;
	int hsmCmdLen = 0;
	char hsmCmdBuf[8192];

	if ( vkIdx < 0 )
	{
		UnionUserErrLog("in RacalCmdTY:: parameters error! vkIdx [%d]\n",vkIdx);
		return(errCodeParameter);
	}

	if ((vkByLmk == NULL) ||(keyLen <=0))
	{
		UnionUserErrLog("in RacalCmdTY:: parameters error!\n");
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf, "TY", 2);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"G%03d",vkIdx);
	hsmCmdLen += 4;

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",keyLen);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf + hsmCmdLen, vkByLmk , keyLen);
	hsmCmdLen += keyLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdTY:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	return 0;
}



/*
   ���� F8:�1�78/F9)��������������Ӳ�ʹ���ض���Կ������

   ����	��
   mk		����Կ	32H/1A+32H/1A+3H	
   mkIndex 	����Կ����
   mkType	�1�7	��Կ����	3H	�����ܲ�ʹ������ʱ���ڴ������롣����֧��00A, 309�е�һ��
   mkDvsNum	������Կ��ɢ����	1N	0~2��
   mkDvsData1	��ɢ����1	32H	��������ɢ����������0ʱ����
   mkDvsData2	��ɢ����2	32H	��������ɢ����������1ʱ����
   seedLen		���ӳ���	2N	Ĭ��Ϊ16��С�ڻ����16

   ���	:
   seedKey	��������	32H	�ɸ���Կ��ɢ��ı�����Կ�������
   seedKeyCheckValue ����У��ֵ	16H	��ԿУ��ֵ������SM4�㷨��

 */

int UnionHsmCmdF8(char *mk, int mkIndex, char*mkType, int mkDvsNum, char* mkDvsData1, char* mkDvsData2 ,int seedLen,int seedAlgorithm, char *seedKey, char *seedKeyCheckValue)
{
	/*δ����*/
	char hsmCmdBuf[1024];

	int hsmCmdLen = 0 ;
	int ret = 0;
	int offset = 0;
	int newSeedLen = 0;
	char seedLenBuf[32];

	if ((mkDvsNum < 0) || (mkDvsNum > 2))
	{
		UnionUserErrLog("in UnionHsmCmdF8:: mkDvsNum=[%d]\n", mkDvsNum);
		return(errCodeParameter);
	}

	if ((seedLen  < 0) || (seedLen > 32))
	{
		UnionUserErrLog("in UnionHsmCmdF8:: seedLen=[%d]\n", seedLen);
		return(errCodeParameter);
	}

	if (seedAlgorithm < 0 || seedAlgorithm > 9)
	{
		UnionUserErrLog("in UnionHsmCmdF8::seedAlgorithm=[%d]\n", seedAlgorithm);
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, "F8", 2);
	hsmCmdLen += 2;

	if ((mk == NULL) || (strlen(mk) == 0))	
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03d", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		strcpy(hsmCmdBuf+hsmCmdLen, mk);
		hsmCmdLen += strlen(mk);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);	
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	if((mkDvsNum == 1) && (mkDvsData1 != NULL))
	{
		strlen(mkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", mkDvsData1, 32-(int)strlen(mkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", mkDvsData1));

		hsmCmdLen += 32;
	}

	if((mkDvsNum == 2) && (mkDvsData1 != NULL) && (mkDvsData2 != NULL))
	{
		strlen(mkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", mkDvsData1, 32-(int)strlen(mkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", mkDvsData1));

		hsmCmdLen += 32;

		strlen(mkDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", mkDvsData2, 32-(int)strlen(mkDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", mkDvsData2));

		hsmCmdLen += 32;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", seedLen);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%1d", seedAlgorithm);
	hsmCmdLen += 1;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdF8:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;	

	memcpy(seedLenBuf, hsmCmdBuf+offset, 4);
	seedLenBuf[4] = 0;
	newSeedLen = atoi(seedLenBuf);

	offset+=4;

	memcpy(seedKey, hsmCmdBuf+offset, newSeedLen*2);	

	offset += newSeedLen*2;

	memcpy(seedKeyCheckValue, hsmCmdBuf+offset, 16);	

	return 0;
}

/*
   ����:	F9/F:)ͬһ������ɢ��Կ�������ݵ�ת����

����:
mk	����Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ���������Ϊ�����������Կ������
mkType	����Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��109��309�е�һ��
srcDvsNum		Դ������Կ��ɢ����	1N	0~2��
srcDvsData1	Դ��ɢ����1	32H	������Դ��ɢ����������0ʱ����
srcDvsData2	Դ��ɢ����2	32H	������Դ��ɢ����������1ʱ����
srcDataPendType	Դ��������ʽ	1N	1�����0x0��2��0x80��3��ǿ��0x80��4��PKCS#5
dstDvsNum	Ŀ�걣����Կ��ɢ����	1N	0~2��
dstDvsData1�1�7	����ɢ����1	32H	������Ŀ����ɢ����������0ʱ����
dstDvsData1�1�7	Ŀ����ɢ����2	32H	������Ŀ����ɢ����������1ʱ����
dstDataPendType	Ŀ����������ʽ	1N	1�����0x0��2��0x80��3��ǿ��0x80��4��PKCS#5
cipherDataLen	���ĳ���	4N	Դ��ɢ��Կ���ܵ����ĳ���
cipherText	Դ����	n*2 H	Դ��ɢ��Կ���ܵ�����

���:
dstCipherLen	���ĳ���	4N	Ŀ����ɢ��Կ���ܵ����ĳ���
dstCipherText	Դ����	n*2 H	Ŀ����ɢ��Կ���ܵ�����
 */
int UnionHsmCmdF9(char *mk, char* mkType, int mkIndex, int srcDvsNum, char* srcDvsData1, char* srcDvsData2, int srcDataPendType, int dstDvsNum, char* dstDvsData1, char* dstDvsData2, int dstDataPendType, int cipherLen, char* cipherText, int* dstCipherLen, char* dstCipherText)
{
	char hsmCmdBuf[1024];
	char dstCipherLenTmp[32];

	int hsmCmdLen = 0;
	int ret = 0;
	int offset = 0;

	if ((srcDvsNum>2) || (srcDvsNum<0) || (dstDvsNum < 0) || (dstDvsNum > 2))
	{
		UnionUserErrLog("in UnionHsmCmdF9:: srcDvsNum=[%d], dstDvsNum[%d]\n", srcDvsNum, dstDvsNum);	
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "F9", 2);
	hsmCmdLen += 2;

	if ((mk == NULL) || (strlen(mk) == 0))	
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03d", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		strcpy(hsmCmdBuf+hsmCmdLen, mk);
		hsmCmdLen += strlen(mk);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", srcDvsNum);	
	hsmCmdLen += 1;

	if ((srcDvsNum == 1) && (srcDvsData1 != NULL))
	{
		strlen(srcDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", srcDvsData1, 32-(int)strlen(srcDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", srcDvsData1));

		hsmCmdLen += 32;
	}

	if ((srcDvsNum == 2) && (srcDvsData1 != NULL) && (srcDvsData2 != NULL))
	{
		strlen(srcDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", srcDvsData1, 32-(int)strlen(srcDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", srcDvsData1));


		hsmCmdLen += 32;

		strlen(srcDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", srcDvsData2, 32-(int)strlen(srcDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", srcDvsData2));

		hsmCmdLen += 32;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", srcDataPendType);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", dstDvsNum);
	hsmCmdLen += 1;

	if ((dstDvsNum == 1) && (dstDvsData1 != NULL))
	{
		strlen(dstDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", dstDvsData1, 32-(int)strlen(dstDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", dstDvsData1));

		hsmCmdLen += 32;
	}

	if ((dstDvsNum == 2) && (dstDvsData1 != NULL) && (dstDvsData2 != NULL))
	{
		strlen(dstDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", dstDvsData1, 32-(int)strlen(dstDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", dstDvsData1));

		hsmCmdLen += 32;

		strlen(dstDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", dstDvsData2, 32-(int)strlen(dstDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", dstDvsData2));

		hsmCmdLen += 32;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", dstDataPendType);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", cipherLen);	
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf+hsmCmdLen,  cipherText, 2*cipherLen);
	hsmCmdLen += 2*cipherLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdF9:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;	

	memcpy(dstCipherLenTmp, hsmCmdBuf+offset, 4);
	dstCipherLenTmp[4] = 0;
	offset += 4;
	*dstCipherLen = atoi(dstCipherLenTmp);

	memcpy(dstCipherText, hsmCmdBuf+offset, (*dstCipherLen)*2);	

	return 0;
}

/*
   ����:	FB:FC)���ɶ�̬��

   ����
   mk	��Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ���������Ϊ�����������Կ������
   mkType ��Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��109��309�е�һ��
   pkDvsNum ������Կ��ɢ����	1N	0~2��
   pkDvsData1 ��ɢ����1	32H	��������ɢ����������0ʱ����
   pkDvsData2 ��ɢ����2	32H	��������ɢ����������1ʱ����

   seedLen    ������Կ���ĳ���	4N	16��������
   seedKeyCiper ������Կ����	32H	�ɸ���Կ��ɢ�����Կ����
   algorithm 1��SM3�㷨
   otpKeyLen ��̬���볤��	2N	Ĭ��Ϊ06
   challengeValLen	��սֵ����	2N	00~~99
   challengeValue ��սֵ nA
   timeChangeCycle ʱ�ӱ仯���� 
   ���
   otpKey	OTP��̬����	6N	6λOTP��̬����
 */

int UnionHsmCmdFB(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int seedLen, char* seedKeyCiper, int otpKeyLen,int algorithm,  int challengeValLen, char* challengeValue, int timeChangeCycle, char* otpKey)
{
	char hsmCmdBuf[1024];

	int hsmCmdLen = 0;
	int ret = 0;
	int offset = 0;

	if ((pkDvsNum < 0) || (pkDvsNum > 2))
	{
		UnionUserErrLog("in unionHsmCmdFB:: pkDvsNum[%d]\n", pkDvsNum);	
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "FB", 2);
	hsmCmdLen += 2;

	if ((mk == NULL) || (strlen(mk) == 0))	
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03d", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		strcpy(hsmCmdBuf+hsmCmdLen, mk);
		hsmCmdLen += strlen(mk);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);	
	hsmCmdLen += 1;

	if ((pkDvsNum == 1) && (pkDvsData1 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;
	}

	if ((pkDvsNum == 2) && (pkDvsData1 != NULL) && (pkDvsData2 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;

		strlen(pkDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData2, 32-(int)strlen(pkDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData2));

		hsmCmdLen += 32;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", seedLen);
	hsmCmdLen += 4;

	strcpy(hsmCmdBuf+hsmCmdLen, seedKeyCiper);
	hsmCmdLen += strlen(seedKeyCiper);

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", algorithm);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", otpKeyLen);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", challengeValLen);
	hsmCmdLen += 2;

	if((challengeValLen!=0) && (challengeValue != NULL))	
	{
		memcpy(hsmCmdBuf+hsmCmdLen, challengeValue, challengeValLen);
		hsmCmdLen += challengeValLen;
	}

	if(timeChangeCycle > 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%03d", timeChangeCycle);
		hsmCmdLen += 3;

		sprintf(hsmCmdBuf+hsmCmdLen, "%016u", (unsigned int)time(0));	
		hsmCmdLen += 16;
	}
	else	
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%03d", 0);
		hsmCmdLen += 3;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFB:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;	

	memcpy(otpKey, hsmCmdBuf+offset, otpKeyLen);	

	return 0;
}

/*
   �������	2	ֵ��D��
   ����Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ��K+�����ţ��������Ϊ�����������Կ������
   ����Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��309�е�һ��
   ������Կ��ɢ����	1N	1~2��
   ��ɢ����1	32H	
   ��ɢ����2	32H	��������ɢ����������1ʱ����
   ������Կ����	32H	�ɸ���Կ��ɢ�����Կ����
   ���ɶ�̬�����㷨	1N	1��SM3�㷨��2��SM4�㷨
   ��̬�����	2N	Ĭ��Ϊ06
   ��սֵ����	2N	00~~99
   ��սֵ	nA	
   ʱ��ֵT0	16N	
   ʱ�ӱ仯����Tc	3N	��Ϊ��λ��1����Ϊ��060��1�룺001
   ʱ�Ӵ���	2N	��n���ٸ�ʱ����������֤����
   ����֤��̬����	nN	

   ��Ӧ����	2	ֵ��E��
   �������	2	
   ʱ��ƫ�Ʒ���	1A	��ǰƫ�ƣ�+
   ���ƫ�ƣ�-
   ʱ��ƫ��ֵ	2N	ƫ���������ڣ�02
 */

int UnionHsmCmdFD(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int seedLen, char* seedKeyCiper, int algorithm, int otpKeyLen, int challengeValLen, char* challengeValue,int timeChangeCycle, int timeValue,int timeWindow, char* otpKey, char* clockSkew,  int* clockSkewValue) 
{
	/*δ����*/
	char hsmCmdBuf[1024];
	char tmpBuf[128];

	int hsmCmdLen = 0;
	int ret = 0;
	int offset = 0;

	if ((pkDvsNum < 0) && (pkDvsNum > 2))
	{
		UnionUserErrLog("in UnionHsmCmdFD:: pkDvsNum[%d]\n", pkDvsNum);	
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "FD", 2);
	hsmCmdLen += 2;

	if ((mk == NULL) || (strlen(mk) == 0))	
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03d", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		strcpy(hsmCmdBuf+hsmCmdLen, mk);
		hsmCmdLen += strlen(mk);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);	
	hsmCmdLen += 1;

	if ((pkDvsNum == 1) && (pkDvsData1 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;
	}

	if ((pkDvsNum == 2) && (pkDvsData1 != NULL) && (pkDvsData2 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;

		strlen(pkDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData2, 32-(int)strlen(pkDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData2));

		hsmCmdLen += 32;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", seedLen);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf+hsmCmdLen, seedKeyCiper, seedLen*2);
	hsmCmdLen += seedLen*2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", algorithm);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", otpKeyLen);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", challengeValLen);
	hsmCmdLen += 2;

	if ((challengeValLen!=0) && (challengeValue != NULL)) 
	{
		memcpy(hsmCmdBuf+hsmCmdLen, challengeValue, challengeValLen);
		hsmCmdLen += challengeValLen;
	}

	if (timeChangeCycle != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%03d", timeChangeCycle);
		hsmCmdLen += 3;

		sprintf(hsmCmdBuf+hsmCmdLen, "%016d", timeValue);
		hsmCmdLen += 16;
	}
	else
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%03d",timeChangeCycle);
		hsmCmdLen += 3;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", timeWindow);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, otpKey, otpKeyLen);

	hsmCmdLen += otpKeyLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFD: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;	
	memcpy(clockSkew, hsmCmdBuf+offset, 1);	

	offset += 1;
	memcpy(tmpBuf, hsmCmdBuf+offset, 2);
	tmpBuf[2] = 0;

	*clockSkewValue = atoi(tmpBuf);

	return 0;
}

/*
   ���ܣ���WWK��Կ�ӽ�������
   �������
cryptoFlag: 	0:���� 1:����
wwk: 		LMK22-23����
encrypFlag:  	0: ECBģʽ 1: CBCģʽ
iv:  		��ʼ����(����CBCģʽ)
lenOfData: 	���������ֽ�����8�ı�����
data:		�����ܻ���ܵ�����
�������
outdata: 	������
 */
int UnionHsmCmdB8(char *wwk,int cryptoFlag,int encrypFlag,char *iv,int lenOfData,char *data,char *outdata,int sizeOfOutData)
{
	int	ret;
	char	hsmCmdBuf[8096+40];
	int	hsmCmdLen = 0;
	int	retLen;
	int	keyLen;
	if (strlen(wwk) == 0 || lenOfData <= 0 || data == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdB8:: parameters error\n");	
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"B8",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf + hsmCmdLen,"006",3);
	hsmCmdLen += 3;
	switch (keyLen = strlen(wwk))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdB8:: wwk [%s] length error!\n",wwk);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,wwk,keyLen);
	hsmCmdLen += keyLen;

	hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"%d%d",cryptoFlag,encrypFlag);
	if (encrypFlag == 1)
	{
		memcpy(hsmCmdBuf + hsmCmdLen,iv,strlen(iv));
		hsmCmdLen += strlen(iv);
	}

	if (lenOfData % 8 != 0)
	{
		UnionUserErrLog("in UnionHsmCmdB8:: lenOfData [%04d] error!\n",lenOfData);
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	bcdhex_to_aschex(data,lenOfData,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (lenOfData * 2);
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	// add  by leipp 20150116
	if (cryptoFlag == 1)
		UnionSetMaskPrintTypeForHSMReqCmd();
	else
		UnionSetMaskPrintTypeForHSMResCmd();
	// end

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	retLen = ret - 8;
	if (retLen > sizeOfOutData)
	{
		UnionUserErrLog("in UnionHsmCmdB8:: sizeOfOutData [%04d] < retLen [%04d]\n",sizeOfOutData,retLen);
		return(errCodeSmallBuffer);
	}
	memcpy(outdata,hsmCmdBuf+8,retLen);
	outdata[retLen] = 0;
	return(retLen);
}

//������	��������	˵����
//�������	2A	ֵ��FE��
//������Կ����Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ��K+�����ţ��������Ϊ�����������Կ������
//����Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��309�е�һ��
//������Կ��ɢ����	1N	0~2��
//������ɢ����	n*32H	��������Կ��ɢ����Ϊ1~2ʱ��
//����n��������Կ��ɢ����
//����ǰ����������	32H	
//�����㷨	1N	1��UNION_SM3��2��UNION_SM4
//�������ݳ���	4N	
//��������	nH	
//�����	����	˵����
//��Ӧ����	2A	ֵ��FF��
//�������	2N	
//���κ����������	32H	
//���κ�������У��ֵ	16H	

int UnionHsmCmdFE(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, char* seedKeyCiper, int algorithm, int tranDataLen, char* tranData,  char* seed, char* seedCheckValue) 
{
	char hsmCmdBuf[1024];

	int hsmCmdLen = 0;
	int ret = 0;
	int offset = 0;

	if ((pkDvsNum < 0) || (pkDvsNum > 2))
	{
		UnionUserErrLog("in UnionHsmCmdFE:: pkDvsNum[%d]\n", pkDvsNum);	
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "FE", 2);
	hsmCmdLen += 2;

	if ((mk == NULL) || (strlen(mk) == 0))	
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03d", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		strcpy(hsmCmdBuf+hsmCmdLen, mk);
		hsmCmdLen += strlen(mk);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);	
	hsmCmdLen += 1;

	if ((pkDvsNum == 1) && (pkDvsData1 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;
	}

	if ((pkDvsNum == 2) && (pkDvsData1 != NULL) && (pkDvsData2 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;

		strlen(pkDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData2, 32-(int)strlen(pkDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData2));

		hsmCmdLen += 32;
	}

	memcpy(hsmCmdBuf+hsmCmdLen, seedKeyCiper, 32);
	hsmCmdLen += 32;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", algorithm);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", tranDataLen);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf+hsmCmdLen, tranData, tranDataLen);
	hsmCmdLen += tranDataLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFE:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;	

	memcpy(seed, hsmCmdBuf+offset, 32);	
	offset += 32;	

	memcpy(seedCheckValue, hsmCmdBuf+offset, 16);	

	return 0;
}

//�������	2A	ֵ��FF��
//������Կ����Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ��K+�����ţ��������Ϊ�����������Կ������
//����Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��309�е�һ��
//����ǰ������Կ��ɢ����	1N	1~2��
//����ǰ������ɢ����	n*32H	��������Կ��ɢ����Ϊ1~2ʱ��
//����n��������Կ��ɢ����
//���κ󱣻���Կ��ɢ����	1N	1~2��
//���κ󱣻���ɢ����	n*32H	��������Կ��ɢ����Ϊ1~2ʱ��
//����n��������Կ��ɢ����
//����ǰ�������ĳ���	4N	16��������
//����ǰ��������	n*2H	�ɸ���Կ��ɢ�����Կ������PKCS#5��䣩
//�����㷨	1N	1�� SM3
//����ǰ����У��ֵ	16H	�����㷨Ϊ1ʱ������sm3(����)ǰ8�ֽ��Ƿ�һ�¡�
//���κ�����ӳ���	4N	�����ҽ�ȡ���ٸ��ֽ�
//����Ҫ�س���	4N	
//����Ҫ��	n*2H	
//���Ӳ��뵽����Ҫ����λ��	4N	��0,���������Ϊ������+����Ҫ��

//�����	����	˵����
//��Ӧ����	2A	ֵ��FG��
//�������	2N	
//���κ���������ĳ���	4N	
//���κ����������	n*2H	�ɱ��κ�ı�����Կ������PKCS#5��䣩
//���κ�������У��ֵ	16H	

int UnionHsmCmdFF(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int newPkDvsNum, char* newPkDvsData1, char* newPkDvsData2,  int seedLen, char* seedKeyCiper, int algorithm, char *seedCheckValue,  int newSeedLen, int tranElementLen, char *tranElement, int tranOffset, char* seed, char* newSeedCheckValue) 
{
	char hsmCmdBuf[1024];
	char tmpBuf[128];
	int hsmCmdLen = 0;
	int ret = 0;
	int offset = 0;
	int seedLength= 0;

	if ((pkDvsNum < 0) || (pkDvsNum > 2))
	{
		UnionUserErrLog("in UnionHsmCmdFF:: pkDvsNum[%d]\n", pkDvsNum);	
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "FF", 2);
	hsmCmdLen += 2;

	if ((mk == NULL) || (strlen(mk) == 0))	
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "K%03d", mkIndex);
		hsmCmdLen += 4;
	}
	else
	{
		strcpy(hsmCmdBuf+hsmCmdLen, mk);
		hsmCmdLen += strlen(mk);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);	
	hsmCmdLen += 1;

	if ((pkDvsNum == 1) && (pkDvsData1 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;
	}

	if ((pkDvsNum == 2) && (pkDvsData1 != NULL) && (pkDvsData2 != NULL))
	{
		strlen(pkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData1, 32-(int)strlen(pkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData1));

		hsmCmdLen += 32;

		strlen(pkDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", pkDvsData2, 32-(int)strlen(pkDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", pkDvsData2));

		hsmCmdLen += 32;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", newPkDvsNum);	
	hsmCmdLen += 1;

	if ((newPkDvsNum == 1) && (newPkDvsData1 != NULL))
	{
		strlen(newPkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", newPkDvsData1, 32-(int)strlen(newPkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", newPkDvsData1));

		hsmCmdLen += 32;
	}

	if ((newPkDvsNum == 2) && (newPkDvsData1 != NULL) && (newPkDvsData2 != NULL))
	{
		strlen(newPkDvsData1)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", newPkDvsData1, 32-(int)strlen(newPkDvsData1), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", newPkDvsData1));

		hsmCmdLen += 32;

		strlen(newPkDvsData2)<32?(sprintf(hsmCmdBuf+hsmCmdLen, "%s%0*d", newPkDvsData2, 32-(int)strlen(newPkDvsData2), 0))
			:(snprintf(hsmCmdBuf+hsmCmdLen, 32+1, "%s", newPkDvsData2));

		hsmCmdLen += 32;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", seedLen);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf+hsmCmdLen, seedKeyCiper, seedLen*2);
	hsmCmdLen += seedLen*2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", algorithm);
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf+hsmCmdLen, seedCheckValue, 16);
	hsmCmdLen += 16;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", newSeedLen);
	hsmCmdLen += 4;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", tranElementLen);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf+hsmCmdLen, tranElement, tranElementLen*2);
	hsmCmdLen += tranElementLen*2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", tranOffset);
	hsmCmdLen += 4;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdFB:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;	

	memcpy(tmpBuf, hsmCmdBuf+offset, 4);	
	tmpBuf[4] = 0;
	seedLength = atoi(tmpBuf);

	offset += 4;	

	memcpy(seed, hsmCmdBuf+offset,seedLength*2);
	offset += seedLength*2;	

	memcpy(newSeedCheckValue, hsmCmdBuf+offset, 16);	

	return 0;
}

int UnionHsmCmdAE(char *keyValue1,char *keyValue2,char *keyByLMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	keyLen;
	int	offset = 0;

	memcpy(hsmCmdBuf,"AE",2);
	hsmCmdLen = 2;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(keyValue1),keyValue1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdAE:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(keyValue2),keyValue2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdAE:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdAE:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	// modify by liwj 20150617
	//switch(strlen(keyValue1))
	// end
	switch(strlen(keyValue2))
	{
		case	16:
			offset = 4;
			keyLen = 16;
			break;
		case	32:
			offset = 4 + 1;
			keyLen = 32;
		case	48:
			offset = 4 + 1;	
			keyLen = 48;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdAE:: keyLen = [%d]\n",(int)strlen(keyValue1));
			return(errCodeParameter);

	}

	memcpy(keyByLMK, hsmCmdBuf+offset, keyLen);
	keyByLMK[keyLen] = 0;
	offset += keyLen;

	strcpy(checkValue, hsmCmdBuf+offset);	

	return(keyLen);

}

int UnionHsmCmdAG(char *tmk,char *tak,char *keyByTMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	keyLen;
	int	offset = 0;

	memcpy(hsmCmdBuf,"AG",2);
	hsmCmdLen = 2;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(tmk),tmk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdAG:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	UnionLog("in UnionHsmCmdAG:: tmk[%s] hsmCmdBuf[%s]!\n",tmk,hsmCmdBuf);

	if ((ret = UnionGenerateX917RacalKeyString(strlen(tak),tak,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdAG:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdAG:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	switch(strlen(tak))
	{
		case	16:
			offset = 4;
			keyLen = 16;
			break;
		case	32:
			offset = 4 + 1;	
			keyLen = 32;
		case	48:
			offset = 4 + 1;	
			keyLen = 48;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdAG:: keyLen = [%d]\n",(int)strlen(tak));
			return(errCodeParameter);

	}

	memcpy(keyByTMK, hsmCmdBuf+offset, keyLen);
	keyByTMK[keyLen] = 0;
	offset += keyLen;

	strcpy(checkValue, hsmCmdBuf+offset);	

	return(keyLen);
}

/*
   ����:�����й�Կ���ܵ�һ�����ݲ���MAC
   �������
vkIndex 	:˽Կ������ָ����˽Կ�����ڽ�����������
lenOfRandom  :���������
random 	:�����
zak 		:���ڼ���MAC����Կ
dataByPK	:������Կ���ܵ�����
�������
macByZAK
 */

int UnionHsmCmd4G(char *vkIndex,int lenOfRandom,char *random,char *zak,char *dataByPK,char *macByZAK)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	char	dataByPKASC[1024];

	if ((strlen(vkIndex) == 0) || (strlen(zak) == 0) || (strlen(dataByPK) == 0))
	{
		UnionUserErrLog("in UnionHsmCmd4G:: vkIndex ��zak or dataByPK is null!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf,"4G",2);
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfRandom);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,random,lenOfRandom);
	hsmCmdLen += lenOfRandom;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(zak),zak,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4G:: UnionGenerateX917RacalKeyString [%s]\n",zak);
		return(ret);
	}
	hsmCmdLen += ret;

	//strcpy(hsmCmdBuf+hsmCmdLen,dataByPK);
	//hsmCmdLen += strlen(dataByPK);
	aschex_to_bcdhex(dataByPK, strlen(dataByPK), dataByPKASC);
	memcpy(hsmCmdBuf+hsmCmdLen, dataByPKASC, strlen(dataByPK) / 2);
	hsmCmdLen += strlen(dataByPK) / 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4G:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (ret == 4+16)
	{
		strcpy(macByZAK,hsmCmdBuf+4);
		return(16);
	}
	else
		return(0);
}

int UnionHsmCmd4H(char *vkIndex,int lenOfRandom,char *random,char *zpk,char *pan,char *pinByPK,char *pinByZPK)
{
	int	ret;
	char	hsmCmdBuf[2048];
	char	dataByPKASC[1024];
	int	hsmCmdLen = 0;
	char	tmpBuf[32];

	if ((strlen(vkIndex) == 0) || (strlen(zpk) == 0) || (strlen(pinByPK) == 0))
	{
		UnionUserErrLog("in UnionHsmCmd4H:: vkIndex or zpk or pinByPK is null!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf,"4H",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfRandom);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,random,lenOfRandom);
	hsmCmdLen += lenOfRandom;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4H:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}

	hsmCmdLen += ret;

	if ((ret = UnionForm12LenAccountNumber(pan,strlen(pan),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4H:: UnionForm12LenAccountNumber for [%s]\n",pan);
		return(ret);
	}
	tmpBuf[ret] = 0;
	ret = sprintf(hsmCmdBuf+hsmCmdLen,"0000%s",tmpBuf);
	hsmCmdLen += ret;

	//memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,strlen(pinByPK));
	//hsmCmdLen += strlen(pinByPK);
	aschex_to_bcdhex(pinByPK, strlen(pinByPK), dataByPKASC);
	memcpy(hsmCmdBuf+hsmCmdLen, dataByPKASC, strlen(pinByPK) / 2);
	hsmCmdLen += strlen(pinByPK) / 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4H:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (ret == 4+16)
	{
		memcpy(pinByZPK,hsmCmdBuf+4,16);
		return(16);
	}
	else
		return(0);
}

//��SM4��Կ��SM2��Կ�¼���ת��ΪLMK�¼���(SM4��SM2)
// ��ݸũ����
int UnionHsmCmdWO(int vkIndex,int lenOfVK,char *vkValue,TUnionDesKeyType keyType,int lenOfSM4,char *sm4BySM2,char *keyValue,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	char	tmpBuf[2048];

	if (sm4BySM2 == NULL || (strlen(sm4BySM2) == 0))
	{
		UnionUserErrLog("in UnionHsmCmdWO:: sm4BySM2 is null!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf,"WO",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;
	if (vkIndex == 99)
	{
		if (vkValue == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdWO:: vkValue is null!\n");
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;	

		// ת��Ϊ������
		aschex_to_bcdhex(vkValue,lenOfVK,tmpBuf);	
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;
	}

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWO:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// ��Կ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfSM4/2);
	hsmCmdLen += 4;			

	// ��Կ
	// ת��Ϊ������
	aschex_to_bcdhex(sm4BySM2,lenOfSM4,tmpBuf);	
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfSM4/2);
	hsmCmdLen += lenOfSM4/2;
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWO:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(keyValue,hsmCmdBuf+4+1,32);	
	memcpy(checkValue,hsmCmdBuf+4+1+32,16);

	return(32);
}

// ������Կ
int UnionHsmCmdWP(int encryFlag,TUnionDesKeyType keyType1,char *keyValue1,char *checkValue1,TUnionDesKeyType keyType2,char *keyValue2,char *checkValue2,char *keyValue)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if (keyValue1 == NULL || (checkValue1 == 0) || keyValue2 == NULL || checkValue2 == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdWP:: keyValue1 or checkValue1 or keyValue2 or checkValue2 is null!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"WP",2);
	hsmCmdLen = 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",encryFlag);
	hsmCmdLen += 1;

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWP:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,keyValue1,33);
	hsmCmdLen += 33;

	memcpy(hsmCmdBuf+hsmCmdLen,checkValue1,16);
	hsmCmdLen += 16;

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWP:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,keyValue2,33);
	hsmCmdLen += 33;

	memcpy(hsmCmdBuf+hsmCmdLen,checkValue2,16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWP:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(keyValue,hsmCmdBuf+4,32);

	return(32);
}

/*
   �������ܣ�(����)
   WPָ��������һ��PIN
   ���������
   pinLength��Ҫ��������ɵ�PIN���ĵĳ���
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK�����������PIN�����ģ���LMK����
sizeofPinBlock: pinBlockByLMK�ĳ���
 */
int UnionHsmCmdWV(int pinLength,char *accNo,int lenOfAccNo,char *pinBlockByLMK,int sizeofPinBlock)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((accNo == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdWV:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// ������
	memcpy(hsmCmdBuf,"WV",2);
	hsmCmdLen = 2;

	// �㷨��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWV:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN����
	if (pinLength < 4 || pinLength > 12)
	{
		UnionUserErrLog("in UnionHsmCmdWV:: pinLength[%d] not in [4,12]\n",pinLength);
		return(errCodeParameter);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pinLength);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWV:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	// ���������PIN�����ģ���LMK����
	if (ret - 5 >= sizeofPinBlock)
	{
		UnionUserErrLog("in UnionHsmCmdWV:: ret[%d] >= sizeofPinBlock[%d]!\n",ret - 5,sizeofPinBlock);
		return(errCodeSmallBuffer);
	}
	memcpy(pinBlockByLMK,hsmCmdBuf+5,ret-5);
	pinBlockByLMK[ret-5] = 0;
	return(ret-5);
}

/*
   �������ܣ�(����)
   CLָ�����VISA��У��ֵCVV
   ���������
   cardValidDate��VISA������Ч��
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   cvv�����ɵ�VISA����CVV
 */
int UnionHsmCmdCL(char *cardValidDate,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,int sizeofCVV)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdCW:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"CL",2);
	hsmCmdLen = 2;

	// ��Կֵ
	memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen, cvk, strlen(cvk));
	hsmCmdLen += strlen(cvk);

	// �ͻ��ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;

	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;

	// VISA������Ч��
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;

	// VISA�������̴���
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;

	// �ŵ�
	memcpy(hsmCmdBuf+hsmCmdLen,"2",1);
	hsmCmdLen += 1;

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCL:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (3 >= sizeofCVV)
	{
		UnionUserErrLog("in UnionHsmCmdCL:: ret[3] >= sizeofCVV[%d]!\n",sizeofCVV);
		return(errCodeSmallBuffer);
	}

	// ���ɵ�VISA����CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	cvv[3] = 0;
	return(3);
}

/*
   �������ܣ�(����)
   WWָ�����һ������PIN/����һ���Ѽ��ܵ�PIN
   ���������
   mode��0:����    1:����
   oriPIN��Դpinֵ
accNo: �˺�
lenOfAccNo���ͻ��ʺų���
sizeofDesPIN:desPIN����󳤶�
sizeofReferenceNum:referenceNum����󳤶�,������ʱ����Ч
���������
desPIN��Ŀ��pinֵ
referenceNum: ������ʱ������Ч 
 */
int UnionHsmCmdWW(int mode,char *oriPIN,char *accNo,int lenOfAccNo,char *desPIN,int sizeofDesPIN,char *referenceNum,int sizeofReferenceNum)
{
	int	ret = 0;
	int	hsmCmdLen = 0;
	char	hsmCmdBuf[128];
	char	tmpBuf[32];
	int	pinLen = 0;
	int	i = 0;

	// ָ��
	memcpy(hsmCmdBuf,"WW",2);
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	// �㷨��ʶ
	if (mode != 0 && mode != 1)
	{
		UnionUserErrLog("in UnionHsmCmdWW:: mode[%d] not in [0,1]!\n",mode);	
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	// Դpinֵ
	if (oriPIN == NULL || strlen(oriPIN) == 0)
	{
		UnionUserErrLog("in UnionHsmCmdWW:: oriPIN is null\n");	
		return(errCodeParameter);
	}

	if (mode == 0)
	{
		// ����ʱ����16���ַ��������F��16
		pinLen = strlen(oriPIN);
		memset(tmpBuf,'F',pinLen+1);	
		memcpy(tmpBuf,oriPIN,strlen(oriPIN));
		tmpBuf[pinLen+1] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,pinLen+1);	
		hsmCmdLen += pinLen+1;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,oriPIN,strlen(oriPIN));	
		hsmCmdLen += strlen(oriPIN);
	}

	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;

	// �˺�	
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWW:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	//add by liwj 2014-12-19
	if (mode == 0)
		UnionSetMaskPrintTypeForHSMReqCmd();
	else
		UnionSetMaskPrintTypeForHSMResCmd();
	//end

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWW:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (mode == 0)
	{
		if (ret-5 >= sizeofDesPIN)
		{
			UnionUserErrLog("in UnionHsmCmdWW:: ret[%d] >= sizeofDesPIN[%d]!\n",ret - 5,sizeofDesPIN);
			return(errCodeSmallBuffer);
		}
		memcpy(desPIN,hsmCmdBuf+5,ret-5);
		desPIN[ret-5] = 0;
	}
	else
	{
		pinLen = ret - 5 -12;
		if (pinLen >= sizeofDesPIN)
		{
			UnionUserErrLog("in UnionHsmCmdWW:: ret[%d] >= sizeofDesPIN[%d]!\n",pinLen,sizeofDesPIN);
			return(errCodeSmallBuffer);
		}

		if (12 >= sizeofReferenceNum)
		{
			UnionUserErrLog("in UnionHsmCmdWW:: ret[%d] >= sizeofReferenceNum[%d]!\n",12,sizeofReferenceNum);
			return(errCodeSmallBuffer);
		}

		// Ŀ��PIN
		memcpy(desPIN,hsmCmdBuf+5,pinLen);
		desPIN[pinLen] = 0;
		for (i = 0; i < pinLen; i++)
		{
			if (desPIN[i] == 'F')
			{
				desPIN[i] = '\0';
				break;
			}
		}

		// �ο���
		memcpy(referenceNum,hsmCmdBuf+5+pinLen,12);
		referenceNum[12] = 0;
	}
	return(ret-5);
}

/*
   ���ܣ���PIN���ZPK��SM4���ܷ��뵽LMK�¼��ܡ�
   ָ�	WX
   ���������
mode: ģʽ�1�70:PIN��LMKת��ZPK
1:PIN��ZPKת��LMK
zpkValue: zpk��Կֵ
oriPinBlock: PIN��	
format:   pin���ʽ
accNo:    �˺�,ȥ��У��λ������12λ
sizeofPinDesPinBlock: Ŀ��pin�����󳤶�
�������:
desPinBlock: LMK�ԣ�02-03���¼��ܵ�PIN/ZPK���ܵ�PIN
 */
int UnionHsmCmdWX(int mode,char *zpkValue,char *pinBlock,char *format,char *accNo,int lenOfAccNo,char *desPinBlock,int sizeofPinDesPinBlock)
{
	int	ret = 0;
	char	hsmCmdBuf[256];
	int	hsmCmdLen = 0;

	if (pinBlock == NULL || strlen(pinBlock) == 0 || zpkValue == NULL || format == NULL || accNo == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdWX:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"WX",2);
	hsmCmdLen = 2;

	// �㷨ѡ��
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	if (mode != 0 && mode != 1)
	{
		UnionUserErrLog("in UnionHsmCmdWX:: mode[%d] not in [0,1]!\n",mode);
		return(errCodeParameter);
	}
	// ģʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	// zpk��Կ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf+hsmCmdLen,zpkValue,32);
	hsmCmdLen += 32;

	// pin��
	UnionLog("in UnionHsmCmdWX:: pinBlock[%s]!\n",pinBlock);
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,strlen(pinBlock));
	hsmCmdLen += strlen(pinBlock);

	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;

	// pin���ʽ 
	memcpy(hsmCmdBuf+hsmCmdLen,format,2);
	hsmCmdLen += 2;

	// �˺�	
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWX:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWX:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 5 >= sizeofPinDesPinBlock)
	{
		UnionUserErrLog("in UnionHsmCmdWX:: ret[%d] >= sizeofReferenceNum[%d]!\n",ret-5,sizeofPinDesPinBlock);
		return(errCodeSmallBuffer);
	}

	memcpy(desPinBlock,hsmCmdBuf+5,ret - 5);
	desPinBlock[ret-5] = 0;

	return(ret-5);
}

/*
   ����: PIN���ת����
   ָ��: W8
   �������:
oriAlgorith:	1N	 ԴPIN���㷨��1-DES/3DES, 2-SM4
desAlgorith:	1N	 Ŀ��PIN���㷨��1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	Դzpk�����Ż�����
desKeyLenFlag: 	1N	Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
desKeyIndex/desZpkValue: 1A+3H/16/32/48H	Ŀ��zpk�����Ż�����
oriFormat:	2N	ԴPIN���ʽ,��01��-��06�������PIN���ʽ
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
desFormat:	2N	Ŀ��PIN���ʽ,��01��-��06�������PIN��
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ք1�7
pinBlock:	16H/32H		��Կ���ܵ�PIN
oriAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
desAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���

sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	16H/32H		ת���ܺ��PIN����
 */
/*int UnionHsmCmdW8(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"W8",2);
	hsmCmdLen = 2;

	// ԴPIN���㷨
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Ŀ��PIN���㷨
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Դ��Կ���ȱ�ʶ
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// Դ��Կ���Ļ�������
	if (oriKeyIndex != NULL && strlen(oriKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriKeyIndex);
		hsmCmdLen += 4;
	}
	else if (oriZpkValue != NULL && strlen(oriZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriKeyIndex and oriZpkValue is null!\n");
		return(errCodeParameter);
	}

	// Ŀ����Կ����
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}

	// Ŀ��zpk��Կ
	if (desKeyIndex != NULL && strlen(desKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desKeyIndex);
		hsmCmdLen += 4;
	}
	else if (desZpkValue != NULL && strlen(desZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",desZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desKeyIndex and desZpkValue is null!\n");
		return(errCodeParameter);
	}

	// ԴPIN��ĸ�ʽ
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// Ŀ��PIN��ĸ�ʽ
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// ԴPIN��
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// Դ�˺�
	if (strcmp(oriFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(oriFormat,"04") == 0)
	{
		if (strlen(oriAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: oriAccNo[%s] len[%d] != 18\n",oriAccNo,(int)strlen(oriAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,oriAccNo,18);
		hsmCmdLen += 18;
	}
	else if (strcmp(oriFormat,"07") == 0)//��������ר��
        {
                if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(oriFormat,"09") == 0) //����ũ��ר��
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// Ŀ���˺�
	if (strcmp(desFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(desFormat,"04") == 0)
	{
		if (strlen(desAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: desAccNo[%s] len[%d] != 18\n",desAccNo,(int)strlen(desAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,desAccNo,18);
		hsmCmdLen += 18;
	}
	else if (strcmp(desFormat,"07") == 0)//��������ר��
        {
                if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(desFormat,"17") == 0)//��������ר��
        {
                if (strlen(desAccNo) != 16)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: desAccNo[%s] len[%d] != 16\n",desAccNo,(int)strlen(desAccNo));
                        return(errCodeParameter);
                }
                memcpy(hsmCmdBuf+hsmCmdLen,desAccNo,16);
                hsmCmdLen += 16;
        }
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 4 >= sizeofPinByZpk)
	{
		UnionUserErrLog("in UnionHsmCmdW8:: ret[%d] >= sizeofPinByZpk[%d]!\n",ret-4,sizeofPinByZpk);
		return(errCodeParameter);
	}
	memcpy(pinByZpk,hsmCmdBuf+4,ret-4);
	pinByZpk[ret-4] = 0;

	return(ret-4);
}*/

//W8_Base,add by huangh 2016.04.15
/*
   ����: PIN���ת����
   ָ��: W8(��������ר��W8)
   �������:
oriAlgorith:	1N	 ԴPIN���㷨��1-DES/3DES, 2-SM4
desAlgorith:	1N	 Ŀ��PIN���㷨��1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	Դzpk�����Ż�����
desKeyLenFlag: 	1N	Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
deskeytype	3N	Ŀ����Կ����:Ŀ����Կ���ͱ�ʶΪYʱ������ڣ�֧��ZPK(001) / ZEK(00A)

desKeyIndex/desZpkValue: 1A+3H/16/32/48H	Ŀ��zpk�����Ż�����
oriFormat:	2N	ԴPIN���ʽ,��01��-��06�������PIN���ʽ
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
desFormat:	2N	Ŀ��PIN���ʽ,��01��-��06�������PIN��
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ք1�7
pinBlock:	16H/32H		��Կ���ܵ�PIN
oriAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
desAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���

sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	16H/32H		ת���ܺ��PIN����
 */
int UnionHsmCmdW8_Base(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *deskeytype,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"W8",2);
	hsmCmdLen = 2;

	// ԴPIN���㷨
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Ŀ��PIN���㷨
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Դ��Կ���ȱ�ʶ
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// Դ��Կ���Ļ�������
	if (oriKeyIndex != NULL && strlen(oriKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriKeyIndex);
		hsmCmdLen += 4;
	}
	else if (oriZpkValue != NULL && strlen(oriZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriKeyIndex and oriZpkValue is null!\n");
		return(errCodeParameter);
	}

	// Ŀ����Կ����
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}
	//(��������ר��)Ŀ����Կ���ͱ�ʶ:Y--����  N--������(Ĭ��ΪN��Ŀ����Կ����Ĭ��ΪZPK(001))
	if ((deskeytype) && (strlen(deskeytype) == 3))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"Y%s",deskeytype);
		hsmCmdLen += 4;
	}
	// Ŀ��ZPK/ZEK��Կ
	if (desKeyIndex != NULL && strlen(desKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desKeyIndex);
		hsmCmdLen += 4;
	}
	else if (desZpkValue != NULL && strlen(desZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",desZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desKeyIndex and desZpkValue is null!\n");
		return(errCodeParameter);
	}

	// ԴPIN��ĸ�ʽ
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// Ŀ��PIN��ĸ�ʽ
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else if((desAlgorith == 2) && (strcmp(desFormat,"17") == 0))//��������ר��
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else if((desAlgorith == 2) &&(strcmp(desFormat,"18") == 0))//۴���������������ջݽ�����Ŀ���Ӹ�ʽ18��add by huangh 20160606
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// ԴPIN��
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// Դ�˺�
	if (strcmp(oriFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(oriFormat,"04") == 0)
	{
		if (strlen(oriAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: oriAccNo[%s] len[%d] != 18\n",oriAccNo,(int)strlen(oriAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,oriAccNo,18);
		hsmCmdLen += 18;
	}
	else if (strcmp(oriFormat,"07") == 0)//��������ר��
        {
                if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(oriFormat,"09") == 0) //����ũ��ר��
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// Ŀ���˺�
	if (strcmp(desFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(desFormat,"04") == 0)
	{
		if (strlen(desAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: desAccNo[%s] len[%d] != 18\n",desAccNo,(int)strlen(desAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,desAccNo,18);
		hsmCmdLen += 18;
	}
	else if (strcmp(desFormat,"07") == 0)//��������ר��
        {
                if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(desFormat,"17") == 0)//��������ר��
        {
                if (strlen(desAccNo) != 16)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: desAccNo[%s] len[%d] != 16\n",desAccNo,(int)strlen(desAccNo));
                        return(errCodeParameter);
                }
                memcpy(hsmCmdBuf+hsmCmdLen,desAccNo,16);
                hsmCmdLen += 16;
        }
	else if (strcmp(desFormat,"18") == 0)//۴���������������ջݽ�����Ŀ���Ӹ�ʽ18��add by huangh 20160606
        {
                if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 4 >= sizeofPinByZpk)
	{
		UnionUserErrLog("in UnionHsmCmdW8:: ret[%d] >= sizeofPinByZpk[%d]!\n",ret-4,sizeofPinByZpk);
		return(errCodeParameter);
	}
	memcpy(pinByZpk,hsmCmdBuf+4,ret-4);
	pinByZpk[ret-4] = 0;

	return(ret-4);
}
int UnionHsmCmdW8(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{	
	int ret;
	ret = UnionHsmCmdW8_Base(oriAlgorith,desAlgorith,oriKeyLenFlag,oriKeyIndex,oriZpkValue,desKeyLenFlag,NULL,desKeyIndex,desZpkValue,oriFormat,desFormat,pinBlock,oriAccNo,desAccNo,pinByZpk,sizeofPinByZpk);
	if(ret < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW8:: fail!\n");
	}
	return (ret);
}


/*
   ����: ����/��֤mac
   ָ��:W9
   �������:
mode:	ģʽ��1:����MAC��2:У��MAC
zakIndex: zak������
zakValue: zak��Կ
macCalcFlag: MAC���㷽ʽ,1:�������ĵ�MAC���㷽��	
2:˳���ļ�MAC�ļ��㷽��
msgType: ��Ϣ����,0:��Ϣ����Ϊ������
1:��Ϣ����Ϊ��չʮ������
lenOfMacData: macData���ݳ��ȣ�������8192
macData: ��������MAC������
oriMac:	��У���MACֵ,����ģʽΪ2ʱ���������
sizeofDesMac:desMac����󳤶�
�������:
desMac: MACֵ

 */
int UnionHsmCmdW9(int mode,char *zakIndex,char *zakValue,int macCalcFlag,int msgType,int lenOfMacData,char *macData,char *oriMac,char *desMac,int sizeofDesMac)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;

	// ָ��
	memcpy(hsmCmdBuf,"W9",2);
	hsmCmdLen = 2;

	// ģʽ
	if (mode != 1 && mode != 2)
	{
		UnionUserErrLog("in UnionHsmCmdW9:: mode[%d] not in [1,2]!\n",mode);
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	//��Կ��������Կ����
	if (zakIndex != NULL && strlen(zakIndex) != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,zakIndex,strlen(zakIndex));
		hsmCmdLen += 4;
	}
	else if (zakValue != NULL && strlen(zakValue) != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,zakValue,32);
		hsmCmdLen += 32;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW9:: zakIndex is null or zakValue is null!\n");	
		return(errCodeParameter);
	}

	// MAC���㷽ʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",macCalcFlag);
	hsmCmdLen += 1;

	// ��Ϣ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",msgType);
	hsmCmdLen += 1;

	if (macData == NULL || lenOfMacData > 8192)
	{
		UnionUserErrLog("in UnionHsmCmdW9:: macData is null or lenOfMacData > 8912!\n");
		return(errCodeParameter);
	}

	// MAC���ݳ���
	if (msgType == 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfMacData);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen,macData,lenOfMacData);
		hsmCmdLen += lenOfMacData;
	}
	else if (msgType == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfMacData/2);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen,macData,lenOfMacData);
		hsmCmdLen += lenOfMacData;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW9:: msgType[%d] not in [0,1]!\n",msgType);
		return(errCodeParameter);
	}

	// ����֤��MACֵ
	if (mode == 2)
	{
		if (oriMac == NULL || strlen(oriMac) != 32)
		{
			UnionUserErrLog("in UnionHsmCmdW9:: oriMac[%s] is null or != 32!\n",oriMac);	
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,oriMac,strlen(oriMac));	
		hsmCmdLen += strlen(oriMac);
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW9:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (32 >= sizeofDesMac)
	{
		UnionUserErrLog("in UnionHsmCmdW9:: ret[32] >= sizeofDesMac[%d]!\n",sizeofDesMac);
		return(errCodeParameter);
	}

	memcpy(desMac,hsmCmdBuf+4,ret-4);
	desMac[ret-4] = 0;

	return(ret-4);
}

/*
   ����:����һ��PINУ��ֵPVV
   ָ��:PV
   ���������
pvk:	1A+32H   ��LMK1415 ���ܣ���S��+3
pvkIndex: 1N	 ȡֵΪ1-F
tpk:	1A+32H	 ��LMK1415 ���ܣ���S��+32
pinBlock:  32H   ��TPK���ܵ�ANSI9.8PIN��
PAN:	   16H   16λ��������
�������:
pvv:	4N	���ɵ�PVV
 */
int UnionHsmCmdPV(char *pvk,char pvkIndex,char *tpk,char *pinBlock,char *pan,char *pvv)
{
	int	ret = 0;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	// ָ��
	memcpy(hsmCmdBuf,"PV",2);
	hsmCmdLen = 2;	

	// pvk��Կֵ
	sprintf(hsmCmdBuf+hsmCmdLen,"S%s",pvk);
	hsmCmdLen += 33;

	// pvk������
	sprintf(hsmCmdBuf+hsmCmdLen,"%c",pvkIndex);
	hsmCmdLen += 1;

	// tpk��Կֵ
	sprintf(hsmCmdBuf+hsmCmdLen,"S%s",tpk);
	hsmCmdLen += 33;

	// pinBlock��
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,32);
	hsmCmdLen += 32;

	// �˺�
	memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPV:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(pvv,hsmCmdBuf+4,4);
	pvv[4] = 0;

	return(4);
}

// ������ԿSM1��Կ
/* 
   �������
   keyType		��Կ����
   �������
   keyByLMK	LMK��������Կ����ֵ
   checkValue	��ԿУ��ֵ
 */
int UnionHsmCmdM0(TUnionDesKeyType keyType, char *keyByLMK, char *checkValue)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		offset;

	memcpy(hsmCmdBuf,"M0",2);
	hsmCmdLen = 2;

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWI:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWI:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(keyByLMK,hsmCmdBuf+offset,32);
	offset += 32;
	memcpy(checkValue,hsmCmdBuf+offset,16);

	return(0);
}

/*
   �������ܣ�
   SM1�㷨��ɢ����Կ�����������Կ���ù�����Կ��������Կ���㲢��֤MAC��(M9)
   ���������
mode: ģʽ��־
0-���������Կ����ɢ����Կ 
1-����MAC
2-У��MAC
mkType: ������Կ����ѡ��ָ����LMK�����������Կ�͹�����Կ 
001 �C ZPK ��LMK0607���ܣ�
008 �C ZAK (LMK2627����)
00A �C ZEK(LMK3031����)
mk: ��Կ����
mkIndex: ��Կ����, K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
mkDvsNum: ָ������Կ��ɢ�Ĵ�����0-3�Σ�
mkDvsData: n*8B ��ɢ��Ƭ��Կ�����ݣ�����n������ɢ����. ��ָ���ķ�ɢ����ƴ�ӷ�ɢ������ֵ��Ϊ�������ݣ������ܼ��㣬������16 �ֽڵĽ����Ϊ����Կ
proKeyFlag: ������Կ��ʶ,�Ƿ�Ա�����Կ���й��̼���,����Ҫ���������Կʱ,ʹ�ù�����Կ��������.
Y:���������Կ
N:�����������Կ
��ѡ��:��û�и���ʱȱʡΪN
proFactor: 8B ��������,�ڼ��������Կʱ������������8 �ֽ������,����ʱ���ܻ���ƴ��8 �ֽ�ȫ ��00����
ivMac: 32H MAC�����IV
checkMac: 8H ��У���MACֵ ��ģʽ��־Ϊ2ʱ���и���
macDataLen: 4N MAC���ݳ���
macData: 8H MAC����
macPrefixDataLen: 4N MACǰ׺������ݳ���
macPrefixData: nB MACǰ׺�������
���������
criperData: 32H ����Կ����,ָ����Կ�����¼��ܣ�ģʽ��־Ϊ0�и���
checkValue: 16H	����ԿУ��ֵ,��ɢ����ԿУ��ֵ��ģʽ��־Ϊ0�и���
proCriperData: 32H ������Կ����,������Կ��ʶΪ��Y��ʱ����ģʽ��־Ϊ0�и���  ָ����Կ�����¼���
proCheckValue: 16H ������ԿУ��ֵ,������Կ��ʶΪ��Y��ʱ����ģʽ��־Ϊ0�и���
SM1��LMK0405�¼���
3DES��ָ����Կ�����¼���
mac:MACֵ 8H	��ģʽ��־Ϊ1���и���
 */

int UnionHsmCmdM9(char *mode, char *mkType, char *mk, int mkIndex, int mkDvsNum, char *mkDvsData,
		char *proKeyFlag, char *proFactor,char *ivMac, char *checkMac, int macDataLen, char *macData,
		int macPrefixDataLen, char *macPrefixData, char *criperData, char *checkValue, char *proCriperData,
		char *proCheckValue, char *mac)
{
	int     ret;
	int     lenOfDvsData = 8;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    szMkBuff[128];

	// �������
	memcpy(hsmCmdBuf,"M9",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// ����Կ
	if ((mk == NULL) || (strlen(mk) == 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mk, 4);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
		memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
		hsmCmdLen += ret;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// ������Կ��ʶ
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// ��������
		if (proKeyFlag[0] == 'Y')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, proFactor, lenOfDvsData);
			hsmCmdLen += lenOfDvsData;
		}
	}

	// MAC�����IV
	if( (ivMac!= NULL) && (strlen(ivMac) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 32);
		hsmCmdLen += 32;
	}

	// ��У���MACֵ ��ģʽ��־Ϊ2ʱ
	if (strcmp(mode,"2") == 0)
	{
		if (strlen(checkMac) > 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, checkMac, 8);
			hsmCmdLen += 8;
		}
	}

	// macDataLen: 4N MAC���ݳ��� macData: 8H MAC����	
	if( (macData!= NULL) && (strlen(macData) != 0) )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macDataLen);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen, macData, macDataLen * 2 );
		hsmCmdLen += macDataLen * 2 ;
	}

	// macPrefixDataLen: 4N MACǰ׺������ݳ��� macPrefixData: nB MACǰ׺�������
	if( (macPrefixData!= NULL) && (strlen(macPrefixData) != 0) )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macPrefixDataLen);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen, macPrefixData, macPrefixDataLen * 2 );
		hsmCmdLen += macPrefixDataLen * 2 ;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM9:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (strcmp(mode,"0") == 0)
	{
		// criperData: 32H ����Կ����,ָ����Կ�����¼��ܣ�ģʽ��־Ϊ0�и���
		memcpy(criperData, hsmCmdBuf+offset, 16);
		offset += 16;
		// checkValue: 16H	����ԿУ��ֵ,��ɢ����ԿУ��ֵ��ģʽ��־Ϊ0�и���
		memcpy(checkValue, hsmCmdBuf+offset, 8);
		offset += 8;
	}

	if (strcmp(proKeyFlag,"Y") == 0)
	{
		// proCriperData: 32H ������Կ����,������Կ��ʶΪ��Y��ʱ����ģʽ��־Ϊ0�и���  ָ����Կ�����¼���
		memcpy(proCriperData, hsmCmdBuf+offset, 16);
		offset += 16;
		// proCheckValue: 16H ������ԿУ��ֵ,������Կ��ʶΪ��Y��ʱ����ģʽ��־Ϊ0�и���
		memcpy(proCheckValue, hsmCmdBuf+offset, 8);
		offset += 8;
	}

	if (mode[0] == '1')
	{
		//mac:MACֵ 8H	��ģʽ��־Ϊ1���и���
		memcpy(mac, hsmCmdBuf+offset, 4);
		offset += 4;
	}

	return(0);
}

// ȡ��RSA��˽��Կ<TW>
int UnionHsmCmdTW(char *mode,char *vkIndex,char *pkEncoding,char *vkByMK,int *vkLen,char *pk,int *pkLen)
{
	int     ret;
	char    hsmCmdBuf[8192];
	int     hsmCmdLen = 0;
	char	tmpBuf[32];

	if ((vkIndex == NULL) || (vkByMK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdTW:: null pointer!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"TW",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,mode,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,4);
	hsmCmdLen += 4;
	if (mode[0] == '0' || mode[0] == '2')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,pkEncoding,2);
		hsmCmdLen += 2;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdTW:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(vkByMK,hsmCmdBuf+4+4,ret - 8);
	if (mode[0] == '0')
	{
		*pkLen = ret - 8;
		memcpy(pk,hsmCmdBuf+4+4,*pkLen);
	}
	else if (mode[0] == '1')
	{
		*vkLen = ret - 8;
		memcpy(vkByMK,hsmCmdBuf+4+4,*vkLen);
	}
	else if (mode[0] == '2')
	{
		memcpy(tmpBuf, hsmCmdBuf+4, 4);
		tmpBuf[4] = 0;
		*pkLen = atoi(tmpBuf);
		memcpy(pk, hsmCmdBuf+8, *pkLen);

		memcpy(tmpBuf, hsmCmdBuf+8+(*pkLen), 4);
		tmpBuf[4] = 0;
		*vkLen = atoi(tmpBuf);
		memcpy(vkByMK, hsmCmdBuf+12+(*pkLen), *vkLen);
	}

	return(ret - 4);
}

// �ѻ�PIN����
int UnionHsmCmdVA(char *mode, char *id, int mkIndex, char *mk, char *iv, char *pan, char *bh, char *atc, char *plainData, char *P2, int *lenOfCiperData, char *ciperData)
{       
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	char keyString[64];
	int offset = 0; 

	// �������     
	memcpy(hsmCmdBuf,"VA",2);
	hsmCmdLen += 2;

	// ģʽ��ʶ     
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID       
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ       
	if ((mk == NULL) || (strlen(mk) == 0))
	{               
		sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}               
	else            
	{       
		ret = UnionGenerateX917RacalKeyString(strlen(mk),mk,keyString);
		keyString[ret] = 0;
		memcpy(hsmCmdBuf+hsmCmdLen, keyString, ret);
		hsmCmdLen += ret;
	}

	// IV-AC
	if ( id[0]=='0' )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	// PAN
	UnionForm16BytesDisperseDataOfV41A(strlen(pan), pan, pan);
	aschex_to_bcdhex(pan, 16, pan);
	memcpy(hsmCmdBuf+hsmCmdLen, pan, 8);
	hsmCmdLen += 8;

	// B/H
	if (id[0]=='0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, bh, 1);
		hsmCmdLen += 1;
	}

	// ATC
	aschex_to_bcdhex(atc, 4, atc);
	memcpy(hsmCmdBuf+hsmCmdLen, atc, 2);
	hsmCmdLen += 2;

	// PIN����
	memcpy(hsmCmdBuf+hsmCmdLen, plainData, strlen(plainData));
	hsmCmdLen += strlen(plainData);
	memcpy(hsmCmdBuf+hsmCmdLen, "F", 1);
	hsmCmdLen++;

	// P2(�ѻ�PINģʽ)
	memcpy(hsmCmdBuf+hsmCmdLen, P2, 1);
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdVA:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	*lenOfCiperData = ret - offset;
	memcpy(ciperData, hsmCmdBuf+offset, *lenOfCiperData);
	return (*lenOfCiperData);
}

/*
   ��������:
   PINBLOCK��ANSI9.8��ʽ�໥ת��
   �������:
zpk1: Դzpk
zpk2: Ŀ��zpk
srcPinBlock: ԴpinBlock
srcFormat: ԴpinBlock��ʽ    1: ANSI9.8��ʽ; 2: ����ũ��PINר�ø�ʽ
dstFormat: Ŀ��pinBlock��ʽ  1: ANSI9.8��ʽ; 2: ����ũ��PINר�ø�ʽ
accNo: �˺�
�������:
dstPinBlock: Ŀ��pinBlock
����ֵ:
�ɹ�>=0, ʧ��<0
 */
int UnionHsmCmdCD(char *zpk1,char *zpk2,char *srcPinBlock,int srcFormat,int dstFormat,char *accNo,char *dstPinBlock)
{
	int	ret = 0; 
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;

	if ((zpk1 == NULL) || (zpk2 == NULL) || (srcPinBlock == NULL) || (strlen(srcPinBlock) != 16) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdCD:: null pointer or len is error!\n");
		return(errCodeParameter);
	}

	// �������     
	memcpy(hsmCmdBuf,"CD",2);
	hsmCmdLen += 2;

	// 
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk1),zpk1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCD:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk2),zpk2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCD:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf + hsmCmdLen,srcPinBlock,16);
	hsmCmdLen += 16;

	sprintf(hsmCmdBuf + hsmCmdLen,"%d",srcFormat);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf + hsmCmdLen,"%d",dstFormat);
	hsmCmdLen += 1;

	if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCD:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCD:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(dstPinBlock,hsmCmdBuf + 4,16);
	dstPinBlock[16] = 0;

	return(16);
}

/*
   ����:��PIN�����������㷨��Anxi X9.8�㷨֮�����ת��
   ���������
zpk1:              ZPK1
zpk2:              ZPk2
changeFlag:        ת����־(00�������㷨תAnsiX9.8��01��AnsiX9.8ת�����㷨)
srcPinBlock:           16λ����Կ1���ܵ�PIN����
accNo:                 ���AnxiX9.8��ʽ���ʺ�
lenOfAccNo:            �˺ų���
���������
dstPinBlock: 16λ����Կ2���ܵ�PIN����
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
 */
int UnionHsmCmdG2(char *zpk1,char *zpk2,int changeFlag,char *srcPinBlock,char *accNo,int lenOfAccNo,char *dstPinBlock)
{
	int	ret = 0; 
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if ((zpk1 == NULL) || (zpk2 == NULL) || (srcPinBlock == NULL) || (strlen(srcPinBlock) != 16) || (accNo == NULL)) 
	{
		UnionUserErrLog("in UnionHsmCmdG2:: null pointer !\n");
		return(errCodeParameter);
	}

	// �������     
	memcpy(hsmCmdBuf,"G2",2);
	hsmCmdLen += 2;

	// 
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk1),zpk1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdG2:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk2),zpk2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdG2:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf + hsmCmdLen,srcPinBlock,16);
	hsmCmdLen += 16;

	sprintf(hsmCmdBuf + hsmCmdLen,"%02d",changeFlag);
	hsmCmdLen += 2;

	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdG2:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdG2:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(dstPinBlock,hsmCmdBuf + 4,16);
	dstPinBlock[16] = 0;

	return(16);
}

/*
   ����:��PIN����ת��ΪMD5����
   ���������
zpk:              ZPK
srcPinBlock:      PIN����
accNo:            ���AnxiX9.8��ʽ���ʺ�
lenOfAccNo:       �˺ų���
fillMode	  ���ģʽ��0������䣬1��PINǰ��䣬2��PIN�����
fillLen		  ǰ��䳤��
fillData	  ǰ�������
���������
dstPinBlock: 	  PIN����
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
���㷽��
1.      ��ԴZPK����Ϊ���ģ�
2.      ����ZPK���ġ�pinblock���˺Ž��ܵõ�PIN���ģ�
3.      ��PIN���Ľ���MD5��
4.      ���PIN��MD5�����
 */
int UnionHsmCmdS4(char *zpk,char *srcPinBlock,char *accNo,int lenOfAccNo,int fillMode,int fillLen,char *fillData,int tailFillLen, char *tailFill, char *dstPinBlock)
{
	int	ret = 0; 
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if ((zpk == NULL) || (srcPinBlock == NULL) || (strlen(srcPinBlock) != 16) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdS4:: null pointer or len is error!\n");
		return(errCodeParameter);
	}
	if (fillMode != 0 && fillData == NULL && strlen(fillData) < fillLen) 
	{
		UnionUserErrLog("in UnionHsmCmdS4:: fillData pointer or len is error!\n");
		return(errCodeParameter);
	}

	// �������     
	memcpy(hsmCmdBuf,"S4",2);
	hsmCmdLen += 2;

	// 
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdS4:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf + hsmCmdLen,srcPinBlock,16);
	hsmCmdLen += 16;

	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdS4:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;

	switch(fillMode)
	{
		case	0:
			break;
		case	1:
			memcpy(hsmCmdBuf + hsmCmdLen,";",1);
			hsmCmdLen += 1;
			sprintf(hsmCmdBuf + hsmCmdLen,"%02d",fillLen);
			hsmCmdLen += 2;
			memcpy(hsmCmdBuf + hsmCmdLen,fillData,fillLen);
			hsmCmdLen += fillLen;
			break;
		case	2:
			memcpy(hsmCmdBuf + hsmCmdLen,"M",1);
			hsmCmdLen += 1;
			sprintf(hsmCmdBuf + hsmCmdLen,"%02d",fillLen);
			hsmCmdLen += 2;
			memcpy(hsmCmdBuf + hsmCmdLen,fillData,fillLen);
			hsmCmdLen += fillLen;
			break;
		case	3:
			if(fillLen)
			{
				memcpy(hsmCmdBuf + hsmCmdLen,";",1);
				hsmCmdLen += 1;
				sprintf(hsmCmdBuf + hsmCmdLen,"%02d",fillLen);
				hsmCmdLen += 2;
				memcpy(hsmCmdBuf + hsmCmdLen,fillData,fillLen);
				hsmCmdLen += fillLen;
			}
			if(tailFillLen)
			{
				memcpy(hsmCmdBuf + hsmCmdLen,"M",1);
				hsmCmdLen += 1;
				sprintf(hsmCmdBuf + hsmCmdLen,"%02d",tailFillLen);
				hsmCmdLen += 2;
				memcpy(hsmCmdBuf + hsmCmdLen,tailFill,tailFillLen);
				hsmCmdLen += tailFillLen;
			}
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdS4:: fillMode [%d] is valid!\n",fillMode);
			return(errCodeParameter);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdS4:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(dstPinBlock,hsmCmdBuf + 4,32);
	dstPinBlock[32] = 0;

	return(32);
}

/*
   ���ܣ�ʹ�ö������ɵ���Կ����AnsiX9.9MACָ��
   ���������
   keyValue	32H	������Կ
   random1		16H	�����A
   random2		16H	�����B
   lenOfData	3N	MAC���ݳ���
   data		 	��չ��MAC����
   ���������
   mac		16H	MAC
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */
int UnionHsmCmdG3(char *keyValue,char *random1,char *random2,int lenOfData,char *data,char *mac)
{
	int	ret = 0; 
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;

	if ((keyValue == NULL) || (strlen(keyValue) != 32) || (random1 == NULL) || (strlen(random1) != 16) || (random2 == NULL) || (strlen(random2) != 16) || (data == NULL) || (lenOfData == 0))
	{
		UnionUserErrLog("in UnionHsmCmdG3:: null pointer or len is error!\n");
		return(errCodeParameter);
	}

	// �������     
	memcpy(hsmCmdBuf,"G3",2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf + hsmCmdLen,keyValue,32);
	hsmCmdLen += 32;

	memcpy(hsmCmdBuf + hsmCmdLen,random1,16);
	hsmCmdLen += 16;

	memcpy(hsmCmdBuf + hsmCmdLen,random2,16);
	hsmCmdLen += 16;

	if (((lenOfData % 16) != 0) || (lenOfData <= 0))
	{
		UnionUserErrLog("in UnionHsmCmdG3:: macData len[%d] error!\n",lenOfData);
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf + hsmCmdLen,"%03d",lenOfData);
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf + hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;

	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdG3:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(mac,hsmCmdBuf + 4,16);
	mac[16] = 0;

	return(16);
}

int UnionHsmCmdE0_GDSNX(int dataBlock, int CrypToFlag,int modeOfAlgorithm,
		char *ZEK, int inDataType,int outDataType,int padMode,char *padChar,int padCountFlag,char *IV,
		char *lenOfMSG,char *MSGBlock,char *datafmt,char *lenOfMSG1,char *MSGBlock1,char *IV1)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		len = 0,msglength = 0;

	if (ZEK==NULL || padChar==NULL || MSGBlock == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdE0_GDSNX:: parameter error!\n");
		return(errCodeParameter);
	}

	if (inDataType != 0 && inDataType != 1)
	{
		UnionUserErrLog("in UnionHsmCmdE0_GDSNX:: inDataType [%d] parameter error!\n", inDataType);
		return(errCodeParameter);
	}

	if (outDataType != 0 && outDataType != 1)
	{
		UnionUserErrLog("in UnionHsmCmdE0_GDSNX:: outDataType [%d] parameter error!\n",outDataType);
		return(errCodeParameter);
	}

	if ((modeOfAlgorithm != 1) && IV != NULL && (strlen(IV) != 16))
	{
		UnionUserErrLog("in UnionHsmCmdE0_GDSNX:: modeOfAlgorithm[%d] or IV parameter error!\n",modeOfAlgorithm);
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"E0",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",dataBlock);
	hsmCmdLen += 1;
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",CrypToFlag);
	hsmCmdLen += 1;
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",modeOfAlgorithm);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	hsmCmdLen += 1;
	switch (len = strlen(ZEK))
	{
		case	16:
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdE0_GDSNX:: invalid [%s]\n",ZEK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZEK,len);
	hsmCmdLen += len;
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",inDataType);
	hsmCmdLen += 1;
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",outDataType);
	hsmCmdLen += 1;
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",padMode);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,padChar,4);
	hsmCmdLen += 4;
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",padCountFlag);
	hsmCmdLen += 1;
	if (strlen(IV) == 16)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,IV,16);
		hsmCmdLen += 16;
	}

	memcpy(hsmCmdBuf+hsmCmdLen,lenOfMSG,3);
	hsmCmdLen += 3;

	msglength = UnionOxToD(lenOfMSG);
	if(inDataType == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,MSGBlock,msglength*2);
		hsmCmdLen += msglength*2;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,MSGBlock,msglength);
		hsmCmdLen += msglength;
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdE0_GDSNX:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(datafmt,hsmCmdBuf+4,1);
	memcpy(lenOfMSG1,hsmCmdBuf+4+1,3);
	lenOfMSG1[3] = 0;

	len = UnionOxToD(lenOfMSG1);

	// modify lisq 20141215 
	/*
	 *lenOfMSG1 = UnionOxToD(tmpBuf);
	 len = *lenOfMSG1;
	 */
	//*lenOfMSG1 = atoi(tmpBuf);
	//len = UnionOxToD(tmpBuf);
	// modify lisq 20141215 end

	if(outDataType == 0)   //modify by hzh in 2011.5.4,���Ӷ������ʽ���ж�
	{
		memcpy(MSGBlock1,hsmCmdBuf+4+1+3,len);
	}
	else{
		len = len * 2;
		memcpy(MSGBlock1,hsmCmdBuf+4+1+3,len);
	}

	if ((modeOfAlgorithm != 1 )&& (dataBlock == 1 || dataBlock == 2))
	{
		memcpy(IV1,hsmCmdBuf+4+1+3+len,16);
	}else
		strcpy(IV1,"");

	return(len);
}

/*
   ����: ��˽Կ���ܺ�ĵ�¼����ת��Ϊ������EDK����
   ���������
   fillMode	1N	��䷽ʽ
   vkIndex	2N	˽Կ����,��00������20�� �� ��99��Ϊ���ⲿ�����˽Կ
   lenOfPinByPK	4N	��Կ���ܵĵ�¼�����������ݳ���
   pinByPK	nB	��Կ���ܵĵ�¼������������
   keyLen	4N	˽Կ����
   keyValue	nB	������Կ���ܵ�˽Կ	
   edk		16H/32H/48H	edk��Կ
   ���������
   pinByEDK	16H	edk���ܵĵ�¼��������
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmd4A(int fillMode,int vkIndex,int lenOfPinByPK,char *pinByPK,int keyLen,char *keyValue,char *edk,char *pinByEDK)
{
	int		ret = 0;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		len = 0;

	if (pinByPK == NULL || edk == NULL)
	{
		UnionUserErrLog("in UnionHsmCmd4A:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"4A",2);
	hsmCmdLen = 2;

	// ���ģʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
	hsmCmdLen += 1;

	// ������
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;

	len = aschex_to_bcdhex(pinByPK,lenOfPinByPK,hsmCmdBuf+hsmCmdLen);	
	hsmCmdLen += len;

	if (vkIndex == 99)
	{
		if (keyLen <= 0 || keyValue == NULL)
		{
			UnionUserErrLog("in UnionHsmCmd4A:: keyLen[%d] <= 0 or keyValue == NULL!\n",keyLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",keyLen/2);
		hsmCmdLen += 4;

		len = aschex_to_bcdhex(keyValue,keyLen,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += len;
	}

	if ((ret = UnionGenerateX917RacalKeyString(strlen(edk),edk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4A:: UnionGenerateX917RacalKeyString [%s]\n",edk);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4A:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret = ret - 4;
	memcpy(pinByEDK,hsmCmdBuf+4,ret);
	pinByEDK[ret] = 0;

	return(ret);
}

/*
   ����:�����ܺ�ĵ�¼����(AN9.8)ת��Ϊ������pinoffset
   ���������
   zpk		16H/32H/48H	zpk��Կֵ
   accNo		12N		�˺�
   pinBlock	16H		zpk���ܵ�pin��������
   edk		16H/32H/48H	edk��Կ
   ���������
   pinOffset	16H	edk���ܵĵ�¼��������
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmd4B(char *zpk,char *accNo,char *pinBlock,char *edk,char *pinOffset)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;

	if (zpk == NULL || accNo == NULL || pinBlock == NULL || edk == NULL)
	{
		UnionUserErrLog("in UnionHsmCmd4B:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"4B",2);
	hsmCmdLen = 2;

	// zpk
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4B:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4B:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,16);
	hsmCmdLen += 16;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(edk),edk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4B:: UnionGenerateX917RacalKeyString [%s]\n",edk);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4B:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret = ret - 4;
	memcpy(pinOffset,hsmCmdBuf+4,ret);
	pinOffset[ret] = 0;
	return(ret);
}

/*
   ����: ���ɹ�Կ���ܵ�PINת����ANSI9.8��׼
   ���������
   vkIndex		2N		������
   keyType		1N		��Կ����, 1��TPK  2��ZPK
   keyValue	16H/32H/48H	��Կֵ
   pinType		1N		pin����,0��ԭʼPIN ,1��ANSI9.8
   fillMode	1N		������䷽ʽ,"0"��������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м���, "1"��PKCS��䷽ʽ
   accNo		16N		�˺�
   pinLen		4N		��Կ���ܵ�pin����	
   pinByPK		B		��Կ���ܵ�pin����
   ���������
   pinBlock	16H		keyValue���ܵ�pin
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */
int UnionHsmCmd4C(int vkIndex,int keyType,char *keyValue,int pinType,int fillMode,char *accNo,int pinLen,char *pinByPK,char *pinBlock)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		len = 0;

	if (keyValue == NULL || accNo == NULL || pinByPK == NULL)
	{
		UnionUserErrLog("in UnionHsmCmd4C:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"4C",2);
	hsmCmdLen = 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",keyType);
	hsmCmdLen += 1;

	// 
	if ((ret = UnionGenerateX917RacalKeyString(strlen(keyValue),keyValue,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4C:: UnionGenerateX917RacalKeyString [%s]\n",keyValue);
		return(ret);
	}
	hsmCmdLen += ret;

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",pinType);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
	hsmCmdLen += 1;

	// modify by leipp 20150716
	memcpy(hsmCmdBuf+hsmCmdLen,"0000",4);
	hsmCmdLen += 4;

	if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4C:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// modify end

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",pinLen/2);
	hsmCmdLen += 4;

	len = aschex_to_bcdhex(pinByPK,pinLen,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += len;

	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();
	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4C:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(pinBlock,hsmCmdBuf+4,16);
	pinBlock[16] = 0;

	return(16);
}
/*
   ����: ��˽Կ���ܺ�ĵ�¼����ת��Ϊ������EDK����
   ���������
   fillMode	1N	��䷽ʽ
   vkIndex	2N	˽Կ����,��00������20�� �� ��99��Ϊ���ⲿ�����˽Կ
   lenOfPinByPK	4N	��Կ���ܵĵ�¼�����������ݳ���
   pinByPK	nB	��Կ���ܵĵ�¼������������
   vkLen	4N	˽Կ����
   vk		nB	������Կ���ܵ�˽Կ	
   edk		16H/32H/48H	edk��Կ
   ���������
   pinByEDK	n*2H	edk���ܵĵ�¼��������
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmd4E(int fillMode,int vkIndex,int lenOfPinByPK,char *pinByPK,int vkLen,char *vk,char *edk,char *pinByEDK)
{
	int		ret = 0;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		len = 0;

	if (pinByPK == NULL || edk == NULL)
	{
		UnionUserErrLog("in UnionHsmCmd4E:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"4E",2);
	hsmCmdLen = 2;

	// ���ģʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
	hsmCmdLen += 1;

	// ������
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;

	len = aschex_to_bcdhex(pinByPK,lenOfPinByPK,hsmCmdBuf+hsmCmdLen);	
	hsmCmdLen += len;

	if (vkIndex == 99)
	{
		if (vkLen <= 0 || vk == NULL)
		{
			UnionUserErrLog("in UnionHsmCmd4E::vkLen[%d] <= 0 or vk == NULL!\n",vkLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLen/2);
		hsmCmdLen += 4;

		len = aschex_to_bcdhex(vk,vkLen,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += len;
	}

	if ((ret = UnionGenerateX917RacalKeyString(strlen(edk),edk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4E:: UnionGenerateX917RacalKeyString [%s]\n",edk);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4E:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret = ret - 4;
	memcpy(pinByEDK,hsmCmdBuf+4,ret);
	pinByEDK[ret] = 0;

	return(ret);
}

/*      
	����:	��C-mac�㷨����MAC 
	���������
	zakKey          32H             lmk(008)���ܵ�ZAK
	IV              16H             ��ʼ����                                            
	macData         NH              ����MAC����                        
	���������
	CMAC           16H             C-MAC          
	ICV		16H		ZAK��벿�ݣ�16λ����C-MAC DES��������   
	����ֵ��
	<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0������ִ�гɹ�
 */
int UnionHsmCmdCF(char *zakKey, char *IV, char *macData, char *CMAC, char *ICV)
{
	int 	ret;
	char	hsmCmdBuf[2048];	
	int 	hsmCmdLen = 0;

	if(zakKey == NULL || IV == NULL || macData == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdCF:: parameters err!\n");
		return(errCodeParameter);
	}

	//������
	memcpy(hsmCmdBuf, "CF", 2);
	hsmCmdLen += 2;

	//lmk(008)���ܵ�ZAK
	memcpy(hsmCmdBuf+hsmCmdLen, "X", 1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen, zakKey, 32);
	hsmCmdLen  += 32;

	//Mac��ʼ����
	memcpy(hsmCmdBuf+hsmCmdLen, IV, 16);
	hsmCmdLen += 16;

	//MAC���ݳ��ȣ�����8�ı���
	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", (int)strlen(macData)/2);
	hsmCmdLen += 4;

	//����MAC����
	memcpy(hsmCmdBuf+hsmCmdLen, macData, strlen(macData));
	hsmCmdLen += strlen(macData);

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCF:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(CMAC, hsmCmdBuf + 4, 16);
	memcpy(ICV, hsmCmdBuf + 20, 16);
	CMAC[16] = 0;
	ICV[16] = 0;

	return 0;

}

// add by lisq 20141117 ��֦������
/*
   ����
   SM2��Կ���ܵ�PIN������SM2˽Կ��SCE���ܺ�����SM4�㷨�������
   �������
   mode	�㷨��ʶ��0��SM2����תΪSM4���ܣ�1��SM2����תΪ��֦��ר���㷨���ܡ�
   vkIndex	˽Կ������01-20��99�������Կ
   vkLen	���˽Կ���ȡ�����vkIndexΪ99ʱ��
   vk	���˽Կ������vkIndexΪ99ʱ��
   cipherTextLen	���ĳ���
   cipherText	����
   zpkValue	ZPK��Կֵ
   accNo	�˺�
   �������
   pinBlkByZpk	ZPK���ܵ�PIN����

   ����ֵ
   >=0���ɹ���<0��ʧ��
 */

int UnionHsmCmdKH(int mode, int vkIndex, int vkLen, char *vk, int cipherTextLen, char *cipherText, char *zpkValue, char *accNo, char *pinBlkByZpk, int sizeofPinBlkByZpk)
{
	int 	ret;
	char	hsmCmdBuf[2048];	
	int 	hsmCmdLen = 0;

	if(cipherText == NULL || zpkValue == NULL || accNo == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdKH:: parameters err!\n");
		return(errCodeParameter);
	}

	if (vkIndex < 1 || (vkIndex > 20 && vkIndex != 99))
	{
		UnionUserErrLog("in UnionHsmCmdKH:: invalid vkIndex [%d]!\n", vkIndex);
		return(errCodeParameter);
	}


	//������
	memcpy(hsmCmdBuf, "KH", 2);
	hsmCmdLen += 2;

	// mode
	if (mode != 0 && mode != 1 && mode != 2)
	{
		UnionUserErrLog("in UnionHsmCmdKH:: invalid mode [%d]!\n", mode);
		return(errCodeParameter);
	}
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", mode);
	hsmCmdLen += 2;

	// vk index
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	hsmCmdLen += 2;

	if (vkIndex == 99)
	{
		// vk length
		sprintf(hsmCmdBuf+hsmCmdLen, "%04d", vkLen/2);
		hsmCmdLen += 4;

		// vk value
		aschex_to_bcdhex(vk, vkLen, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (vkLen/2);
	}

	// cipherTextLen
	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", cipherTextLen/2);
	hsmCmdLen += 4;

	// cipherText
	aschex_to_bcdhex(cipherText, cipherTextLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (cipherTextLen/2);

	if (mode == 0 || mode == 2)
	{
		//lmk���ܵ�ZPK
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen++;
		memcpy(hsmCmdBuf+hsmCmdLen, zpkValue, 32);
		hsmCmdLen  += 32;

		// accNo
		if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdKH::UnionForm12LenAccountNumber accNo [%s]!ret = [%d]\n", accNo, ret);
			return(ret);
		}
		hsmCmdLen += 12;
	}
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKH:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (mode == 0)
	{
		bcdhex_to_aschex(hsmCmdBuf+4+1, 32, pinBlkByZpk);
		ret = 64;
	}
	if (mode == 2)
	{
		bcdhex_to_aschex(hsmCmdBuf+4+1, 16, pinBlkByZpk);
		ret = (16)*2;
	}
	if (mode == 1)
	{
		bcdhex_to_aschex(hsmCmdBuf+4, ret-4, pinBlkByZpk);
		ret = (ret-4)*2;
	}
	return(ret);

}

/*
   ����
   ˽�м����㷨����pin
   �������
   dataLen	�������ݳ��ȣ���֧��6λ
   plainData	�������ݣ���֧��6λ
   sizeofCipherData	�����������������С
   �������
   cipherData	��������
   ����ֵ
   <0��ʧ��
   >=0���ɹ�
 */
int UnionHsmCmdWY(int dataLen, char *plainData, char *cipherData, int sizeofCipherData)
{
	int 	ret;
	char	hsmCmdBuf[2048];	
	int 	hsmCmdLen = 0;

	if(plainData == NULL || dataLen != 6)
	{
		UnionUserErrLog("in UnionHsmCmdWY:: parameters err!\n");
		return(errCodeParameter);
	}

	//������
	memcpy(hsmCmdBuf, "WY", 2);
	hsmCmdLen += 2;

	// dataLen 
	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", dataLen);
	hsmCmdLen += 4;

	// data
	memcpy(hsmCmdBuf+hsmCmdLen, plainData, dataLen);
	hsmCmdLen += dataLen;

	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWY:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	bcdhex_to_aschex(hsmCmdBuf+4, ret-4, cipherData);

	return(ret);

}

// add by lisq 20141117 end  ��֦������




int RacalCmdHN( char *key1,char *key2,char *date, char *flag, int len ,char *cdate ,int *dstlen,char *dstdate)
{
	int ret = 0;
	char hsmCmdBuf[2048];
	char tmplen[32];
	char tmpdstlen[32];
	int hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"HN",2);
	hsmCmdLen = 2;
	switch(strlen(key1))
	{
		case 16:
			break;
		case 32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen +=1;
			break;
		case 48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen +=1;
			break;
		default:
			UnionUserErrLog("in RacalCmdHN:: Wrong keylength!\n");
			return(errCodeEssc_KeyLength);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,key1,strlen(key1));
	hsmCmdLen += strlen(key1);
	memcpy(hsmCmdBuf+hsmCmdLen,"000",3);
	hsmCmdLen += 3;
	switch(strlen(key2))
	{
		case 16:
			break;
		case 32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen +=1;
			break;
		case 48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen +=1;
			break;
		default:
			UnionUserErrLog("in RacalCmdHN:: Wrong keylength!\n");
			return(errCodeEssc_KeyLength);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,key2,strlen(key2));
	hsmCmdLen += strlen(key2);
	memcpy(hsmCmdBuf+hsmCmdLen,date,32);
	hsmCmdLen +=32;
	memcpy(hsmCmdBuf+hsmCmdLen,flag,1);
	hsmCmdLen +=1;
	sprintf(tmplen,"%03d",len);
	memcpy(hsmCmdBuf+hsmCmdLen,tmplen,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,cdate,len*2);
	hsmCmdLen += len*2;

	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdHN:: UnionDirectHsmCmd!\n");
		return ret;
	}

	hsmCmdBuf[ret] = 0;
	memcpy(tmpdstlen,hsmCmdBuf+4,3);
	tmpdstlen[3] = 0;
	*dstlen=atoi(tmpdstlen)*2;
	memcpy(dstdate,hsmCmdBuf+4+3,*dstlen);
	return(ret);
}

int RacalCmdHM(char *key1, char *flag, char *date, int len ,char *macdate ,char *mac)
{
	int ret = 0;
	char hsmCmdBuf[2048];
	char tmplen[32];	
	int hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"HM",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,"000",3);
	hsmCmdLen += 3;
	switch(strlen(key1))
	{
		case 16:
			break;
		case 32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen +=1;
			break;
		case 48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen +=1;
			break;
		default:
			UnionUserErrLog("in RacalCmdHM:: Wrong keylength!\n");
			return(errCodeEssc_KeyLength);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,key1,strlen(key1));
	hsmCmdLen += strlen(key1);
	memcpy(hsmCmdBuf+hsmCmdLen,flag,1);
	hsmCmdLen +=1;
	memcpy(hsmCmdBuf+hsmCmdLen,date,32);
	hsmCmdLen += 32;
	sprintf(tmplen,"%03d",len/2);
	memcpy(hsmCmdBuf+hsmCmdLen,tmplen,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,macdate,len);
	hsmCmdLen += len;

	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdHM:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(mac,hsmCmdBuf+4,ret - 4);
	return(ret);
}

/*
   50ָ��
   ���ܣ���EDK��Կ�ӽ������ݣ�����ǽ���״̬�����������Ȩ�²��ܴ������򱨴�

   ��  ��  ��  Ϣ  ��  ʽ

   ������          ����    ����    ˵����
   ��Ϣͷ          m       A
   �������        2       A       ֵ"50"
   Flag            1       N       0������
   1������
   EDK             16 or 32
   1A+32 or 1A+48
   H       LMK24-25����
   DATA_length     4       N       ���������ֽ�����8�ı�������Χ��0008-4096
   DATA            n*2     H       �����ܻ���ܵ����ݣ���ASC���ʾ��

   ��  ��  ��  Ϣ  ��  ʽ

   �����  ����    ����    ˵����
   ��Ϣͷ          M       A
   ��Ӧ����        2       A       ֵ"51"
   �������        2       N
   DATA            n*2     H       ������

 */
int RacalCmd50ForHR(char flag,char *edk,int lenOfData,char *indata,char *outdata,int *sizeOfOutData)
{
	int             ret;
	char            hsmCmdBuf[8096+40];
	int             hsmCmdLen = 0;
	int             retLen;
	int             keyLen;

	memcpy(hsmCmdBuf,"50",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	switch (keyLen = strlen(edk))
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in RacalCmd50ForHR:: edk [%s] length error!\n",edk);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,edk,keyLen);
	hsmCmdLen += keyLen;
	if ((lenOfData % 8 != 0) || (lenOfData <= 0) || (lenOfData > 8096))
	{
		UnionUserErrLog("in RacalCmd50ForHR:: lenOfData [%04d] error!\n",lenOfData);
		return(errCodeParameter);
	}
	switch (flag)
	{
		case    '0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData/2);
			hsmCmdLen += 4;
			//bcdhex_to_aschex(indata,lenOfData,hsmCmdBuf+hsmCmdLen);
			memcpy(hsmCmdBuf+hsmCmdLen,indata,lenOfData);
			hsmCmdLen += (lenOfData);
			break;
		case    '1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData/2);
			hsmCmdLen += 4;
			memcpy(hsmCmdBuf+hsmCmdLen,indata,lenOfData);
			hsmCmdLen += lenOfData;
			break;
		default:
			UnionUserErrLog("in RacalCmd50ForHR:: flag [%c] error!\n",flag);
			return(errCodeParameter);
	}
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmd50ForHR:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	retLen = ret - 4;
	memcpy(outdata,hsmCmdBuf+4,retLen);
	*sizeOfOutData=retLen;
	return(retLen);
}


/*
   ����ָ���Գ���Կָ���㷨����MAC

   ���������
   modeFlag��1������MAC��2����֤MAC
   algFlag���㷨��־��01��DES_MAC��02��DES_NAMC_EMV��03��AES_MAC��04��AES_MAC_EMV
   keyType����Կ���͡�000��
   keyLen����Կ���ȱ�־��0��������������algFlagΪ01��02����1��˫����
   keyValue����Կֵ��16H/32H
   iv����ʼ������16H
   macDataLen��MAC���ݳ���
   macData��MAC����
   macLen��MAC���ȣ�����modeFlagΪ2ʱ��
   mac��MAC(����modeFlagΪ2ʱ��

   ���������
   mac��MAC(����modeFlagΪ1ʱ

   ����ֵ��

   < 0��ʧ�ܣ�
   =< 0���ɹ�
 */

int RacalCmdTQ(int modeFlag, char *algFlag, char *keyType, int keyLen, char *keyValue, char *iv, int macDataLen, char *macData, int macLen, char *mac)
{
	char    hsmCmdBuf[2048];
	int     hsmCmdLen = 0;
	int     ret = 0;
	char    tmpBuf[2848];

	if ((modeFlag != 1 && modeFlag != 2) || algFlag == NULL || keyType == NULL || keyValue == NULL || iv == NULL || macLen < 0 || macData == NULL)
	{
		UnionUserErrLog("in RacalCmdTQ:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "TQ", 2);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", modeFlag);
	hsmCmdLen++;

	memcpy(hsmCmdBuf+hsmCmdLen, algFlag, 2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", keyLen);
	hsmCmdLen++;

	memcpy(hsmCmdBuf+hsmCmdLen, keyValue, strlen(keyValue));
	hsmCmdLen += strlen(keyValue);

	memcpy(hsmCmdBuf+hsmCmdLen, iv, 16);
	hsmCmdLen += 16;

	if (modeFlag == 2)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", macLen/2);
		hsmCmdLen++;

		aschex_to_bcdhex(mac, macLen, tmpBuf);
		memcpy(hsmCmdBuf+hsmCmdLen, tmpBuf, macLen/2);
		hsmCmdLen += macLen/2;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", macDataLen/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(macData, macDataLen, tmpBuf);
	memcpy(hsmCmdBuf+hsmCmdLen, tmpBuf, macDataLen/2);
	hsmCmdLen += macDataLen/2;

	bcdhex_to_aschex(hsmCmdBuf, hsmCmdLen, tmpBuf);

	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdTQ::UnionDirectHsmCmd!ret = [%d]!\n", ret);
		return(ret);
	}

	hsmCmdBuf[ret] = 0;

	if (modeFlag == 1)
	{
		memcpy(mac, hsmCmdBuf+4, 16);
		return(16);
	}

	return(0);
}

// modify by zhouxw 20160105, ��TSָ���Ϊ��д��ģʽ����Կ����
//int RacalCmdTS( char *key1,char *key2,int blen,char *bdate,int alen,char *adate, char *dstkey)
int RacalCmdTS(char *mode, char *key1,char *key2,char *IV,int blen,char *bdate,int alen,char *adate, char *dstkey)
{
	int	ret = 0;
	int	len;
	char	hsmCmdBuf[2048];
	char	tmplen[32];
	int hsmCmdLen = 0;

	if( mode == NULL || key1 == NULL || key2 == NULL || dstkey == NULL)
	{
		UnionUserErrLog("in RacalCmdTS:: Null pointer!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "TS", 2);
	hsmCmdLen += 2;
	// ģʽ
	if(strcmp(mode, "00") != 0 && strcmp(mode, "01") != 0 && strcmp(mode, "02") != 0 && strcmp(mode, "10") != 0 && strcmp(mode, "11") != 0 && strcmp(mode, "12") != 0)
	{
		UnionUserErrLog("in RacalCmdTS:: mode error!\n");
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 2);
	hsmCmdLen += 2;
	// Դ��Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, "000", 3);
	hsmCmdLen += 3;
	// Դ��Կ����
	switch(strlen(key1))
	{
		case	16:
			memcpy(hsmCmdBuf+hsmCmdLen, "0", 1);
			hsmCmdLen += 1;
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen, "1", 1);
			hsmCmdLen += 1;
			break;
		default:
			UnionUserErrLog("in RacalCmdTS:: key1 Len error!\n");
			return(errCodeParameter);
	}
	// Դ��Կ
	memcpy(hsmCmdBuf+hsmCmdLen, key1, strlen(key1));
	hsmCmdLen += strlen(key1);
	// Ŀ����Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, "000", 3);
	hsmCmdLen += 3;
	// Ŀ����Կ����
	switch(strlen(key2))
	{
		case	16:
			memcpy(hsmCmdBuf+hsmCmdLen, "0", 1);
			hsmCmdLen += 1;
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen, "1", 1);
			hsmCmdLen += 1;
			break;
		default:
			UnionUserErrLog("in RacalCmdTS:: key1 Len error!\n");
			return(errCodeParameter);
	}
	// Ŀ����Կ
	memcpy(hsmCmdBuf+hsmCmdLen, key2, strlen(key2));
	hsmCmdLen += strlen(key2);
	if(memcmp(mode, "10", 2) == 0 || memcmp(mode, "11", 2) == 0 || memcmp(mode, "12", 2) == 0)
	{
		// IV ��ʼ����
		memcpy(hsmCmdBuf+hsmCmdLen, IV, 16);
		hsmCmdLen += 16;
	}
	//ǰ׺
	if(blen != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",blen);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen, bdate, blen);
		hsmCmdLen += blen;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "0000", 4);
		hsmCmdLen += 4;
	}
	//��׺
	if(alen !=0 )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04d",alen);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen, adate, alen);
		hsmCmdLen += alen;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "0000", 4);
		hsmCmdLen += 4;
	}
	// �������ͨѶ
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd( hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdTS:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(tmplen,hsmCmdBuf+4,2);
	len = UnionConvertIntStringToInt(tmplen,2);

	memcpy(dstkey,hsmCmdBuf+4+2,len);

	// modify by leipp 20150330 begin �޸ķ���ֵΪ��Կ����
	return(len);
	// modify end
}

/*
   ������ר���㷨���ܵ�PINתΪ ZPK����(HI)
   �������:
   factorOfKey     ����ר���㷨��Կ����
   keyOfZPK                ���ڼ��ܵ�Ŀ��zpk
   pan                                             12λʵ�ʲ��������˺�
   pin                                             ������ר���㷨���ܵ�PIN��������

   �������:
   pinBlock                        zpk��Կ���ܵ�pin
   errCode��                       �����������
   ����ֵ:
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */
int RacalCmdHI(char *factorOfKey, char *keyOfZPK, char *pan, char *pin ,char *pinBlock)
{
	int ret = 0;
	char hsmCmdBuf[2048];

	int keyLen=0;
	int hsmCmdLen = 0;


	if( factorOfKey == NULL || keyOfZPK == NULL || pan == NULL || pin == NULL )
	{
		UnionUserErrLog("in RacalCmdHI:: Null pointer!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "HI", 2);
	hsmCmdLen += 2;
	// ��Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, factorOfKey, 8);
	hsmCmdLen += 8;
	// Ŀ��zpk
	keyLen=strlen(keyOfZPK);
	switch (keyLen)
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in RacalCmdHI:: keyOfZPK [%s]\n",keyOfZPK);
			return(errCodeParameter);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, keyOfZPK, keyLen);
	hsmCmdLen += keyLen;
	// pan
	if ((ret = UnionForm12LenAccountNumber(pan, strlen(pan), hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdHI:: UnionForm12LenAccountNumber for [%s]\n",pan);
		return(ret);
	}
	hsmCmdLen += ret;
	// Դpin����
	memcpy(hsmCmdBuf+hsmCmdLen, pin, strlen(pin));
	hsmCmdLen += strlen(pin);

	// �������ͨѶ
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdHI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	//Ŀ��zpk���ܵ�����
	// pinBlock
	sprintf(pinBlock,"%s", hsmCmdBuf+4);
	return (ret);
}



/*
   �ѹ�Կ���ܵĵ�¼����תΪ�����¼����ר���㷨(HJ)
   �������:
   vkIndex                         ˽Կ����(00-20��99��ʾRSA���)
   lenOfVK       ˽Կ���ȣ�ֻ�е�˽Կ����Ϊ99ʱ�����д�ֵ��
   valueOfVK     ˽Կֵ(ֻ�е�˽Կ����Ϊ99ʱ,���д�ֵ��
   flag          ��Կ����������䷽ʽ
   keyOfZPK                ���ڼ��ܵ�Ŀ��zpk
   lenOfPin      ��Կ�������ĳ���
   pinBlock      ��Կ���ܵ�����
   clientNo      �ͻ���

   �������:
   pinOfHR                   ����ר���㷨����
   replayFactor  �ط�����
   errCode��                       �����������
   ����ֵ:
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */
int RacalCmdHJ(char *vkIndex, int lenOfVK, char *valueOfVK, char *flag ,char *keyOfZPK ,char *lenOfPin,char *pinBlock,char *clientNo,char *pinOfHR,char *replayFactor)
{
	int 	ret = 0;
	char 	hsmCmdBuf[8192];
	char 	tmplen[32];
	int     hsmCmdLen = 0;
	int 	keyLen=0;


	if( vkIndex == NULL || flag == NULL || keyOfZPK == NULL || lenOfPin == NULL || pinBlock == NULL || clientNo == NULL)
	{
		UnionUserErrLog("in RacalCmdHJ:: Null pointer!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "HJ", 2);
	hsmCmdLen += 2;
	// ˽Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, vkIndex, 2);
	hsmCmdLen += 2;
	/*
	   if( memcmp(vkIndex,"99",2) == 0 )
	   {
	//˽Կ���ȣ���˽Կ����Ϊ99ʱ�и���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d", lenOfVK);
	hsmCmdLen+=4;
	//˽Կֵ(ֻ�е�˽Կ����Ϊ99ʱ,���д�ֵ��
	aschex_to_bcdhex(valueOfVK,strlen(valueOfVK),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfVK;
	}
	 */
	//��Կ����������䷽ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,flag,1);
	hsmCmdLen++;
	//Ŀ��ZPK
	keyLen=strlen(keyOfZPK);
	switch (keyLen)
	{
		case    16:
			break;
		case    32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case    48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in RacalCmdHJ:: keyOfZPK = [%s]\n",keyOfZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, keyOfZPK, keyLen);
	hsmCmdLen += keyLen;
	// ��Կ�������ĳ���
	memcpy(hsmCmdBuf+hsmCmdLen,lenOfPin,4);
	hsmCmdLen += 4;
	//��Կ���ܵ�����
	aschex_to_bcdhex(pinBlock,strlen(pinBlock),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += atoi(lenOfPin);
	//�ͻ���
	aschex_to_bcdhex(clientNo,strlen(clientNo),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += strlen(clientNo)/2;

	// �������ͨѶ
	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdHJ:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;


	//memcpy(pinOfHR,hsmCmdBuf+4,48);
	memcpy(tmplen,hsmCmdBuf+4+32,2);

	if((ret-4-32-2) == UnionConvertIntStringToInt(tmplen,2))
	{
		//����ר���㷨����
		memcpy(pinOfHR,hsmCmdBuf+4,32);
		//�ط�����(?�ط�����)
		memcpy(replayFactor,hsmCmdBuf+4+32+2,ret-4-32-2);
		return(ret);
	}
	else
	{
		//����ר���㷨����
		memcpy(pinOfHR,hsmCmdBuf+4,48);
		memcpy(replayFactor,hsmCmdBuf+4+48+2,ret-4-48-2);
		return(ret);
	}
	return(0);
}



/*              

		�ѹ�Կ���ܵ��ڹ�����תΪHash�е�SHA1�㷨����

		������  ����    ˵����
		�������        2A      ֵ��HK��
		˽Կ����        2N      00-20��99��ʾRSA���
		˽Կ����        4N      ����99��ʾRSA������޴���
		˽Կ    nB      ����99��ʾRSA������޴���
		��Կ����������䷽ʽ    1N      0��0��䷽ʽ
		1��PKCS��䷽ʽ

		��Կ�������ĳ���        4N       
		��Կ���ܵ�����  nB
		��Ա���        nB      


		�����  ����    ˵����
		��Ӧ����        2       ֵ��HL��eo
		�������        2       10����Կ��������У��
		12��������Կ
		15���������ݴ�
		24��PIN���ȴ�
		�ط����ӳ���    N       �ط����ӳ���ֵ
		�ط�����        nH      �ط�����
		Hash�е�SHA1�㷨���ܵ�����      nH      Hash�е�SHA1�㷨���ܵ�����


 */     
int RacalCmdHK(char *index,char * keylen, char *key,char *flag,  char * datalen, char *data, char *num,char *genelen,char *gene,char *pin)        
{       
	char hsmCmdBuf[4096];
	char tmpbuf[4096];
	int hsmCmdLen = 0;
	int ret;
	int numLen;
	int tmplen;
	int intdatalen,intkeylen;
	int tmpint;
	if ((flag == NULL) || (index == NULL) || (data == NULL) || (datalen == NULL)
			|| (key == NULL) || (num == NULL) )
	{
		UnionUserErrLog("in RacalCmdHK:: parameter error!\n");
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "HK", 2);      //�������
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf + hsmCmdLen, index, 2);  //index
	hsmCmdLen += 2;

	//indexΪ99ʱ��������
	if (strncmp(index,"99",2) == 0)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, keylen, 4);  //keylen
		hsmCmdLen += 4;


		intkeylen = UnionConvertIntStringToInt(keylen,4);
		memcpy(hsmCmdBuf + hsmCmdLen, key, intkeylen*2);  //key
		tmpint = aschex_to_bcdhex(key,strlen(data),hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += tmpint;

	}

	memcpy(hsmCmdBuf + hsmCmdLen, flag, 1);  //flag
	hsmCmdLen += 1;


	memcpy(hsmCmdBuf + hsmCmdLen, datalen, 4);  //datalen
	hsmCmdLen += 4;

	//data
	intdatalen = UnionConvertIntStringToInt(datalen,4);
	memcpy(hsmCmdBuf + hsmCmdLen, data, intdatalen*2);  //data
	tmpint = aschex_to_bcdhex(data,strlen(data),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += tmpint;


	numLen = strlen(num);
	memcpy(hsmCmdBuf + hsmCmdLen, num, numLen);  //num
	hsmCmdLen += numLen;
	bcdhex_to_aschex(hsmCmdBuf,hsmCmdLen,tmpbuf);

	/*�����������������*/
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdHK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = '\0';
	memcpy(genelen, hsmCmdBuf+4, 2);  //genelen
	tmplen = UnionConvertIntStringToInt(genelen,2);
	memcpy(gene, hsmCmdBuf+6, tmplen);  //gene
	memcpy(pin,hsmCmdBuf+6+tmplen,ret-6-tmplen);//pin

	return (ret);
}


/*
   ���ܣ���zpk���ܵ��ַ���������ת��Ϊ��һzpk���ܵ��ַ���������
   �������:
   cpOriZPK ��           ԴZPK����ǰ����PIN���ZPK��LMK�ԣ�06-07���¼���
   cpDestZPK ��          Ŀ��ZPK����Ҫ����PIN���ZPK��LMK�ԣ�06-07���¼���
   conEsscFldAccNo ��    �ʺţ�conEsscFldAccNo��
   cpOriPIN ��           ԴPIN��

   �������:
   errCode��               �����������
   cpOutPIN��              PIN����

   ����ֵ:
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */
int RacalCmdHB( char *cpOriZPK, char *cpDestZPK, char *AccNo, char *cpOriPIN,char *cpOutPIN)
{
	int ret = 0;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;

	if( cpOriZPK == NULL || cpDestZPK == NULL || cpOriPIN == NULL || AccNo == NULL )
	{
		UnionUserErrLog("in RacalCmdHB:: parameter error!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "HB", 2);
	hsmCmdLen += 2;

	// ԴZPK
	if( (ret = UnionPutKeyIntoRacalKeyString(cpOriZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen)) < 0 )
	{
		UnionUserErrLog("in RacalCmdHB:: UnionPutKeyIntoRacalKeyString cpOriZPK [%s]!\n", cpOriZPK);
		return (ret);
	}
	hsmCmdLen += ret;

	//  Ŀ��ZPK
	if( (ret = UnionPutKeyIntoRacalKeyString(cpDestZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen)) < 0 )
	{
		UnionUserErrLog("in RacalCmdHB:: UnionPutKeyIntoRacalKeyString cpDestZPK [%s]!\n", cpDestZPK);
		return (ret);
	}
	hsmCmdLen += ret;

	// �ʺ�
	if( (ret = UnionForm12LenAccountNumber(AccNo, strlen(AccNo), hsmCmdBuf+hsmCmdLen)) < 0 )
	{
		UnionUserErrLog("in RacalCmdHB:: UnionForm12LenAccountNumber !\n");
		return (ret);
	}
	hsmCmdLen += ret;


	// ԴPIN��
	if( strlen(cpOriPIN) != 16 )
	{
		UnionUserErrLog("in RacalCmdHB:: cpOriPIN is error!\n");
		return (errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, cpOriPIN, 16);
	hsmCmdLen += 16;


	// �������ͨѶ
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdHB:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(cpOutPIN, hsmCmdBuf+4, 16);

	return (0);
}

/*
   ��zpk���ܵ�PIN��ANSI9.8��׼�����˺ţ�ת�ɻ������ר���㷨���ܡ�

   �������:
   ԴZPK��Կ����:ԴZPK��Կ����
   Դpin����:ԴZpk���ܵ�PIN���ģ�16λ����
   �˺����ͣ�1����13λ�˺ţ�2����19λ���š�
   �˺�:�˺ŷ�13λ��19λ���֣��ڡ�HE��ָ����Ҫ���������˺š�

   ���ز���:
   �������ר���㷨���ܵ�����:�������ר���㷨���ܵ����ģ�8λ����

   ����ֵ:
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�

 */

int RacalCmdHE( char *zpkvalue, char* srcpinblock, int flag,char *account,char *dstpinbolck)
{
	char hsmCmdBuf[2048];
	int hsmCmdLen = 0;
	int ret;

	if ((zpkvalue == NULL) || (srcpinblock == NULL)|| (account == NULL))
	{
		UnionUserErrLog("in RacalCmdHE:: parameter error!\n");
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf,"HE", 2);      /*�������*/
	hsmCmdLen += 2;
	if ((ret = UnionPutKeyIntoRacalKeyString(zpkvalue, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) 
	{
		UnionUserErrLog("in RacalCmdHE:: UnionPutKeyIntoRacalKeyString [%s]!\n", zpkvalue);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf + hsmCmdLen, srcpinblock, 16);  //pin����
	hsmCmdLen += 16;
	if (flag == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "1", 1);      /*�˺ű�־*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,13);
		hsmCmdLen += 13;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "2", 1);      /*�˺ű�־*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,19);
		hsmCmdLen += 19;
	}


	/*�����������������*/
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd( hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdHE:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = '\0';

	memcpy(dstpinbolck, hsmCmdBuf + 4, 16);  /*dstpinbolck*/
	return(0);


}

/*
   ��zpk���ܵ�PIN��ANSI9.8��׼�����˺ţ�ת�ɻ������ר���㷨���ܡ�

   �������:
   ԴZPK��Կ����:ԴZPK��Կ����
   Դpin����:ԴZpk���ܵ�PIN���ģ�16λ����
   �˺����ͣ�1����13λ�˺ţ�2����19λ���š�
   �˺�:�˺ŷ�13λ��19λ���֣��ڡ�HE��ָ����Ҫ���������˺š�

   ���ز���:
   �������ר���㷨���ܵ�����:�������ר���㷨���ܵ����ģ�8λ����

   ����ֵ:
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�

 */
int RacalCmdHF( char *zpkvalue, int flag,char *account,char *gene,char *encrypteddata,char *dstpinbolck,char *nodeid)
{
	char hsmCmdBuf[2048];
	int hsmCmdLen = 0;
	int ret;

	if ((zpkvalue == NULL) ||  (account == NULL)|| (gene == NULL) || (encrypteddata == NULL) )
	{
		UnionUserErrLog("in RacalCmdHF:: parameter error!\n");
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf, "HF", 2);      /*�������*/
	hsmCmdLen = 2;

	//ƴZPKVALUE
	if ((ret = UnionPutKeyIntoRacalKeyString(zpkvalue, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*zpkvalue*/
	{
		UnionUserErrLog("in RacalCmdHF:: UnionPutKeyIntoRacalKeyString [%s]!\n", zpkvalue);
		return(ret);
	}
	hsmCmdLen += ret;

	if (flag == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "1", 1);      /*�˺ű�־*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,13);
		hsmCmdLen += 13;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "2", 1);      /*�˺ű�־*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,19);
		hsmCmdLen += 19;
	}

	//ƴ��������
	memcpy(hsmCmdBuf + hsmCmdLen, gene, 8);
	hsmCmdLen += 8;

	//ƴ��������
	if ((ret = UnionPutKeyIntoRacalKeyString(encrypteddata, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*zpkvalue*/
	{
		UnionUserErrLog("in RacalCmdHE:: UnionPutKeyIntoRacalKeyString [%s]!\n", encrypteddata);
		return(ret);
	}
	hsmCmdLen += ret;

	/*�����������������*/
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd( hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdHF:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = '\0';

	memcpy(dstpinbolck, hsmCmdBuf + 4, 16);  /*dstpinbolck*/

	memcpy(nodeid, hsmCmdBuf + 20, 4);  /*nodeid*/
	return (0);

}




/*
   ���ܣ���Կ���ܵ�PIN����תΪDES��Կ���ܣ������Ӽ��ܣ���
   �������:
   srcAlgorithmID  �ǶԳ���Կ�㷨��ʶ,0:�����㷨,  1:�����㷨
   vkIndex         ָ����˽Կ�����ڽ���PIN��������
   lenOfVK	˽Կ����
   vkValue	˽Կֵ
   flag         ��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
		��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   dstAlgorithmID ZPK��Կ�㷨��ʶ,0:�����㷨,  1:�����㷨
keyValue:       ���ڼ���PIN����Կ
pan             �û���Ч���ʺų���
lenOfPinByPK    ����Կ���ܵ�PIN�������ĳ���
pinByPk         ����Կ���ܵ�PIN��������
�������:
lenOfPinBlock   des��Կ���ܵ�pin����
pinBlock        des��Կ���ܵ�pin
idCodeLen       01-20
idCode          ID������
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
 */
int RacalCmdN8(int srcAlgorithmID,int vkIndex,int lenOfVK,char *vkValue, char flag, int dstAlgorithmID,char *keyValue, char *pan, int lenOfPinByPK,char *pinByPk,int *lenOfPinBlock, char *pinBlock, int *idCodeLen, char *idCode)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	char	szBuf[128];
	int	offset = 0;
	int	len = 0;

	if( keyValue == NULL || pan == NULL || pinByPk == NULL ||((flag!='0')&&(flag!='1'))) 
	{
		UnionUserErrLog("in RacalCmdN8:: Null pointer!\n");
		return (errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	// �������
	memcpy(hsmCmdBuf, "N8", 2);
	hsmCmdLen = 2;

	if (srcAlgorithmID == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
	}

	// vkIndex
	if (vkIndex == 99)
	{
		if (lenOfVK <= 0 || vkValue == NULL)
		{
			UnionUserErrLog("in RacalCmdN8:: lenOfVK[%d] <= 0 or vkValue == null parameter error!\n",lenOfVK);
			return(errCodeParameter);
		}

		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
		hsmCmdLen += 2;

		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue,lenOfVK,hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += lenOfVK/2;
	}
	else
	{
		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%02d", vkIndex);
		hsmCmdLen += 2;
	}

	// flag
	if (srcAlgorithmID == 0)
	{
		*(hsmCmdBuf+hsmCmdLen) = flag;
		hsmCmdLen++;
	}

	if (dstAlgorithmID == 0)
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(keyValue, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdN8:: UnionPutKeyIntoRacalKeyString [%s]!\n", keyValue);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen,keyValue,32);
		hsmCmdLen += 32;
	}

	// pan
	if ((ret = UnionForm12LenAccountNumber(pan, strlen(pan), hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdN8:: UnionForm12LenAccountNumber for [%s][%d]\n",pan,(int)strlen(pan));
		return(ret);
	}
	hsmCmdLen += ret;
	// pinByPk
	memcpy(hsmCmdBuf+hsmCmdLen, pinByPk, lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd( hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdN8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	//lenOfPinBlock
	offset = 4;
	memcpy(szBuf, hsmCmdBuf+offset, 2);
	szBuf[2] = 0;
	offset += 2;
	*lenOfPinBlock = atoi(szBuf);

	// pinBlock
	//memset(szBuf, 0, sizeof(szBuf));
	//memcpy(szBuf, hsmCmdBuf+4+2, 16);
	if (dstAlgorithmID == 0)
	{
		len = 16;
	}
	else
	{
		len = 32;
	}
	memcpy(pinBlock, hsmCmdBuf+offset, len);
	pinBlock[len] = 0;
	offset += len;

	// idCodeLen
	memcpy(szBuf, hsmCmdBuf+offset, 2);
	szBuf[2] = 0;
	offset += 2;
	*idCodeLen = atoi(szBuf);
	len = (*idCodeLen) * 2;

	// idCode
	memcpy(idCode, hsmCmdBuf+offset,len);
	idCode[len] = 0;

	return (0);
}

// add by lisq 20150120 ΢������

int UnionHsmCmdKJ(int mode, int flag, char *mkType, char *zek, char *mk, int divNum, char *divData, char *pk, int *keyByPkLen, char *keyByPk)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0,lenOfPKStr = 0;
	char	tmpBuf[4096];

	if (mode < 0 || (flag != 0 && flag != 1) || divNum < 0 || divData == NULL || pk == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdKJ::parameter error!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "KJ", 2);
	hsmCmdLen += 2;

	// mode
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", mode);
	hsmCmdLen += 2;

	// flag
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", flag);
	hsmCmdLen++;

	// mkType
	if (flag == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mkType, strlen(mkType));
		hsmCmdLen += strlen(mkType);
	}
	else
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(zek, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdKJ::UnionPutKeyIntoRacalKeyString key [%s]!ret = [%d]\n", zek, ret);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// mk
	if ((ret = UnionPutKeyIntoRacalKeyString(mk, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKJ::UnionPutKeyIntoRacalKeyString key [%s]!ret = [%d]\n", mk, ret);
		return(ret);
	}
	hsmCmdLen += ret;

	// divNum
	if (divNum < 2 || divNum >5)
	{
		UnionUserErrLog("in UnionHsmCmdKJ:: divNum [%d] must between 2 to 5!\n", divNum);
		return(errCodeParameter);
	}
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", divNum);
	hsmCmdLen++;

	// divData
	memcpy(hsmCmdBuf+hsmCmdLen, divData, strlen(divData));
	hsmCmdLen += strlen(divData);

	// pk
	if ((lenOfPKStr = UnionFormANSIDERRSAPK(pk, strlen(pk), hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKJ::UnionFormANSIDERRSAPK pk [%s]!ret = [%d]\n", pk, lenOfPKStr);
		return(lenOfPKStr);
	}
	hsmCmdLen += lenOfPKStr;

	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKJ::UnionDirectHsmCmd!ret = [%d]\n", ret);
		return(ret);
	}

	// keyByPkLen
	memcpy(tmpBuf, hsmCmdBuf+4, 4);
	tmpBuf[4] = 0;
	*keyByPkLen = atoi(tmpBuf);

	// keyByPK
	memcpy(tmpBuf, hsmCmdBuf+4+4, *keyByPkLen);
	bcdhex_to_aschex(tmpBuf, *keyByPkLen, keyByPk);

	return(*keyByPkLen);
} 


// add by lisq 20150120 end ΢������

int UnionHsmCmd8A(int mode, int vkIndex,int vkLen,char *vkValue,int  fillMode,int lenOfPinByPK, char *pinByPK, char *factorData,int keyLen, char *keyValue, char *randomData, int lenOfAccNo,char *accNo, char *dataByZEK,char *digest, char *pinBlock)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	offset = 0;

	if ((mode < 1 || mode > 3) || vkIndex < 0 || (vkIndex > 20 && vkIndex != 99) || factorData == NULL || strlen(factorData) != 8)
	{
		UnionUserErrLog("in UnionHsmCmd8A::parameter error!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "8A", 2);
	hsmCmdLen += 2;

	// mode
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mode);
	hsmCmdLen += 1;

	// vkIndex
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	hsmCmdLen += 2;

	// ˽Կֵ
	if (vkIndex == 99)
	{
		if (vkLen <= 0 || vkValue == NULL)
		{
			UnionUserErrLog("in UnionHsmCmd8A:: vkLen[%d] <= 0 or vkValue == NULL parameter error!\n",vkLen);
			return (errCodeParameter);
		}

		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue,vkLen,hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}

	// ���ģʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
	hsmCmdLen += 1;

	// ��Կ���ܵ����ݳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);	
	hsmCmdLen += 4;

	// ��Կ���ܵ���������
	aschex_to_bcdhex(pinByPK,lenOfPinByPK,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// ���ط�����
	memcpy(hsmCmdBuf+hsmCmdLen,factorData ,8);
	hsmCmdLen += 8;

	// ��Կֵ
	if ((ret = UnionGenerateX917RacalKeyString(keyLen,keyValue,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8A:: UnionGenerateX917RacalKeyString for [%d][%s]\n",keyLen,keyValue);
		return(ret);
	}
	hsmCmdLen += ret;

	// PIN�����������
	if (mode == 2)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, randomData, 16);
		hsmCmdLen += 16;
	}

	// �˺�
	if (mode == 3)
	{
		if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmd8A:: UnionForm12LenAccountNumber for [%s]\n",accNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8A::UnionDirectHsmCmd!ret = [%d]\n", ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	offset = 4;
	if (mode == 1)
	{
		memcpy(dataByZEK, hsmCmdBuf+offset, 16);
		offset += 16;
		dataByZEK[16] = 0;
	}

	if (mode == 2 || mode == 1)
	{
		memcpy(digest, hsmCmdBuf+offset, 40);
		offset += 40;
		digest[40] = 0;
		return(40);
	}

	if (mode == 3)
	{
		memcpy(pinBlock, hsmCmdBuf+offset, 16);
		pinBlock[16] = 0;
	}

	return(16);
} 

/*
   �������ܣ�
   8Dָ���IBM��ʽ����һ��PIN��Offset
   ���������
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinLength��LMK���ܵ�PIN���ĳ���
   pinBlockByLMK����LMK���ܵ�PIN����
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinOffset��PIN Offset������룬�Ҳ�'F'

 */
int UnionHsmCmd8D(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	errCode[32];

	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
			(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd8D:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"8D",2);
	hsmCmdLen = 2;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8D:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8D:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8D:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// ������PIN��Offset
		memcpy(pinOffset,hsmCmdBuf+4,12);
		return(12);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}

/*
   ��������:
   KKָ��,SM2��Կ���ܵ�pinת����zpk��DES��SM4������
   ���������
   keyIndex	SM2��Կ����
   vkLen		SM2�����Կ����
   vkValue		SM2�����Կ
   lenOfPinByPK	SM2��Կ���ܵ����ĳ���
   pinByPK		����
   algorithmID	�㷨��ʶ 1��SM4��Կ, 2: 3DES��Կ
   keyType		��Կ����,1��ZPK, 2��TPK 
   pinFormat	PIN��ʽ
   lenOfAccNo	�˺ų���	
   accNo		�˺�
   keyLen		��Կ����
   keyValue	ZPK��TPK��Կ
   specialAlg	��ѡ��
   P-���밲ȫ�ؼ���ȫ�㷨
   (��Կ�㷨ΪSM2ʱ��ѡ��RSAʱ������)
   sizeofPinByKey	pinByKey����󳤶�
   ���������
   pinByKey	DES��SM4�¼��ܵ�PIN

 */

int unionHsmCmdKK(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *pinFormat, int lenOfAccNo,char *accNo,char *keyValue, char *specialAlg, char *pinByKey,int sizeofPinByKey)
{
	int 	ret;
	char    hsmCmdBuf[1024];
	char	tmpBuf[16];
	int     hsmCmdLen = 0;
	int	len = 0;

	if((NULL == keyIndex) || (NULL == pinByPK) || (NULL == pinFormat) || (NULL == accNo) || (NULL == keyValue))
	{
		UnionUserErrLog("in UnionHsmCmdKK:: parameters err\n");
		return(errCodeParameter);
	}

	//������
	memcpy(hsmCmdBuf, "KK", 2);
	hsmCmdLen += 2;

	//SM2��Կ����
	memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
	hsmCmdLen += 2;

	if(!strncmp(keyIndex, "99", 2))
	{
		//SM2�����Կ����
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
	hsmCmdLen += 4;

	//����
	aschex_to_bcdhex(pinByPK, lenOfPinByPK, hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	sprintf(hsmCmdBuf + hsmCmdLen, "%d", algorithmID);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf + hsmCmdLen, "%d", keyType);
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf + hsmCmdLen, pinFormat, 2);
	hsmCmdLen += 2;

	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKK:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;

	if(1 == algorithmID)
		memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
	else
		memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf + hsmCmdLen, keyValue, 32);
	hsmCmdLen += 32;

	//add by lusj 20160118  ��������ר���㷨������ͨ�ð汾
	if((specialAlg != NULL)&&(specialAlg[0] =='P'))
	{
		memcpy(hsmCmdBuf+hsmCmdLen,specialAlg,1);
		hsmCmdLen += 1;
	}
	//add end by lusj 		

	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKK:: UnionDirectHsmCmd\n");
		return(ret);
	}

	memcpy(tmpBuf, hsmCmdBuf + 4, 2);
	tmpBuf[2] = 0;
	len = UnionOxToD(tmpBuf) * 2;
	if (len >= sizeofPinByKey)
	{
		UnionUserErrLog("in UnionHsmCmdKK:: len[%d] >= sizeofPinByKey[%d]\n",len,sizeofPinByKey);
		return(errCodeSmallBuffer);
	}
	memcpy(pinByKey, hsmCmdBuf + 6, len);
	pinByKey[len] = 0;

	return(0);
}

/*
   ��������:
   KLָ��,SM2��Կ���ܵ���������ת����zpk��DES��SM4������
   ���������
   keyIndex        SM2��Կ����
   vkLen           SM2�����Կ����
   vkValue 	SM2�����Կ
   lenOfPinByPK    SM2��Կ���ܵ����ĳ���
   pinByPK  	����
   algorithmID     �㷨��ʶ,1��SM4��Կ, 2: 3DES��Կ
   keyType         ��Կ����, 0��ZMK, 1��ZPK, 2��ZEK 
   keyValue        ��Կֵ
   sizeofPinByKey	pinByKey����󳤶�
   ���������
   pinByKey        DES��SM4�¼��ܵ�PIN

 */
int unionHsmCmdKL(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *keyValue, char *pinByKey,int sizeofPinByKey)
{
	int     ret;
	char    hsmCmdBuf[1024];
	char    tmpBuf[16];
	int     hsmCmdLen = 0;

	if((NULL == keyIndex) || (NULL == pinByPK) || (NULL == keyValue))
	{
		UnionUserErrLog("in UnionHsmCmdKL:: parameters err\n");
		return(errCodeParameter);
	}       

	//������
	memcpy(hsmCmdBuf, "KL", 2);
	hsmCmdLen += 2;

	//SM2��Կ����
	memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
	hsmCmdLen += 2;

	if(!strncmp(keyIndex, "99", 2))
	{
		//SM2�����Կ����
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}

	//���ĳ���
	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
	hsmCmdLen += 4;
	aschex_to_bcdhex(pinByPK,lenOfPinByPK , hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	//�㷨��ʶ
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", algorithmID);
	hsmCmdLen += 1;
	//��Կ����
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", keyType);
	hsmCmdLen += 1;

	if(1 == algorithmID)
		memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
	else
		memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
	hsmCmdLen += 1;

	//��Կ
	memcpy(hsmCmdBuf + hsmCmdLen, keyValue, 32);
	hsmCmdLen += 32;

	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKL:: UnionDirectHsmCmd\n");
		return(ret);
	}

	memcpy(tmpBuf, hsmCmdBuf + 4, 2);
	tmpBuf[2] = 0;
	ret = UnionOxToD(tmpBuf) * 2;
	if (ret >= sizeofPinByKey)
	{
		UnionUserErrLog("in UnionHsmCmdKL:: ret[%d] >= sizeofPinByKey[%d]\n",ret,sizeofPinByKey);
		return(errCodeSmallBuffer);
	}

	memcpy(pinByKey, hsmCmdBuf + 6, ret);
	pinByKey[ret] = 0;

	return(0);
}

int UnionHsmCmdKL(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *keyValue, int saltedOffset, int saltedLen, char *saltedData, char *pinByKey, int sizeofPinByKey, char *hash, int sizeOfHash)
{
        int     ret;
        char    hsmCmdBuf[1024];
        char    tmpBuf[16];
        int     hsmCmdLen = 0;

        if((NULL == keyIndex) || (NULL == pinByPK) )
        {
                UnionUserErrLog("in UnionHsmCmdKL:: parameters err\n");
                return(errCodeParameter);
        }

        //������
        memcpy(hsmCmdBuf, "KL", 2);
        hsmCmdLen += 2;

        //SM2��Կ����
        memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
        hsmCmdLen += 2;

        if(!strncmp(keyIndex, "99", 2))
        {
                //SM2�����Կ����
                sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
                hsmCmdLen += 4;

                aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
                hsmCmdLen += vkLen/2;
        }
        //���ĳ���
        sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
        hsmCmdLen += 4;
        aschex_to_bcdhex(pinByPK,lenOfPinByPK , hsmCmdBuf + hsmCmdLen);
        hsmCmdLen += lenOfPinByPK/2;

        //�㷨��ʶ
        sprintf(hsmCmdBuf + hsmCmdLen, "%d", algorithmID);
        hsmCmdLen += 1;

        if(algorithmID == 1 || algorithmID == 2)
        {
                //��Կ����
                sprintf(hsmCmdBuf + hsmCmdLen, "%d", keyType);
                hsmCmdLen += 1;

                if(1 == algorithmID)
                        memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
                else
                        memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
                hsmCmdLen += 1;

                //��Կ
                memcpy(hsmCmdBuf + hsmCmdLen, keyValue, 32);
                hsmCmdLen += 32;
        }
        else
        {
                // ����ƫ��
                sprintf(hsmCmdBuf + hsmCmdLen, "%04X", saltedOffset);
                hsmCmdLen += 4;
                // ���γ���
                sprintf(hsmCmdBuf + hsmCmdLen, "%04X", saltedLen/2);
                hsmCmdLen += 4;
                // ��������
                memcpy(hsmCmdBuf + hsmCmdLen, saltedData, saltedLen);
                hsmCmdLen += saltedLen;
        }

        hsmCmdBuf[hsmCmdLen] = 0;

        UnionSetBCDPrintTypeForHSMCmd();

        if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdKL:: UnionDirectHsmCmd\n");
                return(ret);
        }

        memcpy(tmpBuf, hsmCmdBuf + 4, 2);
        tmpBuf[2] = 0;
        ret = UnionOxToD(tmpBuf) * 2;
        if (algorithmID == 1 || algorithmID == 2)
        {
                if(ret >= sizeofPinByKey)
                {
                        UnionUserErrLog("in UnionHsmCmdKL:: ret[%d] >= sizeofPinByKey[%d]\n",ret,sizeofPinByKey);
                        return(errCodeSmallBuffer);
                }
                else
                {
                        memcpy(pinByKey, hsmCmdBuf + 6, ret);
                        pinByKey[ret] = 0;
                }
        }
        else
        {
                if(ret >= sizeOfHash)
                {
                        UnionUserErrLog("in UnionHsmCmdKL:: ret[%d] >= sizeOfHash[%d]\n", ret, sizeOfHash);
                        return(errCodeSmallBuffer);
                }
                else
                {
                        memcpy(hash, hsmCmdBuf + 6, ret);
                        hash[ret] = 0;
                }
        }

        return(0);
}

/*
   �������ܣ�
   8Eָ���IBM��ʽ����һ��PIN��Offset
   ���������
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinLength��LMK���ܵ�PIN���ĳ���
   pinBlockByLMK����LMK���ܵ�PIN����
   pvk��LMK���ܵ�PVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinOffset��PIN Offset������룬�Ҳ�'F'

 */
int UnionHsmCmd8E(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	errCode[32];

	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
			(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd8E:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"8E",2);
	hsmCmdLen = 2;

	// LMK���ܵ�PVK
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S");
	memcpy(hsmCmdBuf+hsmCmdLen,pvk,32);
	hsmCmdLen += 32;

	// LMK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8E:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8E:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// ������PIN��Offset
		memcpy(pinOffset,hsmCmdBuf+4,12);
		pinOffset[12] = 0;
		return(12);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}


/*
   ��������:    ����һ�������˫�����ȵ�*CSCK������LMK�ԣ�14-15���ı���4����(RacalCmdRY0)
����:
eparator:          �ָ���          ��ѡ������ʾ����������򣬸��������ʾ,ֵΪ������
keyMethodByZMK       ��Կ����        ��ѡ��,ZMK�¼�����Կ�ķ���
keyMethodByLMK       ��Կ����        ��ѡ�LMK�¼�����Կ�ķ���
checkValueType        ��ԿУ��ֵ����  ��ѡ��,0-kcv������ 1-kcv 6H

���:
CSCKByLMK                *CSCK           LMK�ԣ�14-15���ı���4�¼��ܵ�*CSCK
checkValue       	У��ֵ     
 */
int UnionHsmCmdRY0(char *separator,char *keyMethodByZMK,char *keyMethodByLMK,char *checkValueType,char *CSCKByLMK,char *checkValue)
{
	int	ret = 0;
	int 	hsmCmdLen = 0;
	int 	isOption = 0;
	int 	offset = 0;
	char	hsmCmdBuf[1040] = {0};

	memcpy(hsmCmdBuf,"RY",2);
	hsmCmdLen = 2;

	hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen += 1;

	if(memcmp(separator,";",1) == 0)
	{
		isOption = 1;
	}	

	if(isOption)
	{
		if((keyMethodByZMK != NULL) && (keyMethodByLMK != NULL) && (checkValueType != NULL))
		{
			memcpy(hsmCmdBuf + hsmCmdLen,separator,1);
			hsmCmdLen++ ;

			memcpy(hsmCmdBuf + hsmCmdLen,keyMethodByZMK,1);
			hsmCmdLen++;

			memcpy(hsmCmdBuf + hsmCmdLen,keyMethodByLMK,1);
			hsmCmdLen++;

			memcpy(hsmCmdBuf + hsmCmdLen, checkValueType,1);
			hsmCmdLen++;
		}
		else
		{
			UnionUserErrLog("in UnionHsmCmdRY0:: The parameter separator is error!\n");	
			return(errCodeParameter);
		}
	}

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRY0:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;

	offset = 5;
	if(hsmCmdBuf[offset] == 'X')
	{
		offset++;
		memcpy(CSCKByLMK,hsmCmdBuf+offset,32);
	}
	else
	{
		memcpy(CSCKByLMK,hsmCmdBuf+offset,32);
	}
	offset += 32;
	ret -= offset;
	memcpy(checkValue,hsmCmdBuf+offset,ret);
	return(strlen(CSCKByLMK));
}

/*
   RYָ�����һ��*CSCK
   ���������
   ZMKByLMK,	LMK��(04-05)�¼��ܵ�ZMK
   CSCKByLMK,	LMK��(14-15)�¼��ܵ�CSCK
   ���������
   CSCKByZMK,	�����ṩ��ZMK�¼��ܵ�CSCK
   checkValue,	CSCK�¼���64����0�Ľ��

 */
int UnionHsmCmdRY1(char *ZMKByLMK,char *CSCKByLMK,char *CSCKByZMK,char *checkValue)
{
	int 	ret = 0;
	char	hsmCmdBuf[512] = {0};
	int 	hsmCmdLen = 0;
	int 	offset = 0;

	if((ZMKByLMK == NULL) || (CSCKByLMK == NULL) || (CSCKByZMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdRY1:: wrong parameter!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"RY",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = '1';
	hsmCmdLen++;
	if(strlen(ZMKByLMK) == 32)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"X");
		memcpy(hsmCmdBuf + hsmCmdLen,ZMKByLMK,32);
		hsmCmdLen += 32;
	}
	else if(strlen(ZMKByLMK) == 48)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"Y");
		memcpy(hsmCmdBuf + hsmCmdLen,ZMKByLMK,48);	
		hsmCmdLen += 48;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdRY1:: wrong zmk length!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"X");
	memcpy(hsmCmdBuf + hsmCmdLen,CSCKByLMK,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRY1:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 5;
	if(hsmCmdBuf[offset] == 'X')
	{
		offset++;
		memcpy(CSCKByZMK,hsmCmdBuf + offset,32);
	}
	else
	{
		memcpy(CSCKByZMK,hsmCmdBuf + offset,32);
	}
	offset += 32;
	ret -= offset;
	memcpy(checkValue,hsmCmdBuf + offset,ret);
	return(strlen(CSCKByZMK));
}

/*
   �������ܣ�RYָ��,����һ��*CSCK
   ���������
   ZMKByLMK,	LMK��(04-05)�¼��ܵ�ZMK
   CSCKByZMK,	�����ṩ��ZMK�¼��ܵ�CSCK
   ���������
   CSCKByLMK,	LMK��(14-15)�ı���4�¼��ܵ�CSCK
   checkValue,	CSCK�¼���64����0�Ľ��

 */



int UnionHsmCmdRY2(char *ZMKByLMK,char *CSCKByZMK,char *CSCKByLMK,char *checkValue)
{
	int 	ret = 0;
	int	hsmCmdLen = 0;
	int 	offset = 0;
	char	hsmCmdBuf[512] = {0};

	if((ZMKByLMK == NULL) || (CSCKByZMK == NULL) || (CSCKByLMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdRY2:: wrong parameter!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"RY",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = '2';
	hsmCmdLen++;
	if(strlen(ZMKByLMK) == 32)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"X");
		memcpy(hsmCmdBuf + hsmCmdLen,ZMKByLMK,32);
		hsmCmdLen += 32;
	}
	else if(strlen(ZMKByLMK) == 48)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"Y");
		memcpy(hsmCmdBuf + hsmCmdLen,ZMKByLMK,48);
		hsmCmdLen += 48;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdRY2:: wrong zmk length!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"X");
	memcpy(hsmCmdBuf + hsmCmdLen,CSCKByZMK,32);
	hsmCmdLen += 32;
	hsmCmdBuf[hsmCmdLen] = 0;

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRY2:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 5;
	if(hsmCmdBuf[offset] == 'X')
	{
		offset++;
		memcpy(CSCKByLMK,hsmCmdBuf + offset,32);
	}
	else
	{
		memcpy(CSCKByLMK,hsmCmdBuf + offset,32);
	}
	offset += 32;
	ret -= offset;
	memcpy(checkValue,hsmCmdBuf + offset,ret);
	return(strlen(CSCKByLMK));
}


/*
   �������ܣ� RYָ����㿨��ȫ��
   ���������
   CSCKByLMK,	LMK��(14-15)�ı����¼��ܵ�CSCK
   accNo,		�˺ŵ�ȫ�����֣��������19λ������0��19λ
   activeDate,	"YYMM"��ʽ����Ч��
   ���������
   CSCOf5Digit,	5λ��CSCֵ
   CSCOf4Digit,	4λ��CSCֵ
   CSCOf3Digit,	3λ��CSCֵ
 */



int UnionHsmCmdRY3(char *flag,char *CSCKByLMK,char *accNo,char *activeDate,char *svrCode,char  *CSCOf5Digit,char *CSCOf4Digit,char *CSCOf3Digit)
{
	int 	ret = 0;
	int	hsmCmdLen = 0;
	int	len = 0;
	char	hsmCmdBuf[1040] = {0};

	if((flag == NULL) || (CSCKByLMK == NULL) || (accNo == NULL) || (activeDate == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdRY3:: wrong parameter!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"RY",2);
	hsmCmdLen = 2;

	hsmCmdBuf[hsmCmdLen] = '3';
	hsmCmdLen++;

	if((memcmp(flag,"0",1) != 0) && (memcmp(flag,"2",1) != 0) && (memcmp(flag,"3",1) != 0))
	{
		UnionUserErrLog("in UnionHsmCmdRY3:: the parameter flag [%s] is error!\n", flag);
		return (errCodeParameter);
	}

	memcpy(hsmCmdBuf + hsmCmdLen,flag,1);
	hsmCmdLen++;

	len = strlen(CSCKByLMK);
	memcpy(hsmCmdBuf + hsmCmdLen,CSCKByLMK,len);
	hsmCmdLen += len;

	len = strlen(accNo);
	memcpy(hsmCmdBuf + hsmCmdLen,accNo,len);
	hsmCmdLen += len;

	len = strlen(activeDate);
	memcpy(hsmCmdBuf + hsmCmdLen,activeDate,len);
	hsmCmdLen += len;

	if((memcmp(flag,"2",1) == 0) || (memcmp(flag,"3",1) == 0))
	{
		len = strlen(svrCode);
		memcpy(hsmCmdBuf + hsmCmdLen,svrCode,len);
		hsmCmdLen += len;
	}

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRY3:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;

	if((memcmp(flag,"2",1) == 0) || (memcmp(flag,"0",1) == 0))
	{
		memcpy(CSCOf5Digit,hsmCmdBuf + 5,5);
		CSCOf5Digit[5] = 0;
		memcpy(CSCOf4Digit,hsmCmdBuf + 10,4);
		CSCOf4Digit[4] = 0;
		memcpy(CSCOf3Digit,hsmCmdBuf + 14,3);
		CSCOf3Digit[3] = 0;
	}
	else
	{
		memcpy(CSCOf3Digit,hsmCmdBuf + 5,3);
		CSCOf3Digit[3] = 0;
	}

	return(0);	
}

/*
   �������ܣ� RYָ�У�鿨��ȫ��
   ���������
   CSCKByLMK,	LMK��(14-15)�ı���4�¼��ܵ�CSCK
   accNo,		�˺ŵ�ȫ�����֣��������19λ������0��19λ
   activeDate,	"YYMM"��ʽ����Ч��
   CSCOf5Digit,	5λ��"CSC"ֵ�������������ʾ"FFFFF"
   CSCOf4Digit,	4λ��"CSC"ֵ�������������ʾ"FFFF"
   CSCOf3Digit,	3λ��"CSC"ֵ�������������ʾ"FFF"
   ���������
   checkResultOf5CSC,	5λ"CSC"��У����
   checkResultOf4CSC,	4λ"CSC"��У����
   checkResultOf3CSC,	3λ"CSC"��У����

 */


int UnionHsmCmdRY4(char *flag,char *CSCKByLMK,char *accNo,char *activeDate,char *svrCode,char *CSCOf5Digit,char *CSCOf4Digit,char *CSCOf3Digit,char *checkResultOf5CSC,char *checkResultOf4CSC,char *checkResultOf3CSC)
{
	int	ret = 0;
	int	hsmCmdLen = 0;
	int	len = 0;
	char	hsmCmdBuf[1024];	
	char	errCode[8];

	if((flag == NULL) || (CSCKByLMK == NULL) || (accNo == NULL) || (activeDate == NULL) || (CSCOf5Digit == NULL) || (CSCOf4Digit == NULL) || (CSCOf3Digit == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdRY4:: wrong parameter!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"RY",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = '4';
	hsmCmdLen++;

	if((memcmp(flag,"0",1) != 0) && (memcmp(flag,"2",1) != 0) && (memcmp(flag,"3",1) != 0))
	{
		UnionUserErrLog("in UnionHsmCmdRY4:: The parameter flag[%s] is error!\n",flag);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf + hsmCmdLen,flag,1);
	hsmCmdLen++;

	len = strlen(CSCKByLMK);	
	memcpy(hsmCmdBuf + hsmCmdLen,CSCKByLMK,len);
	hsmCmdLen += len;

	len = strlen(accNo);
	memcpy(hsmCmdBuf + hsmCmdLen,accNo,len);
	hsmCmdLen += len;

	len = strlen(activeDate);
	memcpy(hsmCmdBuf + hsmCmdLen,activeDate,len);
	hsmCmdLen += len;

	if((memcmp(flag,"2",1) == 0) || (memcmp(flag,"3",1) == 0))
	{
		len = strlen(svrCode);
		memcpy(hsmCmdBuf +hsmCmdLen,svrCode,len);	
		hsmCmdLen += len;
	}

	if((memcmp(flag,"2",1) == 0) || (memcmp(flag,"0",1) == 0))	
	{
		len = strlen(CSCOf5Digit);
		memcpy(hsmCmdBuf + hsmCmdLen,CSCOf5Digit,len);
		hsmCmdLen += len;

		len = strlen(CSCOf4Digit);	
		memcpy(hsmCmdBuf + hsmCmdLen,CSCOf4Digit,len);
		hsmCmdLen += len;
	}
	len = strlen(CSCOf3Digit);
	memcpy(hsmCmdBuf + hsmCmdLen,CSCOf3Digit,len);
	hsmCmdLen += len;	

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRY4:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();
	hsmCmdBuf[ret] = 0;

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"01",2) != 0) && (memcmp(errCode,"00",2) != 0))
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));

	if((memcmp(flag,"2",1) == 0) || (memcmp(flag,"0",1) == 0))
	{
		memcpy(checkResultOf5CSC,hsmCmdBuf + 5,1);
		memcpy(checkResultOf4CSC,hsmCmdBuf + 6,1);
		memcpy(checkResultOf3CSC,hsmCmdBuf + 7,1);
		return(4);
	}
	else
	{
		memcpy(checkResultOf3CSC,hsmCmdBuf + 5,1);
		return(2);	
	}	
}



/*
   ��������: B3ָ��˺�������(ƽ������ר��)
   �������:
   zpk,	��LMK�Լ����µ�zpk
   pinBlock,	Դpin��
   accNo,	�˺�
   pvk,	Ŀ��zpk
   �������:
   pvv,	pvk���ܵ�pin����
 */

int UnionHsmCmdB3(char *zpk,char *pinBlock,char *accNo,char *pvk,char *pvv)
{
	int 	ret = 0;
	int 	hsmCmdLen = 0;
	char 	hsmCmdBuf[512] = {0};

	if((zpk == NULL) || (pinBlock == NULL) || (accNo == NULL) || (pvk == NULL) || (pvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdB3:: wrong parameter!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"B3",2);
	hsmCmdLen = 2;

	if(strlen(zpk) == 16)
	{
		memcpy(hsmCmdBuf + hsmCmdLen,zpk,16);
		hsmCmdLen += 16;
	}
	else if(strlen(zpk) == 32)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"X");
		memcpy(hsmCmdBuf + hsmCmdLen,zpk,32);
		hsmCmdLen += 32;
	}
	else if(strlen(zpk) == 48)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"Y");
		memcpy(hsmCmdBuf + hsmCmdLen,zpk,48);
		hsmCmdLen += 48;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdB3:: wrong zpk key length!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf + hsmCmdLen,pinBlock,16);
	hsmCmdLen += 16;

	if(strlen(accNo) == 16)
	{
		memcpy(hsmCmdBuf + hsmCmdLen,accNo,16);
		hsmCmdLen += 16;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdB3:: wrong accNo length!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf + hsmCmdLen,pvk,16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB3:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(pvv,hsmCmdBuf + 4,ret - 4);
	pvv[ret - 4] = 0;
	return(ret - 4);
}

/*
   ��������: B1ָ�����������
   �������:
   zpk,    ��LMK�Լ����µ�zpk
   pinBlock,       Դpin��
   pvk,    Ŀ��zpk
   �������:
   pvv,    pvk���ܵ�pin����
 */


int UnionHsmCmdB1(char *zpk,char *pinBlock,char *pvk,char *pvv)
{
	int     ret = 0;
	int     hsmCmdLen = 0;
	char    hsmCmdBuf[512] = {0};

	if((zpk == NULL) || (pinBlock == NULL) || (pvk == NULL) || (pvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdB1:: wrong parameter!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"B1",2);
	hsmCmdLen = 2;

	if(strlen(zpk) == 16)
	{
		memcpy(hsmCmdBuf + hsmCmdLen,zpk,16);
		hsmCmdLen += 16;
	}
	else if(strlen(zpk) == 32)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"X");
		memcpy(hsmCmdBuf + hsmCmdLen,zpk,32);
		hsmCmdLen += 32;
	}
	else if(strlen(zpk) == 48)
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen,"Y");
		memcpy(hsmCmdBuf + hsmCmdLen,zpk,48);
		hsmCmdLen += 48;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdB1:: wrong zpk key length!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf + hsmCmdLen,pinBlock,16);
	hsmCmdLen += 16;

	memcpy(hsmCmdBuf + hsmCmdLen,pvk,16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB1:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(pvv,hsmCmdBuf + 4,ret - 4);
	pvv[ret - 4] = 0;
	return(ret - 4);
}


int UnionHsmCmdB5(char *TranFlag,char *srcZPK,char *desZPK,
		char *srcPinBlock,char *accNo,char *desPinBlock)
{
	int             ret;
	char            hsmCmdBuf[8192];
	int             offset = 0;
	int             degistLen;

	if ((TranFlag == NULL) || (srcZPK == NULL) || (desZPK == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdB5:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"B5",2);
	offset = 2;

	memcpy(hsmCmdBuf+offset,TranFlag,1);
	offset += 1;
	// srcZPK
	if ((ret = UnionPutKeyIntoRacalKeyString(srcZPK,hsmCmdBuf+offset,sizeof(hsmCmdBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB5:: UnionPutKeyIntoRacalKeyString!\n");
		return(ret);
	}
	offset += ret;
	// desZPK
	if ((ret = UnionPutKeyIntoRacalKeyString(desZPK,hsmCmdBuf+offset,sizeof(hsmCmdBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB5:: UnionPutKeyIntoRacalKeyString!\n");
		return(ret);
	}
	offset += ret;

	memcpy(hsmCmdBuf+offset,srcPinBlock,16);

	offset += 16;

	// modify by leipp 20150831
	if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+offset)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB5:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	offset += ret;
	hsmCmdBuf[offset] = 0;

	/*
	   memcpy(hsmCmdBuf+offset,accNo,12);
	   offset += 12;
	 */
	// modify end

	// �������ͨѶ
	if((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	// PIN����
	offset = 4;
	if ((degistLen = ret - offset) <= 0)
	{
		UnionUserErrLog("in UnionHsmCmdB5:: pinLen = [%02d] error!\n",degistLen);
		return(errCodeParameter);
	}
	memcpy(desPinBlock,hsmCmdBuf+offset,degistLen);
	return(degistLen);

}

//added by zhouxw 20150505 
/*
   ��������: 9Bָ��	��ZPK���ܵ�PIN����תΪ�������㷨����
   �������:
   algorithmID			��Կ�㷨��ʶ	0�1�7:3DES
   1�1�7:SM4
   mode				����ģʽ  0�����Ĵ����㷨
   1�����Ŀ��㷨
   2�����������㷨
   accNo1				�������  �˺���ȥ��У��λ������7λ
   zpk    				��LMK�Լ����µ�zpk
   pinBlock			ZPK���ܵ�PIN����
   format				Pin��ʽ
   accNo2				�˺�  �˺���ȥ��У��λ������12λ
   �������:
   Pin    				˽�����㷨���ܵ�PIN����
 */
int UnionHsmCmd9B(int algorithmID, char *mode, char *accNo1, char *zpk, char *pinBlock, char *format, char *accNo2, char *Pin)
{
	int     ret;
	char    hsmCmdBuf[8096+1];
	int     hsmCmdLen = 0;

	if( (NULL == mode) || (NULL == zpk) || (NULL == pinBlock) || (NULL == accNo2) || (NULL == Pin))
	{
		UnionUserErrLog("in UnionHsmCmd9B:: wrong parameter\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "9B", 2);
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf + hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	if( 0 == memcmp(mode, "1", 2) )	
	{
		if(7 == strlen(accNo1))
		{
			memcpy(hsmCmdBuf + hsmCmdLen, accNo1, 7);
			hsmCmdLen += 7;
		}
	}
	switch(algorithmID)
	{
		case	0:
			memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
			break;
		case	1:
			memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
			break;
		default:
			UnionUserErrLog("in UnionHsmCmd9B:: wrong algorithmID\n");
			return(errCodeParameter);	
	}
	hsmCmdLen += 1;
	if(32 != strlen(zpk))
	{
		UnionUserErrLog("in UnionHsmCmd9B:: wrong zpk len\n");
		return(errCodeParameter);
	}
	else
	{
		memcpy(hsmCmdBuf + hsmCmdLen, zpk, 32);
		hsmCmdLen += 32;
	}

	memcpy(hsmCmdBuf + hsmCmdLen, pinBlock, strlen(pinBlock));
	hsmCmdLen += strlen(pinBlock);

	memcpy(hsmCmdBuf + hsmCmdLen, format, 2);
	hsmCmdLen += 2;

	if ((ret = UnionForm12LenAccountNumber(accNo2,strlen(accNo2),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9B:: UnionForm12LenAccountNumber for [%s]\n",accNo2);
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;


	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9B:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(Pin,hsmCmdBuf+4,ret-4);
	Pin[ret-4] = 0;

	return(ret-4);
}

/*
   �������ܣ�9Cָ��	�������������㷨���ܵ�PIN����תΪZPK����
   ���������
   algorithmID			��Կ�㷨��ʶ	0�1�7:3DES
1 :SM4
zpk                             ��LMK�Լ����µ�zpk
pinBlock                        ZPK���ܵ�PIN����
accNo				�˺�
���������
pinByZPK			ZPK���ܵ�PIN����
 */
int UnionHsmCmd9C(int algorithmID, char *zpk, char *pinBlock, char *accNo, char *pinByZPK)
{
	int     ret;
	char    hsmCmdBuf[8096+1];
	int     hsmCmdLen = 0;

	if( (NULL == zpk) || (NULL == pinBlock) || (NULL == accNo) || (NULL == pinByZPK))
	{
		UnionUserErrLog("in UnionHsmCmd9C:; wrong parameter\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "9C", 2);
	hsmCmdLen = 2;

	switch(algorithmID)
	{
		case    0:
			memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
			break;
		case    1:
			memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
			break;
		default:
			UnionUserErrLog("in UnionHsmCmd9C:: wrong algorithmID\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	if(32 != strlen(zpk))
	{
		UnionUserErrLog("in UnionHsmCmd9C:: wrong zpk len\n");
		return(errCodeParameter);
	}
	else
	{
		memcpy(hsmCmdBuf + hsmCmdLen, zpk, 32);
		hsmCmdLen += 32;                                
	}       

	memcpy(hsmCmdBuf + hsmCmdLen, pinBlock, strlen(pinBlock));
	hsmCmdLen += strlen(pinBlock);

	if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9C:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9C:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(pinByZPK, hsmCmdBuf+4, ret-4);
	pinByZPK[ret-4] = 0;

	return(ret-4);
}

/*
   ��������:9Dָ��		����&�����������MAC
   ���������
   mode		ģʽ	1������MAC
   2��У��MAC
   macDataLen	��������MAC�����ݵĳ���
   macData		��������MAC������
   mac1		��У���MACֵ
   �������
   mac2		���ɵ�MAC	
 */
int UnionHsmCmd9D(char *mode, char *macDataLen, char *macData, char *mac1, char *mac2)
{
	int     ret;
	char    hsmCmdBuf[8096+1];
	int     hsmCmdLen = 0;

	if( NULL == mode )
	{
		UnionUserErrLog("in UnionHsmCmd9D:; wrong parameter\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "9D", 2);
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf + hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	if( (4 != strlen(macDataLen)) || (atoi(macDataLen) != strlen(macData)) )
	{
		UnionUserErrLog("in UnionHsmCmd9D:: macDataLen[%s]/macData[%s]\n", macDataLen, macData);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf + hsmCmdLen, macDataLen, 4);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf + hsmCmdLen, macData, atoi(macDataLen));
	hsmCmdLen += atoi(macDataLen);

	if(0 == memcmp(mode, "2", 2))
	{
		if(8 != strlen(mac1))
		{
			UnionUserErrLog("in UnionHsmCmd9D:: mac1[%s]\n", mac1);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf + hsmCmdLen, mac1, 8);
		hsmCmdLen += 8;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9D:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if(0 == memcmp(mode, "1", 2))
	{
		memcpy(mac2, hsmCmdBuf + 4, ret - 4);
		//bcdhex_to_aschex(hsmCmdBuf + 4, ret - 4, mac2);
		mac2[ret-4] = 0;
		return(ret - 4);
	}
	else
		return(0);
}

//added by zhouxw 20150505 end

/*
   ����: ����չ���ZPK���ܵ���������һ����Կ����(��Ϊ��չ����ʽ)
   ָ��: KM
   �������:
pinFlag:	1A		Y:ԴPIN��Ϊ��չ��ʽ��
N:δ��չԴPIN��
������Ĭ��ΪY
oriAlgorith:	1N		ԴPIN���㷨��1-DES/3DES, 2-SM4
desAlgorith:	1N		Ŀ��PIN���㷨��1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N		Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
oriKeyIndex/oriZpkValue:1A+3H/16/32/48H	Դzpk�����Ż�����
desKeyLenFlag: 	1N		Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
desKeyIndex/desZpkValue:1A+3H/16/32/48H	Ŀ��zpk�����Ż�����
oriFormat:	2N		ԴPIN���ʽ,��01��-��06�������PIN���ʽ
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
desFormat:	2N		Ŀ��PIN���ʽ,��01��-��06�������PIN��
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16��
pinBlock:	32H/48H/64H	��Կ���ܵ�PIN
oriAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
desAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���

sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	32H/48H/64H		ת���ܺ��PIN����
 */
int UnionHsmCmdKM(char pinFlag,int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"KM",2);
	hsmCmdLen = 2;

	if (pinFlag == 'N' || pinFlag == 'n' || pinFlag == 'y' || pinFlag == 'Y')
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%c",toupper(pinFlag));
		hsmCmdLen += 1;
	}

	// ԴPIN���㷨
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdKM:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Ŀ��PIN���㷨
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdKM:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Դ��Կ���ȱ�ʶ
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// Դ��Կ���Ļ�������
	if (oriKeyIndex != NULL && strlen(oriKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriKeyIndex);
		hsmCmdLen += 4;
	}
	else if (oriZpkValue != NULL && strlen(oriZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdKM:: oriKeyIndex and oriZpkValue is null!\n");
		return(errCodeParameter);
	}

	// Ŀ����Կ����
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}

	// Ŀ��zpk��Կ
	if (desKeyIndex != NULL && strlen(desKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desKeyIndex);
		hsmCmdLen += 4;
	}
	else if (desZpkValue != NULL && strlen(desZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",desZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdKM:: desKeyIndex and desZpkValue is null!\n");
		return(errCodeParameter);
	}

	// ԴPIN��ĸ�ʽ
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// Ŀ��PIN��ĸ�ʽ
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// ԴPIN��
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// Դ�˺�
	if (strcmp(oriFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdKM:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(oriFormat,"04") == 0)
	{
		if (strlen(oriAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdKM:: oriAccNo[%s] len[%d] != 18\n",oriAccNo,(int)strlen(oriAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,oriAccNo,18);
		hsmCmdLen += 18;
	}

	// Ŀ���˺�
	if (strcmp(desFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdKM:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(desFormat,"04") == 0)
	{
		if (strlen(desAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdKM:: desAccNo[%s] len[%d] != 18\n",desAccNo,(int)strlen(desAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,desAccNo,18);
		hsmCmdLen += 18;
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKM:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 4 >= sizeofPinByZpk)
	{
		UnionUserErrLog("in UnionHsmCmdKM:: ret[%d] >= sizeofPinByZpk[%d]!\n",ret-4,sizeofPinByZpk);
		return(errCodeParameter);
	}
	memcpy(pinByZpk,hsmCmdBuf+4,ret-4);
	pinByZpk[ret-4] = 0;

	return(ret-4);
}

int UnionHsmCmdSD (int srcAlgorithmID,int pkIndex,int lenOfVK,char *vkValue,char *fillMode,char *dataFlag,int lenOfData,char *data,int lenOfID, 
		char *ID,int dstAlgorithmID,int lenofhashFlag,char *hashFlag,char *EDK,char *pinBlockByPK,char *lenOfPIN,char *pinBlockByEDK)	
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	char	errCode[8];

	UnionSetBCDPrintTypeForHSMCmd();

	if(ID == NULL || EDK == NULL || pinBlockByPK == NULL )
	{
		UnionUserErrLog("in UnionHsmCmdSD:: ID or EDK or pinBlockByPK parameter error!\n");
		return(errCodeParameter);
	}

	//ƴװָ��
	memcpy(hsmCmdBuf,"SD",2);
	hsmCmdLen = 2;
	
	if (srcAlgorithmID == 1)	// �����㷨
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
	}

	if (lenofhashFlag == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,hashFlag,1);
		hsmCmdLen += 1;
	}

	//˽Կ����
	if (pkIndex == 99)
	{
		if (lenOfVK <= 0 || vkValue == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdSD:: lenOfVK[%d] <= 0 or vkValue == null parameter error!\n",lenOfVK);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pkIndex);
		hsmCmdLen += 2;
		
		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue,lenOfVK,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += lenOfVK/2;
	}
	else
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pkIndex);
		hsmCmdLen += 2;
	}

	// �����㷨���д���
	if (srcAlgorithmID == 0)
	{
		//�ù�Կ����ʱ�����õ���䷽ʽ
		if (fillMode == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdSD:: fillMode parameter error!\n");
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,fillMode,1);
		hsmCmdLen++;
	}

	if(strcmp(dataFlag,"T")==0)
	{
		//��������ʶ
		memcpy(hsmCmdBuf+hsmCmdLen,dataFlag,1);
		hsmCmdLen++;
		
		//modefy by lusj 20160316 ��ݸ���ж���ָ����ݼ�����ͬ��ҪҪ�����ݲ���ѹ������ɰ汾һ��
		//������ݳ���
//		sprintf(hsmCmdBuf+hsmCmdLen,"%03d",lenOfData/2);
//		hsmCmdLen += 3;

		//�������
//		aschex_to_bcdhex(data,lenOfData,hsmCmdBuf+hsmCmdLen);
//		hsmCmdLen += lenOfData/2;

		//������ݳ���
		sprintf(hsmCmdBuf+hsmCmdLen,"%03d",lenOfData);
		hsmCmdLen += 3;
		
		//�������
		memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
		hsmCmdLen+=lenOfData;	
		//end by lusj
	}
	
	//modefy by lusj 20160310 ������IDѹ��
	//��¼ID����
//	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfID/2);
//	hsmCmdLen += 2;

	//��¼ID
//	aschex_to_bcdhex(ID,lenOfID,hsmCmdBuf+hsmCmdLen);
//	hsmCmdLen += lenOfID/2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfID);
        hsmCmdLen += 2;

        //��¼ID
        memcpy(hsmCmdBuf+hsmCmdLen,ID,lenOfID);
        hsmCmdLen += lenOfID;

	//modefy end by lusj 
	
	if (dstAlgorithmID == 0)
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(EDK,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdSD:: UnionPutKeyIntoRacalKeyString [%s]!\n",EDK);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else
	{
		//EDK
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen,EDK,32);
		hsmCmdLen += 32;
	}

	//PIN����
	aschex_to_bcdhex(pinBlockByPK,strlen(pinBlockByPK),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += strlen(pinBlockByPK)/2;

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���ò������Ӧ��
	UnionSetIsNotCheckHsmResCode();

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSD:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// ���������PIN�����ģ���LMK����
		memcpy(lenOfPIN,hsmCmdBuf+4,2);
		lenOfPIN[2] = 0;
		if (dstAlgorithmID == 0)
		{
			memcpy(pinBlockByEDK,hsmCmdBuf+4+2,32);
			pinBlockByEDK[32] = 0;
			return(32);
		}
		else
		{
			memcpy(pinBlockByEDK,hsmCmdBuf+4+2,64);
			pinBlockByEDK[64] = 0;
			return(64);
		}
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}

/*
   ����:ת��PIN��
   �������:
   rootKey��Ӧ������Կ
   keyType����Կ���ͣ�001-ZPK��008-ZAK��
   discreteNum����ɢ����
   discreteData1����ɢ����1
   discreteData2����ɢ����2
   discreteData3����ɢ����3
   resPinBlock��Դpinblock
   AccNo���ʺ�
   pinFmt��pin��ʽ
   zpk��Ŀ��zpk
   �������:
   disPinBlock��Ŀ��pinblock��
   errCode������������롣
   ����ֵ:
   <0: ����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0: ����ִ�гɹ�
 */
int UnionHsmCmdY5 (char *rootKey, char *keyType,int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char *resPinBlock, char *AccNo, char *pinFmt, char * zpk, char *disPinBlock)
{
	char hsmCmdBuf[2048];
	int hsmCmdLen = 0;
	int ret;
	char    errCode[8];


	if ((rootKey == NULL) || (keyType == NULL) || (discreteNum <= 0) || (discreteData1 == NULL) \
			|| (resPinBlock == NULL) || (AccNo == NULL) || (pinFmt == NULL) || (zpk == NULL) || (disPinBlock == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdY5:: parameter error!\n");
		return (errCodeParameter);
	}
	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf, "Y5", 2);
	hsmCmdLen = 2;
	if ((ret = UnionPutKeyIntoRacalKeyString(rootKey, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*????*/
	{
		UnionUserErrLog("in UnionHsmCmdY5:: rootKey UnionPutKeyIntoRacalKeyString [%s]!\n", rootKey);
		return(ret);
	}
	//д����Կ����
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", discreteNum); /*��ɢ����*/
	hsmCmdLen += 1;
	/*��ɢ����*/
	memcpy(hsmCmdBuf + hsmCmdLen, discreteData1, 16);
	hsmCmdLen += 16;
	if (discreteNum >= 2)
	{
		if (discreteData2 == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdY5:: parameter error!\n");
			return (errCodeParameter);
		}
		memcpy(hsmCmdBuf + hsmCmdLen, discreteData2, 16);
		hsmCmdLen += 16;
		if (discreteNum == 3)
		{
			if (discreteData3 == NULL)
			{
				UnionUserErrLog("in UnionHsmCmdY5:: parameter error!\n");
				return (errCodeParameter);
			}
			memcpy(hsmCmdBuf + hsmCmdLen, discreteData3, 16);
			hsmCmdLen += 16;
		}
	}
	memcpy(hsmCmdBuf + hsmCmdLen, resPinBlock, 16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf + hsmCmdLen, AccNo, 12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf + hsmCmdLen, pinFmt, 2);
	hsmCmdLen += 2;
	if ((ret = UnionPutKeyIntoRacalKeyString(zpk, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) 
	{
		UnionUserErrLog("in UnionHsmCmdY5:: rootKey UnionPutKeyIntoRacalKeyString [%s]!\n", zpk);
		return(ret);
	}

	hsmCmdLen += ret;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY5:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}


	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// ������pinbock
		memcpy(disPinBlock, hsmCmdBuf + 4, 16);
		return(16);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));

}

/*
   ����:Mac���㣬����MACֵ��
   �������:
   rootKey��Ӧ������Կ
   keyType����Կ���ͣ�001-ZPK��008-ZAK��
   discreteNum����ɢ����
   discreteData1����ɢ����1
   discreteData2����ɢ����2
   discreteData3����ɢ����3
   msgBlockNum����Ϣ��� (0:��һ��,1:��һ��,2:�м��,3:����)
   msgType����Ϣ����(0����Ϣ����Ϊ������,1����Ϣ����Ϊ��չʮ������)
   iv��IV,Ĭ�ϳ�ʼֵ16H��0
   msgLength����Ϣ����
msg: ��Ϣ��
�������:
Mac��MAC
errCode������������롣
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
 */
int UnionHsmCmdY7( char *rootKey, char * keyType,int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char msgBlockNum , char msgType, int  msgLength, char *msg, char *mac)
{
	char 	hsmCmdBuf[2048];
	int 	hsmCmdLen = 0;
	int 	ret;
	char 	iv_mac[16];

	int		len = 0;
	int		offset = 0;
	char	flag;
	int 	sizeOfBuf;
	char    errCode[8];



	if ((rootKey == NULL) || (keyType == NULL) || (discreteNum <= 0) || (discreteData1 == NULL) \
			|| (mac == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdY7:: parameter error!\n");
		return (errCodeParameter);
	}

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf, "Y7", 2);
	hsmCmdLen = 2;

	//����Կֵд����Կ����
	sizeOfBuf = sizeof(hsmCmdBuf) - 1 - hsmCmdLen;
	switch (len = strlen(rootKey))
	{
		case	16:
			break;
		case	32:
			offset = 1;
			flag = 'X';
			break;
		case	48:
			offset = 1;
			flag = 'Y';
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdY7:: invalid rootKey [%s]\n",rootKey);
			return(errCodeParameter);
	}

	if (offset + len > sizeOfBuf)
	{
		UnionUserErrLog("in UnionHsmCmdY7:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}

	if (offset == 1)
		memcpy(hsmCmdBuf + hsmCmdLen,&flag,offset);

	memcpy(hsmCmdBuf+hsmCmdLen+offset,rootKey,len);
	ret = len+offset;

	//д����Կ����
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", discreteNum); /*��ɢ����*/
	hsmCmdLen += 1;

	/*��ɢ����*/
	memcpy(hsmCmdBuf + hsmCmdLen, discreteData1, 16);
	hsmCmdLen += 16;
	if (discreteNum >= 2)
	{
		if (discreteData2 == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdY7:: parameter error!\n");
			return (errCodeParameter);
		}
		memcpy(hsmCmdBuf + hsmCmdLen, discreteData2, 16);
		hsmCmdLen += 16;
		if (discreteNum == 3)
		{
			if (discreteData3 == NULL)
			{
				UnionUserErrLog("in UnionHsmCmdY7:: parameter error!\n");
				return (errCodeParameter);
			}
			memcpy(hsmCmdBuf + hsmCmdLen, discreteData3, 16);
			hsmCmdLen += 16;
		}
	}
	memcpy(hsmCmdBuf + hsmCmdLen, &msgBlockNum, 1);
	++hsmCmdLen;
	memcpy(hsmCmdBuf + hsmCmdLen, &msgType, 1);
	++hsmCmdLen;
	memset(iv_mac, '0', sizeof(iv_mac));
	memcpy(hsmCmdBuf + hsmCmdLen, iv_mac, 16);  /*iv_mac*/
	hsmCmdLen += 16;

	/*����msgType����msgLength*/
	switch (msgType)
	{
		case	'0':
			sprintf(hsmCmdBuf + hsmCmdLen, "%04X", msgLength);
			break;
		case	'1':
			sprintf(hsmCmdBuf + hsmCmdLen, "%04X", msgLength/2);
			break;
		default:
			UnionUserErrLog("in UnionHsmCmdY7:: wrong msgtype flag [%c]!\n", msgType);
			return(errCodeParameter);
	}
	hsmCmdLen += 4;
	if (hsmCmdLen + msgLength > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdY7:: msg length [%d] too long!\n", msgLength);
		return(-1);
	}

	/*������Ϣ��*/
	memcpy(hsmCmdBuf + hsmCmdLen, msg, msgLength);
	hsmCmdLen += msgLength;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY7:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}


	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// ������mac
		memcpy(mac, hsmCmdBuf + 4, 16);
		return(16);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));

}

// add by leipp 20150731
// ����SM2��Կ
/* �������
   index		��Կ����
   lenOfPK	��Կ�����ֽ���
   PK		��Կ����
   �������
   ��
 */
int UnionHsmCmdE8(int index, int lenOfPK, char *PK)
{
	int			ret;
	char			hsmCmdBuf[8192];
	int			hsmCmdLen = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"E8",2);
	hsmCmdLen = 2;

	sprintf(hsmCmdBuf+hsmCmdLen ,"S%02d",index);
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen ,"%04d",lenOfPK/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(PK,lenOfPK/2,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfPK/2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdE8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

//��ZPK��DES��SM4�����ܵ�pinת��SM2��Կ����
/* �������
   algorithmID	1A	�㷨��ʶ,1:SM4��Կ, 2:3DES��Կ
   SM2Index	2N	SM2����,"00"---"20"
   lenOfSM2     4N      SM2˽Կ����
   SM2Value     nB      SM2˽Կֵ
   keyType	1A	��Կ����,1:ZPK, 2:TPK
   keyIndex	1A+3H	��Կ���� 
   keyValue	1A+32H	��Կֵ,3DES 1AΪX��SM4 1A ΪS
   format	2H	�μ�pin��ʽ˵��
   lenOfAccNo	nN	�˺ų��� 
   accNo	nN	�˺�
   pinBlock	16/32H	����㷨��ʶ��3des��16H
   ����㷨��ʶ��SM4��32H
   sizeofPinByPK 4N	����ֵ���ֵ

   �������
   pinByPK	nB	��Կ���ܵ�����	
   ��
 */
int UnionHsmCmdKN(int algorithmID, int SM2Index,int lenOfSM2,char *SM2Value,int keyType,char *keyIndex,char *keyValue,char *format,int lenOfAccNo,char *accNo,char *pinBlock,char *pinByPK,int sizeofPinByPK)
{
	int			ret;
	char			hsmCmdBuf[8192];
	int			hsmCmdLen = 0;
	char			keyLenBuf[8];

	if ((accNo == NULL) || (format == NULL) || (lenOfAccNo <= 0) || (pinBlock == NULL) || (pinByPK == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdKN:: accNo or format or lenOfAccNo <= 0 or pinByPK or pinByPK parameter error!\n");
		return (errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"KN",2);
	hsmCmdLen = 2;

	// �㷨��ʶ
	sprintf(hsmCmdBuf+hsmCmdLen ,"%d",algorithmID);
	hsmCmdLen += 1;

	// ��Կ����
	sprintf(hsmCmdBuf+hsmCmdLen ,"%d",keyType);
	hsmCmdLen += 1;

	// key
	if (algorithmID == 1)
	{
		if ((keyIndex != NULL) && (strlen(keyIndex) > 0))
		{
			sprintf(hsmCmdBuf+hsmCmdLen,"S%s",keyIndex);
			hsmCmdLen += 4;
		}
		else
		{
			sprintf(hsmCmdBuf+hsmCmdLen,"S%s",keyValue);
			hsmCmdLen += 33;
		}
	}
	else
	{
		if ((keyIndex != NULL) && (strlen(keyIndex) > 0))
		{
			sprintf(hsmCmdBuf+hsmCmdLen,"X%s",keyIndex);
			hsmCmdLen += 4;
		}
		else
		{
			sprintf(hsmCmdBuf+hsmCmdLen,"X%s",keyValue);
			hsmCmdLen += 33;
		}
	}

	// pin��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen ,format,2);
	hsmCmdLen += 2;

	// �˺�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKN:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;

	// pinBlock
	if (algorithmID == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen ,pinBlock,32);
		hsmCmdLen += 32;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen ,pinBlock,16);
		hsmCmdLen += 16;
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",SM2Index);
	hsmCmdLen += 2;

	// SM2��Կ����
	if (SM2Index == 99)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfSM2/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(SM2Value,lenOfSM2,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += lenOfSM2/2;
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKN:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(keyLenBuf,hsmCmdBuf+4,4);
	keyLenBuf[4] = 0;

	ret = atoi(keyLenBuf);

	if (sizeofPinByPK < ret * 2)
	{
		UnionUserErrLog("in UnionHsmCmdKN:: sizeofPinByPK[%d] < ret[%d]!\n",sizeofPinByPK,ret*2);
		return(errCodeSmallBuffer);
	}

	bcdhex_to_aschex(hsmCmdBuf+4+4,ret,pinByPK);
	pinByPK[ret*2] = 0;

	return(ret*2);
}
// add by leipp end

//add by lusj 20150813
/*
   ����:ת��PIN��
   �������:
   keyvalue������pin����Կֵ
   keyType����Կ���ͣ�0-ZPK��1-TPK��
   srcPinBlock��SM4��Կ���ܵ�PIN
   AccNo���ʺŹ�AnsiX9.8�㷨ʹ��
HasID:	Hash�㷨0��SM3  	������ʶλ�������ݲ�ʵ��

�������:
PinOfSM3��Ŀ��pinblock��
errCode������������롣

����ֵ:
<0: ����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0: ����ִ�гɹ�
 */
int UnionHsmCmdC6 (char *rootKey, char *keyType, char *srcPinBlock, char *AccNo, char  *HasID, char *disPinBlock)
{
	char hsmCmdBuf[2048];
	int hsmCmdLen = 0;
	int ret;
	char    errCode[8];


	if ((rootKey == NULL) || (keyType == NULL)	|| (srcPinBlock == NULL) || (AccNo == NULL)  || (disPinBlock == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdY5:: parameter error!\n");
		return (errCodeParameter);
	}

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf, "C6", 2);
	hsmCmdLen = 2;


	//д����Կ����
	memcpy(hsmCmdBuf + hsmCmdLen,keyType, 1);
	hsmCmdLen += 1;

	//д����Կֵ	
	sprintf(hsmCmdBuf+hsmCmdLen,"S%s",rootKey);
	hsmCmdLen += 33;

	//����PIN	
	memcpy(hsmCmdBuf + hsmCmdLen, srcPinBlock, 32);
	hsmCmdLen += 32;
	memcpy(hsmCmdBuf + hsmCmdLen, AccNo, 12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf + hsmCmdLen, HasID, 1);
	hsmCmdLen += 1;


	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdC6:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}


	// ������Ҫ�����Ӧ��
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"00",2) == 0))
	{
		// ������pinbock
		memcpy(disPinBlock, hsmCmdBuf + 4, 64);
		return(64);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));

}

//add end 20150813


//add by lizh 20150818

int UnionHsmCmdVB(char* modeOfAlgorithm,char* szMethodID, char* szMKSMC, char* szPanNum, char* szAtc, char* szZPK, char* szPinCryptograph , char* szAccount, char* szLinkOffPinMod, char* szEncryptedData)
{
	int             ret;
	char            hsmCmdBuf[2048+1];
	int             hsmCmdLen = 0;
	char            szTest[1024+1];

	if((szMKSMC == NULL) || (szZPK == NULL) ||(szPinCryptograph ==NULL) || (szAccount == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdVB:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"VB",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,modeOfAlgorithm,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,szMethodID,1);
	hsmCmdLen += 1;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(szMKSMC), szMKSMC, hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdVB:: UnionGenerateX917RacalKeyString for [%s]\n",szMKSMC);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,szPanNum,8);
	hsmCmdLen += 8;

	memcpy(hsmCmdBuf+hsmCmdLen,szAtc,2);
	hsmCmdLen += 2;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(szZPK), szZPK, hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdVB:: UnionGenerateX917RacalKeyString for [%s]\n",szZPK);
		return(ret);
	}
	hsmCmdLen += ret;

	if(strcmp(szMethodID, "9") == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,szPinCryptograph,32);
		hsmCmdLen += 32;
	}
	else if((strcmp(szMethodID, "2") == 0) || (strcmp(szMethodID, "3") == 0)) //add in 2012.3.26
	{
		memcpy(hsmCmdBuf+hsmCmdLen,szPinCryptograph,16);
		hsmCmdLen += 16;
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdVB:: wrong parameters!\n");
		return(errCodeParameter);
	}

	if ((ret = UnionForm12LenAccountNumber(szAccount,strlen(szAccount),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdVB:: UnionForm12LenAccountNumber for [%s]\n",szAccount);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,szLinkOffPinMod,1);
	hsmCmdLen += 1;

	UnionLog("in UnionHsmCmdVB:: hsmCmdBuf=[%s]\n", hsmCmdBuf);

	memset(szTest, 0, sizeof(szTest));
	bcdhex_to_aschex(hsmCmdBuf, hsmCmdLen, szTest);
	UnionLog("in UnionHsmCmdVB:: szTest=[%s]\n", szTest);

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdVB:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret -= 4;

	memcpy(szEncryptedData, hsmCmdBuf+4, ret);
	UnionLog("in UnionHsmCmdVB:: szEncryptedData = [%s]!\n",szEncryptedData);
	return(ret);
}

// add end by lizh  

/*
   ����: �����ַ�PIN��SM4���ܵ�ANSI9.8��ʽ��pinblockת��ΪSM4�㷨������ʽ��PIN
   ָ��: LD
   �������:
oriZpkValue: 1A+32H	Դzpk����
oriZpkCheckValue:	16H	�����Կ��Ҫ������ԿУ��
desZpkValue: 1A+32H	Ŀ��zpk����
dstZpkCheckValue:	16H	�����Կ��Ҫ������ԿУ��
pinBlock:	32H	AnsiX9.8��ʽ��PIN����
lenOfOriAccNo:	N	Դ�˺ų���
oriAccNo:	12N	�û���Ч���ʺ�
lenOfDstAccNo:	N	Ŀ���˺ų���
desAccNo:	12N	�û���Ч���ʺ�
sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinLen		N		pin���ĳ���
pinByZpk:	64H		ת���ܺ��PIN����
 */
int UnionHsmCmdLD(char *oriZpkValue,char *oriZpkCheckValue,char *dstZpkValue,char *dstZpkCheckValue,char *pinBlock,int lenOfOriAccNo,char *oriAccNo,int lenOfDstAccNo,char *dstAccNo,int *pinLen,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	char	pinLenBuf[16];

	memcpy(hsmCmdBuf,"LD",2);
	hsmCmdLen = 2;

	if ((oriZpkValue == NULL) || (strlen(oriZpkValue) == 0) || (oriZpkCheckValue == NULL) || (strlen(oriZpkCheckValue) == 0) || 	\
			(dstZpkValue == NULL) ||(strlen(dstZpkValue) == 0) || (dstZpkCheckValue == NULL) || (strlen(dstZpkCheckValue) == 0) || 	\
			(pinBlock == NULL) || (strlen(pinBlock) == 0))
	{
		UnionUserErrLog("in UnionHsmCmdLD:: parameter is null\n");
		return(errCodeParameter);
	}

	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S%s",oriZpkValue);
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriZpkCheckValue);

	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S%s",dstZpkValue);
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",dstZpkCheckValue);

	// ԴPIN��
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);

	// Դ�˺�
	if ((ret = UnionForm12LenAccountNumber(oriAccNo,lenOfOriAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdLD:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionForm12LenAccountNumber(dstAccNo,lenOfDstAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdLD:: UnionForm12LenAccountNumber for [%s]\n",dstAccNo);
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdLD:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 4 - 2 >= sizeofPinByZpk)
	{
		UnionUserErrLog("in UnionHsmCmdLD:: ret[%d] >= sizeofPinByZpk[%d]!\n",ret-4-2,sizeofPinByZpk);
		return(errCodeParameter);
	}
	snprintf(pinLenBuf,sizeof(pinLenBuf),"%.2s",hsmCmdBuf+4);
	*pinLen = atoi(pinLenBuf);
	memcpy(pinByZpk,hsmCmdBuf+4+2,64);
	pinByZpk[64] = 0;

	return(64);
}

/*
   ����: PIN����ת��Ϊ���������㷨���ܻ�ת
   ָ��: LC
   �������:
   mode		2N	00��PIN��DES/3DES/SM4����תΪ�����������
   01��PIN�ɺ����������תΪSM4/DES/3DES����
   algorithmID	2N	1: des/3des
2: SM4
zpkIndex: 	1A+3H	zpk��Կ������
zpk: 		16H/1A+32H/1A+48H	zpk����
checkValue:	16H	��ZPKΪ��S��+32H�ķ�ʽʱ�������
format: 	2N	��01��-��06�������PIN���ʽ
��PIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
pinBlock:	16H/32H/24H	PIN��Կ���ܵ�PIN������,�����ɼ���ԴPIN����㷨������
��ģʽΪ01�����㷨���ܵ�PIN����
lenOfAccNo:	N	Դ�˺ų���
accNo:		12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	16H/32H		PIN��Կ���ܵ�PIN������,�����ɼ���PIN����㷨������ģʽΪ01������ڡ�
 */
int UnionHsmCmdLC(int mode,int algorithmID,char *zpkIndex,char *zpk,char *checkValue,int format,char *pinBlock,int lenOfAccNo,char *accNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"LC",2);
	hsmCmdLen = 2;

	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%02d",mode);
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);

	if (zpkIndex != NULL && strlen(zpkIndex) > 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"K%s",zpkIndex);
	}
	else
	{
		if (zpk == NULL || strlen(zpk) == 0)
		{
			UnionUserErrLog("in UnionHsmCmdLC:: zpk is null\n");
			return(errCodeParameter);
		}

		if (algorithmID == 1)	// DES/3DES
		{
			// ��Կֵ
			if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk), zpk, hsmCmdBuf+hsmCmdLen)) < 0)
			{
				UnionUserErrLog("in UnionHsmCmdLC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk);
				return(ret);
			}
			hsmCmdLen += ret;

			// PIN��ĸ�ʽ
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%02d",format);
		}
		else	// SM4
		{
			// ��Կֵ
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S%s",zpk);

			// У��ֵ
			if ((checkValue == NULL) || (strlen(checkValue) == 0))
			{
				UnionUserErrLog("in UnionHsmCmdLC:: checkValue is null\n");
				return(errCodeParameter);
			}
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",checkValue);

			// PIN��ĸ�ʽ
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"01");
		}
	}

	// PIN��
	if ((pinBlock == NULL) || (strlen(pinBlock) == 0))
	{
		UnionUserErrLog("in UnionHsmCmdLC:: pinBlock is null\n");
		return(errCodeParameter);
	}

	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);

	if (format == 1)
	{
		// Դ�˺�
		if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdLC:: UnionForm12LenAccountNumber for [%s]\n",accNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (format == 4)
	{
		if (lenOfAccNo != 18)
		{
			UnionUserErrLog("in UnionHsmCmdLC:: format[%02d] lenOfAccNo[%d] != 18\n",format,lenOfAccNo);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
		hsmCmdLen += lenOfAccNo;
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdLC:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 4 >= sizeofPinByZpk)
	{
		UnionUserErrLog("in UnionHsmCmdLC:: ret[%d] >= sizeofPinByZpk[%d]!\n",ret-4,sizeofPinByZpk);
		return(errCodeParameter);
	}
	memcpy(pinByZpk,hsmCmdBuf+4,ret-4);
	pinByZpk[ret-4] = 0;
	return(ret-4);
}

/*
   ����: ˽Կ���ܶԳ���Կ���ٽ��Գ���Կ���ܵĵ�¼����תΪmd5��SHA1������
   ָ��: HR
   �������:
   fillMode	1N	0: ������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油
1: PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
vkIndex		2N	����ֵ"00-20"
lenOfVKValue 	4N	˽Կ����
vkValue		nB	˽Կֵ
lenOfKeyByPK	4N	��Կ���ܵ���Կֵ����
keyByPK		nB	��Կ���ܵ���Կֵ
pinBlock 	16H/32H ��¼��������
random		32H	�����
algorithmID	1N	1��md5, 2��SHA1
sizeofOutPinBlock	nN	pinByMD5����󳤶�
�������:
outPinBlock:	16H/32H
 */
int UnionHsmCmdHR(int fillMode,int vkIndex,int lenOfVKValue,char *vkValue,int lenOfKeyByPK,char *keyByPK,char *pinBlock,char *random,int algorithmID,char *outPinBlock,int sizeofOutPinBlock)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"HR",2);
	hsmCmdLen = 2;

	UnionSetBCDPrintTypeForHSMCmd();
	// ��Կ���ģʽ
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);

	if ((vkIndex >= 0) && (vkIndex <= 20))
	{
		// ˽Կ����
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	}
	else
	{
		// ˽Կ����
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"99");

		if ((vkValue == NULL) || (strlen(vkValue) == 0) || (lenOfKeyByPK <= 0))
		{
			UnionUserErrLog("in UnionHsmCmdHR:: vkValue is null\n");
			return(errCodeParameter);
		}
		// ˽Կ����
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVKValue/2);

		// ˽Կֵ
		aschex_to_bcdhex(vkValue,lenOfVKValue,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += lenOfVKValue/2;
	}

	// ��Կ���ܵ���Կ
	if ((keyByPK == NULL) || (strlen(keyByPK) == 0))
	{
		UnionUserErrLog("in UnionHsmCmdHR:: keyByPK is null\n");
		return(errCodeParameter);
	}

	// ������Կ����
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfKeyByPK/2);

	// ��Կ���ܵ�������Կ
	aschex_to_bcdhex(keyByPK,lenOfKeyByPK,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfKeyByPK/2;

	// ������������
	if ((pinBlock != NULL) && (strlen(pinBlock) == 16))
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"0");
	}
	else if ((pinBlock != NULL) && (strlen(pinBlock) == 32))
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"1");
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdHR:: pinBlock is null or pinBlock is invalid\n");
		return(errCodeParameter);
	}

	// ��������	
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);

	// �����
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",random);

	// �㷨��ʶ
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);
	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdHR:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 4 >= sizeofOutPinBlock)
	{
		UnionUserErrLog("in UnionHsmCmdHR:: ret[%d] >= sizeofOutPinBlock[%d]!\n",ret-4,sizeofOutPinBlock);
		return(errCodeParameter);
	}
	memcpy(outPinBlock,hsmCmdBuf+4,ret-4);
	outPinBlock[ret-4] = 0;
	return(ret-4);
}

// add by leipp 20150926
/*
   ����: ������ɢ��������Կ
   ָ��: US
   �������:
   mode		2N		��ɢģʽ	01: PBOC��ɢ
02: PBOC��ɢ������Կ���������
algorithmID	1A		��ɢ�㷨���� 	X��3DES�㷨	S: SM4�㷨
rootKeyType	3H		����Կ���� 	008,00A,000,001
rootKeyValue	1A+32H/3H	����Կ		�㷨��������ɢ�㷨��������һ��
discreteNum	1N		��ɢ����	1-3
discreteData	N*16H		��ɢ����
processFlag	1A		������Կ��ʶ	Y:�й�����Կ
N:�����ɹ�����Կ
processData	32H		���ɹ�����Կ����
lenOfData	2N		��ɢģʽΪ02ʱ����
data		32H		���ⲿ��0����,��ɢģʽΪ02ʱ����
discreteKeyType	3H		������ɢ����Կ����	008,00A,000,001

�������:
keyValue	32H		��Կ����
checkValue	16H		У��ֵ
lenOfOutData	2N		���ݳ���
outData				��Կ��������������ֵ
 */
int UnionHsmCmdUS(int mode,char algorithmID,TUnionDesKeyType rootKeyType,char *rootKeyValue,int discreteNum,char *discreteData,char *processFlag,char *processData,int lenOfData,char *data,TUnionDesKeyType discreteKeyType,char *keyValue,char *checkValue,int *lenOfOutData,char *outData)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;
	char	tmpBuf[8];
	int	offset = 0;

	if (discreteData == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdUS:: discreteData is null!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"US",2);
	hsmCmdLen = 2;

	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%02d",mode);
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%c",algorithmID);

	// ��Կ����
	if ((ret = UnionTranslateDesKeyTypeTo3CharFormat(rootKeyType, hsmCmdBuf + hsmCmdLen)) <0)
	{
		UnionUserErrLog("in UnionHsmCmdUS::UnionTranslateDesKeyTypeTo3CharFormat!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 3;

	// ��Կֵ
	if (algorithmID == 'S')
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S%s",rootKeyValue);
	}
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(rootKeyValue), rootKeyValue, hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdUS:: UnionGenerateX917RacalKeyString for [%s]\n",rootKeyValue);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// ��ɢ����
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",discreteNum);

	memcpy(hsmCmdBuf+hsmCmdLen,discreteData,discreteNum*16);
	hsmCmdLen += discreteNum*16;

	memcpy(hsmCmdBuf+hsmCmdLen,processFlag,1);
	hsmCmdLen += 1;

	if (processFlag[0] == 'Y')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,processData,32);
		hsmCmdLen += 32;
	}

	if (mode == 2)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfData);
		memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
		hsmCmdLen += lenOfData;
	}

	if ((ret = UnionTranslateDesKeyTypeTo3CharFormat(discreteKeyType, hsmCmdBuf + hsmCmdLen)) <0)
	{
		UnionUserErrLog("in UnionHsmCmdUS::UnionTranslateDesKeyTypeTo3CharFormat!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 3;

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdUS:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	offset = 4;
	memcpy(keyValue,hsmCmdBuf+offset,32);
	keyValue[32] = 0;
	offset += 32;

	memcpy(checkValue,hsmCmdBuf+offset,16);
	checkValue[16] = 0;
	offset += 16;

	if (mode == 2)
	{
		memcpy(tmpBuf,hsmCmdBuf+offset,2);
		tmpBuf[2] = 0;
		*lenOfOutData = atoi(tmpBuf);
		offset += 2;

		memcpy(outData,hsmCmdBuf+offset,ret-offset);
		outData[ret-offset] = 0;
	}
	return(0);
}
// add end by leipp 20150926



//add begin by lusj 20150930
/*
   SM4�����㷨
   ������	����	����	��ע
   ��Ϣͷ	8	A	
   ������	2	A	��WL��
   ��Կ����	3	H	ZEK��00A��
   ZAK��008��
   ZMK����	1A+3H
   1A+32	H	LMK0405�¼��ܵ�ZMK���Ļ���ZMK����
   ZMKУ��ֵ	16	H	����ԿΪ������ʽû�и���
   ��Կ����	32	H	ZMK�¼��ܵ���Կ����
   �����	����	����	��ע
   ��Ϣͷ	8	A	��������ͬ
   ��Ӧ����	2	A	��WM��
   ״̬����	2	N	����Ϊ��00��
   01����ԿУ��ֵ��
   03����Կ���ʹ�
   04��ģʽ����
   12������������Կ��
   13��LMK��
   15���������ݴ�
   33����Կ��������
   36����Կ�㷨��־��
   46�����ݳ��ȴ�
   81����Կ���ȴ�
   Key	32	H          	��Կ���ģ�������Կ������ָ����LMK�Լ���
   У��ֵ	16	H	��Կ����16�ֽ�0�Ľ��
 */

int UnionHsmCmdWL(char *keytype,char *zmkvalue,char *zmkcheckvalue,char *zmkData,char *outkey,char *outcheckvalue)
{

	int		ret;
	char		hsmCmdBuf[8192+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[8192+1];
	//	int			i;
	//	int 		keyLen = 0;

	if ( (keytype == NULL) || (zmkvalue == NULL) || (zmkcheckvalue == NULL) || (zmkData == NULL) )
	{
		UnionUserErrLog("in UnionHsmCmdWL:: wrong parameters!\n");
		return(errCodeParameter);
	}
	//ƴװָ��
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"WL",2);
	hsmCmdLen = 2;	
	//��Կ����
	memcpy(hsmCmdBuf+hsmCmdLen,keytype,3);
	hsmCmdLen+=3;

	//zmk����ֵ
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (strlen(zmkvalue)==3)
	{
		memcpy(tmpBuf,"K",1);
		memcpy(tmpBuf+1,zmkvalue,3);

		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,4);
		hsmCmdLen+=4;		
	}
	else
	{
		if(strlen(zmkvalue)==32)
		{
			memcpy(tmpBuf,"S",1);
			memcpy(tmpBuf+1,zmkvalue,32);

			memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,33);
			hsmCmdLen+=33;	
		}
	}

	//zmkУ��ֵ
	memcpy(hsmCmdBuf+hsmCmdLen,zmkcheckvalue,16);
	hsmCmdLen+=16;	
	//��Կ����
	memcpy(hsmCmdBuf+hsmCmdLen,zmkData,32);
	hsmCmdLen+=32;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWL:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;


	memcpy(outkey,hsmCmdBuf+4,32);
	memcpy(outcheckvalue,hsmCmdBuf+4+32,16);
	return (0);
}
//add end by lusj 20150930


//add begin  by lusj 20151009
/*
   ����: PIN���ת����
   ָ��: W8
   �������:
oriAlgorith:	1N	 ԴPIN���㷨��1-DES/3DES, 2-SM4
desAlgorith:	1N	 Ŀ��PIN���㷨��1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	Դzpk�����Ż�����
desKeyLenFlag: 	1N	Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
desKeyIndex/desZpkValue: 1A+3H/16/32/48H	Ŀ��zpk�����Ż�����
oriFormat:	2N	ԴPIN���ʽ,��01��-��06�������PIN���ʽ
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
desFormat:	2N	Ŀ��PIN���ʽ,��01��-��06�������PIN��
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16��?
pinBlock:	16H/32H		��Կ���ܵ�PIN
oriAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
desAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���

sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	16H/32H		ת���ܺ��PIN����

ע�⣺ԭW8ָ���У���ԴPIN���ʽΪ03ʱ��Դ�˺����ǲ����ڵġ������ڻ��������У�����W8ָ��ʱ��ԴPIN���ʽֻ��Ϊ03����Դ�˺���д��Ϊ12����0��
 */
int UnionHsmCmdW8_HUAX(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"W8",2);
	hsmCmdLen = 2;

	// ԴPIN���㷨
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Ŀ��PIN���㷨
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// Դ��Կ���ȱ�ʶ
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// Դ��Կ���Ļ�������
	if (oriKeyIndex != NULL && strlen(oriKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriKeyIndex);
		hsmCmdLen += 4;
	}
	else if (oriZpkValue != NULL && strlen(oriZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriKeyIndex and oriZpkValue is null!\n");
		return(errCodeParameter);
	}

	// Ŀ����Կ����
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}

	// Ŀ��zpk��Կ
	if (desKeyIndex != NULL && strlen(desKeyIndex) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desKeyIndex);
		hsmCmdLen += 4;
	}
	else if (desZpkValue != NULL && strlen(desZpkValue) != 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",desZpkValue);
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desKeyIndex and desZpkValue is null!\n");
		return(errCodeParameter);
	}

	// ԴPIN��ĸ�ʽ
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// Ŀ��PIN��ĸ�ʽ
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// ԴPIN��
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// Դ�˺�
	if (strcmp(oriFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(oriFormat,"04") == 0)
	{
		if (strlen(oriAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: oriAccNo[%s] len[%d] != 18\n",oriAccNo,(int)strlen(oriAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,oriAccNo,18);
		hsmCmdLen += 18;
	}
	else if (strcmp(oriFormat,"03") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// Ŀ���˺�
	if (strcmp(desFormat,"01") == 0)
	{
		if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (strcmp(desFormat,"04") == 0)
	{
		if (strlen(desAccNo) != 18)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: desAccNo[%s] len[%d] != 18\n",desAccNo,(int)strlen(desAccNo));
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,desAccNo,18);
		hsmCmdLen += 18;
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	// ���Ӽ��ܻ�
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW8:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (ret - 4 >= sizeofPinByZpk)
	{
		UnionUserErrLog("in UnionHsmCmdW8:: ret[%d] >= sizeofPinByZpk[%d]!\n",ret-4,sizeofPinByZpk);
		return(errCodeParameter);
	}
	memcpy(pinByZpk,hsmCmdBuf+4,ret-4);
	pinByZpk[ret-4] = 0;

	return(ret-4);
}
//add end  by lusj 20151009

// add by chenqy 20151025
/*
   ����:��PIN��X9.8תΪ�����㷨����
   ���������
   zpk		16H/32H/48H	zpk��Կֵ
   accNo		12N		�˺�
   pinBlock	16H		zpk���ܵ�pin��������
   algFlag  1A Ŀ��edk��־
   edk		16H/32H/48H	edk��Կ
   ���������
   pin	16H	���ص�pin��
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmdNI(char *zpk,int algFlag, char *edk,char *pinBlock,char *accNo,char *pin)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;

	if (zpk == NULL || accNo == NULL || pinBlock == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdNI:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"NI",2);
	hsmCmdLen = 2;

	// zpk
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNI:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;

	// algFlag
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",algFlag);
	hsmCmdLen += 1;

	if (algFlag == 1)
	{
		// edk
		if(edk == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdNI:: parameter error!\n");
			return(errCodeParameter);
		}
		if ((ret = UnionGenerateX917RacalKeyString(strlen(edk),edk,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdNI:: UnionGenerateX917RacalKeyString [%s]\n",edk);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// pinBlock
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,16);
	hsmCmdLen += 16;

	// accNo
	if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNI:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret = ret - 4;
	memcpy(pin,hsmCmdBuf+4,ret);
	pin[ret] = 0;
	return(ret);
}
// add end

// add by leipp 20151105
/*
   ����:��ZPK���ܵ�PIN������������
   ���������
   algorithmID	1N		�㷨��ʶ,1:des/3des, 2:SM4
   keyIndex	1A+3H		��Կ������
   zpk		16H/1A+32H/1A+48H	zpk��Կֵ
   checkValue	16H		��ZPKΪ����ʱ���������
   format	2N		PIN���ʽ,��PIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ"01"
   pinBlock	16H		zpk���ܵ�pin��������
   accNo	12N/18N		�˺�
   rule		1N		0:6λ����Ϊ��ͬ������
1:ʹ���������/�ݼ�1��˳���������
2:�������0��1����������
���������
����ֵ��
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
=0:	0:��ʾ��������
=1:	1:��ʾ������

 */

int UnionHsmCmdZY(int algorithmID,char *keyIndex,char *zpk,char *checkValue,char *format,char *pinBlock,char *accNo,int rule)
{
	int		ret;
	char		hsmCmdBuf[256];
	int		hsmCmdLen = 0;
	char		tmpBuf[8];

	if (accNo == NULL || pinBlock == NULL || format == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdZY:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"ZY",2);
	hsmCmdLen = 2;

	// PIN���㷨
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);

	if ((keyIndex != NULL) && (strlen(keyIndex) > 0))
	{	// ��Կ������
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",keyIndex);
	}
	else if ((zpk != NULL) && (strlen(zpk) > 0))
	{
		if (algorithmID == 1)
		{
			// zpk
			if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
			{
				UnionUserErrLog("in UnionHsmCmdZY:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
				return(ret);
			}
			hsmCmdLen += ret;
		}
		else
		{
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S%s",zpk);
			if (checkValue == NULL || strlen(checkValue) != 16)
			{
				UnionUserErrLog("in UnionHsmCmdZY:: checkValue parameter error!\n");
				return(errCodeParameter);
			}

			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",checkValue);
		}
	}
	else
	{
		UnionUserErrLog("in UnionHsmCmdZY:: keyIndex or zpk parameter error!\n");
		return(errCodeParameter);
	}

	// pin��ĸ�ʽ
	if (algorithmID == 1)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",format);
	}
	else
	{
		// �����㷨��ֻ��Ϊ01��ʽ
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"01");
	}

	// PIN��
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);

	if (memcmp(format,"01",2) == 0)
	{
		// accNo
		if ((ret = UnionForm12LenAccountNumber(accNo,strlen(accNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdZY:: UnionForm12LenAccountNumber for [%s]\n",accNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else if (memcmp(format,"04",2) == 0)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",accNo);
	}

	// PIN��
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",rule);

	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdZY:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,1);

	return(atoi(tmpBuf));
}
// add end

// add by chenwd 20151106
/*
   ���ܣ����ɹ�Կ���ܵ�PINת����ANSI9.8��׼

   ������  ����    ����    ˵����
   �������        2       A       ֵ"HD"
   �㷨��ʶ        1       N       0
   ˽Կ����        2       N       ָ����˽Կ�����ڽ���PIN��������
   ��䷽ʽ        1       N       0       1
   ԴPIN��ʽ       2       N       07����ũ����ר��
   Ŀ��PIN��ʽ     2       N       01 ANSI9.8
   ��Կ����        3       N       001 ZPK
   ZPKֵ           16/32+A H       ���ڼ���PIN
   ԭ�˺�          12      N       ��ԴPIN��ʽΪ01ʱ����
   Ŀ���˺�        12      N       ��Ŀ��PIN��ʽΪ01ʱ����
   PIN���ĳ���     4       N
   PIN����         n       B       ����Կ���ܵ�PIN��������


   �����  ����    ����    ˵����
   ��Ӧ����        2       A       ֵ"HE"
   �������        2       H
   PIN����         16      H       ���ص�PIN����--ANSI9.8
 */
int UnionHsmCmdHD(char *AlgorithmMode,char *vkIndex,char *DataPadFlag,char *FirstPinCiperFormat,char *SecondPinCiperFormat,
		char *zpk, char *conEsscFldAccNo1, char *conEsscFldAccNo2, int lenOfPinByPK,char *pinByPK,int lenOfVK,char *vk, char *pinByZPK)
{
	int             ret;
	char            hsmCmdBuf[4096];
	int             hsmCmdLen = 0;
	char            tmpBuf[8192];

	if (vkIndex == NULL || zpk == NULL || pinByPK == NULL || pinByZPK == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdHD:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"HD",2);
	hsmCmdLen = 2;

	// �㷨��ʶ
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",AlgorithmMode);

	// ˽Կ����
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",vkIndex);

	//add begin by lusj 20160122 ƽ������HDָ��֧��˽Կ������ܣ����ʹ��HDָ���ֻ�й���ũ�̵�EEZ1���񣬸÷����������ݴ���
	if(strcmp(vkIndex, "99") == 0)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmdHD:: vk is null!\n");
			return(errCodeParameter);
		}

		//˽Կ����
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//˽Կ
		aschex_to_bcdhex(vk,lenOfVK,tmpBuf);
		tmpBuf[lenOfVK/2]=0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;
	}	
	//end by lusj 20160122

	// ��䷽ʽ
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",DataPadFlag);

	// ԴPIN��ʽ
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",FirstPinCiperFormat);

	// Ŀ��PIN��ʽ
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",SecondPinCiperFormat);

	// ��Կ���� 001 ZPK
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"001");

	// ZPKֵ
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdHD:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;

	// ԭ�˺�
	if(strlen(conEsscFldAccNo1) != 0 )
	{
		memcpy(hsmCmdBuf+hsmCmdLen,conEsscFldAccNo1,12);
		hsmCmdLen += 12;
	}

	// Ŀ���˺�
	if(strlen(conEsscFldAccNo2) != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,conEsscFldAccNo2,12);
		hsmCmdLen += 12;
	}

	// PIN���ĳ���
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%04d", lenOfPinByPK);

	// PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;	

	hsmCmdBuf[hsmCmdLen] = 0;
	
	bcdhex_to_aschex(hsmCmdBuf,hsmCmdLen,tmpBuf);	
	tmpBuf[2*hsmCmdLen]=0;
	UnionLog("in UnionHsmCmdHD:: hsmCmdBuf[%s]!\n",tmpBuf);


	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdHD:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret -= 4;
	memcpy(pinByZPK,hsmCmdBuf+4,ret);
	pinByZPK[ret] = 0;

	return(0);
}
// add end 

/*
   add by leipp 20151111
   ����:	�����Ĵ�ũ��˽���㷨��Կ
   ������	��Կ��3���ɷ���ɣ�ÿ���ɷݲ���8���ֽ�ʱ����0.
   �ڼ��ܻ��ڿ���ר�ŵĴ洢���洢�����ٴ�������

   ������   	��������    ˵����
   �������        2A      	ֵ"SJ"
   key1		16H		LMK0607�¼���
   key2		16H		LMK0607�¼���
   key3		16H		LMK0607�¼���

   �����  	��������	˵��
   ��Ӧ����        2A       	ֵ"SK"

 */
int UnionHsmCmdSJ(char *key1, char *key2, char *key3)
{
	int             ret;
	char            hsmCmdBuf[512];
	int             hsmCmdLen = 0;
	char		tmpBuf[32];

	if((key1 == NULL) || (key2 == NULL) || (key3 == NULL) || (strlen(key1)%16 != 0) || (strlen(key2)%16 != 0) || (strlen(key3)%16 != 0))
	{
		UnionUserErrLog("in UnionHsmCmdSJ:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"SJ",2);
	hsmCmdLen = 2;

	memset(tmpBuf, '0', sizeof(tmpBuf));
	memcpy(tmpBuf, key1, strlen(key1));
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,16);
	hsmCmdLen += 16;

	memset(tmpBuf, '0', sizeof(tmpBuf));
	memcpy(tmpBuf, key2, strlen(key2));
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,16);
	hsmCmdLen += 16;

	memset(tmpBuf, '0', sizeof(tmpBuf));
	memcpy(tmpBuf, key3, strlen(key3));
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSJ:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	return(ret);
}


/**
  add by leipp 20151111

  ����:����׼PinBlockתΪ�����㷨���ܵ�Pin

  ������   	��������    ˵����
  �������        2A      	ֵ"SN"
  mode		1N      	ģʽ��־,1:���ܴ���PIN
5:���ܿ�PIN
keyValue	16H/32H/	ZPK��Կֵ
1A+32H/1A+48H
pinType		2A		01��ANSIx9.8��ʽ
accout		12H		PIN�˺�
pinBlock	16H		PIN����,ZPK�¼��ܵ�PIN���ģ�����pin��ʽ��6λ����
sizeOfEncPin	nN		�������ĳ���

�����  	��������	˵��
��Ӧ����        2A       	ֵ"SO"
encPin		16H

 **/

int UnionHsmCmdSN(int mode, char *keyValue, char *pinType, char *accout, char *pinBlock, char *encPin, int sizeOfEncPin)
{
	int             ret;
	char            hsmCmdBuf[512];
	int             hsmCmdLen = 0;

	if ((keyValue == NULL) || (pinType == NULL) || (accout == NULL) || (pinBlock == NULL) || (sizeOfEncPin < 16))
	{
		UnionUserErrLog("in UnionHsmCmdSN:: parameter error!\n");          
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"SN",2);
	hsmCmdLen = 2;

	if(mode != 1 && mode != 5)	
	{
		UnionUserErrLog("in UnionHsmCmdSN:: mode[%d] error!\n",mode);
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mode);
	hsmCmdLen += 1;

	if ((ret = UnionPutKeyIntoRacalKeyString(keyValue,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSN:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyValue);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen,pinType,2);
	hsmCmdLen += 2;

	if ((ret = UnionForm12LenAccountNumber(accout,strlen(accout),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSN:: UnionForm12LenAccountNumber for [%s]\n",accout);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen, pinBlock, 16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSN:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	memcpy(encPin, hsmCmdBuf+4, 16);
	encPin[16] = 0;
	return(16);
}

/**
  add by leipp 20151111

  ���ܣ���������pin(�Ĵ�ũ��)

  ������   	��������    ˵����
  �������        2A      	ֵ"SP"
  mode		1N      	ģʽ��־,1:���ܴ���PIN
5:���ܿ�PIN
pin		6H		pin����,6λ����
sizeOfEncPin	nN		�������ĳ���

�����  	��������	˵��
��Ӧ����        2A       	ֵ"SQ"
encPin		16H

 **/
int UnionHsmCmdSP(int mode, char *pin, char *encPin, int sizeOfEncPin)
{
	int             ret;
	char            hsmCmdBuf[512];
	int             hsmCmdLen = 0;

	if ((pin == NULL) || (sizeOfEncPin < 16))
	{
		UnionUserErrLog("in UnionHsmCmdSP:: parameter error!\n");          
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"SP",2);
	hsmCmdLen = 2;

	if(mode != 1 && mode != 5)	
	{
		UnionUserErrLog("in UnionHsmCmdSP:: mode[%d] error!\n",mode);
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mode);
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf+hsmCmdLen, pin, 6);
	hsmCmdLen += 6;

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSP:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	memcpy(encPin, hsmCmdBuf+4, 16);
	encPin[16] = 0;
	return(16);
}

/*
   �������ܣ�
   SHָ�����VISA��У��ֵCVV
   ���������
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   cvv�����ɵ�VISA����CVV
   ����ֵ��
   <0������ִ��ʧ�ܣ���cvv
   >0���ɹ�������cvv�ĳ���
 */

int UnionHsmCmdSH(char *cardValidDate,int cvkLength,char *cvk, char *accNo,int lenOfAccNo,char *serviceCode,char *cvv)
{
	int             ret;
	char            hsmCmdBuf[512];
	int             hsmCmdLen = 0;

	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdSH:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// ������
	memcpy(hsmCmdBuf,"SH",2);
	hsmCmdLen = 2;
	// LMK���ܵ�CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSH:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// �ͻ��ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA������Ч��
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA�������̴���
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSH:: UnionDirectHsmCmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	// ���ɵ�VISA����CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	cvv[3] = 0;
	return(3);
}

/*
   �������ܣ�
   XSָ���֤VISA����CVV
   ���������
   cvv������֤��VISA����CVV
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   errCode���������
   ����ֵ��
   <0������ִ��ʧ��
   0����֤�ɹ�
 */

int UnionHsmCmdXS(char *cvv,char *cardValidDate, int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode)
{
	int             ret;
	char            hsmCmdBuf[512];
	int             hsmCmdLen = 0;

	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdXS:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// ������
	memcpy(hsmCmdBuf,"XS",2);
	hsmCmdLen = 2;
	// LMK���ܵ�CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdXS:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ����֤��VISA����CVV
	memcpy(hsmCmdBuf+hsmCmdLen,cvv,3);
	hsmCmdLen += 3;
	// �ͻ��ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA������Ч��
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA�������̴���
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdXS:: UnionDirectHsmCmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	return(0);
}


//add begin by lusj 20151116
/*
   ָ��:PQ
   ����:PINת���ܣ�PKCS#7����תZPK���ܣ�
   �ൺ���ж���ָ��
   �������:
   mode  	�㷨��־  				1A  
   0-	DES/RSA
   1-	SM4/SM2
   vkIndex	RSA/SM2˽Կ����			2 N
   lenOfvk	RSA/SM2˽Կ���ĳ���		4 N
   vk		RSA/SM2˽Կ����			n B	
   ��˽Կ����Ϊ"99"ʱ����
   LMK���ܵ�˽Կ������e,d,m��5��CRT�ɷݣ�
   lenOfKeyByPk	�����ܻỰ��Կ����		4 N
   RSA/SM2��Կ���ܵĻỰ(DES/SM4��Կ)��Կ����
   keyByPk	�����ܵĻỰ��Կ			n B				
   RSA/SM2��Կ���ܵĻỰ(DES/SM4��Կ)��Կ��
   ����Ҫ��DES��Կ�������ǿ����У�顣
   dateType	��������ģʽ			1N					
   0-	PIN����
   1-	PIN����
   lenOfpinBlock	�����ܵ�PIN��PIN��������	4 N
   DES/SM4��Կ���ܵ�PIN���Ļ�PIN��������
   Pin		�����ܵ�PIN����/����		n B
   DES/SM4��Կ���ܵ�PIN���Ļ�PIN��������PKCS#5��䣩
   PinTab	PIN����ת����			10 N
   ����������ģʽΪ1��ʱ�������ڡ�
   accNo		�˺�					12 N	�û����˺ţ���Чλ������12�����֡�
   keyOfZpk	Ŀ��ZPK��Կ				16 H/1A + 32 H/1A + 48 H						
   LMK�¼��ܵ�Ŀ��ZPK����
   ���㷨��־Ϊ1ʱ������ǿ��ΪS+32H
   ���㷨��־Ϊ0ʱ������ΪX+32H
   format	PINBLOCK��ʽ			2 N	
   PIN���ݿ�ĸ�ʽ���룬�μ�PINBLOCK��ʽ
   PIN��ʽΪ08ʱ��Ϊ�ൺ����PIN��ʽ��
   �������:
   pinlen	����PIN�ĳ���			2 N	
   dst_pinBlock	PINBLOCK����			16 H/32H

 */
int UnionHsmCmdPQ(int mode, int vkIndex, int lenOfvk, char *vk, int lenOfKeyByPk,char *keyByPk,int dateType,int lenOfpinBlock,char *Pin,char *PinTab,char *accNo,char *keyOfZpk,int format,char *pinlen,char *dst_pinBlock)
{
	int 		ret = 0;
	char		hsmCmdBuf[8192];
	int 		hsmCmdLen = 0;
	int 		len = 0;


	memcpy(hsmCmdBuf,"PQ",2);
	hsmCmdLen = 2;

	if (keyByPk == NULL || Pin == NULL ||accNo ==NULL ||keyOfZpk ==NULL)
	{
		UnionUserErrLog("in UnionHsmCmdPQ:: parameter error!\n");
		return(errCodeParameter);
	}

	// �㷨ģʽ
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	// ������
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;

	//vk
	if (vkIndex == 99)
	{
		if (lenOfvk <= 0 || vk == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdPQ:: keyLen[%d] <= 0 or keyValue == NULL!\n",lenOfvk);
			return(errCodeParameter);
		}


		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfvk/2);
		hsmCmdLen += 4;

		len = aschex_to_bcdhex(vk,lenOfvk,hsmCmdBuf+hsmCmdLen);	
		hsmCmdLen += len;
	}

	//keyByPk
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfKeyByPk/2);
	hsmCmdLen += 4;

	len = aschex_to_bcdhex(keyByPk,lenOfKeyByPk,hsmCmdBuf+hsmCmdLen);	
	hsmCmdLen += len;

	//dateType
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",dateType);
	hsmCmdLen += 1;

	//Pin
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfpinBlock/2);
	hsmCmdLen += 4;

	len = aschex_to_bcdhex(Pin,lenOfpinBlock,hsmCmdBuf+hsmCmdLen);	
	hsmCmdLen += len;	

	//PinTab
	if(dateType == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,PinTab,10);
		hsmCmdLen += 10;
	}

	//accNo
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,12);
	hsmCmdLen += 12;

	//keyOfZpk
	if(mode == 0)
		memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
	else
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);

	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,keyOfZpk,32);
	hsmCmdLen += 32;

	//format
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",format);
	hsmCmdLen += 2;

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPQ:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret = ret - 6;

	memcpy(pinlen,hsmCmdBuf+4,2);
	pinlen[2]=0;


	ret=bcdhex_to_aschex(hsmCmdBuf+6, ret, dst_pinBlock);

	dst_pinBlock[ret] = 0;

	return(0);

}
//add end by lusj 20151116



//add begin by lusj 20151119

/*
   �ൺ���ж���ָ��

   ����: 
   ������SM2�㷨��Կ����ת����ΪSM4�㷨���ܡ�

   ���������
   vkIndex	˽Կ����	2N
   lenOfvk	˽Կ����	4N
   vk		˽Կ����	nB
   keyType	�Գ���Կ����	3H
   keyValue	�Գ���Կ	1A+32H
   lenOfdata	���ݳ���	4N
   data		������	nB

   ���������
   cipherData	�Գ���Կ���ܺ������	n*2

   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmdPR(int vkIndex, int lenOfvk, char *vk,TUnionDesKeyType keyType,char *keyValue,int lenOfdata,char *data,char *cipherData)
{
	int		ret = 0;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		len = 0;

	if (keyValue == NULL || data == NULL || lenOfdata==0)
	{
		UnionUserErrLog("in UnionHsmCmdPR:: parameter error!\n");
		return(errCodeParameter);
	}

	// ָ��
	memcpy(hsmCmdBuf,"PR",2);
	hsmCmdLen = 2;

	// ���ģʽ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	// ������
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;

	//vk
	if (vkIndex == 99)
	{
		if (lenOfvk <= 0 || vk == NULL)
		{
			UnionUserErrLog("in UnionHsmCmdPR:: lenOfvk[%d] <= 0 or vk == NULL!\n",lenOfvk);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfvk/2);
		hsmCmdLen += 4;

		len = aschex_to_bcdhex(vk,lenOfvk,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += len;
	}

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPR:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//��Կֵ
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue,32);
	hsmCmdLen += 32;

	//����
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfdata/2);
	hsmCmdLen += 4;

	len = aschex_to_bcdhex(data,lenOfdata,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += len;

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPR:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	ret = ret - 4;
	memcpy(cipherData,hsmCmdBuf+4,ret);
	cipherData[ret] = 0;

	return(ret);
}
//add end by lusj 20151119


//add begin by lusj 20151203
/*
   �������ܣ�
   9Fָ�ʹ�ú���ũ��ԭ�����㷨У���������ʹ��SM4PVK��Կ����һ��PIN offset��

   ���������
   pvk					pvk					1A+32H			LMK�ԣ�14-15���¼��ܵ�PVK����������offset����SM4)
   zpk					zpk					1A+32H			LMK�ԣ�06-07���¼��ܵ�ZPK��
   algorithmID			�㷨��ʾ		1N  				1��DES�㷨����PINBLOCK
   2��SM4�㷨����PINBLOCK
   pinBlockByZPK		PINBLOCK����		16H/32H		��ZPK���ܵ�PINBLOCK
   pinLength				PINBLOCK���ĳ���
   minPINLength			��鳤��		2N				��С��PIN���ȡ�
   accNo1				�˺�1				12N				����PINBLOCK���˺�
   lenOfAccNo2			�˺�2����		2N	
   accNo2				�˺�2				N				����ũ�ź������㷨�˺�
   checkPin				��У�������	8A			����ũ�����㷨���������������
   decimalizationTable	ʮ����ת����	16H			��ʮ������ת��Ϊʮ���Ƶ�ת����
   checkData			PINУ������		12A			�û�����ġ�����ʮ�������ַ����ַ�"N"�����ݣ�����ָʾHSM�����˺����5λ��λ�á�

   ���������
   pinOffset��PIN Offset������룬�Ҳ�'F'

 */
int UnionHsmCmd9F(char *pvk,char *zpk,int algorithmID,char *pinBlockByZPK,int pinLength,int minPINLength,int lenOfAccNo1,char *accNo1,int lenOfAccNo2,char *accNo2,char *checkPin,char *decimalizationTable,char *checkData,char *pinOffset)
{
	int		ret;
	char	hsmCmdBuf[512];
	int		hsmCmdLen = 0;


	if ((pvk == NULL) || (zpk == NULL) || (pinBlockByZPK == NULL) || (accNo1 == NULL) || (accNo2 == NULL) || (decimalizationTable == NULL)||(checkData == NULL)||(checkPin ==NULL))
	{
		UnionUserErrLog("in UnionHsmCmd9F:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// ������
	memcpy(hsmCmdBuf,"9F",2);
	hsmCmdLen = 2;

	// LMK���ܵ�PVK
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S");
	memcpy(hsmCmdBuf+hsmCmdLen,pvk,32);
	hsmCmdLen += 32;

	//zpk����
	if (algorithmID == 1)
	{
		//DES�㷨
		memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zpk,32);
		hsmCmdLen += 32;
	}
	else
	{
		//SM4�㷨
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zpk,32);
		hsmCmdLen += 32;
	}

	// �㷨��ʾ
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;


	//PINBLOCK����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,pinLength);
	hsmCmdLen += pinLength;

	//��鳤��
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;

	//�˺�1
	if ((ret = UnionForm12LenAccountNumber(accNo1,lenOfAccNo1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9F:: UnionForm12LenAccountNumber for [%s]\n",accNo1);
		return(ret);
	}
	hsmCmdLen += ret;


	//�˺�2����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAccNo2);
	hsmCmdLen += 2;

	//�˺�2
	memcpy(hsmCmdBuf+hsmCmdLen,accNo2,lenOfAccNo2);
	hsmCmdLen += lenOfAccNo2;	

	//��У�������
	memcpy(hsmCmdBuf+hsmCmdLen,checkPin,8);
	hsmCmdLen += 8;	

	//ʮ����ת����
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;

	//PINУ������
	memcpy(hsmCmdBuf+hsmCmdLen,checkData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;


	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9F:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(pinOffset,hsmCmdBuf+4,12);
	pinOffset[12]=0;
	return(12);

}
//add end by lusj 20151203



//add begin by lusj 20151208
/*
   �������ж���ָ��

   �������ܣ�
   ��Կ����PINת��DES���ܣ�֧���ַ��������㷨��

   ���������

   algorithmID	  		�㷨��ʾ				1N		  	1��RSA	   2��SM2
   vkIndex			˽Կ����    				2N   		"00"��"20"  	"99"��ʹ�������Կ
   lenOfVK   			���˽Կ����		4N			������Կ����Ϊ99ʱ�д�����һ���򳤶�		
   vk					���˽Կ
   fillMode				�ù�Կ����ʱ��		1N 	 	 	���㷨��ʾΪ��RSAʱ�д���		
   ���õ���䷽ʽ					
   ZEKValue			zekֵ		
   format				��λ��ʽ				1N   		1�����0x00��    2�����0x80��
   encrypMode			����ģʽ				1N   		1:ECB	 2:CBC
   dataPrefixLen		�������ǰ׺����	2N			����ǣ�00����û��ǰ׺�������
   dataPrefix			�������ǰ׺		3H			��������ݳ���ǰ׺׺����00ʱ�д���
   dataSuffixLen		������ݺ�׺����	2N			����ǣ�00����û�к�׺�������
   dataSuffix			������ݺ�׺		3H			��������ݺ�׺���Ȳ���00ʱ�д���
   lenOfPinByPK		���ĳ���				4N
   PinByPK			��Կ��������   		nB   			����Կ���ܵ�PIN��������SM2�㷨Ϊc1+c3+c2
   specialAlg  			ר���㷨��ʾ    		1A 			��ѡ��
   P-���밲ȫ�ؼ���ȫ�㷨
   Q-�ж����븴�Ӷȣ�����ؼ���ȫ�㷨��
   ���������
   PinByZek			PIN����					nH			zek���ܵ����ݳ���
   complexityOfPin		 PIN���븴�Ӷ�              2N 	 		��ר���㷨Ϊ"Q"ʱ�д���
   01��������
   02������+��ĸ
   03������+��ĸ�����ִ�Сд��
 */
int UnionHsmCmd6A(int algorithmID,int algorithmID_ZEK,int vkIndex,int lenOfVK,char *vk,int fillMode,char *specialAlg ,char *ZEKValue,int format,int encrypMode,int lenOfIV,char *IV,int dataPrefixLen,char *dataPrefix,int dataSuffixLen,char *dataSuffix,int lenOfPinByPK,char *PinByPK,char *PinByZek,char *complexityOfPin)
{

	int 		ret;
	int 		lenOfPinByZek;
	int     	hsmCmdLen = 0;

	char		hsmCmdBuf[8192];
	char		tmpBuf[8192];

	if (PinByPK == NULL)
	{
		UnionUserErrLog("in UnionHsmCmd6A:: wrong parameters!\n");
		return(errCodeParameter);
	}


	//���ܻ�ָ��
	memcpy(hsmCmdBuf,"6A",2);
	hsmCmdLen = 2;

	//�㷨��ʶ
	if((algorithmID !=1) && (algorithmID !=2))
	{
		UnionUserErrLog("in UnionHsmCmd6A:: algorithmID is not 1 or 2!\n");
		return(errCodeParameter);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;


	//˽Կ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;	

	if(vkIndex==99)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmd6A:: vk is null!\n");
			return(errCodeParameter);
		}

		//˽Կ����
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//˽Կ
		aschex_to_bcdhex(vk,lenOfVK,tmpBuf);
		tmpBuf[lenOfVK/2]=0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;	
	}

	if(algorithmID == 1)
	{
		//�ù�Կ����ʱ�����õ���䷽ʽ
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
		hsmCmdLen += 1;		
	}

	if((specialAlg[0] =='P')||(specialAlg[0] =='Q'))
	{
		memcpy(hsmCmdBuf+hsmCmdLen,specialAlg,1);
		hsmCmdLen += 1;	
	}	

	if(specialAlg[0] !='Q')
	{
		//��Կ����
		memcpy(hsmCmdBuf+hsmCmdLen,"00A",3);
		hsmCmdLen +=3;	

		//ZEK
		if(strlen(ZEKValue) == 16)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,ZEKValue,16);
			hsmCmdLen +=16;		
		}
		else if(strlen(ZEKValue) == 32)
		{
			if(algorithmID_ZEK==1)
				memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			else
				memcpy(hsmCmdBuf+hsmCmdLen,"S",1);

			hsmCmdLen +=1;	

			memcpy(hsmCmdBuf+hsmCmdLen,ZEKValue,strlen(ZEKValue));
			hsmCmdLen +=strlen(ZEKValue); 	
		}
		else if(strlen(ZEKValue) == 48)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen +=1;	

			memcpy(hsmCmdBuf+hsmCmdLen,ZEKValue,strlen(ZEKValue));
			hsmCmdLen +=strlen(ZEKValue); 

		}

		//��λ��ʽ
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",format);
		hsmCmdLen += 1;	

		//����ģʽ
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",encrypMode);
		hsmCmdLen += 1;		

		if(encrypMode ==2)
		{
			//IV
			if(algorithmID_ZEK == 2)
			{
				if(lenOfIV !=32)
				{
					UnionUserErrLog("in UnionHsmCmd6A:: lenOfIV is not 32!\n");
					return(errCodeParameter);	
				}
			}
			memcpy(hsmCmdBuf+hsmCmdLen,IV,lenOfIV);
			hsmCmdLen +=lenOfIV; 
		}

		//�������ǰ׺����
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",dataPrefixLen);
		hsmCmdLen += 2;	

		if(dataPrefixLen !=0)
		{
			//ǰ׺����
			memcpy(hsmCmdBuf+hsmCmdLen,dataPrefix,dataPrefixLen);
			hsmCmdLen +=dataPrefixLen; 	
		}

		//������ݺ�׺����
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",dataSuffixLen);
		hsmCmdLen += 2;		

		if(dataSuffixLen !=0)
		{
			//������ݺ�׺
			memcpy(hsmCmdBuf+hsmCmdLen,dataSuffix,dataSuffixLen);
			hsmCmdLen +=dataSuffixLen; 	
		}
	}

	//���ĳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;		

	//����
	aschex_to_bcdhex(PinByPK,lenOfPinByPK,tmpBuf);	
	tmpBuf[lenOfPinByPK/2]=0;
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfPinByPK/2);
	hsmCmdLen += lenOfPinByPK/2;

	hsmCmdBuf[hsmCmdLen] = 0;


	bcdhex_to_aschex(hsmCmdBuf,hsmCmdLen,tmpBuf);	
	tmpBuf[2*hsmCmdLen]=0;
//	UnionDebugLog("in UnionHsmCmd6A:: hsmCmdBuf[%s]!\n",tmpBuf);

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd6A:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if(specialAlg[0] !='Q')
	{
		lenOfPinByZek=ret-6;
		memcpy(PinByZek,hsmCmdBuf+6,lenOfPinByZek);
		PinByZek[lenOfPinByZek]=0;
	}
	else
	{
		memcpy(complexityOfPin,hsmCmdBuf+4,2);
		PinByZek[2]=0;
	}
	return(ret-4);

}		

//add end by lusj 20151208

//edit by yangw begin
/*��lmk������Կת��Ϊpk����
  ���������
  keyType ��Կ����
  keyValue LMK��Կ��������
  checkValue У��ֵ
  pkIndex  ��Կ����
  lenOfPK  ��Կ����
  pk ��Կֵ
  �������
  lenOfKeyByPK PK���ܵ���Կֵ����
  keyByPK  PK���ܵ���Կֵ
  pkHash   ��Կ����HASH
  ����ֵ��
  PK���ܵ���Կֵ����

 */
int UnionHsmCmdWN(char *keyType, char* keyValue, char* checkValue,int pkIndex,int lenOfPK,char *pk,int* lenOfKeyByPK,char* keyByPK,char* pkHash)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	offset = 0;
	char	buf[10];

	if ((keyValue == NULL) || (checkValue == NULL)|| (keyType == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdWN:: wrong parameters!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf + offset,"WN",2);
	offset += 2;

	memcpy(hsmCmdBuf + offset,keyType,3);
	offset += 3;

	//��Կ��ʶ
	memcpy(hsmCmdBuf + offset,"S",1);
	offset += 1;

	memcpy(hsmCmdBuf + offset,keyValue,32);
	offset += 32;

	memcpy(hsmCmdBuf + offset,checkValue,16);
	offset += 16;


	if(pkIndex==99)
	{
		if((lenOfPK==0)||(pk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmdWN:: PK is null!\n");
			return(errCodeParameter);
		}

		// ˽Կ����
		sprintf(hsmCmdBuf+offset,"%02d",pkIndex);
		offset += 2;
		memcpy(hsmCmdBuf + offset,pk,lenOfPK);//lenOfPK = 64
		offset += lenOfPK;


	}	
	else if ((pkIndex >= 0) && (pkIndex <= 20))
	{
		// ˽Կ����
		sprintf(hsmCmdBuf+offset,"%02d",pkIndex);
		offset += 2;
	}
	hsmCmdBuf[offset] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWN:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(buf,hsmCmdBuf+4,4);
	buf[4] = 0;
	*lenOfKeyByPK = atoi(buf);
	memcpy(keyByPK,hsmCmdBuf+4+4,*lenOfKeyByPK);
	memcpy(pkHash,hsmCmdBuf+4+4+*lenOfKeyByPK,32);	 
	return(*lenOfKeyByPK);
}
//edit by yangw end
// add by chenwd 20151216
/*
   ��ZPK���ܻ���PINתΪ����ר���㷨�õ�PIN�������
   ���������
   zpkKeyValue     16H��1A+32H��1A+48H     ԴZPK           ��ǰ����PIN���ZPK��LMK�ԣ�06-07���¼���
   pinBlock        16H                     ԴPIN��         ԴZPK�¼��ܵ�ԴPIN��
   pinFormat       2N                      ԴPIN���ʽ
   accout          12N                     �˺�            �˺���ȥ��У��λ������12λ
   resPinFormat    2N                      Ŀ��PIN���㷨   01������ũ����ר���㷨  02��̫��ũ����ר���㷨  03���Ͼ�����ר���㷨 04: ��������ר���㷨
   parameter       8A/12A/16H              ���д������    8A̫������ 12A�Ͼ�����	16H��������
   ���������
   resPinBlock     16H     Ŀ��PIN��
 */
int UnionHsmCmdS5(char* zpkKeyValue, char* pinBlock, char* pinFormat,char* accout,char* resPinFormat,char* parameter,char* resPinBlock)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	char	pinLenBuf[16];
	int	pinLen = 0;

	if ((zpkKeyValue == NULL) || (pinBlock == NULL) || (pinFormat == NULL) || (accout == NULL) || (resPinFormat == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdS5:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"S5",2);
	hsmCmdLen = 2;

	// add by zhouxw 20160111 begin
	// ���������㷨��Կ�̶�Ϊ16H
	if(memcmp(resPinFormat, "04", 2) == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, zpkKeyValue, 16);
		hsmCmdLen += 16;
	}
	else
		// add by zhouxw end 
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(zpkKeyValue,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdS5:: UnionPutKeyIntoRacalKeyString [%s]!\n",zpkKeyValue);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, pinBlock, 16);
	hsmCmdLen += 16;

	memcpy(hsmCmdBuf+hsmCmdLen, pinFormat, 2);
	hsmCmdLen += 2;

	// modify by zhouxw 20160111 begin
	//memcpy(hsmCmdBuf+hsmCmdLen, accout, 12);
	//hsmCmdLen += 12;
	if ((ret = UnionForm12LenAccountNumber(accout,strlen(accout),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd5S:: UnionForm12LenAccountNumber for [%s]\n",accout);
		return(ret);
	}
	hsmCmdLen += ret;

	// modify by zhouxw 20160111 end

	memcpy(hsmCmdBuf+hsmCmdLen, resPinFormat, 2);
	hsmCmdLen += 2;

	if((memcmp(resPinFormat,"02",2)==0)&&(parameter!=NULL))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, parameter, 8);
		hsmCmdLen += 8;
	}
	else if((memcmp(resPinFormat,"03",2)==0)&&(parameter!=NULL)&&(strlen(parameter)==12))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "12",2);
		hsmCmdLen += 2;
		memcpy(hsmCmdBuf+hsmCmdLen,parameter,12);
		hsmCmdLen += 12;
	}
	else if((memcmp(resPinFormat,"03",2)==0)&&((parameter ==NULL)||(strlen(parameter) ==0)))
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"00",2);
		hsmCmdLen += 2;
	}
	// add by zhouxw 20160111 begin
	else if((memcmp(resPinFormat,"04",2)== 0)&&(parameter!=NULL)&&(strlen(parameter)==16))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, parameter, 16);
		hsmCmdLen += 16;
	}
	// add by zhouxw 20160111 end

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdS5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	// add by zhouxw 20160111 begin
	// �Ͼ�����ר���㷨��û���س���
	if(memcmp(resPinFormat, "03", 2) == 0)
	{
		// add by zhouxw 20160111 end
		memcpy(resPinBlock, hsmCmdBuf+4, 16);
		resPinBlock[16] = 0;
		return(16);
		// add by zhouxw 20160111 begin
	}
	else
	{
		memcpy(pinLenBuf, hsmCmdBuf+4, 2);
		pinLenBuf[2] = 0;
		pinLen = atoi(pinLenBuf);
		memcpy(resPinBlock, hsmCmdBuf+6, pinLen*2);
		resPinBlock[pinLen * 2] = 0;
		return(pinLen * 2);
	}
	// add by zhouxw 20160111 end
}
/*
   ���Ͼ�����ר���㷨��PIN����תΪX9.8��ʽ��PIN�������
   ���������
   parameter	12A			���д������
   resPinFormat    2N                      Ŀ��PIN���㷨     01��ANSI9.8
   zpkKeyValue     16H��1A+32H��1A+48H     Ŀ��ZPK           LMK�ԣ�06-07���¼���
   accout          12N                     �˺�              �˺���ȥ��У��λ������12λ
   pinBlock        16H                     Pin������         �Ͼ�����ר���㷨����
   ���������
   resPinBlock     16H			Ŀ��PIN��
 */
int UnionHsmCmdSR(char* parameter, char* resPinFormat, char* zpkKeyValue, char* accout, char* pinBlock, char* resPinBlock)
{
	int     ret;
	char    hsmCmdBuf[512];
	int     hsmCmdLen = 0;

	if ((zpkKeyValue == NULL) || (pinBlock == NULL) || (accout == NULL) || (resPinFormat == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdSR:: parameter error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf,"SR",2);
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf+hsmCmdLen,"33",2);
	hsmCmdLen += 2;

	if((parameter != NULL)&&(strlen(parameter) == 12))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "12",2);
		hsmCmdLen += 2;
		memcpy(hsmCmdBuf+hsmCmdLen,parameter,12);
		hsmCmdLen += 12;
	}
	else if((parameter == NULL)&&(strlen(parameter) == 0))
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"00",2);
		hsmCmdLen += 2;
	}

	memcpy(hsmCmdBuf+hsmCmdLen, resPinFormat, 2);
	hsmCmdLen += 2;

	if ((ret = UnionPutKeyIntoRacalKeyString(zpkKeyValue,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSR:: UnionPutKeyIntoRacalKeyString [%s]!\n",zpkKeyValue);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf+hsmCmdLen, accout, 12);
	hsmCmdLen += 12;

	memcpy(hsmCmdBuf+hsmCmdLen, pinBlock, 16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSR:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(resPinBlock, hsmCmdBuf+4, 16);
	resPinBlock[16] = 0;
	return(16);
}
//add by chenwd end

// add by leipp 20151228
/*
   ��¼�����ʼ������SM2��Կ���ܵ����ģ�ת����SM3/ZPK�����㷨�������
   ���������
   mode		1A			1-	����PIN������ӣ�SM3ժҪ���
   2-	���PIN������ӣ�SM3ժҪ���
   3-	SM2��Կ����תΪZPK��SM4�㷨���������
   vkIndex		2N			00-20��99��ʾSM2���
   vkLen		4N			������Կ����Ϊ99ʱ�д���
   vkValue     	nB			������Կ����Ϊ99ʱ�д���SM2��Կ����
   lenOfPinByPK	4N			���ĳ���
   pinByPK		nB			SM2��Կ���ܵ����ġ���C1+C3+C2��
   factorData	8H			���ط����ӣ����ⲿ���룬˽Կ�������ĺ󣬻���������ģ��ɴ������ӱȶ�
   keyValue	1A+32H			PIN���������Կ/ZPK, PIN���������PIN���Ľ����ϳ�PINBLOCK����������ɼ��ܻ��������32λ16��������
   ��ģʽΪ1��2��Կ����ΪZEK��SM4��Կ����
   ��ģʽΪ3ΪZPK(SM4��Կ)��
   randomData	32H			PIN�����������
   lenOfAccNo	2N			�˺ų���
   accNo		nN			�˺�

   ���������
   pinByZEK	32H			��ģʽΪ1����ZEK���ܵ�pin����������ġ�
   digest		64H			SM3�㷨ժҪ����ģʽ��־Ϊ1��2�д���
   pinBlock	32H			��ģʽΪ3��Zpk���ܵ�PINBLOCK���ģ�ANSI 9.8��ʽ����
 */

int UnionHsmCmd8B(int mode, int vkIndex,int vkLen,char *vkValue,int lenOfPinByPK, char *pinByPK, char *factorData, char *keyValue, char *randomData, int lenOfAccNo,char *accNo,char *pinByZEK, char *digest, char *pinBlock)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	offset = 0;

	if ((mode < 1 || mode > 3) || vkIndex < 0 || (vkIndex > 20 && vkIndex != 99) || 
			factorData == NULL || strlen(factorData) != 8 || keyValue == NULL || strlen(keyValue) != 32)
	{
		UnionUserErrLog("in UnionHsmCmd8B::parameter error!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "8B", 2);
	hsmCmdLen += 2;

	// mode
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mode);
	hsmCmdLen += 1;

	// vkIndex
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	hsmCmdLen += 2;

	// ˽Կֵ
	if (vkIndex == 99)
	{
		if (vkLen <= 0 || vkValue == NULL)
		{
			UnionUserErrLog("in UnionHsmCmd8B:: vkLen[%d] <= 0 or vkValue == NULL parameter error!\n",vkLen);
			return (errCodeParameter);
		}

		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue,vkLen,hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}

	// ��Կ���ܵ����ݳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);	
	hsmCmdLen += 4;

	// ��Կ���ܵ���������
	aschex_to_bcdhex(pinByPK,lenOfPinByPK,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// ���ط�����
	memcpy(hsmCmdBuf+hsmCmdLen,factorData ,8);
	hsmCmdLen += 8;

	// ��Կֵ
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"S%s",keyValue);
	hsmCmdLen += 33;

	// PIN�����������
	if (mode == 2)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, randomData, 32);
		hsmCmdLen += 32;
	}

	// �˺�
	if (mode == 3)
	{
		if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmd8B:: UnionForm12LenAccountNumber for [%s]\n",accNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8B::UnionDirectHsmCmd!ret = [%d]\n", ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	offset = 4;
	if (mode == 1)
	{
		memcpy(pinByZEK, hsmCmdBuf+offset, 32);
		offset += 32;
		pinByZEK[32] = 0;
	}

	if (mode == 2 || mode == 1)
	{
		memcpy(digest, hsmCmdBuf+offset, 64);
		digest[64] = 0;
		return(64);
	}

	if (mode == 3)
	{
		memcpy(pinBlock, hsmCmdBuf+offset, 32);
		pinBlock[32] = 0;
	}

	return(32);
}
// add end 20151228

// add by zhouxw 20160114
/*
   ZMK���ζ��������ݽ���n����ɢ������У�鴦��󣬲���ָ������Կ�������
   ���������
   zmkKey		16H/1A+32H/1A+48H	LMK���ܵ�ZMK
   keyFlag		1N			��Կ��־λ
   0��ZMK
   1��EDK
   2��ZPK
   3��ZAK
   bankID		16			��ɢ����1
   PSAMID		16			��ɢ����2����ѡ��
   random		16			��ɢ����3����ѡ��
   ���������
   keyValue	32H			lmk���ܵ�����
 */
int UnionHsmCmd98(char *zmkKey, int keyFlag, char *bankID, char *PSAMID, char *random, char *keyValue)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	offset = 0;

	if ( zmkKey == NULL || bankID == NULL || keyValue == NULL )
	{
		UnionUserErrLog("in UnionHsmCmd98::parameter error!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "98", 2);
	hsmCmdLen += 2;

	// zmkKey
	if ((ret = UnionPutKeyIntoRacalKeyString(zmkKey,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd98:: UnionPutKeyIntoRacalKeyString [%s]!\n",zmkKey);
		return(ret);
	}
	hsmCmdLen += ret; 

	// keyFlag
	if(keyFlag != 0 && keyFlag != 1 && keyFlag != 2 && keyFlag != 3)
	{
		UnionUserErrLog("in UnionHsmCmd98:: parameter error!\n");
		return(errCodeParameter);
	}
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", keyFlag);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, bankID, 16);
	hsmCmdLen += 16;

	// PSAMID
	if(PSAMID != NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, PSAMID, 16);
		hsmCmdLen += 16;
	}

	// random
	if(random != NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, random, 16);
		hsmCmdLen += 16;
	}

	//UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd98::UnionDirectHsmCmd!ret = [%d]\n", ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	offset = 4;
	memcpy(keyValue, hsmCmdBuf+offset, 32);
	keyValue[32] = 0;

	return(32);
}
// add end 20160114


//add begin by lusj 20160125
/*
   ƽ������	PHָ��

   �������ܣ�
   ��SM2��Կ���ܻ�������ת��Ϊר���㷨����

   ���������
   ID			�㷨ID		1A			   1-  ��SM2��Կ���ܵ�����תΪ��ҵ������¼����ר���㷨����
   								   2-  ��SM2��Կ���ܵ�����תΪ����������¼����ר���㷨����
   								   3-  ��SM2��Կ���ܵ�����תΪMD5�㷨����
   								   4-  ��SM2��Կ���ܵ�����תΪZPK����
   vkIndex		˽Կ����	 	2N			   00-20��99��ʾSM2���
   lenOfVK		˽Կ����		4N			   ������Կ����Ϊ99ʱ�д�����һ���򳤶�
   vk			˽Կ����		nB			   ������Կ����Ϊ99ʱ�д���SM2��Կ����
   lenOfPinByPK		���ĳ���		4N			
   PinByPK		����			nB			   SM2��Կ���ܵ����ġ���C1+C3+C2��
   ZPKValue		zpk			1A+16 H /32 H /48 H		��ID=1��4ʱ�и���DES�㷨ZPK
   lenOfaccNO		�ʺų��Ȼ��߸�����Ϣ����	2N			��ID=1ʱ�и���4~20
   accNO		�ʺţ�PAN�����߸�����Ϣ		N			��ID=1ʱ�и����û����ʺŻ��߸�����Ϣ����Ϊȫ"0"������򲻲������PIN������
   										��ID=4ʱ�и����û���Ч�˺�12λ

   ���������
   referFactor	�ط�����	
   PinBlock		pin����					��ID=1ʱ����ҵ������¼����ר���㷨�������� 32H
								  ID=2ʱ������������¼����ר���㷨�������� 28B
  				 			        ��ID=3ʱ��MD5�㷨�������� 				 32H
							        ��ID=4ʱ��ZPK���ܵ����ģ�ZPK���ܣ�DES�㷨�������ģ�ANSI 9.8��ʽ 16H
 */

int UnionHsmCmdPH(int ID,int vkIndex,int lenOfVK,char *vk,int lenOfPinByPK,char *PinByPK,char *ZPKValue,int lenOfaccNO,char *accNO,char *referFactor,char *PinBlock)
{

	int 		ret;
	int     	hsmCmdLen = 0;
	int			lenOfreferFactor;
	int			lenOfPin;


	char		hsmCmdBuf[8192];
	char		tmpBuf[8192];

	if (PinByPK == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdPH:: wrong parameters!\n");
		return(errCodeParameter);
	}


	//���ܻ�ָ��
	memcpy(hsmCmdBuf,"PH",2);
	hsmCmdLen = 2;

	//�㷨ID
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",ID);
	hsmCmdLen += 1;


	//˽Կ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;	

	if(vkIndex==99)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmdPH:: vk is null!\n");
			return(errCodeParameter);
		}

		//˽Կ����
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//˽Կ
		aschex_to_bcdhex(vk,lenOfVK,tmpBuf);
		tmpBuf[lenOfVK/2]=0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;	
	}

	//���ĳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;		

	//����
	aschex_to_bcdhex(PinByPK,lenOfPinByPK,tmpBuf);	
	tmpBuf[lenOfPinByPK/2]=0;
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfPinByPK/2);
	hsmCmdLen += lenOfPinByPK/2;


	//zpk ��ID=1��4ʱ�и���DES�㷨ZPK
	if((ID==1)||(ID==4))
	{
		if(strlen(ZPKValue) == 16)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,"Z",1);
                        hsmCmdLen +=1;	

			memcpy(hsmCmdBuf+hsmCmdLen,ZPKValue,16);
			hsmCmdLen +=16;		
		}
		else if(strlen(ZPKValue) == 32)
		{

			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen +=1;	

			memcpy(hsmCmdBuf+hsmCmdLen,ZPKValue,strlen(ZPKValue));
			hsmCmdLen +=strlen(ZPKValue); 	
		}
		else if(strlen(ZPKValue) == 48)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen +=1;	

			memcpy(hsmCmdBuf+hsmCmdLen,ZPKValue,strlen(ZPKValue));
			hsmCmdLen +=strlen(ZPKValue); 
		}	
		else
		{
			UnionUserErrLog("in UnionHsmCmdPH:: ZPKValue is err!\n");
			return(errCodeParameter);	
		}
	}

	//�ʺų��Ȼ��߸�����Ϣ����
	if(ID==1)
	{
			sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfaccNO);
			hsmCmdLen += 2;

			memcpy(hsmCmdBuf+hsmCmdLen,accNO,lenOfaccNO);
			hsmCmdLen += lenOfaccNO;

	}
	else if(ID==4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,accNO,12);
		hsmCmdLen += 12;
	}

	hsmCmdBuf[hsmCmdLen] = 0;


	bcdhex_to_aschex(hsmCmdBuf,hsmCmdLen,tmpBuf);	
	tmpBuf[2*hsmCmdLen]=0;
	UnionLog("in UnionHsmCmdPH:: hsmCmdBuf[%s]!\n",tmpBuf);

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPH:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	memcpy(tmpBuf,hsmCmdBuf+4,2);
	tmpBuf[2] = 0;
	lenOfreferFactor = 2*atoi(tmpBuf);
	memcpy(referFactor,hsmCmdBuf+4+2,lenOfreferFactor);
	referFactor[lenOfreferFactor]=0;

	lenOfPin = ret-4-2-lenOfreferFactor;

	memcpy(PinBlock,hsmCmdBuf+4+2+lenOfreferFactor,lenOfPin);
	PinBlock[lenOfPin]=0;
	return(lenOfPin);

}
//add end by lusj 20160125

// add by zhouxw 20160223
/*
   ��ZPK���ܵ�pinBlockת����MD5��SHA-1��SM3
   ���������
   algorithmID		1N			PIN���㷨
						1�� DES/3DES
						2�� SM4
   keyType		3H			��Կ����
						001�� ZPK
						002�� TPK
   keyValue		1A+3H/16H/32H/48H	��Կֵ
   checkValue		16H			У��ֵ����ΪSM4�㷨ʱ�и���
   format		2N			PIN���ʽ
   pinBlock		16H/32H			��Կ���ܵ�PIN��
   accNo		N			�˺�
   hashID		2N			��ϣ�㷨��ʶ
   dataOffset		4N			����ƫ��
   length		4N			���γ���
   data			nH			��������
   ���������
   hash			nH			��ϣ
 */
int UnionHsmCmdZW(int algorithmID, char *keyType, char *keyValue, char *checkValue, char *format, char *pinBlock, char *accNo, int lenOfAccNo, char *hashID, int dataOffset, int length, char *data, char *hash, int sizeOfHash)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	offset = 0;
	char	hashLen[16];

	if ( (algorithmID != 1 && algorithmID != 2) || keyType == NULL || keyValue == NULL || format == NULL || pinBlock == NULL || accNo == NULL || hashID == NULL || data == NULL || hash == NULL )
	{
		UnionUserErrLog("in UnionHsmCmdZW::parameter error!\n");
		return (errCodeParameter);
	}

	// �������
	memcpy(hsmCmdBuf, "ZW", 2);
	hsmCmdLen += 2;
	
	// algorithmID
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", algorithmID);
	hsmCmdLen += 1;
	
	// keyType
	if( (strncmp(keyType, "001", 3) != 0) && (strncmp(keyType, "002", 3) != 0))
	{
		UnionUserErrLog("in UnionHsmCmdZW:: keyType error!\n");
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// keyValue
	if(strlen(keyValue) == 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, keyValue, 4);
		hsmCmdLen += 4;
	}
	else if(algorithmID == 1)
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(keyValue,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdZW:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyValue);
			return(ret);
		}
		else
		{
			hsmCmdLen += ret; 
		}
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, keyValue, 32);
		hsmCmdLen += 32;
	}

	if(algorithmID == 2)
	{
		// checkValue
		memcpy(hsmCmdBuf+hsmCmdLen, checkValue, 16);
		hsmCmdLen += 16;
	}

	// format
	if(algorithmID == 2)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
		hsmCmdLen += 2;
		memcpy(hsmCmdBuf+hsmCmdLen, pinBlock, 32);
		hsmCmdLen += 32;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, format, 2);
		hsmCmdLen += 2;
		memcpy(hsmCmdBuf+hsmCmdLen, pinBlock, 16);
		hsmCmdLen += 16;
	}
	
	// accNo
	if(algorithmID == 1 && strncmp(format, "04", 2))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, accNo, 18);
		hsmCmdLen += 18;
	}
	else if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdCC:: UnionForm12LenAccountNumber for [%s]\n",accNo);
                return(ret);
        }
	hsmCmdLen += ret;      
	
	// hashID
	memcpy(hsmCmdBuf+hsmCmdLen, hashID, 2);
	hsmCmdLen += 2;
	
	// dataOffset
	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", dataOffset);
	hsmCmdLen += 4;
	
	// length
	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", length/2);
	hsmCmdLen += 4;
	
	// data
	memcpy(hsmCmdBuf+hsmCmdLen, data, length);
	hsmCmdLen += length;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdZW::UnionDirectHsmCmd!ret = [%d]\n", ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	offset = 4;
	memcpy(hashLen, hsmCmdBuf+offset, 2);
	hashLen[2] = 0;
	ret = atoi(hashLen) * 2;
	if(ret > sizeOfHash)
	{
		UnionUserErrLog("in UnionHsmCmdZW:: hashLen[%d] >= sizeOfHash[%d]\n", ret, sizeOfHash);
                return(errCodeSmallBuffer);
	}
	memcpy(hash, hsmCmdBuf+offset+2, ret);
	hash[ret] = 0;

	return(ret);
}
// add end 20160223

//add begin by zhouxw 20160418 for HaiXiaBank
int UnionHsmCmdW1(char *algorithmID, char *vkIndex, TUnionDesKeyType dstKeyType, char *keyValue, int dataFillMode, int cipherDataLen, char *cipherData, int lenOfUserName, char *userName, char *pinBlock, int sizeOfPinBlock)
{       
        int     ret = 0;
        char    hsmCmdBuf[8192];
        int     hsmCmdLen = 0;
	int	len = 0;
	char	keyType[16];
                                                                                                        
        if ( vkIndex == NULL || keyValue == NULL || cipherData == NULL || userName == NULL || pinBlock == NULL )
        {                                                                                               
                UnionUserErrLog("in UnionHsmCmdW1::parameter error!\n");                                
                return (errCodeParameter);                                                              
        }                                                                                               
                                                                                                        
        // �������                                                                                     
        memcpy(hsmCmdBuf, "W1", 2);                                                                     
        hsmCmdLen += 2;                                                                                 
                                                                                                        
        // algorithmID
	if(algorithmID == NULL || strlen(algorithmID) == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "R", 1);
		hsmCmdLen += 1;
	}
	else if(memcmp(algorithmID, "R", 1) != 0 && memcmp(algorithmID, "S", 1) != 0)
	{
		UnionUserErrLog("in UnionHsmCmdW1:: parameter error!\n");
		return(errCodeParameter);
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 1);                                                
		hsmCmdLen += 1;                                                                                 
        }
		
	//vkIndex
	memcpy(hsmCmdBuf+hsmCmdLen, vkIndex, 2);
	hsmCmdLen += 2;
		
        // keyType
	UnionTranslateDesKeyTypeTo3CharFormat(dstKeyType, keyType);
        if( (memcmp(keyType, "007", 3) != 0) && (memcmp(keyType, "00A", 3) != 0))                     
        {                                                                                               
                UnionUserErrLog("in UnionHsmCmdW1:: keyType error!\n");                                 
                return(errCodeParameter);                                                               
        }                                                                                               
        memcpy(hsmCmdBuf+hsmCmdLen, keyType, 3);                                                        
        hsmCmdLen += 3;                                                                                 
                                                                                                        
        // keyValue
	if ((ret = UnionPutKeyIntoRacalKeyString(keyValue,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
        {                                                                                       
                UnionUserErrLog("in UnionHsmCmdW1:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyValue);
                return(ret);
        }                                                                                       
        else                                                                                    
        {                                                                                       
                hsmCmdLen += ret;                                                               
        }
        // dataFillMode
	if(memcmp(algorithmID, "R", 1) == 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", dataFillMode);
		hsmCmdLen += 1;
	}
                                                                                                        
        // cipherDataLen
	if(cipherDataLen != strlen(cipherData))
	{
		UnionUserErrLog("in UnionHsmCmdW1::parameter error!\n");                                
                return (errCodeParameter);
	}
	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", cipherDataLen / 2);
	hsmCmdLen += 4;
		
	//cipherData
	len = aschex_to_bcdhex(cipherData, cipherDataLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += len;
        
	if(lenOfUserName != strlen(userName))
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", dataFillMode);
		hsmCmdLen += 1;
	}
        // userName                                                                                        
        memcpy(hsmCmdBuf+hsmCmdLen, userName, lenOfUserName);
	hsmCmdLen += lenOfUserName;                                                                                                        
       
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in RacalCmdW1:: UnionDirectHsmCmd!\n");
                return ret;
        }
	hsmCmdBuf[ret] = 0;
	if(sizeOfPinBlock <= 32)
	{
		UnionUserErrLog("in UnionHsmCmdW1::parameter error!\n");                                
                return (errCodeParameter);
	}
	else
	{
		memcpy(pinBlock, hsmCmdBuf + 4, 32);
		pinBlock[32] = 0;
		return(32);
	}
}

int UnionHsmCmdW3(char *srcZPK, TUnionDesKeyType dstKeyType, char *dstKeyValue, int maxPinLen, char *srcPinBlock, char *format, char *accNo, int lenOfUserName, char *userName, char *dstPinBlock)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;
	int	len = 0;
	char	keyType[16];
	
	if( srcZPK == NULL || dstKeyValue == NULL || srcPinBlock == NULL || accNo == NULL || userName == NULL || dstPinBlock == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdW3:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memcpy(hsmCmdBuf, "W3", 2);
	hsmCmdLen += 2;
	
	// Դzpk
	if((ret = UnionPutKeyIntoRacalKeyString(srcZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW3:: UnionPutKeyIntoRacalKeyString [%s]!\n", srcZPK);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// Ŀ����Կ����
	UnionTranslateDesKeyTypeTo3CharFormat(dstKeyType, keyType);
	memcpy(hsmCmdBuf+hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// Ŀ����Կ
	if((ret = UnionPutKeyIntoRacalKeyString(dstKeyValue, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW3:: UnionPutKeyIntoRacalKeyString [%s]!\n", dstKeyValue);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// ���PIN����
	len = sprintf(hsmCmdBuf + hsmCmdLen, "%d", maxPinLen);
	hsmCmdLen += len;
	
	// ԴPIN��
	memcpy(hsmCmdBuf + hsmCmdLen, srcPinBlock, 16);
	hsmCmdLen += 16;
	
	// ԴPIN��ʽ
	memcpy(hsmCmdBuf + hsmCmdLen, format, 2);
	hsmCmdLen += 2;
	
	// �˺�
	if((ret = UnionForm12LenAccountNumber(accNo, strlen(accNo), hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW3:: UnionForm12LenAccountNumber for [%s]!\n", accNo);
		return(ret);
	}
	hsmCmdLen += 12;
	
	// �����û���
	memcpy(hsmCmdBuf + hsmCmdLen, userName, lenOfUserName);
	hsmCmdLen += lenOfUserName;
	hsmCmdBuf[hsmCmdLen] = 0;
	
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdW3:: UnionDirectHsmCmd!\n");
                return ret;
        }
	hsmCmdBuf[ret] = 0;
	
	memcpy(dstPinBlock, hsmCmdBuf + 4, 32);
	dstPinBlock[32] = 0;
	return(32);
	
}

int UnionHsmCmdH6(char *srcZPK, int lenOfPin, char *pin, int lenOfPan, char *pan, int lenOfPinBlock, char *pinBlock)
{
	int		ret = 0;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	char		lenBuf[16];
	
	if(srcZPK == NULL || pin == NULL || pan == NULL || pinBlock == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdH6:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memcpy(hsmCmdBuf, "H6", 2);
	hsmCmdLen += 2;
	
	// Դzpk
	if((ret = UnionPutKeyIntoRacalKeyString(srcZPK, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH6:: UnionPutKeyIntoRacalKeyString [%s]!\n", srcZPK);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// PIN���ģ��Ѿ���չ�����Ϊ48λ��pin��
	if(lenOfPin != 48)
	{
		UnionUserErrLog("in UnionHsmCmdH6:: lenOfPin[%d] error!\n", lenOfPin);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, pin, lenOfPin);
	hsmCmdLen += lenOfPin;
	
	// �˺�
	if((ret = UnionForm12LenAccountNumber(pan, lenOfPan, hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH6:: UnionForm12LenAccountNumber [%s][%d]!\n", pan, lenOfPan);
		return(ret);
	}
	hsmCmdLen += ret;
	
	UnionSetMaskPrintTypeForHSMReqCmd();
	
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in RacalCmdH6:: UnionDirectHsmCmd!\n");
                return ret;
        }
	hsmCmdBuf[ret] = 0;
	memcpy(lenBuf, hsmCmdBuf+4, 2);
	lenBuf[2] = 0;
	lenOfPinBlock = atoi(lenBuf);
	
	memcpy(pinBlock, hsmCmdBuf + 6, lenOfPinBlock*2);
	pinBlock[lenOfPinBlock*2] = 0;
	
	return(lenOfPinBlock * 2);
}

int UnionHsmCmdY8(char *srcZPK, int lenOfPin, char *pin, int lenOfPan, char *pan, int lenOfPlainPin, char *plainPin)
{
	int		ret = 0;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	char		lenBuf[16];
	
	if(srcZPK == NULL || pin == NULL || pan == NULL || plainPin == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdY8:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memcpy(hsmCmdBuf, "Y8", 2);
	hsmCmdLen += 2;
	
	// ԴZPK
	if((ret = UnionPutKeyIntoRacalKeyString(srcZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY8:: UnionPutKeyIntoRacalKeyString [%s]!\n", srcZPK);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// �Ѿ���չ�����Ϊ48λ��pin��
	if(lenOfPin != 48)
	{
		UnionUserErrLog("in UnionHsmCmdY8:: lenOfPin[%d] error!\n", lenOfPin);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, pin, lenOfPin);
	hsmCmdLen += lenOfPin;
	
	// �˺�
	if((ret = UnionForm12LenAccountNumber(pan, lenOfPan, hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY8:: UnionForm12LenAccountNumber [%s][%d]!\n", pan, lenOfPan);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	UnionSetMaskPrintTypeForHSMResCmd();
	
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in RacalCmdY8:: UnionDirectHsmCmd!\n");
                return ret;
        }
	hsmCmdBuf[ret] = 0;
	
	memcpy(lenBuf, hsmCmdBuf+4, 2);
	lenBuf[2] = 0;
	lenOfPlainPin = atoi(lenBuf);
	
	memcpy(plainPin, hsmCmdBuf+6, lenOfPlainPin * 2);
	plainPin[lenOfPlainPin * 2] = 0;
	
	return(lenOfPlainPin * 2);
}
//add end by zhouxw 20160418

/*
	���ܣ�������SM2��Կ���ܵ����ݣ���SM2˽Կ���ܺ�����MAC
	ָ�4I
	���������
		vkIndex		2N	SM2��Կ����,00 - 20, 99Ϊ�����Կ
		lenOfVkValue	4N	������Կ����Ϊ99���д���,˽Կ����
		vkValue		nB	������Կ����Ϊ99���д���,˽Կ
		lenOfPinByPK	4N	��Կ���ܵ����ĳ���
		pinByPK		nB	��Կ���ܵ�����
		lenOfRandom	2N	0-20,���������
		random		nA	�����
		zak�㷨��ʶ	1N	0:����SM4�㷨, 1:����DES/3DES�㷨
		zak		16H/1A+32H/1A+48H/	��MAC���ݲ���MAC����Կ
	�����������
		mac		16H/32H	����ԿΪ�����㷨ʱ�����ص�MAC-ANSI 9.9,�1�7,16λ����
					����ԿΪ�����㷨ʱ�����ص�MAC-ANSI 9.9,32λ����
*/
int UnionHsmCmd4I(int vkIndex,int lenOfVkValue,char *vkValue,int lenOfPinByPK,char *pinByPK,int lenOfRandom,char *random,int algorithmID,char *zakValue,char *mac)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if ((lenOfPinByPK == 0) || (lenOfRandom < 0) || (lenOfRandom > 20) || (pinByPK == NULL) || (zakValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd4I:: lenOfPinByPK == 0 or lenOfRandom < 0 or lenOfRandom > 20 or pinByPK == NULL or zakValue == NULL!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf,"4I",2);
	hsmCmdLen = 2;

	// ˽Կ����
	if (vkIndex < 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"99",2);
		hsmCmdLen += 2;

		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfVkValue/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, lenOfVkValue, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += lenOfVkValue/2;
	}
	else
	{
		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%02d",vkIndex);
		hsmCmdLen += 2;
	}

	// ���ĳ���
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;

	// ����
	aschex_to_bcdhex(pinByPK, lenOfPinByPK, hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// ���������
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%02d",lenOfRandom);
	hsmCmdLen += 2;

	// �����
	memcpy(hsmCmdBuf+hsmCmdLen,random,lenOfRandom);
	hsmCmdLen += lenOfRandom;

	// zak�㷨��ʶ
	snprintf(hsmCmdBuf + hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;

	// zak��Կֵ
	if (algorithmID == 1)
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(zakValue),zakValue,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmd4I:: UnionGenerateX917RacalKeyString [%s]\n",zakValue);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zakValue,32);
		hsmCmdLen += 32;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4I:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (algorithmID == 1)
	{
		memcpy(mac,hsmCmdBuf+4,16);
		mac[16] = 0;
		return(16);
	}
	else
	{
		memcpy(mac,hsmCmdBuf+4,32);
		mac[32] = 0;
		return(32);
	}
}

/*
	���ܣ�������SM2��Կ���ܵ�PINת��ANSI9.8��׼
	ָ�4J
	���������
		vkIndex		2N	SM2��Կ����,00 - 20, 99Ϊ�����Կ
		lenOfVkValue	4N	������Կ����Ϊ99���д���,˽Կ����
		vkValue		nB	������Կ����Ϊ99���д���,˽Կ
		lenOfPinByPK	4N	��Կ���ܵ����ĳ���
		pinByPK		nB	��Կ���ܵ�����
		lenOfRandom	2N	0-20,���������
		random		nA	�����
		zpk�㷨��ʶ	1N	0:����SM4�㷨, 
					1:����DES/3DES�㷨
		zpk		16H/1A+32H/1A+48H/32H	��MAC���ݲ���MAC����Կ,��Ϊ����ʱ������Ϊ32H
	�����������
		pinByZPK	16H/32H	����ԿΪ�����㷨ʱ�����ص�PIN����ΪANSI 9.8,�1�7,16λ����
					����ԿΪ�����㷨ʱ�����ص�PIN����ΪANSI 9.8,32λ����
*/
int UnionHsmCmd4J(int vkIndex,int lenOfVkValue,char *vkValue,int lenOfPinByPK,char *pinByPK,int lenOfRandom,char *random,int algorithmID,char *zpkValue,char *pan,char *pinByZPK)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	if ((lenOfPinByPK == 0) || (lenOfRandom < 0) || (lenOfRandom > 20) || (pinByPK == NULL) || (zpkValue == NULL))
	{
		UnionUserErrLog("in UnionHsmCmd4J:: lenOfPinByPK == 0 or lenOfRandom < 0 or lenOfRandom > 20 or pinByPK == NULL or zpkValue == NULL!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf,"4J",2);
	hsmCmdLen = 2;

	// ˽Կ����
	if (vkIndex < 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"99",2);
		hsmCmdLen += 2;

		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfVkValue/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, lenOfVkValue, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += lenOfVkValue/2;
	}
	else
	{
		snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%02d",vkIndex);
		hsmCmdLen += 2;
	}

	// ���ĳ���
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;

	// ����
	aschex_to_bcdhex(pinByPK, lenOfPinByPK, hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// ���������
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%02d",lenOfRandom);
	hsmCmdLen += 2;

	// �����
	memcpy(hsmCmdBuf+hsmCmdLen,random,lenOfRandom);
	hsmCmdLen += lenOfRandom;

	// zpk�㷨��ʶ
	snprintf(hsmCmdBuf + hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;

	// zpk��Կֵ
	if (algorithmID == 1)
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(zpkValue),zpkValue,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmd4J:: UnionGenerateX917RacalKeyString [%s]\n",zpkValue);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zpkValue,32);
		hsmCmdLen += 32;
	}

	// �û����˺�
	if ((ret = UnionForm16LenAccountNumber(pan,strlen(pan),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4J:: UnionForm16LenAccountNumber [%s]\n",pan);
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd4J:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (algorithmID == 1)
	{
		memcpy(pinByZPK,hsmCmdBuf+4,16);
		pinByZPK[16] = 0;
		return(16);
	}
	else
	{
		memcpy(pinByZPK,hsmCmdBuf+4,32);
		pinByZPK[32] = 0;
		return(32);
	}
}

// add by zhouxw 20160628 begin
/*
   ����:����Կ���ܵ�����ת��ΪMD5�����
   ���������
   keyIndex        RSA��Կ����
   vkLen           RSA�����Կ����
   vkValue         RSA�����Կ
   lenOfPinByPK    SM2��Կ���ܵ����ĳ���
   pinByPK         ����
   dataFillMode    �ù�Կ����ʱ�����õ���䷽ʽ
   DataFillHeadLen  ǰ������ݳ���
   DataFillHead     ǰ�������
   DataFillTailLen  ��������ݳ���
   DataFillTail     ���������
   hash             MD5�����Ľ��
*/

int UnionHsmCmdS9(char *keyIndex,int vkLen ,char *vkValue,int lenOfPinByPK,char *pinByPK,int dataFillMode,int DataFillHeadLen,char *DataFillHead,int DataFillTailLen,char *DataFillTail,char *hash, int sizeofHash)
{
	int     ret;
    char    hsmCmdBuf[1024];
	int		hsmCmdLen = 0;
	
	if( (NULL == keyIndex) || (NULL == pinByPK) )
	{
			UnionUserErrLog("in UnionHsmCmdS9:: parameters err\n");
			return(errCodeParameter);
	}
	
	 //������
	memcpy(hsmCmdBuf,"S9",2);
	hsmCmdLen = 2;
	//SM2��Կ����
	memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
	hsmCmdLen += 2;
	
	if(!strncmp(keyIndex, "99", 2))
	{
		//SM2�����Կ����
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}
	//�ù�Կ����ʱ�����õ���䷽ʽ
	if(dataFillMode != 0 && dataFillMode != 1)
	{
		UnionUserErrLog("in UnionHsmCmdS9:: dataFillMode error\n");
		return(errCodeParameter);
	}
	else
	{
		sprintf(hsmCmdBuf + hsmCmdLen, "%d", dataFillMode);
		hsmCmdLen += 1;
	}
	
	//���ĳ���
	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
	hsmCmdLen += 4;
	aschex_to_bcdhex(pinByPK,lenOfPinByPK , hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;
	
	//ǰ�������
	if(DataFillHeadLen)
	{
		//ǰ���ָ���
		memcpy(hsmCmdBuf + hsmCmdLen, ";", 1);
        hsmCmdLen += 1;
		
		//ǰ������ݳ���
		sprintf(hsmCmdBuf + hsmCmdLen, "%02d", DataFillHeadLen);
		hsmCmdLen += 2;
		
		//ǰ�������
		memcpy(hsmCmdBuf + hsmCmdLen, DataFillHead, DataFillHeadLen);
		hsmCmdLen += DataFillHeadLen;
	}
	//���������
	if(DataFillHeadLen)
	{
		//�����ָ���
		memcpy(hsmCmdBuf + hsmCmdLen, "M", 1);
        hsmCmdLen += 1;
		//��������ݳ���
		sprintf(hsmCmdBuf + hsmCmdLen, "%02d", DataFillTailLen);
		hsmCmdLen += 2;	
		//���������
		memcpy(hsmCmdBuf + hsmCmdLen, DataFillHead, DataFillTailLen);
		hsmCmdLen += DataFillTailLen;
	}
	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();
	
	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
			UnionUserErrLog("in UnionHsmCmdS9:: UnionDirectHsmCmd\n");
			return(ret);
	}
	if(sizeofHash < ret - 4)
	{
		UnionUserErrLog("in UnionHsmCmdS9:: buffer too small\n");
		return(errCodeParameter);
	}
	memcpy(hash,hsmCmdBuf+4,ret - 4);
	hash[ret - 4] = 0;
	return(ret - 4);
}
// add by zhoxuw 20160628 end

