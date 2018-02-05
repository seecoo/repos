//	Author:		科友公司研发部
//	Copyright:	Union Tech. Guangzhou
//	Date:		2008/3

// 	本程序提供密码服务平台的底层API函数

#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "esscFldTagDef.h"
#include "unionDesKey.h"

#define conMaxSizeOfBuf	8192

// 断开与ESSC的连接
void UnionDisconnectEsscSvr()
{
	UnionReleaseCommWithHsmSvrToEssc();
}


// 将一个ESSC报文域打入到包中,返回打入到包中的数据的长度
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

// 从一个ESSC报文中读取一个域，返回域在包中占的长度
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

	// 域数目
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
		if (tmpFldTag == fldTag)	// 找到
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


// ================================= DP函数 =================================
/*
功能
	使用U1指令加密数据
输入参数：
	key：			如果是密钥索引，则为密钥索引的字符串形式；如果是密钥名称，
				则为密钥名称全名；
	mode：			加密模式，与U1指令对应；
	plainTextLen：		明文数据长度，扩展后的数据长度；
	plainText：		扩展后的明文数据；
	sizeofCipherText：	接受密文数据的缓冲区大小，接受扩展后的密文数据；
输出参数
	cipherText：		扩展后的密文数据；
返回值
	>=0			密文数据的长度	
	<0			失败，返回错误码
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
	// 域数目
	sprintf(packageBuf+offset,"%03d",6);
	offset += 3;
	// 域
	// 加密模式标识
	if ((ret = UnionPutFldIntoStr(conEsscFldAlgorithmMode,"0",1,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldAlgorithmMode!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// 方案ID
	if ((ret = UnionPutFldIntoStr(conEsscFldKeyModuleID,mode,strlen(mode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldKeyModuleID!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// 密钥名称或密钥索引
	if ((ret = UnionPutFldIntoStr(conEsscFldKeyName,key,strlen(key),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldKeyName!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	if (strstr(key,".") == NULL)
	{
		// 密钥类型conMKAC
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
	// 离散次数0
	if ((ret = UnionPutFldIntoStr(conEsscDisperseTimes,"0",1,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscDisperseTimes!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// 明文数据
	if ((ret = UnionPutFldIntoStr(conEsscFldData,plainText,plainTextLen,packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldData!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// 域数目
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
功能
	使用Y1指令转加密数据
输入参数：
	srcKey：			源加密密钥，如果是密钥索引，则为密钥索引的字符串形式；如果是密钥名称，则为密钥名称全名；
	srcMode：		源加密模式，与Y1指令对应；
	dstKey：			目的加密密钥，如果是密钥索引，则为密钥索引的字符串形式；如果是密钥名称，则为密钥名称全名；
	dstMode：		目的加密模式，与Y1指令对应；
	srcCipherTextLen：	源密文数据长度，扩展后的数据长度；
	srcCipherText：		扩展后的源密文数据；
	sizeofdstCipherText：	接受目的密文数据的缓冲去大小，接受扩展后的密文数据；
输出参数
	dstCipherText：		扩展后的目的密文数据；
返回值
	>=0			密文数据的长度	
	<0			失败，返回错误码
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
	// 域数目
	//sprintf(packageBuf+offset,"%03d",8);
	offset += 3;
	// 域
	// 源密钥名称或密钥索引
	if ((ret = UnionPutFldIntoStr(conEsscFldFirstWKName,srcKey,strlen(srcKey),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldFirstWKName!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	if (strstr(srcKey,".") == NULL)
	{
		// 密钥类型conZEK
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
	// 源加密算法
	if ((ret = UnionPutFldIntoStr(conEsscFldAlgorithm01Mode,srcMode,strlen(srcMode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldAlgorithm01Mode!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// 目的密钥名称或密钥索引
	if ((ret = UnionPutFldIntoStr(conEsscFldSecondWKName,dstKey,strlen(dstKey),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldSecondWKName!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	if (strstr(dstKey,".") == NULL)
	{
		// 密钥类型conZEK
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
	// 目的加密算法
	if ((ret = UnionPutFldIntoStr(conEsscFldAlgorithm02Mode,dstMode,strlen(dstMode),packageBuf+offset,sizeof(packageBuf)-offset)) < 0)
	{
		UnionUserErrLog("in UnionTransEncryptDataForDP:: UnionPutFldIntoStr for conEsscFldAlgorithm02Mode!\n");
		return(ret);
	}
	fldNum++;
	offset += ret;
	// 密文数据
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
	//域数目
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
函数名称	UnionVerifySignatureUsingInputPK
功能	        使用输入的公钥验证签名。这个函数与UnionVerifyRsaSignature的区别
		在于，PK是由客户端发送过来的
输入参数	pkValue：公开密钥值
                flag：数据填充方式：0，如果数据不是密钥长度的整倍数，
                后面补0x00；1，PKCS填充方式。一般使用PKCS填充方式。
                hashID：HASH算法标志，如果采用RACAL标准指令密码机指令EY：
                01，SHA-1，02，MD5，03，ISO 1011802，04，NoHash；
                如果采用SJL06 38指令：‘0’：MD5，‘1’：SHA-1；
                如果不需要做HASH，为空值，即“NULL”。
                lenOfData：签名数据的长度
                data：签名数据
                sign：待验证的签名
                lenOfSign：待验证签名的长度
输出参数	无
返回值	        <0：函数执行失败，值为失败的错误码
                =0：签名验证成功
密钥	        1．pkValue：验证签名的公钥
服务代码	135
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
	// 域数目
	if (hashID != NULL && strcmp(hashID,"N") != 0)
		sprintf(packageBuf+offset,"%03d",5);
	else
		sprintf(packageBuf+offset,"%03d",4);
	offset += 3;
	// 域
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
        //压缩签名
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


