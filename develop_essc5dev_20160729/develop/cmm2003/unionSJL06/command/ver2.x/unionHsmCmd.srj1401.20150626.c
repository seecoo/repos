//	Author:		liwj
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionDesKey.h"
#include "unionSJJ1127Cmd.h"
#include "commWithHsmSvr.h"
#include "conHsmFldTagDef.h"
#include "base64.h"
#include "unionSRJ1401Cmd.h"

#define conMaxSizeOfBuf	8192
#define ECC_OFFSET	32
#define ECC_REAL_SIZE	32
#define FLD_LEN		8


static int MAX_FLD_VALUE(int n)
{
	char buf[20] = {0};

	memset(buf, '9', n);
	return atol(buf);
}


// ��һ��ESSC��������뵽����,���ش��뵽���е����ݵĳ���
static int UnionPutFldIntoStr(int fldTag, const char *value, int lenOfValue, char *buf, int sizeOfBuf)
{
	if ((buf == NULL) || (lenOfValue < 0) || (value == NULL))
	{
		UnionUserErrLog("in UnionPutFldIntoStr:: parameter [buf == NULL] or [lenOfValue[%d] < 0] or [value == NULL]!\n", lenOfValue);
		return(errCodeParameter);
	}

	if ( fldTag < 0 || fldTag > 999 || lenOfValue > MAX_FLD_VALUE(FLD_LEN))
	{
		UnionUserErrLog("in UnionPutFldIntoStr:: parameter [fldTag[%d]] or [lenOfValue[%d]]!\n", fldTag, lenOfValue);
		return(errCodeParameter);
	}

	if (lenOfValue + 3 + FLD_LEN >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionPutFldIntoStr:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf,"%03d%0*d", fldTag, FLD_LEN, lenOfValue); 
	memcpy(buf + 3 + FLD_LEN, value, lenOfValue);
	return(3 + FLD_LEN + lenOfValue);
}

// ��һ��ESSC������������뵽����,���ش��뵽���е����ݵĳ���
static int UnionPutIntFldIntoStr(int fldTag, int iValue, char *buf, int sizeOfBuf)
{
	char value[100];

	sprintf(value, "%d", iValue);
	return UnionPutFldIntoStr(fldTag, value, strlen(value), buf, sizeOfBuf);
}

// ��һ��ESSC�����ж�ȡһ���򣬷������ڰ���ռ�ĳ���
static int UnionReadSpecFldFromStr(const char *data,int len,int fldTag,char *value,int sizeOfValue)
{
	char	tmpBuf[10];
	int	fldLen;
	int	fldNum;
	int	offset;
	int	index;
	int	tmpFldTag;

	if ((data == NULL) || (len < 3) || (value == NULL))
	{
		UnionUserErrLog("in UnionReadSpecFldFromStr:: parameter!\n");
		return(errCodeParameter);
	}

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
		if (offset + 3 + FLD_LEN > len)
		{
			UnionUserErrLog("in UnionReadSpecFldFromStr:: fldTag [%03d] not exists!\n",fldTag);
			return(errCodeEsscMDL_FldNotExists);
		}
		memcpy(tmpBuf,data+offset,3);
		offset += 3;
		tmpBuf[3] = 0;
		tmpFldTag = atoi(tmpBuf);
		memcpy(tmpBuf,data+offset,FLD_LEN);
		offset += FLD_LEN;
		tmpBuf[FLD_LEN] = 0;	
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
				UnionUserErrLog("in UnionReadSpecFldFromStr:: sizeOfValue [%d] smaller than expected [%04d]\n", sizeOfValue,fldLen);
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

// ��һ��ESSC�����ж�ȡһ���򣬷������ڰ���ռ�ĳ���
int UnionReadSpecIntFldFromStr(const char *data,int len,int fldTag,int *iValue)
{
	char	value[20];
	int	ret = 0;

	memset(value,0,sizeof(value));
	if((ret = UnionReadSpecFldFromStr(data,len,fldTag,value,sizeof(value))) < 0)
		return ret;
	ret = atoi(value);
	if(iValue != NULL)
		*iValue = ret;
	return ret;
}


// �������Ƿ�����(100)
int UnionSRJ1401CmdCM100(char *testBuf, int lenOfTestBuf, char *resBuf, int lenOfResBuf)
{
	int     ret;
	int     hsmCmdLen = 0;
	char    hsmCmdBuf[1024*8+1];
	char	errCode[8];

	memcpy(hsmCmdBuf, testBuf, lenOfTestBuf);
	hsmCmdLen += lenOfTestBuf;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{       
		UnionUserErrLog("in UnionSRJ1401Cmd100:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;

	if (lenOfResBuf > ret)
		memcpy(resBuf, hsmCmdBuf, ret);
	else
		memcpy(resBuf, hsmCmdBuf, lenOfResBuf);

	if (memcmp(hsmCmdBuf + 4, "000000",6) != 0)
	{
		UnionUserErrLog("in UnionSRJ1401Cmd100:: response[%s]!\n", hsmCmdBuf);
		memcpy(errCode, hsmCmdBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	return(0);
}

/*
������ǩ(880)

����˵���� �����ݽ���ǩ��������ǩ�����
����˵����
����:
	pOrgData����ǩ������
	nOrgDataLen:��ǩ�����ݳ���
	appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪ  
	Ӧ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
	iAlgorithm��ժҪ�㷨1 ��ʾSHA1, 2 ��ʾSM3
	RSA֤��ֻ��ѡ��SHA1��SM2֤��ֻ��ѡ��SM3�㷨
���:
	pSignData: base64ǩ�����
	pSignDataLen:base64ǩ������
���ز���ֵ�� 
	0��ǩ���ɹ� ������ǩ��ʧ��
*/
int UnionSRJ1401CmdCM880(unsigned char* pOrgData, int nOrgDataLen, char* appName, int iAlgorithm, char* pSignData, int * pSignDataLen)
{
	int     ret;
        char    hsmCmdBuf[1024*8+1];
	char	tmpBuf[8192+1]={0};
        int     hsmCmdLen = 0;
	int	len = 0;
	int	offset = 0;
	char	errCode[8];

	// �������
	memcpy(hsmCmdBuf + hsmCmdLen, "CM880", 5);
	hsmCmdLen += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(hsmCmdBuf + hsmCmdLen, "1", 1);
	hsmCmdLen += 1;

	// 3λ����������
	memcpy(hsmCmdBuf + hsmCmdLen, "003", 3);
	hsmCmdLen += 3;

	//  ԭʼ����	81
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,hsmCmdBuf + hsmCmdLen,sizeof(hsmCmdBuf) - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM880:: UnionPutFldIntoStr fldTag[conEsscFldPlainData]!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//  ֤��Ӧ����	301
	if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),hsmCmdBuf + hsmCmdLen,sizeof(hsmCmdBuf) - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM880:: UnionPutFldIntoStr fldTag[conBankCode]!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	//  �㷨��ʶ	145
	if ((ret = UnionPutIntFldIntoStr(conHsmFldAlgID,iAlgorithm,hsmCmdBuf + hsmCmdLen,sizeof(hsmCmdBuf) - hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM880:: UnionPutFldIntoStr fldTag[conHsmFldAlgID]!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	UnionSetBCDPrintTypeForHSMCmd();
        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdCM880:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	hsmCmdBuf[ret] = 0;

	if (memcmp(hsmCmdBuf + 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdCM880:: response[%s]!\n", hsmCmdBuf);
		memcpy(errCode, hsmCmdBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	// ǩ�����	92
	if ((ret = UnionReadSpecFldFromStr(hsmCmdBuf + offset,len,conEsscFldSign,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM880:: UnionReadSpecFldFromStr [conEsscFldSign]!\n");
		return(ret);
	}
	*pSignDataLen = ret;
	if(pSignData)
		memcpy(pSignData,tmpBuf,ret);
	return(0);
}

//������ǩ��
int UnionRawVerifyEx(unsigned char* pOrgData, int nOrgDataLen,char *appName, char *sCertDN, unsigned char *pSignData, int nSignDataLen, int iAlgorithm, int simpleFlag,int bRetCert, CERTINFO *pRetCertInfo)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	int	isUseAppName = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM881", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyEx:: UnionPutFldIntoStr for conEsscFldPlainData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
		isUseAppName = 1;
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
		isUseAppName = 0;
	}
	offset += ret;
	num++;

	if ((ret = UnionPutFldIntoStr(conEsscFldSign,(char *)pSignData,nSignDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyEx:: UnionPutFldIntoStr for conEsscFldSign!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//conHsmFldFileBuffer
	if (isUseAppName)
	{
		if ((ret = UnionPutIntFldIntoStr(conHsmFldFileBuffer,bRetCert,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionPutFldIntoStr for conHsmFldFileBuffer!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}

	//ժҪ�㷨
	if ((ret = UnionPutIntFldIntoStr(conHsmFldAlgID,iAlgorithm,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyEx:: UnionPutFldIntoStr for conHsmFldAlgID!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if ((ret = UnionPutIntFldIntoStr(conFlag2,simpleFlag,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyEx:: UnionPutFldIntoStr for conFlag2!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	UnionSetBCDPrintTypeForHSMCmd();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionRawVerifyEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionRawVerifyEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if(bRetCert && pRetCertInfo != NULL)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOissuer,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOissuer);
			return(ret);
		}
		strncpy(pRetCertInfo->issuer,tmpBuf,sizeof(pRetCertInfo->issuer));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOserialNumber,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOserialNumber);
			return(ret);
		}
		strncpy(pRetCertInfo->serialNumber,tmpBuf,sizeof(pRetCertInfo->serialNumber));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsubject,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsubject);
			return(ret);
		}
		strncpy(pRetCertInfo->subject,tmpBuf,sizeof(pRetCertInfo->subject));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotBefore,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotBefore);
			return(ret);
		}
		strncpy(pRetCertInfo->notBefore,tmpBuf,sizeof(pRetCertInfo->notBefore));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotAfter,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotAfter);
			return(ret);
		}
		strncpy(pRetCertInfo->notAfter,tmpBuf,sizeof(pRetCertInfo->notAfter));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsignresult,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsignresult);
			return(ret);
		}
		memcpy(pRetCertInfo->signresult,tmpBuf,sizeof(pRetCertInfo->signresult));

		memset(pRetCertInfo->cert,0,sizeof(pRetCertInfo->cert));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOcert,(char *)pRetCertInfo->cert,sizeof(pRetCertInfo->cert))) < 0)
		{
			UnionUserErrLog("in UnionRawVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOcert);
			return(ret);
		}
		pRetCertInfo->certLen = ret;
	}

	return 0;
}

//��֤�������ǩ��
int UnionRawVerifyWithCertEx(unsigned char* pOrgData, int nOrgDataLen, char *sCert, int sCertLen, unsigned char *pSignData, int nSignDataLen, int iAlgorithm, int simpleFlag)
{
	int	ret = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM881", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyWithCertEx:: UnionPutFldIntoStr for conEsscFldPlainData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	// ֤������
	if ((ret = UnionPutFldIntoStr(conCertData,sCert,sCertLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyWithCertEx:: UnionPutFldIntoStr for conCertData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if ((ret = UnionPutFldIntoStr(conEsscFldSign,(char *)pSignData,nSignDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyWithCertEx:: UnionPutFldIntoStr for conEsscFldSign!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//ժҪ�㷨
	if ((ret = UnionPutIntFldIntoStr(conHsmFldAlgID,iAlgorithm,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyWithCertEx:: UnionPutFldIntoStr for conHsmFldAlgID!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if ((ret = UnionPutIntFldIntoStr(conFlag2,simpleFlag,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionRawVerifyWithCertEx:: UnionPutFldIntoStr for conFlag2!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	UnionSetBCDPrintTypeForHSMCmd();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionRawVerifyWithCertEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionRawVerifyWithCertEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	return 0;
}

//������ǩ��881
/*
   ����˵���� ��DNָ����֤�������ǩ��������֤���
   ����˵����  
����:
	pOrgData��ԭ������
	nOrgDataLen��ԭ�ĳ���
	appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
	pSignData��base64ǩ����Ϣ
	iAlgorithm��ժҪ�㷨1 ��ʾSHA1, 2 ��ʾSM3
	RSA֤��ֻ��ѡ��SHA1��SM2֤��ֻ��ѡ��SM3�㷨
	bRetCert: �Ƿ���Ҫ�����֤ǩ���Ĺ�Կ֤��(base64��ʽ)��
	0-���裬1-��Ҫ
���: 
	sCert����Կ֤��(base64��ʽ),��bRetCertΪ1ʱ������
	pCertLen: ��Կ֤�鳤�ȣ���bRetCertΪ1ʱ������
���ز���ֵ��
	0:��֤�����ȷ  ��������֤���ʧ��
 */
int UnionSRJ1401CmdCM881(unsigned char* pOrgData, int nOrgDataLen, char* appName, char *sCertDN,char* pSignData, int nSignDataLen, int iAlgorithm, int bRetCert, char *sCert, int *pCertLen)
{
	CERTINFO	tRetCertInfo;
	unsigned char	tBuf[8192] = {0};
	int		ret = 0;
	int		nCertLen;

	if (pOrgData == NULL || pSignData == NULL || (appName == NULL && sCertDN == NULL))
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM881:: parameter error!\n");
		return(errCodeParameter);
	}
	if (bRetCert && (sCert == NULL || pCertLen == NULL))
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM881:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(&tRetCertInfo, 0, sizeof(tRetCertInfo));
	//nSignDataLen = from64tobits((char *)tBuf,pSignData);
	if (appName != NULL)
	{
		ret = UnionRawVerifyEx(pOrgData,nOrgDataLen,appName,NULL,(unsigned char *)pSignData,nSignDataLen,iAlgorithm,0,bRetCert,&tRetCertInfo);
	}
	else
	{
		nCertLen = strlen(sCertDN);
		ret = UnionRawVerifyWithCertEx(pOrgData,nOrgDataLen,sCertDN,nCertLen,(unsigned char *)pSignData,nSignDataLen,iAlgorithm,0);
	}
	if(ret != 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM881:: UnionRawVerifyEx fail!\n");
		return ret;
	}
	if(bRetCert)
	{
		memset(tBuf,0,sizeof(tBuf));
		to64frombits((unsigned char *)tBuf,tRetCertInfo.cert,tRetCertInfo.certLen);
		*pCertLen = strlen((char *)tBuf);
		if(sCert) 
			strcpy(sCert,(char *)tBuf);
	}
	return 0;
}


//���ƴ���Կ֤���ǩ��
int UnionDetachedSignDer(unsigned char *pOrgData, int nOrgDataLen, char *appName, char *sCertDN, int iAlgorithm,unsigned char *pSignData, int *pSignDataLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM882", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionDetachedSignDer:: UnionPutFldIntoStr for conEsscFldPlainData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDetachedSignDer:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDetachedSignDer:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	//ժҪ�㷨
	if ((ret = UnionPutIntFldIntoStr(conHsmFldAlgID,iAlgorithm,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionDetachedSignDer:: UnionPutFldIntoStr for conHsmFldAlgID!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionDetachedSignDer:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionDetachedSignDer:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conEsscFldSign,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDetachedSignDer:: UnionReadSpecFldFromStr [%03d]!\n",conEsscFldSign);
		return(ret);
	}
	*pSignDataLen = ret;
	if(pSignData)
		memcpy(pSignData,tmpBuf,ret);

	return 0;
}

//�������Կ֤���ǩ��
int UnionDetachedVerifyEx(unsigned char *pOrgData, int nOrgDataLen, unsigned char *pSignData, int nSignDataLen, int bRetCert, int simpleFlag, CERTINFO *pRetCertInfo)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM883", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionDetachedVerifyEx:: UnionPutFldIntoStr for conEsscFldPlainData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if ((ret = UnionPutFldIntoStr(conEsscFldSign,(char *)pSignData,nSignDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionDetachedVerifyEx:: UnionPutFldIntoStr for conEsscFldSign!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//conHsmFldFileBuffer
	sprintf(tmpBuf,"%d",bRetCert);
	if ((ret = UnionPutFldIntoStr(conHsmFldFileBuffer,(char *)tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionDetachedVerifyEx:: UnionPutFldIntoStr for conHsmFldFileBuffer!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(tmpBuf,"%d",simpleFlag);
	if ((ret = UnionPutFldIntoStr(conFlag2,(char *)tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionDetachedVerifyEx:: UnionPutFldIntoStr for conFlag!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionDetachedVerifyEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionDetachedVerifyEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if(bRetCert && pRetCertInfo != NULL)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOissuer,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDetachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOissuer);
			return(ret);
		}
		strncpy(pRetCertInfo->issuer,tmpBuf,sizeof(pRetCertInfo->issuer));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOserialNumber,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDetachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOserialNumber);
			return(ret);
		}
		strncpy(pRetCertInfo->serialNumber,tmpBuf,sizeof(pRetCertInfo->serialNumber));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsubject,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDetachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsubject);
			return(ret);
		}
		strncpy(pRetCertInfo->subject,tmpBuf,sizeof(pRetCertInfo->subject));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotBefore,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDetachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotBefore);
			return(ret);
		}
		strncpy(pRetCertInfo->notBefore,tmpBuf,sizeof(pRetCertInfo->notBefore));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotAfter,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDetachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotAfter);
			return(ret);
		}
		strncpy(pRetCertInfo->notAfter,tmpBuf,sizeof(pRetCertInfo->notAfter));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsignresult,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDetachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsignresult);
			return(ret);
		}
		memcpy(pRetCertInfo->signresult,tmpBuf,sizeof(pRetCertInfo->signresult));

		memset(pRetCertInfo->cert,0,sizeof(pRetCertInfo->cert));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOcert,(char *)pRetCertInfo->cert,sizeof(pRetCertInfo->cert))) < 0)
		{
			UnionUserErrLog("in UnionDetachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOcert);
			return(ret);
		}
		pRetCertInfo->certLen = ret;
	}

	return 0;
}

//�������Կ֤���ǩ��
int UnionDetachedVerify(unsigned char *pOrgData, int nOrgDataLen, char *pSignData,int nSignDataLen, int bRetCert,char *sCert, int *pCertLen)
{
	CERTINFO	tRetCertInfo;
	char		tBuf[8192] = {0};
	int		ret = 0;
	//int		nSignDataLen = 0;

	memset(&tRetCertInfo, 0, sizeof(tRetCertInfo));
	//nSignDataLen = from64tobits(tBuf,pSignData);
	ret = UnionDetachedVerifyEx(pOrgData,nOrgDataLen,(unsigned char *)pSignData,nSignDataLen,bRetCert,0,&tRetCertInfo);
	if(ret != 0)
	{
		UnionUserErrLog("in UnionDetachedVerify::UnionDetachedVerifyEx failed!\n");
		return ret;
	}
	if(bRetCert)
	{
		to64frombits((unsigned char *)tBuf,tRetCertInfo.cert,tRetCertInfo.certLen);
		*pCertLen = strlen(tBuf);
		if(sCert) 	
			strcpy(sCert,tBuf);
	}
	return 0;
}

//�������Կ֤���ǩ��
/*
����˵���� ��pkcs7 detachedǩ��������֤��������ǩ���
����˵����  
����:
	pOrgData����ǩ������ԭ��
	nOrgDataLen: ����ԭ�ĳ���
	pSignData��base64��ʽPKCS7����
    bRetCert: �Ƿ���Ҫ�����֤ǩ���Ĺ�Կ֤��(base64��ʽ)��
              0-���裬1-��Ҫ
���: 
    sCert����Կ֤��(base64��ʽ),��bRetCertΪ1ʱ������
    pCertLen: ��Կ֤�鳤�ȣ���bRetCertΪ1ʱ������
���ز���ֵ��0:��ǩ��ȷ  ��������ǩʧ��
*/
int UnionSRJ1401CmdCM883(unsigned char *pOrgData, int nOrgDataLen, char *pSignData,int nSignDataLen, int bRetCert,char *sCert, int *pCertLen)
{
	return UnionDetachedVerify(pOrgData, nOrgDataLen, pSignData, nSignDataLen, bRetCert, sCert, pCertLen);
}

//���ƴ���Կ֤���Attched P7ǩ��
int UnionAttachedSignDer(unsigned char* pOrgData, int nOrgDataLen, char *appName, char* sCertDN, int iAlgorithm,unsigned char *pSignData, int* pSignDataLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM894", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionAttachedSignDer:: UnionPutFldIntoStr for conEsscFldPlainData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionAttachedSignDer:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionAttachedSignDer:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	//ժҪ�㷨
	if ((ret = UnionPutIntFldIntoStr(conHsmFldAlgID,iAlgorithm,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionAttachedSignDer:: UnionPutFldIntoStr for conHsmFldAlgID!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	UnionSetBCDPrintTypeForHSMCmd();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionAttachedSignDer:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionAttachedSignDer:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conEsscFldSign,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionAttachedSignDer:: UnionReadSpecFldFromStr [%03d]!\n",conEsscFldSign);
		return(ret);
	}
	*pSignDataLen = ret;
	if(pSignData)
		memcpy(pSignData,tmpBuf,ret);

	return 0;
}

//���ƴ���Կ֤���Attched P7ǩ��
// 3.2.5  PKCS7 Attachedǩ��(894)
/*
����˵���� �����ݽ���pkcs7 attched��ʽǩ��������ǩ�����
����˵���� 
����:
	pOrgData����ǩ������
	nOrgDataLen����ǩ�����ݳ���
	appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
	iAlgorithm��ժҪ�㷨1 ��ʾSHA1, 2 ��ʾSM3
	RSA֤��ֻ��ѡ��SHA1��SM2֤��ֻ��ѡ��SM3�㷨
���:
	pSignData: base64��ʽP7ǩ�����ݰ�
	pSignDataLen: ǩ������
���ز���ֵ��
	0:ǩ���ɹ�   ����: ǩ��ʧ��
*/
int UnionSRJ1401CmdCM894(unsigned char* pOrgData, int nOrgDataLen, char* appName, int iAlgorithm,unsigned char *pSignData, int* pSignDataLen)
{
	int		ret = 0;
	unsigned char	buf[8192] = {0};
	//char		tbuf[8192*2] = {0};
	int		len = 0;

	ret = UnionAttachedSignDer(pOrgData,nOrgDataLen,appName,NULL,iAlgorithm,buf,&len);
	if(ret != 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM894:: UnionAttachedSignDer failed [%d]!\n",ret);
		return(ret);
	}
	//to64frombits((unsigned char *)tbuf,buf,len);
	//*pSignDataLen = strlen(tbuf);
	*pSignDataLen = len;
	if(pSignData) 
	{
		//strcpy((char *)pSignData,tbuf);
		memcpy(pSignData, buf, len);
	}
	return 0;
}

//���ƴ���Կ֤���ǩ��
//3.2.3  PKCS7 Detachedǩ��(882)
/*
����˵���� �����ݽ���pkcs7 detached��ʽǩ��������ǩ�����
����˵���� 
����:
	pOraData����ǩ������
	nOraDataLen����ǩ�����ݳ���
	appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
	iAlgorithm��ժҪ�㷨1 ��ʾSHA1, 2 ��ʾSM3
	RSA֤��ֻ��ѡ��SHA1��SM2֤��ֻ��ѡ��SM3�㷨
���:
	pSignData: base64��ʽP7ǩ�����ݰ�
	pSignDataLen: ǩ������
���ز���ֵ��
	0:ǩ���ɹ�   ����: ǩ��ʧ��
*/
int UnionSRJ1401CmdCM882(unsigned char *pOrgData, int nOrgDataLen, char *appName, int iAlgorithm, char *pSignData, int *pSignDataLen)
{
	unsigned char	buf[8192] = {0};
	//unsigned char	tbuf[8192] = {0};
	int		len = 0;
	int		ret = 0;

	ret = UnionDetachedSignDer(pOrgData, nOrgDataLen,appName,NULL, iAlgorithm,buf, &len);
	if(ret != 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM882:: UnionDetachedSignDer failed [%d]!\n",ret);
		return(ret);
	}
	//to64frombits(tbuf,buf,len);
	//*pSignDataLen = strlen((char *)tbuf);
	*pSignDataLen = len;
	if(pSignData) 
		memcpy(pSignData, buf, len);
	return 0;
}

//��֤Attched P7ǩ��
int UnionAttachedVerifyEx(unsigned char* pSignData, int nSignDataLen,int bRetCert,int simpleFlag, CERTINFO *pRetCertInfo)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM895", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if ((ret = UnionPutFldIntoStr(conEsscFldSign,(char *)pSignData,nSignDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionAttachedVerifyEx:: UnionPutFldIntoStr for conEsscFldSign!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//conHsmFldFileBuffer
	sprintf(tmpBuf,"%d",bRetCert);
	if ((ret = UnionPutFldIntoStr(conHsmFldFileBuffer,(char *)tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionAttachedVerifyEx:: UnionPutFldIntoStr for conHsmFldFileBuffer!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(tmpBuf,"%d",simpleFlag);
	if ((ret = UnionPutFldIntoStr(conFlag2,(char *)tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionAttachedVerifyEx:: UnionPutFldIntoStr for conFlag!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	UnionSetBCDPrintTypeForHSMCmd();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionAttachedVerifyEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionAttachedVerifyEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if(bRetCert && pRetCertInfo != NULL)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOissuer,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionAttachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOissuer);
			return(ret);
		}
		strncpy(pRetCertInfo->issuer,tmpBuf,sizeof(pRetCertInfo->issuer));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOserialNumber,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionAttachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOserialNumber);
			return(ret);
		}
		strncpy(pRetCertInfo->serialNumber,tmpBuf,sizeof(pRetCertInfo->serialNumber));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsubject,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionAttachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsubject);
			return(ret);
		}
		strncpy(pRetCertInfo->subject,tmpBuf,sizeof(pRetCertInfo->subject));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotBefore,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionAttachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotBefore);
			return(ret);
		}
		strncpy(pRetCertInfo->notBefore,tmpBuf,sizeof(pRetCertInfo->notBefore));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotAfter,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionAttachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotAfter);
			return(ret);
		}
		strncpy(pRetCertInfo->notAfter,tmpBuf,sizeof(pRetCertInfo->notAfter));

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsignresult,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionAttachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsignresult);
			return(ret);
		}
		memcpy(pRetCertInfo->signresult,tmpBuf,sizeof(pRetCertInfo->signresult));

		memset(pRetCertInfo->cert,0,sizeof(pRetCertInfo->cert));
		if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOcert,(char *)pRetCertInfo->cert,sizeof(pRetCertInfo->cert))) < 0)
		{
			UnionUserErrLog("in UnionAttachedVerifyEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOcert);
			return(ret);
		}
		pRetCertInfo->certLen = ret;
	}

	return 0;
}

//��֤Attched P7ǩ��
int UnionAttachedVerify(char* pSignData,int nSignDataLen, int bRetCert,char *sCert, int *pCertLen)
{
	CERTINFO	tRetCertInfo;
	char		tBuf[8192] = {0};
	int		ret = 0;
	//int		nSignDataLen = 0;

	memset(&tRetCertInfo, 0, sizeof(tRetCertInfo));
	//nSignDataLen = from64tobits(tBuf,pSignData);
	ret = UnionAttachedVerifyEx((unsigned char *)pSignData,nSignDataLen,bRetCert,0,&tRetCertInfo);
	if(ret != 0)
	{
		UnionUserErrLog("in UnionAttachedVerify::UnionAttachedVerifyEx failed!\n");
		return ret;
	}
	if(bRetCert)
	{
		to64frombits((unsigned char *)tBuf,tRetCertInfo.cert,tRetCertInfo.certLen);
		*pCertLen = strlen(tBuf);
		if(sCert) strcpy(sCert,tBuf);
	}
	return 0;
}

//��֤Attched P7ǩ��
/*
   ����˵���� ��pkcs7 attachedǩ��������֤��������ǩ���
   ����˵����  
����:
pSignData��base64��ʽPKCS7����
bRetCert: �Ƿ���Ҫ�����֤ǩ���Ĺ�Կ֤��(base64��ʽ)��
0-���裬1-��Ҫ
���: 
sCert����Կ֤��(base64��ʽ),��bRetCertΪ1ʱ������
pCertLen: ��Կ֤�鳤�ȣ���bRetCertΪ1ʱ������
���ز���ֵ��0:��ǩ��ȷ  ��������ǩʧ��
 */
int UnionSRJ1401CmdCM895(char* pSignData,int nSignDataLen,int bRetCert,char *sCert, int *pCertLen)
{
	return UnionAttachedVerify(pSignData,nSignDataLen, bRetCert,sCert,pCertLen);
}

//��֤�齫���ݷ�װ�������ŷ�
int UnionEnvelopedMessageWithCert(unsigned char* pOrgData,int nOrgDataLen, char* sCert,int nCertLen,unsigned char* pMessData, int* pMessDataLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM896", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// conCertData
	if ((ret = UnionPutFldIntoStr(conCertData,sCert,nCertLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEnvelopedMessageWithCert:: UnionPutFldIntoStr for conCertData");
		return(ret);
	}
	offset += ret;
	num++;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEnvelopedMessageWithCert:: UnionPutFldIntoStr for conEsscFldPlainData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	UnionSetBCDPrintTypeForHSMCmd();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionEnvelopedMessageWithCert:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionEnvelopedMessageWithCert:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conEsscFldCiperData,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionEnvelopedMessageWithCert:: UnionReadSpecFldFromStr [%03d]!\n",conEsscFldCiperData);
		return(ret);
	}
	*pMessDataLen = ret;
	if(pMessData)
		memcpy(pMessData,tmpBuf,ret);
	pMessData[ret] = '\0'; 

	return 0;
}

//��֤��Ӧ������DNָ����֤�齫���ݷ�װ��PKCS7��ʽ�������ŷ�
int UnionEnvelopedMessageEx(unsigned char* pOrgData,int nOrgDataLen,char *appName, char* sDN,unsigned char* pMessData, int* pMessDataLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM896", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionEnvelopedMessageEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sDN,strlen(sDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionEnvelopedMessageEx:: UnionPutFldIntoStr for conCertDN");
			return(ret);
		}
	}
	offset += ret;
	num++;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldPlainData,(char *)pOrgData,nOrgDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEnvelopedMessageEx:: UnionPutFldIntoStr for conEsscFldPlainData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
	UnionSetBCDPrintTypeForHSMCmd();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionEnvelopedMessageEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionEnvelopedMessageEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conEsscFldCiperData,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionEnvelopedMessageEx:: UnionReadSpecFldFromStr [%03d]!\n",conEsscFldCiperData);
		return(ret);
	}
	*pMessDataLen = ret;
	if(pMessData)
		memcpy(pMessData,tmpBuf,ret);
	pMessData[ret] = '\0'; 

	return 0;
}

//��DNָ����֤�齫���ݷ�װ��PKCS7��ʽ�������ŷ�
/*
����˵������DNָ����֤�齫���ݷ�װ��PKCS7��ʽ�������ŷ�
����˵���� 
����:
         pOrgData��������ԭ��
         nOrgDataLen������ԭ�ĳ���
         appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
���:
          pMessData:base64��ʽPKCS7�����ŷ��
                 pMessDataLen:�����ŷⳤ��
���ز���ֵ��0:���������ŷ�ɹ�  ������ʧ��
*/
int UnionSRJ1401CmdCM896(unsigned char* pOrgData,int nOrgDataLen,char *appName, char* sDN,unsigned char* pMessData, int* pMessDataLen)
{
	int nCertLen;

	if (appName != NULL)
	{
		return UnionEnvelopedMessageEx(pOrgData,nOrgDataLen,appName, NULL,pMessData, pMessDataLen);
	}

	if (sDN == NULL)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM896:: sDN parameter is NULL\n");
		return(errCodeParameter);
	}
	nCertLen = strlen(sDN);
	return UnionEnvelopedMessageWithCert(pOrgData,nOrgDataLen, sDN,nCertLen, pMessData, pMessDataLen);
}

//ȡPKCS7��ʽ�����ŷ��ԭ����
int UnionDeEnvelopedMessageEx(char *pMessData,char *appName,char *sDN,unsigned char* pOrgData,int *pOrgDataLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM897", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDeEnvelopedMessageEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sDN,strlen(sDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDeEnvelopedMessageEx:: UnionPutFldIntoStr for conCertDN");
			return(ret);
		}
	}
	offset += ret;
	num++;

	//ԭʼ����
	if ((ret = UnionPutFldIntoStr(conEsscFldCiperData,pMessData,strlen(pMessData),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionDeEnvelopedMessageEx:: UnionPutFldIntoStr for conEsscFldCiperData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionDeEnvelopedMessageEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionDeEnvelopedMessageEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conEsscFldPlainData,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDeEnvelopedMessageEx:: UnionReadSpecFldFromStr [%03d]!\n",conEsscFldPlainData);
		return(ret);
	}
	*pOrgDataLen = ret;
	if(pOrgData)
		memcpy(pOrgData,tmpBuf,ret);
	pOrgData[ret] = '\0'; 

	return 0;
}


//ȡPKCS7��ʽ�����ŷ��ԭ����
/*
   ����˵����ȡPKCS7��ʽ�����ŷ��ԭ����
   ����˵��:
����:
pMessData��base64��ʽPKCS7�����ŷ��
appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
���:
pOrgData��������ܺ�ԭ��
pOrgDataLen��������ܺ󳤶�
���ز���ֵ��0:���ܳɹ�   ����:ʧ��
 */
int UnionSRJ1401CmdCM897(char *pMessData,char *appName,unsigned char* pOrgData,int *pOrgDataLen)
{
	return UnionDeEnvelopedMessageEx(pMessData,appName,NULL,pOrgData,pOrgDataLen);
}

//�ϴ�֤��870
int  UnionUpLoadCertA(char *bankCode,char *bankName,unsigned char *pCertData, int nCertDataLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	ExistRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM870", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;
	
	if(bankCode && strlen(bankCode) > 0)
	{
		// ��������
		if ((ret = UnionPutFldIntoStr(conBankCode,(char *)bankCode,strlen(bankCode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionUpLoadCertA:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}

	if(bankName && strlen(bankName) > 0)
	{
		// ������
		if ((ret = UnionPutFldIntoStr(conBankName,(char *)bankName,strlen(bankName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionUpLoadCertA:: UnionPutFldIntoStr for conBankName!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}
	

	// ֤������
	if ((ret = UnionPutFldIntoStr(conCertData,(char *)pCertData,nCertDataLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionUpLoadCertA:: UnionPutFldIntoStr for conCertData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionUpLoadCertA:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionUpLoadCertA:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&ExistRet)) < 0)
	{
		UnionUserErrLog("in UnionUpLoadCertA:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}

	if(ExistRet == 1)
	{
		UnionLog("in UnionUpLoadCertA::cert already exist.\n");
		return 1;
	}

	return 0;
}

//�ϴ��ⲿ֤�鵽������
/*
����˵�����ϴ��ⲿ֤�鵽������. 
����˵����  
����:
    appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
		sCert:��Կ֤��(base64��ʽ) 
		nCertLen:֤�鳤��
       ���: ��
���ز���ֵ��0:�ɹ� 1:֤���Ѵ���   ������������
*/
int UnionSRJ1401CmdCM870(char *appName,unsigned char* sCert,int nCertLen)
{
	return UnionUpLoadCertA(appName,NULL,sCert,nCertLen);
}

//���֤���Ƿ����
//0 �޴�֤�飬1֤����ڣ���0��1ʧ��
int UnionIsCertExistEx(char *appName,char *sCertDN)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	ExistRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM877", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(appName != NULL)
		UnionLog("in UnionIsCertExistEx::appName=[%s].\n",appName);
	if(sCertDN != NULL)
		UnionLog("in UnionIsCertExistEx::sCertDN=[%s].\n",sCertDN);
	
	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionIsCertExistEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionIsCertExistEx:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionIsCertExistEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionIsCertExistEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&ExistRet)) < 0)
	{
		UnionUserErrLog("in UnionIsCertExistEx:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}

	if(ExistRet != 1)
	{
		UnionLog("in UnionIsCertExistEx::cert not exist.\n");
		return 0;
	}
	 
	return 1;
}

//���֤���Ƿ����
/*
����˵�������֤���Ƿ��ڷ�����
����˵����  
����:
	appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
���: ��
���ز���ֵ��0:������ 1:����   ������������
*/
int UnionSRJ1401CmdCM877(char *appName)
{
	return UnionIsCertExistEx(appName,NULL);
}

//����DN��ȡ��Կ֤����Ϣ873
//��Կ֤������ӦΪDER��ʽ
int UnionDownloadCertDerEx(char *appName,char *sCertDN, unsigned char *pCertData, int *pCertDataLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM873", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDownloadCertDerEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDownloadCertDerEx:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionDownloadCertDerEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionDownloadCertDerEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOcert,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDownloadCertDerEx:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOcert);
		return(ret);
	}
	if(pCertDataLen)
		*pCertDataLen = ret;

	if(pCertData)
		memcpy(pCertData,tmpBuf,ret);

	return 0;
}

//����DN��ȡ��Կ֤����Ϣ
/*
����˵������ȡ֤������DN��Ӧ��֤��
����˵����  
����:
      appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
���:
      pCert����Կ֤��(base64��ʽ)
      pCertLen: ��Կ֤�鳤��
���ز���ֵ��0:��ȡ֤��ɹ�   ������ʧ��
*/
int UnionSRJ1401CmdCM873(char *appName, unsigned char *pCertData, int *pCertDataLen)
{
	unsigned char	certDer[2048] = {0};
	int		len = 0;
	char		tBuf[8192] = {0};
	int		ret = 0;

	ret = UnionDownloadCertDerEx(appName,NULL,certDer,&len);
	if(ret != 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM873::UnionDownloadCertDerEx of appName[%s] failed!\n",appName);
		return ret;
	}
	to64frombits((unsigned char *)tBuf,certDer,len);
	*pCertDataLen = strlen(tBuf);
	if(pCertData) 
		strcpy((char *)pCertData,tBuf);
	return 0;
}

//����֤��
int UnionDecodeCertA(char *appName, char *sCert,int nCertLen, CERTINFO* pRetCertInfo)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM898", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	 
	if (appName != NULL && appName[0] != 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDecodeCertA:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionPutFldIntoStr(conCertData,sCert,nCertLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionDecodeCertA:: UnionPutFldIntoStr for conCertData!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionDecodeCertA:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionDecodeCertA:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOissuer,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOissuer);
		return(ret);
	}
	strncpy(pRetCertInfo->issuer,tmpBuf,sizeof(pRetCertInfo->issuer));
	 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOserialNumber,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOserialNumber);
		return(ret);
	}
	strncpy(pRetCertInfo->serialNumber,tmpBuf,sizeof(pRetCertInfo->serialNumber));

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsubject,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsubject);
		return(ret);
	}
	strncpy(pRetCertInfo->subject,tmpBuf,sizeof(pRetCertInfo->subject));

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotBefore,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotBefore);
		return(ret);
	}
	strncpy(pRetCertInfo->notBefore,tmpBuf,sizeof(pRetCertInfo->notBefore));

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOnotAfter,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOnotAfter);
		return(ret);
	}
	strncpy(pRetCertInfo->notAfter,tmpBuf,sizeof(pRetCertInfo->notAfter));

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conHsmFldAlgID2,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conHsmFldAlgID2);
		return(ret);
	}
	pRetCertInfo->algID = atoi(tmpBuf);
	UnionLog("in UnionDecodeCertA pRetCertInfo->algID=[%d].\n",pRetCertInfo->algID);
	 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conHsmFlducPublicKey,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conHsmFlducPublicKey);
		return(ret);
	}
	strncpy(pRetCertInfo->derPK,tmpBuf,sizeof(pRetCertInfo->derPK));

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOsignresult,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOsignresult);
		return(ret);
	}
	memcpy(pRetCertInfo->signresult,tmpBuf,sizeof(pRetCertInfo->signresult));

	memset(pRetCertInfo->cert,0,sizeof(pRetCertInfo->cert));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conCERTINFOcert,(char *)pRetCertInfo->cert,sizeof(pRetCertInfo->cert))) < 0)
	{
		UnionUserErrLog("in UnionDecodeCertA:: UnionReadSpecFldFromStr [%03d]!\n",conCERTINFOcert);
		return(ret);
	}
	pRetCertInfo->certLen = ret;
	
	return 0;
}

//����֤��
int UnionDecodeCert(char *appName, char *sCert,int nCertLen,int ntag, char* data, int* dataLen)
{
	CERTINFO	tCertInfo;
	int		ret = 0;

	memset(&tCertInfo, 0, sizeof(tCertInfo));
	if (appName != NULL && appName[0] != 0)
	{
		ret = UnionDecodeCertA(appName, NULL, nCertLen,&tCertInfo);
	}
	else if (sCert != NULL)
	{
		ret = UnionDecodeCertA(NULL, sCert,nCertLen,&tCertInfo);
	}
	else
	{
		UnionUserErrLog("in UnionDecodeCert:: parameter error!\n");
		return (errCodeParameter);
	}
	if(ret != 0)
	{
		UnionUserErrLog("in UnionDecodeCert::UnionDecodeCertA sCert[%s] failed!\n",sCert);
		return ret;
	}
	switch(ntag)
	{
		case certInfoSubjectDNTag:
			*dataLen = strlen(tCertInfo.subject);
			if(data) strcpy(data,tCertInfo.subject);
			break;
		case certInfoIssuerDNTag:
			*dataLen = strlen(tCertInfo.issuer);
			if(data) strcpy(data,tCertInfo.issuer);
			break;
		case certInfoSnTag:
			*dataLen = strlen(tCertInfo.serialNumber);
			if(data) strcpy(data,tCertInfo.serialNumber);
			break;
		case certInfoBeforeTimeTag:
			*dataLen = strlen(tCertInfo.notBefore);
			if(data) strcpy(data,tCertInfo.notBefore);
			break;
		case certInfoAfterTimeTag:
			*dataLen = strlen(tCertInfo.notAfter);
			if(data) strcpy(data,tCertInfo.notAfter);
			break;
		case certInfoAlgIDTag:
			if(tCertInfo.algID == 0)
			{
				*dataLen = strlen("RSA");
				if(data) strcpy(data,"RSA");
			}
			else if (tCertInfo.algID == 1)
			{
				*dataLen = strlen("SM2");
				if(data) strcpy(data,"SM2");
			}
			else {
				char buf[20] = {0};
				sprintf(buf,"%d",tCertInfo.algID);
				*dataLen = strlen(buf);
				if(data) strcpy(data,buf);
			}
			break;
		case certInfoPKTag:
			*dataLen = strlen(tCertInfo.derPK);
			if(data) strcpy(data,tCertInfo.derPK);
			break;
		default:
			return -1;
	}
	return 0;
}

//����֤��
/*
����˵������ȡ֤����Ϣ
����˵����  
����:
    sCert:base64����֤������
    nCertLen��֤�鳤��
    ntag�� 0--֤��Ψһ��ʾ��subject��(DN)
           1--�䷢��Ψһ��ʾ��issuer��(DN)
           2--֤�����к�SN��(H���ַ���)
           3--��Чʱ��BeforeTime(YYYYMMDDHHMMSS)
           4--����ʱ��AfterTime(YYYYMMDDHHMMSS)
           5---֤�鹫Կ�㷨��ʶ(���ء�RSA����SM2��)
           6---֤�鹫Կ����(RSA��Կ��ʽΪDER Hex��ʽ,SM2��Կ Ϊ��04||xHex||yHex��)
���:
     data��   ֤��ntag��Ӧ���������
     dataLen��������ݳ���
���ز���ֵ��0:��ȡ֤����Ϣ�ɹ�   ������ʧ��
*/
int UnionSRJ1401CmdCM898(char *appName, char *sCert,int nCertLen,int ntag, char* data, int* dataLen)
{
	int	ret;

	if ((ret = UnionDecodeCert(appName, sCert,nCertLen,ntag,data,dataLen)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM898:: UnionDecodeCert error!\n");
		return(ret);
	}
	return 0;
}

//���֤��Ϸ�����֤֤����
//���ָ��DN��֤���Ƿ���ǩ���������д��ڣ�֤����ڻ�ȡ֤�飻����֤���֤������֤���Ƿ�ΪCFCAǩ����
//0 �Ϸ�����0֤��Ƿ�
int UnionCheckCertChainAEx(char *appName,char* sCertDN,char *rootDN)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	VerifyRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM879", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;


	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionCheckCertChainAEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionCheckCertChainAEx:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	if(rootDN != NULL && strlen(rootDN) != 0)
	{
		// DN
		if ((ret = UnionPutFldIntoStr(conCERTINFOissuer,rootDN,strlen(rootDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionCheckCertChainAEx:: UnionPutFldIntoStr for conCERTINFOissuer!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionCheckCertChainAEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionCheckCertChainAEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&VerifyRet)) < 0)
	{
		UnionUserErrLog("in UnionCheckCertChainAEx:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}

	if(VerifyRet != 1)  //У��δͨ��
	{
		UnionLog("in UnionCheckCertChainAEx::failed.\n");
		return -1;
	}

	return 0;
}

//���֤��Ϸ�����֤֤����,��֤ǩ��֤���Ƿ��ɷ����������ε�CA�����䷢
int UnionCheckCertChainC(char *sCert,int nCertLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	VerifyRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM879", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// DN
	if ((ret = UnionPutFldIntoStr(conCertData,sCert,nCertLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionCheckCertChainC:: UnionPutFldIntoStr for conCertData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionCheckCertChainC:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionCheckCertChainC:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&VerifyRet)) < 0)
	{
		UnionUserErrLog("in UnionCheckCertChainC:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}

	if(VerifyRet != 1)  //У��δͨ��
	{
		UnionLog("in UnionCheckCertChainC::failed.\n");
		return -1;
	}

	return 0;
}

//���֤��Ϸ�����֤֤����,��֤ǩ��֤���Ƿ��ɷ����������ε�CA�����䷢
/*
����˵������֤ǩ��֤���Ƿ��ɷ����������ε�CA�����䷢
����˵����  
����:
	appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
���:
	��
���ز���ֵ��0���ɹ�  ������ʧ��
*/
int UnionSRJ1401CmdCM879(char* appName, char *sCert,int nCertLen)
{
	if (appName != NULL && appName[0] != 0)
	{
		return UnionCheckCertChainAEx(appName,NULL,NULL);
	}
	else if (sCert != NULL)
	{
		return UnionCheckCertChainC(sCert,nCertLen);
	}
	else
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM879:: parameter error!\n");
		return(errCodeParameter);
	}
	return 0;
}

//���֤��Ϸ�����֤CRL 0 �Ϸ�����0֤��Ƿ�
int UnionCheckCertCRLEx(char *appName,char *sCertDN)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	VerifyRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM878", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionCheckCertCRLEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionCheckCertCRLEx:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionCheckCertCRLEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionCheckCertCRLEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&VerifyRet)) < 0)
	{
		UnionUserErrLog("in UnionCheckCertCRL:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}

	if(VerifyRet == 0)  //����CRL
	{
		UnionLog("in UnionCheckCertCRLEx::cert not in crl.\n");
	}
	else if(VerifyRet == 1){
		UnionLog("in UnionCheckCertCRLEx::cert in crl.\n");
	}
	else {
		VerifyRet = -1;
		return VerifyRet;
	}

	return VerifyRet;
}

//���֤��Ϸ�����֤CRL 0 �Ϸ�����0֤��Ƿ�
int UnionCheckCertCRLC(char *sCert,int nCertLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	VerifyRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM878", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// DN
	if ((ret = UnionPutFldIntoStr(conCertData,sCert,nCertLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionCheckCertCRLC:: UnionPutFldIntoStr for conCertData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionCheckCertCRLC:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionCheckCertCRLC:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&VerifyRet)) < 0)
	{
		UnionUserErrLog("in UnionCheckCertCRLC:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}

	if(VerifyRet == 0)  //����CRL
	{
		UnionLog("in UnionCheckCertCRLC::cert not in crl.\n");
	}
	else if(VerifyRet == 1){
		UnionLog("in UnionCheckCertCRLC::cert in crl.\n");
	}
	else {
		VerifyRet = -1;
		return VerifyRet;
	}

	return VerifyRet;
}

//���֤��Ϸ�����֤CRL
/*
����˵������֤ǩ��֤���Ƿ���CRL�б���
����˵����  
����:
		appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
���:
		��
���ز���ֵ��0��������CRL�б��� 1����CRL�б���  ������ʧ��
*/
int UnionSRJ1401CmdCM878(char *appName, char *sCert,int nCertLen)
{
	if (appName != NULL)
	{
		return UnionCheckCertCRLEx(appName,NULL);
	}
	else if (sCert != NULL)
	{
		return UnionCheckCertCRLC(sCert,nCertLen);
	}
	else
	{
		UnionUserErrLog("in UnionSRJ1401CmdCM878:: parameter error!\n");
		return(errCodeParameter);
	}
	return 0;
}

/*
   ����:��֤֤����Ч��,��������֤֤����Ч�ڣ�֤������飬��֤֤������б�
  ����:
  sCertDN:֤��ӵ����DN
  ����:
  0 ֤��������1֤����ڣ�2֤�����쳣��3֤�鴦��֤������б��� 4��֤�鲻���� 
*/
int UnionVerifyCertEx(char *appName,char *sCertDN)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	VerifyRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM876", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	
	if(appName != NULL && strlen(appName) > 0)
	{
		if ((ret = UnionPutFldIntoStr(conBankCode,appName,strlen(appName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionVerifyCertEx:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
	}
	else {
		// DN
		if ((ret = UnionPutFldIntoStr(conCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionVerifyCertEx:: UnionPutFldIntoStr for conCertDN!\n");
			return(ret);
		}
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionVerifyCertEx:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionVerifyCertEx:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&VerifyRet)) < 0)
	{
		UnionUserErrLog("in UnionVerifyCertEx:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}
	 
	if(VerifyRet == 0)
		UnionLog("in UnionVerifyCertEx::OK.\n");

	return VerifyRet;
}

//��֤֤����Ч��
int UnionVerifyCertC(char *sCert,int nCertLen)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	VerifyRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "CM876", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;
	
	// DN
	if ((ret = UnionPutFldIntoStr(conCertData,sCert,nCertLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionVerifyCertC:: UnionPutFldIntoStr for conCertData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionVerifyCertC:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionVerifyCertC:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf + offset,len,conFlag,&VerifyRet)) < 0)
	{
		UnionUserErrLog("in UnionVerifyCertC:: UnionReadSpecIntFldFromStr [%03d]!\n",conFlag);
		return(ret);
	}
	UnionLog("in UnionVerifyCertC::VerifyRet[%d].\n",VerifyRet);
	 
	if(VerifyRet == 0)
		UnionLog("in UnionVerifyCertC::OK.\n");

	return VerifyRet;
}

//��֤֤����Ч��
/*
����˵������֤֤����Ч�ڣ�֤������飬��֤֤��CRL�б�
����˵����  
����:
	appName:֤��Ӧ������,��������ѭ����appID.bankID.ver, appIDΪӦ�ñ�ʶ��bankIDΪ������ʶ, verΪ�汾��
���:
    ��
���ز���ֵ�� 0 ֤��������1֤����ڣ�2֤�����쳣��3֤�鴦��֤������б��� 4��֤�鲻���� ������ʧ��
*/
int UnionSRJ1401CmdCM876(char *appName, char *sCert,int nCertLen)
{
	if (appName != NULL)
	{
		return UnionVerifyCertEx(appName,NULL);
	}
	else
	{
		return UnionVerifyCertC(sCert,nCertLen);
	}
	return 0;
}


// 3.2  ��ȡ��������ǰ֤������(623)
//��ȡ֤������
int UnionSRJ1401CmdES623(int *totalNum)
{
	int		ret = 0,len = 0;
	char		packageBuf[conMaxSizeOfBuf+1]={0};
	int		offset = 0;
	char		tmpBuf[8192+1]={0};
	char		numStr[4];
	int		num = 0;
	int		fldNumberOffset = 0;
	char		errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES623", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES623:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES623:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngTotalNum,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES623:: UnionReadSpecFldFromStr [%03d]!\n",conMngTotalNum);
		return(ret);
	}
	if(totalNum)
		*totalNum = atoi(tmpBuf);
	return 0;
}

//3.4  �ϴ���֤��(631)
//�ϴ���֤�� 631
int UnionSRJ1401CmdES631(char *bankCode,char *bankName,unsigned char* certBuf,int nCertDataSize, int *result)
{
	int	ret=0;
	int	offset=0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	len=0;
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];
	int	ExistRet;

	// �������
	memcpy(packageBuf + offset, "ES631", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(bankCode && strlen(bankCode) > 0)
	{
		// ��������
		if ((ret = UnionPutFldIntoStr(conMngBankCode,(char *)bankCode,strlen(bankCode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES631:: UnionPutFldIntoStr for conMngBankCode!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}
	if(bankName && strlen(bankName) > 0)
	{
		// ������
		if ((ret = UnionPutFldIntoStr(conMngBankName,(char *)bankName,strlen(bankName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES631:: UnionPutFldIntoStr for conMngBankName!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}

	if (certBuf && strlen((char*)certBuf)>0)
	{
		// ֤������
		if ((ret = UnionPutFldIntoStr(conMngCertData,(char *)certBuf,nCertDataSize,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES631:: UnionPutFldIntoStr for conMngCertData!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}
	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES631:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES631:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf+offset,len,conMngcertFlag,&ExistRet)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES631:: UnionReadSpecIntFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}
	*result = ExistRet;
	if (ExistRet == 1)
	{
		UnionLog("in UnionSRJ1401CmdES631::cert already exist.\n");
		return 1;
	}else if(ExistRet == 0){
		return 0;
	}
	else
	{
		return ExistRet;
	}
}

//�ϴ��ⲿ֤�� 620
int UnionSRJ1401CmdES620(char *bankCode,char *bankName,unsigned char *pCertData, int nCertDataSize, int *result)
{

	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	ExistRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES620", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(bankCode && strlen(bankCode) > 0)
	{
		// ��������
		if ((ret = UnionPutFldIntoStr(conMngBankCode,(char *)bankCode,strlen(bankCode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES620:: UnionPutFldIntoStr for conMngBankCode!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}

	if(bankName && strlen(bankName) > 0)
	{
		// ������
		if ((ret = UnionPutFldIntoStr(conMngBankName,(char *)bankName,strlen(bankName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES620:: UnionPutFldIntoStr for conMngBankName!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}
	// ֤������
	if ((ret = UnionPutFldIntoStr(conMngCertData,(char *)pCertData,nCertDataSize,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES620:: UnionPutFldIntoStr for conMngCertData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES620:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES620:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf+offset,len,conMngcertFlag,&ExistRet)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES620:: UnionReadSpecIntFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}
	*result = ExistRet;
	if(ExistRet == 1)
	{
		UnionLog("in UnionSRJ1401CmdES620::cert already exist.\n");
		return 1;
	}
	return ExistRet;
}

//�ϴ��ڲ�֤�飬ͨ��֤���ַ���
int UnionSRJ1401CmdES632(char *bankCode,char *bankName,int keyType,int keyIndex,char* certBuf,int nCertDataSize, int *result)
{
	int	ret=0;
	int	offset=0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	ExistRet = 0;
	int	len=0;
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES632", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(bankCode && strlen(bankCode) > 0)
	{
		// ��������
		if ((ret = UnionPutFldIntoStr(conMngBankCode,(char *)bankCode,strlen(bankCode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES632:: UnionPutFldIntoStr for conMngBankCode!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}
	if(bankName && strlen(bankName) > 0)
	{
		// ������
		if ((ret = UnionPutFldIntoStr(conMngBankName,(char *)bankName,strlen(bankName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES632:: UnionPutFldIntoStr for conMngBankName!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}

	// keyindex
	if ((ret = UnionPutIntFldIntoStr(conHsmFldKeyIndex,keyIndex,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES632:: UnionPutFldIntoStr for conHsmFldKeyIndex");
		return(ret);
	}
	offset += ret;
	num++;

	// keyType
	if ((ret = UnionPutIntFldIntoStr(conMngKeyType,keyType,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES632:: UnionPutFldIntoStr for conMngKeyType");
		return(ret);
	}
	offset += ret;
	num++;

	//certBuf
	if ((ret = UnionPutFldIntoStr(conMngCertData,(char *)certBuf,nCertDataSize,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES632:: UnionPutFldIntoStr for conMngCertData!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES632:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES632:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;

	if ((ret = UnionReadSpecIntFldFromStr(packageBuf+offset,len,conMngcertFlag,&ExistRet)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES632:: UnionReadSpecIntFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}
	*result = ExistRet;
	if (ExistRet == 1)
	{
		UnionLog("in UnionSRJ1401CmdES632::cert already exist.\n");
	}
	else if(ExistRet == 2)
	{
		UnionLog("in UnionSRJ1401CmdES632::cert keyType is not equal input keyType!.\n");
	}
	else if (ExistRet == 3)
	{
		UnionLog("in UnionSRJ1401CmdES632:: save cert failed.\n");
	}
	return ExistRet;
}

//�ϴ�pfx֤��630
int  UnionSRJ1401CmdES630(char *bankCode,char* bankName,int vkindex,int keyType,char *passwd,unsigned char *certBuf, int nCertDataSize, int *result)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	ExistRet = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES630", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(bankCode && strlen(bankCode) > 0)
	{
		// ��������
		if ((ret = UnionPutFldIntoStr(conMngBankCode,(char *)bankCode,strlen(bankCode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionPutFldIntoStr for conBankCode!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}
	if(bankName && strlen(bankName) > 0)
	{
		// ������
		if ((ret = UnionPutFldIntoStr(conMngBankName,(char *)bankName,strlen(bankName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionPutFldIntoStr for conMngBankName!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}
	// ����
	if ((ret = UnionPutFldIntoStr(conMngFlducPassword,(char *)passwd,strlen(passwd),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionPutFldIntoStr for conMngFlducPassword!\n");
		return(ret);
	}
	offset += ret;
	num++;

	// ˽Կ�洢����λ��
	if ((ret = UnionPutIntFldIntoStr(conHsmFldKeyIndex,vkindex,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionPutIntFldIntoStr for conHsmFldKeyIndex!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if ((ret = UnionPutIntFldIntoStr(conMngKeyType,keyType,packageBuf+offset,sizeof(packageBuf)-offset))<0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionPutIntFldIntoStr for conMngKeyType!\n");
		return(ret);
	}
	offset += ret;
	num++;

	if (certBuf && strlen((char*)certBuf)>0)
	{
		// ֤������
		if ((ret = UnionPutFldIntoStr(conMngCertData,(char *)certBuf,nCertDataSize,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
		{
			UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionPutFldIntoStr for conMngCertData!\n");
			return(ret);
		}
		offset += ret;
		num++;
	}

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES630:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}

	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf+offset,len,conMngcertFlag,&ExistRet)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionReadSpecIntFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}

	*result = ExistRet;
	if(ExistRet == 1)
	{
		UnionLog("in UnionSRJ1401CmdES630::cert already exist.\n");
	}
	else if(ExistRet == 2)
	{
		UnionLog("in UnionSRJ1401CmdES630::index[%d] invaild.\n",vkindex);
	}
	else if(ExistRet == 0) 
	{
		UnionLog("in UnionSRJ1401CmdES630::OK!\n");
	}
	return ExistRet;
}

//ɾ��֤��  621
int UnionSRJ1401CmdES621(char *sCertDN)
{

	int	ret = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES621", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// DN
	if ((ret = UnionPutFldIntoStr(conMngCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES621:: UnionPutFldIntoStr for conMngCertDN!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES630:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES630:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	return ret;
}

//����DN��ȡ��Կ֤����Ϣ(����֤��)
//��Կ֤������ӦΪDER��ʽ
int UnionSRJ1401CmdES622(char *sCertDN, unsigned char *pCertData, int *pCertDataSize)
{


	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	tmpBuf[8192+1]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES622", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// DN
	if ((ret = UnionPutFldIntoStr(conMngCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES622:: UnionPutFldIntoStr for conMngCertDN!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES622:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES622:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngCERTINFOcert,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES622:: UnionReadSpecFldFromStr [%03d]!\n",conMngCERTINFOcert);
		return(ret);
	}
	if(pCertDataSize)
		*pCertDataSize = ret;

	if(pCertData)
		memcpy(pCertData,tmpBuf,ret);

	return 0;
}

//���CRL�ļ��Ƿ��Ѿ�����
int UnionSRJ1401CmdES626(char* fileName)
{

	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	int	ExistRet=0;
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES626", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	//�ļ�����
	if ((ret = UnionPutFldIntoStr(conMngHsmFldFileName,fileName,strlen(fileName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES626:: UnionPutFldIntoStr for conMngHsmFldFileName!\n");
		return(ret);
	}
	num++;
	offset += ret;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES626:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES626:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;
	if ((ret = UnionReadSpecIntFldFromStr(packageBuf+offset,len,conMngCRLFlag,&ExistRet)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES626:: UnionReadSpecIntFldFromStr [%03d]!\n",conMngCRLFlag);
		return(ret);
	}
	return ExistRet;
}

//��ȡÿҳ��֤��DN�嵥
int UnionSRJ1401CmdES821(int PageNo, int nMaxCntOfPage,TUnionX509CertinfoESSC *certInfo,int *pRetCnt)
{
	int	ret = 0,len = 0;
	char	packageBuf[8192*8*4]={0};
	int	offset = 0;
	char	tmpBuf[8192*8*4]={0};
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES821", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if(nMaxCntOfPage > 60)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES821::nMaxCntOfPage[%d] too large!\n",nMaxCntOfPage);
		return errCodeParameter;
	}

	sprintf(tmpBuf,"%d",PageNo);
	//ҳ��
	if ((ret = UnionPutFldIntoStr(conMngPageNo,tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES821:: UnionPutFldIntoStr for conMngPageNo!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(tmpBuf,"%d",nMaxCntOfPage);
	// ÿҳ������¼��
	if ((ret = UnionPutFldIntoStr(conMngMaxNumPerPage,tmpBuf,strlen(tmpBuf),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES821:: UnionPutFldIntoStr for conMngMaxNumPerPage!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES821:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES821:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngCurrentNum,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES821:: UnionReadSpecFldFromStr [%03d]!\n",conMngCurrentNum);
		return(ret);
	}
	if(pRetCnt)
		*pRetCnt = atoi(tmpBuf);

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngCertinfo,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES821:: UnionReadSpecFldFromStr [%03d]!\n",conMngCertinfo);
		return(ret);
	}
	memcpy(certInfo,tmpBuf,sizeof(TUnionX509CertinfoESSC)*(*pRetCnt));

	return 0;
}

//��ѯָ��userDN��֤��
int UnionSRJ1401CmdES822(char *sCertDN,PTTUnionX509Certinfo certInfo,int *certExist)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	char	tmpBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES822", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// DN
	if ((ret = UnionPutFldIntoStr(conMngCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES822:: UnionPutFldIntoStr for conMngCertDN[%d]!\n",conMngCertDN);
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES822:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES822:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngcertFlag,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES822:: UnionReadSpecFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}
	if(certExist)
		*certExist = atoi(tmpBuf);

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf + offset,len,conMngCertinfo,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES822:: UnionReadSpecFldFromStr [%03d]!\n",conMngCertinfo);
		return(ret);
	}
	memcpy(certInfo,tmpBuf,sizeof(TUnionX509Certinfo));
	return 0;
}

//��ѯָ��֤��Ӧ������֤��
int UnionSRJ1401CmdES831(char *sCertappName,PTTUnionX509Certinfo certInfo,int *certExist)
{
	int	ret = 0,len = 0;
	char	packageBuf[conMaxSizeOfBuf+1]={0};
	char	tmpBuf[conMaxSizeOfBuf+1]={0};
	int	offset = 0;
	char	numStr[4];
	int	num = 0;
	int	fldNumberOffset = 0;
	char	errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES831", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// ֤��Ӧ����
	if ((ret = UnionPutFldIntoStr(conBankCode,sCertappName,strlen(sCertappName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES831:: UnionPutFldIntoStr for conBankCode[%d]!\n",conBankCode);
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES831:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES831:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngcertFlag,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES831:: UnionReadSpecFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}
	if(certExist)
		*certExist = atoi(tmpBuf);

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngCertinfo,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES831:: UnionReadSpecFldFromStr [%03d]!\n",conMngCertinfo);
		return(ret);
	}
	memcpy(certInfo,tmpBuf,sizeof(TUnionX509Certinfo));
	return 0;
}

//����֤�������ļ�
/*
����:��˽Կ�ļ�����PEM��ʽ��p10֤�������ļ�
����:
keyindex: ˽Կ����
algID: ��Կ�㷨��־.0-RSA 1--SM2
countryName    :������(C)
stateOrProvinceName   :ʡ��(S)
localityName 		:������(L)
organizationName  :��λ��(O)
organizationalUnitName  :����(OU)
commonName 		:�û���(CN)
email 			:EMail��ַ(E)
fileName:���ɵ�p10֤�������ļ���(��·��)
����:
>=0 �ɹ�
<0 ʧ��
*/
int UnionSRJ1401CmdES824(int keyindex,int algID,char *countryName,char *stateOrProvinceName,char *localityName,char *organizationName,
			char *organizationalUnitName,char *commonName,char *email,char *p10buf)
{
	int		ret = 0,len = 0;
	char		packageBuf[8192*4]={0};
	int		offset = 0;
	char		tmpBuf[8192*4]={0};
	char		numStr[4];
	int		num = 0;
	int		fldNumberOffset = 0;
	char		errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES824", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	// keyindex
	if ((ret = UnionPutIntFldIntoStr(conHsmFlduiISKIndex,keyindex,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for conHsmFlduiISKIndex");
		return(ret);
	}
	offset += ret;
	num++;

	// algID
	if ((ret = UnionPutIntFldIntoStr(conFlag,algID,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for conFlag");
		return(ret);
	}
	offset += ret;
	num++;

	// conCountryName
	if ((ret = UnionPutFldIntoStr(conCountryName,countryName,strlen(countryName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for conCountryName");
		return(ret);
	}
	offset += ret;
	num++;

	//conStateOrProvinceName
	if ((ret = UnionPutFldIntoStr(conStateOrProvinceName,(char *)stateOrProvinceName,strlen(stateOrProvinceName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for conStateOrProvinceName!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//localityName
	if ((ret = UnionPutFldIntoStr(conLocalityName,(char *)localityName,strlen(localityName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for localityName!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//organizationName
	if ((ret = UnionPutFldIntoStr(conOrganizationName,(char *)organizationName,strlen(organizationName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for organizationName!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//organizationalUnitName
	if ((ret = UnionPutFldIntoStr(conOrganizationalUnitName,(char *)organizationalUnitName,strlen(organizationalUnitName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for organizationalUnitName!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//commonName
	if ((ret = UnionPutFldIntoStr(conCommonName,(char *)commonName,strlen(commonName),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for commonName!\n");
		return(ret);
	}
	offset += ret;
	num++;

	//email
	if ((ret = UnionPutFldIntoStr(conEmail,(char *)email,strlen(email),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionPutFldIntoStr for email!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES824:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngPKCS10Buf,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES824:: UnionReadSpecFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}
	memcpy(p10buf,tmpBuf,ret);
	return ret;
}

//����userDN����֤�������ļ�
int UnionSRJ1401CmdES825(int keyindex,int algID,char *sCertDN,char *p10buf)
{

	int		ret = 0,len = 0;
	char		packageBuf[conMaxSizeOfBuf+1]={0};
	int		offset = 0;
	char		tmpBuf[8192+1]={0};
	char 		numStr[4];
	int		num = 0;
	int		fldNumberOffset = 0;
	char		errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES825", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if ((ret = UnionPutIntFldIntoStr(conHsmFlduiISKIndex,keyindex,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES825:: UnionPutFldIntoStr for conHsmFlduiISKIndex");
		return(ret);
	}
	offset += ret;
	num++;

	// algID
	if ((ret = UnionPutIntFldIntoStr(conFlag,algID,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES825:: UnionPutFldIntoStr for conFlag");
		return(ret);
	}
	offset += ret;
	num++;

	//UserDN
	if ((ret = UnionPutFldIntoStr(conMngCertDN,sCertDN,strlen(sCertDN),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES825:: UnionPutFldIntoStr for conMngCertDN[%d]!\n",conMngCertDN);
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES825:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES825:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+ offset,len,conMngPKCS10Buf,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES825:: UnionReadSpecFldFromStr [%03d]!\n",conMngcertFlag);
		return(ret);
	}
	memcpy(p10buf,tmpBuf,ret);
	return ret;
}
 
// ����֤��ͬ���ļ�
int UnionSRJ1401CmdES823(TUnionTargetDevWithCount targetInfo)
{
	int		ret=0;
	char		packageBuf[conMaxSizeOfBuf+1]={0};
	int		offset = 0;
	char		tmpBuf[8192+1]={0};
	char		numStr[8];
	int		num = 0;
	int		fldNumberOffset = 0;
	char		errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES823", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	memcpy(tmpBuf,&targetInfo,sizeof(TUnionTargetDevWithCount));
	if ((ret = UnionPutFldIntoStr(conMngCertSynDevinfo,tmpBuf,sizeof(TUnionTargetDevWithCount),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES823:: UnionPutFldIntoStr for conMngCertinfo!\n");
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES823:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES823:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	return(0);
}

// ֤��ͬ���豸�鿴(843)
int UnionSRJ1401CmdES843(TUnionTargetDevWithCount *targetInfo)
{
	int		ret = 0,len = 0;
	char		packageBuf[conMaxSizeOfBuf+1]={0};
	int		offset = 0;
	char		tmpBuf[8192+1]={0};
	char		numStr[4];
	int		num = 0;
	int		fldNumberOffset = 0;
	char		errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES843", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES843:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES843:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecFldFromStr(packageBuf+offset,len,conMngCertSynDevinfo,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES843:: UnionReadSpecFldFromStr [%03d]!\n",conMngCertSynDevinfo);
		return(ret);
	}
	memcpy(targetInfo,tmpBuf,sizeof(TUnionTargetDevWithCount));
	return 0;
}

// ֤��ͬ���豸����(844)
int UnionSRJ1401CmdES844(char *ip, int port)
{

	int		ret = 0,len = 0;
	char		packageBuf[8192*8*4]={0};
	int		offset = 0;
	char		tmpBuf[8192*8*4]={0};
	char 		numStr[4];
	int		num = 0;
	int		value=0;
	int		fldNumberOffset = 0;
	char		errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES844", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if (ip == NULL || port <0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES844:: parameter err!\n");
		return errCodeParameter;
	}

	//IP
	if ((ret = UnionPutFldIntoStr(conMngCertSynTargetHsmIp,ip,strlen(ip),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES844:: UnionPutFldIntoStr for conMngCertSynTargetHsmIp=[%s]!\n",ip);
		return(ret);
	}
	offset += ret;
	num++;

	//port
	if ((ret = UnionPutIntFldIntoStr(conMngCertSynTargetHsmport,port,packageBuf+offset,sizeof(packageBuf)-offset))<0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES844:: UnionPutFldIntoStr for conMngCertSynTargetHsmport=[%d]!\n",port);
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES844:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES844:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret =UnionReadSpecIntFldFromStr(packageBuf+offset,len,conMngCertSynDevIsExist,&value))<0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES844:: UnionReadSpecFldFromStr conMngCertSynDevIsExist=[%d]!\n",value);
		return(ret);
	}
	return value;
}

// ֤��ͬ���豸ɾ��(845)
int UnionSRJ1401CmdES845(char *ip, int port)
{
	int		ret = 0,len = 0;
	char		packageBuf[8192*8*4]={0};
	int		offset = 0;
	char		tmpBuf[8192*8*4]={0};
	char		numStr[4];
	int		num = 0;
	int		value=0;
	int		fldNumberOffset = 0;
	char		errCode[8];

	// �������
	memcpy(packageBuf + offset, "ES845", 5);
	offset += 5;

	// 1�ֽ�����/��Ӧ��ʶ���̶�Ϊ 1
	memcpy(packageBuf + offset, "1", 1);
	offset += 1;

	// 3λ����������
	fldNumberOffset = offset;
	offset += 3;

	if (ip == NULL || port <0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES845:: parameter err!\n");
		return errCodeParameter;
	}

	//IP
	if ((ret = UnionPutFldIntoStr(conMngCertSynTargetHsmIp,ip,strlen(ip),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES845:: UnionPutFldIntoStr for conMngCertSynTargetHsmIp=[%s]!\n",ip);
		return(ret);
	}
	offset += ret;
	num++;

	//port
	if ((ret = UnionPutIntFldIntoStr(conMngCertSynTargetHsmport,port,packageBuf+offset,sizeof(packageBuf)-offset))<0)
	{
		UnionUserErrLog("in UnionDeleteCertSynDevinfo:: UnionPutFldIntoStr for conMngCertSynTargetHsmport=[%d]!\n",port);
		return(ret);
	}
	offset += ret;
	num++;

	sprintf(numStr,"%03d",num);
	memcpy(packageBuf + fldNumberOffset,numStr,3);
	packageBuf[offset] = 0;

	// �������ͨѶ
	UnionSetLenOfHsmCmdHeader(0);
	UnionSetIsNotCheckHsmResCode();
        if ((ret = UnionDirectHsmCmd(packageBuf,offset,packageBuf,sizeof(packageBuf))) < 0)
        {       
                UnionUserErrLog("in UnionSRJ1401CmdES845:: UnionDirectHsmCmd!\n");
                return(ret);
        }
	packageBuf[ret] = 0;

	if (memcmp(packageBuf+ 4, "000000", 6) != 0)
	{
                UnionUserErrLog("in UnionSRJ1401CmdES845:: response[%s]!\n", packageBuf);
		memcpy(errCode, packageBuf + 4, 6);
		errCode[6] = 0;
		return(errCodeOffsetOfSRJ1401 + atoi(errCode));
	}
	len = ret;
	offset = 10;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret =UnionReadSpecIntFldFromStr(packageBuf+offset,len,conMngCertSynDevIsExist,&value))<0)
	{
		UnionUserErrLog("in UnionSRJ1401CmdES845:: UnionReadSpecFldFromStr conMngCertSynDevIsExist=[%d]!\n",value);
		return(ret);
	}
	return value;
}
