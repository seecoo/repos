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
//int gunionIsUseSpecZmkType = 1;
extern int gunionIsUseSpecZmkType;

// add by lisq 20141021 交通部专用指令集
/*
   功能
   生成一个密钥，以LMK加密输出，同时可选使用ZMK加密输出或保存在指定索引下
   输入参数
   mode	模式，0：产生密钥；1：产生密钥并在ZMK下加密；2：产生密钥并保存在指定索引下
   keyType	密钥类型
   keyModel	密钥方案表。0：单倍长DES密钥；1：双倍长DES密钥；2：三倍长DES密钥；3：SM4密钥；4：SM1密钥；5：AES密钥
   zmk	LMK下加密的ZMK或密钥索引。仅当模式为1时有此域
   keyIndex	密钥索引。产生密钥存放位置，仅当模式为2时有此域
   输出参数
   keyByLMK	LMK加密下的密钥
   keyByZMK	ZMK加密下的密钥。仅当模式为1时有此域
   checkValue	密钥校验值
   返回值
   >0：成功	
   <0：失败
 */
int UnionJTBHsmCmdA0(int mode,TUnionDesKeyType keyType,int keyModel,char *zmk,int keyIndex,char *keyByLMK,char *keyByZMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512+1];
	int	hsmCmdLen = 0;
	int	zmkLength,keyLen;
	int	offsetOfKeyByLMK,offsetOfKeyByZMK,offsetOfCheckValue;
	int	cvLen;

	memcpy(hsmCmdBuf,"A0",2);
	hsmCmdLen = 2;

	if (mode == 0)
		hsmCmdBuf[hsmCmdLen] = '0';
	else if(mode == 1)
		hsmCmdBuf[hsmCmdLen] = '1';
	else if(mode == 2)
		hsmCmdBuf[hsmCmdLen] = '2';
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdA0:: invalid parameter mode [%d]\n", mode);
		return(errCodeParameter);
	}
	hsmCmdLen++;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdA0:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	switch(keyModel)
	{
		case 0:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			break; 
		case 1: 
			hsmCmdBuf[hsmCmdLen] = 'X'; 
			break; 
		case 2: 
			hsmCmdBuf[hsmCmdLen] = 'Y'; 
			break; 
		case 3: 
			hsmCmdBuf[hsmCmdLen] = 'S'; 
			break; 
		case 4: 
			hsmCmdBuf[hsmCmdLen] = 'P'; 
			break; 
		case 5: 
			hsmCmdBuf[hsmCmdLen] = 'L'; 
			break; 
		default: 
			UnionUserErrLog("in UnionJTBHsmCmdA0:: invalid parameter keyModel [%d]\n", keyModel); 
			return(errCodeParameter); 
	}
	hsmCmdLen += 1;

	if (mode == 1)
	{
		switch (zmkLength=strlen(zmk))
		{
			case	4:
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
				UnionUserErrLog("in UnionJTBHsmCmdA0:: invalid zmkLength ! zmk = [%s] zmkLen = [%d]\n",zmk,zmkLength);
				return(errCodeParameter);
		}
		hsmCmdLen += ret;

		memcpy(hsmCmdBuf+hsmCmdLen,zmk,zmkLength);
		hsmCmdLen += zmkLength;

		switch(keyModel)
		{
			case 3:
				hsmCmdBuf[hsmCmdLen] = 'S';
				hsmCmdLen += 1;
				break;
			case 4:
				hsmCmdBuf[hsmCmdLen] = 'P';
				hsmCmdLen += 1;
				break;
			case 5:
				hsmCmdBuf[hsmCmdLen] = 'L';
				hsmCmdLen += 1;
				break;
			default:
				ret = UnionTranslateHsmKeyKeyScheme(strlen(zmk),hsmCmdBuf+hsmCmdLen);
				hsmCmdLen += ret;
				break;
		}
	}

	if (mode == 2)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "K%03d", keyIndex);
		hsmCmdLen += 4;
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdA0:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offsetOfKeyByZMK = -1;
	switch (keyModel)
	{
		case    0:
			offsetOfKeyByLMK = 4;
			if (mode == 1)
			{
				offsetOfKeyByZMK = 4 + 16;
				offsetOfCheckValue = 4 + 16 + 16;
			}
			else
				offsetOfCheckValue = 4 + 16;
			keyLen = 16;
			break;
		case    1:
		case    3:
		case    4:
		case    5:
			offsetOfKeyByLMK = 4 + 1;
			if (mode == 1)
			{
				offsetOfKeyByZMK = 4 + 1 + 32 + 1;
				offsetOfCheckValue = 4 + 1 + 32 + 1 + 32;
			}
			else
				offsetOfCheckValue = 4 + 1 + 32;
			keyLen = 32;
			break;
		case    2:
			offsetOfKeyByLMK = 4 + 1;
			if (mode == 1)
			{
				offsetOfKeyByZMK = 4 + 1 + 48 + 1;
				offsetOfCheckValue = 4 + 1 + 48 + 1 + 48;
			}
			else
				offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;	 // Mary add, 2004-4-12
		default:
			UnionUserErrLog("in UnionJTBHsmCmdA0:: invalid keyLength type [%d]\n",keyModel);
			return(errCodeParameter);
	}
	memcpy(keyByLMK,hsmCmdBuf+offsetOfKeyByLMK,keyLen);

	if (mode == 1)
		memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);

	if ((cvLen = ret - offsetOfKeyByZMK - keyLen) > 0)
	{
		if (cvLen >= 16)
			cvLen = 16;
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,cvLen);
	}
	return(keyLen);
}

// 输入一个DES/SM4密钥 A6
/*
   功能
   将ZMK下加密的密钥转换为LMK下加密，可选保存到指定索引下
   输入参数
   algFlag	模式标识。默认为T。T：ZMK加密下密钥导入；R：保存LMK下加密密钥到指定索引。此域为可选项
   keyType	密钥类型
   zmk	LMK下加密的ZMK。仅当模式为T时有此域
   keyByZmk	密钥。当模式为T时，为ZMK加密的密钥；当模式为R时，为LMK加密的密钥。
   keyModel	密钥类型。0：DES密钥；非0：SM4密钥。
   keyIndex	密钥索引。当模式为R时有此域，当模式为T时此域可选。
   输出参数
   keyByLmk	密钥值。LMK加密的密钥，仅当模式为T时有。
   checkValue	密钥校验值
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdA6(char *algFlag, TUnionDesKeyType keyType,char *zmk,char *keyByZmk,int keyModel,int keyIndex,char *keyByLmk,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512+1];
	int	hsmCmdLen = 0;
	int	keyByLmkStrLen = 0;

	memcpy(hsmCmdBuf,"A6",2);
	hsmCmdLen = 2;

	if (algFlag != NULL && strlen(algFlag) != 0)
		memcpy(hsmCmdBuf+hsmCmdLen, algFlag, 1);
	else
		memcpy(hsmCmdBuf+hsmCmdLen, "T", 1);
	hsmCmdLen += 1;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdA6:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if (strcmp(algFlag, "T") == 0)	
	{
		if (strlen(zmk) == 4)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, zmk, 4);
			hsmCmdLen += 4;
		}
		else
		{
			if (keyModel == 0)
			{
				if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
				{
					UnionUserErrLog("in UnionJTBHsmCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",zmk);
					return(ret);
				}
				hsmCmdLen += ret;
			}
			else 
			{
				sprintf(hsmCmdBuf+hsmCmdLen,"S%s", zmk);
				hsmCmdLen += (1+strlen(zmk));
			}
		}
	}

	if (keyModel == 0)
	{
		if ((ret = UnionPutKeyIntoRacalKeyString(keyByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionJTBHsmCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyByZmk);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	else
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "S%s", keyByZmk);
		hsmCmdLen += (1+strlen(keyByZmk));
	}	

	if (strcmp(algFlag, "T") == 0)
	{
		if (keyModel == 0)
		{
			if ((ret = UnionCaculateHsmKeyKeyScheme(strlen(keyByZmk),hsmCmdBuf+hsmCmdLen)) < 0)
			{
				UnionUserErrLog("in UnionJTBHsmCmdA6:: UnionCaculateHsmKeyKeyScheme [%s]!\n",keyByZmk);
				return(ret);
			}
		}
		else
			memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen++;
	}

	if (keyIndex > 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "K%03d", keyIndex);
		hsmCmdLen += 4;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdA6:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (strcmp(algFlag, "T") == 0)
	{
		if (keyModel == 0)
		{
			if ((keyByLmkStrLen = UnionReadKeyFromRacalKeyString(hsmCmdBuf+4,ret-4,keyByLmk)) < 0)
			{
				UnionUserErrLog("in UnionJTBHsmCmdA6:: UnionReadKeyFromRacalKeyString! hsmCmdBuf = [%s]!\n",hsmCmdBuf);
				return(keyByLmkStrLen);
			}
		}
		else
		{	
			memcpy(keyByLmk, hsmCmdBuf+4+1, 32);
			keyByLmkStrLen = (1+32);
		}
	}

	memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,ret-keyByLmkStrLen-4);

	return(0);
}

// 输出一个密钥DES/SM4 A8
/*
   功能
   LMK加密或索引下密钥使用ZMK加密输出
   输入参数
   mode	模式。0：备份索引下密钥到LMK加密；1：LMK加密或索引密钥导出为ZMK加密。
   keyType	密钥类型
   zmk	LMK下加密的ZMK或密钥索引。仅当模式为1时有。
   key	需要输出的密钥。仅当模式为1时，为ZMK加密的密钥；当模式为0时，为密钥索引，K+3H。
   keyMode	密钥模式。0：DES密钥；1：SM4密钥。
   输出参数
   keyByZMK	密钥值。当模式为0时，为LMK加密；当模式为1时，为ZMK加密。
   checkValue	密钥校验值
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdA8(int mode,TUnionDesKeyType keyType,char *key,int keyMode,char *zmk,char *keyByZMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512+1];
	int	hsmCmdLen = 0;
	int	keyLen;
	int	offsetOfKeyByZMK;
	int	offsetOfCheckValue;

	memcpy(hsmCmdBuf,"A8",2);
	hsmCmdLen = 2;

	if (mode == 0)
		hsmCmdBuf[hsmCmdLen] = 'R';
	else if (mode == 1)
		hsmCmdBuf[hsmCmdLen] = 'T';
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdA8:: invalid mode [%d]\n", mode);
		return(errCodeParameter);
	}
	hsmCmdLen++;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdA8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if (mode == 1)
	{
		if (strlen(zmk) == 4)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, zmk, 4);
			hsmCmdLen += 4;
		}
		else
		{
			if (keyMode == 0)
			{
				if ((ret = UnionGenerateX917RacalKeyString(strlen(zmk),zmk,hsmCmdBuf+hsmCmdLen)) < 0)
				{
					UnionUserErrLog("in UnionJTBHsmCmdA8:: UnionGenerateX917RacalKeyString!\n");
					return(ret);
				}
				hsmCmdLen += ret;

			}
			else
			{
				sprintf(hsmCmdBuf+hsmCmdLen, "S%s", zmk);
				hsmCmdLen += (1+strlen(zmk));
			}
		}
	}

	if (mode == 1)
	{
		if (keyMode == 0)
		{
			if ((ret = UnionGenerateX917RacalKeyString(strlen(key),key,hsmCmdBuf+hsmCmdLen)) < 0)
			{
				UnionUserErrLog("in UnionJTBHsmCmdA8:: UnionGenerateX917RacalKeyString!\n");
				return(ret);
			}
			hsmCmdLen += ret;
		}
		else
		{
			sprintf(hsmCmdBuf+hsmCmdLen, "S%s", key);
			hsmCmdLen += (1+strlen(key));
		}
	}
	else
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "K%s", key);
		hsmCmdLen += 4;
	}

	if (keyMode == 0)
	{
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
				UnionUserErrLog("in UnionJTBHsmCmdA8:: keylen [%d] error!\n",(int)strlen(key));
				return(errCodeParameter);
		}
	}
	else if (keyMode == 1)
		memcpy(hsmCmdBuf+hsmCmdLen,"S",1);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdA8:: invalid parameter keyMode [%d]\n", keyMode);
		return(errCodeParameter);
	}
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdA8:: UnionDirectHsmCmd!\n");
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
			UnionUserErrLog("in UnionJTBHsmCmdA8:: invalid keyLen[%d]\n",(int)strlen(key));
			return(errCodeParameter);
	}
	memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	if (ret - offsetOfCheckValue > 16)
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,16);
	else
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,ret - offsetOfCheckValue);
	return(keyLen);
}

// 装载一个私钥 EK
/*
   功能
   将LMK加密的RSA/SM2私钥保存到指定索引下
   输入参数
   algFlag	算法标识。1：SM2；其它:RSA。
   vkIndex	私钥索引。范围01到20。
   lenOfVK	私钥长度
   vk	私钥
   输出参数
   无
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEK(int algFlag,int vkIndex,int lenOfVK,unsigned char *vk)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		offset = 0;

	memcpy(hsmCmdBuf,"EK",2);
	offset = 2;
	if (algFlag == 1)
		hsmCmdBuf[offset] = 'S';
	else
		hsmCmdBuf[offset] = 'R';
	offset ++;

	sprintf(hsmCmdBuf+offset,"%02d",vkIndex);
	offset += 2;

	sprintf(hsmCmdBuf+offset,"%04d",lenOfVK/2);
	offset += 4;
	if (offset + lenOfVK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in UnionJTBHsmCmdEK:: lenOfVK = [%04d] too long\n",lenOfVK);
		return(errCodeParameter);
	}
	aschex_to_bcdhex((char *)vk,lenOfVK,hsmCmdBuf+offset);
	offset += (lenOfVK/2);
	UnionSetBCDPrintTypeForHSMCmd();
	hsmCmdBuf[offset] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdEK:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

// 产生非对称密钥对 EI
/*
   功能
   产生非对称密钥
   输入参数
   algFlag	算法标识。0：RSA；1：SM2。
   type	密钥类型。0：签名和加密；1：密钥管理；2：签名和加密，密钥管理；3：密钥协商，当且仅当算法标识为1时有。
   length	密钥长度
   pkEncoding	公钥编码。当算法标识为0时有。01：DER。
   lenOfPKExponent	公钥指数的长度。当算法标识为0时有。
   pkExponent	公钥指数。当算法标识为0时有。
   exportNullPK	裸公钥标识
   index	索引。01-20：密码机内保存新生成的密钥；99：不保存新生成的密钥。
   输出参数
   pk	公钥
   lenOfVK	私钥长度
   vk	私钥
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEI(int algFlag,char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,int index,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	int		lenOfPK;
	int		lenOfPKStr = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"EI",2);
	hsmCmdLen = 2;
	if (algFlag == 0)
		hsmCmdBuf[hsmCmdLen] = 'R';
	else if (algFlag == 1)
		hsmCmdBuf[hsmCmdLen] = 'S';
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEI:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen++;

	if ('0' == type || '1' == type || '2' == type)
		memcpy(hsmCmdBuf+hsmCmdLen,"0",1);
	else if ('3' == type)
		memcpy(hsmCmdBuf+hsmCmdLen,"1",1);
	else if ('4' == type)
		memcpy(hsmCmdBuf+hsmCmdLen,"2",1);
	else if ('5' == type)
	{
		if (algFlag == 1)
			memcpy(hsmCmdBuf+hsmCmdLen,"3",1);
	}
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEI:: invalid parameter type [%d]\n", type);
		return(errCodeParameter);
	}
	hsmCmdLen++;

	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",length);
	hsmCmdLen += 4;
	if (algFlag == 0)
	{
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
				UnionUserErrLog("in UnionJTBHsmCmdEI:: lenOfPKExponent [%d] too long !\n",lenOfPKExponent);
				return(errCodeParameter);
			}
			memcpy(hsmCmdBuf+hsmCmdLen,pkExponent,lenOfPKExponent);
			hsmCmdLen += lenOfPKExponent;
		}
	}

	if ((index > 0 && index < 21) || (index == 99))
	{
		hsmCmdBuf[hsmCmdLen] = ';';
		hsmCmdLen ++;

		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", index);
		hsmCmdLen += 2;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdEI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	if (algFlag == 0)
	{
		if ((lenOfPKStr = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)hsmCmdBuf+4,ret-4,pk,&lenOfPK,sizeOfPK)) < 0)
		{
			UnionUserErrLog("in UnionJTBHsmCmdEI:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
			UnionMemLog("in UnionJTBHsmCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
			return(lenOfPKStr);
		}
		if (lenOfPKStr + 4 > ret - 4)
		{
			UnionUserErrLog("in UnionJTBHsmCmdEI:: lenOfPKStr + 4 = [%d] longer than [%d]!\n",lenOfPKStr+4,ret-4);
			UnionMemLog("in UnionJTBHsmCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
			return(errCodeParameter);
		}
		if (!exportNullPK)
		{
			if (lenOfPKStr > sizeOfPK)
			{
				UnionUserErrLog("in UnionJTBHsmCmdEI:: sizeOfPK [%04d] < expected [%04d]!\n",sizeOfPK,lenOfPKStr);
				return(errCodeSmallBuffer);
			}
			memcpy(pk,hsmCmdBuf+4,lenOfPKStr);
			lenOfPK = lenOfPKStr;
		}		
	}
	else
	{
		memcpy(pk,hsmCmdBuf+4,64);
		lenOfPK = 64;
		lenOfPKStr = 64;
	}

	*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+lenOfPKStr+4,4);
	if ((*lenOfVK <= 0) || (*lenOfVK + 4 + 4 + lenOfPKStr > ret) || (*lenOfVK > sizeOfVK))
	{
		UnionUserErrLog("in UnionJTBHsmCmdEI:: lenOfVK [%d] error!\n",*lenOfVK);
		UnionMemLog("in UnionJTBHsmCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	memcpy(vk,hsmCmdBuf+4+lenOfPKStr+4,*lenOfVK);
	return(lenOfPK);
}

// 从私钥获取公钥 EJ
/*
   功能
   从私钥获取公钥
   输入参数
   algFlag 算法标识。0：RSA；1：SM2。
   vkIndex 私钥索引。01-20。99：外带私钥。
   vkDataLen	私钥数据块长度。仅当私钥索引为99时有。
   vkData	私钥数据块。仅当私钥索引为99时有。
   pkEncode	公钥编码。当算法标识为0时有。01：DER。
   输出参数
   pk	公钥。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEJ(int algFlag,int vkIndex,int vkDataLen,char *vkData,char *pkEncode,char *pk,int sizeOfPK)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"EJ",2);
	hsmCmdLen = 2;
	if (algFlag == 0)
		hsmCmdBuf[hsmCmdLen] = 'R';
	else if (algFlag == 1)
		hsmCmdBuf[hsmCmdLen] = 'S';
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEJ:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen++;
	if ((vkIndex > 0 && vkIndex < 21) || (vkIndex == 99))
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
		hsmCmdLen += 2;
	}
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEJ:: invalid parameter vkIndex [%d]\n", vkIndex);
		return(errCodeParameter);
	}	
	if (vkIndex == 99)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkDataLen/2);
		hsmCmdLen += 4;
		aschex_to_bcdhex(vkData,vkDataLen,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (vkDataLen/2);
	}

	if (algFlag == 0)
	{
		if (pkEncode == NULL || strlen(pkEncode) == 0)
			memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
		else
			memcpy(hsmCmdBuf+hsmCmdLen,pkEncode,2);
		hsmCmdLen += 2;
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdEJ:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	bcdhex_to_aschex(hsmCmdBuf+4, ret-4, pk);
	return(ret-4);
}

// 导出非对称私钥 EH
/*
   功能
   导出非对称私钥
   输入参数
   algFlag 算法标识。0：ECB；1：CBC。
   proKeyType	保护密钥类型。0：KEK；1：MDK。
   proKey	保护密钥。用于保护RSA/SM2的密钥或密钥索引。
   proKeyModel	保护密钥方案。0：DES双倍长加密；1：SM4加密。
   speNum	分散次数。范围0-5.
   speData	分散数据
   expAlgFlag	导出算法标识。0：RSA；1：SM2。
   vkIndex	私钥索引。01-20：加密机内私钥；99：外带私钥。
   vkLen	私钥长度
   vkData	私钥
   padFlag	私钥分量填充标识。0：不强制填充；1：强制填充80。
   iv	初始向量。仅算法标识为1时有。
   输出参数
   module	模长
   pkM	公钥模M
   pkE	公钥指数E
   vkD	私钥指数D
   vkP	私钥分量P
   vkQ	私钥分量Q
   vkDP	私钥分量DP
   vkDQ	私钥分量DQ
   vkQINV	私钥分量QINV
   sm2X	SM2公钥X
   sm2Y	SM2公钥Y
   sm2D	SM2私钥分量D
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEH(int algFlag,TUnionDesKeyType proKeyType,char *proKey,int proKeyModel,int speNum,char *speData,int expAlgFlag,int vkIndex,int vkLen,char *vkData,int padFlag,char *iv,int *module,char *pkM,char *pkE,char *vkE,char *vkD,char *vkP,char *vkQ,char *vkDP,char *vkDQ,char *vkQINV,char *sm2X,char *sm2Y,char *sm2D)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	int		offset = 0;
	char		tmpBuf[10];
	int		i = 0;

	memcpy(hsmCmdBuf,"EH",2);
	hsmCmdLen = 2;
	if (algFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "00", 2);
	else if (algFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEH:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (proKeyType == conZMK)
		memcpy(hsmCmdBuf+hsmCmdLen, "000", 3);
	else if (proKeyType == conMKAC)
		memcpy(hsmCmdBuf+hsmCmdLen, "109", 3);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEH:: invalid parameter proKeyType [%d]\n", proKeyType);
		return(errCodeParameter);
	}
	hsmCmdLen += 3;

	if (strlen(proKey) == 3)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "K%s", proKey);
		hsmCmdLen += 4;
	}
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(proKey),proKey,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionJTBHsmCmdEH:: UnionGenerateX917RacalKeyString err!ret = [%d]\n", ret);
			return(ret);
		}
		hsmCmdLen += ret;
	}


	if (proKeyModel == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "X", 1);
	else if (proKeyModel == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEH:: invalid parameter proKeyModel [%d]\n", proKeyModel);
		return(errCodeParameter);
	}	
	hsmCmdLen++;

	sprintf(hsmCmdBuf+hsmCmdLen, "%d", speNum);
	hsmCmdLen++;

	for(i=0;i<speNum;i++)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, speData, strlen(speData));
		hsmCmdLen += strlen(speData);
	}

	if (expAlgFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "R", 1);
	else if (expAlgFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEH:: invalid parameter expAlgFlag [%d]\n", expAlgFlag);
		return(errCodeParameter);
	}
	hsmCmdLen++;

	if ((vkIndex > 0 && vkIndex < 21) || vkIndex == 99)
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEH:: invalid parameter vkIndex [%d]\n", vkIndex);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (vkIndex == 99)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLen/2);
		hsmCmdLen += 4;
		aschex_to_bcdhex(vkData,vkLen,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (vkLen/2);
	}

	if (padFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "00", 2);
	else if (padFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEH:: invalid parameter padFlag [%d]\n", padFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (algFlag == 1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, strlen(iv));
		hsmCmdLen += strlen(iv);
	}
	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdEH:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memset(tmpBuf, 0, sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+offset,4);
	*module = atoi(tmpBuf);
	offset += 4;

	if (expAlgFlag == 0)
	{
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), pkM);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), pkE);
		offset += atoi(tmpBuf);
		
		//edit by chenwd 指令文件没有返回vkE域 20151229
		/*
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), vkE);
		offset += atoi(tmpBuf);
		
		*/
		
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), vkD);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), vkP);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), vkQ);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), vkDP);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), vkDQ);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), vkQINV);
		offset += atoi(tmpBuf);
	}
	else
	{
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), sm2X);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), sm2Y);
		offset += atoi(tmpBuf);

		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), sm2D);
		offset += atoi(tmpBuf);
	}

	return(0);
}

// 私钥解密运算 EP
/*
   功能
   使用RSA/SM2私钥解密数据
   输入参数
   algFlag	算法标识。0：RSA；1：SM2。
   padFlag	填充标识。0：不填充；1：填充，PKCS#1 1.5。当算法标识为0时有。
   dataLen	数据块密文长度。
   cipherData	数据块密文。
   vkIndex	私钥索引。01-20：加密机内私钥；99：外带私钥。
   vkDataLen	私钥长度。
   vkData	私钥。
   输出参数
   plainData	明文数据。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEP(int algFlag,int padFlag,int dataLen,char *cipherData,int vkIndex,int vkDataLen,char *vkData,char *plainData,int sizeOfPlainData)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[10];
	int		offset = 0;

	memcpy(hsmCmdBuf,"EP",2);
	hsmCmdLen = 2;
	if (algFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else if (algFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "07", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEP:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (algFlag == 0)
	{
		if (padFlag == 0)
			memcpy(hsmCmdBuf+hsmCmdLen, "00", 2);
		else if (padFlag == 1)
			memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
		else
		{
			UnionUserErrLog("in UnionJTBHsmCmdEP:: invalid parameter padFlag [%d]\n", padFlag);
			return(errCodeParameter);
		}
		hsmCmdLen += 2;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", (int)strlen(cipherData)/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(cipherData, dataLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (dataLen/2);

	memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
	hsmCmdLen++;

	if ((vkIndex > 0 && vkIndex < 21) || (vkIndex == 99))
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
		hsmCmdLen += 2;
	}
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEP:: invalid parameter vkIndex [%d]\n", vkIndex);
		return(errCodeParameter);
	}	

	if (vkIndex == 99)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkDataLen/2);
		hsmCmdLen += 4;
		aschex_to_bcdhex(vkData, vkDataLen, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (vkDataLen/2);
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdEP:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memset(tmpBuf, 0, sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+offset, 4);
	offset += 4;

	bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), plainData);

	return(atoi(tmpBuf));
}

// 公钥加密运算 ER
/*
   功能
   使用RSA/SM2公钥加密数据
   输入参数
   algFlag	算法标识。0：RSA；1：SM2。
   padFlag	填充标识。0：不填充；1：填充，PKCS#1 1.5。当算法标识为0时有。
   dataLen	数据长度。
   plainData	数据。
   pkIndex	公钥索引。01-20：加密机内公钥；99：外带公钥。
   pkDataLen	公钥长度。
   pkData	公钥。
   输出参数
   cipherData	密文数据。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdER(int algFlag,int padFlag,int dataLen,char *plainData,int pkIndex,int pkDataLen,char *pkData,char *cipherData,int sizeOfcipherData)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[10];
	int		offset = 0;

	memcpy(hsmCmdBuf,"ER",2);
	hsmCmdLen = 2;
	if (algFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else if (algFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "07", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdER:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (algFlag ==0)
	{
		if (padFlag == 0)
			memcpy(hsmCmdBuf+hsmCmdLen, "00", 2);
		else if (padFlag == 1)
			memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
		else
		{
			UnionUserErrLog("in UnionJTBHsmCmdER:: invalid parameter padFlag [%d]\n", padFlag);
			return(errCodeParameter);
		}
		hsmCmdLen += 2;
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", dataLen/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(plainData, dataLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (dataLen/2);

	memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
	hsmCmdLen++;

	if ((pkIndex > 0 && pkIndex < 21) || (pkIndex == 99))
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", pkIndex);
		hsmCmdLen += 2;
	}
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdER:: invalid parameter pkIndex [%d]\n", pkIndex);
		return(errCodeParameter);
	}	

	if (pkIndex == 99)
	{
		aschex_to_bcdhex(pkData, pkDataLen, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (pkDataLen/2);
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdER:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memset(tmpBuf, 0, sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+offset, 4);
	offset += 4;

	bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), cipherData);

	return(atoi(tmpBuf));
}

// 私钥签名运算 EW
/*
   功能
   使用RSA/SM2私钥计算数据的签名
   输入参数
   hashFlag	HASH算法标识。0：SHA-1；1：MD5；2：ISO 10118-2；3：NO HASH；4：SM3。
   signFlag	签名算法标识。0：RSA；1：SM2。
   padFlag		填充标识。0：不填充；1：填充，PKCS#1 1.5。当签名算法标识为0时有。
   userIDLen	用户标识长度。范围0000-0032。当签名算法标识为1时有。
   userID		用户标识。当签名算法标识为1时有。
   dataLen		签名数据长度。
   data		签名数据。
   vkIndex		私钥索引。01-20：加密机内私钥；99：外带私钥。
   vkDataLen	私钥长度。
   vkData		私钥。
   输出参数
   sign		签名。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEW(int hashFlag,int signFlag,int padFlag,int userIDLen,char *userID,int dataLen,char *data,int vkIndex,int vkDataLen,char *vkData,char *sign,int sizeOfSing)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[10];
	int		offset = 0;
	int		signLen = 0;

	memcpy(hsmCmdBuf,"EW",2);
	hsmCmdLen = 2;
	if (hashFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else if (hashFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "02", 2);
	else if (hashFlag == 2)
		memcpy(hsmCmdBuf+hsmCmdLen, "03", 2);
	else if (hashFlag == 3)
		memcpy(hsmCmdBuf+hsmCmdLen, "04", 2);
	else if (hashFlag == 4)
		memcpy(hsmCmdBuf+hsmCmdLen, "20", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEW:: invalid parameter hashFlag [%d]\n", hashFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (signFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else if (signFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "07", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEW:: invalid parameter signFlag [%d]\n", signFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (signFlag == 0)
	{
		if (padFlag == 0)
			memcpy(hsmCmdBuf+hsmCmdLen, "00", 2);
		else if (padFlag == 1)
			memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
		else
		{
			UnionUserErrLog("in UnionJTBHsmCmdEW:: invalid parameter padFlag [%d]\n", padFlag);
			return(errCodeParameter);
		}
		hsmCmdLen += 2;
	}

	if (signFlag == 1)
	{
		if (userIDLen > 0)
		{
			sprintf(hsmCmdBuf+hsmCmdLen, "%04d", userIDLen/2);
			hsmCmdLen += 4;

			//aschex_to_bcdhex(userID, userIDLen, hsmCmdBuf+hsmCmdLen);
			//hsmCmdLen += (userIDLen/2);
			memcpy(hsmCmdBuf+hsmCmdLen, userID, strlen(userID));
			hsmCmdLen += strlen(userID);
		}
		else
		{
			memcpy(hsmCmdBuf+hsmCmdLen, "0016", 4);
			hsmCmdLen += 4;

			memcpy(hsmCmdBuf+hsmCmdLen, "1234567812345678", 16);
			hsmCmdLen += 16;
		}
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", dataLen/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(data, dataLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (dataLen/2);

	memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
	hsmCmdLen++;

	if ((vkIndex > 0 && vkIndex < 21) || (vkIndex == 99))
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%02d", vkIndex);
		hsmCmdLen += 2;
	}
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEW:: invalid parameter vkIndex [%d]\n", vkIndex);
		return(errCodeParameter);
	}	

	if (vkIndex == 99)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkDataLen/2);
		hsmCmdLen += 4;
		aschex_to_bcdhex(vkData,vkDataLen,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (vkDataLen/2);
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdEW:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if (hashFlag == 4)
	{
		memcpy(sign, hsmCmdBuf+offset, ret-4);
		signLen = (ret - 4);
	}
	else
	{
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+offset, 4);
		offset += 4;

		memcpy(sign, hsmCmdBuf+offset,atoi(tmpBuf));
		signLen = atoi(tmpBuf);
	}

	return(signLen);
}

// 公钥验签运算 EY
/*
   功能
   使用RSA/SM2公钥验证签名
   输入参数
   hashFlag		HASH算法标识。0：SHA-1；1：MD5；2：ISO 10118-2；3：NO HASH；4：SM3。
   signFlag		签名算法标识。0：RSA；1：SM2。
   padFlag			填充标识。0：不填充；1：填充，PKCS#1 1.5。当签名算法标识为0时有。
   userIDLen		用户标识长度。范围0000-0032。当签名算法标识为1时有。
   userID			用户标识。当签名算法标识为1时有。
   signLen			签名长度。
   sign			签名。
   dataLen			签名数据长度。
   data			签名数据。
   macLen			MAC长度。
   mac			MAC。
   pkLen			公钥长度。
   pk			公钥。
   macDataLen		签别数据长度。
   macData			签别数据。
   输出参数
   无
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEY(int hashFlag,int signFlag,int padFlag,int userIDLen,char *userID,int signLen, char *sign,int dataLen,char *data,int macLen, char *mac,int pkLen,char *pk,int macDataLen,char *macData)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;

	memcpy(hsmCmdBuf,"EY",2);
	hsmCmdLen = 2;
	if (hashFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else if (hashFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "02", 2);
	else if (hashFlag == 2)
		memcpy(hsmCmdBuf+hsmCmdLen, "03", 2);
	else if (hashFlag == 3)
		memcpy(hsmCmdBuf+hsmCmdLen, "04", 2);
	else if (hashFlag == 4)
		memcpy(hsmCmdBuf+hsmCmdLen, "20", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEY:: invalid parameter hashFlag [%d]\n", hashFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (signFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else if (signFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "07", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdEY:: invalid parameter signFlag [%d]\n", signFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	if (signFlag == 0)
	{
		if (padFlag == 0)
			memcpy(hsmCmdBuf+hsmCmdLen, "00", 2);
		else if (padFlag == 1)
			memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
		else
		{
			UnionUserErrLog("in UnionJTBHsmCmdEY:: invalid parameter padFlag [%d]\n", padFlag);
			return(errCodeParameter);
		}
		hsmCmdLen += 2;
	}
	
	if (signFlag == 1)
	{
		if (userIDLen > 0)
		{
			sprintf(hsmCmdBuf+hsmCmdLen, "%04d", userIDLen);
			hsmCmdLen += 4;

			memcpy(hsmCmdBuf+hsmCmdLen, userID, strlen(userID));
			hsmCmdLen += strlen(userID);
		}
		else
		{
			memcpy(hsmCmdBuf+hsmCmdLen, "0016", 4);
			hsmCmdLen += 4;

			memcpy(hsmCmdBuf+hsmCmdLen, "1234567812345678", 16);
			hsmCmdLen += 16;
		}
	}

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", signLen/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(sign, signLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (signLen/2);

	memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
	hsmCmdLen++;

	sprintf(hsmCmdBuf+hsmCmdLen, "%04d", dataLen/2);
	hsmCmdLen += 4;

	aschex_to_bcdhex(data, dataLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (dataLen/2);

	memcpy(hsmCmdBuf+hsmCmdLen, ";", 1);
	hsmCmdLen++;

	if (macLen > 0)
	{
		aschex_to_bcdhex(mac, macLen, hsmCmdBuf+hsmCmdLen);
		//memcpy(hsmCmdBuf+hsmCmdLen, mac, macLen);
		hsmCmdLen += (macLen/2);
	}
	else
	{
		aschex_to_bcdhex("00000000", 8, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += 8;
	}

	aschex_to_bcdhex(pk, pkLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (pkLen/2);

	if (macData != NULL && strlen(macData) != 0)
	{
		aschex_to_bcdhex(macData, macDataLen, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += macDataLen;
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdEY:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	return(0);
}

// 计算数据摘要 GM
/*
   功能
   计算数据摘要
   输入参数
   hashFlag		HASH算法标识。1：SHA-1；2：MD5；3：ISO 10118-2；4：SM3。
   dataLen			数据长度。
   data			数据。
   userIDLen		用户标识长度。范围0000-0032。当签名HSAH算法标识为3时有。
   userID			用户标识。当签名HSAH算法标识为3时有。
   pkLen			公钥长度。
   pk			公钥。
   输出参数
   hash			HASH值。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdGM(int hashFlag,int dataLen,char *data,int userIDLen,char *userID,int pkLen,char *pk,int sizeofHash,char *hash)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"GM",2);
	hsmCmdLen = 2;
	if (hashFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "01", 2);
	else if (hashFlag == 2)
		memcpy(hsmCmdBuf+hsmCmdLen, "02", 2);
	else if (hashFlag == 3)
		memcpy(hsmCmdBuf+hsmCmdLen, "03", 2);
	else if (hashFlag == 4)
		memcpy(hsmCmdBuf+hsmCmdLen, "20", 2);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdGM:: invalid parameter hashFlag [%d]\n", hashFlag);
		return(errCodeParameter);
	}
	hsmCmdLen += 2;

	sprintf(hsmCmdBuf+hsmCmdLen, "%05d", dataLen/2);
	hsmCmdLen += 5;

	aschex_to_bcdhex(data, dataLen, hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += (dataLen/2);

	if (hashFlag == 4)
	{
		sprintf(hsmCmdBuf+hsmCmdLen, "%04d", userIDLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(userID, userIDLen, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (userIDLen/2);

		sprintf(hsmCmdBuf+hsmCmdLen, "%04d", pkLen/2);
		hsmCmdLen += 4;

		aschex_to_bcdhex(pk, pkLen, hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += (pkLen/2);
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdGM:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(hash, hsmCmdBuf+4, ret-4);

	return(ret-4);
}

// 计算及校验MAC/TAC UB
/*
   功能
   对指定密钥进行指定次数的离散得到子密钥或过程密钥作为MAC/TC计算密钥对输入数据计算MAC/TC。
   输入参数
   algFlag		算法标识。0：DES；1：SM4。
   mode		模式标识。0：计算MAC；1：校验MAC。
   id		方案ID。0：使用子密钥进行3DES/SM4 计算MAC；1：使用过程密钥进行DES MAC；2：TAC；3：使用过程密钥进行3DES/SM4 计算MAC。
   mkType		主密钥类型。0：MK-AC；1：MK-SMI；2：MK-SMC；3：MK-DN；4：TAK；5：ZAK。
   mk		主密钥。
   mkIndex		主密钥索引。
   mkDvsNum	主密钥离散次数。范围0-5.
   mkDvsData	主密钥离散数据。
   proFactor	过程数据。仅当方案ID为1或3时有。
   macFillFlag	MAC数据填充标识。0：强制填充0X80；1：不强制填充0X80。
   ivMac		IV-MAC。
   lenOfData	MAC计算数据长度。
   data		MAC计算数据。
   macFlag		MAC长度。
   checkMac	待校验的MAC。仅当模式标识为1时有
   输出参数
   mac		MAC。仅当模式标识为0时有。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdUB(int algFlag, char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, int macLen, char *checkMac, char *mac)
{
	int     ret;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    szMkBuff[64+1];
	char    proBuf[16+1];
	int	i = 0;

	// 命令代码
	memcpy(hsmCmdBuf,"UB",2);
	hsmCmdLen += 2;

	// 算法标识
	if (algFlag == 0)
		memcpy(hsmCmdBuf + hsmCmdLen, "X", 1);
	else if (algFlag == 1)
		memcpy(hsmCmdBuf + hsmCmdLen, "S", 1);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdUB:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen++;

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
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
		szMkBuff[ret] = 0;
		memcpy(hsmCmdBuf + hsmCmdLen, szMkBuff, ret);
		hsmCmdLen += ret;
	}

	// 离散次数
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen++;

	// 离散数据
	for(i=0;i<mkDvsNum;i++)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, 16);
		hsmCmdLen += 16;
	}

	// 过程数据
	if ('1' == id[0])
		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, strlen(proFactor));
	else if ('3' == id[0])
	{
		memset(proBuf,'0',sizeof(proBuf));
		memcpy(proBuf+16-strlen(proFactor),proFactor,strlen(proFactor));
		memcpy(hsmCmdBuf + hsmCmdLen, proBuf, 16);
	}
	hsmCmdLen += 16;

	// MAC数据填充标识
	memcpy(hsmCmdBuf + hsmCmdLen, macFillFlag, 1);
	hsmCmdLen++;
	// IV-MAC
	if ( ivMac != NULL && strlen(ivMac) != 0)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, ivMac, strlen(ivMac));
		hsmCmdLen += 16;
	}
	else
	{
		if (algFlag == 0)
		{
			memcpy(hsmCmdBuf + hsmCmdLen, "0000000000000000", 16);
			hsmCmdLen += 16;
		}
		else
		{
			memcpy(hsmCmdBuf + hsmCmdLen, "00000000000000000000000000000000", 32);
			hsmCmdLen += 32;
		}
	}

	// macDataLen
	sprintf(hsmCmdBuf + hsmCmdLen, "%03d", lenOfData/2);
	hsmCmdLen += 3;

	// macData
	memcpy(hsmCmdBuf + hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;

	// MAC长度
	sprintf(hsmCmdBuf + hsmCmdLen, "%02d", macLen);
	hsmCmdLen += 2;

	if (mode[0] == '2')
	{
		memcpy(hsmCmdBuf + hsmCmdLen, checkMac, strlen(checkMac));
		hsmCmdLen += strlen(checkMac);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdUB:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	offset = 4;
	if (mode[0] == '1')
	{
		strcpy(mac, hsmCmdBuf + offset); 
		return(2*macLen);
	}
	return(0);
}

// ARQC/TC/ACC校验，ARPC产生 KW
/*
   功能
   ARQC/TC/ACC的校验，ARPC的产生或同时验证ARQC并产生ARPC
   输入参数
   mode	模式标识。0：仅验证ARQC；1：验证ARQC并EMV4.1方式一产生ARPC；2：仅EMV4.1方式一产生ARPC；3：验证ARQC并EMV4.1方式二产生ARPC；4：仅EMV4.1方式二产生ARPC；5：仅验证ARQC（VISA）；6：验证ARQC并EMV4.1方式一产生ARPC（VISA）。
   id	密钥离散方案。
   mkIndex	主密钥索引。
   mk	主密钥。
   iv	初始向量。
   dvsNum	离散次数。
   dvsData	离散数据。
   lenOfPan	账号长度。
   pan	账号
   bh	B/H参数
   atc	ATC
   lenOfData	交易数据长度
   data		交易数据
   ARQC		待验证的ARQC
   ARC	ARC
   csu	CSU
   lenOfData1	鉴定数据长度
   data1	鉴定数据
   输出数据
   ARPC	ARPC
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdKW(char *mode, char *id, int mkIndex, char *mk, char *iv,int dvsNum,char *dvsData,
		int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,
		char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,
		char *data1,char *ARPC)
{
	int		ret;
	char		hsmCmdBuf[1024*8+1];
	int		hsmCmdLen = 0;
	char		keyString[49+1];
	int		offset = 0;
	char		ARPCBuf[32+1];

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

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", dvsNum);
	hsmCmdLen++;

	// 离散数据
	aschex_to_bcdhex(dvsData, 16, dvsData);
	memcpy(hsmCmdBuf+hsmCmdLen, dvsData, dvsNum*8);
	hsmCmdLen += dvsNum*8;

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
		UnionUserErrLog("in UnionJTBHsmCmdKW:: UnionDirectHsmCmd!\n");
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

// 产生安全通道会话密钥 G1
/*
   功能
   产生安全通道会话密钥
   输入参数
   algFlag		分散标识。0：先分散子密钥后分散过程密钥；1：只分散过程密钥。
   keyType		主密钥类型。
   keyIndex	主密钥索引。
   key		主密钥值。
   keyData		分散密钥数据。仅当分散标识为0时有。
   SN		SN。
   输出参数
   sencKey		安全通道认证密钥
   cmacKey		CMAC计算密钥
   rmacKey		RMAC计算密钥
   sdekKey		数据加密密钥
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdG1(int algFlag,TUnionDesKeyType keyType,int keyIndex,char *key,char *keyData,char *SN,char *sencKey,char *cmacKey,char *rmacKey,char *sdekKey)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		offset = 0;

	memcpy(hsmCmdBuf,"G1",2);
	offset = 2;
	sprintf(hsmCmdBuf+offset, "%01d", algFlag);
	offset ++;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+offset)) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdG1:: UnionTranslateHsmKeyTypeString err!ret = [%d]\n", ret);
		return(ret);
	}
	offset += ret;

	if (key == NULL || strlen(key) == 0)
	{
		sprintf(hsmCmdBuf+offset, "K%03X", keyIndex);
		offset += 4;
	}
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(strlen(key), key, hsmCmdBuf+offset)) < 0)
		{	
			UnionUserErrLog("in UnionJTBHsmCmdG1:: UnionGenerateX917RacalKeyString !\n");
			return(ret);
		}
		offset += ret;
	}

	if (algFlag == 0)
	{
		memcpy(hsmCmdBuf+offset, keyData, strlen(keyData));
		offset += strlen(keyData);
	}

	memcpy(hsmCmdBuf+offset, SN, strlen(SN));
	offset += strlen(SN);

	hsmCmdBuf[offset] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdG1:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memcpy(sencKey, hsmCmdBuf+offset, 32);
	offset += 32;

	memcpy(cmacKey, hsmCmdBuf+offset, 32);
	offset += 32;

	memcpy(rmacKey, hsmCmdBuf+offset, 32);
	offset += 32;

	memcpy(sdekKey, hsmCmdBuf+offset, 32);
	offset += 32;

	return(0);
}

// 对称转加密数据 UY
/*
   功能
   对称转加密数据，钱包/联机业务。
   输入参数
   tranFlag	算法转换标识。0：DES转SM4；1：SM4转DES；2：SM4转SM4；3：DES转DES。
   srcKeyType	源密钥类型
   srcKeyIndex	源密钥索引
   srcKey		源密钥
   srcAlgFlag	源算法标识。0：ECB；1：ECB_LP；2：ECP_P；3：CBC；4：CBC_LP：5：CBC_P。
   destKeyType	目的密钥类型
   destKeyIndex	目的密钥索引
   destKey		目的密钥
   destAlgFlag	目的算法标识。0：ECB；1：ECB_LP；2：ECP_P；3：CBC；4：CBC_LP：5：CBC_P。
   srcIv		源初始向量。当源算法标识为CBC时有。
   destIv		目的初始向量。当目的算法标识为CBC时有。
   dataLen		数据长度
   data		数据
   输出参数
   destData	目的数据
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdUY(int tranFlag,TUnionDesKeyType srcKeyType,int srcKeyIndex,char *srcKey,int srcAlgFlag,TUnionDesKeyType destKeyType,int destKeyIndex,char *destKey,int destAlgFlag,char *srcIv,char *destIv,int dataLen,char *data,int sizeofDestData,char *destData)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		offset = 0;
	char		tmpBuf[10];

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"UY",2);
	offset = 2;

	sprintf(hsmCmdBuf+offset, "%02d", tranFlag);
	offset += 2;

	if ((ret = UnionTranslateHsmKeyTypeString(srcKeyType,hsmCmdBuf+offset)) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdUY:: UnionTranslateHsmKeyTypeString err!ret = [%d]\n", ret);
		return(ret);
	}
	offset += ret;

	if (srcKey == NULL || strlen(srcKey) == 0)
	{
		sprintf(hsmCmdBuf+offset, "K%03X", srcKeyIndex);
		offset += 4;
	}
	else
	{
		if (tranFlag == 2 || tranFlag == 3)
		{
			sprintf(hsmCmdBuf+offset, "S%s", srcKey);
			offset += (1+strlen(srcKey));
		}
		else
		{
			if ((ret = UnionGenerateX917RacalKeyString(strlen(srcKey), srcKey, hsmCmdBuf+offset)) < 0)
			{
				UnionUserErrLog("in UnionJTBHsmCmdUY:: UnionGenerateX917RacalKeyString err!ret = [%d]\n", ret);
				return(ret);
			}
			offset += ret;
		}
	}

	switch(srcAlgFlag)
	{
		case 0:
			memcpy(hsmCmdBuf+offset, "00", 2);
			break;
		case 1:
			memcpy(hsmCmdBuf+offset, "01", 2);
			break;
		case 2:
			memcpy(hsmCmdBuf+offset, "02", 2);
			break;
		case 3:
			memcpy(hsmCmdBuf+offset, "10", 2);
			break;
		case 4:
			memcpy(hsmCmdBuf+offset, "11", 2);
			break;
		case 5:
			memcpy(hsmCmdBuf+offset, "12", 2);
			break;
		default:
			UnionUserErrLog("in UnionJTBHsmCmdUY:: invalid parameter srcAlgFlag [%d]\n", srcAlgFlag);
			return(errCodeParameter);
	}
	offset += 2;

	if ((ret = UnionTranslateHsmKeyTypeString(destKeyType,hsmCmdBuf+offset)) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdUY:: UnionTranslateHsmKeyTypeString err!ret = [%d]\n", ret);
		return(ret);
	}
	offset += ret;

	if (destKey == NULL || strlen(destKey) == 0)
	{
		sprintf(hsmCmdBuf+offset, "K%03X", destKeyIndex);
		offset += 4;
	}
	else
	{
		if (tranFlag == 1 || tranFlag == 3)
		{
			sprintf(hsmCmdBuf+offset, "S%s", destKey);
			offset += (1+strlen(destKey));
		}
		else
		{
			if ((ret = UnionGenerateX917RacalKeyString(strlen(destKey), destKey, hsmCmdBuf+offset)) < 0)
			{
				UnionUserErrLog("in UnionJTBHsmCmdUY:: UnionGenerateX917RacalKeyString err!ret = [%d]\n", ret);
				return(ret);
			}
			offset += ret;
		}
	}

	switch(destAlgFlag)
	{
		case 0:
			memcpy(hsmCmdBuf+offset, "00", 2);
			break;
		case 1:
			memcpy(hsmCmdBuf+offset, "01", 2);
			break;
		case 2:
			memcpy(hsmCmdBuf+offset, "02", 2);
			break;
		case 3:
			memcpy(hsmCmdBuf+offset, "10", 2);
			break;
		case 4:
			memcpy(hsmCmdBuf+offset, "11", 2);
			break;
		case 5:
			memcpy(hsmCmdBuf+offset, "12", 2);
			break;
		default:
			UnionUserErrLog("in UnionJTBHsmCmdUY:: invalid parameter destAlgFlag [%d]\n", destAlgFlag);
			return(errCodeParameter);
	}
	offset += 2;

	if (srcAlgFlag == 3 || srcAlgFlag == 4 || srcAlgFlag == 5)
	{
		memcpy(hsmCmdBuf+offset, srcIv, strlen(srcIv));
		offset += strlen(srcIv);
	}

	if (destAlgFlag == 3 || destAlgFlag == 4 || destAlgFlag == 5)
	{
		memcpy(hsmCmdBuf+offset, destIv, strlen(destIv));
		offset += strlen(destIv);
	}

	sprintf(hsmCmdBuf+offset, "%04d", dataLen/2);
	offset += 4;

	aschex_to_bcdhex(data, dataLen, hsmCmdBuf+offset);
	offset += (dataLen/2);

	hsmCmdBuf[offset] = 0;
	UnionSetBCDPrintTypeForHSMCmd();

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdUY:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	memset(tmpBuf, 0, sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+offset, 4);
	offset += 4;

	bcdhex_to_aschex(hsmCmdBuf+offset, atoi(tmpBuf), destData);
	offset += atoi(tmpBuf);

	return(atoi(tmpBuf));
}

// 分散密钥数据加解密计算 U1
/*
   功能
   对指定密钥进行指定次数的离散得到子密钥或过程密钥，使用该密钥对输入数据进行加密或解密。
   输入参数
   algFlag		算法标识。0：DES；1：SM4。
   mode		加密模式标识
   id		方案ID
   mkType		主密钥类型
   mk		主密钥
   mkIndex		主密钥索引
   mkDvsNum	离散次数
   mkDvsData	离散数据
   proFactor	过程数据
   iv		初始向量
   lenOfData	数据长度
   data		数据
   输出参数
   criperData	密文或明文
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdU1(int algFlag, int mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, char *proFactor, char *iv, int lenOfData,
		char *data, int *criperDataLen, char *criperData)
{
	int     ret;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	char    tmpBuf[3+1];
	int     offset = 0;
	char    szMkBuff[64+1];
	char    lenBuf[4+1];
	int     len,i;

	// 命令代码
	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"U1",2);
	hsmCmdLen += 2;

	// 算法标识
	if (algFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "X", 1);
	else if (algFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdU1:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen++;

	// 加密模式标识
	sprintf(hsmCmdBuf+hsmCmdLen, "%01d", mode);
	hsmCmdLen++;

	// 方案ID
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", atoi(id));
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
		if (algFlag == 1)
		{
			sprintf(hsmCmdBuf + hsmCmdLen,"S%s", mk);
			hsmCmdLen += (1+strlen(mk));
		}
		else
		{
			ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
			szMkBuff[ret] = 0;
			memcpy(hsmCmdBuf + hsmCmdLen, szMkBuff, ret);
			hsmCmdLen += ret;
		}
	}

	// 离散次数
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	for (i=0;i<mkDvsNum;i++)
	{
		// 离散数据
		memcpy(hsmCmdBuf + hsmCmdLen, mkDvsData, 16);
		hsmCmdLen += 16;
	}

	// 过程数据
	if (6 == mode || 7 == mode || 8 == mode || 9 == mode)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 16);
		hsmCmdLen += 16;
	}
	// 数据填充标识
	if (2 == mode || 3 == mode)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, "02", 2);
		hsmCmdLen += 2;
	}
	else
	{
		memcpy(hsmCmdBuf + hsmCmdLen, "01", 2);
		hsmCmdLen += 2;
	}

	// iv
	if (strcmp(id, "02") == 0)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, iv, strlen(iv));
		hsmCmdLen += strlen(iv);
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
		UnionUserErrLog("in UnionJTBHsmCmdU1:: UnionDirectHsmCmd!\n");
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

// 安全报文方式导出密钥 U3
/*
   功能
   离散卡片密钥并以安全报文方式导出
   输入参数
   algFlag		算法标识。0：DES；1：SM4。
   mode		模式标识。0：仅加密；1：加密并计算MAC。
   id		方案ID
   mkType		主密钥类型
   mk		主密钥
   mkIndex		主密钥索引
   mkDvsNum	离散次数
   mkDvsData	离散数据
   proKeyType	保护密钥类型
   proKey		保护密钥
   proKeyIndex	保护密钥索引
   proDvsNum	保护密钥离散次数
   proDvsData	过程密钥离散数据
   proFlag		过程密钥标识
   proFactor	过程数据
   iv		初始向量
   encPadDataLen	加密数据长度
   encPadData	加密数据
   ivMac		IV-MAC
   macPadDataLen	MAC填充数据长度
   macPadData	MAC填充数据
   dataOffset	偏移量
   输出参数
   criperDataLen	密文数据长度
   criperData	密文数据
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdU3(int algFlag, char *mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, int proKeyType, char *proKey, int proKeyIndex,
		int proDvsNum, char *proDvsData,int proFlag, char *proFactor, char *iv, int encPadDataLen,
		char *encPadData, char *ivMac, int macPadDataLen, char *macPadData, char *dataOffset, char *mac, int *criperDataLen, char *criperData)
{
	int     ret;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	char    tmpBuf[3+1];
	int     offset = 0;
	int	i = 0;

	// 命令代码
	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"U3",2);
	hsmCmdLen += 2;

	// 算法标识
	if (algFlag == 0)
		memcpy(hsmCmdBuf+hsmCmdLen, "X", 1);
	else if (algFlag == 1)
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
	else
	{
		UnionUserErrLog("in UnionJTBHsmCmdU3:: invalid parameter algFlag [%d]\n", algFlag);
		return(errCodeParameter);
	}
	hsmCmdLen++;

	// 加密模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen++;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen++;

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
		if (algFlag == 1)
		{
			sprintf(hsmCmdBuf + hsmCmdLen,"S%s", mk);
			hsmCmdLen += (1+strlen(mk));
		}
		else
		{
			ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, hsmCmdBuf + hsmCmdLen);
			hsmCmdLen += ret;
		}
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

	// 保护密钥类型
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", proKeyType);
	hsmCmdLen ++;

	// 保护密钥
	if (proKey == NULL || strlen(proKey) == 0)
	{
		sprintf(hsmCmdBuf + hsmCmdLen,"K%03X", mkIndex);
		hsmCmdLen += 4;
	}
	else if (strlen(mk) == 4)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, proKey, 4);
		hsmCmdLen += 4;
	}
	else
	{
		ret = UnionGenerateX917RacalKeyString(strlen(proKey), proKey, hsmCmdBuf + hsmCmdLen);
		hsmCmdLen += ret;
	}

	if (proKeyType == 1)
	{
		// 保护密钥离散次数
		sprintf(hsmCmdBuf + hsmCmdLen, "%d", proDvsNum);
		hsmCmdLen++;

		// 保护密钥离散数据
		for(i=0;i<proDvsNum;i++)
		{
			memcpy(hsmCmdBuf + hsmCmdLen, proDvsData, 16);
			hsmCmdLen += 16;
		}
	}

	// 过程密钥标识
	if (proFlag == 0)
		memcpy(hsmCmdBuf + hsmCmdLen, "N", 1);
	else
		memcpy(hsmCmdBuf + hsmCmdLen, "Y", 1);
	hsmCmdLen++;

	// 过程因子
	if (proFlag != 0)
	{
		memcpy(hsmCmdBuf + hsmCmdLen, proFactor, 16);
		hsmCmdLen += 16;
	}

	// IV
	if (('0' == id[0]) || ('3' == id[0]))
	{
		memcpy(hsmCmdBuf + hsmCmdLen, iv, strlen(iv));
		hsmCmdLen += strlen(iv);
	}

	if ('2' == id[0])
	{
		// 加密填充数据长度
		sprintf(hsmCmdBuf + hsmCmdLen, "%04X", encPadDataLen);
		hsmCmdLen +=  encPadDataLen;

		// 加密填充数据
		memcpy(hsmCmdBuf + hsmCmdLen, encPadData, encPadDataLen);
		hsmCmdLen += encPadDataLen;
	}

	if ('1' == mode[0])
	{
		// iv-mac
		memcpy(hsmCmdBuf + hsmCmdLen, ivMac, strlen(ivMac));
		hsmCmdLen += strlen(ivMac);

		// MAC填充数据长度
		sprintf(hsmCmdBuf + hsmCmdLen, "%04X", macPadDataLen/2);
		hsmCmdLen += 4;

		// MAC填充数据
		memcpy(hsmCmdBuf + hsmCmdLen, macPadData, macPadDataLen);
		hsmCmdLen += macPadDataLen;

		// 偏移量
		memcpy(hsmCmdBuf + hsmCmdLen, dataOffset, strlen(dataOffset));
		hsmCmdLen += strlen(dataOffset);
	}

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdU3:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	if ('1' == mode[0])
	{
		memcpy(mac, hsmCmdBuf + offset, 16);
		offset += 16;
	}

	memset(tmpBuf, 0, sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf + offset, 4);
	offset += 4;

	*criperDataLen = atoi(tmpBuf);

	memcpy(criperData, hsmCmdBuf + offset, 2*atoi(tmpBuf));
	return(*criperDataLen);
}

// 生成一个对称密钥并以分量形式打印 NE
/*
   功能
   生成一个对称密钥并以分量形式打印
   输入参数
   keyType		密钥类型
   keyLength	密钥强度
   sm4Flag		SM4密钥标识。非0：SM4密钥；0：DES密钥。
   fldNum		分量个数
   fld		分量值
   输出参数
   component	密钥值
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdNE(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int sm4Flag,int fldNum,char fld[][80],char *component)
{
	int	ret;
	char	hsmCmdBuf[1024+1];
	int	hsmCmdLen = 0;
	int	i;

	memcpy(hsmCmdBuf,"NE",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionJTBHsmCmdNE:: UnionTranslateHsmKeyTypeString!\n");
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

	if (sm4Flag)
	{
		memcmp(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen++;
	}
	else
	{
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in UnionJTBHsmCmdNE:: UnionTranslateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
	}

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
		UnionUserErrLog("in UnionJTBHsmCmdNE:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (sm4Flag)
	{
		memcpy(component,hsmCmdBuf+4+1,32);
		return(32);
	}
	else
	{
		switch (keyLength)
		{
			case    con64BitsDesKey:
				memcpy(component,hsmCmdBuf+4,16);
				return(16);
			case    con128BitsDesKey:
				memcpy(component,hsmCmdBuf+4+1,32);
				return(32);
			case    con192BitsDesKey:
				memcpy(component,hsmCmdBuf+4+1,48);
				return(48);
			default:
				UnionUserErrLog("in UnionJTBHsmCmdNE:: unknown key length!\n");
				return(errCodeParameter);
		}
	}
}
// add by lisq 20141021 end

