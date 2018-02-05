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
#include "unionSJL05Cmd.h"
#include "commWithHsmSvr.h"

#include "unionRSA.h"

// 使用特殊的ZMK类型
extern int gunionIsUseSpecZmkType;

int UnionTranslate30HsmKeyTypeString(TUnionDesKeyType type, char *keyTypeString)
{
	switch (type)
	{
		case    conCVK:
			*keyTypeString = '6';
			return 1;
		case    conTMK:
			*keyTypeString = '5';
			return 1;
		case    conPVK:
			*keyTypeString = '7';
			return 1;
		case    conWWK:
			*keyTypeString = '8';
			return 1;
		case    conZAK:
			*keyTypeString = '3';
			return 1;
		case    conZMK:
			*keyTypeString = '2';
			return 1;
		case    conZPK:
			*keyTypeString = '4';
			return 1;
		case    conZEK:
			*keyTypeString = 'A'; // add by chenliang, 2011-04-26
			return 1;
		case    conTPK:
		case    conTAK:
		case    conKMUSeed:
		case    conKMCSeed:
		case    conMDKSeed:
		case	conEDK:
		case	conBDK:
		case	conMKAC:
		case	conMKSMC:
		default:
			UnionUserErrLog("in UnionTranslate30HsmKeyTypeString:: invalid key type [%d]!\n",type);
			return(errCodeSJL06MDL_InvalidKeyExchange);
	}
}

int UnionTranslate30HsmKeyKeyScheme(TUnionDesKeyLength length,int *keyScheme)
{
	switch (length)
	{
		case	con64BitsDesKey:
			*keyScheme = 8;
			return(*keyScheme);
		case	con128BitsDesKey:
			*keyScheme = 16;
			return(*keyScheme);
		case	con192BitsDesKey:
			*keyScheme = 24;
			return(*keyScheme);
		default:
			UnionUserErrLog("in UnionTranslate30HsmKeyKeyScheme:: invalid key length [%d]!\n",length);
			return(errCodeParameter);
	}
}
int UnionSJL05CmdX0(char mode, char *mediaType, char keyType,int compNum, int keyLen, int derivateNum, char derivateData[][32 + 1], int printNum,	char printFld[][32], char *value, char *checkvalue)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	
	int		mediaTypeLen = 0;
	int		i = 0;
	
	int		derivateDataLen = 0;
	int		printFldLen = 0;
	
	int		keyOffset = 0;
	int		lenOfValue = 0;
	
	hsmCmdLen = 0;
	
	// 命令码
	memcpy(hsmCmdBuf, "X0", 2);
	hsmCmdLen += 2;
	
	// Mode
	hsmCmdBuf[hsmCmdLen] = mode;
	hsmCmdLen += 1;
	
	// MediaType
	mediaTypeLen = strlen(mediaType);
	memcpy(hsmCmdBuf + hsmCmdLen, mediaType, mediaTypeLen);
	hsmCmdLen += 2;

	// keyType

	if ((ret = UnionTranslate30HsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdX0:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// CompNum
	hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%d", compNum);
	if(mode=='0')
	{	
		// keyLen
		if ((ret = UnionTranslate30HsmKeyKeyScheme(keyLen, &keyLen)) < 0)
		{
			UnionUserErrLog("in UnionSJL05CmdX0:: UnionTranslate30HsmKeyKeyScheme error, ret[%d].\n", ret);
			return ret;
		}
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%04d", keyLen);
		hsmCmdLen += ret;	
	}
	if(mode=='1')
	{
		// DerivateNum
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%d", derivateNum);
		
		// derivateData
		for (i = 0; i < derivateNum; ++i)
		{
			derivateDataLen = strlen(derivateData[i]);
			memcpy(hsmCmdBuf + hsmCmdLen, derivateData[i], derivateDataLen);
			
			hsmCmdLen += derivateDataLen;
		}
	}
	
	if(memcmp(mediaType,"10",2)==0||memcmp(mediaType,"11",2)==0)	
	{
		// PrintNum
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%d", printNum);
		
		// Print Fields
		for (i = 0; i < printNum; ++i)
		{
			printFldLen = strlen(printFld[i]);
			
			memcpy(hsmCmdBuf + hsmCmdLen, printFld[i], printFldLen);
			
			hsmCmdLen += printFldLen;
			
			if (i + 1 < printNum)
			{
				hsmCmdBuf[hsmCmdLen] = ';';
				hsmCmdLen += 1;
			}
		}
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdX0:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	
	// 用LMK加密的Key
	if ('X' == *(hsmCmdBuf + 4))
	{
		lenOfValue = 32;
		keyOffset = 5;
		
		memcpy(value, hsmCmdBuf + keyOffset, lenOfValue);
		value[lenOfValue] = 0;
		memcpy(checkvalue, hsmCmdBuf + keyOffset + lenOfValue, 16);
		checkvalue[16] = 0;
	}
	else if ('Y' == *(hsmCmdBuf + 4))
	{
		lenOfValue = 48;
		keyOffset = 5;
		
		memcpy(value, hsmCmdBuf + keyOffset, lenOfValue);
		value[lenOfValue] = 0;
		memcpy(checkvalue, hsmCmdBuf + keyOffset + lenOfValue, 16);
		checkvalue[16] = 0;
	}
	else
	{
		lenOfValue = 16;
		keyOffset = 5;
		
		memcpy(value, hsmCmdBuf + keyOffset, lenOfValue);
		value[lenOfValue] = 0;
		memcpy(checkvalue, hsmCmdBuf + keyOffset + lenOfValue, 16);
		checkvalue[16] = 0;
	}
	
	return lenOfValue;
}

/*
输入域	长度	类型	备注
命令码	2	A	“A2”
Key type	3	H	密钥类型
Key scheme （LMK）	1	A	用LMK加密方式标志
Print Field 0	n	A	打印字段0，不包含“;”
Delimiter	1	A	值为‘；’，打印字段结束符
Print Field 1	n	A	打印字段1，不包含“;”
…	…	…	…
Last Print Field 	n	A	最后一个打印字段，不包含“;”
消息尾	Nt	A	
输出域	长度	类型	备注
消息头	Nh	A	与输入相同
响应代码	2	A	“A3”
出错代码	2	N	正常为“00”
			“13”：LMK错
“15”：输入数据错误
“17”：授权认证失败
“18”：格式没有定义或打印机故障
Component	16H/
1A+32H/
1A+48H	H          	变种的LMK加密后的密钥分量
*/

int UnionSJL05CmdA2(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component)
{
	int	ret;
	char	hsmCmdBuf[1024+1];
	int	hsmCmdLen = 0;
	int	i;

	memcpy(hsmCmdBuf,"A2",2);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = 0;
	
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA2:: UnionTranslateHsmKeyTypeString!\n");
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
		UnionUserErrLog("in UnionSJL05CmdA2:: UnionTranslateHsmKeyKeyScheme!\n");
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


        UnionLog ("hsmbuf is [%s]\n",hsmCmdBuf);
        hsmCmdBuf[hsmCmdLen]=0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA2:: UnionDirectHsmCmd!\n");
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
			UnionUserErrLog("in UnionSJL05CmdA2:: unknown key length!\n");
			return(errCodeParameter);
	}
}

/*
   函数功能：
   A4指令，将密钥分量密文送入密码机解密后合成一个密钥，并用对应LMK加密后返回。
   输入参数：
   keyType：密钥的类型
   keyLength：密钥的长度
   partKeyNum：密钥成分的数量
   partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
   输出参数：
   keyByLMK：合成的密钥密文，由LMK加密
   checkValue：合成的密钥的校验值

 */
int UnionSJL05CmdA4(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[1024+1];
	int	hsmCmdLen = 0;
	int	i;

	if ((keyByLMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionSJL05CmdA4:: wrong parameters!\n");
		return(errCodeParameter);
	}
	if (partKeyNum < 2 || partKeyNum > 9)
	{
		UnionUserErrLog("in UnionSJL05CmdA4:: partKeyNum [%d] must between 2 and 9!\n",partKeyNum);
		return(errCodeParameter);
	}

	// 命令字
	memcpy(hsmCmdBuf,"A4",2);
	hsmCmdLen += 2;
	// 密钥成分数量
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// 密钥类型代码
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA4:: UnionTranslateHsmKeyTypeString!\n");
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
			UnionUserErrLog("in UnionSJL05CmdA4:: UnionTranslateHsmKeyKeyScheme!\n");
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
				UnionUserErrLog("in UnionSJL05CmdA4:: UnionGenerateX917RacalKeyString for [%s][%d]\n",partKey[i],keyLength);
				return(ret);
			}
			hsmCmdLen += ret;
		}
	}

        UnionLog ("hsmbuf is [%s]\n",hsmCmdBuf);
        hsmCmdBuf[hsmCmdLen]=0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA4:: UnionDirectHsmCmd!\n");
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
			UnionUserErrLog("in UnionSJL05CmdA4:: unknown key length!\n");
			return(errCodeParameter);
	}
}

int UnionSJL05CmdA8(TUnionDesKeyType keyType,char *key,char *zmk,char *keyByZMK,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[512+1];
	int	hsmCmdLen = 0;
	int	keyLen;
	int	offsetOfKeyByZMK;
	int	offsetOfCheckValue;

	memcpy(hsmCmdBuf,"A8",2);
	hsmCmdLen += 2;

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(zmk),zmk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(key),key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA8:: UnionGenerateX917RacalKeyString!\n");
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
			UnionUserErrLog("in UnionSJL05CmdA8:: keylen [%d] error!\n",(int)strlen(key));
			return(errCodeParameter);
	}
	hsmCmdLen++;

        UnionLog ("hsmbuf is [%s]\n",hsmCmdBuf);
        hsmCmdBuf[hsmCmdLen]=0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdA8:: UnionDirectHsmCmd!\n");
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
			UnionUserErrLog("in UnionSJL05CmdA8:: invalid keyLen[%d]\n",(int)strlen(key));
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

int UnionSJL05CmdBU(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	char		tmpBuf1[100];

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
		memcpy(hsmCmdBuf+hsmCmdLen,key,strlen(key));
		hsmCmdLen += strlen(key);
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

#ifndef MAXMSGBLOCK
#define MAXMSGBLOCK 4000 
//#define MAXMSGBLOCK 512 
#endif
/*一次性发送加密机数据块最大长度*/
int UnionSJL05CmdE0(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData)
{
	int     nLoop,i;
	int     ret;
	int     p_len =0;
	int     lastDataLen;

	UnionSetBCDPrintTypeForHSMResCmd();

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
			UnionUserErrLog("in UnionSJL05CmdE0 : SizeOfBuffer[%d] > sizeOfData[%d]\n",ret,sizeOfEncryptedData);
			return (errCodeSmallBuffer);
		}

	if ( MsgLen <= MAXMSGBLOCK )
	{
		return UnionSJL05CmdE0_f(crytoFlag,blockFlag,encrypMode,zekLen,\
				Zek,datatype,IV,MsgLen,Msg,\
				encrypData,encryDataLen,sizeOfEncryptedData);
	}

	if ( MsgLen > MAXMSGBLOCK )
	{
		nLoop = MsgLen / MAXMSGBLOCK;

		for ( i = 0; i < nLoop; i++ )
		{
			ret = UnionSJL05CmdE0_f(crytoFlag,blockFlag,encrypMode,zekLen,\
					Zek,datatype,IV,MAXMSGBLOCK,Msg + i *MAXMSGBLOCK,\
					encrypData + i*MAXMSGBLOCK,&p_len,sizeOfEncryptedData);
			if ( ret < 0  )
			{
				UnionUserErrLog("in UnionSJL05CmdE0 : RacalCmdE0_f error !!\n");
				return ( ret );
			}
			*encryDataLen += p_len;
		}

		lastDataLen = MsgLen % MAXMSGBLOCK;
		if ( lastDataLen != 0 )
		{
			ret = UnionSJL05CmdE0_f(crytoFlag,blockFlag,encrypMode,zekLen,\
					Zek,datatype,IV,lastDataLen,Msg + i *MAXMSGBLOCK,\
					encrypData + i*MAXMSGBLOCK,&p_len,sizeOfEncryptedData);

			if ( ret < 0  )
			{
				UnionUserErrLog("in UnionSJL05CmdE0 : UnionSJL05CmdE0_f error !!\n");
				return ( ret );
			}

			*encryDataLen += p_len;
		}
	}
	return *encryDataLen;
}

int UnionSJL05CmdE0_f(int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData)
{
	int     offset,ret;
	char    iMsg_Len[10];
	char    hsmCmd[2+1];
	char	hsmCmdBuf[MAXMSGBLOCK+ 256 + 1];
	char    v_zek[60 + 1];
	offset = 0;
	
	strcpy(hsmCmd+offset,"E0");

	if( blockFlag < 0 ||  blockFlag > 3 )
	{
		UnionUserErrLog("in UnionSJL05CmdE0_f:: parameter error! blockFlag[%d]\n",blockFlag);
		return(errCodeParameter);
	}
	if( encrypMode < 1 ||encrypMode > 5 )
	{
		UnionUserErrLog("in UnionSJL05CmdE0_f:: parameter  encrypMode=[%d]error!\n",encrypMode);
		return(errCodeParameter);
	}
	sprintf(hsmCmdBuf+offset ,"%s%.1d%.1d%.1d%.1d",hsmCmd,blockFlag,crytoFlag,encrypMode,0);
	offset += 6;

	memset(v_zek,0,sizeof(v_zek));
	ret = UnionPutKeyIntoRacalKeyString(Zek,v_zek,sizeof(v_zek));
	if ( ret < 0 )
	{
		UnionUserErrLog("in UnionSJL05CmdE0_f:: UnionTranslateHsmKeyKeyScheme!\n");
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
		UnionUserErrLog("in UnionSJL05CmdE0_f:: parameter  datatype=[%d]error!\n",datatype);
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

        UnionLog ("hsmbuf is [%s]\n",hsmCmdBuf);
        hsmCmdBuf[offset]=0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdE0_f:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	memcpy(iMsg_Len,hsmCmdBuf + 4 + 1,3);
	iMsg_Len[3] = 0;

	*encryDataLen = UnionOxToD(iMsg_Len);
	if ( datatype == 1 )
		*encryDataLen *= 2;

	if ( encrypData != NULL && ret-8 <= sizeOfEncryptedData )
		memcpy(encrypData,hsmCmdBuf + 8,*encryDataLen);
	else 
		return(errCodeSmallBuffer);

	return *encryDataLen;
}

// 生成一对RSA密钥
/* 输入参数
   mode，模式	“0”－产生密钥并保存在HSM内，只输出公钥
							“1”－产生密钥并保存在HSM内，输出公钥和LMK保护的私钥
							“2”－产生密钥，输出公钥和LMK保护的私钥
							“3”－产生密钥并保存在HSM内，不输出公私钥
							“4”-产生密钥并将密钥存入到IC卡备份
   length，模数	0320/512/1024/2048
   pkEncoding	公钥的编码方式
   index	产生密钥存储在HSM内的索引号。Mode!=0时，该域不存在，如：G000
   lenOfPKExponent	公钥exponent的长度	可选参数
   pkExponent	公钥的pkExponent	可选参数
   sizeOfPK	接收公钥的缓冲大小
   sizeOfVK	接收私钥的缓冲大小
   输出参数
   pk		公钥
   lenOfVK		私钥串的长度
   vk		私钥
 */
int UnionSJL05CmdEI(char mode,int length,char *pkEncoding,char *index,
		int lenOfPKExponent,unsigned char *pkExponent,char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK)
{
	int		ret;
	char	hsmCmdBuf[8096+1];
	int	hsmCmdLen = 0;
	int	lenOfPK = 0;
	int	lenOfPKStr;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf,"EI",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = mode;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",length);
	hsmCmdLen += 4;
	
	if (mode != '3')
	{
		if ((pkEncoding == NULL) || (strlen(pkEncoding) == 0))
			memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
		else
			memcpy(hsmCmdBuf+hsmCmdLen,pkEncoding,2);
		hsmCmdLen += 2;
	}

	if (mode == '0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,index,4);
		hsmCmdLen += 4;
	}
	
	if ((pkExponent != NULL) && (lenOfPKExponent > 0))
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfPKExponent);
		hsmCmdLen += 4;
		if (lenOfPKExponent + hsmCmdLen >= sizeof(hsmCmdBuf))
		{
			UnionUserErrLog("in UnionSJL05CmdEI:: lenOfPKExponent [%d] too long !\n",lenOfPKExponent);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,pkExponent,lenOfPKExponent);
		hsmCmdLen += lenOfPKExponent;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdEI:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	if (mode != '3')
	{
		if ((lenOfPKStr = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)hsmCmdBuf+4,ret-4,pk,&lenOfPK,sizeOfPK)) < 0)
		{
			UnionUserErrLog("in UnionSJL05CmdEI:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
			UnionMemLog("in UnionSJL05CmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
			return(lenOfPKStr);
		}
		if (lenOfPKStr + 4 > ret - 4)
		{
			UnionUserErrLog("in UnionSJL05CmdEI:: lenOfPKStr + 4 = [%d] longer than [%d]!\n",lenOfPKStr+4,ret-4);
			UnionMemLog("in UnionSJL05CmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
			return(errCodeParameter);
		}
	}		

	if (mode != '0' && mode != '3')
	{	
		*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+lenOfPKStr+4,4);
		if ((*lenOfVK <= 0) || (*lenOfVK + 4 + 4 + lenOfPKStr > ret) || (*lenOfVK > sizeOfVK))
		{
			UnionUserErrLog("in UnionSJL05CmdEI:: lenOfVK [%d] error!\n",*lenOfVK);
			UnionMemLog("in UnionSJL05CmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
			return(errCodeParameter);
		}
		memcpy(vk,hsmCmdBuf+4+lenOfPKStr+4,*lenOfVK);
		vk[*lenOfVK] = 0;
	}

	return(lenOfPK);
}

/*
函数功能：
	EW指令，用私钥签名 
输入参数：
	hashID:	HASH标识
	signID:	签名标识
	padModeID: 填充模式标识	
	indexOfVK：私钥索引号
	signDataLength：待签名数据的长度
	signData：待签名的数据
	vkLength：LMK加密的私钥长度
	vk：LMK加密的私钥
输出参数：
	signature：生成的签名
返回值：
	<0，函数执行失败，无signature
	0，加密机执行指令失败
	>0，成功，返回signature的长度
*/

int UnionSJL05CmdEW(char *hashID,char *signID,char *padModeID,char *indexOfVK,int signDataLength,char *signData,int vkLength,char *vk,char *signature)
{
	int	ret;
	char	hsmCmdBuf[512+1],tmpBuf[10];
	int	hsmCmdLen = 0;
	
	if ((indexOfVK == NULL) || (signData == NULL) || (vk == NULL) || (signature == NULL))
	{
		UnionUserErrLog("in UnionSJL05CmdEW:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字

	memcpy(hsmCmdBuf,"EW",2);
	hsmCmdLen = 2;
	// Hash Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,hashID,2);
	hsmCmdLen += 2;
	// Signature Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,signID,2);
	hsmCmdLen += 2;
	// Pad Mode Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,padModeID,2);
	hsmCmdLen += 2;
	// 待签名数据的长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signDataLength);
	hsmCmdLen += 4;
	// 待签名的数据
	memcpy(hsmCmdBuf+hsmCmdLen,signData,signDataLength);
	hsmCmdLen += signDataLength;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// 私钥索引号
	memcpy(hsmCmdBuf+hsmCmdLen,indexOfVK,2);
	hsmCmdLen += 2;
	if(atoi(indexOfVK)>20)
	{
		// LMK加密的私钥长度
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLength);
		hsmCmdLen += 4;
		// LMK加密的私钥
		memcpy(hsmCmdBuf+hsmCmdLen,vk,vkLength);
		hsmCmdLen += vkLength;
	}
	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdEI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	// 生成的签名长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	tmpBuf[4] = 0;
	// 生成的签名
	memcpy(signature,hsmCmdBuf+4+4,atoi(tmpBuf));
	signature[atoi(tmpBuf)] = 0;
	return(atoi(tmpBuf));
}


/*
   函数功能：
   UA指令，请求转换RSA私钥从LMK到KEK,LMK保护的RSA私钥为REF结构,KEK使用Mode定义的算法保护REF结构的RSA私钥的每个元素。
   输入参数：
	keyType：密钥类型
	key: 该密钥可由HSM内安全非易失存储区内密钥以及由LMK加密的密文送入
	vkLength: 私钥数据的数据长度
	vk: nB 由LMK保护的RSA私钥（REF结构）
	输出参数：
	vkByKey: key加密的vk数据串
 */

int UnionSJL05CmdUA(TUnionDesKeyType keyType,char *key, int vkLength, char *vk, char *vkByKey)
{
	int     ret;
	int     hsmCmdLen = 0;
	char    lenOfVK[4 + 1];
	char    hsmCmdBuf[1024 * 8 +1];
	char    tmpBuf[1024 * 8 +1];

	UnionSetBCDPrintTypeForHSMCmd();

	if ((key == NULL) || (vk == NULL) || (vkByKey == NULL))
	{
		UnionUserErrLog("in UnionSJL05CmdUA:: parameters error!\n");
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf, "UA", 2);
	hsmCmdLen += 2;

	//密钥类型
	UnionTranslateDesKeyTypeTo3CharFormat(keyType,tmpBuf);
	memcpy(hsmCmdBuf + hsmCmdLen, tmpBuf, 3);
	hsmCmdLen += 3;

	if ((ret = UnionPutKeyIntoRacalKeyString(key,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUA:: UnionPutKeyIntoRacalKeyString [%s]!\n",key);
		return(ret);
	}
	hsmCmdLen += ret;

	sprintf(lenOfVK, "%04d", vkLength);
	memcpy(hsmCmdBuf + hsmCmdLen, lenOfVK, 4);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf + hsmCmdLen, vk, vkLength);
	hsmCmdLen += vkLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	memcpy(vkByKey, hsmCmdBuf + 4, ret - 4);
	vkByKey[ret - 4] = 0;

	return ret - 4;
}


/*
   函数功能：
   UC指令，使用指定的应用主密钥进行离散得到卡片应用子密钥，
   使用指定控制密钥子密钥进行加密保护输出并进行MAC计算。
   输入参数：
  
  algoType:	算法标识
	mk: 根密钥
	mkType: 根密钥类型109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
	mkDvsNum: 根密钥离散次数, 1-3次
	mkDvsFlag: 根密钥离散算法 1、银联标准
	mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

	pkType: 保护密钥类型 0=TK(传输密钥)1=DK-SMC(使用MK-SMC实时离散生成的子密钥)
	pk: 保护密钥
	pkDvsNum: 保护密钥离散次数,仅当“保护密钥类型”为1时有,从MK-SMC离散得到DK-SMC的离散次数,范围为1-3
	pkDvsFlag: 保护密钥分散算法 1、银联标准
	pkDvsData: 保护密钥离散数据,n*16H仅当“保护密钥类型”为1时有,保护密钥的离散数据，其中n为“保护密钥离散次数”

输出参数：
criperDataLen: 密文数据长度 4H 密文数据长度(必须是8的倍数，并等于前缀长度、后缀长度、密钥长度的和)
criperData: nB 输出的密文数据
checkValue: 校验值
 */
int UnionSJL05CmdUC(char algoType, char *mkType, char *mk,
		int mkDvsNum, char *mkDvsFlag, char *mkDvsData, char *pkType, char *pk,
		int pkDvsNum, char *pkDvsFlag, char *pkDvsData, int *criperDataLen, char *criperData, char *checkValue)
{
	int     ret;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	int     offset = 0;
	char    szMkBuff[64+1];

	// 命令代码
	memcpy(hsmCmdBuf,"UC",2);
	hsmCmdLen += 2;

	// 算法标识
	hsmCmdBuf[hsmCmdLen] = algoType;
	hsmCmdLen++;

	// 主控密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 3);
	hsmCmdLen += 3;

	// 主控密钥
	memset(szMkBuff, 0, sizeof(szMkBuff));
	ret = UnionGenerateX917RacalKeyString(strlen(pk), pk, szMkBuff);
	memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
	hsmCmdLen += ret;

	// 主控密钥分散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
	hsmCmdLen += 1;

	// 主控密钥分散算法和分散数据
	if (pkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*strlen(pkDvsData));
		hsmCmdLen += pkDvsNum*strlen(pkDvsData);
	}

	// 应用主密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 应用主密钥
	memset(szMkBuff, 0, sizeof(szMkBuff));
	ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
	memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
	hsmCmdLen += ret;
	
	// 应用主密钥分散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 应用主密钥分散算法和分散数据
	if (mkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*strlen(mkDvsData));
		hsmCmdLen += mkDvsNum*strlen(mkDvsData);
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUC:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	// 返回校验值
	if (algoType == '1')
	{
		memcpy(checkValue, hsmCmdBuf + ret - 16, 16);
		checkValue[16] = 0;
		offset += 16;
	}
	else if (algoType == '7')
	{
		memcpy(checkValue, hsmCmdBuf + ret - 32, 32);
		checkValue[32] = 0;
		offset += 32;
	}

	// 密文数据长度
	offset += 2;
	*criperDataLen = ret - offset;

	// 密文数据
	memcpy(criperData, hsmCmdBuf + 6, *criperDataLen);
	criperData[*criperDataLen] = 0;

	return(0);
}

int UnionSJL05CmdQA(char *mkType, char *mk,
		int mkDvsNum, char *mkDvsFlag, char *mkDvsData, char *pkType, char *pk,
		int pkDvsNum, char *pkDvsFlag, char *pkDvsData, int *criperDataLen, char *criperData, char *checkValue)
{
	int     ret;
	int     lenOfDvsData = 16;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	// 命令代码
	memcpy(hsmCmdBuf,"QA",2);
	hsmCmdLen += 2;

	// 主控密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 3);
	hsmCmdLen += 3;

	// 主控密钥
	if (strlen(pk) == 32)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, pk, strlen(pk));
		hsmCmdLen += strlen(pk);
	}

	// 主控密钥分散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
	hsmCmdLen += 1;

	// 主控密钥分散算法和分散数据
	if (pkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
		hsmCmdLen += pkDvsNum*lenOfDvsData;
	}

	// 应用主密钥类型
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// 应用主密钥
	if (strlen(mk) == 32)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, mk, strlen(mk));
		hsmCmdLen += strlen(mk);
	}

	// 应用主密钥分散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 应用主密钥分散算法和分散数据
	if (mkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
		hsmCmdLen += mkDvsNum*lenOfDvsData;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdQA:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	// 返回校验值
	memcpy(checkValue, hsmCmdBuf + ret - 32, 32);
	checkValue[32] = 0;
	offset += 32;

	// 密文数据长度
	offset += 4;
	*criperDataLen = ret - offset;

	// 密文数据
	memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
	criperData[*criperDataLen] = 0;
	
	return(0);
}

// 生成一对SM2密钥
/* 输入参数
   	type，类型
	1：签名；2：加密；3：签名和加密
   	length，长度 固定256
   	index,	密钥索引
   	keyPasswd,	密钥口令
   	sizeOfPK	接收公钥的缓冲大小
   	sizeOfVK	接收私钥的缓冲大小
   	输出参数
	pk		公钥
   	lenOfVK		私钥串的长度
   	vk		私钥
   	vkByLmk		纯私钥密文
 */
int UnionSJL05CmdUO(char type,int length,	char *index, char *keyPasswd, char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,unsigned char *vkByLmk)
{
	int	ret;
	char	hsmCmdBuf[4096+1];
	int	hsmCmdLen = 0;
	char	bcdPK[4096+1];

	UnionSetBCDPrintTypeForHSMResCmd();	

	memcpy(hsmCmdBuf,"UO",2);
	hsmCmdLen = 2;
	
	// 密钥长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",length);
	hsmCmdLen += 4;
	
	// 密钥用途
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	
	// 密钥索引
	if (strncmp(index, "99", 2) != 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",index);
		hsmCmdLen += 2;
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",keyPasswd);
		hsmCmdLen += 8;
	}
	else
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%2d",99);
		hsmCmdLen += 2;	
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUO:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+4,4);
	if ((*lenOfVK <= 0) || (*lenOfVK + 4 + 4 > ret) || (*lenOfVK > sizeOfVK))
	{
		UnionUserErrLog("in UnionSJL05CmdUO:: lenOfVK [%d] error!\n",*lenOfVK);
		UnionMemLog("in UnionSJL05CmdUO:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	memcpy(vk,hsmCmdBuf+4+4,*lenOfVK);
	vk[*lenOfVK] = 0;
	memcpy(bcdPK,hsmCmdBuf+4+4+*lenOfVK,64);
	bcdPK[64] = 0;
	bcdhex_to_aschex(bcdPK,64,pk);
	pk[128] = 0;
	memcpy(vkByLmk,hsmCmdBuf+4+4+*lenOfVK+64,32);
	vkByLmk[32] = 0;
	return(128);
}

// 生成签名的指令
int UnionSJL05CmdUQ(char *hashID, int lenOfUsrID, char *usrID, int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign)
{
	int	ret;
	char	hsmCmdBuf[8192+1];
	int	offset = 0;

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf + offset,"UQ",2);
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
		UnionUserErrLog("in UnionSJL05CmdUQ:: UnionDirectHsmCmd!\n");
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

/*
函数功能：
	EY指令，用公钥验证签名
输入参数：
	macOfPK：公钥的MAC值
	signatureLength：待验证的签名的长度
	signature：待验证的签名
	signDataLength：待签名数据的长度
	signData：待签名的数据
	publicKeyLength：公钥的长度
	publicKey：公钥
输出参数：
	无
返回值：
        <0，函数执行失败
        0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int UnionSJL05CmdEY(char *macOfPK,int signatureLength,char *signature,int signDataLength,char *signData,int publicKeyLength,char *publicKey)
{       
	int	ret;
	char	hsmCmdBuf[512+1];
	int	hsmCmdLen = 0;

	if ((macOfPK == NULL) || (signData == NULL) || (publicKey == NULL) || (signature == NULL))
	{       
		UnionUserErrLog("in UnionSJL05CmdEY:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memcpy(hsmCmdBuf,"EY",2);
	hsmCmdLen = 2;
	// Hash Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
	hsmCmdLen += 2;
	// Signature Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
	hsmCmdLen += 2;
	// Pad Mode Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
	hsmCmdLen += 2;
	// 待验证的签名长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signatureLength);
	hsmCmdLen += 4;
	// 待验证的签名
	memcpy(hsmCmdBuf+hsmCmdLen,signature,signatureLength);
	hsmCmdLen += signatureLength;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// 待签名数据的长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signDataLength);
	hsmCmdLen += 4;
	// 待签名的数据
	memcpy(hsmCmdBuf+hsmCmdLen,signData,signDataLength);
	hsmCmdLen += signDataLength;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// 公钥的MAC值
	memcpy(hsmCmdBuf+hsmCmdLen,macOfPK,4);
	hsmCmdLen += 4;
	// 公钥
	memcpy(hsmCmdBuf+hsmCmdLen,publicKey,publicKeyLength);
	hsmCmdLen += publicKeyLength;
	hsmCmdBuf[hsmCmdLen] = 0;
	
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdEY:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	return(0);
}

int UnionSJL05CmdTI(int pkLength, char *pk, int inputDataLength, unsigned char *inputData, unsigned char *outputData)
{
	int	ret;
	char	bcdDerPK[4096+1];
	
	bcdhex_to_aschex(pk,pkLength,bcdDerPK);
	bcdDerPK[pkLength*2] = 0;
	ret = UnionPKDec(inputData, inputDataLength, bcdDerPK, 0, outputData);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdTI:: UnionPKDec error!\n");
		return(ret);
	}

	return 0;
}

//生成验签的指令
int UnionSJL05CmdUS(int vkIndex, char *hashID, int lenOfUsrID, char *usrID, int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal)
{
	int	ret;
	char	hsmCmdBuf[8192*2+1];
	int	offset = 0;

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf + offset,"US",2);
	offset += 2;
	if (vkIndex < 0)
	{
		// 密钥索引
		memcpy(hsmCmdBuf + offset,"99",2);
		offset += 2;
		// 外带公钥
		aschex_to_bcdhex(rCaPK,lengRCaPK,hsmCmdBuf + offset);
		offset += lengRCaPK/2;
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
	offset += 4;

	// 数据
	aschex_to_bcdhex(hashVal,lenHashVal,hsmCmdBuf + offset);
	offset += lenHashVal/2;

	hsmCmdBuf[offset] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUS:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}

// 国密算法随机产生密钥
int UnionSJL05CmdWI(char mode, char *mediaType, char keyType,int compNum, int keyLen, int derivateNum, char derivateData[][32 + 1], int printNum,	char printFld[][32], char *value, char *checkvalue)
{
	int	ret;
	char	hsmCmdBuf[8096+1];
	int	hsmCmdLen = 0;
	int	i = 0;
	int	derivateDataLen = 0;
	int	printFldLen = 0;
	int	keyOffset = 0;
	int	lenOfValue = 0;
		
	// 命令码
	memcpy(hsmCmdBuf, "WI", 2);
	hsmCmdLen += 2;
	
	// Mode
	hsmCmdBuf[hsmCmdLen] = mode;
	hsmCmdLen += 1;
	
	// MediaType
	memcpy(hsmCmdBuf + hsmCmdLen, mediaType, strlen(mediaType));
	hsmCmdLen += 2;

	// keyType
	if ((ret = UnionTranslate30HsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdWI:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	
	// CompNum
	hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%d", compNum);
	if(mode=='0')
	{	
		// keyLen
		if ((ret = UnionTranslate30HsmKeyKeyScheme(keyLen, &keyLen)) < 0)
		{
			UnionUserErrLog("in UnionSJL05CmdWI:: UnionTranslate30HsmKeyKeyScheme error, ret[%d].\n", ret);
			return ret;
		}
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%04d", keyLen);
		hsmCmdLen += ret;	
	}
	if(mode=='1')
	{
		// DerivateNum
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%d", derivateNum);
		
		// derivateData
		for (i = 0; i < derivateNum; ++i)
		{
			derivateDataLen = strlen(derivateData[i]);
			memcpy(hsmCmdBuf + hsmCmdLen, derivateData[i], derivateDataLen);
			hsmCmdLen += derivateDataLen;
		}
	}
	
	if(memcmp(mediaType,"10",2)==0||memcmp(mediaType,"11",2)==0)	
	{
		// PrintNum
		hsmCmdLen += sprintf(hsmCmdBuf + hsmCmdLen, "%d", printNum);
		
		// Print Fields
		for (i = 0; i < printNum; ++i)
		{
			printFldLen = strlen(printFld[i]);
			memcpy(hsmCmdBuf + hsmCmdLen, printFld[i], printFldLen);
			hsmCmdLen += printFldLen;
			if (i + 1 < printNum)
			{
				hsmCmdBuf[hsmCmdLen] = ';';
				hsmCmdLen += 1;
			}
		}
	}

	hsmCmdBuf[hsmCmdLen] = 0;
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdWI:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	
	// 用LMK加密的Key
	lenOfValue = 32;
	keyOffset = 5;
	
	memcpy(value, hsmCmdBuf + keyOffset, lenOfValue);
	value[lenOfValue] = 0;
	memcpy(checkvalue, hsmCmdBuf + keyOffset + lenOfValue, 16);
	checkvalue[16] = 0;
	
	return lenOfValue;
}

// 计算密钥校验值
int UnionSJL05CmdBS(int SM4Mode, TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue)
{
	int	ret;
	char	hsmCmdBuf[1024+1];
	int	hsmCmdLen = 0;
	char	tmpBuf[100];
	char	tmpBuf1[100];

	if ((key == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in UnionSJL05CmdBS:: wrong parameters!\n");
		return(errCodeParameter);
	}

	// 命令字
	memcpy(hsmCmdBuf,"BS",2);
	hsmCmdLen = 2;
	// 加密密钥的LMK对指示符，即密钥类型代码的最后2位
	memset(tmpBuf,0,sizeof(tmpBuf));
	memset(tmpBuf1,0,sizeof(tmpBuf1));

	if ((ret = UnionTranslateHsmKeyTypeStringForBU(keyType,tmpBuf,tmpBuf1)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdBS:: UnionTranslateHsmKeyTypeStringForBU!\n");
		return(ret);
	}
	
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
			UnionUserErrLog("in UnionSJL05CmdBS:: unknown key length!\n");
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
			UnionUserErrLog("in UnionSJL05CmdBS:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
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
		UnionUserErrLog("in UnionSJL05CmdBS:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	
	memcpy(checkValue,hsmCmdBuf+4,16);
	checkValue[16] = 0;
	return(16);
}

//SM2私钥转加密
/*输入参数
  algoType	算法标识
  vkByLmk		私钥密文，LMK(36,37)加密
  keyType		密钥类型
  key		主密钥加密下的传输(SM4)密钥
  encMode	加密算法模式
  iv		CBC模式时存在  DES/3DES ：8字节   SM4：16字节
  
  输出参数
  vk		加密下的SM2私钥密文
 */
int UnionSJL05CmdUY(char algoType, unsigned char *vkByLmk, TUnionDesKeyType keyType,char *key,char encMode,char *iv,char *vk)
{
	int	ret;
	char	hsmCmdBuf[1024+1];
	char	szMkBuff[128+1];
	int	hsmCmdLen = 0;
	
	memcpy(hsmCmdBuf,"UY",2);
	hsmCmdLen += 2;
	
	//密钥算法标识
	hsmCmdBuf[hsmCmdLen] = algoType;
	hsmCmdLen++;

	//私钥密文
	memcpy(hsmCmdBuf+hsmCmdLen,vkByLmk,32);
	hsmCmdLen += 32;

	//密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUY:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += 3;

	//LMK加密的传输（SM4）密钥
	memset(szMkBuff, 0, sizeof(szMkBuff));
        ret = UnionGenerateX917RacalKeyString(strlen(key), key, szMkBuff);
        memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
        hsmCmdLen += ret;

	//加密算法啊模式
	hsmCmdBuf[hsmCmdLen] = encMode;
	hsmCmdLen++;

	if (encMode == '1')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,iv,strlen(iv));
		hsmCmdLen += strlen(iv);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	//与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUY:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	
	memcpy(vk,hsmCmdBuf+4,64);
	vk[64] = 0;
	return(64);
}



// 定义打印格式
/*
输入参数：
	format 	打印格式控制符，长度最多不超过299

*/


int UnionSJL05CmdPA(char *format)
{
	int	     ret;
	char	    hsmCmdBuf[1024+1];
	int	     hsmCmdLen = 0;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"PA",2);
	hsmCmdLen = 2;
	strcpy(hsmCmdBuf+hsmCmdLen,format);
	hsmCmdLen += strlen(format);
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdPA:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}



//生成SM4密钥并打印一个成份
/* 输入参数
   keyType              密钥类型
   fldNumi              打印域个数
   fld          打印域 
   输出参数
   component    成分
 */
int UnionSJL05CmdWM(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component)
{
	int     ret;
	char    hsmCmdBuf[1024+1];              
	int     hsmCmdLen = 0;  
	int     i;
                
	memcpy(hsmCmdBuf,"WM",2);
	hsmCmdLen = 2;
	//密钥类型
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{       
		UnionUserErrLog("in UnionSJL05CmdWM:: UnionTranslateHsmKeyTypeString!\n");
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
		UnionUserErrLog("in UnionSJL05CmdWM:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//提取成分
	memcpy(component,hsmCmdBuf+4+1,32);
	component[32] = 0;
	return(32);
}

