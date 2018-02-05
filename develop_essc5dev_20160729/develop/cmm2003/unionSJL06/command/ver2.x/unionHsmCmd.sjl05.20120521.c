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
#include "unionSJL05Cmd.h"
#include "commWithHsmSvr.h"

#include "unionRSA.h"

// ʹ�������ZMK����
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
	
	// ������
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
	
	// ��LMK���ܵ�Key
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
������	����	����	��ע
������	2	A	��A2��
Key type	3	H	��Կ����
Key scheme ��LMK��	1	A	��LMK���ܷ�ʽ��־
Print Field 0	n	A	��ӡ�ֶ�0����������;��
Delimiter	1	A	ֵΪ����������ӡ�ֶν�����
Print Field 1	n	A	��ӡ�ֶ�1����������;��
��	��	��	��
Last Print Field 	n	A	���һ����ӡ�ֶΣ���������;��
��Ϣβ	Nt	A	
�����	����	����	��ע
��Ϣͷ	Nh	A	��������ͬ
��Ӧ����	2	A	��A3��
�������	2	N	����Ϊ��00��
			��13����LMK��
��15�����������ݴ���
��17������Ȩ��֤ʧ��
��18������ʽû�ж�����ӡ������
Component	16H/
1A+32H/
1A+48H	H          	���ֵ�LMK���ܺ����Կ����
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
   �������ܣ�
   A4ָ�����Կ��������������������ܺ�ϳ�һ����Կ�����ö�ӦLMK���ܺ󷵻ء�
   ���������
   keyType����Կ������
   keyLength����Կ�ĳ���
   partKeyNum����Կ�ɷֵ�����
   partKey����Ÿ�����Կ�ɷֵ����黺�壬ΪLMK���ܵ���Կ����
   ���������
   keyByLMK���ϳɵ���Կ���ģ���LMK����
   checkValue���ϳɵ���Կ��У��ֵ

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

	// ������
	memcpy(hsmCmdBuf,"A4",2);
	hsmCmdLen += 2;
	// ��Կ�ɷ�����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// ��Կ���ʹ���
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
			UnionUserErrLog("in UnionSJL05CmdA4:: UnionTranslateHsmKeyKeyScheme!\n");
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
		memcpy(hsmCmdBuf+hsmCmdLen,key,strlen(key));
		hsmCmdLen += strlen(key);
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

#ifndef MAXMSGBLOCK
#define MAXMSGBLOCK 4000 
//#define MAXMSGBLOCK 512 
#endif
/*һ���Է��ͼ��ܻ����ݿ���󳤶�*/
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

// ����һ��RSA��Կ
/* �������
   mode��ģʽ	��0����������Կ��������HSM�ڣ�ֻ�����Կ
							��1����������Կ��������HSM�ڣ������Կ��LMK������˽Կ
							��2����������Կ�������Կ��LMK������˽Կ
							��3����������Կ��������HSM�ڣ��������˽Կ
							��4��-������Կ������Կ���뵽IC������
   length��ģ��	0320/512/1024/2048
   pkEncoding	��Կ�ı��뷽ʽ
   index	������Կ�洢��HSM�ڵ������š�Mode!=0ʱ�����򲻴��ڣ��磺G000
   lenOfPKExponent	��Կexponent�ĳ���	��ѡ����
   pkExponent	��Կ��pkExponent	��ѡ����
   sizeOfPK	���չ�Կ�Ļ����С
   sizeOfVK	����˽Կ�Ļ����С
   �������
   pk		��Կ
   lenOfVK		˽Կ���ĳ���
   vk		˽Կ
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
�������ܣ�
	EWָ���˽Կǩ�� 
���������
	hashID:	HASH��ʶ
	signID:	ǩ����ʶ
	padModeID: ���ģʽ��ʶ	
	indexOfVK��˽Կ������
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	vkLength��LMK���ܵ�˽Կ����
	vk��LMK���ܵ�˽Կ
���������
	signature�����ɵ�ǩ��
����ֵ��
	<0������ִ��ʧ�ܣ���signature
	0�����ܻ�ִ��ָ��ʧ��
	>0���ɹ�������signature�ĳ���
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
	// ������

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
	// ��ǩ�����ݵĳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signDataLength);
	hsmCmdLen += 4;
	// ��ǩ��������
	memcpy(hsmCmdBuf+hsmCmdLen,signData,signDataLength);
	hsmCmdLen += signDataLength;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// ˽Կ������
	memcpy(hsmCmdBuf+hsmCmdLen,indexOfVK,2);
	hsmCmdLen += 2;
	if(atoi(indexOfVK)>20)
	{
		// LMK���ܵ�˽Կ����
		sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLength);
		hsmCmdLen += 4;
		// LMK���ܵ�˽Կ
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

	// ���ɵ�ǩ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	tmpBuf[4] = 0;
	// ���ɵ�ǩ��
	memcpy(signature,hsmCmdBuf+4+4,atoi(tmpBuf));
	signature[atoi(tmpBuf)] = 0;
	return(atoi(tmpBuf));
}


/*
   �������ܣ�
   UAָ�����ת��RSA˽Կ��LMK��KEK,LMK������RSA˽ԿΪREF�ṹ,KEKʹ��Mode������㷨����REF�ṹ��RSA˽Կ��ÿ��Ԫ�ء�
   ���������
	keyType����Կ����
	key: ����Կ����HSM�ڰ�ȫ����ʧ�洢������Կ�Լ���LMK���ܵ���������
	vkLength: ˽Կ���ݵ����ݳ���
	vk: nB ��LMK������RSA˽Կ��REF�ṹ��
	���������
	vkByKey: key���ܵ�vk���ݴ�
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

	//��Կ����
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

	// �������ͨѶ
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
   �������ܣ�
   UCָ�ʹ��ָ����Ӧ������Կ������ɢ�õ���ƬӦ������Կ��
   ʹ��ָ��������Կ����Կ���м��ܱ������������MAC���㡣
   ���������
  
  algoType:	�㷨��ʶ
	mk: ����Կ
	mkType: ����Կ����109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
	mkDvsNum: ����Կ��ɢ����, 1-3��
	mkDvsFlag: ����Կ��ɢ�㷨 1��������׼
	mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

	pkType: ������Կ���� 0=TK(������Կ)1=DK-SMC(ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ)
	pk: ������Կ
	pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
	pkDvsFlag: ������Կ��ɢ�㷨 1��������׼
	pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������

���������
criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
criperData: nB �������������
checkValue: У��ֵ
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

	// �������
	memcpy(hsmCmdBuf,"UC",2);
	hsmCmdLen += 2;

	// �㷨��ʶ
	hsmCmdBuf[hsmCmdLen] = algoType;
	hsmCmdLen++;

	// ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 3);
	hsmCmdLen += 3;

	// ������Կ
	memset(szMkBuff, 0, sizeof(szMkBuff));
	ret = UnionGenerateX917RacalKeyString(strlen(pk), pk, szMkBuff);
	memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
	hsmCmdLen += ret;

	// ������Կ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
	hsmCmdLen += 1;

	// ������Կ��ɢ�㷨�ͷ�ɢ����
	if (pkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*strlen(pkDvsData));
		hsmCmdLen += pkDvsNum*strlen(pkDvsData);
	}

	// Ӧ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// Ӧ������Կ
	memset(szMkBuff, 0, sizeof(szMkBuff));
	ret = UnionGenerateX917RacalKeyString(strlen(mk), mk, szMkBuff);
	memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
	hsmCmdLen += ret;
	
	// Ӧ������Կ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// Ӧ������Կ��ɢ�㷨�ͷ�ɢ����
	if (mkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*strlen(mkDvsData));
		hsmCmdLen += mkDvsNum*strlen(mkDvsData);
	}

	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUC:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	// ����У��ֵ
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

	// �������ݳ���
	offset += 2;
	*criperDataLen = ret - offset;

	// ��������
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

	// �������
	memcpy(hsmCmdBuf,"QA",2);
	hsmCmdLen += 2;

	// ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, pkType, 3);
	hsmCmdLen += 3;

	// ������Կ
	if (strlen(pk) == 32)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, pk, strlen(pk));
		hsmCmdLen += strlen(pk);
	}

	// ������Կ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
	hsmCmdLen += 1;

	// ������Կ��ɢ�㷨�ͷ�ɢ����
	if (pkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
		hsmCmdLen += pkDvsNum*lenOfDvsData;
	}

	// Ӧ������Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkType, 3);
	hsmCmdLen += 3;

	// Ӧ������Կ
	if (strlen(mk) == 32)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "S", 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, mk, strlen(mk));
		hsmCmdLen += strlen(mk);
	}

	// Ӧ������Կ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// Ӧ������Կ��ɢ�㷨�ͷ�ɢ����
	if (mkDvsNum != 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsFlag, 1);
		hsmCmdLen += 1;
		memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
		hsmCmdLen += mkDvsNum*lenOfDvsData;
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdQA:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	offset = 4;
	// ����У��ֵ
	memcpy(checkValue, hsmCmdBuf + ret - 32, 32);
	checkValue[32] = 0;
	offset += 32;

	// �������ݳ���
	offset += 4;
	*criperDataLen = ret - offset;

	// ��������
	memcpy(criperData, hsmCmdBuf + 8, *criperDataLen);
	criperData[*criperDataLen] = 0;
	
	return(0);
}

// ����һ��SM2��Կ
/* �������
   	type������
	1��ǩ����2�����ܣ�3��ǩ���ͼ���
   	length������ �̶�256
   	index,	��Կ����
   	keyPasswd,	��Կ����
   	sizeOfPK	���չ�Կ�Ļ����С
   	sizeOfVK	����˽Կ�Ļ����С
   	�������
	pk		��Կ
   	lenOfVK		˽Կ���ĳ���
   	vk		˽Կ
   	vkByLmk		��˽Կ����
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
	
	// ��Կ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",length);
	hsmCmdLen += 4;
	
	// ��Կ��;
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	
	// ��Կ����
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

// ����ǩ����ָ��
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
		UnionUserErrLog("in UnionSJL05CmdUQ:: UnionDirectHsmCmd!\n");
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

/*
�������ܣ�
	EYָ��ù�Կ��֤ǩ��
���������
	macOfPK����Կ��MACֵ
	signatureLength������֤��ǩ���ĳ���
	signature������֤��ǩ��
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	publicKeyLength����Կ�ĳ���
	publicKey����Կ
���������
	��
����ֵ��
        <0������ִ��ʧ��
        0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
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
	// ����֤��ǩ������
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signatureLength);
	hsmCmdLen += 4;
	// ����֤��ǩ��
	memcpy(hsmCmdBuf+hsmCmdLen,signature,signatureLength);
	hsmCmdLen += signatureLength;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// ��ǩ�����ݵĳ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signDataLength);
	hsmCmdLen += 4;
	// ��ǩ��������
	memcpy(hsmCmdBuf+hsmCmdLen,signData,signDataLength);
	hsmCmdLen += signDataLength;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// ��Կ��MACֵ
	memcpy(hsmCmdBuf+hsmCmdLen,macOfPK,4);
	hsmCmdLen += 4;
	// ��Կ
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

//������ǩ��ָ��
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
		// ��Կ����
		memcpy(hsmCmdBuf + offset,"99",2);
		offset += 2;
		// �����Կ
		aschex_to_bcdhex(rCaPK,lengRCaPK,hsmCmdBuf + offset);
		offset += lengRCaPK/2;
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
	offset += 4;

	// ����
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

// �����㷨���������Կ
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
		
	// ������
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
	
	// ��LMK���ܵ�Key
	lenOfValue = 32;
	keyOffset = 5;
	
	memcpy(value, hsmCmdBuf + keyOffset, lenOfValue);
	value[lenOfValue] = 0;
	memcpy(checkvalue, hsmCmdBuf + keyOffset + lenOfValue, 16);
	checkvalue[16] = 0;
	
	return lenOfValue;
}

// ������ԿУ��ֵ
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

	// ������
	memcpy(hsmCmdBuf,"BS",2);
	hsmCmdLen = 2;
	// ������Կ��LMK��ָʾ��������Կ���ʹ�������2λ
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
			UnionUserErrLog("in UnionSJL05CmdBS:: unknown key length!\n");
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
			UnionUserErrLog("in UnionSJL05CmdBS:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
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
		UnionUserErrLog("in UnionSJL05CmdBS:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	
	memcpy(checkValue,hsmCmdBuf+4,16);
	checkValue[16] = 0;
	return(16);
}

//SM2˽Կת����
/*�������
  algoType	�㷨��ʶ
  vkByLmk		˽Կ���ģ�LMK(36,37)����
  keyType		��Կ����
  key		����Կ�����µĴ���(SM4)��Կ
  encMode	�����㷨ģʽ
  iv		CBCģʽʱ����  DES/3DES ��8�ֽ�   SM4��16�ֽ�
  
  �������
  vk		�����µ�SM2˽Կ����
 */
int UnionSJL05CmdUY(char algoType, unsigned char *vkByLmk, TUnionDesKeyType keyType,char *key,char encMode,char *iv,char *vk)
{
	int	ret;
	char	hsmCmdBuf[1024+1];
	char	szMkBuff[128+1];
	int	hsmCmdLen = 0;
	
	memcpy(hsmCmdBuf,"UY",2);
	hsmCmdLen += 2;
	
	//��Կ�㷨��ʶ
	hsmCmdBuf[hsmCmdLen] = algoType;
	hsmCmdLen++;

	//˽Կ����
	memcpy(hsmCmdBuf+hsmCmdLen,vkByLmk,32);
	hsmCmdLen += 32;

	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUY:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += 3;

	//LMK���ܵĴ��䣨SM4����Կ
	memset(szMkBuff, 0, sizeof(szMkBuff));
        ret = UnionGenerateX917RacalKeyString(strlen(key), key, szMkBuff);
        memcpy(hsmCmdBuf+hsmCmdLen, szMkBuff, ret);
        hsmCmdLen += ret;

	//�����㷨��ģʽ
	hsmCmdBuf[hsmCmdLen] = encMode;
	hsmCmdLen++;

	if (encMode == '1')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,iv,strlen(iv));
		hsmCmdLen += strlen(iv);
	}
	hsmCmdBuf[hsmCmdLen] = 0;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJL05CmdUY:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	
	memcpy(vk,hsmCmdBuf+4,64);
	vk[64] = 0;
	return(64);
}



// �����ӡ��ʽ
/*
���������
	format 	��ӡ��ʽ���Ʒ���������಻����299

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



//����SM4��Կ����ӡһ���ɷ�
/* �������
   keyType              ��Կ����
   fldNumi              ��ӡ�����
   fld          ��ӡ�� 
   �������
   component    �ɷ�
 */
int UnionSJL05CmdWM(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component)
{
	int     ret;
	char    hsmCmdBuf[1024+1];              
	int     hsmCmdLen = 0;  
	int     i;
                
	memcpy(hsmCmdBuf,"WM",2);
	hsmCmdLen = 2;
	//��Կ����
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{       
		UnionUserErrLog("in UnionSJL05CmdWM:: UnionTranslateHsmKeyTypeString!\n");
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
		UnionUserErrLog("in UnionSJL05CmdWM:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	//��ȡ�ɷ�
	memcpy(component,hsmCmdBuf+4+1,32);
	component[32] = 0;
	return(32);
}

