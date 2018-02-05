// Author:	Wolfgang Wang
// Date:	2006/3/2

#define _RacalCmdForNewRacal_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sjl06.h"
#include "sjl06Protocol.h"
#include "racalRsaCmd.h"
#include "UnionStr.h"
#include "unionDesKey.h"
#include "unionSJL06API.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "3DesRacalSyntaxRules.h"

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
   	errCode		�����������
*/
int RacalCmdEI(int hsmSckHDL,PUnionSJL06 pSJL06,char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	int		lenOfPK;
	int		lenOfPKStr;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
			UnionUserErrLog("in RacalCmdEI:: lenOfPKExponent [%d] too long !\n",lenOfPKExponent);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+hsmCmdLen,pkExponent,lenOfPKExponent);
		hsmCmdLen += lenOfPKExponent;
	}
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEI:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EJ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdEI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	if ((lenOfPKStr = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)hsmCmdBuf+4,ret-4,pk,&lenOfPK,sizeOfPK)) < 0)
	{
		UnionUserErrLog("in RacalCmdEI:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
		UnionMemLog("in RacalCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(lenOfPKStr);
	}
	if (lenOfPKStr + 4 > ret - 4)
	{
		UnionUserErrLog("in RacalCmdEI:: lenOfPKStr + 4 = [%d] longer than [%d]!\n",lenOfPKStr+4,ret-4);
		UnionMemLog("in RacalCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	if (!exportNullPK)
	{
		if (lenOfPKStr > sizeOfPK)
		{
			UnionUserErrLog("in RacalCmdEI:: sizeOfPK [%04d] < expected [%04d]!\n",sizeOfPK,lenOfPKStr);
			return(errCodeSmallBuffer);
		}
		memcpy(pk,hsmCmdBuf+4,lenOfPKStr);
		lenOfPK = lenOfPKStr;
	}		
	
	*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+lenOfPKStr+4,4);
	if ((*lenOfVK <= 0) || (*lenOfVK + 4 + 4 + lenOfPKStr > ret) || (*lenOfVK > sizeOfVK))
	{
		UnionUserErrLog("in RacalCmdEI:: lenOfVK [%d] error!\n",*lenOfVK);
		UnionMemLog("in RacalCmdEI:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	memcpy(vk,hsmCmdBuf+4+lenOfPKStr+4,*lenOfVK);			
	return(lenOfPK);
}

// �洢˽Կ
/* �������
	vkIndex		˽Կ��������
	lenOfVK		˽Կ����
	vk		˽Կ
   �������
   	errCode		�����������
*/
int RacalCmdEK(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,int lenOfVK,unsigned char *vk,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		offset = 0;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EK",2);
	offset = 2;
	sprintf(hsmCmdBuf+offset,"%02d",vkIndex%100);
	offset += 2;
	sprintf(hsmCmdBuf+offset,"%04d",lenOfVK);
	offset += 4;
	if (offset + lenOfVK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in RacalCmdEK:: lenOfVK = [%04d] too long\n",lenOfVK);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+offset,vk,lenOfVK);
	offset += lenOfVK;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEK:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EL",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

// ���ɹ�Կ��MACֵ
int RacalCmdEO(int hsmSckHDL,PUnionSJL06 pSJL06,char *pkEncoding,int isNullPK,int lenOfPK,char *pk,char *authData,char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192+1];
	int		hsmCmdLen = 0;
	int		lenOfPKStr;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
			UnionUserErrLog("in RacalCmdEO:: UnionFormANSIDERRSAPK for [%s]\n",pk);
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
	UnionProgramerMemLog("in RacalCmdEO:: req =",(unsigned char *)hsmCmdBuf,hsmCmdLen);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEO:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdEO:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"EP",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEO:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdEO:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	//bcdhex_to_aschex(hsmCmdBuf+4,4,mac);
	memcpy(mac,hsmCmdBuf+4,4);
	return(4);
}

// ��֤��Կ��MACֵ
int RacalCmdEQ(int hsmSckHDL,PUnionSJL06 pSJL06,char *pk,char *authData,char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[4096+1];
	int		hsmCmdLen = 0;
	int		lenOfPKStr;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EQ",2);
	hsmCmdLen = 2;
	aschex_to_bcdhex(mac,8,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 4;
	if ((lenOfPKStr = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEQ:: UnionFormANSIDERRSAPK for [%s]\n",pk);
		return(lenOfPKStr);
	}
	hsmCmdLen += lenOfPKStr;
	if ((authData != NULL) && (strlen(authData) != 0))
	{
		strcpy(hsmCmdBuf+hsmCmdLen,authData);
		hsmCmdLen += strlen(authData);
	}
	UnionProgramerMemLog("in RacalCmdEQ:: req =",(unsigned char *)hsmCmdBuf,hsmCmdLen);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEQ:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdEQ:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"ER",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEQ:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdEQ:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	return(0);
}

// ��һ��LMK���ܵ���Կת��Ϊ��Կ����
int RacalCmdGK(int hsmSckHDL,PUnionSJL06 pSJL06,char *encyFlag,char *padMode,char *lmkType,char *desKeyByLMK,
		char *checkValue,char *pkMac,char *pk,char *authData,
		char *initValue,char *desKeyByPK,int sizeOfBuf,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[4224+1];
	int		hsmCmdLen = 0;
	int		lenOfPKStr;
	int		desKeyLen;

	if ((encyFlag == NULL) || (padMode == NULL) || (lmkType == NULL) || (desKeyByLMK == NULL) ||
		(checkValue == NULL) || (pkMac == NULL) || (pk == NULL) || (initValue == NULL) || (desKeyByPK == NULL))
	{
		UnionUserErrLog("in RacalCmdGK:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
			UnionUserErrLog("in RacalCmdGK:: desKeyByLMK = [%s] error!\n",desKeyByLMK);
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
		UnionUserErrLog("in RacalCmdGK:: UnionFormANSIDERRSAPK for [%s]\n",pk);
		return(lenOfPKStr);
	}
	hsmCmdLen += lenOfPKStr;
	if ((authData != NULL) && (strlen(authData) != 0))
	{
		strcpy(hsmCmdBuf+hsmCmdLen,authData);
		hsmCmdLen += strlen(authData);
	}
	UnionProgramerMemLog("in RacalCmdGK:: req =",(unsigned char *)hsmCmdBuf,hsmCmdLen);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdGK:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"GL",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdGK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdGK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(initValue,hsmCmdBuf+4,16);
	if (((desKeyLen = UnionConvertIntoLen(hsmCmdBuf+4+16,4)) < 0) || (desKeyLen * 2 > sizeOfBuf))
	{
		UnionUserErrLog("in RacalCmdGK:: desKeyLen [%d] error!\n",desKeyLen);
		return(errCodeParameter);
	}
	UnionProgramerMemLog("in RacalCmdGK:: res =",(unsigned char *)hsmCmdBuf,ret);
	//bcdhex_to_aschex(hsmCmdBuf+4+16+4,desKeyLen,desKeyByPK);
	//return(desKeyLen*2);
	memcpy(desKeyByPK,hsmCmdBuf+4+16+4,desKeyLen);
	return(desKeyLen);
}
/* 
ʮһ��	����ժҪ

*/
int RacalCmdGM(int hsmSckHDL,PUnionSJL06 pSJL06,char *method,int lenOfData,char *data,
		char *hashResult,int sizeOfBuf,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192+1];
	int		hsmCmdLen = 0;
	int		retDataLen;
	
	if ((method == NULL) || (data == NULL) || (hashResult == NULL) || (lenOfData <= 0))
	{
		UnionUserErrLog("in RacalCmdGM:: null pointer or lenOfData Error [%d]\n",lenOfData);
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"GM",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+2,method,2);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%05d",lenOfData);
	hsmCmdLen += 5;
	if (lenOfData + hsmCmdLen >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in in RacalCmdGM:: lenOfData too long [%d]\n",lenOfData);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdGM:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"GN",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdGM:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdGM:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	if (((retDataLen = ret - 4) < 0) || (retDataLen * 2 >= sizeOfBuf))
	{
		UnionUserErrLog("in RacalCmdGM:: retDataLen [%d] error!\n",retDataLen);
		return(errCodeParameter);
	}
	memcpy(hashResult,hsmCmdBuf+4,retDataLen);
	return(retDataLen);
}

// ����ǩ����ָ��
int RacalCmdEW(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,int lenOfData,char *data,
		int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192*2+1];
	int		offset = 0;
	int		lenOfSign;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
		UnionUserErrLog("in RacalCmdEW:: lenOfData [%04d] too long!\n",lenOfData);
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
			UnionUserErrLog("in RacalCmdEW:: lenOfVK [%04d] error!\n",lenOfVK);
			return(errCodeParameter);
		}
		memcpy(hsmCmdBuf+offset,vk,lenOfVK);
		offset += lenOfVK;
	}
	UnionProgramerMemLog("in RacalCmdEW:: req =",(unsigned char *)hsmCmdBuf,offset);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEW:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdEW:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"EX",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEW:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdEW:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	offset = 4;
	if ((lenOfSign = UnionConvertIntStringToInt(hsmCmdBuf+offset,4)) < 0)
	{
		UnionUserErrLog("in RacalCmdEW:: lenOfSign = [%04d]!\n",lenOfSign);
		return(errSJL06Abnormal);
	}
	offset += 4;
	//if (lenOfSign >= sizeOfSign)
	if (lenOfSign * 2 >= sizeOfSign)
	{
		UnionUserErrLog("in RacalCmdEW:: lenOfSign = [%04d] too long!\n",lenOfSign);
		return(errCodeSmallBuffer);
	}
	//memcpy(sign,hsmCmdBuf+offset,lenOfSign);
	bcdhex_to_aschex(hsmCmdBuf+offset,lenOfSign,sign);
	//return(lenOfSign);
	return(lenOfSign*2);
}

// ��֤ǩ����ָ��
int RacalCmdEY(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData,char *errCode)
{
	int		ret;
	int		lenOfPKStr;
	char		hsmCmdBuf[8192*2+1];
	int		offset = 0;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
		UnionUserErrLog("in RacalCmdEY:: lenOfSign [%04d] too long!\n",lenOfSign);
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
		UnionUserErrLog("in RacalCmdEY:: lenOfData [%04d] too long!\n",lenOfData);
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
			UnionUserErrLog("in RacalCmdEY:: UnionFormANSIDERRSAPK for [%s]\n",PK);
			return(lenOfPKStr);
		}
		offset += lenOfPKStr;
	}
	else
	{
		if (lenOfPK + offset >= sizeof(hsmCmdBuf))
		{
			UnionUserErrLog("in RacalCmdEY:: lenOfPK [%04d] too long!\n",lenOfPK);
			return(errCodeSmallBuffer);
		}
		memcpy(hsmCmdBuf+offset,PK,lenOfPK);
		offset += lenOfPK;
	}
	// ��֤����
	if (lenOfAuthData + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in RacalCmdEY:: lenOfAuthData [%04d] too long!\n",lenOfAuthData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,authData,lenOfAuthData);
	offset += lenOfAuthData;

	UnionProgramerMemLog("in RacalCmdEY:: req =",(unsigned char *)hsmCmdBuf,offset);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEY:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdEY:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"EZ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEY:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdEY:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	return(0);
}


// Add By Huangbaoxin, 20090531
// ���ɹ�Կ��MACֵ
// ���30�����ܻ�����Կ���ǰ���׼ASN.1��ʽ�����DER��λ��
int RacalCmdEO_For30Hsm(int hsmSckHDL,PUnionSJL06 pSJL06,char *pkEncoding,int isNullPK,
	int lenOfPK,char *pk,char *authData,char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[2048+1], tmpPK[2048+1];
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EO",2);
	hsmCmdLen = 2;
	if ((pkEncoding == NULL) || (strlen(pkEncoding) == 0))
		memcpy(hsmCmdBuf+hsmCmdLen,"01",2);
	else
		memcpy(hsmCmdBuf+hsmCmdLen,pkEncoding,2);
	hsmCmdLen += 2;
	if (isNullPK)
	{
		/*
		if ((lenOfPKStr = UnionFormANSIDERRSAPK(pk,lenOfPK,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdEO:: UnionFormANSIDERRSAPK for [%s]\n",pk);
			return(lenOfPKStr);
		}
		hsmCmdLen += lenOfPKStr;
		*/
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memset(tmpPK, 0, sizeof(tmpPK));
		memcpy(tmpBuf, "30818902818100", 14);
		memcpy(tmpBuf+14, pk, lenOfPK);
		memcpy(tmpBuf+14+lenOfPK, "0203010001", 10);
		
		aschex_to_bcdhex(tmpBuf, lenOfPK+14+10, tmpPK);
		
		memcpy(hsmCmdBuf+hsmCmdLen,tmpPK,(lenOfPK+14+10)/2);
		hsmCmdLen += (lenOfPK+14+10)/2;
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
	UnionProgramerMemLog("in RacalCmdEO:: req =",(unsigned char *)hsmCmdBuf,hsmCmdLen);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEO:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdEO:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"EP",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEO:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdEO:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	//bcdhex_to_aschex(hsmCmdBuf+4,4,mac);
	memcpy(mac,hsmCmdBuf+4,4);
	return(4);
}
// End Add By Huangbaoxin, 20090531


// Add By Huangbaoxin, 20090531
// ��֤ǩ����ָ��
// ���30�����ܻ�����Կ���ǰ���׼ASN.1��ʽ�����DER��λ��
int RacalCmdEY_For30Hsm(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192*2];
	int		offset = 0;
	char		tmpBuf[2048+1], tmpPK[2048+1];

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
		UnionUserErrLog("in RacalCmdEY:: lenOfSign [%04d] too long!\n",lenOfSign);
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
		UnionUserErrLog("in RacalCmdEY:: lenOfData [%04d] too long!\n",lenOfData);
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
		/*
		if ((lenOfPKStr = UnionFormANSIDERRSAPK(PK,lenOfPK,hsmCmdBuf+offset,sizeof(hsmCmdBuf)-offset)) < 0)
		{
			UnionUserErrLog("in RacalCmdEY:: UnionFormANSIDERRSAPK for [%s]\n",PK);
			return(lenOfPKStr);
		}
		offset += lenOfPKStr;
		*/
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memset(tmpPK, 0, sizeof(tmpPK));
		memcpy(tmpBuf, "30818902818100", 14);
		memcpy(tmpBuf+14, PK, lenOfPK);
		
		memcpy(tmpBuf+14+lenOfPK, "0203010001", 10);
		
		aschex_to_bcdhex(tmpBuf, lenOfPK+14+10, tmpPK);
		
		memcpy(hsmCmdBuf+offset,tmpPK,(lenOfPK+14+10)/2);
		offset += (lenOfPK+14+10)/2;
	}
	else
	{
		if (lenOfPK + offset >= sizeof(hsmCmdBuf))
		{
			UnionUserErrLog("in RacalCmdEY:: lenOfPK [%04d] too long!\n",lenOfPK);
			return(errCodeSmallBuffer);
		}
		memcpy(hsmCmdBuf+offset,PK,lenOfPK);
		offset += lenOfPK;
	}
	// ��֤����
	if (lenOfAuthData + offset >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in RacalCmdEY:: lenOfAuthData [%04d] too long!\n",lenOfAuthData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+offset,authData,lenOfAuthData);
	offset += lenOfAuthData;

	UnionProgramerMemLog("in RacalCmdEY:: req =",(unsigned char *)hsmCmdBuf,offset);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEY:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdEY:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"EZ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEY:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdEY:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	return(0);
}

/*
	��һ����Կ���ܵ���Կת��ΪLMK����
*/
int RacalCmdGI(int hsmSckHDL,PUnionSJL06 pSJL06, char *encyFlag, char *padMode, 
	char *lmkType, int keyLength, int lenOfDesKeyByPK,
	char *desKeyByPK, int vkIndex, int lenOfVK, unsigned char *vk,
	char *initValue, char *desKeyByLMK, char *checkValue, char *errCode)
{
	int		ret=-1;
	int		offset;
	char		hsmCmdBuf[1024*8+1];
	int		hsmCmdLen = 0;
	char 		tmpBuf[2046+1];

	if ((encyFlag == NULL) || (padMode == NULL) || (lmkType == NULL) || (desKeyByPK == NULL) ||
		(initValue == NULL) || (desKeyByLMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in RacalCmdGI:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
		UnionUserErrLog("in RacalCmdGI:: lenOfDesKeyByPK [%04d] too long!\n",lenOfDesKeyByPK);
		return(errCodeSmallBuffer);
	}
	memset(tmpBuf, 0, sizeof(tmpBuf));
	aschex_to_bcdhex(desKeyByPK, lenOfDesKeyByPK, tmpBuf);
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
			UnionUserErrLog("in RacalCmdGI:: lenOfVK [%04d] error!\n",lenOfVK);
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
		UnionUserErrLog("in RacalCmdGI:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	
	// LMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdGI:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	
	hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen++;
	
	UnionMemLog("in RacalCmdGI:: req =",(unsigned char *)hsmCmdBuf,hsmCmdLen);
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdGI:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	offset = 0;

	if ((memcmp(hsmCmdBuf,"GJ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdGI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}

	offset += 2;
	memcpy(errCode, hsmCmdBuf + offset, 2);
	if (memcmp(errCode,"00",2) != 0)
        {
                UnionUserErrLog("in RacalCmdGI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
                return(errSJL06CmdResultError);
        }
	offset += 2;

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
		UnionUserErrLog("in RacalCmdGI:: des key length %d error!\n",keyLength);
		return errCodeKeyCacheMDL_InvalidKeyLength;
	}
	return 0;
}
// add by LiangHong 20091014
// End Add By Huangbaoxin, 20090531
