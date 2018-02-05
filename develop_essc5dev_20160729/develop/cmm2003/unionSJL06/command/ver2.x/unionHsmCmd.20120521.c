//	Author:		张永定
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

// 使用特殊的ZMK类型
int gunionIsUseSpecZmkType = 1;


// 设置使用正常的ZMK类型
int UnionSetIsUseNormalZmkType()
{
	gunionIsUseSpecZmkType = 0;
	return(0);
}

// add by leipp 20150419
// PVKI值
int gunionUseSpecPVKI = 0;

// 设置PVKI
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
		case conEDK://20120827加入
			sprintf(keyType, "%s", "007");
			break;
		case conBDK:
			sprintf(keyType, "%s", "009");
			break;
		case conMKSMC://20131106
			sprintf(keyType, "%s", "309");
			break;
		case conMKAC:	//20140416 张永定 add
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

// 生成一对RSA密钥
/* 输入参数
   type，类型	0，只用于签名
   1，只用于密钥管理
   2，签名和管理
   length，模数	0320/512/1024/2048
   pkEncoding	公钥的编码方式
   lenOfPKExponent	公钥exponent的长度	可选参数
   pkExponent	公钥的pkExponent	可选参数
   exportNullPK	1，输出裸PK，0，编码方式的PK
   sizeOfPK	接收公钥的缓冲大小
   sizeOfVK	接收私钥的缓冲大小
   输出参数
   pk		公钥
   lenOfVK		私钥串的长度
   vk		私钥
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

// 存储私钥
/* 输入参数
   vkIndex		私钥的索引号
   lenOfVK		私钥长度
   vk		私钥
   输出参数
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

// 生成公钥的MAC值
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

// 验证公钥的MAC值
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

// 将一个LMK加密的密钥转换为公钥加密
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
   十一、	产生摘要

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

// 生成签名的指令
int UnionHsmCmdEW(char *hashID,char *signID,char *padID,int lenOfData,char *data,
		int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign)
{
	int		ret;
	char		hsmCmdBuf[8192*2];
	int		offset = 0;
	int		lenOfSign;

	memcpy(hsmCmdBuf,"EW",2);
	offset = 2;
	// 摘要算法标识
	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;
	// 签名算法标识		
	if ((signID == NULL) || (strlen(signID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,signID,2);
	offset += 2;
	// 补位标志
	if ((padID == NULL) || (strlen(padID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,padID,2);
	offset += 2;
	// 数据长度
	sprintf(hsmCmdBuf+offset,"%04d",lenOfData);
	offset += 4;
	// 数据
	if (lenOfData + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEW:: lenOfData [%04d] too long!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,data,lenOfData);
	offset += lenOfData;
	// 分隔符
	hsmCmdBuf[offset] = ';';
	offset++;
	// 私钥索引
	if (vkIndex < 0)
		vkIndex = 99;
	sprintf(hsmCmdBuf+offset,"%02d",vkIndex);
	offset += 2;
	if (vkIndex == 99)
	{
		// 私钥长度
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

// 验证签名的指令
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
	// 摘要算法标识
	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;
	// 签名算法标识		
	if ((signID == NULL) || (strlen(signID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,signID,2);
	offset += 2;
	// 补位标志
	if ((padID == NULL) || (strlen(padID) == 0))
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,padID,2);
	offset += 2;
	// 签名长度
	sprintf(hsmCmdBuf+offset,"%04d",lenOfSign);
	offset += 4;
	// 签名
	if (lenOfSign + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEY:: lenOfSign [%04d] too long!\n",lenOfSign);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,sign,lenOfSign);
	offset += lenOfSign;
	// 分隔符
	hsmCmdBuf[offset] = ';';
	offset++;
	// 数据长度
	sprintf(hsmCmdBuf+offset,"%04d",lenOfData);
	offset += 4;
	// 数据
	if (lenOfData + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionHsmCmdEY:: lenOfData [%04d] too long!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,data,lenOfData);
	offset += lenOfData;
	// 分隔符
	hsmCmdBuf[offset] = ';';
	offset++;
	// MAC
	memcpy(hsmCmdBuf+offset,mac,4);
	offset += 4;
	// 公钥
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
	// 认证数据
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
   将一个公钥加密的密钥转换为LMK加密
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

	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;

	// 私钥索引
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	hsmCmdLen += 2;

	if (vkIndex == 99)
	{
		// 私钥长度
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

	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;

	// ZMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdGI:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// LMK加密的密钥密文长度标志
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

// 生成一对SM2密钥
/* 输入参数
   type，类型
   0,协商	
   1，签名
   2, 加密
   3, 签名、加密和协商
   length，长度 固定256
   sizeOfPK	接收公钥的缓冲大小
   sizeOfVK	接收私钥的缓冲大小
   输出参数
   pk		公钥
   lenOfVK		私钥串的长度
   vk		私钥
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

// 使用SM3算法进行HASH计算
/* 输入参数
   algorithmID	算法标识，3-SM3
   lenOfData	数据长度
   hashData	做Hash的数据
   sizeOfBuf	接收Hash结果的缓冲大小
   输出参数
   hashValue	Hash结果
 */
int UnionHsmCmdM7(char *algorithmID,int lenOfData,char *hashData,char *hashValue,int sizeOfBuf)
{
	int			ret;
	char			hsmCmdBuf[8096];
	int			hsmCmdLen = 0;

	// 指令代码
	memcpy(hsmCmdBuf,"M7",2);
	hsmCmdLen = 2;

	//模式标志
	memcpy(hsmCmdBuf + hsmCmdLen,"1",1);
	hsmCmdLen += 1;

	// 算法标识
	memcpy(hsmCmdBuf + hsmCmdLen,algorithmID,1);
	hsmCmdLen += 1;

	// 数据长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfData/2);
	hsmCmdLen += 4;	

	// 数据
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

// 生成签名的指令
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
		// 密钥索引
		memcpy(hsmCmdBuf + offset,"99",2);
		offset += 2;
		// 外带密钥长度
		sprintf(hsmCmdBuf + offset,"%04d",lenOfVK);
		offset += 4;
		// 外带密钥
		memcpy(hsmCmdBuf + offset,vk,lenOfVK);
		offset += lenOfVK;
	}
	else
	{
		// 密钥索引
		sprintf(hsmCmdBuf + offset,"%02d",vkIndex);
		offset += 2;
	}

	// HASH算法
	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;

	// 用户标识
	if (strcmp(hashID, "02") == 0)
	{
		sprintf(hsmCmdBuf + offset,"%04d",lenOfUsrID);
		offset += 4;
		memcpy(hsmCmdBuf + offset, usrID, lenOfUsrID);
		offset += lenOfUsrID;
	}

	// 数据长度
	sprintf(hsmCmdBuf+offset,"%04d",lenOfData/2);
	offset += 4;

	// 数据
	aschex_to_bcdhex(data,lenOfData,hsmCmdBuf + offset);
	offset += lenOfData/2;

	hsmCmdBuf[offset] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK3:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// 签名结果的R部分
	bcdhex_to_aschex(hsmCmdBuf + 4,32,sign);
	// 签名结果的S部分
	bcdhex_to_aschex(hsmCmdBuf + 4 + 32,32,sign+64);
	sign[128] = 0;
	//return(lenOfSign);
	return(128);
}

//生成验签的指令
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
		// 密钥索引
		memcpy(hsmCmdBuf + offset,"99",2);
		offset += 2;
		// 外带公钥
		aschex_to_bcdhex(rCaPK,lengRCaPK,hsmCmdBuf + offset);
		offset += lengRCaPK/2;
		//memcpy(hsmCmdBuf + offset,rCaPK,lengRCaPK);
		//offset += lengRCaPK;
	}
	else
	{
		// 密钥索引
		sprintf(hsmCmdBuf + offset,"%02d",vkIndex);
		offset += 2;
	}

	// 签名结果
	aschex_to_bcdhex(caCertDataSign,lenCaCertDataSign,hsmCmdBuf + offset);
	offset += lenCaCertDataSign/2;
	//memcpy(hsmCmdBuf + offset,caCertDataSign,lenCaCertDataSign);
	//offset += lenCaCertDataSign;

	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;

	// 用户标识
	if (strcmp(hashID, "02") == 0)
	{
		sprintf(hsmCmdBuf + offset,"%04d",lenOfUsrID);
		offset += 4;
		memcpy(hsmCmdBuf + offset, usrID, lenOfUsrID);
		offset += lenOfUsrID;
	}

	// 数据长度
	sprintf(hsmCmdBuf+offset,"%04d",lenHashVal/2);
	//sprintf(hsmCmdBuf+offset,"0032",4);
	//UnionLog("in UnionHsmCmdK4:: UnionDirectHsmCmd lenHashVal = [%d]!\n",lenHashVal/2);
	offset += 4;

	// 数据
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

//用SM2公钥做SCE加密
/*输入参数
  pkIndex			公钥索引
  lenOfPK			公钥明文长度
  PK			公钥明文
  lenOfData		数据长度
  data			数据
  输出参数
  lenOfCiphertext		密文长度
  ciphertext		密文
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
		//公钥索引
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		//公钥明文
		aschex_to_bcdhex(PK,lenOfPK,hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += lenOfPK/2;
	}
	else
	{
		//公钥索引
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",pkIndex);
		hsmCmdLen += 2;
	}
	//数据长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	//数据
	memcpy(hsmCmdBuf + hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMReqCmd();
	// end 

	//与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//密文长度
	// modify by zhangyd 20150305 
	if ((*lenOfCiphertext = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK5:: len[%d] error!\n",*lenOfCiphertext);
		return(errCodeParameter);
	}
	//密文
	memcpy(ciphertext,hsmCmdBuf+4+4,*lenOfCiphertext);
	ciphertext[*lenOfCiphertext] = 0;
	return(*lenOfCiphertext);
}

//用SM2公钥做SCE加密
/*输入参数
  pkIndex			公钥索引
  lenOfPK			公钥明文长度
  PK			公钥明文
  lenOfData		数据长度
  data			数据
  输出参数
  lenOfCiphertext		密文长度
  ciphertext		密文
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
		//公钥索引
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		//公钥明文
		aschex_to_bcdhex(PK,lenOfPK,hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += lenOfPK/2;
	}
	else
	{
		//公钥索引
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",pkIndex);
		hsmCmdLen += 2;
	}
	//数据长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	//数据
	memcpy(hsmCmdBuf + hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMReqCmd();
	// end 

	//与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKE:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//密文长度
	// modify by zhangyd 20150305 
	if ((*lenOfCiphertext = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKE:: len[%d] error!\n",*lenOfCiphertext);
		return(errCodeParameter);
	}
	//密文
	memcpy(ciphertext,hsmCmdBuf+4+4,*lenOfCiphertext);
	ciphertext[*lenOfCiphertext] = 0;
	return(*lenOfCiphertext);
}

//用SM2私钥做SM2解密
/*输入参数
  vkIndex			密钥索引
  lenOfVK			外带密钥长度
  VK			外带密钥
  lenOfCiphertext		密文长度
  ciphertext		密文
  输出参数
  lenOfData		数据长度
  data			数据
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
		//密钥索引
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// 外带密钥长度
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfVK);
		hsmCmdLen += 4;
		// 外带密钥
		memcpy(hsmCmdBuf + hsmCmdLen,VK,lenOfVK);
		hsmCmdLen += lenOfVK;
	}
	else
	{
		//密钥索引
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",vkIndex);
		hsmCmdLen += 2;
	}
	//密文长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfCiphertext);
	hsmCmdLen += 4;
	//密文
	memcpy(hsmCmdBuf + hsmCmdLen,ciphertext,lenOfCiphertext);
	//hsmCmdLen += *lenOfData;
	hsmCmdLen += lenOfCiphertext;
	hsmCmdBuf[hsmCmdLen] = 0;

	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMResCmd();
	// end

	//与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK6:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//数据长度
	if ((*lenOfData = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdK6:: UnionDirectHsmCmd!\n");
		return(errCodeParameter);
	}
	//数据
	memcpy(data,hsmCmdBuf+4+4,*lenOfData);
	return(*lenOfData);
}

//用SM2私钥做SM2解密
/*输入参数
  vkIndex			密钥索引
  lenOfVK			外带密钥长度
  VK			外带密钥
  lenOfCiphertext		密文长度
  ciphertext		密文
  输出参数
  lenOfData		数据长度
  data			数据
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
		//密钥索引
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// 外带密钥长度
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfVK);
		hsmCmdLen += 4;
		// 外带密钥
		memcpy(hsmCmdBuf + hsmCmdLen,VK,lenOfVK);
		hsmCmdLen += lenOfVK;
	}
	else
	{
		//密钥索引
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",vkIndex);
		hsmCmdLen += 2;
	}
	//密文长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfCiphertext);
	hsmCmdLen += 4;
	//密文
	memcpy(hsmCmdBuf + hsmCmdLen,ciphertext,lenOfCiphertext);
	//hsmCmdLen += *lenOfData;
	hsmCmdLen += lenOfCiphertext;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();
	// add by leipp 20150116
	UnionSetMaskPrintTypeForHSMResCmd();
	// end

	//与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKF:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//数据长度
	if ((*lenOfData = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdKF:: UnionDirectHsmCmd!\n");
		return(errCodeParameter);
	}
	//数据
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
   转换DES密钥：从主密钥加密到公钥加密
   用于分发密钥。
   输入参数
   pk 公钥
   keyByMK 密钥密文
   输出参数
   checkValue 检查值 
   sizeOfBuf 密钥长度
   keyByPK 密钥密文（用公钥加密的DES密钥）
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
   转换DES密钥：从公钥加密到主密钥加密
   用于接收密钥。
   输出参数
   vkIndex 私钥索引，"00"－"20"：用密码机内的私钥
   lenOfKeyByPK 密钥长度，DES密钥密文的字节数
   keyByPK 用公钥加密的DES密钥
   输出参数
   keyByMK 用主密钥加密的DES密钥
   checkValue 检查值
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


//add 20130719,34指令函数，同时输出公钥私钥
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
	//modify by hzh in 2012.01.13, 解决加密机升级后，公钥取出偏移的问题. (如30818902818100..., 左边多了00,右边末尾字节丢失)
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
   功能：将由公钥加密的PIN转换成ANSI9.8标准
   输入参数
   vkIndex 私钥索引，指定的私钥，用于解密PIN数据密文
   type 密钥类型，1：TPK  2：ZPK
   zpk 用于加密PIN的密钥
   pinType PIN类型，1：ANSI9.8
   pan 用户主帐号
   pinByPK 经公钥加密的PIN数据密文
   lenOfPinByPK 密文长度
   输出参数
   pinByZPK 返回的PIN密文--ANSI9.8

修改：20160125 lusj
修改内容：平安银行43指令扩展，支持外带私钥，涉及的服务有E201	

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
		
	//add begin  by lusj 20160125 平安银行43指令修改后支持外带私钥
	if(atoi(vkIndex)==99)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmd43:: vk is null!\n");
			return(errCodeParameter);
		}

		//私钥长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//私钥
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
   功能：将由公钥加密的PIN转换成旧网银加密算法加密
   输入参数
   vkIndex 私钥索引，指定的私钥，用于解密PIN数据密文
   pinByPK 经公钥加密的PIN数据密文
   lenOfPinByPK 密文长度
   输出参数
   hash 返回的PIN密文--ANSI9.8
 */
// modify by zhouxw 20160415 允许外带私钥
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
   功能：用EDK密钥加解密数据，如果是解密状态，则必须在授权下才能处理，否则报错。
   输入参数
   flag 0：加密
   edk LMK24-25加密
   lenOfData 输入数据字节数（8的倍数）范围：0008-4096
   indata 待加密或解密的数据（以BCD码表示）
   输出参数
   outdata 输出结果
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
   功能：用RC4算法进行数据加/解密。由于RC4算法的特性，加密和解密都是调用同一指令。
   参数：
   desKeyType	密钥类型，支持edk和zek
   inData		输入数据，binary
   inLen		输入数据长度
   outData		输出数据
   sizeOfOutData	outData指向的缓冲区的长度
   返回：
   <0 出错
   >0 返回数据长度
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


// 将ZMK加密的密钥转换为LMK加密的ZAK/ZEK
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
   函数功能:
   TI指令，公钥解密
   输入参数:
encryMode: 1H 算法标识 1－解密数据 2－加密数据 3－验证签名 4－加密密钥 5－解密密钥
pkLength: 送入公钥长度
pk: nB/1A+3H DER编码的公钥或公钥在HSM安全存储区内的位置
signLength: 待验证签名值长度，encryMode = 3时该域存在
sign: nB 待验证签名值，encryMode = 3时该域存在
keyLength: 1H 待加/解密密钥长度，0 = 单长度密钥，1 = 双长度密钥；encryMode = 4 或 encryMode = 5时该域存在
inputDataLength: 加解密数据长度
inputData: nB/1A+3H 待加解密数据或待加解密密钥在HSM安全存储区内的索引
输出参数:
outputData: nB 加解密/加解密密钥数据
返回值:
>0	ouputData的长度
<0	失败
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

	// 与密码机通讯
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


// 将一个ZMK加密的密钥转换为LMK加密,支持国产算法SM1和SFF33
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

	if (strlen(zmk) == 4)	// 索引号
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
	if (keyByLmkStrLen >= ret)      // 没有校验值
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


// 将一个ZMK加密的密钥转换为LMK加密,SM4算法
int UnionHsmCmdSV(TUnionDesKeyType keyType, char *zmk, char *keyByZmk, char *keyByLmk, char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	offset = 0;

	memcpy(hsmCmdBuf,"SV",2);
	hsmCmdLen = 2;

	//密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSV:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//zmk密钥
	if (strlen(zmk) == 4)	// 索引号
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

	//ZMK下加密的密钥
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


// 生成银联MAC的指令
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

	// 消息块号
	hsmCmdBuf[hsmCmdLen] = msgNo;
	hsmCmdLen += 1;

	// 密钥类型	
	sprintf(hsmCmdBuf + hsmCmdLen,"%d",keyType);
	hsmCmdLen += 1;

	// 密钥长度标志
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
			// 2008/07/11,增加结束
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

// 将ZMK加密的ZPK转换为LMK加密
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

// 生成ZAK/ZEK
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

	// 消息块号
	hsmCmdBuf[hsmCmdLen] = msgNo;
	hsmCmdLen += 1;
	//UnionProgramerLog("in UnionHsmCmdMS:: after msgNo hsmCmdBuf [%s]!\n", hsmCmdBuf);

	// 密钥类型	
	hsmCmdBuf[hsmCmdLen] = keyType + '0';
	hsmCmdLen += 1;
	//UnionProgramerLog("in UnionHsmCmdMS:: after keyType hsmCmdBuf [%s]!\n", hsmCmdBuf);

	// 密钥长度标志
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
			// 2008/07/11,增加结束
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
   函数功能：
   DG指令，用PVK生成PIN的PVV(PIN Verification Value)，
   采用的加密标准为Visa Method
   输入参数：
   pinLength：LMK加密的PIN密文长度
   pin：LMK加密的PIN密文
   pvkLength：LMK加密的PVK对长度
   pvk：LMK加密的PVK对
   lenOfAccNo：客户帐号长度
   accNo：客户帐号
   输出参数：
   pvv：产生的PVV

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

	// 命令字
	memcpy(hsmCmdBuf,"DG",2);
	hsmCmdLen = 2;
	// LMK加密的PVK对
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN
	memcpy(hsmCmdBuf+hsmCmdLen,pin,pinLength);
	hsmCmdLen += pinLength;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;

	// modify by leipp 20150418 begin
	// PVKI（0－6之间）
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
	// 产生的PVV
	memcpy(pvv,hsmCmdBuf+4,4);
	pvv[4] = 0;
	return(4);
}

/*
   函数功能：
   CA指令，将一个TPK加密的PIN转换为由ZPK加密
   输入参数：
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinFormat1：转换前的PIN格式
   pinBlockByTPK：转换前由TPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   pinFormat2：转换后的PIN格式
   输出参数：
   pinBlockByZPK：转换后由ZPK加密的PIN密文
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

	// 命令字
	memcpy(hsmCmdBuf,"CA",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN最大长度
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// 由TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// 转换前的PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,2);
	hsmCmdLen += 2;
	// 转换后的PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,2);
	hsmCmdLen += 2;
	// 12位客户帐号
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
	// 转换后ZPK加密的PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4+2,16);
	pinBlockByZPK[16] = 0;
	return(16);
}



/*
   函数功能：
   JE指令，将一个ZPK加密的PIN转换为由LMK加密
   输入参数：
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinFormat：PIN格式
   pinBlockByZPK：转换前由ZPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：转换后由LMK加密的PIN密文

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

	// 命令字
	memcpy(hsmCmdBuf,"JE",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;

	if (strcmp(pinFormat,"04") == 0)
	{
		strcpy(hsmCmdBuf+hsmCmdLen,"FFFFFF");	
		hsmCmdLen += 6;
	}

	// 12位客户帐号
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
	// 转换后LMK加密的PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   函数功能：
   JC指令，将一个TPK加密的PIN转换为由LMK加密
   输入参数：
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   pinFormat：PIN格式
   pinBlockByTPK：转换前由TPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：转换后由LMK加密的PIN密文

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

	// 命令字
	memcpy(hsmCmdBuf,"JC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
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
	// 转换后LMK加密的PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   函数功能：
   JG指令，将一个LMK加密的PIN转换为由ZPK加密
   输入参数：
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinFormat：PIN格式
   pinLength：LMK加密的PIN密文长度
   pinBlockByZPK：转换前由ZPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByZPK：转换后由LMK加密的PIN密文

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

	// 命令字
	memcpy(hsmCmdBuf,"JG",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;

	if (strcmp(pinFormat,"04") == 0)
	{
		strcpy(hsmCmdBuf+hsmCmdLen,"FFFFFF");	
		hsmCmdLen += 6;
	}

	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJG:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// 转换后ZPK加密的PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4,16);
	pinBlockByZPK[16] = 0;
	return(16);
}


/*
   函数功能：
   JA指令，随机产生一个PIN
   输入参数：
   pinLength：要求随机生成的PIN明文的长度
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：随机产生的PIN的密文，由LMK加密

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
	// 命令字
	memcpy(hsmCmdBuf,"JA",2);
	hsmCmdLen = 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pinLength);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdJA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// 随机产生的PIN的密文，由LMK加密
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   函数功能：
   EE指令，用IBM方式产生一个PIN
   输入参数：
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinOffset：PIN Offset，左对齐，右补'F'
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：随机产生的PIN的密文，由LMK加密

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

	// 命令字
	memcpy(hsmCmdBuf,"EE",2);
	hsmCmdLen = 2;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 设置不检查响应码
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// 随机产生的PIN的密文，由LMK加密
		memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
		pinBlockByLMK[ret-4] = 0;
		return(ret-4);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}


/*
   函数功能：
   DE指令，用IBM方式产生一个PIN的Offset
   输入参数：
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinLength：LMK加密的PIN密文长度
   pinBlockByLMK：由LMK加密的PIN密文
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinOffset：PIN Offset，左对齐，右补'F'

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

	// 命令字
	memcpy(hsmCmdBuf,"DE",2);
	hsmCmdLen = 2;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 设置不检查响应码
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// 产生的PIN的Offset
		memcpy(pinOffset,hsmCmdBuf+4,12);
		pinOffset[12] = 0;
		return(12);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}


/*
   函数功能：
   BA指令，使用本地主密钥加密一个PIN明文
   输入参数：
   pinCryptogramLen：加密后的PIN密文的长度
   pinTextLength：PIN明文的长度
   pinText：PIN明文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：LMK加密的PIN密文

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

	// 命令字
	memcpy(hsmCmdBuf,"BA",2);
	hsmCmdLen = 2;

	// 填充PIN明文
	if ((pinCryptogramLen >= sizeof(tmpBuf)) || (pinTextLength >= sizeof(tmpBuf)))
	{
		UnionUserErrLog("in UnionHsmCmdBA:: pinCryptogramLen = [%d] or pinTextLength = [%d] too long!\n",pinCryptogramLen,pinTextLength);
		return(errCodeParameter);
	}
	memset(tmpBuf,'F',pinCryptogramLen);
	tmpBuf[pinCryptogramLen] = 0;
	memcpy(tmpBuf,pinText,pinTextLength);

	// 填充后的PIN明文
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	// 12位客户帐号
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
	// LMK加密的PIN密文
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	pinBlockByLMK[ret-4] = 0;
	return(ret-4);
}


/*
   函数功能：
   NG指令，使用本地主密钥解密一个PIN密文
   输入参数：
   pinCryptogramLen：LMK加密的PIN密文的长度
   pinCryptogram：LMK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   referenceNumber：用LMK18-19加密帐号得到的偏移值
   pinText：PIN明文

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

	// 命令字
	memcpy(hsmCmdBuf,"NG",2);
	hsmCmdLen = 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinCryptogram,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetMaskPrintTypeForHSMResCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdNG:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// 用LMK18-19加密帐号得到的偏移值
	memcpy(referenceNumber,hsmCmdBuf+4+pinCryptogramLen,12);
	referenceNumber[12] = 0;

	// 填充的PIN明文
	memcpy(tmpBuf,hsmCmdBuf+4,pinCryptogramLen);
	for (i=0;i<pinCryptogramLen;i++)
	{
		if (tmpBuf[i] == 'F')
		{
			tmpBuf[i] = '\0';
			break;
		}
	}
	// PIN明文
	memcpy(pinText,tmpBuf,i);
	pinText[i] = 0;
	return(i);
}


/*
   函数功能：
   DA指令，用IBM方式验证终端的PIN
   输入参数：
   pinBlockByTPK：TPK加密的PIN密文
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinOffset：IBM Offset，左对齐，右补'F'
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

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

	// 命令字
	memcpy(hsmCmdBuf,"DA",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 最大PIN长度
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
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
   函数功能：
   EA指令，用IBM方式验证交换中心的PIN
   输入参数：
   pinBlockByZPK：ZPK加密的PIN密文
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinOffset：IBM Offset，左对齐，右补'F'
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pvkLength：LMK加密的TPK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

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

	// 命令字
	memcpy(hsmCmdBuf,"EA",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 最大PIN长度
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	// IBM Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 设置不检查响应码
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

	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	// modify by leipp 20160306 当返回码为88时,检查pinOffset中pin明文长度,如果长度等于4,返回正确,否则报错返回
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0) || (memcmp(errCode,"88",2) == 0 && pinLen == 4))
		return(0);
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
	// modify end 20160306
}


/*
   函数功能：
   DC指令，用VISA方式验证终端的PIN
   输入参数：
   pinBlockByTPK：TPK加密的PIN密文
   pvv：终端PIN的4位VISA PVV
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

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

	// 命令字
	memcpy(hsmCmdBuf,"DC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	hsmCmdLen += 1;
	// 终端PIN的4位VISA PVV
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
   函数功能：
   EC指令，用VISA方式验证交换中心的PIN
   输入参数：
   pinBlockByZPK：ZPK加密的PIN密文
   pvv：终端PIN的4位VISA PVV
   zpkLength：LMK加密的ZPK
   zpk：LMK加密的ZPK
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

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

	// 命令字
	memcpy(hsmCmdBuf,"EC",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdEC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	hsmCmdLen += 1;
	// 终端PIN的4位VISA PVV
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
   函数功能：
   BC指令，用比较方式验证终端的PIN
   输入参数：
   pinBlockByTPK：TPK加密的PIN密文
   tpkLength：LMK加密的TPK
   tpk：LMK加密的TPK
   pinByLMKLength：主机PIN密文长度
   pinByLMK：主机PIN密文，由LMK02-03加密
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

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

	// 命令字
	memcpy(hsmCmdBuf,"BC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 主机PIN密文，由LMK02-03加密
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
   函数功能：
   BE指令，用比较方式验证交换中心的PIN
   输入参数：
   pinBlockByZPK：ZPK加密的PIN密文
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinByLMKLength：主机PIN密文长度
   pinByLMK：主机PIN密文，由LMK02-03加密
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

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

	// 命令字
	memcpy(hsmCmdBuf,"BE",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdBE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 主机PIN密文，由LMK02-03加密
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
   函数功能：
   CW指令，产生VISA卡校验值CVV
   输入参数：
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   cvv：生成的VISA卡的CVV

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

	// 命令字
	memcpy(hsmCmdBuf,"CW",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCW:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 客户帐号
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA卡的有效期
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA卡服务商代码
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCW:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	// 生成的VISA卡的CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	cvv[3] = 0;
	return(3);
}


/*
   函数功能：
   CY指令，验证VISA卡的CVV
   输入参数：
   cvv：待验证的VISA卡的CVV
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

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

	// 命令字
	memcpy(hsmCmdBuf,"CY",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdCY:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 待验证的VISA卡的CVV
	memcpy(hsmCmdBuf+hsmCmdLen,cvv,3);
	hsmCmdLen += 3;
	// 客户帐号
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA卡的有效期
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA卡服务商代码
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

	// 命令字
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;

	// 加密密钥的LMK对指示符，即密钥类型代码的最后2位
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
	// 密钥长度标志
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
	// LMK加密的密钥密文
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
		// 分隔符
		hsmCmdBuf[hsmCmdLen] = ';';
		hsmCmdLen += 1;
		// 密钥类型代码
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
   函数功能：
   A4指令，用几把密钥的密文成分合成一把密钥，并生成校验值
   输入参数：
   keyType：密钥的类型
   keyLength：密钥的长度
   partKeyNum：密钥成分的数量
   partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
   输出参数：
   keyByLMK：合成的密钥密文，由LMK加密
   checkValue：合成的密钥的校验值

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

	// 命令字
	memcpy(hsmCmdBuf,"A4",2);
	hsmCmdLen = 2;
	// 密钥成分数量
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// 密钥类型代码
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
	// 密钥长度标志
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
		// LMK加密的密钥密文
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
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			keyByLMK[16] = 0;
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+16,6);
			checkValue[6] = 0;
			return(16+6);
		case    con128BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			keyByLMK[32] = 0;
			// 合成的密钥的校验值
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
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			keyByLMK[48] = 0;
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			checkValue[6] = 0;
			return(48+6);
		default:
			UnionUserErrLog("in UnionHsmCmdA4:: unknown key length!\n");
			return(errCodeParameter);
	}
}



/*
   功能    由源帐号生成的PINBLOCK转换为由目的帐号生成的PINBLOCK，并受指定的密钥加密保护
   说明    根据密钥长度确定算法
   输入参数
   zpk 被 LMK 06-07 加密的ZPK
   pvk 被 LMK 14-15加密的ZPK
   maxPinLen 最大PIN长度
   decimalizationTable 十进制数表
   pinValidationData 
   pinBlock1 被ZPK采用ANSI X9.8标准加密
   accNo1 源账号
   accNo2 目标账号

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
	// 命令字
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
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo1,strlen(accNo1),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo1,(int)strlen(accNo1));
		return(ret);
	}
	hsmCmdLen += ret;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo2,strlen(accNo2),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo2,(int)strlen(accNo2));
		return(ret);
	}
	hsmCmdLen += ret;
	// end of modification 2014-04-25
	hsmCmdBuf[hsmCmdLen] = 0;

	// 设置不检查响应码
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdDF:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// 设置需要检查响应码
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


// 将一个LMK加密的密钥转换为ZMK加密,SM4算法
int UnionHsmCmdST(TUnionDesKeyType keyType, char *key, char *zmk, char *keyByZMK, char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;
	int	offset = 0;

	memcpy(hsmCmdBuf,"ST",2);
	hsmCmdLen = 2;

	//密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdST:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//ZMK密钥
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,zmk,32);
	hsmCmdLen += 32;

	//LMK下加密的密钥
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
   函数功能：
   33指令，私钥解密
   输入参数：
   flag：用公钥加密时所采用的填充方式，
   '0'：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
   '1'：PKCS填充方式（一般情况下使用此方式）
   vkIndex：私钥索引，"00"－"20"
   lenOfVK：私钥长度
   vk：私钥密文
   cipherDataLen：密文数据的字节数
   cipherData：用于解密的密文数据
   sizeOfPlainData：plainData所在的存储空间大小
   输出参数：
   plainData：解密得到的明文数据

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
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   pvkIndex：      指定的私钥，用于解密PIN数据密文
   fillMode：      “0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
   “1”：PKCS填充方式（一般情况下使用此方式）
   ZPK：	  用于加密PIN的密钥。
   accNo：	用户主帐号
lenOfPinBlock:  公钥加密的PIN密文的长度
pinBlock：      经公钥加密的PIN数据密文
输出参数
lenOfPin：      密钥长度
pinBlock1：     返回ZPK加密下的PIN密文
pinBlock2：     返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
lenOfUniqueID： 01-20
UniqueID：      返回的经ASCII扩展后ID码明文

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
功能
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
   srcAlgorithmID  非对称密钥算法标识，0:国际算法,  1:国密算法
   pvkIndex：   指定的私钥，用于解密PIN数据密文
   lenOfVK	私钥长度
   vkValue	私钥值
   fillMode：   “0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
		“1”：PKCS填充方式（一般情况下使用此方式）
   dstAlgorithmID  ZPK密钥算法标识,0:国际算法,  1:国密算法
   ZPK：	用于加密PIN的密钥。
   accNo：	用户主帐号
   lenOfPinBlock:  公钥加密的PIN密文的长度
   pinBlock：      经公钥加密的PIN数据密文
输出参数
   lenOfPin：      密钥长度
   pinBlock1：     返回ZPK加密下的PIN密文
   pinBlock2：     返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
   lenOfUniqueID： 01-20
   UniqueID：      返回的经ASCII扩展后ID码明文

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

	if (srcAlgorithmID == 1)	// 国密算法
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
	}

	if (dstAlgorithmID == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"H",1);
		hsmCmdLen += 1;
	}

	//私钥索引
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

	// 国际算法才有此域
	if (srcAlgorithmID == 0)
	{
		//用公钥加密时所采用的填充方式
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
   功能：
   将PIN由X9.8转为特殊算法加密
   参数:
   srcZPK 当前加密PIN块的ZPK；LMK对（06-07）下加密
   dstZPK 将要加密PIN块的ZPK；LMK对（06-07）下加密。
   srcPin 源ZPK加密的字符密码密文
   srcPan 用户有效主帐号
   dstPan 用户有效主帐号
   输出:
   lenOfPin	pin明文长度
   dstPinBlock 返回PIN密文 
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
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   OriKeyType：    源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
   OriKey：	源KEY，用于解密PIN的密钥。
   lenOfAcc：      帐号长度，4~20。
   accNo：	用户主帐号。
   DesKeyType：    ’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
   DesKey：	目标KEY，用于加密PIN的密钥。
   pinBlock：      经公钥加密的PIN数据密文。
   输出参数
   lenOfPinBlock1：密码长度。
   pinBlock1：     返回ZPK2/PVK加密下的PIN密文。

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
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   OriKeyType：    源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
   OriKey：	源KEY，用于解密PIN的密钥。
   lenOfAcc：      帐号长度，4~20。
   accNo：	用户主帐号。
   DesKeyType：    ’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
   DesKey：	目标KEY，用于加密PIN的密钥。
   pinBlock：      经公钥加密的PIN数据密文。
   输出参数
   lenOfPinBlock1：密码长度。
   pinBlock1：     返回ZPK2/PVK加密下的PIN密文。

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
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   ZPK：	  用于解密PIN的密钥。
   lenOfAcc：      帐号长度，4~20。
   accNo：	用户主帐号。
   pinBlockByZPK： 经ZPK加密的PIN数据密文。
   PVK：	  PVK。
   pinBlockByPVK： 经PVK加密的PIN数据密文。
   输出参数


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
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   ZPK：	  用于加密PIN的密钥。
   accNo：	用户主帐号。
   pinBlockByZPK： 经ZPK加密的PIN数据密文。
   dataOfZAK：     产生ZAK密钥的因素之一。
   输出参数
   lenOfPin：      密码长度
   ZAK：	  返回LMK26-27对对加密下的ZAK密钥。


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
   功能：把PinOffset转换为专用算法（FINSE算法）加密的密文
   输入参数 
   PVK LMK对（14-15）下加密的PVK；用于解密offset
   pinOffSet Offset的值；采用左对齐的方式在右边填充字符“F”
   minPinLen 最小的PIN长度
   accNo 账号
   decimalizationTable 十进制转换表
   pinValidationData PIN校验数据
   输出参数
   pinBlock PIN密文

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
   功能：把PinBlock转换为专用算法（FINSE算法）加密的密文
   输入参数
   ZPK LMK对（14-15）下加密的PVK；用于解密offset
   minPinLen 最小的PIN长度
   accNo 账号
   OriPinBlock 源PIN块
   输出参数
   pinBlock PIN密文 

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
   功能：采用离散MAK计算MAC
   输入参数
   ZAK 被LMK26-27对加密
   scatData 用于计算离散密钥的数据
   lenOfmac MAC数据长度
   macData MAC数据
   输出参数
   mac 返回的MAC值

   计算流程：
   1.      用ZAK密钥对离散数据采用IC卡标准离散算法进行离散，如果ZAK为64bit长，则直接对离散数据进行DES加密即可。
   2.      用离散密钥对MAC数据采用Ansi9.19（ZAK双倍长以上）或9.9（ZAK单倍长）算法计算MAC；
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
   产生随机数指令
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
/*一次性发送加密机数据块最大长度*/
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



/* Y4指令,功能:产生密钥.使用根密钥对离散数据1进行离散,得到离散密钥;使用保护密钥(ZMK)加密输出和LMK06-07对加密输出.
   输入参数:
   kek：保护密钥KEK
   rootKey：应用主密钥
   keyType：密钥类型；001-ZPK，008-ZAK。
   discreteNum：离散次数
   discreteData1：离散数据1
   discreteData2：离散数据2
   discreteData3：离散数据3
   输出参数:
   keyByKek：使用KEK加密的key
checkValue:校验值

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

	memcpy(hsmCmdBuf, "Y4", 2);      /*命令代码*/
	hsmCmdLen = 2;

	if ((ret = UnionPutKeyIntoRacalKeyString(kek, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*保护密钥*/
	{
		UnionUserErrLog("in UnionHsmCmdY4:: kek UnionPutKeyIntoRacalKeyString [%s]!\n", kek);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(rootKey, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*根密钥*/
	{
		UnionUserErrLog("in UnionHsmCmdY4:: rootKey UnionPutKeyIntoRacalKeyString [%s]!\n", rootKey);
		return(ret);
	}
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", discreteNum); /*离散次数*/
	hsmCmdLen += 1;
	/*离散数据*/
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

	/*向密码机发送请求报文*/
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY4:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(keyByLmk, hsmCmdBuf + 4, 32);  /*返回keyByLmk*/
	memcpy(keyByKek, hsmCmdBuf + 36, 32);  /*返回keyByKek*/
	memcpy(checkValue,hsmCmdBuf+36+32,16);

	return (0);
}



/*
   函数功能：
   A5指令，输入密钥明文的几个成份合成最终的密钥，同时用相应的LMK密钥对加密
   输入参数：
   keyType：密钥的类型
   keyLength：密钥的长度
   partKeyNum：密钥成分的数量
   partKey：存放各个密钥成分的数组缓冲
   输出参数：
   keyByLMK：合成的密钥密文，由LMK加密
   checkValue：合成的密钥的校验值

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

	// 命令字
	memcpy(hsmCmdBuf,"A5",2);
	hsmCmdLen = 2;
	// 密钥成分数量
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// 密钥类型代码
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdA5:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// 密钥长度标志
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
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			keyByLMK[16] = 0;
			// 合成的密钥的校验值
			//memcpy(checkValue,hsmCmdBuf+4+16,6);
			ret = sprintf(checkValue,"%s",hsmCmdBuf+4+16);		// modify by leipp 20150731 目前基本国际返回校验值为16
			checkValue[ret] = 0;
			return(16+ret);
		case    con128BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			keyByLMK[32] = 0;
			// 合成的密钥的校验值
			if (SM4Mode == 1)
			{
				memcpy(checkValue,hsmCmdBuf+4+1+32,16);
				return(32+16);
			}
			else
			{
				//memcpy(checkValue,hsmCmdBuf+4+1+32,6);
				ret = sprintf(checkValue,"%s",hsmCmdBuf+4+1+32); // modify by leipp 20150731 目前基本国际返回校验值为16
				checkValue[ret] = 0;
				return(32+ret);
			}
		case    con192BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			keyByLMK[48] = 0;
			// 合成的密钥的校验值
			//memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			ret = sprintf(checkValue,"%s",hsmCmdBuf+4+1+48);	// modify by leipp 20150731 目前基本国际返回校验值为16
			checkValue[ret] = 0;
			return(48+ret);
		default:
			UnionUserErrLog("in UnionHsmCmdA5:: unknown key length!\n");
			return(errCodeParameter);
	}
}

/*
   函数功能：
   N4指令 使用离散密钥转加密PIN
   输入参数：
importProgram: 导入方案		0-不使用过程密钥（新增域）
1-使用过程密钥(同离散算法)
2-使用过程密钥（标准算法）
3：使用过程密钥（密钥左右部分分别做DES）
importKeyFlag: 导入密钥标志	0 – 使用加密机内部密钥
1 – 使用LMK保护的密钥
importKey:	  加密机内部导入密钥	1A+3H	若密钥标志为0，才有该域
1A+3H表示使用K+3位索引方式读取加密机内保存密钥。
mkDvsNum1:      离散次数		指定对根密钥离散的次数（0-3次）
DvsData1:       离散数据	n*16H	离散卡片密钥的数据，其中n代表离散次数
ProcessData1:   过程数据	N*16H	用于产生过程密钥的数据，仅当方案ID为1,2,3时有此域
KeyLengthFlag1:导入密钥长度标识	1N	若密钥标志为1，才有该域  密钥长度：0=单倍长度DES密钥
1=双倍长度DES密钥（暂时只支持双倍长密钥）
2=三倍长度DES密钥
importKeyByLMK:	  LMK保护的导入密钥	16H/32H/48H	若密钥标志为1，才有该域密钥密文
exportProgram: 导出方案	1N	0-不使用过程密钥（新增域）
1-使用过程密钥(同离散算法)
2-使用过程密钥（标准算法）
3：使用过程密钥（密钥左右部分分别做DES）
exportKeyFlag: 导出密钥标志	1N	0 – 使用加密机内部密钥
1 – 使用LMK保护的密钥
exportKey:	  加密机内部导出密钥	1A+3H	若密钥标志为0，才有该域
1A+3H表示使用K+3位索引方式读取加密机内保存密钥。
mkDvsNum2:   离散次数	1N	指定对根密钥离散的次数（0-3次）
DvsData2:	  离散数据	n*16H	离散卡片密钥的数据，其中n代表离散次数
ProcessData2:过程数据	N*16H	用于产生过程密钥的数据，仅当方案ID为1,2,3时有此域
KeyLengthFlag2:导出密钥长度标识	1N	若密钥标志为1，才有该域 密钥长度：0=单倍长度DES密钥
1=双倍长度DES密钥（暂时只支持双倍长密钥）
2=三倍长度DES密钥
exportKeyByLMK:	LMK保护的导出密钥	16H/32H/48H	若密钥标志为1，才有该域 密钥密文
maxPasswordLength:	最大密码长度	2N	“12”
accNo1:			原帐号	12N	账号中去除校验位的最右12位。原PINBLOCK加密帐号
accNo2:			目的帐号	12N	账号中去除校验位的最右12位。目的PINBLOCK加密帐号
format1:		源PIN块格式	2N	PIN块的格式代码。“01”为ANSI X9.8格式。目前仅需要支持“01”，”03”,“13”，“14”格式。
format2:		目的PIN块格式	2N	PIN块的格式代码。“01”为ANSI X9.8格式。目前仅需要支持“01”，”03”,“13”，“15”，“16”格式。
accNo:			目的全帐号	16N	仅当目的格式为16时有此域。
pinBlockByZPK:		源PINBLOCK	16H	源ZPK下加密的源PIN块。


输出参数：
pinBlock:		目标PIN 	16H	目标ZPK和目的帐号加密的PIN密文
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

	// 命令代码
	memcpy(hsmCmdBuf,"N4",2);
	hsmCmdLen += 2;

	// 导入方案
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",importProgram);
	hsmCmdLen += 1;

	// 导入密钥标志
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",importKeyFlag);
	hsmCmdLen += 1;

	// 加密机内部导入密钥
	if (importKeyFlag == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, importKey, 4);
		hsmCmdLen += 4;
	}

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mkDvsNum1);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, DvsData1, mkDvsNum1*16);
	hsmCmdLen += mkDvsNum1*16;

	// 过程数据
	if (importProgram == 1 || importProgram == 2 || importProgram == 3)
	{
		// modify by chenqy 20151227 mkDvsNum1*16 改成 16
		memcpy(hsmCmdBuf+hsmCmdLen, ProcessData1, 16);
		hsmCmdLen += 16;
		// modify end
	}

	if (importKeyFlag == 1)
	{
		// 导入密钥长度标识
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",KeyLengthFlag1);
		hsmCmdLen += 1;

		// LMK保护的导入密钥
		memcpy(hsmCmdBuf+hsmCmdLen, importKeyByLMK, strlen(importKeyByLMK));
		hsmCmdLen += strlen(importKeyByLMK);
	}

	// 导出方案
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",exportProgram);
	hsmCmdLen += 1;

	// 导出密钥标志
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",exportKeyFlag);
	hsmCmdLen += 1;

	// 加密机内部导出密钥
	if (exportKeyFlag == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, exportKey, 4);
		hsmCmdLen += 4;
	}

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mkDvsNum2);
	hsmCmdLen += 1;

	if (mkDvsNum2 != 0)
	{	
		// 离散数据
		memcpy(hsmCmdBuf+hsmCmdLen, DvsData2, mkDvsNum2*16);
		hsmCmdLen += mkDvsNum2*16; // modify by chenqy 20151225 "mkDvsNum1 改成 mkDvsNum2"
	}

	// 过程数据
	if (exportProgram == 1 || exportProgram == 2 || exportProgram == 3)
	{
		// modify by chenqy 20151227 mkDvsNum2*16 改成 16
		memcpy(hsmCmdBuf+hsmCmdLen, ProcessData2, 16);
		hsmCmdLen += 16;
		// modify end
	}

	if (exportKeyFlag == 1)
	{
		// 导出密钥长度标识
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",KeyLengthFlag2);
		hsmCmdLen += 1;

		// LMK保护的导出密钥
		memcpy(hsmCmdBuf+hsmCmdLen, exportKeyByLMK, strlen(exportKeyByLMK));
		hsmCmdLen += strlen(exportKeyByLMK);
	}

	// 最大密码长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",maxPasswordLength);
	hsmCmdLen += 2;

	// 源帐号
	memcpy(hsmCmdBuf+hsmCmdLen, accNo1, 12);
	hsmCmdLen += 12;

	// 目的帐号
	memcpy(hsmCmdBuf+hsmCmdLen, accNo2, 12);
	hsmCmdLen += 12;

	// 源PIN块格式
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",format1);
	hsmCmdLen += 2;

	// 目的PIN块格式
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",format2);
	hsmCmdLen += 2;

	// 目的全帐号	
	if (format2 == 16)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, accNo, 16);
		hsmCmdLen += 16;
	}

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, pinBlockByZPK, 16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
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
   函数功能：
   18指令 使用离散密钥加解密数据或计算MAC
   输入参数：
keyIndex: 密钥索引 3H:  001-57F（已支持）
K+3H: K001-K57F （新增）
T109(新增)
注：SJL06需支持T109方式，索引密钥方式可不需支持

withKey: 外带密钥 仅当上一个域为TXXX时有此域
mkDvsNum: 离散次数 指定对根密钥离散的次数（0-2次）
DvsData: 离散数据 n*16H 离散卡片密钥的数据，其中n代表离散次数
proKeyAlgorithmFlag: 过程密钥算法标志			0：标准算法（3DES(Kc),64bit）
1：DES(Kcl+Kcr,128bit)
2：AES(Kc,128bit)
3：3DES（Kc，128bit）
4：不计算过程密钥 （支持昆山农商行需求）
GC_Data: 用于产生过程密钥的数据（AES算法要求过程数据为128bit）
（当过程密钥算法标志域为4时，无此域）
encryptFlag: 加密标志 0：不加/解密
1：加密
2：解密
encryptAlgorithmSelect: 加密算法选择 0：标准算法(1-DES-CBC(Kg))
1：3-DES-CBC(Kg)
2：AES-ECB(Kg)
3：1-DES-ECB(Kgl)
4：异或-ECB(Kg)
(加密标志为1、2 时存在)
macFlag:  MAC 标志 0：不做MAC
1：对原报文做MAC
2：对处理后的报文做MAC
macAlgorithmSelect: MAC 算法选择 0：标准算法（1-DES-CBC(Kg)）
1：1-DES-CBC(Kgl)
2：1-DES-CBC-Lite(Kgl)
3：SHA1(Kg)
4：异或MAC（昆山农商行需求）
(MAC 标志为1、2 时存在)
lengthOfMessage: 报文长度 报文的字节数
Message: 报文 长度N

输出参数：
mac: MAC值 仅当MAC 标志为1/2 时有此域
criperMessage: 报文密文 仅当加密标志为1/2 时有此域长度N
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

	// 命令代码
	memcpy(hsmCmdBuf,"18",2);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, keyIndex, strlen(keyIndex));
	hsmCmdLen += strlen(keyIndex);

	// 外带密钥
	if (keyIndex[0] == 'T')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, withKey, 32);
		hsmCmdLen += 32;
	}

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, DvsData, (mkDvsNum * 16));
	hsmCmdLen += (mkDvsNum * 16);

	// 过程密钥算法标志
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", proKeyAlgorithmFlag);
	hsmCmdLen += 1;

	// 用于产生过程密钥的数据
	if (proKeyAlgorithmFlag != 4)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, GC_Data, strlen(GC_Data));
		hsmCmdLen += strlen(GC_Data);
	}

	// 加密标志
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", encryptFlag);
	hsmCmdLen += 1;

	// 加密算法选择
	if (encryptFlag == 1 || encryptFlag == 2)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", encryptAlgorithmSelect);
		hsmCmdLen += 1;
	}

	// MAC 标志
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", macFlag);
	hsmCmdLen += 1;

	// MAC 算法选择
	if (macFlag == 1 || macFlag == 2)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", macAlgorithmSelect);
		hsmCmdLen += 1;
	}

	// 添加IV初始向量
	if (encryptAlgorithmSelect == 0 || encryptAlgorithmSelect == 1)
	{
		memmove(Message+16,Message,lengthOfMessage);
		sprintf(Message,"%016d%s",0,Message+16);
		lengthOfMessage += 16;
	}

	// 报文长度
	sprintf(hsmCmdBuf+hsmCmdLen, "%03d", lengthOfMessage/2);
	hsmCmdLen += 3;

	// 报文内容
	memcpy(hsmCmdBuf+hsmCmdLen, Message, lengthOfMessage);
	hsmCmdLen += lengthOfMessage;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
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
   函数功能：
   U2指令，使用指定的应用主密钥进行2次离散得到卡片应用子密钥，
   使用指定控制密钥子密钥进行加密保护输出并进行MAC计算。
   输入参数：
securityMech: 安全机制(S：单DES加密和MAC, T：三DES加密和MAC)
mode: 模式标志, 0-仅加密 1-加密并计算MAC
id: 方案ID, 0=M/Chip4(CBC模式，强制填充X80) 1=VISA/PBOC(带长度指引的ECB) 2=PBOC1.0模式(ECB模式，外带填充数据)

mk: 根密钥
mkType: 根密钥类型109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
当使用加密机内密钥时该域不起作用
mkIndex: 根密钥索引, K+3位索引方式读取加密机内保存密钥
mkDvsNum: 根密钥离散次数, 1-3次
mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

pkType: 保护密钥类型 0=TK(传输密钥)1=DK-SMC(使用MK-SMC实时离散生成的子密钥)
pk: 保护密钥
pkIndex: 保护密钥索引
pkDvsNum: 保护密钥离散次数,仅当“保护密钥类型”为1时有,从MK-SMC离散得到DK-SMC的离散次数,范围为1-3
pkDvsData: 保护密钥离散数据,n*16H仅当“保护密钥类型”为1时有,保护密钥的离散数据，其中n为“保护密钥离散次数”

proKeyFlag:过程密钥标识，Y:计算过程密钥　N:不计算过程密钥　可选项:当没有该域时缺省为N
proFactor: 过程因子(16H),可选项:仅当过程密钥标志为Y时有

ivCbc: IV-CBC,8H 仅当“方案ID”为0时有

encryptFillDataLen: 加密填充数据长度(4H),仅当“方案ID”为2时有（不大于1024）
和密钥明文一起进行加密的数据长度
encryptFillData: 加密填充数据 nB 仅当“方案ID”为2时有,和密钥明文一起进行加密
encryptFillOffset: 加密填充数据偏移量 4H 仅当“方案ID”为2时有
将密钥明文插入到加密填充数据的位置, 数值必须在0到加密填充数据长度之间

ivMac: IV-MAC,16H 仅当“模式标志”为1时有
macDataLen: MAC填充数据长度 4H 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据长度
macData: MAC填充数据 nB 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据
macOffset: 偏移量 4H 仅当“模式标志”为1时有,将密钥密文插入到MAC填充数据的位置
数值必须在0到MAC填充数据长度之间
输出参数：
mac: MAC值 8B 仅当“模式标志”为1时有
criperDataLen: 密文数据长度 4H 密文数据长度(必须是8的倍数，并等于前缀长度、后缀长度、密钥长度的和)
criperData: nB 输出的密文数据
checkValue: 校验值

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
	// 命令代码
	memcpy(hsmCmdBuf,"U2",2);
	hsmCmdLen += 2;

	// 安全机制
	memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
	hsmCmdLen += 1;
	if ((securityMech[0] == 'Q') || (securityMech[0] == 'U'))
		lenOfDvsData = 32;
	else
		lenOfDvsData = 16;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// 保护密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 1);
	hsmCmdLen += 1;

	// 保护密钥
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

	// 保护密钥离散次数, 保护密钥离散数据
	if (pkType[0] == '1')
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
		hsmCmdLen += pkDvsNum*lenOfDvsData;
	}

	// modify by lix,20090730
	// 过程密钥标识
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// 过程因子
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
		//lizh于20140328修改，修改内容为把ivCbc由复制8个到16个
		//	memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 8);
		//	hsmCmdLen += 8;
		memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 16);
		hsmCmdLen += 16;
	}

	// 加密填充数据长度、加密填充数据、加密填充数据偏移量
	if (id[0] == '2')
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, encryptFillData, encryptFillDataLen);
		hsmCmdLen += encryptFillDataLen;

		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillOffset/2);
		hsmCmdLen += 4;
	}

	// IV-MAC, MAC填充数据长度, MAC填充数据, 偏移量
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

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU2:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (mode[0] == '1') // MAC值
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
	else if (mode[0] == '2') // 模式2时,返回校验值
	{
		memcpy(checkValue, hsmCmdBuf + ret - 16, 16);
		checkValue[16] = 0;
		offset += 16;
	}


	// 密文数据长度
	offset += 4;
	*criperDataLen = ret - offset;

	//edit by chenwd 20140719
	// 密文数据
	// memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
	// UnionLog("in UnionHsmCmdU2::*criperDataLen=[%d]\n",*criperDataLen);

	if (mode[0] == '1')
	{
		if (securityMech[0] == 'U')
		{
			// 密文数据
			memcpy(criperData, hsmCmdBuf + 40, *criperDataLen);
			criperData[*criperDataLen] = 0;
		}
		else
		{
			// 密文数据
			memcpy(criperData, hsmCmdBuf + 24, *criperDataLen);
			criperData[*criperDataLen] = 0;
		}
	}
	else
	{
		// 密文数据
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

	// 命令代码
	memcpy(hsmCmdBuf,"KW",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥
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

	// add by lisq 20150120 微众银行客户化扩展
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
	// add by lisq 20150120 end 微众银行客户化扩展

	// PAN长度
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

	// 交易数据长度
	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6') )
	{
		// modified 2011-11-08
		//sprintf(hsmCmdBuf+hsmCmdLen, "%2x", lenOfData/2);
		sprintf(hsmCmdBuf+hsmCmdLen, "%02X", lenOfData/2);
		hsmCmdLen += 2;
	}

	// 交易数据
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

	// 与密码机通讯
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


// added 2011-11-08 ０６机用，与09KW指令相同
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

	// 命令代码
	memcpy(hsmCmdBuf,"KX",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥
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

	// PAN长度
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

	// 交易数据长度
	if ( (mode[0]=='0') || (mode[0]=='1') || (mode[0]=='3') || (mode[0]=='5') || (mode[0]=='6') )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02X", lenOfData/2);
		hsmCmdLen += 2;
	}

	// 交易数据
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

	// 与密码机通讯
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

	// 命令代码
	memcpy(hsmCmdBuf,"U0",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥
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

	// 处理明文数据
	aschex_to_bcdhex(plainData, lenOfPlainData, plainData);
	ret = UnionPBOCEMVFormPlainDataBlock(lenOfPlainData/2,(unsigned char *)plainData,(unsigned char *)plainData);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionHsmCmdU0:: UnionPBOCEMVFormPlainDataBlock!\n");
		return(ret);
	}

	// 明文数据长度
	sprintf(hsmCmdBuf+hsmCmdLen, "%04x", ret);
	hsmCmdLen += 4;

	// 明文数据
	memcpy(hsmCmdBuf+hsmCmdLen, plainData, ret);
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
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
   函数功能：
   UG指令，请求转换RSA私钥从LMK到KEK,LMK保护的RSA私钥为REF结构,KEK使用Mode定义的算法保护REF结构的RSA私钥的每个元素。
   输入参数：
mode: 加密私钥每个元素的加密算法 "00"-DES_ECB, "01"-DES_ECB_LP, "02"-DES_ECB_P,
"10"-DES_CBC, "11"-DES_CBC_LP, "12"-DES_CBC_P
keyType：密钥类型
key: 该密钥可由HSM内安全非易失存储区内密钥以及由LMK加密的密文送入
iv: 初始向量, mode为CBC编码时，该域存在。
vkLength: 私钥数据的数据长度
vk: nB 由LMK保护的RSA私钥（REF结构）
输出参数：
vkByKey: key加密的vk数据串

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

	// 与密码机通讯
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
   函数功能:
   TK指令，私钥加密
   输入参数:
encryMode: 1H 算法标识 1－解密数据 2－加密数据 3－签名 4－加密密钥 5－解密密钥
vkLength: 送入私钥长度
vk: nB/1A+3H DER编码的私钥或私钥在HSM安全存储区内的索引
keyLength: 1H 待加/解密密钥长度，0 = 单长度密钥，1 = 双长度密钥；encryMode = 4 或 encryMode = 5时该域存在
inputDataLength: 加解密数据长度
inputData: nB/1A+3H 待加解密数据或待加解密密钥在HSM安全存储区内的索引
输出参数:
outputData: nB 加解密/加解密密钥数据

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

	// 与密码机通讯
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
   函数功能:
   产生随机数
   输入参数:
   lenOfRandomData 随机数长度
   输出参数:
   randomData 随机数

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
   函数功能:
   产生分散/过程密钥
   输入参数:
   mode 算法标志:
   "00" - EMV2000
   "01" - DES_CBC
   "02" - DES_ECB
   "10" - VISA
   "11" - EMV2000
   masterKeyType	种子密钥类型
   masterKey	种子密钥
   desKeyType	子密钥类型
   keyLengthFlag	待分散主密钥长度	
   0 = 单长度密钥
   1 = 双长度密钥
   data		离散数据
   masterKey	种子密钥
   iv		向量
   输出参数:
   key	子密钥密文

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

	// 模式
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 2);
	hsmCmdLen += 2;

	// 种子密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(masterKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// 子密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(desKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// 密钥长度
	memcpy(hsmCmdBuf + hsmCmdLen, keyLengthFlag, 1);
	hsmCmdLen += 1;

	// 分散数据长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%03d", (int)strlen(data)/2);
	hsmCmdLen += 3;

	// 分散数据
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
   函数功能:
   将一把密钥加密的数据转换为另一把密钥加密
   输入参数:
   srcKeyType			 源密钥的类型
   srcKey	 1A+3H/16H/32H/48H       源密钥
   srcModel				源加密算法
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4（CBC模式，强制填充X80）
   "21"=VISA/PBOC（带长度指引的ECB）
   dstKeyType			 目的密钥类型
   dstKey	 1A+3H/16H/32H/48H       目的密钥
   dstModel				目的加密算法
   "00"=DES_ECB
   "02"=DES_ECB_P
   lenOfSrcCiphertext		 源密钥加密的密文数据长度
   srcCiphertext   nB		 源密钥加密的密文数据
   iv_cbc	 16H		初始向量。源加密算法为CBC编码时，该域存在
   输出参数:
   dstCiphertext   nB		 目的密钥加密的密文数据

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

	/* 与密码机通讯 */
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
   函数功能：
   UD指令，增加终端控制命令，管理员在授权状态下操作，实现对密钥的恢复或备份功能。
   输入参数：
   algorithmFlag：	[1A]	算法标识，‘D’-des算法，‘S’-SM4算法，默认为'D’
modeFlag:       [1N]		模式标识：
1-恢复密钥 2-备份密钥
mkIndex:	[1A+3H] 密钥位置：      用来产生卡片密钥的发卡行主密钥。
使用LMK对28-29相应变种加密。
1A+3H表示使用K+3位索引方式读取加密机内保存密钥。
（暂时先实现1A+3H取密钥）
keyLocale       [32H/1A+32H]
key:	[32H]		  密钥
仅当模式标志为1时有，LMK04_05加密


输出参数：
encryKey:       [N*8B/N*16B/N*24B]      LMK对加密的密钥密文(lmk04_05加密)
encryKeyLen:			密钥长度
checkValue:     [16H]		  用密钥加密0的结果

 */

int UnionHsmCmdUD(char *algorithmFlag,char *modeFlag,char *mkIndex,char *keyLocale,
		char *key,char *encryKey,int *pencryKeyLen,char *checkValue)
{
	int     ret;
	int     hsmCmdLen = 0;
	int     offset,encryKeyLen;
	char    szMkBuff[128];
	char    hsmCmdBuf[1024];

	// 命令代码
	memcpy(hsmCmdBuf,"UD",2);
	hsmCmdLen += 2;

	UnionSetBCDPrintTypeForHSMCmd();

	// 算法标识
	if ((algorithmFlag != NULL) && (algorithmFlag[0] != 'D'))
	{
		memcpy(hsmCmdBuf+hsmCmdLen, algorithmFlag, 1);
		hsmCmdLen += 1;
	}

	// 模式标识
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

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdUD:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 4;

	// 密文数据
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
   输入参数
   mode 加密模式标识 0=离散密钥3DES加密
   1=离散密钥3DES解密
   2=做1DES_MAC（密钥的左8字节）(ANSI X9.19 MAC)
   3=做3DES_MAC(ANSI X9.19 MAC)
   4=离散密钥DES加密
   5=离散密钥DES解密
   6=过程密钥DES加密
   7=过程密钥DES解密
   id 方案ID 加解密算法模式：
   01=ECB
   02=CBC
   03=CFB
   04=OFB
   mkType 根密钥类型，产生卡片密钥的发卡行密钥类型：
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   lenOfData 数据长度
   data 数据
   输出参数
   criperDataLen 数据长度  
   criperData 数据   
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

	// 命令代码
	memcpy(hsmCmdBuf,"U1",2);
	hsmCmdLen += 2;

	// 加密模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 2);
	hsmCmdLen += 2;

	// 根密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	if ('A' == mode[0] || 'B' == mode[0] || 'C' == mode[0] || 'D' == mode[0])
	{
		// 离散数据
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*32);
		hsmCmdLen += mkDvsNum*32;
	}
	else
	{
		// 离散数据
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
		hsmCmdLen += mkDvsNum*16;
	}

	// 过程数据
	if ('6' == mode[0] || '7' == mode[0] || '8' == mode[0] || '9' == mode[0] || 'C' == mode[0] || 'D' == mode[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 16);
		hsmCmdLen += 16;
	}
	// 数据填充标识
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

	// 数据长度
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	// 数据
	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 奇偶校验标识 add by wuhy at 20140903
	if ('A' == mode[0] || 'B' == mode[0] || 'C' == mode[0] || 'D' == mode[0])
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "N", 1);
		hsmCmdLen += 1;
	}

	// 与密码机通讯
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
   输入参数
   mode 模式标识 1=计算MAC
   2=校验MAC
   1=计算C-MAC
   id 方案ID
   0=3DESMAC（使用子密钥进行3DESMAC）
   1=DESMAC（使用过程密钥进行DESMAC）
   2=TAC(使用子密钥进行DESMAC)
   3=3DESMAC（使用2倍长过程密钥进行3DESMAC）
   mkType 根密钥类型 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   macFillFlagMAC 数据填充标识
   1=强制填充0X80
   0=不强制填充0X80
   ivMac IV-MAC 
   lenOfData MAC计算数据长度
   data MAC计算数据
   macFlagMAC 长度标识   
   1=4 BYTE
   2=8 BYTE
   输出参数
   checkMac
   mac MAC值

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

	// 命令代码
	memcpy(hsmCmdBuf,"UB",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf + hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥类型
	memcpy(hsmCmdBuf + hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
	hsmCmdLen += mkDvsNum*16;

	// 过程数据
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
	// MAC数据填充标识
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
	// MAC计算数据长度
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
		// add by chenwd 20150911 兼容交通部指令
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

	// 与密码机通讯
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
   输入参数
   mode 模式标识 
   2=校验MAC
   1=计算C-MAC
   id 方案ID
   0=3DESMAC（使用子密钥进行3DESMAC）
   1=DESMAC（使用过程密钥进行DESMAC）
   2=TAC(使用子密钥进行DESMAC)
   mk 根密钥
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据 
   macFillFlag MAC数据填充标识
   1=强制填充0X80
   0=不强制填充0X80
   ivMac IV-MAC 
   lenOfData MAC计算数据长度
   data MAC计算数据
   macFlag MAC长度标识
   1=4 BYTE
   2=8 BYTE
   输出参数
   checkMac
   mac MAC值

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

	// 命令代码
	memcpy(hsmCmdBuf,"U3",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	if ('4' == id[0] || '5' == id[0])
	{
		// 离散数据
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*32);
		hsmCmdLen += mkDvsNum*32;
	}
	else
	{
		// 离散数据
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
		hsmCmdLen += mkDvsNum*16;
	}

	// 过程数据
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

	// MAC数据填充标识
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
	// MAC计算数据长度
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

	// 与密码机通讯
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
   输入参数
   mode 模式标志
   1-加密并计算密钥校验值(checkvalue)
   id 方案ID
   0=DES_ECB(直接加密，不需填充)
   1=DES_CBC(直接加密，不需填充)
   mkType 根密钥类型
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkDvsNum 离散次数
   mkDvsData 离散数据
   pkType 保护密钥类型
   0=TK(ZEK)（传输密钥）
   1=ZMK
   pk 保护密钥 
   ivCbc IV-CBC，仅当“方案ID”为1时有
   输出参数
   keyCheckValue 密钥校验值
   keyValue 密文数据

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

	// 命令代码
	memcpy(hsmCmdBuf,"UK",2);
	hsmCmdLen += 2;

	// 安全机制
	if (securityMech != NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
		hsmCmdLen += 1;
	}
	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*32);
	hsmCmdLen += mkDvsNum*32;

	// 保护密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 1);
	hsmCmdLen += 1;

	// 保护密钥
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

	// 与密码机通讯
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
   功能：将安全报文保护的密钥导入到加密机中（EMV 4.1/PBOC）
   输入参数
   mode 模式标志 
   1-验证MAC并解密
   id 方案ID 
   0=M/Chip4（CBC模式，强制填充X80）
   1=VISA/PBOC（带长度指引的ECB）
   mkFlag 存储标志 
   1-将密钥存储到加密机中
   mkIndex 存储索引
   mkLengthFlag 密钥方案，指定导入密钥类型，以确定加密的LMK：
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN

   pkType 保护密钥类型
   0=TK（传输密钥）
   1=DK-SMC（使用MK-SMC实时离散生成的子密钥）
   pk 保护密钥 
   pkDvsNum 保护密钥离散次数
   pkDvsData 保护密钥离散数据 
   ivCbc IV-CBC
   criperDataLen 密文长度 
   criperData 密文数据
   mac MAC
   ivMac IV-MAC 
   macDataLen MAC填充数据长度
   macData MAC填充数据 
   macOffset 偏移量
   输出参数
   keyValue LMK保护的密钥密钥校验值
   checkValue 密钥校验值

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

	// 命令代码
	memcpy(hsmCmdBuf,"U4",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen,mode,1);
	hsmCmdLen += 1;
	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen,id,1);
	hsmCmdLen += 1;
	// 存储标志
	memcpy(hsmCmdBuf+hsmCmdLen,mkFlag,1);
	hsmCmdLen += 1;
	// 存储索引
	sprintf(hsmCmdBuf+hsmCmdLen,"K%03X", mkIndex);
	hsmCmdLen += 4;
	// 密钥方案
	memcpy(hsmCmdBuf+hsmCmdLen,mkType,3);
	hsmCmdLen += 3;
	// 输出密钥模式
	memcpy(hsmCmdBuf+hsmCmdLen,mkLengthFlag,1);
	hsmCmdLen += 1;

	// 保护密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen,pkType,1);
	hsmCmdLen += 1;

	// 保护密钥
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
	// 保护密钥离散次数, 保护密钥离散数据
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
	// 密文长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02X",criperDataLen);
	hsmCmdLen += 2;
	// 密文数据
	memcpy(hsmCmdBuf+hsmCmdLen,criperData,criperDataLen);
	hsmCmdLen += criperDataLen;
	// MAC
	memcpy(hsmCmdBuf+hsmCmdLen,mac,8);
	hsmCmdLen += 8;

	// IV-MAC, MAC填充数据长度, MAC填充数据, 偏移量
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

	// 与密码机通讯
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
   功能
   加解密数据
   输入参数：
   encryFlag：1－解密；2－加密
   mode：01－DES_ECB；02－DES_CBC
   keyType：密钥类型，000：ZMK；00A：ZAK
   keyLen：密钥长度，0 = 单长度密钥；1 = 双长度密钥
   key：加解密密钥
   iv：CBC模式时使用的初始化向量
   lenOfData：需要做摘要的数据长度
   pData：需要做摘要的数据
   输出参数
   pCipherDataLen：密文数据长度
   pCipherData：密文数据

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
		//ECB 模式 对数据进行填充
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

	/* 与密码机通讯 */
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
   函数功能:
   将数据有keyA加密转加密为keyB加密
   输入参数:
   srcKeyType			 源密钥的类型
   srcKey	 1A+3H/16H/32H/48H       源密钥
   srcModel				源加密算法
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4（CBC模式，强制填充X80）
   "21"=VISA/PBOC（带长度指引的ECB）
   srcIV	  16H		初始向量。源加密算法为CBC编码时，该域存在
   dstKeyType			 目的密钥类型
   dstKey	 1A+3H/16H/32H/48H       目的密钥
   dstModel				目的加密算法
   "00"=DES_ECB
   "02"=DES_ECB_P
   dstIV	  16H		初始向量。目的加密算法为CBC编码时，该域存在
   lenOfSrcCiphertext		 源密钥加密的密文数据长度
   srcCiphertext   nH		 源密钥加密的密文数据

   输出参数:
   dstCiphertext   nH		 目的密钥加密的密文数据

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

	/* 与密码机通讯 */
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
   功能
   将密钥由KEK加密转为LMK加密
   输入参数：
   mode：算法标识
   00－DES_ECB
   01－DES_ECB_LP
   02－DES_ECB_P
   10－DES_CBC
   11－DES_CBC_LP
   12－DES_CBC_P
   kekType：密钥类型，000：ZMK；
kekLen:密钥长度，0 = 单长度密钥，1 = 双长度密钥
kek：密钥加密密钥
iv：CBC模式时使用的初始化向量
keyType：密钥类型，000：ZMK；
lenOfKeyCipherTextByKek：需要做摘要的数据长度
keyCipherTextByKek：需要做摘要的数据
输出参数
keyCipherTextByLmk：密文数据

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

	/* 与密码机通讯 */
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

	// 命令字
	memcpy(hsmCmdBuf,"PE",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,"C",1);
	hsmCmdLen++;
	// 12位客户帐号

	memcpy(hsmCmdBuf+hsmCmdLen,accNo,12);
	hsmCmdLen += 12;

	// PIN密文
	strcpy(hsmCmdBuf+hsmCmdLen,pinByLmk);
	hsmCmdLen += strlen(pinByLmk);

	// 域
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
	//日期
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

/* 输入参数
   algorithmID	算法标识，0:ECB	1:CBC
   keyType		密钥类型
   zek		保护密钥
   iv		iv
   keyByZek	密钥密文
   keyFlag		密钥方案X/U
   sizeOfBuf	接收keyByLmk结果的缓冲大小
   输出参数
   keyByLmk	密钥密文
   checkValue	检验值
 */
int UnionHsmCmdM8(char *algorithmID,TUnionDesKeyType keyType,char *zek,char *iv,char *keyByZek,char *keyFlag,char *keyByLmk,char *checkValue,int sizeOfBuf)
{
	int			ret;
	char			hsmCmdBuf[8192];
	int			hsmCmdLen = 0;

	// 指令代码
	memcpy(hsmCmdBuf,"M8",2);
	hsmCmdLen = 2;

	// 算法标识
	memcpy(hsmCmdBuf + hsmCmdLen,algorithmID,1);
	hsmCmdLen += 1;

	// 密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// 密钥
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

	// 密钥密文
	memcpy(hsmCmdBuf + hsmCmdLen,keyByZek,32);
	hsmCmdLen += 32;

	// LMK加密密钥方案
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
   作用：M1加解密数据
输入: 
 */

int UnionHsmCmdM1(char *algorithmID,TUnionDesKeyType keyType, char *zek, char *iv, char *inData, int dataLen, char * encryptData, int sizefEncryptDaga)
{
	int     ret;
	char    hsmCmdBuf[8192];
	char		tmpBuf[8];
	int     hsmCmdLen = 0;
	int			dataLenTmp=0;

	// 指令代码
	memcpy(hsmCmdBuf,"M1",2);
	hsmCmdLen = 2;

	// 算法标识
	memcpy(hsmCmdBuf + hsmCmdLen,algorithmID,1);
	hsmCmdLen += 1;

	// 密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// 密钥
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

	//数据长度，为16的倍数
	if (dataLen % 16 || dataLen > 4000)
	{
		UnionUserErrLog("in UnionHsmCmdM1::dataLen illegal [%d]!\n", dataLen);
		return(errCodeParameter);
	}


	// 数据加密
	if (algorithmID[0] == '0' || algorithmID[0] == '2')
	{
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%04d", dataLen);
		dataLenTmp = dataLen *2;
		memcpy(hsmCmdBuf + hsmCmdLen, inData, dataLen);
		hsmCmdLen += dataLen;
	}
	else//解密数据为BCD编码，需要压缩
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

////////////////////以下为pboc30增加的指令////////////////////

// 导入SM2密钥对
/* 输入参数
   index		密钥索引
   lenOfVK	密钥密文字节数
   VK		密钥密文
   输出参数
   无
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

//导出SM2私钥
/*输入参数
  index		密钥索引
  isOutPutPK	是否需要导出公钥, 0或者无该域：表示不需要导出公钥, 1：表示需要导出公钥
  输出参数
  lenOfVK		密钥密文字节数
  VK		密钥密文
  PK		公钥值
 */
int UnionHsmCmdK8(int index,char *isOutPutPK,int *lenOfVK,unsigned char *VK,unsigned char *PK)
{
	int	ret;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"K8",2);
	hsmCmdLen = 2;
	//密钥索引
	sprintf(hsmCmdBuf+hsmCmdLen ,"%02d",index);
	hsmCmdLen += 2;

	// 是否需要导出公钥
	if (strcmp(isOutPutPK, "1") == 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen ,"%s",isOutPutPK);
		hsmCmdLen += 1;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	//与密码机通讯
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

//导出SM2私钥（TK保护）
/*输入参数
  SM2Index	密钥索引
  lenOfSM2	外带密钥密文长度
  SM2		外带密钥密文
  keyType		密钥类型
  Key		主密钥加密下的传输(SM4)密钥
  checkValue	传输密钥校验值
  输出参数
  lenOfVK		密钥密文字节数
  VK		TK加密下的SM2私钥密文

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
		//密钥索引
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// 外带密钥长度
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfSM2);
		hsmCmdLen += 4;
		// 外带密钥
		memcpy(hsmCmdBuf + hsmCmdLen,SM2,lenOfSM2);
		hsmCmdLen += lenOfSM2;
	}
	else
	{
		//密钥索引
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",SM2Index);
		hsmCmdLen += 2;
	}
	//密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(keyType,tmpBuf);
	tmpBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, tmpBuf, 3);
	hsmCmdLen += 3;
	//LMK加密的传输（SM4）密钥
	memcpy(hsmCmdBuf+hsmCmdLen,Key,32);
	hsmCmdLen += 32;
	//密钥校验值
	memcpy(hsmCmdBuf+hsmCmdLen,checkValue,16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = 0;

	//与密码机通讯
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

	// 模式标识
	if (mode == 'N')
	{
		hsmCmdBuf[hsmCmdLen] = mode;
		hsmCmdLen++;
	}

	if(SM2Index < 0)
	{
		//密钥索引
		memcpy(hsmCmdBuf + hsmCmdLen,"99",2);
		hsmCmdLen += 2;
		// 外带密钥长度
		sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfSM2);
		hsmCmdLen += 4;
		// 外带密钥
		memcpy(hsmCmdBuf + hsmCmdLen,SM2,lenOfSM2);
		hsmCmdLen += lenOfSM2;
	}
	else
	{
		//密钥索引
		sprintf(hsmCmdBuf + hsmCmdLen,"%02d",SM2Index);
		hsmCmdLen += 2;
	}

	if (mode == 'N')
	{
		//公钥X
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPK/2);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf + hsmCmdLen, pk, lenOfPK/2);
		hsmCmdLen += lenOfPK/2;

		//公钥Y
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPK/2);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf + hsmCmdLen, pk + lenOfPK/2, lenOfPK/2);
		hsmCmdLen += lenOfPK/2;

		hsmCmdBuf[hsmCmdLen] = '3';
		hsmCmdLen++;
	}

	//密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(keyType,tmpBuf);
	tmpBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, tmpBuf, 3);
	hsmCmdLen += 3;
	//LMK加密的传输（SM4）密钥
	memcpy(hsmCmdBuf+hsmCmdLen,Key,32);
	hsmCmdLen += 32;
	//密钥校验值
	memcpy(hsmCmdBuf+hsmCmdLen,checkValue,16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = 0;

	//与密码机通讯
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


// SM4算法加解密数据
/* 输入参数
   encryptFlag		加解密标识
   1——解密
   2——加密
mode:
01 –SM4-ECB
02 --SM4-CBC
03 --SM4-CFB
04 --SM4-OFB
keyType			支持的密钥类型为ZEK/ZMK
lenOfKey			1 = 双长度密钥
Key		‘S’表示SM4密钥,该密钥由LMK加密的密文送入
checkValue			Key校验值
iv			初始向量。 Mode=02和，该域存在。
lenOfDate			加解密数据长度，长度必须是16的倍数
data			待加解密数据
输出参数
value			加解密后的数据
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

	//读取加解密标识
	sprintf(hsmCmdBuf+hsmCmdLen ,"%01d",encryptFlag);
	hsmCmdLen += 1;

	// add by leipp 20150116
	UnionSetBCDPrintTypeForHSMCmd();
	if (encryptFlag == 2)
		UnionSetMaskPrintTypeForHSMReqCmd();
	else
		UnionSetMaskPrintTypeForHSMResCmd();
	// end

	//加解密模式
	memcpy(hsmCmdBuf+hsmCmdLen,mode,2);
	hsmCmdLen += 2;

	//密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	//1 = 双长度密钥
	sprintf(hsmCmdBuf+hsmCmdLen ,"%01d",lenOfKey);
	hsmCmdLen += 1;

	//LMK加密的密钥
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,Key,32);
	hsmCmdLen += 32;

	//Key校验值
	memcpy(hsmCmdBuf+hsmCmdLen,checkValue,16);
	hsmCmdLen += 16;

	//	if(strcmp(mode, "02") == 0)
	if((strcmp(mode, "02") == 0)||(strcmp(mode, "03") == 0)||(strcmp(mode, "04") == 0))//changed by lusj 20151031

	{
		memcpy(hsmCmdBuf+hsmCmdLen,iv,32);
		hsmCmdLen += 32;
	}

	//待加、解密数据
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


//脚本数据加密命令（EMV 4.1/PBOC）
/*输入参数
  mode			模式
  id			方案ID
  mkIndex			MK-SMC索引
  mk			MK-SMC
  checkMk			MK-SMC校验值
  pan			PAN/PAN序列号
  atc			ATC
  iv			模式标志位0时有此域
  lenOfPlainData		明文数据长度
  plainData		铭文数据
  输出参数
  lenOfCipherData		密文数据长度
  cipherData		密文数据
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

	// 命令代码
	memcpy(hsmCmdBuf,"WB",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
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

	// 处理明文数据
	aschex_to_bcdhex(plainData, lenOfPlainData, plainDataTmp);
	ret = UnionPBOCEMVFormPlainDataBlockForSM4(lenOfPlainData/2, (unsigned char *)plainDataTmp, (unsigned char *)plainDataTmp);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWB:: UnionPBOCEMVFormPlainDataBlockForSM4!\n");
		return(ret);
	}

	// 明文数据长度
	sprintf(hsmCmdBuf+hsmCmdLen, "%04x", ret);
	hsmCmdLen += 4;

	// 明文数据
	memcpy(hsmCmdBuf+hsmCmdLen, plainDataTmp, ret);
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = 0;

	UnionSetBCDPrintTypeForHSMCmd();
	// 与密码机通讯
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


// 生成密钥SM4密钥
/* 输入参数
mode:
0－产生密钥
1－产生密钥并在ZMK下加密
keyType		密钥类型
zmk			仅当模式为1时才显示该ZMK域
输出参数
keyByLMK		LMK保护的密钥密文值
keyByZMK		ZMK保护的密钥密文值
checkValue		密钥校验值
 */
int UnionHsmCmdWI(char *mode, TUnionDesKeyType keyType, char *zmk, char *keyByLMK,char *keyByZMK, char *checkValue)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		offset;

	memcpy(hsmCmdBuf,"WI",2);
	hsmCmdLen = 2;

	//模式
	memcpy(hsmCmdBuf+hsmCmdLen,mode,1);
	hsmCmdLen += 1;

	//密钥类型
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


//生成SM4密钥并打印一个成份 
/* 输入参数
   keyType		密钥类型
   fldNumi		打印域个数
   fld		打印域
   输出参数
   component	成分
 */
int UnionHsmCmdWJ(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component)
{
	int     ret;
	char    hsmCmdBuf[1024];
	int     hsmCmdLen = 0;
	int     i;

	memcpy(hsmCmdBuf,"WJ",2);
	hsmCmdLen = 2;
	//密钥类型
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
	//打印域
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

	//与服务器通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWJ:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//提取成分
	memcpy(component,hsmCmdBuf+4+1,32);
	component[32] = 0;
	return(32);
}

//生成SM4密钥并以分开的成份形式打印
/* 输入参数          
   keyType         密钥类型
   fldNumi         打印域个数
   fld             打印域
   输出参数
   component       成分
   checkValue	密钥校验值
 */              
int UnionHsmCmdWK(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component,char *checkValue)
{       
	int     ret;
	char    hsmCmdBuf[1024];
	int     hsmCmdLen = 0;
	int     i; 

	memcpy(hsmCmdBuf,"WK",2);
	hsmCmdLen = 2;
	//密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWK:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	//打印域
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

	//与服务器通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdLen = 4+1;
	//提取成分
	memcpy(component,hsmCmdBuf+hsmCmdLen,32);
	hsmCmdLen += 32;
	memcpy(checkValue,hsmCmdBuf+hsmCmdLen,16);
	return(16);
}


/*
   输入参数
   mode 加密模式标识
   G=离散密钥SM4加密
   H=离散密钥SM4解密
   I=过程密钥SM4加密
   J=过程密钥SM4解密
   id 方案ID
   01=ECB
   02=CBC
   03=CFB
   mkType 根密钥类型
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkCheckValue 根密钥校验值
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   lenOfData 数据长度
   data 数据
   输出参数
   criperDataLen 数据长度  
   criperData 数据   
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

	// 命令代码
	memcpy(hsmCmdBuf,"WC",2);
	hsmCmdLen += 2;

	// 加密模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 2);
	hsmCmdLen += 2;

	// 根密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
	hsmCmdLen += mkDvsNum*16;

	// 过程数据
	if ('I' == mode[0] || 'J' == mode[0])
	{
		aschex_to_bcdhex(proFactor,strlen(proFactor),hsmCmdBuf + hsmCmdLen);	
		hsmCmdLen += strlen(proFactor)/2;

	}

	// 数据长度
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	// 数据
	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;

	hsmCmdBuf[hsmCmdLen] = 0;
	// 与密码机通讯
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
   函数功能：
   WH离散卡片密钥并以安全报文方式导出（PBOC3.0）
   输入参数：
securityMech: 安全机制(R：SM4加密和MAC)
mode: 模式标志, 0-仅加密 1-加密并计算MAC
id: 方案ID, 0=PBOC3.0模式（CBC模式，强制填充X80） 1= PBOC3.0模式（带长度指引的ECB） 2=PBOC3.0模式(ECB模式，外带填充数据)

mk: 根密钥
mkType: 根密钥类型109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
当使用加密机内密钥时该域不起作用
mkIndex: 根密钥索引, K+3位索引方式读取加密机内保存密钥
mkCheckValue: 根密钥校验值
mkDvsNum: 根密钥离散次数, 1-3次
mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

pkType: 保护密钥类型 0=TK(传输密钥)1=DK-SMC(使用MK-SMC实时离散生成的子密钥)
pk: 保护密钥
pkIndex: 保护密钥索引
pkDvsNum: 保护密钥离散次数,仅当“保护密钥类型”为1时有,从MK-SMC离散得到DK-SMC的离散次数,范围为1-3
pkDvsData: 保护密钥离散数据,n*16H仅当“保护密钥类型”为1时有,保护密钥的离散数据，其中n为“保护密钥离散次数”
pkCheckValue: 保护密钥校验值

proKeyFlag:过程密钥标识，Y:计算过程密钥　N:不计算过程密钥　可选项:当没有该域时缺省为N
proFactor: 过程因子(32H),可选项:仅当过程密钥标志为Y时有

ivCbc: IV-CBC,32H 仅当“方案ID”为0时有

encryptFillDataLen: 加密填充数据长度(4H),仅当“方案ID”为2时有（不大于1024）
和密钥明文一起进行加密的数据长度
encryptFillData: 加密填充数据 nB 仅当“方案ID”为2时有,和密钥明文一起进行加密
encryptFillOffset: 加密填充数据偏移量 4H 仅当“方案ID”为2时有
将密钥明文插入到加密填充数据的位置, 数值必须在0到加密填充数据长度之间

ivMac: IV-MAC,32H 仅当“模式标志”为1时有
macDataLen: MAC填充数据长度 4H 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据长度
macData: MAC填充数据 nB 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据
macOffset: 偏移量 4H 仅当“模式标志”为1时有,将密钥密文插入到MAC填充数据的位置
数值必须在0到MAC填充数据长度之间
输出参数：
mac: MAC值 16H 仅当“模式标志”为1时有
criperDataLen: 密文数据长度 4H 密文数据长度(必须是8的倍数，并等于前缀长度、后缀长度、密钥长度的和)
criperData: nB 输出的密文数据
checkValue: 16H 校验值

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


	// 命令代码
	memcpy(hsmCmdBuf,"WH",2);
	hsmCmdLen += 2;

	// 安全机制
	memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
	hsmCmdLen += 1;
	if (securityMech[0] == 'Q')
		lenOfDvsData = 32;
	else
		lenOfDvsData = 16;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// 保护密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 1);
	hsmCmdLen += 1;

	// 保护密钥
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
	// 保护密钥离散次数, 保护密钥离散数据
	if (pkType[0] == '1')
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
		hsmCmdLen += 1;

		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
		hsmCmdLen += pkDvsNum*lenOfDvsData;
	}

	// modify by lix,20090730
	// 过程密钥标识
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// 过程因子
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

	// 加密填充数据长度、加密填充数据、加密填充数据偏移量
	if (id[0] == '2')
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, encryptFillData, encryptFillDataLen);
		hsmCmdLen += encryptFillDataLen;

		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillOffset/2);
		hsmCmdLen += 4;
	}

	// IV-MAC, MAC填充数据长度, MAC填充数据, 偏移量
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

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWH:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (mode[0] == '1') // MAC值
	{
		memcpy(mac, hsmCmdBuf+offset, 16);
		mac[16] = 0;
		offset += 16;
	}
	else if (mode[0] == '2') //模式2,返回校验值
	{
		offset += 32;
		memcpy(checkValue, hsmCmdBuf + ret - 32, 16);
		checkValue[16] = 0;
	}

	// 密文数据长度
	offset += 4;
	*criperDataLen = ret - offset;

	// 密文数据
	//memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
	// add by wuhy at 20140826
	if (mode[0] == '1')
	{
		// 密文数据
		memcpy(criperData, hsmCmdBuf + 24, *criperDataLen);
		criperData[*criperDataLen] = 0;
		UnionLog("in UnionHsmCmdWH::*criperDataLen=[%d]\n",*criperDataLen);
	}
	else
	{
		// 密文数据
		memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
		criperData[*criperDataLen] = 0;
		UnionLog("in UnionHsmCmdWH::*criperDataLen=[%d]\n",*criperDataLen);
	}
	// end add

	return(0);
}
/*
   函数功能：
   WD:: 计算及校验MAC/TAC（原UB）
   输入参数
   mode 模式标识 1=计算MAC 2=校验MAC
   id 方案ID
   4=SM4MAC（使用2倍长过程密钥进行SM4）
   mkType 根密钥类型 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkCheckValue 根密钥校验值 
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   macFillFlagMAC 数据填充标识
   1=强制填充0X80
   0=不强制填充0X80
   ivMac IV-MAC 
   lenOfData MAC计算数据长度
   data MAC计算数据
   macFlagMAC 长度标识   
   1=4 BYTE
   2=8 BYTE
   输出参数
   checkMac
   mac MAC值
   返回值：
   成功：>=0

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

	// 命令代码
	memcpy(hsmCmdBuf,"WD",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf + hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥类型
	memcpy(hsmCmdBuf + hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

		//根密钥校验值
		memcpy(hsmCmdBuf + hsmCmdLen, mkCheckValue, 16);
		hsmCmdLen += 16;	
	}


	// 离散次数
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, mkDvsNum*16);
	hsmCmdLen += mkDvsNum*16;

	// 过程数据 
	if(id[0] != '1') // 方案ID＝1没有该域 2016-05-06
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

	// MAC数据填充标识
	memcpy(hsmCmdBuf + hsmCmdLen, macFillFlag, 1);
	hsmCmdLen += 1;

	// IV-MAC
	memcpy(hsmCmdBuf+ hsmCmdLen, ivMac, 32);	
	hsmCmdLen += 32;

	// MAC计算数据长度
	sprintf(lenBuf, "%03d", lenOfData/2);
	lenBuf[3] = 0;
	memcpy(hsmCmdBuf + hsmCmdLen, lenBuf, 3);
	hsmCmdLen += 3;

	//MAC计算数据
	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;

	//MAC长度标识
	if (macFlag[0] != '1' && macFlag[0] !='2')
	{
		UnionUserErrLog("in UnionHsmCmdWD::macFlag is error!macFlag = [%s]\n",macFlag);
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf + hsmCmdLen, macFlag, 1);
	hsmCmdLen += 1;

	//待校验的MAC值
	if ('2' == mode[0])
	{
		memcpy(hsmCmdBuf + hsmCmdLen, checkMac, 16*atoi(macFlag));
		hsmCmdLen += 16*atoi(macFlag);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
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
   函数功能:
   WE::  产生分散/过程密钥
   输入参数:
   mode 算法标志:
   "01" - SM4_CBC
   "02" - SM4_ECB
   masterKeyType   种子密钥类型
   masterKey       种子密钥
   desKeyType      子密钥类型
   keyLengthFlag   待分散主密钥长度   1 = 双长度密钥
   data            离散数据
   masterKey      	种子密钥 
   masterKeyIndex  K+3位索引方式读取加密机内保存密钥
   masterKeyCheckValue 种子密钥校验值
   iv              向量
   输出参数:
   key     子密钥密文
   返回值：
   成功：>=0

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
	//命令码
	memcpy(hsmCmdBuf, "WE", 2);
	hsmCmdLen = 2;

	// 模式
	memcpy(hsmCmdBuf + hsmCmdLen, mode, 2);
	hsmCmdLen += 2;

	// 种子密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(masterKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// 子密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(desKeyType, keyType);
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// 密钥长度
	memcpy(hsmCmdBuf + hsmCmdLen, keyLengthFlag, 1);
	hsmCmdLen += 1;

	// 分散数据长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%03d", (int)strlen(data)/2);
	hsmCmdLen += 3;

	// 分散数据
	aschex_to_bcdhex(data,strlen(data),hsmCmdBuf + hsmCmdLen);	
	hsmCmdLen += strlen(data)/2;

	// 主密钥
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

		//主密钥校验值 
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
   函数功能:
   WF::  将一把密钥加密的数据转换为另一把密钥加密
   输入参数:
   algType	算法转换类型
01:3DES算法转SM4
02:SM4算法转3DES
03:SM4算法转SM4
srcKeyType             源密钥的类型
srcKey                 源密钥
srcKeyCheckValue       源密钥校验值
srcModel               源加密算法
"00"=ECB
"01"=ECB_LP
"02"=ECB_P
"10"=CBC
"11"=CBC_LP
"12"=CBC_P
dstKeyType                            目的密钥类型
dstKey    1A+3H/16H/32H/48H       	目的密钥
dstKeyCheckValue              		目的密钥校验值 
dstModel                                目的加密算法
"00"=ECB
"02"=ECB_P
"10"=CBC
"12"=ECB_P
lenOfSrcCiphertext                    源密钥加密的密文数据长度
srcCiphertext   nB                    源密钥加密的密文数据
iv_cbc    16H                初始向量。源加密算法为CBC编码时，该域存在
iv_cbcDstKey 16H                     目的加密算法位cbc北嗦胧保糜虼嬖�

输出参数:
dstCiphertext   nB                    目的密钥加密的密文数据
返回值：
成功：>=0

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

	//命令码
	memcpy(hsmCmdBuf, "WF", 2);
	hsmCmdLen += 2;

	//算法转换类型	
	memcpy(hsmCmdBuf+hsmCmdLen, algType, 2);
	hsmCmdLen += 2;

	//源密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(srcKeyType, keyType);
	strncpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	//源密钥
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

		//源密钥校验值 
		if (strcmp(algType, "01") != 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, srcKeyCheckValue, 16);
			hsmCmdLen += 16;
		}	
	}

	//源加密模式
	memcpy(hsmCmdBuf+hsmCmdLen,srcModel,2);
	hsmCmdLen += 2;

	//目的密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(dstKeyType, keyType);
	strncpy(hsmCmdBuf+hsmCmdLen,keyType,3);
	hsmCmdLen += 3;

	//目的密钥
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

		//目的密钥校验值
		if (strcmp(algType, "02") != 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, dstKeyCheckValue, 16);
			hsmCmdLen += 16;
		}
	}

	//目的加密算法
	memcpy(hsmCmdBuf+hsmCmdLen,dstModel,2);
	hsmCmdLen += 2;

	//密文数据长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfSrcCiphertext/2);
	hsmCmdLen += 4;

	//密文数据
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

	/* 与密码机通讯 */
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
   RQC校验
   1=执行ARQC校验和ARPC产生
   2=仅仅执行ARPC产生
   id        方案ID   0=PBOC3.0(采用SM4算法计算)
   mkIndex           mk索引，如果mk值为空则使用索引方式
   mk                加密计算主密钥
   mkCheckValue      加密计算主密钥校验值
   pan               离散卡片密钥使用的帐号或者帐号序列号
   atc               使用ATC来计算过程密钥
   lenOfData         加密数据长度
   data             要加密的数据
   ARQC             ARQC
   ARC              ARC
   输出参数:
   ARPC             ARPC
   返回值：
   成功：>=0
 */
int UnionHsmCmdWG(char *mode, char *id, int mkIndex, char *mk, char* mkCheckValue, int lenOfPan, char *pan, char *atc, int lenOfData, char *data, char *ARQC, char *ARC, char *ARPC)
{
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	char keyString[64];
	int offset = 0;
	char    ARPCBuf[64];

	// 命令代码
	memcpy(hsmCmdBuf,"WG",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥
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

		//根密钥 校验值
		memcpy(hsmCmdBuf+hsmCmdLen, mkCheckValue, 16);
		hsmCmdLen += 16;
	}

	//PAN/PAN 序列号
	aschex_to_bcdhex(pan, 16, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 8;

	//ATC
	aschex_to_bcdhex(atc, 4, hsmCmdBuf + hsmCmdLen);	
	hsmCmdLen += 2;

	// 交易数据长度
	if ( (mode[0]=='0') || (mode[0]=='1') )
	{
		// modified 2011-11-08
		//sprintf(hsmCmdBuf+hsmCmdLen, "%2x", lenOfData/2);
		sprintf(hsmCmdBuf+hsmCmdLen, "%02X", lenOfData/2);
		hsmCmdLen += 2;

		// 交易数据
		ret = aschex_to_bcdhex(data, lenOfData, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += ret;
		//分隔符
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

	// 与密码机通讯
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
   功能： 
   RSA私钥解密数据转ZPK加密数据
   参数:   
   vkeyLen	私钥长度 私钥存储在加密机内部时，当私钥长度取0
   vkey	私钥只能为外部送入或者为HSM内安全存储区内密钥。
   dataPaddingMode 	� 00=非填充 01=PKCS#1 v1.5 02=OAEP
   dataLength  加解密数据长度	
   data 加解密数据
   keyType	ZMKTYPE=0;ZPKTYPE=1;TPKTYPE=2 
   Key  用对应的LMK密钥对解密
   Specialalgorithm     专用算法标识
   输出:
   pin 密文
 */
int UnionHsmCmd70(int vkeyLen, char *vkey, int vkIndex, int dataPaddingMode, int dataLength, char* data,  int keyType, char* key, char* Specialalgorithm, char *pin)
{
	/*未测试*/
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

	if(vkeyLen == 0) //使用私钥索引
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
   功能：
   把zpk加密的字符密码密文转换为另一zpk加密的字符密码密文
   参数:
   srcAlgorithmID 源密钥算法标识，0代表国际,1代表国密
   srcZPK 当前加密PIN块的ZPK；LMK对（06-07）下加密
   dstAlgorithmID 目的密钥算法标识，0代表国际,1代表国密
   srcPin 源ZPK加密的字符密码密文
   srcFormat 源PIN格式	,01-ANSI9.8
			 09-农商行格式
   dstFormat 目的PIN格式,01-ANSI9.8
			 09-农商行格式
   srcPan 源12位有效主账号
   dstPan 目的12位 有效主账号
   输出:
   pin 返回PIN密文 
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
   功能：8/R9生成随机数

   输入参数: 
   genKeyLen 	指定产生口令的长度
   genKeyType 口令的复杂度		1：纯数字 2、纯字母 3、数字+字母

   输出参数：
   outPutKey  产生的口令 

   返回值：
   产生的口令的长度
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
   Mode    1       H       0－产生密钥
   2－加密密钥
   Key type        3       H       “000”
   Key length      1       H       0 = 单长度密钥
   1 = 双长度密钥
   Key Plain       16H/32H H       明文密钥。Mode＝2时，该域存在。
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

	// 与密码机通讯
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

	//私钥属性
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

	// 与密码机通讯
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

	// 与密码机通讯
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
   功能 F8:�8/F9)随机生成令牌种子并使用特定密钥保护输

   输入	：
   mk		根密钥	32H/1A+32H/1A+3H	
   mkIndex 	根密钥索引
   mkType	�	密钥类型	3H	当根密不使用索引时存在此域输入。至少支持00A, 309中的一种
   mkDvsNum	保护密钥离散次数	1N	0~2次
   mkDvsData1	离散数据1	32H	仅当“离散次数”大于0时存在
   mkDvsData2	离散数据2	32H	仅当“离散次数”大于1时存在
   seedLen		种子长度	2N	默认为16，小于或等于16

   输出	:
   seedKey	种子密文	32H	由根密钥离散后的保护密钥保护输出
   seedKeyCheckValue 种子校验值	16H	密钥校验值（国密SM4算法）

 */

int UnionHsmCmdF8(char *mk, int mkIndex, char*mkType, int mkDvsNum, char* mkDvsData1, char* mkDvsData2 ,int seedLen,int seedAlgorithm, char *seedKey, char *seedKeyCheckValue)
{
	/*未测试*/
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
   功能:	F9/F:)同一根下离散密钥加密数据的转加密

输入:
mk	根密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥（外带密文为由密码机主密钥保护）
mkType	根密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，109，309中的一种
srcDvsNum		源保护密钥离散次数	1N	0~2次
srcDvsData1	源离散数据1	32H	仅当“源离散次数”大于0时存在
srcDvsData2	源离散数据2	32H	仅当“源离散次数”大于1时存在
srcDataPendType	源数据填充格式	1N	1：填充0x0；2：0x80；3：强制0x80；4：PKCS#5
dstDvsNum	目标保护密钥离散次数	1N	0~2次
dstDvsData1�	标离散数据1	32H	仅当“目标离散次数”大于0时存在
dstDvsData1�	目标离散数据2	32H	仅当“目标离散次数”大于1时存在
dstDataPendType	目标数据填充格式	1N	1：填充0x0；2：0x80；3：强制0x80；4：PKCS#5
cipherDataLen	密文长度	4N	源离散密钥加密的密文长度
cipherText	源密文	n*2 H	源离散密钥加密的密文

输出:
dstCipherLen	密文长度	4N	目标离散密钥加密的密文长度
dstCipherText	源密文	n*2 H	目标离散密钥加密的密文
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
   功能:	FB:FC)生成动态码

   输入
   mk	密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥（外带密文为由密码机主密钥保护）
   mkType 密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，109，309中的一种
   pkDvsNum 保护密钥离散次数	1N	0~2次
   pkDvsData1 离散数据1	32H	仅当“离散次数”大于0时存在
   pkDvsData2 离散数据2	32H	仅当“离散次数”大于1时存在

   seedLen    种子密钥密文长度	4N	16的整数倍
   seedKeyCiper 种子密钥密文	32H	由根密钥离散后的密钥保护
   algorithm 1：SM3算法
   otpKeyLen 动态密码长度	2N	默认为06
   challengeValLen	挑战值长度	2N	00~~99
   challengeValue 挑战值 nA
   timeChangeCycle 时钟变化周期 
   输出
   otpKey	OTP动态密码	6N	6位OTP动态密码
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
   命令代码	2	值“D”
   根密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥，K+索引号（外带密文为由密码机主密钥保护）
   根密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，309中的一种
   保护密钥离散次数	1N	1~2次
   离散数据1	32H	
   离散数据2	32H	仅当“离散次数”大于1时存在
   种子密钥密文	32H	由根密钥离散后的密钥保护
   生成动态口令算法	1N	1：SM3算法，2：SM4算法
   动态口令长度	2N	默认为06
   挑战值长度	2N	00~~99
   挑战值	nA	
   时钟值T0	16N	
   时钟变化周期Tc	3N	秒为单位，1分钟为：060，1秒：001
   时钟窗口	2N	在n多少个时钟周期内验证口令
   待验证动态口令	nN	

   响应代码	2	值“E”
   错误代码	2	
   时钟偏移方向	1A	向前偏移：+
   向后偏移：-
   时钟偏移值	2N	偏移两个周期：02
 */

int UnionHsmCmdFD(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int seedLen, char* seedKeyCiper, int algorithm, int otpKeyLen, int challengeValLen, char* challengeValue,int timeChangeCycle, int timeValue,int timeWindow, char* otpKey, char* clockSkew,  int* clockSkewValue) 
{
	/*未测试*/
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
   功能：用WWK密钥加解密数据
   输入参数
cryptoFlag: 	0:解密 1:加密
wwk: 		LMK22-23加密
encrypFlag:  	0: ECB模式 1: CBC模式
iv:  		初始向量(仅当CBC模式)
lenOfData: 	输入数据字节数（8的倍数）
data:		待加密或解密的数据
输出参数
outdata: 	输出结果
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

//输入域	长度类型	说　明
//命令代码	2A	值“FE”
//保护密钥根密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥，K+索引号（外带密文为由密码机主密钥保护）
//根密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，309中的一种
//保护密钥离散次数	1N	0~2次
//保护离散数据	n*32H	当保护密钥离散次数为1~2时有
//其中n代表保护密钥离散次数
//变形前的种子密文	32H	
//变形算法	1N	1：UNION_SM3，2：UNION_SM4
//变形数据长度	4N	
//变形数据	nH	
//输出域	长度	说　明
//响应代码	2A	值“FF”
//错误代码	2N	
//变形后的种子密文	32H	
//变形后密种子校验值	16H	

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

//命令代码	2A	值“FF”
//保护密钥根密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥，K+索引号（外带密文为由密码机主密钥保护）
//根密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，309中的一种
//变形前保护密钥离散次数	1N	1~2次
//变形前保护离散数据	n*32H	当保护密钥离散次数为1~2时有
//其中n代表保护密钥离散次数
//变形后保护密钥离散次数	1N	1~2次
//变形后保护离散数据	n*32H	当保护密钥离散次数为1~2时有
//其中n代表保护密钥离散次数
//变形前种子密文长度	4N	16的整数倍
//变形前种子密文	n*2H	由根密钥离散后的密钥保护（PKCS#5填充）
//变形算法	1N	1： SM3
//变形前种子校验值	16H	变形算法为1时，与检查sm3(种子)前8字节是否一致。
//变形后的种子长度	4N	从左到右截取多少个字节
//变形要素长度	4N	
//变形要素	n*2H	
//种子插入到变形要素中位置	4N	如0,则变形数据为：种子+变形要素

//输出域	长度	说　明
//响应代码	2A	值“FG”
//错误代码	2N	
//变形后的种子密文长度	4N	
//变形后的种子密文	n*2H	由变形后的保护密钥保护（PKCS#5填充）
//变形后密种子校验值	16H	

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
   功能:将对有公钥加密的一段数据产生MAC
   输入参数
vkIndex 	:私钥索引，指定的私钥，用于解密数据密文
lenOfRandom  :随机数长度
random 	:随机数
zak 		:用于计算MAC的密钥
dataByPK	:经过公钥加密的数据
输出参数
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
		UnionUserErrLog("in UnionHsmCmd4G:: vkIndex 、zak or dataByPK is null!\n");
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

//将SM4密钥从SM2公钥下加密转换为LMK下加密(SM4、SM2)
// 东莞农商行
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

		// 转换为二进制
		aschex_to_bcdhex(vkValue,lenOfVK,tmpBuf);	
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;
	}

	//密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWO:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// 密钥长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfSM4/2);
	hsmCmdLen += 4;			

	// 密钥
	// 转换为二进制
	aschex_to_bcdhex(sm4BySM2,lenOfSM4,tmpBuf);	
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfSM4/2);
	hsmCmdLen += lenOfSM4/2;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 连接加密机
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

// 导出密钥
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

	//密钥类型
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

	//密钥类型
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

	// 连接加密机
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
   函数功能：(国密)
   WP指令，随机产生一个PIN
   输入参数：
   pinLength：要求随机生成的PIN明文的长度
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：随机产生的PIN的密文，由LMK加密
sizeofPinBlock: pinBlockByLMK的长度
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
	// 命令字
	memcpy(hsmCmdBuf,"WV",2);
	hsmCmdLen = 2;

	// 算法标识
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWV:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN长度
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
	// 随机产生的PIN的密文，由LMK加密
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
   函数功能：(国密)
   CL指令，产生VISA卡校验值CVV
   输入参数：
   cardValidDate：VISA卡的有效期
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   cvv：生成的VISA卡的CVV
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

	// 命令字
	memcpy(hsmCmdBuf,"CL",2);
	hsmCmdLen = 2;

	// 密钥值
	memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen, cvk, strlen(cvk));
	hsmCmdLen += strlen(cvk);

	// 客户帐号
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;

	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;

	// VISA卡的有效期
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;

	// VISA卡服务商代码
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;

	// 磁道
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

	// 生成的VISA卡的CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	cvv[3] = 0;
	return(3);
}

/*
   函数功能：(国密)
   WW指令，加密一个明文PIN/解密一个已加密的PIN
   输入参数：
   mode：0:加密    1:解密
   oriPIN：源pin值
accNo: 账号
lenOfAccNo：客户帐号长度
sizeofDesPIN:desPIN的最大长度
sizeofReferenceNum:referenceNum的最大长度,当加密时才有效
输出参数：
desPIN：目的pin值
referenceNum: 当解密时，才有效 
 */
int UnionHsmCmdWW(int mode,char *oriPIN,char *accNo,int lenOfAccNo,char *desPIN,int sizeofDesPIN,char *referenceNum,int sizeofReferenceNum)
{
	int	ret = 0;
	int	hsmCmdLen = 0;
	char	hsmCmdBuf[128];
	char	tmpBuf[32];
	int	pinLen = 0;
	int	i = 0;

	// 指令
	memcpy(hsmCmdBuf,"WW",2);
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	// 算法标识
	if (mode != 0 && mode != 1)
	{
		UnionUserErrLog("in UnionHsmCmdWW:: mode[%d] not in [0,1]!\n",mode);	
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	// 源pin值
	if (oriPIN == NULL || strlen(oriPIN) == 0)
	{
		UnionUserErrLog("in UnionHsmCmdWW:: oriPIN is null\n");	
		return(errCodeParameter);
	}

	if (mode == 0)
	{
		// 加密时不足16个字符，则填充F至16
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

	// 账号	
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

	// 连接加密机
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

		// 目的PIN
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

		// 参考数
		memcpy(referenceNum,hsmCmdBuf+5+pinLen,12);
		referenceNum[12] = 0;
	}
	return(ret-5);
}

/*
   功能：将PIN块从ZPK下SM4加密翻译到LMK下加密。
   指令：	WX
   输入参数：
mode: 模式�0:PIN从LMK转到ZPK
1:PIN从ZPK转到LMK
zpkValue: zpk密钥值
oriPinBlock: PIN块	
format:   pin块格式
accNo:    账号,去除校验位的最右12位
sizeofPinDesPinBlock: 目的pin块的最大长度
输出参数:
desPinBlock: LMK对（02-03）下加密的PIN/ZPK加密的PIN
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

	// 算法选择
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	if (mode != 0 && mode != 1)
	{
		UnionUserErrLog("in UnionHsmCmdWX:: mode[%d] not in [0,1]!\n",mode);
		return(errCodeParameter);
	}
	// 模式
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	// zpk密钥
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf+hsmCmdLen,zpkValue,32);
	hsmCmdLen += 32;

	// pin块
	UnionLog("in UnionHsmCmdWX:: pinBlock[%s]!\n",pinBlock);
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,strlen(pinBlock));
	hsmCmdLen += strlen(pinBlock);

	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;

	// pin块格式 
	memcpy(hsmCmdBuf+hsmCmdLen,format,2);
	hsmCmdLen += 2;

	// 账号	
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdWX:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;

	hsmCmdBuf[hsmCmdLen] = 0;

	// 连接加密机
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
   功能: PIN块的转加密
   指令: W8
   输入参数:
oriAlgorith:	1N	 源PIN块算法，1-DES/3DES, 2-SM4
desAlgorith:	1N	 目的PIN块算法，1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 源密钥长度标识,1-单倍，2-双倍，3-三倍
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	源zpk索引号或密文
desKeyLenFlag: 	1N	源密钥长度标识,1-单倍，2-双倍，3-三倍
desKeyIndex/desZpkValue: 1A+3H/16/32/48H	目的zpk索引号或密文
oriFormat:	2N	源PIN块格式,‘01’-‘06’输出的PIN块格式
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
desFormat:	2N	目的PIN块格式,‘01’-‘06’输出的PIN块
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字�
pinBlock:	16H/32H		密钥加密的PIN
oriAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
desAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。

sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	16H/32H		转加密后的PIN密文
 */
/*int UnionHsmCmdW8(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"W8",2);
	hsmCmdLen = 2;

	// 源PIN块算法
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 目的PIN块算法
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 源密钥长度标识
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// 源密钥密文或索引号
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

	// 目的密钥长度
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}

	// 目的zpk密钥
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

	// 源PIN块的格式
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 目的PIN块的格式
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 源PIN块
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// 源账号
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
	else if (strcmp(oriFormat,"07") == 0)//桂林银行专用
        {
                if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(oriFormat,"09") == 0) //湖南农信专用
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// 目的账号
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
	else if (strcmp(desFormat,"07") == 0)//桂林银行专用
        {
                if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(desFormat,"17") == 0)//桂林银行专用
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

	// 连接加密机
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
   功能: PIN块的转加密
   指令: W8(桂林银行专用W8)
   输入参数:
oriAlgorith:	1N	 源PIN块算法，1-DES/3DES, 2-SM4
desAlgorith:	1N	 目的PIN块算法，1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 源密钥长度标识,1-单倍，2-双倍，3-三倍
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	源zpk索引号或密文
desKeyLenFlag: 	1N	源密钥长度标识,1-单倍，2-双倍，3-三倍
deskeytype	3N	目的密钥类型:目的密钥类型标识为Y时该域存在；支持ZPK(001) / ZEK(00A)

desKeyIndex/desZpkValue: 1A+3H/16/32/48H	目的zpk索引号或密文
oriFormat:	2N	源PIN块格式,‘01’-‘06’输出的PIN块格式
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
desFormat:	2N	目的PIN块格式,‘01’-‘06’输出的PIN块
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字�
pinBlock:	16H/32H		密钥加密的PIN
oriAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
desAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。

sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	16H/32H		转加密后的PIN密文
 */
int UnionHsmCmdW8_Base(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *deskeytype,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"W8",2);
	hsmCmdLen = 2;

	// 源PIN块算法
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 目的PIN块算法
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 源密钥长度标识
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// 源密钥密文或索引号
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

	// 目的密钥长度
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}
	//(桂林银行专用)目的密钥类型标识:Y--存在  N--不存在(默认为N，目的密钥类型默认为ZPK(001))
	if ((deskeytype) && (strlen(deskeytype) == 3))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"Y%s",deskeytype);
		hsmCmdLen += 4;
	}
	// 目的ZPK/ZEK密钥
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

	// 源PIN块的格式
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 目的PIN块的格式
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else if((desAlgorith == 2) && (strcmp(desFormat,"17") == 0))//桂林银行专用
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else if((desAlgorith == 2) &&(strcmp(desFormat,"18") == 0))//鄞州银行宁波人行普惠金融项目增加格式18，add by huangh 20160606
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 源PIN块
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// 源账号
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
	else if (strcmp(oriFormat,"07") == 0)//桂林银行专用
        {
                if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(oriFormat,"09") == 0) //湖南农信专用
	{
		if ((ret = UnionForm12LenAccountNumber(oriAccNo,strlen(oriAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",oriAccNo);
			return(ret);
		}
		hsmCmdLen += ret;
	}

	// 目的账号
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
	else if (strcmp(desFormat,"07") == 0)//桂林银行专用
        {
                if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	else if (strcmp(desFormat,"17") == 0)//桂林银行专用
        {
                if (strlen(desAccNo) != 16)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: desAccNo[%s] len[%d] != 16\n",desAccNo,(int)strlen(desAccNo));
                        return(errCodeParameter);
                }
                memcpy(hsmCmdBuf+hsmCmdLen,desAccNo,16);
                hsmCmdLen += 16;
        }
	else if (strcmp(desFormat,"18") == 0)//鄞州银行宁波人行普惠金融项目增加格式18，add by huangh 20160606
        {
                if ((ret = UnionForm12LenAccountNumber(desAccNo,strlen(desAccNo),hsmCmdBuf+hsmCmdLen)) < 0)
                {
                        UnionUserErrLog("in UnionHsmCmdW8:: UnionForm12LenAccountNumber for [%s]\n",desAccNo);
                        return(ret);
                }
                hsmCmdLen += ret;
        }
	hsmCmdBuf[hsmCmdLen] = 0;

	// 连接加密机
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
   功能: 计算/验证mac
   指令:W9
   输入参数:
mode:	模式，1:产生MAC，2:校验MAC
zakIndex: zak索引号
zakValue: zak密钥
macCalcFlag: MAC计算方式,1:联机报文的MAC计算方法	
2:顺序文件MAC的计算方法
msgType: 消息类型,0:消息数据为二进制
1:消息数据为扩展十六进制
lenOfMacData: macData数据长度，不大于8192
macData: 用来计算MAC的数据
oriMac:	待校验的MAC值,仅当模式为2时，此域存在
sizeofDesMac:desMac的最大长度
输出参数:
desMac: MAC值

 */
int UnionHsmCmdW9(int mode,char *zakIndex,char *zakValue,int macCalcFlag,int msgType,int lenOfMacData,char *macData,char *oriMac,char *desMac,int sizeofDesMac)
{
	int	ret = 0;
	char	hsmCmdBuf[8192];
	int	hsmCmdLen = 0;

	// 指令
	memcpy(hsmCmdBuf,"W9",2);
	hsmCmdLen = 2;

	// 模式
	if (mode != 1 && mode != 2)
	{
		UnionUserErrLog("in UnionHsmCmdW9:: mode[%d] not in [1,2]!\n",mode);
		return(errCodeParameter);
	}

	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	//密钥索引或密钥密文
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

	// MAC计算方式
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",macCalcFlag);
	hsmCmdLen += 1;

	// 消息类型
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",msgType);
	hsmCmdLen += 1;

	if (macData == NULL || lenOfMacData > 8192)
	{
		UnionUserErrLog("in UnionHsmCmdW9:: macData is null or lenOfMacData > 8912!\n");
		return(errCodeParameter);
	}

	// MAC数据长度
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

	// 待验证的MAC值
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

	// 连接加密机
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
   功能:产生一个PIN校验值PVV
   指令:PV
   输入参数：
pvk:	1A+32H   用LMK1415 加密，‘S’+3
pvkIndex: 1N	 取值为1-F
tpk:	1A+32H	 用LMK1415 加密，‘S’+32
pinBlock:  32H   用TPK加密的ANSI9.8PIN块
PAN:	   16H   16位完整卡号
输出参数:
pvv:	4N	生成的PVV
 */
int UnionHsmCmdPV(char *pvk,char pvkIndex,char *tpk,char *pinBlock,char *pan,char *pvv)
{
	int	ret = 0;
	char	hsmCmdBuf[512];
	int	hsmCmdLen = 0;

	// 指令
	memcpy(hsmCmdBuf,"PV",2);
	hsmCmdLen = 2;	

	// pvk密钥值
	sprintf(hsmCmdBuf+hsmCmdLen,"S%s",pvk);
	hsmCmdLen += 33;

	// pvk索引号
	sprintf(hsmCmdBuf+hsmCmdLen,"%c",pvkIndex);
	hsmCmdLen += 1;

	// tpk密钥值
	sprintf(hsmCmdBuf+hsmCmdLen,"S%s",tpk);
	hsmCmdLen += 33;

	// pinBlock块
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,32);
	hsmCmdLen += 32;

	// 账号
	memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
	hsmCmdLen += 16;

	hsmCmdBuf[hsmCmdLen] = 0;

	// 连接加密机
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

// 生成密钥SM1密钥
/* 
   输入参数
   keyType		密钥类型
   输出参数
   keyByLMK	LMK保护的密钥密文值
   checkValue	密钥校验值
 */
int UnionHsmCmdM0(TUnionDesKeyType keyType, char *keyByLMK, char *checkValue)
{
	int		ret;
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;
	int		offset;

	memcpy(hsmCmdBuf,"M0",2);
	hsmCmdLen = 2;

	//密钥类型
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
   函数功能：
   SM1算法离散子密钥或产生过程密钥，用过程密钥或者子密钥计算并验证MAC。(M9)
   输入参数：
mode: 模式标志
0-计算过程密钥或离散子密钥 
1-计算MAC
2-校验MAC
mkType: 根据密钥类型选择指定的LMK加密输出子密钥和过程密钥 
001 – ZPK （LMK0607加密）
008 – ZAK (LMK2627加密)
00A – ZEK(LMK3031加密)
mk: 密钥密文
mkIndex: 密钥索引, K+3位索引方式读取加密机内保存密钥
mkDvsNum: 指定对密钥离散的次数（0-3次）
mkDvsData: n*8B 离散卡片密钥的数据，其中n代表离散次数. 用指定的分散因子拼接分散因子求反值作为输入数据，做加密计算，产生的16 字节的结果作为子密钥
proKeyFlag: 过程密钥标识,是否对保护密钥进行过程计算,当需要计算过程密钥时,使用过程密钥保护数据.
Y:计算过程密钥
N:不计算过程密钥
可选项:当没有该域时缺省为N
proFactor: 8B 过程因子,在计算过程密钥时，输入数据是8 字节随机数,计算时加密机内拼接8 字节全 “00”。
ivMac: 32H MAC计算的IV
checkMac: 8H 待校验的MAC值 当模式标志为2时，有该域
macDataLen: 4N MAC数据长度
macData: 8H MAC数据
macPrefixDataLen: 4N MAC前缀填充数据长度
macPrefixData: nB MAC前缀填充数据
输出参数：
criperData: 32H 子密钥密文,指定密钥类型下加密，模式标志为0有该域
checkValue: 16H	子密钥校验值,离散子密钥校验值，模式标志为0有该域
proCriperData: 32H 过程密钥密文,过程密钥标识为‘Y’时，且模式标志为0有该域  指定密钥类型下加密
proCheckValue: 16H 过程密钥校验值,过程密钥标识为‘Y’时，且模式标志为0有该域
SM1，LMK0405下加密
3DES，指定密钥类型下加密
mac:MAC值 8H	当模式标志为1，有该域
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

	// 命令代码
	memcpy(hsmCmdBuf,"M9",2);
	hsmCmdLen += 2;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 根密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 根密钥
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

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// 过程密钥标识
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// 过程因子
		if (proKeyFlag[0] == 'Y')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, proFactor, lenOfDvsData);
			hsmCmdLen += lenOfDvsData;
		}
	}

	// MAC计算的IV
	if( (ivMac!= NULL) && (strlen(ivMac) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 32);
		hsmCmdLen += 32;
	}

	// 待校验的MAC值 当模式标志为2时
	if (strcmp(mode,"2") == 0)
	{
		if (strlen(checkMac) > 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, checkMac, 8);
			hsmCmdLen += 8;
		}
	}

	// macDataLen: 4N MAC数据长度 macData: 8H MAC数据	
	if( (macData!= NULL) && (strlen(macData) != 0) )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macDataLen);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen, macData, macDataLen * 2 );
		hsmCmdLen += macDataLen * 2 ;
	}

	// macPrefixDataLen: 4N MAC前缀填充数据长度 macPrefixData: nB MAC前缀填充数据
	if( (macPrefixData!= NULL) && (strlen(macPrefixData) != 0) )
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macPrefixDataLen);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf+hsmCmdLen, macPrefixData, macPrefixDataLen * 2 );
		hsmCmdLen += macPrefixDataLen * 2 ;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdM9:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (strcmp(mode,"0") == 0)
	{
		// criperData: 32H 子密钥密文,指定密钥类型下加密，模式标志为0有该域
		memcpy(criperData, hsmCmdBuf+offset, 16);
		offset += 16;
		// checkValue: 16H	子密钥校验值,离散子密钥校验值，模式标志为0有该域
		memcpy(checkValue, hsmCmdBuf+offset, 8);
		offset += 8;
	}

	if (strcmp(proKeyFlag,"Y") == 0)
	{
		// proCriperData: 32H 过程密钥密文,过程密钥标识为‘Y’时，且模式标志为0有该域  指定密钥类型下加密
		memcpy(proCriperData, hsmCmdBuf+offset, 16);
		offset += 16;
		// proCheckValue: 16H 过程密钥校验值,过程密钥标识为‘Y’时，且模式标志为0有该域
		memcpy(proCheckValue, hsmCmdBuf+offset, 8);
		offset += 8;
	}

	if (mode[0] == '1')
	{
		//mac:MAC值 8H	当模式标志为1，有该域
		memcpy(mac, hsmCmdBuf+offset, 4);
		offset += 4;
	}

	return(0);
}

// 取出RSA公私密钥<TW>
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

// 脱机PIN加密
int UnionHsmCmdVA(char *mode, char *id, int mkIndex, char *mk, char *iv, char *pan, char *bh, char *atc, char *plainData, char *P2, int *lenOfCiperData, char *ciperData)
{       
	int ret;
	char hsmCmdBuf[8192];
	int hsmCmdLen = 0;
	char keyString[64];
	int offset = 0; 

	// 命令代码     
	memcpy(hsmCmdBuf,"VA",2);
	hsmCmdLen += 2;

	// 模式标识     
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID       
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥       
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

	// PIN明文
	memcpy(hsmCmdBuf+hsmCmdLen, plainData, strlen(plainData));
	hsmCmdLen += strlen(plainData);
	memcpy(hsmCmdBuf+hsmCmdLen, "F", 1);
	hsmCmdLen++;

	// P2(脱机PIN模式)
	memcpy(hsmCmdBuf+hsmCmdLen, P2, 1);
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
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
   函数功能:
   PINBLOCK与ANSI9.8格式相互转换
   输入参数:
zpk1: 源zpk
zpk2: 目的zpk
srcPinBlock: 源pinBlock
srcFormat: 源pinBlock格式    1: ANSI9.8格式; 2: 江西农信PIN专用格式
dstFormat: 目的pinBlock格式  1: ANSI9.8格式; 2: 江西农信PIN专用格式
accNo: 账号
输出参数:
dstPinBlock: 目的pinBlock
返回值:
成功>=0, 失败<0
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

	// 命令代码     
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

	// 与密码机通讯
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
   功能:将PIN密文在特殊算法和Anxi X9.8算法之间进行转换
   输入参数：
zpk1:              ZPK1
zpk2:              ZPk2
changeFlag:        转换标志(00：特殊算法转AnsiX9.8　01：AnsiX9.8转特殊算法)
srcPinBlock:           16位被密钥1加密的PIN密文
accNo:                 针对AnxiX9.8格式的帐号
lenOfAccNo:            账号长度
输出参数：
dstPinBlock: 16位被密钥2加密的PIN密文
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
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

	// 命令代码     
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

	// 与密码机通讯
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
   功能:将PIN密文转换为MD5加密
   输入参数：
zpk:              ZPK
srcPinBlock:      PIN密文
accNo:            针对AnxiX9.8格式的帐号
lenOfAccNo:       账号长度
fillMode	  填充模式，0：不填充，1：PIN前填充，2：PIN后填充
fillLen		  前填充长度
fillData	  前填充数据
输出参数：
dstPinBlock: 	  PIN密文
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
计算方法
1.      将源ZPK解密为明文；
2.      根据ZPK明文、pinblock、账号解密得到PIN明文；
3.      对PIN明文进行MD5；
4.      输出PIN的MD5结果；
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

	// 命令代码     
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

	// 与密码机通讯
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
   功能：使用二次生成的密钥生成AnsiX9.9MAC指令
   输入参数：
   keyValue	32H	基础密钥
   random1		16H	随机数A
   random2		16H	随机数B
   lenOfData	3N	MAC数据长度
   data		 	扩展的MAC数据
   输出参数：
   mac		16H	MAC
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 命令代码     
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

	// 与密码机通讯
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

	// 与密码机通讯
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

	if(outDataType == 0)   //modify by hzh in 2011.5.4,增加对输出格式的判断
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
   功能: 将私钥解密后的登录口令转换为网银的EDK加密
   输入参数：
   fillMode	1N	填充方式
   vkIndex	2N	私钥索引,“00”－“20” ， “99”为从外部输入的私钥
   lenOfPinByPK	4N	公钥加密的登录口令密文数据长度
   pinByPK	nB	公钥加密的登录口令密文数据
   keyLen	4N	私钥长度
   keyValue	nB	用主密钥加密的私钥	
   edk		16H/32H/48H	edk密钥
   输出参数：
   pinByEDK	16H	edk加密的登录口令数据
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 指令
	memcpy(hsmCmdBuf,"4A",2);
	hsmCmdLen = 2;

	// 填充模式
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
	hsmCmdLen += 1;

	// 索引号
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

	// 与密码机通讯
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
   功能:将解密后的登录口令(AN9.8)转换为网银的pinoffset
   输入参数：
   zpk		16H/32H/48H	zpk密钥值
   accNo		12N		账号
   pinBlock	16H		zpk加密的pin密文数据
   edk		16H/32H/48H	edk密钥
   输出参数：
   pinOffset	16H	edk加密的登录口令数据
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 指令
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

	// 与密码机通讯
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
   功能: 将由公钥加密的PIN转换成ANSI9.8标准
   输入参数：
   vkIndex		2N		索引号
   keyType		1N		密钥类型, 1：TPK  2：ZPK
   keyValue	16H/32H/48H	密钥值
   pinType		1N		pin类型,0：原始PIN ,1：ANSI9.8
   fillMode	1N		数据填充方式,"0"：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密, "1"：PKCS填充方式
   accNo		16N		账号
   pinLen		4N		公钥加密的pin长度	
   pinByPK		B		公钥加密的pin数据
   输出参数：
   pinBlock	16H		keyValue加密的pin
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 指令
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
	// 与密码机通讯
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
   功能: 将私钥解密后的登录口令转换为网银的EDK加密
   输入参数：
   fillMode	1N	填充方式
   vkIndex	2N	私钥索引,“00”－“20” ， “99”为从外部输入的私钥
   lenOfPinByPK	4N	公钥加密的登录口令密文数据长度
   pinByPK	nB	公钥加密的登录口令密文数据
   vkLen	4N	私钥长度
   vk		nB	用主密钥加密的私钥	
   edk		16H/32H/48H	edk密钥
   输出参数：
   pinByEDK	n*2H	edk加密的登录口令数据
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 指令
	memcpy(hsmCmdBuf,"4E",2);
	hsmCmdLen = 2;

	// 填充模式
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
	hsmCmdLen += 1;

	// 索引号
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

	// 与密码机通讯
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
	功能:	用C-mac算法计算MAC 
	输入参数：
	zakKey          32H             lmk(008)加密的ZAK
	IV              16H             初始向量                                            
	macData         NH              运算MAC数据                        
	输出参数：
	CMAC           16H             C-MAC          
	ICV		16H		ZAK左半部份（16位）对C-MAC DES加密运算   
	返回值：
	<0：函数执行失败，值为失败的错误码
	>=0：函数执行成功
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

	//命令码
	memcpy(hsmCmdBuf, "CF", 2);
	hsmCmdLen += 2;

	//lmk(008)加密的ZAK
	memcpy(hsmCmdBuf+hsmCmdLen, "X", 1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen, zakKey, 32);
	hsmCmdLen  += 32;

	//Mac初始向量
	memcpy(hsmCmdBuf+hsmCmdLen, IV, 16);
	hsmCmdLen += 16;

	//MAC数据长度，满足8的倍数
	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", (int)strlen(macData)/2);
	hsmCmdLen += 4;

	//运算MAC数据
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

// add by lisq 20141117 攀枝花商行
/*
   功能
   SM2公钥加密的PIN明文用SM2私钥做SCE解密后，再用SM4算法加密输出
   输入参数
   mode	算法标识。0：SM2加密转为SM4加密；1：SM2加密转为攀枝花专用算法加密。
   vkIndex	私钥索引。01-20；99：外带密钥
   vkLen	外带私钥长度。仅当vkIndex为99时有
   vk	外带私钥。仅当vkIndex为99时有
   cipherTextLen	密文长度
   cipherText	密文
   zpkValue	ZPK密钥值
   accNo	账号
   输出参数
   pinBlkByZpk	ZPK加密的PIN密文

   返回值
   >=0：成功；<0：失败
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


	//命令码
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
		//lmk加密的ZPK
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
   功能
   私有加密算法加密pin
   输入参数
   dataLen	明文数据长度，仅支持6位
   plainData	明文数据，仅支持6位
   sizeofCipherData	密文数据输出参数大小
   输出参数
   cipherData	密文数据
   返回值
   <0：失败
   >=0：成功
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

	//命令码
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

// add by lisq 20141117 end  攀枝花商行




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
   50指令
   功能：用EDK密钥加解密数据，如果是解密状态，则必须在授权下才能处理，否则报错。

   输  入  消  息  格  式

   输入域          长度    类型    说　明
   消息头          m       A
   命令代码        2       A       值"50"
   Flag            1       N       0：加密
   1：解密
   EDK             16 or 32
   1A+32 or 1A+48
   H       LMK24-25加密
   DATA_length     4       N       输入数据字节数（8的倍数）范围：0008-4096
   DATA            n*2     H       待加密或解密的数据（以ASC码表示）

   输  出  消  息  格  式

   输出域  长度    类型    说　明
   消息头          M       A
   响应代码        2       A       值"51"
   错误代码        2       N
   DATA            n*2     H       输出结果

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
   根据指定对称密钥指定算法产生MAC

   输入参数：
   modeFlag，1：产生MAC；2：验证MAC
   algFlag，算法标志，01：DES_MAC；02：DES_NAMC_EMV；03：AES_MAC；04：AES_MAC_EMV
   keyType，密钥类型“000”
   keyLen，密钥长度标志，0：单倍长（仅当algFlag为01或02）；1：双倍长
   keyValue，密钥值，16H/32H
   iv，初始向量，16H
   macDataLen，MAC数据长度
   macData，MAC数据
   macLen，MAC长度（仅当modeFlag为2时）
   mac，MAC(仅当modeFlag为2时）

   输出参数：
   mac，MAC(仅当modeFlag为1时

   返回值：

   < 0，失败；
   =< 0，成功
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

// modify by zhouxw 20160105, 将TS指令改为不写死模式和密钥长度
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

	// 命令代码
	memcpy(hsmCmdBuf, "TS", 2);
	hsmCmdLen += 2;
	// 模式
	if(strcmp(mode, "00") != 0 && strcmp(mode, "01") != 0 && strcmp(mode, "02") != 0 && strcmp(mode, "10") != 0 && strcmp(mode, "11") != 0 && strcmp(mode, "12") != 0)
	{
		UnionUserErrLog("in RacalCmdTS:: mode error!\n");
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 2);
	hsmCmdLen += 2;
	// 源密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, "000", 3);
	hsmCmdLen += 3;
	// 源密钥长度
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
	// 源密钥
	memcpy(hsmCmdBuf+hsmCmdLen, key1, strlen(key1));
	hsmCmdLen += strlen(key1);
	// 目的密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, "000", 3);
	hsmCmdLen += 3;
	// 目的密钥长度
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
	// 目的密钥
	memcpy(hsmCmdBuf+hsmCmdLen, key2, strlen(key2));
	hsmCmdLen += strlen(key2);
	if(memcmp(mode, "10", 2) == 0 || memcmp(mode, "11", 2) == 0 || memcmp(mode, "12", 2) == 0)
	{
		// IV 初始向量
		memcpy(hsmCmdBuf+hsmCmdLen, IV, 16);
		hsmCmdLen += 16;
	}
	//前缀
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
	//后缀
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
	// 与密码机通讯
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

	// modify by leipp 20150330 begin 修改返回值为密钥长度
	return(len);
	// modify end
}

/*
   将北理专用算法加密的PIN转为 ZPK加密(HI)
   输入参数:
   factorOfKey     北理专用算法密钥因子
   keyOfZPK                用于加密的目的zpk
   pan                                             12位实际参与运算账号
   pin                                             经北理专用算法加密的PIN数据密文

   输出参数:
   pinBlock                        zpk密钥加密的pin
   errCode：                       密码机返回码
   返回值:
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 命令代码
	memcpy(hsmCmdBuf, "HI", 2);
	hsmCmdLen += 2;
	// 密钥因子
	memcpy(hsmCmdBuf+hsmCmdLen, factorOfKey, 8);
	hsmCmdLen += 8;
	// 目的zpk
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
	// 源pin密文
	memcpy(hsmCmdBuf+hsmCmdLen, pin, strlen(pin));
	hsmCmdLen += strlen(pin);

	// 与密码机通讯
	UnionSetBCDPrintTypeForHSMCmd();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdHI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	//目的zpk加密的密文
	// pinBlock
	sprintf(pinBlock,"%s", hsmCmdBuf+4);
	return (ret);
}



/*
   把公钥加密的登录密码转为华润登录密码专用算法(HJ)
   输入参数:
   vkIndex                         私钥索引(00-20，99表示RSA外带)
   lenOfVK       私钥长度（只有当私钥索引为99时，才有此值）
   valueOfVK     私钥值(只有当私钥索引为99时,才有此值）
   flag          公钥加密数据填充方式
   keyOfZPK                用于加密的目的zpk
   lenOfPin      公钥加密密文长度
   pinBlock      公钥加密的密文
   clientNo      客户号

   输出参数:
   pinOfHR                   华润专用算法密文
   replayFactor  重放因子
   errCode：                       密码机返回码
   返回值:
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 命令代码
	memcpy(hsmCmdBuf, "HJ", 2);
	hsmCmdLen += 2;
	// 私钥索引
	memcpy(hsmCmdBuf+hsmCmdLen, vkIndex, 2);
	hsmCmdLen += 2;
	/*
	   if( memcmp(vkIndex,"99",2) == 0 )
	   {
	//私钥长度，当私钥索引为99时有该域
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d", lenOfVK);
	hsmCmdLen+=4;
	//私钥值(只有当私钥索引为99时,才有此值）
	aschex_to_bcdhex(valueOfVK,strlen(valueOfVK),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfVK;
	}
	 */
	//公钥加密数据填充方式
	memcpy(hsmCmdBuf+hsmCmdLen,flag,1);
	hsmCmdLen++;
	//目的ZPK
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
	// 公钥加密密文长度
	memcpy(hsmCmdBuf+hsmCmdLen,lenOfPin,4);
	hsmCmdLen += 4;
	//公钥加密的密文
	aschex_to_bcdhex(pinBlock,strlen(pinBlock),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += atoi(lenOfPin);
	//客户号
	aschex_to_bcdhex(clientNo,strlen(clientNo),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += strlen(clientNo)/2;

	// 与密码机通讯
	// 与密码机通讯
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
		//华润专用算法密文
		memcpy(pinOfHR,hsmCmdBuf+4,32);
		//重放因子(?重放因子)
		memcpy(replayFactor,hsmCmdBuf+4+32+2,ret-4-32-2);
		return(ret);
	}
	else
	{
		//华润专用算法密文
		memcpy(pinOfHR,hsmCmdBuf+4,48);
		memcpy(replayFactor,hsmCmdBuf+4+48+2,ret-4-48-2);
		return(ret);
	}
	return(0);
}



/*              

		把公钥加密的内管密码转为Hash中的SHA1算法加密

		输入域  长度    说　明
		命令代码        2A      值“HK”
		私钥索引        2N      00-20，99表示RSA外带
		私钥长度        4N      索引99表示RSA外带，无此域
		私钥    nB      索引99表示RSA外带，无此域
		公钥加密数据填充方式    1N      0：0填充方式
		1：PKCS填充方式

		公钥加密密文长度        4N       
		公钥加密的密文  nB
		柜员编号        nB      


		输出域  长度    说　明
		响应代码        2       值“HL”eo
		错误代码        2       10：密钥不符合奇校验
		12：无主密钥
		15：输入数据错
		24：PIN长度错
		重放因子长度    N       重放因子长度值
		重放因子        nH      重放因子
		Hash中的SHA1算法加密的密文      nH      Hash中的SHA1算法加密的密文


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

	memcpy(hsmCmdBuf, "HK", 2);      //命令代码
	hsmCmdLen = 2;

	memcpy(hsmCmdBuf + hsmCmdLen, index, 2);  //index
	hsmCmdLen += 2;

	//index为99时有以下域
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

	/*向密码机发送请求报文*/
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
   功能：把zpk加密的字符密码密文转换为另一zpk加密的字符密码密文
   输入参数:
   cpOriZPK ：           源ZPK，当前加密PIN块的ZPK；LMK对（06-07）下加密
   cpDestZPK ：          目的ZPK，将要加密PIN块的ZPK；LMK对（06-07）下加密
   conEsscFldAccNo ：    帐号（conEsscFldAccNo）
   cpOriPIN ：           源PIN块

   输出参数:
   errCode：               密码机返回码
   cpOutPIN：              PIN密文

   返回值:
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 命令代码
	memcpy(hsmCmdBuf, "HB", 2);
	hsmCmdLen += 2;

	// 源ZPK
	if( (ret = UnionPutKeyIntoRacalKeyString(cpOriZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen)) < 0 )
	{
		UnionUserErrLog("in RacalCmdHB:: UnionPutKeyIntoRacalKeyString cpOriZPK [%s]!\n", cpOriZPK);
		return (ret);
	}
	hsmCmdLen += ret;

	//  目的ZPK
	if( (ret = UnionPutKeyIntoRacalKeyString(cpDestZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-hsmCmdLen)) < 0 )
	{
		UnionUserErrLog("in RacalCmdHB:: UnionPutKeyIntoRacalKeyString cpDestZPK [%s]!\n", cpDestZPK);
		return (ret);
	}
	hsmCmdLen += ret;

	// 帐号
	if( (ret = UnionForm12LenAccountNumber(AccNo, strlen(AccNo), hsmCmdBuf+hsmCmdLen)) < 0 )
	{
		UnionUserErrLog("in RacalCmdHB:: UnionForm12LenAccountNumber !\n");
		return (ret);
	}
	hsmCmdLen += ret;


	// 源PIN块
	if( strlen(cpOriPIN) != 16 )
	{
		UnionUserErrLog("in RacalCmdHB:: cpOriPIN is error!\n");
		return (errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, cpOriPIN, 16);
	hsmCmdLen += 16;


	// 与密码机通讯
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
   把zpk加密的PIN（ANSI9.8标准，带账号）转成华润核心专用算法加密。

   输入参数:
   源ZPK密钥名称:源ZPK密钥名称
   源pin密文:源Zpk加密的PIN密文，16位长。
   账号类型：1代表13位账号，2代表19位卡号。
   账号:账号分13位和19位两种，在“HE”指令需要传入完整账号。

   返回参数:
   华润核心专用算法加密的密文:华润核心专用算法加密的密文，8位长度

   返回值:
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功

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

	memcpy(hsmCmdBuf,"HE", 2);      /*命令代码*/
	hsmCmdLen += 2;
	if ((ret = UnionPutKeyIntoRacalKeyString(zpkvalue, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) 
	{
		UnionUserErrLog("in RacalCmdHE:: UnionPutKeyIntoRacalKeyString [%s]!\n", zpkvalue);
		return(ret);
	}
	hsmCmdLen += ret;

	memcpy(hsmCmdBuf + hsmCmdLen, srcpinblock, 16);  //pin密文
	hsmCmdLen += 16;
	if (flag == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "1", 1);      /*账号标志*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,13);
		hsmCmdLen += 13;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "2", 1);      /*账号标志*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,19);
		hsmCmdLen += 19;
	}


	/*向密码机发送请求报文*/
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
   把zpk加密的PIN（ANSI9.8标准，带账号）转成华润核心专用算法加密。

   输入参数:
   源ZPK密钥名称:源ZPK密钥名称
   源pin密文:源Zpk加密的PIN密文，16位长。
   账号类型：1代表13位账号，2代表19位卡号。
   账号:账号分13位和19位两种，在“HE”指令需要传入完整账号。

   返回参数:
   华润核心专用算法加密的密文:华润核心专用算法加密的密文，8位长度

   返回值:
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功

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

	memcpy(hsmCmdBuf, "HF", 2);      /*命令代码*/
	hsmCmdLen = 2;

	//拼ZPKVALUE
	if ((ret = UnionPutKeyIntoRacalKeyString(zpkvalue, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*zpkvalue*/
	{
		UnionUserErrLog("in RacalCmdHF:: UnionPutKeyIntoRacalKeyString [%s]!\n", zpkvalue);
		return(ret);
	}
	hsmCmdLen += ret;

	if (flag == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "1", 1);      /*账号标志*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,13);
		hsmCmdLen += 13;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "2", 1);      /*账号标志*/
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,account,19);
		hsmCmdLen += 19;
	}

	//拼加密因子
	memcpy(hsmCmdBuf + hsmCmdLen, gene, 8);
	hsmCmdLen += 8;

	//拼加密数据
	if ((ret = UnionPutKeyIntoRacalKeyString(encrypteddata, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*zpkvalue*/
	{
		UnionUserErrLog("in RacalCmdHE:: UnionPutKeyIntoRacalKeyString [%s]!\n", encrypteddata);
		return(ret);
	}
	hsmCmdLen += ret;

	/*向密码机发送请求报文*/
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
   功能：公钥加密的PIN密文转为DES密钥加密（带因子加密）。
   输入参数:
   srcAlgorithmID  非对称密钥算法标识,0:国际算法,  1:国密算法
   vkIndex         指定的私钥，用于解密PIN数据密文
   lenOfVK	私钥长度
   vkValue	私钥值
   flag         “0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
		“1”：PKCS填充方式（一般情况下使用此方式）
   dstAlgorithmID ZPK密钥算法标识,0:国际算法,  1:国密算法
keyValue:       用于加密PIN的密钥
pan             用户有效主帐号长度
lenOfPinByPK    经公钥加密的PIN数据密文长度
pinByPk         经公钥加密的PIN数据密文
输出参数:
lenOfPinBlock   des密钥加密的pin长度
pinBlock        des密钥加密的pin
idCodeLen       01-20
idCode          ID码明文
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
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

	// 命令代码
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

	// 与密码机通讯
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

// add by lisq 20150120 微众银行

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

	// 命令代码
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


// add by lisq 20150120 end 微众银行

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

	// 命令代码
	memcpy(hsmCmdBuf, "8A", 2);
	hsmCmdLen += 2;

	// mode
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mode);
	hsmCmdLen += 1;

	// vkIndex
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	hsmCmdLen += 2;

	// 私钥值
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

	// 填充模式
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);
	hsmCmdLen += 1;

	// 公钥加密的数据长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);	
	hsmCmdLen += 4;

	// 公钥加密的数据密文
	aschex_to_bcdhex(pinByPK,lenOfPinByPK,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// 防重放因子
	memcpy(hsmCmdBuf+hsmCmdLen,factorData ,8);
	hsmCmdLen += 8;

	// 密钥值
	if ((ret = UnionGenerateX917RacalKeyString(keyLen,keyValue,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8A:: UnionGenerateX917RacalKeyString for [%d][%s]\n",keyLen,keyValue);
		return(ret);
	}
	hsmCmdLen += ret;

	// PIN随机因子密文
	if (mode == 2)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, randomData, 16);
		hsmCmdLen += 16;
	}

	// 账号
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
   函数功能：
   8D指令，用IBM方式产生一个PIN的Offset
   输入参数：
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinLength：LMK加密的PIN密文长度
   pinBlockByLMK：由LMK加密的PIN密文
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinOffset：PIN Offset，左对齐，右补'F'

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

	// 命令字
	memcpy(hsmCmdBuf,"8D",2);
	hsmCmdLen = 2;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8D:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8D:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 设置不检查响应码
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8D:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// 产生的PIN的Offset
		memcpy(pinOffset,hsmCmdBuf+4,12);
		return(12);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}

/*
   函数功能:
   KK指令,SM2公钥加密的pin转换成zpk（DES和SM4）加密
   输入参数：
   keyIndex	SM2密钥索引
   vkLen		SM2外带密钥长度
   vkValue		SM2外带密钥
   lenOfPinByPK	SM2公钥加密的密文长度
   pinByPK		密文
   algorithmID	算法标识 1：SM4密钥, 2: 3DES密钥
   keyType		密钥类型,1：ZPK, 2：TPK 
   pinFormat	PIN格式
   lenOfAccNo	账号长度	
   accNo		账号
   keyLen		密钥长度
   keyValue	ZPK或TPK密钥
   specialAlg	可选项
   P-密码安全控件安全算法
   (公钥算法为SM2时可选，RSA时不存在)
   sizeofPinByKey	pinByKey的最大长度
   输出参数：
   pinByKey	DES或SM4下加密的PIN

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

	//命令码
	memcpy(hsmCmdBuf, "KK", 2);
	hsmCmdLen += 2;

	//SM2密钥索引
	memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
	hsmCmdLen += 2;

	if(!strncmp(keyIndex, "99", 2))
	{
		//SM2外带密钥长度
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
	hsmCmdLen += 4;

	//密文
	aschex_to_bcdhex(pinByPK, lenOfPinByPK, hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	sprintf(hsmCmdBuf + hsmCmdLen, "%d", algorithmID);
	hsmCmdLen += 1;

	sprintf(hsmCmdBuf + hsmCmdLen, "%d", keyType);
	hsmCmdLen += 1;

	memcpy(hsmCmdBuf + hsmCmdLen, pinFormat, 2);
	hsmCmdLen += 2;

	// 12位客户帐号
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

	//add by lusj 20160118  柳州银行专用算法，兼容通用版本
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
   函数功能:
   KL指令,SM2公钥加密的渠道密码转换成zpk（DES和SM4）加密
   输入参数：
   keyIndex        SM2密钥索引
   vkLen           SM2外带密钥长度
   vkValue 	SM2外带密钥
   lenOfPinByPK    SM2公钥加密的密文长度
   pinByPK  	密文
   algorithmID     算法标识,1：SM4密钥, 2: 3DES密钥
   keyType         密钥类型, 0：ZMK, 1：ZPK, 2：ZEK 
   keyValue        密钥值
   sizeofPinByKey	pinByKey的最大长度
   输出参数：
   pinByKey        DES或SM4下加密的PIN

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

	//命令码
	memcpy(hsmCmdBuf, "KL", 2);
	hsmCmdLen += 2;

	//SM2密钥索引
	memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
	hsmCmdLen += 2;

	if(!strncmp(keyIndex, "99", 2))
	{
		//SM2外带密钥长度
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}

	//密文长度
	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
	hsmCmdLen += 4;
	aschex_to_bcdhex(pinByPK,lenOfPinByPK , hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	//算法标识
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", algorithmID);
	hsmCmdLen += 1;
	//密钥类型
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", keyType);
	hsmCmdLen += 1;

	if(1 == algorithmID)
		memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
	else
		memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
	hsmCmdLen += 1;

	//密钥
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

        //命令码
        memcpy(hsmCmdBuf, "KL", 2);
        hsmCmdLen += 2;

        //SM2密钥索引
        memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
        hsmCmdLen += 2;

        if(!strncmp(keyIndex, "99", 2))
        {
                //SM2外带密钥长度
                sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
                hsmCmdLen += 4;

                aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
                hsmCmdLen += vkLen/2;
        }
        //密文长度
        sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
        hsmCmdLen += 4;
        aschex_to_bcdhex(pinByPK,lenOfPinByPK , hsmCmdBuf + hsmCmdLen);
        hsmCmdLen += lenOfPinByPK/2;

        //算法标识
        sprintf(hsmCmdBuf + hsmCmdLen, "%d", algorithmID);
        hsmCmdLen += 1;

        if(algorithmID == 1 || algorithmID == 2)
        {
                //密钥类型
                sprintf(hsmCmdBuf + hsmCmdLen, "%d", keyType);
                hsmCmdLen += 1;

                if(1 == algorithmID)
                        memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
                else
                        memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
                hsmCmdLen += 1;

                //密钥
                memcpy(hsmCmdBuf + hsmCmdLen, keyValue, 32);
                hsmCmdLen += 32;
        }
        else
        {
                // 加盐偏移
                sprintf(hsmCmdBuf + hsmCmdLen, "%04X", saltedOffset);
                hsmCmdLen += 4;
                // 加盐长度
                sprintf(hsmCmdBuf + hsmCmdLen, "%04X", saltedLen/2);
                hsmCmdLen += 4;
                // 加盐数据
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
   函数功能：
   8E指令，用IBM方式产生一个PIN的Offset
   输入参数：
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinLength：LMK加密的PIN密文长度
   pinBlockByLMK：由LMK加密的PIN密文
   pvk：LMK加密的PVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinOffset：PIN Offset，左对齐，右补'F'

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

	// 命令字
	memcpy(hsmCmdBuf,"8E",2);
	hsmCmdLen = 2;

	// LMK加密的PVK
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S");
	memcpy(hsmCmdBuf+hsmCmdLen,pvk,32);
	hsmCmdLen += 32;

	// LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8E:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 设置不检查响应码
	UnionSetIsNotCheckHsmResCode();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd8E:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// 产生的PIN的Offset
		memcpy(pinOffset,hsmCmdBuf+4,12);
		pinOffset[12] = 0;
		return(12);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
}


/*
   函数功能:    生成一个随机的双倍长度的*CSCK，并用LMK对（14-15）的变种4加密(RacalCmdRY0)
输入:
eparator:          分隔符          可选项，如果显示后面的三个域，该域必须显示,值为“；”
keyMethodByZMK       密钥方案        可选项,ZMK下加密密钥的方案
keyMethodByLMK       密钥方案        可选项，LMK下加密密钥的方案
checkValueType        密钥校验值类型  可选项,0-kcv向后兼容 1-kcv 6H

输出:
CSCKByLMK                *CSCK           LMK对（14-15）的变种4下加密的*CSCK
checkValue       	校验值     
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
   RY指令，导出一个*CSCK
   输入参数：
   ZMKByLMK,	LMK对(04-05)下加密的ZMK
   CSCKByLMK,	LMK对(14-15)下加密的CSCK
   输出参数：
   CSCKByZMK,	由所提供的ZMK下加密的CSCK
   checkValue,	CSCK下加密64比特0的结果

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
   函数功能：RY指令,导入一个*CSCK
   输入参数：
   ZMKByLMK,	LMK对(04-05)下加密的ZMK
   CSCKByZMK,	由所提供的ZMK下加密的CSCK
   输出参数：
   CSCKByLMK,	LMK对(14-15)的变种4下加密的CSCK
   checkValue,	CSCK下加密64比特0的结果

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
   函数功能： RY指令，计算卡安全码
   输入参数：
   CSCKByLMK,	LMK对(14-15)的变种下加密的CSCK
   accNo,		账号的全部数字，如果不足19位则左补齐0至19位
   activeDate,	"YYMM"格式的有效期
   输出参数：
   CSCOf5Digit,	5位的CSC值
   CSCOf4Digit,	4位的CSC值
   CSCOf3Digit,	3位的CSC值
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
   函数功能： RY指令，校验卡安全码
   输入参数：
   CSCKByLMK,	LMK对(14-15)的变种4下加密的CSCK
   accNo,		账号的全部数字，如果不足19位则左补齐0到19位
   activeDate,	"YYMM"格式的有效期
   CSCOf5Digit,	5位的"CSC"值，如果不是则显示"FFFFF"
   CSCOf4Digit,	4位的"CSC"值，如果不是则显示"FFFF"
   CSCOf3Digit,	3位的"CSC"值，如果不是则显示"FFF"
   输出参数：
   checkResultOf5CSC,	5位"CSC"的校验结果
   checkResultOf4CSC,	4位"CSC"的校验结果
   checkResultOf3CSC,	3位"CSC"的校验结果

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

	// 设置不检查响应码
	UnionSetIsNotCheckHsmResCode();

	if((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdRY4:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// 设置需要检查响应码
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
   函数功能: B3指令，账号密码检查(平安银行专用)
   输入参数:
   zpk,	在LMK对加密下的zpk
   pinBlock,	源pin块
   accNo,	账号
   pvk,	目的zpk
   输出参数:
   pvv,	pvk加密的pin密文
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
   函数功能: B1指令，卡号密码检查
   输入参数:
   zpk,    在LMK对加密下的zpk
   pinBlock,       源pin块
   pvk,    目的zpk
   输出参数:
   pvv,    pvk加密的pin密文
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

	// 与密码机通讯
	if((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdB5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	// PIN长度
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
   函数功能: 9B指令	将ZPK加密的PIN密文转为行内软算法加密
   输入参数:
   algorithmID			密钥算法标识	0�:3DES
   1�:SM4
   mode				加密模式  0：核心存折算法
   1：核心卡算法
   2：村镇银行算法
   accNo1				卡号序号  账号中去除校验位的最右7位
   zpk    				在LMK对加密下的zpk
   pinBlock			ZPK加密的PIN密文
   format				Pin格式
   accNo2				账号  账号中去除校验位的最右12位
   输出参数:
   Pin    				私有软算法加密的PIN密文
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
   函数功能：9C指令	将村镇银行软算法加密的PIN密文转为ZPK加密
   输入参数：
   algorithmID			密钥算法标识	0�:3DES
1 :SM4
zpk                             在LMK对加密下的zpk
pinBlock                        ZPK加密的PIN密文
accNo				账号
输出参数：
pinByZPK			ZPK加密的PIN密文
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
   函数功能:9D指令		生成&计算村镇银行MAC
   输入参数：
   mode		模式	1：产生MAC
   2：校验MAC
   macDataLen	用来计算MAC的数据的长度
   macData		用来计算MAC的数据
   mac1		待校验的MAC值
   输出参数
   mac2		生成的MAC	
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
   功能: 将扩展后的ZPK加密的密文用另一把密钥加密(仍为扩展后形式)
   指令: KM
   输入参数:
pinFlag:	1A		Y:源PIN块为扩展形式，
N:未扩展源PIN块
不填则默认为Y
oriAlgorith:	1N		源PIN块算法，1-DES/3DES, 2-SM4
desAlgorith:	1N		目的PIN块算法，1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N		源密钥长度标识,1-单倍，2-双倍，3-三倍
oriKeyIndex/oriZpkValue:1A+3H/16/32/48H	源zpk索引号或密文
desKeyLenFlag: 	1N		源密钥长度标识,1-单倍，2-双倍，3-三倍
desKeyIndex/desZpkValue:1A+3H/16/32/48H	目的zpk索引号或密文
oriFormat:	2N		源PIN块格式,‘01’-‘06’输出的PIN块格式
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
desFormat:	2N		目的PIN块格式,‘01’-‘06’输出的PIN块
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16节
pinBlock:	32H/48H/64H	密钥加密的PIN
oriAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
desAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。

sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	32H/48H/64H		转加密后的PIN密文
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

	// 源PIN块算法
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdKM:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 目的PIN块算法
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdKM:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 源密钥长度标识
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// 源密钥密文或索引号
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

	// 目的密钥长度
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}

	// 目的zpk密钥
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

	// 源PIN块的格式
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 目的PIN块的格式
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 源PIN块
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// 源账号
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

	// 目的账号
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

	// 连接加密机
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

	//拼装指令
	memcpy(hsmCmdBuf,"SD",2);
	hsmCmdLen = 2;
	
	if (srcAlgorithmID == 1)	// 国密算法
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
	}

	if (lenofhashFlag == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,hashFlag,1);
		hsmCmdLen += 1;
	}

	//私钥索引
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

	// 国际算法才有此域
	if (srcAlgorithmID == 0)
	{
		//用公钥加密时所采用的填充方式
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
		//数据填充标识
		memcpy(hsmCmdBuf+hsmCmdLen,dataFlag,1);
		hsmCmdLen++;
		
		//modefy by lusj 20160316 东莞银行定制指令，根据技术部同事要要求数据不用压缩，与旧版本一致
		//填充数据长度
//		sprintf(hsmCmdBuf+hsmCmdLen,"%03d",lenOfData/2);
//		hsmCmdLen += 3;

		//填充数据
//		aschex_to_bcdhex(data,lenOfData,hsmCmdBuf+hsmCmdLen);
//		hsmCmdLen += lenOfData/2;

		//填充数据长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%03d",lenOfData);
		hsmCmdLen += 3;
		
		//填充数据
		memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
		hsmCmdLen+=lenOfData;	
		//end by lusj
	}
	
	//modefy by lusj 20160310 不进行ID压缩
	//登录ID长度
//	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfID/2);
//	hsmCmdLen += 2;

	//登录ID
//	aschex_to_bcdhex(ID,lenOfID,hsmCmdBuf+hsmCmdLen);
//	hsmCmdLen += lenOfID/2;

	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfID);
        hsmCmdLen += 2;

        //登录ID
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

	//PIN密文
	aschex_to_bcdhex(pinBlockByPK,strlen(pinBlockByPK),hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += strlen(pinBlockByPK)/2;

	hsmCmdBuf[hsmCmdLen] = 0;

	// 设置不检查响应码
	UnionSetIsNotCheckHsmResCode();

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSD:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}

	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// 随机产生的PIN的密文，由LMK加密
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
   功能:转化PIN。
   输入参数:
   rootKey：应用主密钥
   keyType：密钥类型；001-ZPK，008-ZAK。
   discreteNum：离散次数
   discreteData1：离散数据1
   discreteData2：离散数据2
   discreteData3：离散数据3
   resPinBlock：源pinblock
   AccNo：帐号
   pinFmt：pin格式
   zpk：目的zpk
   输出参数:
   disPinBlock：目的pinblock。
   errCode：密码机返回码。
   返回值:
   <0: 函数执行失败，值为失败的错误码
   >=0: 函数执行成功
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
	//写入密钥类型
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", discreteNum); /*离散次数*/
	hsmCmdLen += 1;
	/*离散数据*/
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

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY5:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}


	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// 产生的pinbock
		memcpy(disPinBlock, hsmCmdBuf + 4, 16);
		return(16);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));

}

/*
   功能:Mac计算，返回MAC值。
   输入参数:
   rootKey：应用主密钥
   keyType：密钥类型；001-ZPK，008-ZAK。
   discreteNum：离散次数
   discreteData1：离散数据1
   discreteData2：离散数据2
   discreteData3：离散数据3
   msgBlockNum：消息块号 (0:仅一块,1:第一块,2:中间块,3:最后块)
   msgType：消息类型(0－消息数据为二进制,1－消息数据为扩展十六进制)
   iv：IV,默认初始值16H的0
   msgLength：消息长度
msg: 消息块
输出参数:
Mac：MAC
errCode：密码机返回码。
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
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

	//把密钥值写入密钥域中
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

	//写入密钥类型
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", discreteNum); /*离散次数*/
	hsmCmdLen += 1;

	/*离散数据*/
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

	/*根据msgType设置msgLength*/
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

	/*设置消息块*/
	memcpy(hsmCmdBuf + hsmCmdLen, msg, msgLength);
	hsmCmdLen += msgLength;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY7:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}


	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		// 产生的mac
		memcpy(mac, hsmCmdBuf + 4, 16);
		return(16);
	}
	else
		return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));

}

// add by leipp 20150731
// 导入SM2公钥
/* 输入参数
   index		密钥索引
   lenOfPK	密钥密文字节数
   PK		密钥密文
   输出参数
   无
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

//将ZPK（DES和SM4）加密的pin转成SM2公钥加密
/* 输入参数
   algorithmID	1A	算法标识,1:SM4密钥, 2:3DES密钥
   SM2Index	2N	SM2索引,"00"---"20"
   lenOfSM2     4N      SM2私钥长度
   SM2Value     nB      SM2私钥值
   keyType	1A	密钥类型,1:ZPK, 2:TPK
   keyIndex	1A+3H	密钥索引 
   keyValue	1A+32H	密钥值,3DES 1A为X，SM4 1A 为S
   format	2H	参见pin格式说明
   lenOfAccNo	nN	账号长度 
   accNo	nN	账号
   pinBlock	16/32H	如果算法标识是3des则16H
   如果算法标识是SM4则32H
   sizeofPinByPK 4N	返回值最大值

   输出参数
   pinByPK	nB	公钥加密的密文	
   无
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

	// 算法标识
	sprintf(hsmCmdBuf+hsmCmdLen ,"%d",algorithmID);
	hsmCmdLen += 1;

	// 密钥类型
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

	// pin格式
	memcpy(hsmCmdBuf+hsmCmdLen ,format,2);
	hsmCmdLen += 2;

	// 账号
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

	// SM2密钥索引
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
   功能:转化PIN。
   输入参数:
   keyvalue：加密pin的密钥值
   keyType：密钥类型；0-ZPK，1-TPK。
   srcPinBlock：SM4密钥加密的PIN
   AccNo：帐号供AnsiX9.8算法使用
HasID:	Hash算法0：SM3  	其他标识位保留，暂不实现

输出参数:
PinOfSM3：目的pinblock。
errCode：密码机返回码。

返回值:
<0: 函数执行失败，值为失败的错误码
>=0: 函数执行成功
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


	//写入密钥类型
	memcpy(hsmCmdBuf + hsmCmdLen,keyType, 1);
	hsmCmdLen += 1;

	//写入密钥值	
	sprintf(hsmCmdBuf+hsmCmdLen,"S%s",rootKey);
	hsmCmdLen += 33;

	//插入PIN	
	memcpy(hsmCmdBuf + hsmCmdLen, srcPinBlock, 32);
	hsmCmdLen += 32;
	memcpy(hsmCmdBuf + hsmCmdLen, AccNo, 12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf + hsmCmdLen, HasID, 1);
	hsmCmdLen += 1;


	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdC6:: UnionDirectHsmCmd!\n");
		UnionSetIsCheckHsmResCode();
		return(ret);
	}


	// 设置需要检查响应码
	UnionSetIsCheckHsmResCode();

	memcpy(errCode,hsmCmdBuf+2,2);
	errCode[2] = 0;
	if ((memcmp(errCode,"00",2) == 0))
	{
		// 产生的pinbock
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
   功能: 翻译字符PIN，SM4加密的ANSI9.8格式的pinblock转换为SM4算法网银格式的PIN
   指令: LD
   输入参数:
oriZpkValue: 1A+32H	源zpk密文
oriZpkCheckValue:	16H	外带密钥需要进行密钥校验
desZpkValue: 1A+32H	目的zpk密文
dstZpkCheckValue:	16H	外带密钥需要进行密钥校验
pinBlock:	32H	AnsiX9.8格式的PIN密文
lenOfOriAccNo:	N	源账号长度
oriAccNo:	12N	用户有效主帐号
lenOfDstAccNo:	N	目的账号长度
desAccNo:	12N	用户有效主帐号
sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinLen		N		pin明文长度
pinByZpk:	64H		转加密后的PIN密文
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

	// 源PIN块
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);

	// 源账号
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

	// 连接加密机
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
   功能: PIN密文转换为核心特殊算法加密互转
   指令: LC
   输入参数:
   mode		2N	00：PIN由DES/3DES/SM4加密转为核心特殊加密
   01：PIN由核心特殊加密转为SM4/DES/3DES加密
   algorithmID	2N	1: des/3des
2: SM4
zpkIndex: 	1A+3H	zpk密钥索引号
zpk: 		16H/1A+32H/1A+48H	zpk密文
checkValue:	16H	当ZPK为‘S’+32H的方式时该域存在
format: 	2N	‘01’-‘06’输出的PIN块格式
当PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
pinBlock:	16H/32H/24H	PIN密钥加密的PIN块密文,长度由加密源PIN块的算法决定。
当模式为01核心算法加密的PIN密文
lenOfAccNo:	N	源账号长度
accNo:		12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	16H/32H		PIN密钥加密的PIN块密文,长度由加密PIN块的算法决定。模式为01该域存在。
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
			// 密钥值
			if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk), zpk, hsmCmdBuf+hsmCmdLen)) < 0)
			{
				UnionUserErrLog("in UnionHsmCmdLC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk);
				return(ret);
			}
			hsmCmdLen += ret;

			// PIN块的格式
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%02d",format);
		}
		else	// SM4
		{
			// 密钥值
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S%s",zpk);

			// 校验值
			if ((checkValue == NULL) || (strlen(checkValue) == 0))
			{
				UnionUserErrLog("in UnionHsmCmdLC:: checkValue is null\n");
				return(errCodeParameter);
			}
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",checkValue);

			// PIN块的格式
			hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"01");
		}
	}

	// PIN块
	if ((pinBlock == NULL) || (strlen(pinBlock) == 0))
	{
		UnionUserErrLog("in UnionHsmCmdLC:: pinBlock is null\n");
		return(errCodeParameter);
	}

	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);

	if (format == 1)
	{
		// 源账号
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

	// 连接加密机
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
   功能: 私钥解密对称密钥，再将对称密钥加密的登录密码转为md5或SHA1结果输出
   指令: HR
   输入参数:
   fillMode	1N	0: 如果数据长度小于密钥长度，加密时先在数据前面补
1: PKCS填充方式（一般情况下使用此方式）
vkIndex		2N	索引值"00-20"
lenOfVKValue 	4N	私钥长度
vkValue		nB	私钥值
lenOfKeyByPK	4N	公钥加密的密钥值长度
keyByPK		nB	公钥加密的密钥值
pinBlock 	16H/32H 登录密码密文
random		32H	随机数
algorithmID	1N	1：md5, 2：SHA1
sizeofOutPinBlock	nN	pinByMD5的最大长度
输出参数:
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
	// 公钥填充模式
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",fillMode);

	if ((vkIndex >= 0) && (vkIndex <= 20))
	{
		// 私钥索引
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	}
	else
	{
		// 私钥索引
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"99");

		if ((vkValue == NULL) || (strlen(vkValue) == 0) || (lenOfKeyByPK <= 0))
		{
			UnionUserErrLog("in UnionHsmCmdHR:: vkValue is null\n");
			return(errCodeParameter);
		}
		// 私钥长度
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVKValue/2);

		// 私钥值
		aschex_to_bcdhex(vkValue,lenOfVKValue,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += lenOfVKValue/2;
	}

	// 公钥加密的密钥
	if ((keyByPK == NULL) || (strlen(keyByPK) == 0))
	{
		UnionUserErrLog("in UnionHsmCmdHR:: keyByPK is null\n");
		return(errCodeParameter);
	}

	// 密文密钥长度
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfKeyByPK/2);

	// 公钥加密的密文密钥
	aschex_to_bcdhex(keyByPK,lenOfKeyByPK,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfKeyByPK/2;

	// 密码扩充类型
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

	// 密码密文	
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);

	// 随机数
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",random);

	// 算法标识
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);
	hsmCmdBuf[hsmCmdLen] = 0;

	// 连接加密机
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
   功能: 计算离散、过程密钥
   指令: US
   输入参数:
   mode		2N		离散模式	01: PBOC离散
02: PBOC离散后字密钥与数据异或
algorithmID	1A		离散算法方案 	X：3DES算法	S: SM4算法
rootKeyType	3H		根密钥类型 	008,00A,000,001
rootKeyValue	1A+32H/3H	根密钥		算法与上述离散算法方案保持一致
discreteNum	1N		离散次数	1-3
discreteData	N*16H		离散数据
processFlag	1A		过程密钥标识	Y:有过程密钥
N:不生成过程密钥
processData	32H		生成过程密钥数据
lenOfData	2N		离散模式为02时存在
data		32H		由外部用0补齐,离散模式为02时存在
discreteKeyType	3H		生成离散子密钥类型	008,00A,000,001

输出参数:
keyValue	32H		密钥密文
checkValue	16H		校验值
lenOfOutData	2N		数据长度
outData				密钥明文与数据运算值
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

	// 密钥类型
	if ((ret = UnionTranslateDesKeyTypeTo3CharFormat(rootKeyType, hsmCmdBuf + hsmCmdLen)) <0)
	{
		UnionUserErrLog("in UnionHsmCmdUS::UnionTranslateDesKeyTypeTo3CharFormat!\n");
		return(errCodeParameter);
	}
	hsmCmdLen += 3;

	// 密钥值
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

	// 离散次数
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

	// 连接加密机
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
   SM4国密算法
   输入域	长度	类型	备注
   消息头	8	A	
   命令码	2	A	“WL”
   密钥类型	3	H	ZEK（00A）
   ZAK（008）
   ZMK密文	1A+3H
   1A+32	H	LMK0405下加密的ZMK密文或者ZMK索引
   ZMK校验值	16	H	当密钥为索引方式没有该域。
   密钥密文	32	H	ZMK下加密的密钥密文
   输出域	长度	类型	备注
   消息头	8	A	与输入相同
   响应代码	2	A	“WM”
   状态代码	2	N	正常为“00”
   01：密钥校验值错。
   03：密钥类型错。
   04：模式错误。
   12：索引下无密钥。
   13：LMK错。
   15：输入数据错。
   33：密钥索引错误。
   36：密钥算法标志错
   46：数据长度错。
   81：密钥长度错。
   Key	32	H          	密钥密文，根据密钥类型由指定的LMK对加密
   校验值	16	H	密钥加密16字节0的结果
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
	//拼装指令
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"WL",2);
	hsmCmdLen = 2;	
	//密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen,keytype,3);
	hsmCmdLen+=3;

	//zmk密文值
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

	//zmk校验值
	memcpy(hsmCmdBuf+hsmCmdLen,zmkcheckvalue,16);
	hsmCmdLen+=16;	
	//密钥密文
	memcpy(hsmCmdBuf+hsmCmdLen,zmkData,32);
	hsmCmdLen+=32;

	// 与密码机通讯
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
   功能: PIN块的转加密
   指令: W8
   输入参数:
oriAlgorith:	1N	 源PIN块算法，1-DES/3DES, 2-SM4
desAlgorith:	1N	 目的PIN块算法，1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 源密钥长度标识,1-单倍，2-双倍，3-三倍
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	源zpk索引号或密文
desKeyLenFlag: 	1N	源密钥长度标识,1-单倍，2-双倍，3-三倍
desKeyIndex/desZpkValue: 1A+3H/16/32/48H	目的zpk索引号或密文
oriFormat:	2N	源PIN块格式,‘01’-‘06’输出的PIN块格式
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
desFormat:	2N	目的PIN块格式,‘01’-‘06’输出的PIN块
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字?
pinBlock:	16H/32H		密钥加密的PIN
oriAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
desAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。

sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	16H/32H		转加密后的PIN密文

注意：原W8指令中，当源PIN快格式为03时，源账号域是不存在的。但是在华兴银行中，调用W8指令时，源PIN块格式只能为03并且源账号域写死为12个“0”
 */
int UnionHsmCmdW8_HUAX(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk)
{
	int	ret;
	char	hsmCmdBuf[2048];
	int	hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"W8",2);
	hsmCmdLen = 2;

	// 源PIN块算法
	if (oriAlgorith == 1 || oriAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: oriAlgorith[%d] is invalid!\n",oriAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 目的PIN块算法
	if (desAlgorith == 1 || desAlgorith == 2)
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desAlgorith);
	else
	{
		UnionUserErrLog("in UnionHsmCmdW8:: desAlgorith[%d] is invalid!\n",desAlgorith);
		return(errCodeParameter);

	}
	hsmCmdLen += 1;

	// 源密钥长度标识
	if (oriAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",oriKeyLenFlag);
		hsmCmdLen += 1;
	}

	// 源密钥密文或索引号
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

	// 目的密钥长度
	if (desAlgorith == 1)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",desKeyLenFlag);
		hsmCmdLen += 1;
	}

	// 目的zpk密钥
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

	// 源PIN块的格式
	if (oriAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",oriFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 目的PIN块的格式
	if (desAlgorith == 1)
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",desFormat);
	else
		sprintf(hsmCmdBuf+hsmCmdLen,"%s","01");
	hsmCmdLen += 2;

	// 源PIN块
	sprintf(hsmCmdBuf+hsmCmdLen,"%s",pinBlock);
	hsmCmdLen += strlen(pinBlock);

	// 源账号
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

	// 目的账号
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

	// 连接加密机
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
   功能:将PIN由X9.8转为特殊算法加密
   输入参数：
   zpk		16H/32H/48H	zpk密钥值
   accNo		12N		账号
   pinBlock	16H		zpk加密的pin密文数据
   algFlag  1A 目的edk标志
   edk		16H/32H/48H	edk密钥
   输出参数：
   pin	16H	返回的pin块
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 指令
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

	// 与密码机通讯
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
   功能:将ZPK加密的PIN进行弱密码检测
   输入参数：
   algorithmID	1N		算法标识,1:des/3des, 2:SM4
   keyIndex	1A+3H		密钥索引号
   zpk		16H/1A+32H/1A+48H	zpk密钥值
   checkValue	16H		当ZPK为国密时，此域存在
   format	2N		PIN块格式,当PIN块算法为SM4，该域指示的格式只能为"01"
   pinBlock	16H		zpk加密的pin密文数据
   accNo	12N/18N		账号
   rule		1N		0:6位密码为相同的数字
1:使用逐项递增/递减1的顺序数字组合
2:规则包含0和1两种弱密码
输出参数：
返回值：
<0：函数执行失败，值为失败的错误码
=0:	0:表示非弱密码
=1:	1:表示弱密码

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

	// 指令
	memcpy(hsmCmdBuf,"ZY",2);
	hsmCmdLen = 2;

	// PIN块算法
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);

	if ((keyIndex != NULL) && (strlen(keyIndex) > 0))
	{	// 密钥索引号
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

	// pin块的格式
	if (algorithmID == 1)
	{
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",format);
	}
	else
	{
		// 国密算法，只能为01格式
		hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"01");
	}

	// PIN块
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

	// PIN块
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%d",rule);

	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
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
   功能：将由公钥加密的PIN转换成ANSI9.8标准

   输入域  长度    类型    说　明
   命令代码        2       A       值"HD"
   算法标识        1       N       0
   私钥索引        2       N       指定的私钥，用于解密PIN数据密文
   填充方式        1       N       0       1
   源PIN格式       2       N       07深圳农商行专用
   目的PIN格式     2       N       01 ANSI9.8
   密钥类型        3       N       001 ZPK
   ZPK值           16/32+A H       用于加密PIN
   原账号          12      N       当源PIN格式为01时存在
   目的账号        12      N       当目的PIN格式为01时存在
   PIN密文长度     4       N
   PIN密文         n       B       经公钥加密的PIN数据密文


   输出域  长度    类型    说　明
   响应代码        2       A       值"HE"
   错误代码        2       H
   PIN密文         16      H       返回的PIN密文--ANSI9.8
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

	// 指令
	memcpy(hsmCmdBuf,"HD",2);
	hsmCmdLen = 2;

	// 算法标识
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",AlgorithmMode);

	// 私钥索引
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",vkIndex);

	//add begin by lusj 20160122 平安银行HD指令支持私钥外带功能！６褂肏D指令的只有广州农商的EEZ1服务，该服务以作兼容处理
	if(strcmp(vkIndex, "99") == 0)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmdHD:: vk is null!\n");
			return(errCodeParameter);
		}

		//私钥长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//私钥
		aschex_to_bcdhex(vk,lenOfVK,tmpBuf);
		tmpBuf[lenOfVK/2]=0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;
	}	
	//end by lusj 20160122

	// 填充方式
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",DataPadFlag);

	// 源PIN格式
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",FirstPinCiperFormat);

	// 目的PIN格式
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%s",SecondPinCiperFormat);

	// 密钥类型 001 ZPK
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"001");

	// ZPK值
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdHD:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;

	// 原账号
	if(strlen(conEsscFldAccNo1) != 0 )
	{
		memcpy(hsmCmdBuf+hsmCmdLen,conEsscFldAccNo1,12);
		hsmCmdLen += 12;
	}

	// 目的账号
	if(strlen(conEsscFldAccNo2) != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,conEsscFldAccNo2,12);
		hsmCmdLen += 12;
	}

	// PIN密文长度
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"%04d", lenOfPinByPK);

	// PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;	

	hsmCmdBuf[hsmCmdLen] = 0;
	
	bcdhex_to_aschex(hsmCmdBuf,hsmCmdLen,tmpBuf);	
	tmpBuf[2*hsmCmdLen]=0;
	UnionLog("in UnionHsmCmdHD:: hsmCmdBuf[%s]!\n",tmpBuf);


	// 与密码机通讯
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
   功能:	导入四川农信私有算法密钥
   描述：	密钥由3个成份组成，每个成份不足8个字节时，后补0.
   在加密机内开辟专门的存储区存储，不再带索引。

   输入域   	长度类型    说　明
   命令代码        2A      	值"SJ"
   key1		16H		LMK0607下加密
   key2		16H		LMK0607下加密
   key3		16H		LMK0607下加密

   输出域  	长度类型	说明
   响应代码        2A       	值"SK"

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

  功能:将标准PinBlock转为定制算法加密的Pin

  输入域   	长度类型    说　明
  命令代码        2A      	值"SN"
  mode		1N      	模式标志,1:加密存折PIN
5:加密卡PIN
keyValue	16H/32H/	ZPK密钥值
1A+32H/1A+48H
pinType		2A		01：ANSIx9.8格式
accout		12H		PIN账号
pinBlock	16H		PIN密文,ZPK下加密的PIN密文，明文pin格式是6位数字
sizeOfEncPin	nN		返回密文长度

输出域  	长度类型	说明
响应代码        2A       	值"SO"
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

  功能：加密明文pin(四川农信)

  输入域   	长度类型    说　明
  命令代码        2A      	值"SP"
  mode		1N      	模式标志,1:加密存折PIN
5:加密卡PIN
pin		6H		pin明文,6位数字
sizeOfEncPin	nN		返回密文长度

输出域  	长度类型	说明
响应代码        2A       	值"SQ"
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
   函数功能：
   SH指令，产生VISA卡校验值CVV
   输入参数：
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   cvv：生成的VISA卡的CVV
   返回值：
   <0，函数执行失败，无cvv
   >0，成功，返回cvv的长度
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
	// 命令字
	memcpy(hsmCmdBuf,"SH",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSH:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 客户帐号
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA卡的有效期
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA卡服务商代码
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSH:: UnionDirectHsmCmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	// 生成的VISA卡的CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	cvv[3] = 0;
	return(3);
}

/*
   函数功能：
   XS指令，验证VISA卡的CVV
   输入参数：
   cvv：待验证的VISA卡的CVV
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   errCode：错误代码
   返回值：
   <0，函数执行失败
   0，验证成功
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
	// 命令字
	memcpy(hsmCmdBuf,"XS",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdXS:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 待验证的VISA卡的CVV
	memcpy(hsmCmdBuf+hsmCmdLen,cvv,3);
	hsmCmdLen += 3;
	// 客户帐号
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA卡的有效期
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA卡服务商代码
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
   指令:PQ
   功能:PIN转加密（PKCS#7加密转ZPK加密）
   青岛银行定制指令
   输入参数:
   mode  	算法标志  				1A  
   0-	DES/RSA
   1-	SM4/SM2
   vkIndex	RSA/SM2私钥索引			2 N
   lenOfvk	RSA/SM2私钥密文长度		4 N
   vk		RSA/SM2私钥密文			n B	
   当私钥索引为"99"时存在
   LMK加密的私钥（包括e,d,m和5个CRT成份）
   lenOfKeyByPk	被加密会话密钥长度		4 N
   RSA/SM2公钥加密的会话(DES/SM4密钥)密钥长度
   keyByPk	被加密的会话密钥			n B				
   RSA/SM2公钥加密的会话(DES/SM4密钥)密钥。
   这里要求DES密钥必须符合强制奇校验。
   dateType	加密数据模式			1N					
   0-	PIN明文
   1-	PIN索引
   lenOfpinBlock	被加密的PIN或PIN索引长度	4 N
   DES/SM4密钥加密的PIN明文或PIN索引长度
   Pin		被加密的PIN密文/索引		n B
   DES/SM4密钥加密的PIN明文或PIN索引。（PKCS#5填充）
   PinTab	PIN索引转换表			10 N
   当加密数据模式为1的时候该域存在。
   accNo		账号					12 N	用户主账号，有效位的最有12个数字。
   keyOfZpk	目标ZPK密钥				16 H/1A + 32 H/1A + 48 H						
   LMK下加密的目标ZPK密文
   当算法标志为1时，该域强制为S+32H
   当算法标志为0时，该域为X+32H
   format	PINBLOCK格式			2 N	
   PIN数据块的格式代码，参见PINBLOCK格式
   PIN格式为08时，为青岛银行PIN格式。
   输出参数:
   pinlen	明文PIN的长度			2 N	
   dst_pinBlock	PINBLOCK密文			16 H/32H

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

	// 算法模式
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",mode);
	hsmCmdLen += 1;

	// 索引号
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
   青岛银行定制指令

   功能: 
   数据由SM2算法公钥加密转加密为SM4算法加密。

   输入参数：
   vkIndex	私钥索引	2N
   lenOfvk	私钥长度	4N
   vk		私钥密文	nB
   keyType	对称密钥类型	3H
   keyValue	对称密钥	1A+32H
   lenOfdata	数据长度	4N
   data		数　据	nB

   输出参数：
   cipherData	对称密钥加密后的密文	n*2

   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
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

	// 指令
	memcpy(hsmCmdBuf,"PR",2);
	hsmCmdLen = 2;

	// 填充模式
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;

	// 索引号
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

	//密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdPR:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//密钥值
	memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue,32);
	hsmCmdLen += 32;

	//数据
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfdata/2);
	hsmCmdLen += 4;

	len = aschex_to_bcdhex(data,lenOfdata,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += len;

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	// 与密码机通讯
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
   函数功能：
   9F指令，使用湖南农信原核心算法校验密码后再使用SM4PVK密钥生成一个PIN offset。

   输入参数：
   pvk					pvk					1A+32H			LMK对（14-15）下加密的PVK；用于生成offset。（SM4)
   zpk					zpk					1A+32H			LMK对（06-07）下加密的ZPK；
   algorithmID			算法标示		1N  				1：DES算法加密PINBLOCK
   2：SM4算法加密PINBLOCK
   pinBlockByZPK		PINBLOCK密文		16H/32H		被ZPK加密的PINBLOCK
   pinLength				PINBLOCK密文长度
   minPINLength			检查长度		2N				最小的PIN长度。
   accNo1				账号1				12N				加密PINBLOCK的账号
   lenOfAccNo2			账号2长度		2N	
   accNo2				账号2				N				湖南农信核心老算法账号
   checkPin				待校验的密码	8A			湖南农信老算法计算出的密码密文
   decimalizationTable	十进制转换表	16H			将十六进制转换为十进制的转换表。
   checkData			PIN校验数据		12A			用户定义的、包含十六进制字符和字符"N"的数据，用来指示HSM插入账号最后5位的位置。

   输出参数：
   pinOffset：PIN Offset，左对齐，右补'F'

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

	// 命令字
	memcpy(hsmCmdBuf,"9F",2);
	hsmCmdLen = 2;

	// LMK加密的PVK
	hsmCmdLen += sprintf(hsmCmdBuf+hsmCmdLen,"S");
	memcpy(hsmCmdBuf+hsmCmdLen,pvk,32);
	hsmCmdLen += 32;

	//zpk密文
	if (algorithmID == 1)
	{
		//DES算法
		memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zpk,32);
		hsmCmdLen += 32;
	}
	else
	{
		//SM4算法
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen,zpk,32);
		hsmCmdLen += 32;
	}

	// 算法标示
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;


	//PINBLOCK密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,pinLength);
	hsmCmdLen += pinLength;

	//检查长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;

	//账号1
	if ((ret = UnionForm12LenAccountNumber(accNo1,lenOfAccNo1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmd9F:: UnionForm12LenAccountNumber for [%s]\n",accNo1);
		return(ret);
	}
	hsmCmdLen += ret;


	//账号2长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAccNo2);
	hsmCmdLen += 2;

	//账号2
	memcpy(hsmCmdBuf+hsmCmdLen,accNo2,lenOfAccNo2);
	hsmCmdLen += lenOfAccNo2;	

	//待校验的密码
	memcpy(hsmCmdBuf+hsmCmdLen,checkPin,8);
	hsmCmdLen += 8;	

	//十进制转换表
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;

	//PIN校验数据
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
   柳州银行定制指令

   函数功能：
   公钥加密PIN转成DES加密（支持字符，国密算法）

   输入参数：

   algorithmID	  		算法标示				1N		  	1：RSA	   2：SM2
   vkIndex			私钥索引    				2N   		"00"－"20"  	"99"：使用外带密钥
   lenOfVK   			外带私钥长度		4N			仅当密钥索引为99时有此域，下一个域长度		
   vk					外带私钥
   fillMode				用公钥加密时所		1N 	 	 	当算法标示为：RSA时有此域		
   采用的填充方式					
   ZEKValue			zek值		
   format				补位方式				1N   		1：填充0x00：    2：填充0x80：
   encrypMode			加密模式				1N   		1:ECB	 2:CBC
   dataPrefixLen		填充数据前缀长度	2N			如果是：00，侧没有前缀填充数据
   dataPrefix			填充数据前缀		3H			当填充数据长度前缀缀不是00时有此域
   dataSuffixLen		填充数据后缀长度	2N			如果是：00，侧没有后缀填充数据
   dataSuffix			填充数据后缀		3H			当填充数据后缀长度不是00时有此域
   lenOfPinByPK		密文长度				4N
   PinByPK			公钥加密密文   		nB   			经公钥加密的PIN数据密文SM2算法为c1+c3+c2
   specialAlg  			专用算法标示    		1A 			可选项
   P-密码安全控件安全算法
   Q-判断密码复杂度（密码控件安全算法）
   输出参数：
   PinByZek			PIN密文					nH			zek加密的数据长度
   complexityOfPin		 PIN密码复杂度              2N 	 		当专用算法为"Q"时有此域
   01：纯数字
   02：数字+字母
   03：数字+字母（区分大小写）
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


	//加密机指令
	memcpy(hsmCmdBuf,"6A",2);
	hsmCmdLen = 2;

	//算法标识
	if((algorithmID !=1) && (algorithmID !=2))
	{
		UnionUserErrLog("in UnionHsmCmd6A:: algorithmID is not 1 or 2!\n");
		return(errCodeParameter);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;


	//私钥索引
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;	

	if(vkIndex==99)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmd6A:: vk is null!\n");
			return(errCodeParameter);
		}

		//私钥长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//私钥
		aschex_to_bcdhex(vk,lenOfVK,tmpBuf);
		tmpBuf[lenOfVK/2]=0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;	
	}

	if(algorithmID == 1)
	{
		//用公钥加密时所采用的填充方式
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
		//密钥类型
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

		//补位方式
		sprintf(hsmCmdBuf+hsmCmdLen,"%d",format);
		hsmCmdLen += 1;	

		//加密模式
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

		//填充数据前缀长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",dataPrefixLen);
		hsmCmdLen += 2;	

		if(dataPrefixLen !=0)
		{
			//前缀数据
			memcpy(hsmCmdBuf+hsmCmdLen,dataPrefix,dataPrefixLen);
			hsmCmdLen +=dataPrefixLen; 	
		}

		//填充数据后缀长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",dataSuffixLen);
		hsmCmdLen += 2;		

		if(dataSuffixLen !=0)
		{
			//填充数据后缀
			memcpy(hsmCmdBuf+hsmCmdLen,dataSuffix,dataSuffixLen);
			hsmCmdLen +=dataSuffixLen; 	
		}
	}

	//密文长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;		

	//密文
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
/*将lmk加密密钥转换为pk加密
  输入参数：
  keyType 密钥类型
  keyValue LMK密钥加密密文
  checkValue 校验值
  pkIndex  公钥索引
  lenOfPK  公钥长度
  pk 公钥值
  输出参数
  lenOfKeyByPK PK加密的密钥值长度
  keyByPK  PK加密的密钥值
  pkHash   密钥明文HASH
  返回值：
  PK加密的密钥值长度

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

	//密钥标识
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

		// 私钥索引
		sprintf(hsmCmdBuf+offset,"%02d",pkIndex);
		offset += 2;
		memcpy(hsmCmdBuf + offset,pk,lenOfPK);//lenOfPK = 64
		offset += lenOfPK;


	}	
	else if ((pkIndex >= 0) && (pkIndex <= 20))
	{
		// 私钥索引
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
   将ZPK加密机的PIN转为银行专用算法得到PIN密文输出
   输入参数：
   zpkKeyValue     16H或1A+32H或1A+48H     源ZPK           当前加密PIN块的ZPK；LMK对（06-07）下加密
   pinBlock        16H                     源PIN块         源ZPK下加密的源PIN块
   pinFormat       2N                      源PIN块格式
   accout          12N                     账号            账号中去除校验位的最右12位
   resPinFormat    2N                      目的PIN块算法   01：无锡农商行专用算法  02：太仓农商行专用算法  03：南京银行专用算法 04: 徽商银行专用算法
   parameter       8A/12A/16H              银行传入参数    8A太仓银行 12A南京银行	16H徽商银行
   输出参数：
   resPinBlock     16H     目的PIN块
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
	// 徽商银行算法密钥固定为16H
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
	// 南京银行专用算法的没返回长度
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
   将南京银行专用算法的PIN密文转为X9.8格式的PIN密文输出
   输入参数：
   parameter	12A			银行传入参数
   resPinFormat    2N                      目的PIN块算法     01：ANSI9.8
   zpkKeyValue     16H或1A+32H或1A+48H     目的ZPK           LMK对（06-07）下加密
   accout          12N                     账号              账号中去除校验位的最右12位
   pinBlock        16H                     Pin块密文         南京银行专用算法加密
   输出参数：
   resPinBlock     16H			目的PIN块
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
   登录密码初始化，把SM2公钥加密的密文，转换成SM3/ZPK国密算法加密输出
   输入参数：
   mode		1A			1-	产生PIN随机因子，SM3摘要输出
   2-	外带PIN随机因子，SM3摘要输出
   3-	SM2公钥加密转为ZPK（SM4算法）加密输出
   vkIndex		2N			00-20，99表示SM2外带
   vkLen		4N			仅当密钥索引为99时有此域
   vkValue     	nB			仅当密钥索引为99时有此域，SM2密钥密文
   lenOfPinByPK	4N			密文长度
   pinByPK		nB			SM2公钥加密的密文。（C1+C3+C2）
   factorData	8H			防重放因子，由外部传入，私钥解密密文后，获得因子明文，由传入因子比对
   keyValue	1A+32H			PIN随机因子密钥/ZPK, PIN随机因子与PIN明文结合组合成PINBLOCK，随机因子由加密机随机生成32位16进制数。
   当模式为1、2密钥类型为ZEK（SM4密钥），
   当模式为3为ZPK(SM4密钥)。
   randomData	32H			PIN随机因子密文
   lenOfAccNo	2N			账号长度
   accNo		nN			账号

   输出参数：
   pinByZEK	32H			当模式为1，由ZEK加密的pin随机因子密文。
   digest		64H			SM3算法摘要。当模式标志为1、2有此域
   pinBlock	32H			当模式为3，Zpk加密的PINBLOCK密文（ANSI 9.8格式）。
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

	// 命令代码
	memcpy(hsmCmdBuf, "8B", 2);
	hsmCmdLen += 2;

	// mode
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mode);
	hsmCmdLen += 1;

	// vkIndex
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	hsmCmdLen += 2;

	// 私钥值
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

	// 公钥加密的数据长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);	
	hsmCmdLen += 4;

	// 公钥加密的数据密文
	aschex_to_bcdhex(pinByPK,lenOfPinByPK,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// 防重放因子
	memcpy(hsmCmdBuf+hsmCmdLen,factorData ,8);
	hsmCmdLen += 8;

	// 密钥值
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"S%s",keyValue);
	hsmCmdLen += 33;

	// PIN随机因子密文
	if (mode == 2)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, randomData, 32);
		hsmCmdLen += 32;
	}

	// 账号
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
   ZMK依次对输入数据进行n级离散，经奇校验处理后，并按指定的密钥类型输出
   输入参数：
   zmkKey		16H/1A+32H/1A+48H	LMK加密的ZMK
   keyFlag		1N			密钥标志位
   0：ZMK
   1：EDK
   2：ZPK
   3：ZAK
   bankID		16			离散数据1
   PSAMID		16			离散数据2（可选）
   random		16			离散数据3（可选）
   输出参数：
   keyValue	32H			lmk加密的密文
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

	// 命令代码
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

	// 银行ID
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
   平安银行	PH指令

   函数功能：
   把SM2公钥加密机的密码转换为专用算法加密

   输入参数：
   ID			算法ID		1A			   1-  把SM2公钥加密的密码转为企业网银登录密码专用算法加密
   								   2-  把SM2公钥加密的密码转为个人网银登录密码专用算法加密
   								   3-  把SM2公钥加密的密码转为MD5算法加密
   								   4-  把SM2公钥加密的密码转为ZPK加密
   vkIndex		私钥索引	 	2N			   00-20，99表示SM2外带
   lenOfVK		私钥长的		4N			   仅当密钥索引为99时有此域，下一个域长度
   vk			私钥密文		nB			   仅当密钥索引为99时有此域，SM2密钥密文
   lenOfPinByPK		密文长度		4N			
   PinByPK		密文			nB			   SM2公钥加密的密文。（C1+C3+C2）
   ZPKValue		zpk			1A+16 H /32 H /48 H		当ID=1，4时有该域，DES算法ZPK
   lenOfaccNO		帐号长度或者附件信息长度	2N			当ID=1时有该域，4~20
   accNO		帐号（PAN）或者附件信息		N			当ID=1时有该域，用户主帐号或者附件信息，若为全"0"，则该域不参与加密PIN的运算
   										当ID=4时有该域，用户有效账号12位

   输出参数：
   referFactor	重放因子	
   PinBlock		pin密文					当ID=1时，企业网银登录密码专用算法加密密文 32H
								  ID=2时，个人网银登录密码专用算法加密密文 28B
  				 			        当ID=3时，MD5算法加密密文 				 32H
							        当ID=4时，ZPK加密的密文，ZPK加密（DES算法）的密文，ANSI 9.8格式 16H
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


	//加密机指令
	memcpy(hsmCmdBuf,"PH",2);
	hsmCmdLen = 2;

	//算法ID
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",ID);
	hsmCmdLen += 1;


	//私钥索引
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;	

	if(vkIndex==99)
	{
		if((lenOfVK==0)||(vk==NULL))

		{
			UnionUserErrLog("in UnionHsmCmdPH:: vk is null!\n");
			return(errCodeParameter);
		}

		//私钥长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK/2);
		hsmCmdLen += 4;

		//私钥
		aschex_to_bcdhex(vk,lenOfVK,tmpBuf);
		tmpBuf[lenOfVK/2]=0;
		memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfVK/2);
		hsmCmdLen += lenOfVK/2;	
	}

	//密文长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;		

	//密文
	aschex_to_bcdhex(PinByPK,lenOfPinByPK,tmpBuf);	
	tmpBuf[lenOfPinByPK/2]=0;
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,lenOfPinByPK/2);
	hsmCmdLen += lenOfPinByPK/2;


	//zpk 当ID=1，4时有该域，DES算法ZPK
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

	//帐号长度或者附件信息长度
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
   将ZPK加密的pinBlock转换成MD5，SHA-1和SM3
   输入参数：
   algorithmID		1N			PIN块算法
						1： DES/3DES
						2： SM4
   keyType		3H			密钥类型
						001： ZPK
						002： TPK
   keyValue		1A+3H/16H/32H/48H	密钥值
   checkValue		16H			校验值，当为SM4算法时有该域
   format		2N			PIN块格式
   pinBlock		16H/32H			密钥加密的PIN块
   accNo		N			账号
   hashID		2N			哈希算法标识
   dataOffset		4N			加盐偏移
   length		4N			加盐长度
   data			nH			加盐数据
   输出参数：
   hash			nH			哈希
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

	// 命令代码
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
                                                                                                        
        // 命令代码                                                                                     
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
	
	// 源zpk
	if((ret = UnionPutKeyIntoRacalKeyString(srcZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW3:: UnionPutKeyIntoRacalKeyString [%s]!\n", srcZPK);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// 目的密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(dstKeyType, keyType);
	memcpy(hsmCmdBuf+hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;

	// 目的密钥
	if((ret = UnionPutKeyIntoRacalKeyString(dstKeyValue, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW3:: UnionPutKeyIntoRacalKeyString [%s]!\n", dstKeyValue);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// 最大PIN长度
	len = sprintf(hsmCmdBuf + hsmCmdLen, "%d", maxPinLen);
	hsmCmdLen += len;
	
	// 源PIN块
	memcpy(hsmCmdBuf + hsmCmdLen, srcPinBlock, 16);
	hsmCmdLen += 16;
	
	// 源PIN格式
	memcpy(hsmCmdBuf + hsmCmdLen, format, 2);
	hsmCmdLen += 2;
	
	// 账号
	if((ret = UnionForm12LenAccountNumber(accNo, strlen(accNo), hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdW3:: UnionForm12LenAccountNumber for [%s]!\n", accNo);
		return(ret);
	}
	hsmCmdLen += 12;
	
	// 网银用户名
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
	
	// 源zpk
	if((ret = UnionPutKeyIntoRacalKeyString(srcZPK, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdH6:: UnionPutKeyIntoRacalKeyString [%s]!\n", srcZPK);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// PIN明文，已经扩展并填充为48位的pin块
	if(lenOfPin != 48)
	{
		UnionUserErrLog("in UnionHsmCmdH6:: lenOfPin[%d] error!\n", lenOfPin);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, pin, lenOfPin);
	hsmCmdLen += lenOfPin;
	
	// 账号
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
	
	// 源ZPK
	if((ret = UnionPutKeyIntoRacalKeyString(srcZPK, hsmCmdBuf+hsmCmdLen, sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdY8:: UnionPutKeyIntoRacalKeyString [%s]!\n", srcZPK);
		return(ret);
	}
	else
		hsmCmdLen += ret;
	
	// 已经扩展并填充为48位的pin块
	if(lenOfPin != 48)
	{
		UnionUserErrLog("in UnionHsmCmdY8:: lenOfPin[%d] error!\n", lenOfPin);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen, pin, lenOfPin);
	hsmCmdLen += lenOfPin;
	
	// 账号
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
	功能：将对由SM2公钥加密的数据，用SM2私钥解密后，生成MAC
	指令：4I
	输入参数：
		vkIndex		2N	SM2密钥索引,00 - 20, 99为外带密钥
		lenOfVkValue	4N	仅当密钥索引为99是有此域,私钥长度
		vkValue		nB	仅当密钥索引为99是有此域,私钥
		lenOfPinByPK	4N	公钥加密的密文长度
		pinByPK		nB	公钥加密的密文
		lenOfRandom	2N	0-20,随机数长度
		random		nA	随机数
		zak算法标识	1N	0:国密SM4算法, 1:国际DES/3DES算法
		zak		16H/1A+32H/1A+48H/	对MAC数据产生MAC的密钥
	输入出参数：
		mac		16H/32H	当密钥为国际算法时，返回的MAC-ANSI 9.9,�,16位长度
					当密钥为国密算法时，返回的MAC-ANSI 9.9,32位长度
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

	// 私钥索引
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

	// 密文长度
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;

	// 密文
	aschex_to_bcdhex(pinByPK, lenOfPinByPK, hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// 随机数长度
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%02d",lenOfRandom);
	hsmCmdLen += 2;

	// 随机数
	memcpy(hsmCmdBuf+hsmCmdLen,random,lenOfRandom);
	hsmCmdLen += lenOfRandom;

	// zak算法标识
	snprintf(hsmCmdBuf + hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;

	// zak密钥值
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
	功能：将对由SM2公钥加密的PIN转成ANSI9.8标准
	指令：4J
	输入参数：
		vkIndex		2N	SM2密钥索引,00 - 20, 99为外带密钥
		lenOfVkValue	4N	仅当密钥索引为99是有此域,私钥长度
		vkValue		nB	仅当密钥索引为99是有此域,私钥
		lenOfPinByPK	4N	公钥加密的密文长度
		pinByPK		nB	公钥加密的密文
		lenOfRandom	2N	0-20,随机数长度
		random		nA	随机数
		zpk算法标识	1N	0:国密SM4算法, 
					1:国际DES/3DES算法
		zpk		16H/1A+32H/1A+48H/32H	对MAC数据产生MAC的密钥,当为国密时，长度为32H
	输入出参数：
		pinByZPK	16H/32H	当密钥为国际算法时，返回的PIN密文为ANSI 9.8,�,16位长度
					当密钥为国密算法时，返回的PIN密文为ANSI 9.8,32位长度
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

	// 私钥索引
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

	// 密文长度
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%04d",lenOfPinByPK/2);
	hsmCmdLen += 4;

	// 密文
	aschex_to_bcdhex(pinByPK, lenOfPinByPK, hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;

	// 随机数长度
	snprintf(hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%02d",lenOfRandom);
	hsmCmdLen += 2;

	// 随机数
	memcpy(hsmCmdBuf+hsmCmdLen,random,lenOfRandom);
	hsmCmdLen += lenOfRandom;

	// zpk算法标识
	snprintf(hsmCmdBuf + hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen,"%d",algorithmID);
	hsmCmdLen += 1;

	// zpk密钥值
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

	// 用户主账号
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
   功能:将公钥加密的数据转化为MD5后输出
   输入参数：
   keyIndex        RSA密钥索引
   vkLen           RSA外带密钥长度
   vkValue         RSA外带密钥
   lenOfPinByPK    SM2公钥加密的密文长度
   pinByPK         密文
   dataFillMode    用公钥加密时所采用的填充方式
   DataFillHeadLen  前填充数据长度
   DataFillHead     前填充数据
   DataFillTailLen  后填充数据长度
   DataFillTail     后填充数据
   hash             MD5运算后的结果
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
	
	 //命令码
	memcpy(hsmCmdBuf,"S9",2);
	hsmCmdLen = 2;
	//SM2密钥索引
	memcpy(hsmCmdBuf + hsmCmdLen, keyIndex, 2);
	hsmCmdLen += 2;
	
	if(!strncmp(keyIndex, "99", 2))
	{
		//SM2外带密钥长度
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", vkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(vkValue, vkLen, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += vkLen/2;
	}
	//用公钥加密时所采用的填充方式
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
	
	//密文长度
	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", lenOfPinByPK/2);
	hsmCmdLen += 4;
	aschex_to_bcdhex(pinByPK,lenOfPinByPK , hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfPinByPK/2;
	
	//前填充数据
	if(DataFillHeadLen)
	{
		//前填充分隔符
		memcpy(hsmCmdBuf + hsmCmdLen, ";", 1);
        hsmCmdLen += 1;
		
		//前填充数据长度
		sprintf(hsmCmdBuf + hsmCmdLen, "%02d", DataFillHeadLen);
		hsmCmdLen += 2;
		
		//前填充数据
		memcpy(hsmCmdBuf + hsmCmdLen, DataFillHead, DataFillHeadLen);
		hsmCmdLen += DataFillHeadLen;
	}
	//后填充数据
	if(DataFillHeadLen)
	{
		//后填充分隔符
		memcpy(hsmCmdBuf + hsmCmdLen, "M", 1);
        hsmCmdLen += 1;
		//后填充数据长度
		sprintf(hsmCmdBuf + hsmCmdLen, "%02d", DataFillTailLen);
		hsmCmdLen += 2;	
		//后填充数据
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

