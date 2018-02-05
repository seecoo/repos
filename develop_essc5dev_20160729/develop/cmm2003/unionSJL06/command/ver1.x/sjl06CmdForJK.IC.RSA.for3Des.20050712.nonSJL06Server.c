// Author:	Wolfgang Wang
// Date:	2003/10/09

// 2006/03/03 增加了	
//	3A/3B/30/33/3D/39/
#define _SJL06CmdForJK_IC_RSA_

#include <stdio.h>
#include <string.h>

#include "sjl06.h"
#include "sjl06Protocol.h"
#include "sjl06Cmd.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"

/*
七、	转换DES密钥：从主密钥加密到公钥加密
用于分发密钥。

输入域	长度	类型	说　明
命令代码	2	A	值"3B"
密钥密文	32	H	用主密钥加密的DES密钥
公　钥	n	B	


输出域	长度	类型	说　明
响应代码	2	A	"3C"
错误代码	2	H	
检查值	16	H	DES密钥加密64bits的0
密钥长度	4	N	DES密钥密文的字节数
密钥密文	n	B	用公钥加密的DES密钥
*/
int SJL06Cmd3B(int hsmSckHDL,PUnionSJL06 pSJL06,char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192+100];
	int		hsmCmdLen = 0;
	int		len;
	
	if ((keyByPK == NULL) || (keyByMK == NULL) || (errCode == NULL) || (pk == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd3B:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"3B",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,keyByMK,32);
	hsmCmdLen += 32;
	if ((ret = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd3E:: UnionFormANSIDERRSAPK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd3B:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"3C",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd3B:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	hsmCmdBuf[ret] = 0;
	memcpy(errCode,hsmCmdBuf+2,2);
	if (strncmp(errCode,"00",2) != 0)
		return(0);
	memcpy(checkValue,hsmCmdBuf+4,16);
	if ((len = UnionConvertIntoLen(hsmCmdBuf+4+16,4)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd3B:: UnionConvertIntoLen!\n");
		UnionMemErrLog("hsmReturn::",hsmCmdBuf,ret);
		return(len);
	}
	if (len > sizeOfBuf)
	{
		UnionUserErrLog("in SJL06Cmd3B:: lenOfKeyByPK [%d] too long!\n",len);
		return(errCodeParameter);
	}
	memcpy(keyByPK,hsmCmdBuf+4+16+4,len);
	return(len);
}

/*
六、	转换DES密钥：从公钥加密到主密钥加密
用于接收密钥。

输入域	长度	类型	说　明
命令代码	2	A	值"3A"
私钥索引	2	N	"00"－"20"：用密码机内的私钥
密钥长度	4	N	DES密钥密文的字节数
密钥密文	n	B	用公钥加密的DES密钥


输出域	长度	类型	说　明
响应代码	2	A	"3B"
错误代码	2	H	
DES密钥	32	H	用主密钥加密的DES密钥
检查值	16	H	DES密钥加密64bits的0
*/
int SJL06Cmd3A(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192+100];
	int		hsmCmdLen = 0;
	
	if ((keyByPK == NULL) || (keyByMK == NULL) || (errCode == NULL) || (lenOfKeyByPK <= 0) || (checkValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd3A:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"3A",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",vkIndex);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfKeyByPK);
	hsmCmdLen += 4;
	if (hsmCmdLen + lenOfKeyByPK >= sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd3A:: lenOfKeyByPK [%d] too long!\n",lenOfKeyByPK);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyByPK,lenOfKeyByPK);
	hsmCmdLen += lenOfKeyByPK;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd3A:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"3B",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd3A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	hsmCmdBuf[ret] = 0;
	memcpy(errCode,hsmCmdBuf+2,2);
	if (strncmp(errCode,"00",2) != 0)
		return(0);
	memcpy(keyByMK,hsmCmdBuf+4,32);
	memcpy(checkValue,hsmCmdBuf+4+32,16);
	return(32);
}


int SJL06Cmd3E(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk,char *pk,char *keyByBMK,char *checkValue,char *keyByPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[4048+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[10];
	int		len;
	
	if ((bmk == NULL) || (pk == NULL) || (keyByBMK == NULL) || (checkValue == NULL) || (keyByPK == NULL))
	{
		UnionUserErrLog("in SJL06Cmd3E:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"3E",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmk,3);
	hsmCmdLen += 3;
	if ((ret = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd3E:: UnionFormANSIDERRSAPK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd3E:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"3F",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd3E:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	hsmCmdBuf[ret] = 0;
	memcpy(errCode,hsmCmdBuf+2,2);
	if (strncmp(errCode,"00",2) != 0)
		return(0);
	memcpy(keyByBMK,hsmCmdBuf+4,16);
	memcpy(checkValue,hsmCmdBuf+4+16,16);
	memcpy(tmpBuf,hsmCmdBuf+4+16+16,4);
	tmpBuf[4] = 0;
	if ((len = atol(tmpBuf)) != 128)
	{
		UnionUserErrLog("in SJL06Cmd3E:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	bcdhex_to_aschex(hsmCmdBuf+4+16+16+4,128,keyByPK);
	return(0);
}


int SJL06Cmd3C(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,int lenOfData,char *data,char *hash,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[4048+1];
	int		hsmCmdLen = 0;
	
	if ((lenOfData <= 0) || (data == NULL) || (hash == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in SJL06Cmd3C:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"3C",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData);
	hsmCmdLen += 4;
	if (hsmCmdLen + lenOfData > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd3C:: lenOfData [%d] too long!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,lenOfData);
	hsmCmdLen += lenOfData;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd38:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"3D",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd3C:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (flag == '1')
	{
		if (ret - 4 != 20)
		{
			UnionUserErrLog("in SJL06Cmd3C:: ret = [%d] error for sha-1!\n",ret-4);
			return(errCodeSJL06MDL_InvalidHsmRetLen);
		}
		bcdhex_to_aschex(hsmCmdBuf+4,20,hash);
		return(40);
	}
	else
	{
		if (ret - 4 != 16)
		{
			UnionUserErrLog("in SJL06Cmd3C:: ret = [%d] error for sha-1!\n",ret-4);
			return(errCodeSJL06MDL_InvalidHsmRetLen);
		}
		bcdhex_to_aschex(hsmCmdBuf+4,16,hash);
		return(32);
	}
}

int SJL06Cmd38(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[4048+1];
	int		hsmCmdLen = 0;
	
	if ((signLen <= 0) || (pk == NULL) || (dataLen <= 0) || (data == NULL) || (signature == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in SJL06Cmd38:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"38",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signLen);
	hsmCmdLen += 4;
	if (hsmCmdLen + signLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd38:: signLen [%d] too long!\n",signLen);
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
		UnionUserErrLog("in SJL06Cmd38:: dataLen [%d] too long!\n",dataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	if ((ret = UnionFormANSIDERRSAPK(pk,strlen(pk),hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd38:: UnionFormANSIDERRSAPK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd38:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"39",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd38:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int SJL06Cmd37(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int dataLen,char *data,char *signature,int sizeOfSignature,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		signLen;
	
	if ((vkIndex == NULL) || (dataLen <= 0) || (data == NULL) || (signature == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in SJL06Cmd37:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"37",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",dataLen);
	hsmCmdLen += 4;
	if (hsmCmdLen + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd37:: dataLen [%d] too long!\n",dataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd37:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"38",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd37:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd37:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	if ((signLen = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd37:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	
	if (signLen > sizeOfSignature)
	{
		UnionUserErrLog("in SJL06Cmd37:: sizeOfSignature [%d] too small!\n",sizeOfSignature);
		return(errCodeSmallBuffer);
	}
	
	memcpy(signature,hsmCmdBuf+4+4,signLen);
	return(signLen);
}

int SJL06Cmd34(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,int lenOfVK,char *pk,int sizeOfPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[2048+1];
	int		vkLen;
	int		tmpLen;
		
	if ((vkIndex == NULL) || ((lenOfVK != 256) && (lenOfVK != 512) && (lenOfVK != 1024) && (lenOfVK != 2048) && (lenOfVK != 4096)))
	{
		UnionUserErrLog("in SJL06Cmd34:: lenOfVK = [%d]\n",lenOfVK);
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"34",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVK);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd34:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"35",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd35:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd35:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	// Get PK
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	vkLen = atoi(tmpBuf);
	UnionDebugLog("in SJL06Cmd35:: vkLen = [%04d] derPKLen = [%04d]\n",vkLen,ret-4-4-vkLen);
	UnionMemLog("in SJL06Cmd35::",hsmCmdBuf+4+4+vkLen,ret-4-4-vkLen);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (lenOfVK / 8 * 2 > sizeOfPK)
	{
		UnionUserErrLog("in SJL06Cmd35:: sizeOfPK [%d] too small!\n",sizeOfPK);
		return(errCodeSmallBuffer);
	}
	switch (lenOfVK)
	{
		case	512:
			tmpLen = 4;
			break;
		case	1024:
			tmpLen = 6;
			break;
		case	2048:
			tmpLen = 8;
			break;
		case	4096:
			tmpLen = 8;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd35:: lenOfVK error! [%d]\n",lenOfVK);
			return(errCodeSmallBuffer);
	}	
	bcdhex_to_aschex(hsmCmdBuf+4+4+vkLen+tmpLen,lenOfVK/8,pk);
	//memset(tmpBuf,0,sizeof(tmpBuf));
	//bcdhex_to_aschex(hsmCmdBuf+4+4+vkLen,ret-4-4-vkLen,tmpBuf);
	//UnionLog("in SJL06Cmd35:: pk = [%s]\n",tmpBuf);
	return(lenOfVK/8*2);
}

int SJL06Cmd40(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0;
		
	if ((vkIndex == NULL) || (bmk == NULL) || (pvk == NULL) || (pinByPK == NULL) || (pinByPVK == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
		UnionUserErrLog("in SJL06Cmd40:: lenOfPinByPK [%d] too long!\n",lenOfPinByPK);
		return(errCodeSmallBuffer);
	}
		
	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd40:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"41",2) != 0) || ((ret != 4) && (ret != 4+16)))
	{
		UnionUserErrLog("in SJL06Cmd40:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4+16)
	{
		memcpy(pinByPVK,hsmCmdBuf+4,16);
		return(16);
	}
	else
		return(0);
}

int SJL06Cmd41(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0;
		
	if ((vkIndex == NULL) || (bmk == NULL) || (zpk == NULL) || (pan == NULL) || (pinByPK == NULL) || (pinByZPK == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"41",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmk,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,zpk,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + lenOfPinByPK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd41:: lenOfPinByPK [%d] too long!\n",lenOfPinByPK);
		return(errCodeSmallBuffer);
	}
		
	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd41:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"42",2) != 0) || ((ret != 4) && (ret != 4+16)))
	{
		UnionUserErrLog("in SJL06Cmd41:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4+16)
	{
		memcpy(pinByZPK,hsmCmdBuf+4,16);
		return(16);
	}
	else
		return(0);
}

int SJL06Cmd42(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0;
		
	if ((vkIndex == NULL) || (encryptedData == NULL) || (plainText == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"42",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	if (hsmCmdLen + lenOfEncryptedData > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd42:: lenOfEncryptedData [%d] too long!\n",lenOfEncryptedData);
		return(errCodeSmallBuffer);
	}
		
	memcpy(hsmCmdBuf+hsmCmdLen,encryptedData,lenOfEncryptedData);
	hsmCmdLen += lenOfEncryptedData;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd42:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"43",2) != 0) || ((ret != 4) && (ret != 4+lenOfEncryptedData)))
	{
		UnionUserErrLog("in SJL06Cmd42:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4+lenOfEncryptedData)
	{
		memcpy(plainText,hsmCmdBuf+4,lenOfEncryptedData);
		return(lenOfEncryptedData);
	}
	else
		return(0);
}

int SJL06CmdE2(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0;
		
	if ((version == NULL) || (group == NULL) || (index == NULL) || (key == NULL) || (keyCheckValue == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"E2",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,key,32);
	hsmCmdLen += 32;
	memcpy(hsmCmdBuf+hsmCmdLen,keyCheckValue,4);
	hsmCmdLen += 4;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06CmdE2:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"E3",2) != 0) || (ret != 4))
	{
		UnionUserErrLog("in SJL06CmdE2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}


int SJL06Cmd76(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int macDataHeadLen,char *macData,
		int dataLen,char *data,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2024];
	int		hsmCmdLen = 0;
		
	if ((version == NULL) || (group == NULL) || (index == NULL) || (lsdata == NULL) || (gcdata == NULL) ||
		(macData == NULL) || (data == NULL) || (macDataHeadLen <= 0) || (dataLen <= 0) || 
		(macDataHeadLen >= 1000) || (dataLen >= 1000))
	{
		UnionUserErrLog("in SJL06Cmd76:: macDataHeadLen = [%d] dataLen = [%d]\n",macDataHeadLen,dataLen);
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"76",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,gcdata,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + 3 + 3 + macDataHeadLen + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd76:: too long string [%d]!\n",hsmCmdLen + 3 + 3 + macDataHeadLen + dataLen);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",macDataHeadLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,macData,macDataHeadLen);
	hsmCmdLen += macDataHeadLen;
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",dataLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd76:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"77",2) != 0) || (ret != 4))
	{
		UnionUserErrLog("in SJL06Cmd76:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int SJL06Cmd72(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		int pinOffset1,int pinLen1,int pinOffset2,int pinLen2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version == NULL) || (group == NULL) || (index == NULL) || (lsdata == NULL) || (gcdata == NULL) ||
		(bmk == NULL) || (pik == NULL) || (accNo == NULL) || (dataLen <= 0) || (dataLen >= 1000))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"72",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,gcdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,bmk,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,pik,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,16);
	hsmCmdLen += 16;
	if (pinLen1 > 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%03d%02d%03d%02d",pinOffset1,pinLen1,pinOffset2,pinLen2);
		hsmCmdLen += 10;
	}
	else
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%03d%02d",pinOffset1,pinLen1);
		hsmCmdLen += 5;
	}
	
	if (hsmCmdLen + 3 + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd72:: too long string [%d]!\n",hsmCmdLen + 3 + dataLen);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",dataLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd72:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"73",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd72:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4)
		return(0);
	memcpy(tmpBuf,hsmCmdBuf+4,3);
	tmpBuf[3] = 0;
	if ((((len = atoi(tmpBuf)) > sizeOfPlainDataBuf)) || (len < 0))
	{
		UnionUserErrLog("in SJL06Cmd72:: small receivebuf [%d] expected  [%d]\n",sizeOfPlainDataBuf,len);
		return(errCodeSmallBuffer);
	}
	memcpy(plainData,hsmCmdBuf+4+3,len);
	return(len);
}

int SJL06Cmd71(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version1 == NULL) || (group1 == NULL) || (index1 == NULL) || (lsdata == NULL) || (cardNo == NULL) ||
		(version2 == NULL) || (group2 == NULL) || (index2 == NULL) ||
		(key == NULL) || (random == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"71",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version1,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,version2,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,cardNo,16);
	hsmCmdLen += 16;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd71:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"72",2) != 0) || (ret < 4 ))
	{
		UnionUserErrLog("in SJL06Cmd71:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4)
		return(0);
	if (ret != 4 + 32 + 16)
	{
		UnionUserErrLog("in SJL06Cmd71:: ret = [%d]!\n",ret);
		return(errCodeSJL06MDL_InvalidHsmRetLen);
	}
	memcpy(key,hsmCmdBuf+4,32);
	memcpy(random,hsmCmdBuf+4+32,16);
	return(0);
}

int SJL06Cmd73(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		char seprator,int pinFld1,int pinFld2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version == NULL) || (group == NULL) || (index == NULL) || (lsdata == NULL) || (gcdata == NULL) ||
		(bmk == NULL) || (pik == NULL) || (accNo == NULL) || (dataLen <= 0) || (dataLen >= 1000))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"73",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,gcdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,bmk,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,pik,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,16);
	hsmCmdLen += 16;
	hsmCmdBuf[hsmCmdLen] = seprator;
	hsmCmdLen += 1;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d%02d",pinFld1,pinFld2);
	hsmCmdLen += 4;
	if (hsmCmdLen + 3 + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd73:: too long string [%d]!\n",hsmCmdLen + 3 + dataLen);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",dataLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd73:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"74",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd73:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4)
		return(0);
	memcpy(tmpBuf,hsmCmdBuf+4,3);
	tmpBuf[3] = 0;
	if ((((len = atoi(tmpBuf)) > sizeOfPlainDataBuf)) || (len < 0))
	{
		UnionUserErrLog("in SJL06Cmd73:: small receivebuf [%d] expected  [%d]\n",sizeOfPlainDataBuf,len);
		return(errCodeSmallBuffer);
	}
	memcpy(plainData,hsmCmdBuf+4+3,len);
	return(len);
}

int SJL06Cmd74(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version == NULL) || (group == NULL) || (index == NULL) || (lsdata == NULL) || (gcdata == NULL) ||
		(dataLen <= 0) || (dataLen >= 1000))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"74",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,gcdata,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + 3 + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd74:: too long string [%d]!\n",hsmCmdLen + 3 + dataLen);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",dataLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd74:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"75",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd74:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4)
		return(0);
	memcpy(tmpBuf,hsmCmdBuf+4,3);
	tmpBuf[3] = 0;
	if ((((len = atoi(tmpBuf)) > sizeOfEncryptedDataBuf)) || (len < 0))
	{
		UnionUserErrLog("in SJL06Cmd74:: small receivebuf [%d] expected  [%d]\n",sizeOfEncryptedDataBuf,len);
		return(errCodeSmallBuffer);
	}
	memcpy(encryptedData,hsmCmdBuf+4+3,len);
	return(len);
}

int SJL06Cmd75(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version == NULL) || (group == NULL) || (index == NULL) || (lsdata == NULL) || (gcdata == NULL) ||
		(dataLen <= 0) || (dataLen >= 1000))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"75",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,gcdata,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + 3 + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd75:: too long string [%d]!\n",hsmCmdLen + 3 + dataLen);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",dataLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd75:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"76",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd75:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4)
		return(0);
	memcpy(mac,hsmCmdBuf+4,16);
	return(0);
}

int SJL06Cmd77(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version1 == NULL) || (group1 == NULL) || (index1 == NULL) || (lsdata == NULL) || (cardNo == NULL) ||
		(version2 == NULL) || (group2 == NULL) || (index2 == NULL) ||
		(key == NULL) || (random == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"77",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version1,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,version2,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,cardNo,16);
	hsmCmdLen += 16;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd77:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"78",2) != 0) || (ret < 4 ))
	{
		UnionUserErrLog("in SJL06Cmd77:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4)
		return(0);
	if (ret != 4 + 32 + 16)
	{
		UnionUserErrLog("in SJL06Cmd77:: ret = [%d]!\n",ret);
		return(errCodeSJL06MDL_InvalidHsmRetLen);
	}
	memcpy(key,hsmCmdBuf+4,32);
	memcpy(random,hsmCmdBuf+4+32,16);
	return(0);
}

int SJL06Cmd7C(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,
		char *version2,char *group2,char *index2,
		char *lsdata,char *gcdata,
		int macDataHeadLen,char *macDataHead,
		int dataLen,char *data,
		int offset,
		char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version1 == NULL) || (group1 == NULL) || (index1 == NULL) || (lsdata == NULL) || (gcdata == NULL) ||
		(version1 == NULL) || (group1 == NULL) || (index1 == NULL) ||
		(macDataHeadLen <= 0) || (macDataHeadLen >= 1000) || (dataLen <= 0) || (dataLen >= 1000) ||
		(offset < 0) || (offset >= 1000))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"7C",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version1,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version2,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,gcdata,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + 3 + macDataHeadLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd7C:: too long string [%d]!\n",hsmCmdLen + 3 + macDataHeadLen);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",macDataHeadLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,macDataHead,macDataHeadLen);
	if (hsmCmdLen + 3 + dataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd7C:: too long string [%d]!\n",hsmCmdLen + 3 + dataLen);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",dataLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,data,dataLen);
	hsmCmdLen += dataLen;
	if (hsmCmdLen + 3 > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd7C:: too long string [%d]!\n",hsmCmdLen + 3);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",offset);
	hsmCmdLen += 3;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd7C:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"7D",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd7C:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4)
		return(0);
	memcpy(tmpBuf,hsmCmdBuf+4,3);
	tmpBuf[3] = 0;
	if ((((len = atoi(tmpBuf)) > sizeOfEncryptedDataBuf)) || (len < 0))
	{
		UnionUserErrLog("in SJL06Cmd7C:: small receivebuf [%d] expected  [%d]\n",sizeOfEncryptedDataBuf,len);
		return(errCodeSmallBuffer);
	}
	memcpy(encryptedData,hsmCmdBuf+4+3,len);
	return(len);
}

int SJL06Cmd7D(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *lsData,
		char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0,len=0;
	char		tmpBuf[10];
		
	if ((version1 == NULL) || (group1 == NULL) || (index1 == NULL) || (lsdata == NULL) || (lsData == NULL) ||
		(version2 == NULL) || (group2 == NULL) || (index2 == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"7D",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,version1,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsdata,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,version2,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,group2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,index2,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,lsData,16);
	hsmCmdLen += 16;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd7D:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"7E",2) != 0) || (ret < 4 ))
	{
		UnionUserErrLog("in SJL06Cmd7D:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int SJL06Cmd60(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0;
	int		lenOfKey;
		
	if ((pin == NULL) || (bmk == NULL) || (pik == NULL) || (pan == NULL) || (pinByPIK == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"60",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	memcpy(hsmCmdBuf+hsmCmdLen,bmk,3);
	hsmCmdLen += 3;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd60:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,pik,lenOfKey);
	hsmCmdLen += lenOfKey;
	memcpy(hsmCmdBuf+hsmCmdLen,pin,16);
	hsmCmdLen += 16;
	if (type == '3')
	{
		memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
		hsmCmdLen += 16;
	}
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd60:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"61",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd60:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (ret == 4+16)
	{
		memcpy(pinByPIK,hsmCmdBuf+4,16);
		return(16);
	}
	else
		return(0);
}

int SJL06Cmd36(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		vkLen;
	
	if ((vkIndex == NULL) || (vkByMK == NULL))
	{
		UnionUserErrLog("in SJL06Cmd36:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"36",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd36:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"37",2) != 0) || (ret < 8))
	{
		UnionUserErrLog("in SJL06Cmd36:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd36:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	if (ret - 8 > sizeOfVKByMK)
	{
		UnionUserErrLog("in SJL06Cmd36:: small receve buffer!\n");
		return(errCodeSmallBuffer);
	}
	
	memcpy(vkByMK,hsmCmdBuf+4+4,ret - 8);
	
	return(ret - 8);
}
int SJL06Cmd35(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int lenOfVKByMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		vkLen;
	
	if ((vkIndex == NULL) || (vkByMK == NULL) || (lenOfVKByMK < 0))
	{
		UnionUserErrLog("in SJL06Cmd35:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"35",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	if (lenOfVKByMK > sizeof(hsmCmdBuf) - hsmCmdLen)
	{
		UnionUserErrLog("in SJL06Cmd35:: too long vk length [%d]!\n",lenOfVKByMK);
		return(errCodeSmallBuffer);
	}
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfVKByMK);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf+hsmCmdLen,vkByMK,lenOfVKByMK);
	hsmCmdLen += lenOfVKByMK;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd35:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"36",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd35:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd35:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	return(0);
}

int SJL06Cmd2A(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		lenOfKey;
	
	if ((bmkIndex == NULL) || (bmkValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd2A:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"2A",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd2A:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,bmkValue,lenOfKey);
	hsmCmdLen += lenOfKey;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd2A:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"2B",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd2A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int SJL06Cmd31(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		vkLen;
	
	if ((bmkIndex == NULL) || (bmkValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd31:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"31",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd31:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"32",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd31:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd31:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	switch (ret - 4)
	{
		case	32:
		case	48:
			memcpy(bmkValue,hsmCmdBuf+4,ret-4);
			return(ret-4);
		default:
			UnionUserErrLog("in SJL06Cmd31:: ret = [%d]\n",ret);
			return(errCodeSmallBuffer);
	}
}

int SJL06Cmd11(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		lenOfKey;
	
	if ((bmkIndex == NULL) || (bmkValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd11:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"11",2);
	hsmCmdLen = 2;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			//hsmCmdBuf[hsmCmdLen] = 'Y';
			//hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd11:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd11:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"12",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd11:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd11:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(bmkValue,hsmCmdBuf+4,lenOfKey);
	return(lenOfKey);
}

int SJL06Cmd1A(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		lenOfKey;
	
	if ((bmkIndex == NULL) || (keyValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd1A:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"1A",2);
	hsmCmdLen = 2;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd60:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd1A:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"1B",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd1A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd1A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(keyValue,hsmCmdBuf+4,lenOfKey);
	return(lenOfKey);
}

int SJL06Cmd13(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		lenOfKey;
	
	if ((bmkIndex == NULL) || (keyValue == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd13:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"13",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd13:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue,lenOfKey);
	hsmCmdLen += lenOfKey;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd13:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"14",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd13:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd13:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(checkValue,hsmCmdBuf+4,16);
	return(16);
}

int SJL06Cmd15(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue1,char *keyValue2,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		vkLen;
	
	if ((bmkIndex == NULL) || (keyValue1 == NULL) || (keyValue2 == NULL))
	{
		UnionUserErrLog("in SJL06Cmd15:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"15",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue1,16);
	hsmCmdLen += 16;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd15:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"16",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd15:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd15:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(keyValue2,hsmCmdBuf+4,16);
	return(16);
}

int SJL06Cmd16(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		lenOfKey;
	
	if ((bmkIndex == NULL) || (keyValue == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in SJL06Cmd16:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"16",2);
	hsmCmdLen = 2;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd16:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd16:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"17",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd16:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd16:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(keyValue,hsmCmdBuf+4,lenOfKey);
	memcpy(checkValue,hsmCmdBuf+4+lenOfKey,16);
	return(lenOfKey);
}

int SJL06Cmd17(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex1,char *bmkIndex2,char *keyValue1,char *keyValue2,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		lenOfKey;
	
	if ((bmkIndex1 == NULL) || (bmkIndex2 == NULL) || (keyValue1 == NULL) || (keyValue2 == NULL))
	{
		UnionUserErrLog("in SJL06Cmd17:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"17",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex1,3);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex2,3);
	hsmCmdLen += 3;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd17:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue1,lenOfKey);
	hsmCmdLen += lenOfKey;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd17:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"18",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd17:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd17:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(keyValue2,hsmCmdBuf+4,lenOfKey);
	return(lenOfKey);
}

int SJL06Cmd61(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen1,TUnionDesKeyLength keyLen2,char *bmkIndex1,char *bmkIndex2,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		lenOfKey1,lenOfKey2;
	
	if ((bmkIndex1 == NULL) || (bmkIndex2 == NULL) || (keyValue1 == NULL) || (keyValue2 == NULL) || (pin1 == NULL) || 
		(pin2 == NULL) || (pan == NULL))
	{
		UnionUserErrLog("in SJL06Cmd61:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"61",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex1,3);
	hsmCmdLen += 3;
	switch (keyLen1)
	{
		case	con64BitsDesKey:
			lenOfKey1 = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey1 = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey1 = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd61:: invalid key length [%d]\n",keyLen1);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue1,lenOfKey1);
	hsmCmdLen += lenOfKey1;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex2,3);
	hsmCmdLen += 3;
	switch (keyLen2)
	{
		case	con64BitsDesKey:
			lenOfKey2 = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey2 = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey2 = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd61:: invalid key length [%d]\n",keyLen2);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue2,lenOfKey2);
	hsmCmdLen += lenOfKey2;
	memcpy(hsmCmdBuf+hsmCmdLen,pin1,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
	hsmCmdLen += 16;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd61:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"62",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd61:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd61:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pin2,hsmCmdBuf+4,16);
	return(16);
}

int SJL06Cmd80(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1050+1];
	int		hsmCmdLen = 0;
	int		lenOfKey;
	
	if ((bmkIndex == NULL) || (keyValue == NULL) || (macData == NULL) || (mac == NULL) || 
		(macDataLen <= 0) || (macDataLen >= 1000))
	{
		UnionUserErrLog("in SJL06Cmd80:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"80",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen += 3;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd80:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyValue,lenOfKey);
	hsmCmdLen += lenOfKey;
	sprintf(hsmCmdBuf+hsmCmdLen,"%03d",macDataLen);
	hsmCmdLen += 3;
	memcpy(hsmCmdBuf+hsmCmdLen,macData,macDataLen);
	hsmCmdLen += macDataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd80:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"81",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd80:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd80:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	return(16);
}

//added by JiangWei at 2003-11-6
int SJL06Cmd1B(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;
  if(bmkIndex==NULL||keyValue==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd1B:: null pointer!\n");
    return(errCodeParameter);
  }
  
  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

  memcpy(hsmCmdBuf,"1B",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd1B:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"1C",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd1B:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd1B:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(keyValue,hsmCmdBuf+4,32);
  return(32);
}

int SJL06Cmd1C(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue1,char *variant,char *keyValue2,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;
  if ( bmkIndex == NULL || keyValue1 == NULL || variant == NULL || 
       keyValue2 == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd1C:: null pointer!\n");
    return(errCodeParameter);
  }
  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"1C",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,keyValue1,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,variant,2);
  hsmCmdLen+=2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd1C:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"1D",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd1C:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd1C:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(keyValue2,hsmCmdBuf+4,16);
  return(16);
}

int SJL06Cmd21(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if(bmkIndex==NULL||tmkIndex==NULL||tmkValue==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd21:: null pointer!\n");
    return(errCodeParameter);
  }
  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"21",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkValue,16);
  hsmCmdLen+=16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd21:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"22",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd21:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd21:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }

  return(0);
}

int SJL06Cmd32(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if(bmkIndex==NULL||tmkIndex==NULL||tmkValue==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd32:: null pointer!\n");
    return(errCodeParameter);
  }
  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"32",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkIndex,3);
  hsmCmdLen+=3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd32:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"33",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd32:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd32:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(tmkValue,hsmCmdBuf+4,16);
  return(16);
}


int SJL06Cmd62(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *clearPin,char *pan,char *enPin,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if(pinType==NULL||tmkIndex==NULL||clearPin==NULL||
     pan==NULL||enPin==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd62:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

  memcpy(hsmCmdBuf,"62",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,pinType,1);
  hsmCmdLen+=1;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,clearPin,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,pan,16);

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd62:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"63",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd62:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd62:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(enPin,hsmCmdBuf+4,16);
  return(16);
}

int SJL06Cmd63(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if(pinType==NULL||tmkIndex==NULL||bmkIndex==NULL||
     pik==NULL||enPin1==NULL||account==NULL||enPin2==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd63:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

  memcpy(hsmCmdBuf,"63",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,pinType,1);
  hsmCmdLen+=1;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,enPin1,16);
  hsmCmdLen+=16;

  if(memcmp(pinType,"3",1)==0||memcmp(pinType,"4",1)==0||memcmp(pinType,"5",1)==0||
     memcmp(pinType,"6",1)==0||memcmp(pinType,"7",1)==0){
    memcpy(hsmCmdBuf+hsmCmdLen,account,16);
    hsmCmdLen+=16;
  }

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd63:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"64",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd63:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd63:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(enPin2,hsmCmdBuf+4,16);
  return(16);
}

int SJL06Cmd64(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;
  
  if(pinType==NULL||tmkIndex==NULL||bmkIndex==NULL||
     pik==NULL||enPin1==NULL||enPin2==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd64:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

  memcpy(hsmCmdBuf,"64",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,pinType,1);
  hsmCmdLen+=1;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,enPin1,16);
  hsmCmdLen+=16;

  if(memcmp(pinType,"3",1)==0||memcmp(pinType,"4",1)==0||memcmp(pinType,"5",1)==0||
     memcmp(pinType,"6",1)==0||memcmp(pinType,"7",1)==0){
    memcpy(hsmCmdBuf+hsmCmdLen,account,16);
    hsmCmdLen+=16;
  }

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd64:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"65",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd63:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd64:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(enPin2,hsmCmdBuf+4,16);
  return(16);
}

int SJL06Cmd65(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pik,char *enPin1,char *pan,char *tmkIndex,char *enPin2,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if(bmkIndex==NULL||pik==NULL||enPin1==NULL||
     pan==NULL||tmkIndex==NULL||enPin2==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd65:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"65",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,enPin1,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkIndex,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,enPin2,16);
  hsmCmdLen+=16;


  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd65:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"66",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd65:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd65:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  return(0);
}

int SJL06Cmd67(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan,char *enPin2,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if(bmkIndex1==NULL||pik1==NULL||bmkIndex2==NULL||pik2==NULL||
     enPin1==NULL||pan==NULL||enPin2==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd67:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"67",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex1,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik1,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex2,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik2,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,enPin1,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
  hsmCmdLen+=16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd67:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"68",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd67:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd67:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(enPin2,hsmCmdBuf+4,16);
  return(16);
}

int SJL06Cmd68(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode)
{
	int 	ret;
	char	hsmCmdBuf[512+1];
	int 	hsmCmdLen;
	int	lenOfKey;

	if(bmkIndex==NULL||pik==NULL||enPin==NULL||pan==NULL||
     		clearPin==NULL||errCode==NULL)
     	{
   		UnionUserErrLog("in SJL06Cmd68:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"68",2);
	hsmCmdLen=2;
	memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
	hsmCmdLen+=3;
	switch (keyLen)
	{
		case	con64BitsDesKey:
			lenOfKey = 16;
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			lenOfKey = 32;
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = 'Z';
			hsmCmdLen++;
			lenOfKey = 48;
			break;
		default:
			UnionUserErrLog("in SJL06Cmd68:: invalid key length [%d]\n",keyLen);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,pik,lenOfKey);
	hsmCmdLen+=lenOfKey;
	memcpy(hsmCmdBuf+hsmCmdLen,enPin,16);
	hsmCmdLen+=16;
	memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
	hsmCmdLen+=16;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd68:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"69",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd68:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd68:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(clearPin,hsmCmdBuf+4,16);
	return(16);
}

int SJL06Cmd69(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan1,char *pan2,char *enPin2,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if(bmkIndex1==NULL||pik1==NULL||bmkIndex2==NULL||pik2==NULL||
     enPin1==NULL||pan1==NULL||pan2==NULL||enPin2==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd69:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"69",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex1,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik1,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex2,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik2,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,enPin1,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,pan1,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,pan2,16);
  hsmCmdLen+=16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd69:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"6:",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd69:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd69:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(0);
    }
  memcpy(enPin2,hsmCmdBuf+4,16);
  return(16);
}

int SJL06Cmd81(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode)
{
	int	ret;
	char	localMac[16+1];
	
	if ((ret = SJL06Cmd80(hsmSckHDL,pSJL06,keyLen,bmkIndex,mak,macDataLength,macData,localMac,errCode)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd81:: SJL06Cmd80\n");
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
		return(0);
	if (strncmp(localMac,mac,16) != 0)
	{
		UnionUserErrLog("in SJL06Cmd81:: localmac != mac\n");
		memcpy(errCode,"10",2);
		return(0);
	}
	else
		return(0);
}

int SJL06Cmd84(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *mak,char *enPin1,char *pan,char *addInfo,int macDataLength,char *macData,char *enPin2,char *mac,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  char hsmCmdLen;
  char tmp[10];
  if(pinType==NULL||tmkIndex==NULL||bmkIndex==NULL||pik==NULL||mak==NULL||
     macData==NULL||enPin2==NULL||mac==NULL||errCode==NULL){
    UnionUserErrLog("in SJL06Cmd84:: null pointer!\n");
    return(errCodeParameter);
  }
  if(macDataLength>999){
    UnionUserErrLog("in SJL06Cmd84::mac data length =[%d]\n",macDataLength);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"84",2);
  hsmCmdLen=2;
  memcpy(hsmCmdBuf+hsmCmdLen,pinType,1);
  hsmCmdLen+=1;
  memcpy(hsmCmdBuf+hsmCmdLen,tmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,bmkIndex,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,pik,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,mak,16);
  hsmCmdLen+=16;
  memcpy(hsmCmdBuf+hsmCmdLen,enPin1,16);
  hsmCmdLen+=16;
  if(memcmp(pinType,"3",1)==0){
    memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
    hsmCmdLen+=16;
  }else if(memcmp(pinType,"4",1)==0||memcmp(pinType,"5",1)==0||
	   memcmp(pinType,"6",1)==0||memcmp(pinType,"7",1)==0){
    memcpy(hsmCmdBuf+hsmCmdLen,addInfo,16);
    hsmCmdLen+=16;
  }
  sprintf(tmp,"%03d",macDataLength);
  memcpy(hsmCmdBuf+hsmCmdLen,tmp,3);
  hsmCmdLen+=3;
  memcpy(hsmCmdBuf+hsmCmdLen,macData,macDataLength);
  hsmCmdLen+=16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd84:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"85",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd84:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd84:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(enPin2,hsmCmdBuf+4,16);
  memcpy(mac,hsmCmdBuf+4+16,16);
  return(32);
}

int SJL06Cmd85(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *mak1,char *bmkIndex2,char *mak2,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *mac2,char *errCode){
  int ret;
  char hsmCmdBuf[512 + 1];
  int hsmCmdLen;
  char tmp[10];

  if (bmkIndex1 == NULL || mak1 == NULL || bmkIndex2 == NULL || mak2 == NULL || mac1 == NULL ||
     macData1 == NULL || macData2 == NULL || mac2 == NULL || errCode == NULL){
    UnionUserErrLog("in SJL06Cmd85:: null pointer!\n");
    return(errCodeParameter);
  }
  if (macDataLength1 > 999 || macDataLength2 > 999){
    UnionUserErrLog("in SJL06Cmd85::mac data length one=[%d]\n",macDataLength1);
    UnionUserErrLog("in SJL06Cmd85::mac data length two=[%d]\n",macDataLength2);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"85",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex1,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,mak1,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex2,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,mak2,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,mac1,8);
  hsmCmdLen += 8;

  sprintf(tmp,"%03d",macDataLength1);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData1,macDataLength1);
  hsmCmdLen += macDataLength1;

  memset(tmp,0,sizeof(tmp));
  sprintf(tmp,"%03d",macDataLength2);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData2,macDataLength2);
  hsmCmdLen += macDataLength2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd85:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"86",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd85:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd85:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac2,hsmCmdBuf+4,16);
  return (16);
}

int SJL06Cmd86(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *mak1,char *bmkIndex2,char *pik2,char *mak2,char *enPin1,char *pan,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *enPin2,char *mac2,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];
  if (bmkIndex1 == NULL || pik1 == NULL || mak1 == NULL ||
      bmkIndex2 == NULL || pik2 == NULL || mak2 == NULL ||
      enPin1 == NULL || mac1 == NULL || macData1 == NULL ||
      macData2 == NULL || enPin2 == NULL || mac2 == NULL || 
      errCode == NULL || pan == NULL){
    UnionUserErrLog("in SJL06Cmd86:: null pointer!\n");
    return(errCodeParameter);
  }

  if (macDataLength1 > 999 || macDataLength2 > 999){
    UnionUserErrLog("in SJL06Cmd86::mac data length one=[%d]\n",macDataLength1);
    UnionUserErrLog("in SJL06Cmd86::mac data length two=[%d]\n",macDataLength2);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"86",2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex1,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,pik1,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,mak1,16);
  hsmCmdLen += 16;

  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex2,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,pik2,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,mak2,16);
  hsmCmdLen += 16;

  memcpy(hsmCmdBuf + hsmCmdLen,enPin1,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pan,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,mac1,8);
  hsmCmdLen += 8;

  sprintf(tmp,"%03d",macDataLength1);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData1,macDataLength1);
  hsmCmdLen += macDataLength1;

  memset(tmp,0,sizeof(tmp));
  sprintf(tmp,"%03d",macDataLength2);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData2,macDataLength2);
  hsmCmdLen += macDataLength2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd86:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"87",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd86:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd86:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(enPin2,hsmCmdBuf + 4,16);
  memcpy(mac2,hsmCmdBuf + 4 + 16,16);
  return (32);
}

int SJL06Cmd01(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode){
  int ret;
  char hsmCmdBuf[32+1];
  int hsmCmdLen;

  if ( errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd01:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"01",2);
  hsmCmdLen = 2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd01:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"02",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd01:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd01:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return (0);
}

int SJL06Cmd03(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *errCode){
  int ret;
  char hsmCmdBuf[32+1];
  int hsmCmdLen;

  if ( bmkIndex == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd03:: null point!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"03",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex,3);
  hsmCmdLen += 3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd03:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"04",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd03:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd03:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return (0);
}

int SJL06Cmd04(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *errCode){
  int ret;
  char hsmCmdBuf[32+1];
  int hsmCmdLen;

  if ( tmkIndex == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd04:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"04",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd04:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"05",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd04:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd04:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return (0);
}

int SJL06Cmd90(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;
  int len;
  if ( bmkIndex == NULL || cvka == NULL || cvkb == NULL ||
       data == NULL || cvv == NULL ||errCode == NULL ) {
    UnionUserErrLog("in SJL06Cmd90:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"90",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,cvka,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,cvkb,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,data,32);
  hsmCmdLen += 32;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd90:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"91",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd90:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd90:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  if ((len = ret - 4) > 16)
  	len = 16;
  memcpy(cvv,hsmCmdBuf+4,len);
  return (len);
}

int SJL06Cmd91(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  
  if ( bmkIndex == NULL || cvka == NULL || cvkb == NULL ||
       data == NULL || cvv == NULL ||errCode == NULL ) {
    UnionUserErrLog("in SJL06Cmd91:: null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"91",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,cvv,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,cvka,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,cvkb,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,data,32);
  hsmCmdLen += 32;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd91:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"92",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd91:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd91:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return (0);
}

int SJL06Cmd92(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if ( bmkIndex == NULL || pvkIndex == NULL || pvka == NULL ||
       pvkb == NULL || pik == NULL || pinBlock == NULL ||
       pan == NULL || pvv == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd92::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"92",2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,pvkIndex,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,pvka,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pvkb,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pik,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pinBlock,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pan,16);
  hsmCmdLen += 16;


  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd92:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"93",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd92:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd92:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(pvv,hsmCmdBuf+4,4);
  return (4);
}

int SJL06Cmd93(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode){
  int ret;
  char hsmCmdBuf[512+1];
  int hsmCmdLen;

  if ( bmkIndex == NULL || pvkIndex == NULL || pvka == NULL ||
       pvkb == NULL || pik == NULL || pinBlock == NULL ||
       pan == NULL || pvv == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd93::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"93",2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,pvkIndex,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,pvv,4);
  hsmCmdLen += 4;
  memcpy(hsmCmdBuf + hsmCmdLen,pvka,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pvkb,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pik,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pinBlock,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,pan,16);
  hsmCmdLen += 16;


  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd93:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"94",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd93:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd93:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return (0);
}

int SJL06Cmd7A(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,int dataLength,char *flag,char *data,char *enData,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( bmkIndex == NULL || keyValue == NULL || flag == NULL ||
       data == NULL || enData == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd::null pointer!\n");
    return(errCodeParameter);
  }

  if ( dataLength > 999 ){
    UnionUserErrLog("in SJL06Cmd:: data length=[%d]\n",dataLength);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"7A",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,bmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,keyValue,16);
  hsmCmdLen += 16;
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,flag,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,data,dataLength);
  hsmCmdLen += dataLength;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd7A:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"7B",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd7A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd7A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(enData,hsmCmdBuf+4,ret-4);
  return (ret-4);
}

//IC command added by jiangwei
int SJL06CmdE0(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode){
  int ret;
  char hsmCmdBuf[128+1];
  int hsmCmdLen;

  if ( version == NULL || group == NULL || index == NULL || key == NULL ||
       keyCheckValue == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdE0::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"E0",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdE0:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"E1",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdE0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdE0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(key,hsmCmdBuf+4,32);
  memcpy(keyCheckValue,hsmCmdBuf+4+32,4);
  return 36;
}

int SJL06CmdE4(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *errCode){
  int ret;
  char hsmCmdBuf[32+1];
  int hsmCmdLen;

  if ( version == NULL || group == NULL || index == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdE4::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"E4",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf,index,2);
  hsmCmdLen += 2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdE4:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"E5",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdE4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdE4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
   return 0;
}


int SJL06CmdE6(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode){
  int ret;
  char hsmCmdBuf[128+1];
  int hsmCmdLen;

  if ( version == NULL || group == NULL || index == NULL ||
       key == NULL || keyCheckValue == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdE6::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

  memcpy(hsmCmdBuf,"E6",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdE6:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"E7",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdE6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdE6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(key,hsmCmdBuf+4,32);
  memcpy(keyCheckValue,hsmCmdBuf+4+32,4);
  return 36;
}

int SJL06CmdD0(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode){
  int ret;
  char hsmCmdBuf[48+1];
  int hsmCmdLen;

  if ( tmkIndex == NULL || key == NULL || keyCheckValue == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdD0::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"D0",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdD0:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"D1",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdD0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdD0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(key,hsmCmdBuf+4,16);
  memcpy(keyCheckValue,hsmCmdBuf+4+16,4);
  return 20;
}

int SJL06CmdD2(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode){
  int ret;
  char hsmCmdBuf[48+1];
  int hsmCmdLen;

  if ( tmkIndex == NULL || key == NULL || keyCheckValue == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdD2::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"D2",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,key,16);
  hsmCmdLen += 16;
  memcpy(hsmCmdBuf + hsmCmdLen,keyCheckValue,4);
  hsmCmdLen += 4;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdD2:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"D3",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdD2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdD2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }

  return 0;
}

int SJL06CmdD4(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *errCode){
  int ret;
  char hsmCmdBuf[32+1];
  int hsmCmdLen;

  if ( tmkIndex == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdD4::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"D4",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdD4:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"D5",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdD4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdD4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return 0;
}

int SJL06CmdD6(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode){
  int ret;
  char hsmCmdBuf[48+1];
  int hsmCmdLen;

  if ( tmkIndex == NULL || key == NULL || keyCheckValue == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdD4::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"D6",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdD6:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"D7",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdD6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdD6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(key,hsmCmdBuf+4,16);
  memcpy(keyCheckValue,hsmCmdBuf+4+16,4);
  return 20;
}

int SJL06Cmd10(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version ,char *group,char *index,char *lsData,char *errCode){
  int ret;
  char hsmCmdBuf[64+1];
  int hsmCmdLen;

  if ( desType == NULL || version == NULL || group == NULL ||
       index == NULL || lsData == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd10::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"10",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,lsData,16);
  hsmCmdLen += 16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd10:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"11",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd10:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd10:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return 0;
}

int SJL06Cmd12(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *gcData,char *errCode){
  int ret;
  char hsmCmdBuf[32+1];
  int hsmCmdLen;

  if ( desType == NULL || gcData == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd12::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"12",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,gcData,16);
  hsmCmdLen += 16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd12:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"13",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd12:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd12:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return 0;
}

int SJL06Cmd14(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *tmkIndex,char *txKey,char *errCode){
  int ret;
  char hsmCmdBuf[32+1];
  int hsmCmdLen;


  if ( desType == NULL || tmkIndex == NULL || txKey == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd14::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"14",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd14:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"15",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd14:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd14:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(txKey,hsmCmdBuf+4,16);
  return 16;
}

int SJL06Cmd20(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd20::null pointer!\n");
    return(errCodeParameter);
  }

  if ( dataLength > 999 ){
    UnionUserErrLog("in SJL06Cmd20::data length=[%d]\n",dataLength);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"20",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd20:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"21",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd20:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd20:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac,hsmCmdBuf+4,8);
  return 8;
}

int SJL06Cmd22(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd22::null pointer!\n");
    return(errCodeParameter);
  }

  if ( dataLength > 999 ){
    UnionUserErrLog("in SJL06Cmd22::data length=[%d]\n",dataLength);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"22",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd22:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"23",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd22:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd22:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac,hsmCmdBuf+4,8);
  return 8;
}

int SJL06Cmd24(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd24::null pointer!\n");
    return(errCodeParameter);
  }

  if ( dataLength > 999 ){
    UnionUserErrLog("in SJL06Cmd24::data length=[%d]\n",dataLength);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"24",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd24:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"25",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd24:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd24:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac,hsmCmdBuf+4,8);
  return 8;
}

int SJL06Cmd26(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *keyType,char *macMode,char *tmkIndex,char *txKey,int dataLength,char *inputData,char *mac,char *errCode){

  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || keyType == NULL || macMode == NULL ||
       tmkIndex == NULL || txKey == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd26::null pointer!\n");
    return(errCodeParameter);
  }

  if ( dataLength > 999 ){
    UnionUserErrLog("in SJL06Cmd26::data length=[%d]\n",dataLength);
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"26",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,keyType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,macMode,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,txKey,16);
  hsmCmdLen += 16;
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd26:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"27",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd26:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd26:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac,hsmCmdBuf+4,8);
  return 8;
}

int SJL06CmdC0(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *tmkIndex,char *inputData,char *outputData,char *errCode){
  int ret;
  char hsmCmdBuf[48+1];
  int hsmCmdLen;

  if ( desType == NULL || tmkIndex == NULL || inputData == NULL ||
       outputData == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdC0::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"C0",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,16);
  hsmCmdLen += 16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdC0:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"C1",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdC0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdC0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(outputData,hsmCmdBuf+4,16);
  return 16;
}

int SJL06CmdC2(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *inputData,char *outputData,char *errCode){
  int ret;
  char hsmCmdBuf[48+1];
  int hsmCmdLen;

  if ( desType == NULL || inputData == NULL ||
       outputData == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdC2::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memcpy(hsmCmdBuf,"C2",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,16);
  hsmCmdLen += 16;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdC2:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"C3",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdC2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdC2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(outputData,hsmCmdBuf+4,16);
  return 16;
}

int SJL06Cmd50(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *inputData,char *mac,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || version == NULL || group == NULL || index == NULL ||
       keyFlag == NULL || lsData == NULL || inputData == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd50::null pointer!\n");
    return(errCodeParameter);
  }

  if ( memcmp(keyFlag,"1",1) && gcData == NULL ){
    UnionUserErrLog("in SJL06Cmd50::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"50",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,keyFlag,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,lsData,16);
  hsmCmdLen += 16;
  
  if ( memcmp(keyFlag,"1",1)){
    memcpy(hsmCmdBuf + hsmCmdLen,gcData,16);
    hsmCmdLen += 16;
  }

  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd50:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"51",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd50:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd50:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac,hsmCmdBuf+4,8);
  return 8;
}

int SJL06Cmd52(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *macData,char *mac,char *errCode){
int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || version == NULL || group == NULL || index == NULL ||
       keyFlag == NULL || lsData == NULL || macData == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd52::null pointer!\n");
    return(errCodeParameter);
  }

  if ( memcmp(keyFlag,"1",1) && gcData == NULL ){
    UnionUserErrLog("in SJL06Cmd52::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"52",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,keyFlag,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,lsData,16);
  hsmCmdLen += 16;
  
  if ( memcmp(keyFlag,"1",1)){
    memcpy(hsmCmdBuf + hsmCmdLen,gcData,16);
    hsmCmdLen += 16;
  }

  memcpy(hsmCmdBuf + hsmCmdLen,mac,8);
  hsmCmdLen += 8;

  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd52:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"53",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd52:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd52:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  return 0;
}

int SJL06Cmd54(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength1,char *macData1,char *mac1,
	       int dataLength2,char *macData2,char *mac2,char *errCode){
  int ret;
  char hsmCmdBuf[4092+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || version == NULL || group == NULL || index == NULL ||
       keyFlag == NULL || lsData == NULL || macData1 == NULL || mac1 == NULL || 
       macData2 == NULL || mac2 == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd54::null pointer!\n");
    return(errCodeParameter);
  }

  if ( memcmp(keyFlag,"1",1) && gcData == NULL ){
    UnionUserErrLog("in SJL06Cmd54::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"54",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,keyFlag,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,lsData,16);
  hsmCmdLen += 16;
  
  if ( memcmp(keyFlag,"1",1)){
    memcpy(hsmCmdBuf + hsmCmdLen,gcData,16);
    hsmCmdLen += 16;
  }

  sprintf(tmp,"%03d",dataLength1);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData1,dataLength1*2);
  hsmCmdLen += dataLength1*2;

  memset(tmp,0,sizeof(tmp));
  sprintf(tmp,"%03d",dataLength2);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData2,dataLength2*2);
  hsmCmdLen += dataLength2*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd54:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"55",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd54:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd54:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac2,hsmCmdBuf+4,8);
  return 8;
}

int SJL06Cmd56(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || version == NULL || group == NULL || index == NULL ||
       lsData == NULL || macData == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd56::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"56",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;
  
  memcpy(hsmCmdBuf + hsmCmdLen,lsData,16);
  hsmCmdLen += 16;
  
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd56:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"57",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd56:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd56:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac,hsmCmdBuf+4,8);
  return 8;
}

int SJL06Cmd58(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || version == NULL || group == NULL || index == NULL ||
       lsData == NULL || macData == NULL || mac == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd58::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"58",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;

  memcpy(hsmCmdBuf + hsmCmdLen,lsData,16);
  hsmCmdLen += 16;

  memcpy(hsmCmdBuf + hsmCmdLen, mac,8);
  hsmCmdLen += 8;
  
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,macData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd58:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"59",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd58:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd58:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
 
  return 0;
}

int SJL06Cmd70(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *enFlag,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *inputData,char *mac,char *outputData,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || version == NULL || group == NULL || index == NULL ||
       lsData == NULL || inputData == NULL || mac == NULL || 
       outputData == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06Cmd70::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"70",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,version,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,group,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,index,2);
  hsmCmdLen += 2;
  memcpy(hsmCmdBuf + hsmCmdLen,lsData,16);
  hsmCmdLen += 16;
  
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06Cmd70:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"71",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06Cmd70:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06Cmd70:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }
  memcpy(mac,hsmCmdBuf+4,8);
  if ( memcmp(desType,"1",1) ){
    memcpy(outputData,hsmCmdBuf+4+8,sizeof(hsmCmdBuf)-12);
    return (sizeof(hsmCmdBuf)-4);
  }
  return 8;
}

int SJL06CmdC4(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *enFlag,char *tmkIndex,char *txKey,
	       int dataLength,char *inputData,char *outputData,char *errCode){
  int ret;
  char hsmCmdBuf[2048+1];
  int hsmCmdLen;
  char tmp[10];

  if ( desType == NULL || enFlag == NULL || tmkIndex == NULL ||
       txKey == NULL || inputData == NULL || outputData == NULL || errCode == NULL ){
    UnionUserErrLog("in SJL06CmdC4::null pointer!\n");
    return(errCodeParameter);
  }

  memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
  memset(tmp,0,sizeof(tmp));

  memcpy(hsmCmdBuf,"C4",2);
  hsmCmdLen = 2;
  memcpy(hsmCmdBuf + hsmCmdLen,desType,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,enFlag,1);
  hsmCmdLen += 1;
  memcpy(hsmCmdBuf + hsmCmdLen,tmkIndex,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,txKey,16);
  hsmCmdLen += 16;
  
  sprintf(tmp,"%03d",dataLength);
  memcpy(hsmCmdBuf + hsmCmdLen,tmp,3);
  hsmCmdLen += 3;
  memcpy(hsmCmdBuf + hsmCmdLen,inputData,dataLength*2);
  hsmCmdLen += dataLength*2;

  if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
    {
      UnionUserErrLog("in SJL06CmdC4:: UnionLongConnSJL06Cmd!\n");
      return(ret);
    }
  hsmCmdBuf[ret] = 0;
  if ((memcmp(hsmCmdBuf,"C5",2) != 0) || (ret < 4))
    {
      UnionUserErrLog("in SJL06CmdC4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return(errSJL06Abnormal);
    }
  memcpy(errCode,hsmCmdBuf+2,2);
  if (memcmp(errCode,"00",2) != 0)
    {
      UnionUserErrLog("in SJL06CmdC4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
      return (0);
    }

  memcpy(outputData,hsmCmdBuf+4,sizeof(hsmCmdBuf)-4);
  return (sizeof(hsmCmdBuf)-4);

}
