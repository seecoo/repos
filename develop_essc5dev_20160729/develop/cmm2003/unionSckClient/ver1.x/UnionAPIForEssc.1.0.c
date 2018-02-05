//	Author:		���ѹ�˾�з���
//	Copyright:	Union Tech. Guangzhou
//	Date:		2008/3

// 	�������ṩ�������ƽ̨�ĵײ�API����

#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "esscFldTagDef.h"
#include "unionDesKey.h"

#define conMaxSizeOfBuf	8192

// �Ͽ���ESSC������
void UnionDisconnectEsscSvr()
{
	UnionReleaseCommWithHsmSvrToEssc();
}


// ��һ��ESSC��������뵽����,���ش��뵽���е����ݵĳ���
int UnionPutFldIntoStr(int fldTag,char *value,int lenOfValue,char *buf,int sizeOfBuf)
{
	if ((buf == NULL) || (lenOfValue < 0) || (value == NULL))
		return(errCodeParameter);

	if ( fldTag < 0 || fldTag > 999 || lenOfValue > 9999)
		return(errCodeParameter);

	if (lenOfValue + 3 + 4 >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionPutFldIntoStr:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf,"%03d%04d",fldTag,lenOfValue);
	memcpy(buf+7,value,lenOfValue);
	return(7+lenOfValue);
}

// ��һ��ESSC�����ж�ȡһ���򣬷������ڰ���ռ�ĳ���
int UnionReadSpecFldFromStr(char *data,int len,int fldTag,char *value,int sizeOfValue)
{
	char	tmpBuf[10];
	int	fldLen;
	int	fldNum;
	int	offset;
	int	index;
	int	tmpFldTag;

	if ((data == NULL) || (len < 3) || (value == NULL))
		return(errCodeParameter);

	// ����Ŀ
	memcpy(tmpBuf,data,3);
	tmpBuf[3] = 0;	
	if ((fldNum = atoi(tmpBuf)) <= 0)
	{
		UnionUserErrLog("in UnionReadSpecFldFromStr:: fldNum = [%d]\n",fldNum);
		return(errCodeEsscMDL_FldNotExists);
	}
	offset = 3;
	for (index = 0; index < fldNum; index++)
	{
		if (offset + 7 > len)
		{
			UnionUserErrLog("in UnionReadSpecFldFromStr:: fldTag [%03d] not exists!\n",fldTag);
			return(errCodeEsscMDL_FldNotExists);
		}
		memcpy(tmpBuf,data+offset,3);
		offset += 3;
		tmpBuf[3] = 0;
		tmpFldTag = atoi(tmpBuf);
		memcpy(tmpBuf,data+offset,4);
		offset += 4;
		tmpBuf[4] = 0;	
		fldLen = atoi(tmpBuf);
		if (tmpFldTag == fldTag)	// �ҵ�
		{
			if (fldLen < 0)
			{
				UnionUserErrLog("in UnionReadSpecFldFromStr:: pfld->len = [%d] for fldIndex = [%03d] error!\n",fldLen,tmpFldTag);
				return(errCodeEsscMDL_EsscPackageFldLength);
			}
			if (fldLen >= sizeOfValue)
			{
				UnionUserErrLog("in UnionReadSpecFldFromStr:: sizeOfValue [%d] smaller than expected [%04d]\n",
					sizeOfValue,fldLen);
				return(errCodeSmallBuffer);
			}
			memcpy(value,data+offset,fldLen);
			return(fldLen);
		}
		offset += fldLen;
	}
	UnionUserErrLog("in UnionReadSpecFldFromStr:: fldTag [%03d] not exists!\n",fldTag);
	return(errCodeEsscMDL_FldNotExists);
}


// ================================= DP���� =================================
/*
����
	ʹ��U1ָ���������
���������
	key��			�������Կ��������Ϊ��Կ�������ַ�����ʽ���������Կ���ƣ�
				��Ϊ��Կ����ȫ����
	mode��			����ģʽ����U1ָ���Ӧ��
	plainTextLen��		�������ݳ��ȣ���չ������ݳ��ȣ�
	plainText��		��չ����������ݣ�
	sizeofCipherText��	�����������ݵĻ�������С��������չ����������ݣ�
�������
	cipherText��		��չ����������ݣ�
����ֵ
	>=0			�������ݵĳ���	
	<0			ʧ�ܣ����ش�����
*/
int UnionEncryptDataForDP(char *key,char *mode,int plainTextLen,char *plainText,char *cipherText,int sizeofCipherText)
{
	int	ret;
	char	packageBuf[conMaxSizeOfBuf+1];
	int	offset = 0;
	char	tmpBuf[100+1];
	int	fldNum = 0;

	if (key == NULL || plainText == NULL || plainTextLen <= 0 || cipherText == NULL)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: parameter is error!\n");
		return(errCodeAPIParameter);
	}

	memset(packageBuf,0,sizeof(packageBuf));
	// ����Ŀ
	sprintf(packageBuf+offset,"%03d",6);
	offset += 3;
	// ��
	// ����ģʽ��ʶ
	if ((ret = UnionPutFldIntoStr(conEsscFldAlgorithmMode,"0",1,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldAlgorithmMode!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// ����ID
	if ((ret = UnionPutFldIntoStr(conEsscFldKeyModuleID,mode,strlen(mode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldKeyModuleID!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// ��Կ���ƻ���Կ����
	if ((ret = UnionPutFldIntoStr(conEsscFldKeyName,key,strlen(key),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldKeyName!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	if (strstr(key,".") == NULL)
	{
		// ��Կ����conMKAC
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",conMKAC);
		if ((ret = UnionPutFldIntoStr(conEsscFldKeyTypeFlag,tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldKeyTypeFlag!\n");
			return(ret);
		}
		fldNum++;
		offset += ret;
	}
	// ��ɢ����0
	if ((ret = UnionPutFldIntoStr(conEsscDisperseTimes,"0",1,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscDisperseTimes!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// ��������
	if ((ret = UnionPutFldIntoStr(conEsscFldData,plainText,plainTextLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldData!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// ����Ŀ
	sprintf(packageBuf,"%03d%s",fldNum,packageBuf+3);
	
	if ((ret = UnionCommWithHsmSvrToEssc("538",packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionCommWithHsmSvrToEssc!\n");
		return(ret);
	}
	if ((ret = UnionReadSpecFldFromStr(packageBuf,ret,conEsscFldData,cipherText,sizeofCipherText)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionReadSpecFldFromStr [%03d]!\n",conEsscFldData);
		return(ret);
	}
	return (ret);
}

/*
����
	ʹ��Y1ָ��ת��������
���������
	srcKey��			Դ������Կ���������Կ��������Ϊ��Կ�������ַ�����ʽ���������Կ���ƣ���Ϊ��Կ����ȫ����
	srcMode��		Դ����ģʽ����Y1ָ���Ӧ��
	dstKey��			Ŀ�ļ�����Կ���������Կ��������Ϊ��Կ�������ַ�����ʽ���������Կ���ƣ���Ϊ��Կ����ȫ����
	dstMode��		Ŀ�ļ���ģʽ����Y1ָ���Ӧ��
	srcCipherTextLen��	Դ�������ݳ��ȣ���չ������ݳ��ȣ�
	srcCipherText��		��չ���Դ�������ݣ�
	sizeofdstCipherText��	����Ŀ���������ݵĻ���ȥ��С��������չ����������ݣ�
�������
	dstCipherText��		��չ���Ŀ���������ݣ�
����ֵ
	>=0			�������ݵĳ���	
	<0			ʧ�ܣ����ش�����
*/
int UnionTransEncryptDataForDP(char *srcKey,char *srcMode,char *dstKey,char *dstMode,int srcCipherTextLen,char *srcCipherText,char * dstCipherText,int sizeofdstCipherText)
{
	int	ret;
	char	packageBuf[conMaxSizeOfBuf+1];
	int	offset = 0;
	char	tmpBuf[100+1];
	char	iv[16+1];
	int	fldNum = 0;

	if (srcKey == NULL || srcMode == NULL || dstKey == NULL || dstMode == NULL || srcCipherTextLen <= 0 || srcCipherText == NULL || dstCipherText == NULL)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: parameter is error!\n");
		return(errCodeAPIParameter);
	}

	memset(packageBuf,0,sizeof(packageBuf));
	// ����Ŀ
	//sprintf(packageBuf+offset,"%03d",8);
	offset += 3;
	// ��
	// Դ��Կ���ƻ���Կ����
	if ((ret = UnionPutFldIntoStr(conEsscFldFirstWKName,srcKey,strlen(srcKey),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldFirstWKName!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	if (strstr(srcKey,".") == NULL)
	{
		// ��Կ����conZEK
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",conZEK);
		if ((ret = UnionPutFldIntoStr(conEsscFldKeyTypeFlag,tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldKeyTypeFlag!\n");
			return(ret);
		}
		fldNum++;
		offset += ret;
	}
	// Դ�����㷨
	if ((ret = UnionPutFldIntoStr(conEsscFldAlgorithm01Mode,srcMode,strlen(srcMode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldAlgorithm01Mode!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// Ŀ����Կ���ƻ���Կ����
	if ((ret = UnionPutFldIntoStr(conEsscFldSecondWKName,dstKey,strlen(dstKey),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldSecondWKName!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	if (strstr(dstKey,".") == NULL)
	{
		// ��Կ����conZEK
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",conZEK);
		if ((ret = UnionPutFldIntoStr(conEsscFldKeyLenFlag,tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldKeyLenFlag!\n");
			return(ret);
		}
		fldNum++;
		offset += ret;
	}
	// Ŀ�ļ����㷨
	if ((ret = UnionPutFldIntoStr(conEsscFldAlgorithm02Mode,dstMode,strlen(dstMode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldAlgorithm02Mode!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// ��������
	if ((ret = UnionPutFldIntoStr(conEsscFldData,srcCipherText,srcCipherTextLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldData!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// iv_cbc
	if ((strncmp(srcMode,"10",2) == 0) || (strncmp(srcMode,"11",2) == 0) || (strncmp(srcMode,"12",2) == 0) || (strncmp(srcMode,"20",2) == 0))
	{
		memset(iv,'0',sizeof(iv));
		if ((ret = UnionPutFldIntoStr(conEsscFldIV,iv,16,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldIV!\n");
			return(ret);
		}
		fldNum++;
		offset += ret;
	}
	//����Ŀ
	sprintf(packageBuf,"%03d%s",fldNum,packageBuf+3);
	
	if ((ret = UnionCommWithHsmSvrToEssc("539",packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionCommWithHsmSvrToEssc!\n");
		return(ret);
	}
	if ((ret = UnionReadSpecFldFromStr(packageBuf,ret,conEsscFldData,dstCipherText,sizeofdstCipherText)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionReadSpecFldFromStr [%03d]!\n",conEsscFldData);
		return(ret);
	}
	return (ret);
}

/*
��������	UnionVerifySignatureUsingInputPK
����	        ʹ������Ĺ�Կ��֤ǩ�������������UnionVerifyRsaSignature������
		���ڣ�PK���ɿͻ��˷��͹�����
�������	pkValue��������Կֵ
                flag��������䷽ʽ��0��������ݲ�����Կ���ȵ���������
                ���油0x00��1��PKCS��䷽ʽ��һ��ʹ��PKCS��䷽ʽ��
                hashID��HASH�㷨��־���������RACAL��׼ָ�������ָ��EY��
                01��SHA-1��02��MD5��03��ISO 1011802��04��NoHash��
                �������SJL06 38ָ���0����MD5����1����SHA-1��
                �������Ҫ��HASH��Ϊ��ֵ������NULL����
                lenOfData��ǩ�����ݵĳ���
                data��ǩ������
                sign������֤��ǩ��
                lenOfSign������֤ǩ���ĳ���
�������	��
����ֵ	        <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
                =0��ǩ����֤�ɹ�
��Կ	        1��pkValue����֤ǩ���Ĺ�Կ
�������	135
*/
int UnionVerifySignatureUsingInputBCDPK(int lenOfPK,char *pkValue,char *flag,char *hashID,int lenOfData,unsigned char *data,unsigned char *sign,int lenOfSign)
{
	int	ret;
	char	packageBuf[conMaxSizeOfBuf+1];
        char    tmpBuf[2048+1];
	int	offset = 0;
	
	if ((pkValue == NULL) || (data == NULL) || (sign == NULL) || (lenOfData <= 0) || (flag == NULL))
	{
		UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: parameter error!\n");
		return(errCodeAPIParameter);
	}

	memset(packageBuf,0,sizeof(packageBuf));
        memset(tmpBuf,0,sizeof(tmpBuf));
	// ����Ŀ
	if (hashID != NULL && strcmp(hashID,"N") != 0)
		sprintf(packageBuf+offset,"%03d",5);
	else
		sprintf(packageBuf+offset,"%03d",4);
	offset += 3;
	// ��
	if ((ret = UnionPutFldIntoStr(conEsscFldKeyValue,pkValue,lenOfPK,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: UnionPutFldIntoStr for conEsscFldKeyValue!\n");
		return(ret);
	}
	offset += ret;
	if ((ret = UnionPutFldIntoStr(conEsscFldSignData,(char *)data,lenOfData,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: UnionPutFldIntoStr for conEsscFldSignData!\n");
		return(ret);
	}
	offset += ret;
        //ѹ��ǩ��
        if ((lenOfSign/2) > sizeof(tmpBuf))
        {
                UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: sizeof(tmpBuf) is too small [%03d]!\n", sizeof(tmpBuf));
                return(errCodeSmallBuffer);
	}
	else
	{
		aschex_to_bcdhex((char *)sign,lenOfSign,tmpBuf);
	}
        lenOfSign /= 2;
	if ((ret = UnionPutFldIntoStr(conEsscFldSign,tmpBuf,lenOfSign,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: UnionPutFldIntoStr for conEsscFldSign!\n");
		return(ret);
	}
	offset += ret;
	if ((ret = UnionPutFldIntoStr(conEsscFldSignDataPadFlag,flag,strlen(flag),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: UnionPutFldIntoStr for conEsscFldSignDataPadFlag!\n");
		return(ret);
	}
	offset += ret;
	if (hashID != NULL && strcmp(hashID,"N") != 0)
	{
		if ((ret = UnionPutFldIntoStr(conEsscFldAlgorithmMode,hashID,strlen(hashID),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: UnionPutFldIntoStr for conEsscFldAlgorithmMode!\n");
			return(ret);
		}
		offset += ret;
	}
	
	if ((ret = UnionCommWithHsmSvrToEssc("135",packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
	{
		UnionUserErrLog("in UnionVerifySignatureUsingInputBCDPK:: UnionCommWithHsmSvrToEssc!\n");
		return(ret);
	}

	return(0);
}


