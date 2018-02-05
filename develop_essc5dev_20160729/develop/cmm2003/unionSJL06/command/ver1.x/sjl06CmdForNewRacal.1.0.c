// Author:	Wolfgang Wang
// Date:	2003/10/09

// 2004-3-9，陈家梅，在1.0版本的基础上修改，增加了拼装以下指令的函数
// DG、CA、JE、JC、JG、JA、EE、DE、BA、NG、DA、EA、DC、EC、BC、BE、CW、CY、EW、EY

// 2006-4-9，王纯军增加了
// MU指令

#define _RacalCmdForNewRacal_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sjl06.h"
#include "sjl06Protocol.h"
#include "sjl06Cmd.h"
#include "3DesRacalSyntaxRules.h"
#include "unionDesKey.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "UnionLog.h"

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

/* 2007/5/14 增加 */
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
		UnionUserErrLog("in SJL06Cmd3C:: UnionLongConnSJL06Cmd!\n");
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

	UnionProgramerMemLog("in RacalCmd38:: req =",hsmCmdBuf,hsmCmdLen);
	
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
	int   lenOfPK = 0;  //add in 2012.01.13
		
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
		UnionUserErrLog("in SJL06Cmd34:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd34:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	// Get PK
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	vkLen = atoi(tmpBuf);
	UnionDebugLog("in SJL06Cmd34:: vkLen = [%04d] derPKLen = [%04d]\n",vkLen,ret-4-4-vkLen);
	UnionMemLog("in SJL06Cmd34::",hsmCmdBuf+4+4+vkLen,ret-4-4-vkLen);
	/*
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (lenOfVK / 8 * 2 > sizeOfPK)
	{
		UnionUserErrLog("in SJL06Cmd34:: sizeOfPK [%d] too small!\n",sizeOfPK);
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
			UnionUserErrLog("in SJL06Cmd34:: lenOfVK error! [%d]\n",lenOfVK);
			return(errCodeSmallBuffer);
	}	
	bcdhex_to_aschex(hsmCmdBuf+4+4+vkLen+tmpLen,lenOfVK/8,pk);
	//memset(tmpBuf,0,sizeof(tmpBuf));
	//bcdhex_to_aschex(hsmCmdBuf+4+4+vkLen,ret-4-4-vkLen,tmpBuf);
	//UnionLog("in SJL06Cmd34:: pk = [%s]\n",tmpBuf);
	return(lenOfVK/8*2);
	*/
	//modify by hzh in 2012.01.13, 解决加密机升级后，公钥取出偏移的问题. (如30818902818100..., 左边多了00,右边末尾字节丢失) 
	if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)hsmCmdBuf+4+4+vkLen,ret-4-4-vkLen,pk,&lenOfPK,sizeOfPK)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd34:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
		UnionMemLog("in SJL06Cmd34:: hsmReturnStr::",hsmCmdBuf,ret);
		return(ret);
	}
	return lenOfPK;
	//modify end
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
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd41:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;
	
	memcpy(hsmCmdBuf+hsmCmdLen,pan,16);
	hsmCmdLen += 16;
	if (hsmCmdLen + lenOfPinByPK > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd41:: lenOfPinByPK [%d] too long!\n",lenOfPinByPK);
		return(errCodeSmallBuffer);
	}
		
	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;
	UnionMemLog("in SJL06Cmd41::",hsmCmdBuf,hsmCmdLen);
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
/* 2007/5/14 增加结束 */

/*
功能：将由公钥加密的PIN转换成ANSI9.8标准

输入域	长度	类型	说　明
命令代码	2	A	值"43"
私钥索引	2	N	指定的私钥，用于解密PIN数据密文
密钥类型	1	N	1：TPK  2：ZPK
ZPK/TPK	16/1A+32/1A+48	H	用于加密PIN的密钥
PIN类型	1	N	1：ANSI9.8
PAN	16	H	用户主帐号
PIN密文	n	B	经公钥加密的PIN数据密文


输出域	长度	类型	说　明
响应代码	2	A	值"44"
错误代码	2	H	
PIN密文	16	H	返回的PIN密文--ANSI9.8
*/
int SJL06Cmd43(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char type,char *zpk,char pinType,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024];
	int		hsmCmdLen = 0;
	int		panLen;
		
	if ((vkIndex == NULL) || (zpk == NULL) || (pan == NULL) || (pinByPK == NULL) || (pinByZPK == NULL))
		return(errCodeParameter);

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"43",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd43:: UnionGenerateX917RacalKeyString [%s]\n",zpk);
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
		UnionUserErrLog("in SJL06Cmd43:: lenOfPinByPK [%d] too long!\n",lenOfPinByPK);
		return(errCodeSmallBuffer);
	}
		
	memcpy(hsmCmdBuf+hsmCmdLen,pinByPK,lenOfPinByPK);
	hsmCmdLen += lenOfPinByPK;
	UnionMemLog("in SJL06Cmd43::",hsmCmdBuf,hsmCmdLen);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd43:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"44",2) != 0) || ((ret != 4) && (ret != 4+16)))
	{
		UnionUserErrLog("in SJL06Cmd43:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
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


/*
2007/4/11,王纯军，增加
50指令
功能：用EDK密钥加解密数据，如果是解密状态，则必须在授权下才能处理，否则报错。

输  入  消  息  格  式

输入域		长度	类型	说　明
消息头		m	A	
命令代码	2	A	值"50"
Flag		1	N	0：加密
			1：解密
EDK		16 or 32
		1A+32 or 1A+48	
			H	LMK24-25加密
DATA_length	4	N	输入数据字节数（8的倍数）范围：0008-4096
DATA		n*2	H	待加密或解密的数据（以BCD码表示）

输  出  消  息  格  式

输出域	长度	类型	说　明
消息头		M	A	
响应代码	2	A	值"51"
错误代码	2	N	
DATA		n*2	H	输出结果

*/
int RacalCmd50(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *edk,int lenOfData,char *indata,char *outdata,int sizeOfOutData,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8096+40];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		retLen;
	int		offset;
	int		keyLen;
		
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"50",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	switch (keyLen = strlen(edk))
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
			UnionUserErrLog("in RacalCmd50:: edk [%s] length error!\n",edk);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,edk,keyLen);
	hsmCmdLen += keyLen;
	if ((lenOfData % 8 != 0) || (lenOfData <= 0) || (lenOfData > 8096))
	{
		UnionUserErrLog("in RacalCmd50:: lenOfData [%04d] error!\n",lenOfData);
		return(errCodeParameter);
	}
	switch (flag)
	{
		case	'0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData);
			hsmCmdLen += 4;
			bcdhex_to_aschex(indata,lenOfData,hsmCmdBuf+hsmCmdLen);
			hsmCmdLen += (lenOfData * 2);
			break;
		case	'1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04d",lenOfData/2);
			hsmCmdLen += 4;
			memcpy(hsmCmdBuf+hsmCmdLen,indata,lenOfData);
			hsmCmdLen += lenOfData;
			break;
		default:
			UnionUserErrLog("in RacalCmd50:: flag [%c] error!\n",flag);
			return(errCodeParameter);
	}
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmd50:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"51",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmd50:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (strcmp(errCode,"00") != 0)
	{
		//UnionUserErrLog("in RacalCmd50:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	retLen = ret - 4;
	switch (flag)
	{
		case	'0':
			if (retLen > sizeOfOutData)
			{
				UnionUserErrLog("in RacalCmd50:: sizeOfOutData [%04d] < retLen [%04d]\n",sizeOfOutData,retLen);
				return(errCodeSmallBuffer);
			}
			memcpy(outdata,hsmCmdBuf+4,retLen);
			return(retLen);
		case	'1':
			if (retLen / 2 > sizeOfOutData)
			{
				UnionUserErrLog("in RacalCmd50:: sizeOfOutData [%04d] < retLen [%04d]\n",sizeOfOutData,retLen/2);
				return(errCodeSmallBuffer);
			}
			aschex_to_bcdhex(hsmCmdBuf+4,retLen,outdata);
			return(retLen/2);
		default:
			return(errCodeParameter);
	}
}

// 20051206，王纯军增加
// 将ZMK加密的密钥转换为LMK加密的ZAK/ZEK
int RacalCmdFK(int hsmSckHDL,PUnionSJL06 pSJL06,char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		keyLen;
	int		offset;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"FK",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	switch (keyLen = strlen(zmk))
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
			UnionUserErrLog("in RacalCmdFK:: zmk [%s] length error!\n",zmk);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,zmk,keyLen);
	hsmCmdLen += keyLen;
	switch (keyLen = strlen(keyByZMK))
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
			UnionUserErrLog("in RacalCmdFK:: keyByZMK [%s] length error!\n",keyByZMK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyByZMK,keyLen);
	hsmCmdLen += keyLen;
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	switch (keyLen)
	{
		case	16:
			memcpy(hsmCmdBuf+hsmCmdLen,"ZZ0",3);
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"XX0",3);
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"YY0",3);
			break;
	}
	hsmCmdLen += 3;
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdFK:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"FL",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdFK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdFK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	switch (keyLen)
	{
		case	16:
			offset = 0;
			break;
		case	32:
		case	48:
			offset = 1;
			break;
		default:
			UnionUserErrLog("in RacalCmdFK:: keyByZMK [%s] length error!\n",keyByZMK);
			return(errCodeParameter);
	}
	if (offset + keyLen + 4 > ret)
	{
		UnionUserErrLog("in RacalCmdFK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(keyByLMK,hsmCmdBuf+4+offset,keyLen);
	memcpy(checkValue,hsmCmdBuf+4+offset+keyLen,ret-4-offset-keyLen);
	return(keyLen);
}
/*
输  出  消  息  格  式
输入域	长度	类型	说　明
命令代码		2A	值为"TI"。
源KEY标志	1	N	1：TPK           2：ZPK
源KEY	16
1A+32
1A+48	H	当KEY标志为1时为TPK，LMK对（14-15）下加密；
当KEY标志为2时为ZPK，LMK对（06-07）下加密。
目的KEY标志	1	N	1：TPK           2：ZPK
目的KEY	16
1A+32
1A+48	H	当KEY标志为1时为TPK，LMK对（14-15）下加密；
当KEY标志为2时为ZPK，LMK对（06-07）下加密。
源PIN块	16	H	源ZPK下加密的源PIN块。
源PIN格式	1	N	1：ANSI9.8格式    2：IBM格式
源PAN	16	H	用户主帐号，仅用到最右12位; 当源PIN格式为1时有此域
目的PIN格式	1	N	1：ANSI9.8格式    2：IBM格式
目的PAN	16	H	用户主帐号，仅用到最右12位; 当源PIN格式为1时有此域

输  出  消  息  格  式
输出域	长度	类型	说　明
响应代码	2	A	值"TJ"
错误代码	2	H	
PIN密文	16	H	TPK或ZPK下加密
*/
int RacalCmdTI(int hsmSckHDL,PUnionSJL06 pSJL06,char zpk1type,int zpk1Length,char *zpk1,char zpk2type,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo1,int lenOfAccNo1,char *accNo2,int lenOfAccNo2,
		char *pinFormat2,char *pinBlockByZPK2,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((zpk1 == NULL) || (zpk2 == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || (accNo1 == NULL) ||
		(pinBlockByZPK1 == NULL) || (pinBlockByZPK2 == NULL) || (accNo2 == NULL))
	{
		UnionUserErrLog("in RacalCmdTI:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"TI",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = zpk1type;
	hsmCmdLen++;
	if ((ret = UnionGenerateX917RacalKeyString(zpk1Length,zpk1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionGenerateX917RacalKeyString for [%s]\n",zpk1);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = zpk2type;
	hsmCmdLen++;
	if ((ret = UnionGenerateX917RacalKeyString(zpk2Length,zpk2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionGenerateX917RacalKeyString for [%s]\n",zpk2);
		return(ret);
	}
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK1,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,"0000",4);
	hsmCmdLen += 4;
	if ((ret = UnionForm12LenAccountNumber(accNo1,lenOfAccNo1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionForm12LenAccountNumber for [%s]\n",accNo1);
		return(ret);
	}
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,"0000",4);
	hsmCmdLen += 4;
	if ((ret = UnionForm12LenAccountNumber(accNo2,lenOfAccNo2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionForm12LenAccountNumber for [%s]\n",accNo2);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"TJ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdTI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdTI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlockByZPK2,hsmCmdBuf+4,16);
	return(16);
}

// 将一个ZMK加密的密钥转换为LMK加密
int RacalCmdA6(int hsmSckHDL,PUnionSJL06 pSJL06,
	TUnionDesKeyType keyType,char *zmk,char *keyByZmk,
	char *keyByLmk,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		keyByLmkStrLen;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A6",2);
	hsmCmdLen = 2;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(keyByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionCaculateHsmKeyKeyScheme(strlen(keyByZmk),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionCaculateHsmKeyKeyScheme [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen++;
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A7",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	if ((keyByLmkStrLen = UnionReadKeyFromRacalKeyString(hsmCmdBuf+4,ret-4,keyByLmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionReadKeyFromRacalKeyString! hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(keyByLmkStrLen);
	}
	if (keyByLmkStrLen >= ret)	// 没有校验值
		return(0);
	if (ret - keyByLmkStrLen > 16)
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,16);
	else
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,ret-keyByLmkStrLen-4);
	return(0);
}

// 将一个ZMK加密的密钥转换为LMK加密,支持国产算法SM1和SFF33
int RacalCmdA6_SM1_SFF33(int hsmSckHDL,PUnionSJL06 pSJL06, char *algFlag,
	TUnionDesKeyType keyType,char *zmk,char *keyByZmk,
	char *keyByLmk,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		keyByLmkStrLen;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A6",2);
	hsmCmdLen = 2;

	if (algFlag != NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, algFlag, 1);
		hsmCmdLen += 1;
	}

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: UnionPutKeyIntoRacalKeyString [%s]!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(keyByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionCaculateHsmKeyKeyScheme(strlen(keyByZmk),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: UnionCaculateHsmKeyKeyScheme [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen++;
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A7",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	if ((keyByLmkStrLen = UnionReadKeyFromRacalKeyString(hsmCmdBuf+4,ret-4,keyByLmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6_SM1_SFF33:: UnionReadKeyFromRacalKeyString! hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(keyByLmkStrLen);
	}
	if (keyByLmkStrLen >= ret)	// 没有校验值
		return(0);
	if (ret - keyByLmkStrLen > 16)
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,16);
	else
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,ret-keyByLmkStrLen-4);
	return(0);
}

// 生成银联MAC的指令
int RacalCmdMU(int hsmSckHDL,PUnionSJL06 pSJL06,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((key == NULL) || (msg == NULL) || (mac == NULL) || (pSJL06 == NULL))
	{
		UnionUserErrLog("in RacalCmdMU:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"MU0",3);
	hsmCmdLen = 3;
	hsmCmdBuf[hsmCmdLen] = keyType;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = keyLength;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = msgType;
	++hsmCmdLen;
	switch (keyLength)
	{
		case	'0':
			memcpy(hsmCmdBuf+hsmCmdLen,key,16);
			hsmCmdLen += 16;
			break;
		case	'1':
			memcpy(hsmCmdBuf+hsmCmdLen,key,32);
			hsmCmdLen += 32;
			break;
		case	'2':
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen += 1;
			memcpy(hsmCmdBuf+hsmCmdLen,key,48);
			hsmCmdLen += 48;
			break;
		default:
			UnionUserErrLog("in RacalCmdMU:: wrong keyLength flag [%c]!\n",keyLength);
			return(errCodeParameter);
	}
	switch (msgType)
	{
		case	'0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen);
			break;
		case	'1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen/2);
			break;
		default:
			UnionUserErrLog("in RacalCmdMU:: wrong msgtype flag [%c]!\n",msgType);
			return(errCodeParameter);
	}
	hsmCmdLen += 4;
	if (hsmCmdLen + msgLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in RacalCmdMU:: msg length [%d] too long!\n",msgLen);
		return(-1);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,msg,msgLen);
	hsmCmdLen += msgLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdMU:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"MV",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdMU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdMU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	return(16);
}

// 将ZMK加密的ZPK转换为LMK加密
int RacalCmdFA(int hsmSckHDL,PUnionSJL06 pSJL06,char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		offset;
	int		len;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"FA",2);
	hsmCmdLen = 2;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionPutKeyIntoRacalKeyString zmk [%s]error!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(zpkByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionPutKeyIntoRacalKeyString zpkByZmk [%s]error!\n",zpkByZmk);
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
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"FB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdFA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdFA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	offset = 4;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,zpkByLmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	UnionDebugLog("in RacalCmdFA:: zpkByLmk = [%s]\n",zpkByLmk);
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
int RacalCmdFI(int hsmSckHDL,PUnionSJL06 pSJL06,char keyFlag,char *zmk,int variant,char keyLenFlag,
	char *keyByZmk,char *keyByLmk,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		offset;
	int		len;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"FI",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = keyFlag;
	hsmCmdLen++;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionPutKeyIntoRacalKeyString zmk [%s]error!\n",zmk);
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
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"FJ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdFI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdFI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	offset = 4;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,keyByZmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	offset += len;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,keyByLmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
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

int RacalCmdA0(int hsmSckHDL,PUnionSJL06 pSJL06,
	int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,
	char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		zmkLength,keyLen;
	int		offsetOfKeyByLMK,offsetOfKeyByZMK,offsetOfCheckValue;
	int		cvLen;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A0",2);
	hsmCmdLen = 2;
	if (outputByZMK)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
		hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen += 1;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	/*****************************
	// xusj add begin, 2009-7-02
        if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
                memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
        // xusj add end, 2009-7-02
	*****************************/
	hsmCmdLen += ret;
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyKeyScheme!\n");
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
			case	16:
				ret = 0;
				break;
			case	32:
				memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
				ret = 1;
				break;
			case	48:
				memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
				ret = 1;
				break;
			default:
				UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme! zmk = [%s] zmkLen = [%d]\n",zmk,zmkLength);
				return(errCodeParameter);
		}
		hsmCmdLen += ret;
		memcpy(hsmCmdBuf+hsmCmdLen,zmk,zmkLength);
		hsmCmdLen += zmkLength;
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		//if ((ret = UnionCaculateHsmKeyKeyScheme(zmkLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyKeyScheme keyLength = [%d]!\n",keyLength);
			//UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
	}		

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A1",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	offsetOfKeyByZMK = -1;
	switch (keyLength)
	{
		case	con64BitsDesKey:
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
		case	con128BitsDesKey:
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
		case	con192BitsDesKey:
			offsetOfKeyByLMK = 4 + 1;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 1 + 48 + 1;
				offsetOfCheckValue = 4 + 1 + 48 + 1 + 48;
			}
			else
				offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;		// Mary add, 2004-4-12
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
			UnionUserErrLog("in RacalCmdA0:: invalid keyLength type [%d]\n",keyLength);
			return(errCodeParameter);
	}
	memcpy(keyByLMK,hsmCmdBuf+offsetOfKeyByLMK,keyLen);
	if (outputByZMK)
		memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	//UnionUserErrLog("in RacalCmdA0:: cvLen = [%d]\n",cvLen = ret - offsetOfKeyByZMK - keyLen);
	if ((cvLen = ret - offsetOfKeyByZMK - keyLen) > 0)
	{
		if (cvLen >= 16)
			cvLen = 16;
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,cvLen);
	}
	return(keyLen);
}

int RacalCmdA0ForComp(int hsmSckHDL,PUnionSJL06 pSJL06,
	int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,
	char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		zmkLength,keyLen;
	int		offsetOfKeyByLMK,offsetOfKeyByZMK,offsetOfCheckValue;
	int		cvLen;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A0",2);
	hsmCmdLen = 2;
	if (outputByZMK)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
		hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen += 1;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// xusj add begin, 2009-7-02
        if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
                memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
        // xusj add end, 2009-7-02
	hsmCmdLen += ret;
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyKeyScheme!\n");
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
			case	16:
				ret = 0;
				break;
			case	32:
				memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
				ret = 1;
				break;
			case	48:
				memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
				ret = 1;
				break;
			default:
				UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme! zmk = [%s] zmkLen = [%d]\n",zmk,zmkLength);
				return(errCodeParameter);
		}
		hsmCmdLen += ret;
		memcpy(hsmCmdBuf+hsmCmdLen,zmk,zmkLength);
		hsmCmdLen += zmkLength;
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		//if ((ret = UnionCaculateHsmKeyKeyScheme(zmkLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyKeyScheme keyLength = [%d]!\n",keyLength);
			//UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
	}		

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A1",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	offsetOfKeyByZMK = -1;
	switch (keyLength)
	{
		case	con64BitsDesKey:
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
		case	con128BitsDesKey:
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
		case	con192BitsDesKey:
			offsetOfKeyByLMK = 4 + 1;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 1 + 48 + 1;
				offsetOfCheckValue = 4 + 1 + 48 + 1 + 48;
			}
			else
				offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;		// Mary add, 2004-4-12
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
			UnionUserErrLog("in RacalCmdA0:: invalid keyLength type [%d]\n",keyLength);
			return(errCodeParameter);
	}
	memcpy(keyByLMK,hsmCmdBuf+offsetOfKeyByLMK,keyLen);
	if (outputByZMK)
		memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	//UnionUserErrLog("in RacalCmdA0:: cvLen = [%d]\n",cvLen = ret - offsetOfKeyByZMK - keyLen);
	if ((cvLen = ret - offsetOfKeyByZMK - keyLen) > 0)
	{
		if (cvLen >= 16)
			cvLen = 16;
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,cvLen);
	}
	return(keyLen);
}

int RacalCmd0A(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"0A",2);
	hsmCmdLen = 2;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmd0A:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"0B",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmd0A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int RacalCmdRA(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"RA",2);
	hsmCmdLen = 2;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdRA:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"RB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdRA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int RacalCmdPA(int hsmSckHDL,PUnionSJL06 pSJL06,char *format,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"PA",2);
	hsmCmdLen = 2;
	strcpy(hsmCmdBuf+hsmCmdLen,format);
	hsmCmdLen += strlen(format);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdPA:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"PB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdPA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int RacalCmdA2(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	int		keyTmp = 0;
	char		tmpBuf[100];
	int		i;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A2",2);
	hsmCmdLen = 2;

	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA2:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
		memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
	// Mary add end, 2004-4-15
	hsmCmdLen += ret;

	if (keyTmp >= 10000)
        {
                memcpy(hsmCmdBuf+hsmCmdLen,"U",1);
                hsmCmdLen ++;
        }
	else
	{
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA2:: UnionTranslateHsmKeyKeyScheme!\n");
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

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA2:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A3",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (keyLength)
	{
		case	con64BitsDesKey:
		case	16:
		case	64:
			memcpy(component,hsmCmdBuf+4,16);
			return(16);
		case	con128BitsDesKey:
		case	32:
		case	128:
			memcpy(component,hsmCmdBuf+4+1,32);
			return(32);
		case	con192BitsDesKey:
		case	48:
		case	192:
			memcpy(component,hsmCmdBuf+4+1,48);
			return(48);
		default:
			UnionUserErrLog("in RacalCmdA2:: unknown key length!\n");
			return(errCodeParameter);
	}
}

int RacalCmdA2SpecForZmk(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	int		keyTmp = 0;
	char		tmpBuf[100];
	int		i;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A2",2);
	hsmCmdLen = 2;
	
	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA2SpecForZmk:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if (keyTmp >= 10000)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"U",1);
		hsmCmdLen ++;
	}
	else
	{
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA2SpecForZmk:: UnionTranslateHsmKeyKeyScheme!\n");
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

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA2SpecForZmk:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A3",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA2SpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA2SpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (keyLength)
	{
		case	con64BitsDesKey:
			memcpy(component,hsmCmdBuf+4,16);
			return(16);
		case	con128BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,32);
			return(32);
		case	con192BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,48);
			return(48);
		default:
			UnionUserErrLog("in RacalCmdA2SpecForZmk:: unknown key length!\n");
			return(errCodeParameter);
	}
}

int RacalCmdNC(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"NC",2);
	hsmCmdLen = 2;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNC:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"ND",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(version,hsmCmdBuf+4,ret - 4);
	return(ret - 4);
}

int RacalCmdCC(int hsmSckHDL,PUnionSJL06 pSJL06,int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((zpk1 == NULL) || (zpk2 == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || (accNo == NULL) ||
		(pinBlockByZPK1 == NULL) || (pinBlockByZPK2 == NULL))
	{
		UnionUserErrLog("in RacalCmdCC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CC",2);
	hsmCmdLen = 2;
	
	if ((ret = UnionGenerateX917RacalKeyString(zpk1Length,zpk1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk1);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(zpk2Length,zpk2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk2);
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
		UnionUserErrLog("in RacalCmdCC:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNC:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlockByZPK2,hsmCmdBuf+4+2,16);
	return(16);
}

int RacalCmdMS(int hsmSckHDL,PUnionSJL06 pSJL06,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8096+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((key == NULL) || (msg == NULL) || (mac == NULL) || (pSJL06 == NULL))
	{
		UnionUserErrLog("in RacalCmdMS:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"MS0",3);
	hsmCmdLen = 3;
	hsmCmdBuf[hsmCmdLen] = keyType;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = keyLength;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = msgType;
	++hsmCmdLen;
	switch (keyLength)
	{
		case	'0':
			memcpy(hsmCmdBuf+hsmCmdLen,key,16);
			hsmCmdLen += 16;
			break;
		case	'1':
			// 2008/07/11，王纯军增加
			sprintf(tmpBuf,"MSCmdOf%sNotStandard",pSJL06->staticAttr.hsmGrpID);
			if (UnionReadIntTypeRECVar(tmpBuf) > 0)
			{
				hsmCmdBuf[hsmCmdLen] = 'X';
				hsmCmdLen++;
			}
			// 2008/07/11,增加结束
			memcpy(hsmCmdBuf+hsmCmdLen,key,32);
			hsmCmdLen += 32;
			break;
		case	'2':
			hsmCmdBuf[hsmCmdLen] = 'Y';
			hsmCmdLen++;
			memcpy(hsmCmdBuf+hsmCmdLen,key,48);
			hsmCmdLen += 48;
			break;
			
		default:
			UnionUserErrLog("in RacalCmdMS:: wrong keyLength flag [%c]!\n",keyLength);
			return(errCodeParameter);
	}
	switch (msgType)
	{
		case	'0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen);
			break;
		case	'1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen/2);
			break;
		default:
			UnionUserErrLog("in RacalCmdMS:: wrong msgtype flag [%c]!\n",msgType);
			return(errCodeParameter);
	}
	hsmCmdLen += 4;
	if (hsmCmdLen + msgLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in RacalCmdMS:: msg length [%d] too long!\n",msgLen);
		return(-1);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,msg,msgLen);
	hsmCmdLen += msgLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNC:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"MT",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	return(16);
}

// Mary add begin, 2004-3-9
/*
函数功能：
	DG指令，用PVK生成PIN的PVV(PIN Verification Value)，
	采用的加密标准为Visa Method
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：LMK加密的PIN密文长度
	pin：LMK加密的PIN密文
	pvkLength：LMK加密的PVK对长度
	pvk：LMK加密的PVK对
	lenOfAccNo：客户帐号长度
	accNo：客户帐号
输出参数：
	pvv：产生的PVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无PVV
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回PVV的长度
*/
int RacalCmdDG(int hsmSckHDL,PUnionSJL06 pSJL06,int pinLength,char *pin,int pvkLength,char *pvk,\
		int lenOfAccNo,char *accNo,char *pvv,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((pin == NULL) || (pvk == NULL) || (accNo == NULL) || (pvv == NULL))
	{
		UnionUserErrLog("in RacalCmdDG:: wrong parameters!\n");
		return(errCodeParameter);
	}
	
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DG",2);
	hsmCmdLen = 2;
	// LMK加密的PVK对
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN
	memcpy(hsmCmdBuf+hsmCmdLen,pin,pinLength);
	hsmCmdLen += pinLength;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI（0－6之间）
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DH",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDG:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 产生的PVV
	memcpy(pvv,hsmCmdBuf+4,4);
	return(4);
}

/*
函数功能：
	CA指令，将一个TPK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdCA(int hsmSckHDL,PUnionSJL06 pSJL06,int tpkLength,char *tpk,int zpkLength,\
		char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((tpk == NULL) || (zpk == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || \
		(accNo == NULL) || (pinBlockByTPK == NULL) || (pinBlockByZPK == NULL))
	{
		UnionUserErrLog("in RacalCmdCA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CA",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
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
		UnionUserErrLog("in RacalCmdCA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdCA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后ZPK加密的PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4+2,16);
	return(16);
}

/*
函数功能：
	JE指令，将一个ZPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJE(int hsmSckHDL,PUnionSJL06 pSJL06,int zpkLength,char *zpk,\
		char *pinFormat,char *pinBlockByZPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((zpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
		(pinBlockByZPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JE",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJE:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后LMK加密的PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	JC指令，将一个TPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	tpkLength：LMK加密的TPK长度
	tpk：LMK加密的TPK
	pinFormat：PIN格式
	pinBlockByTPK：转换前由TPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJC(int hsmSckHDL,PUnionSJL06 pSJL06,int tpkLength,char *tpk,\
		char *pinFormat,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((tpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
		(pinBlockByTPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
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
		UnionUserErrLog("in RacalCmdJC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJC:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后LMK加密的PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	JG指令，将一个LMK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinLength：LMK加密的PIN密文长度
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByZPK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdJG(int hsmSckHDL,PUnionSJL06 pSJL06,int zpkLength,char *zpk,\
		char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,\
		int lenOfAccNo,char *pinBlockByZPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((zpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
		(pinBlockByZPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJG:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JG",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JH",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJG:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后ZPK加密的PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4,16);
	return(16);
}

/*
函数功能：
	JA指令，随机产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：要求随机生成的PIN明文的长度
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：随机产生的PIN的密文，由LMK加密
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJA(int hsmSckHDL,PUnionSJL06 pSJL06,int pinLength,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((accNo == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JA",2);
	hsmCmdLen = 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pinLength);
	hsmCmdLen += 2;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJA:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 随机产生的PIN的密文，由LMK加密
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	EE指令，用IBM方式产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdEE(int hsmSckHDL,PUnionSJL06 pSJL06,int minPINLength,char *pinValidData,\
		char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,\
		char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
		(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in RacalCmdEE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EE",2);
	hsmCmdLen = 2;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
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
		UnionUserErrLog("in RacalCmdEE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEE:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdEE:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}
	// 随机产生的PIN的密文，由LMK加密
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	DE指令，用IBM方式产生一个PIN的Offset
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinOffset
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinOffset的长度
*/
int RacalCmdDE(int hsmSckHDL,PUnionSJL06 pSJL06,int minPINLength,char *pinValidData,\
		char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,\
		char *pvk,char *accNo,int lenOfAccNo,char *pinOffset,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
		(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in RacalCmdDE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DE",2);
	hsmCmdLen = 2;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
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
		UnionUserErrLog("in RacalCmdDE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDE:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdDE:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}
	// 产生的PIN的Offset
	memcpy(pinOffset,hsmCmdBuf+4,12);
	return(12);
}

/*
函数功能：
	BA指令，使用本地主密钥加密一个PIN明文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：加密后的PIN密文的长度
	pinTextLength：PIN明文的长度
	pinText：PIN明文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdBA(int hsmSckHDL,PUnionSJL06 pSJL06,int pinCryptogramLen,int pinTextLength,\
		char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1],tmpBuf[100];
	int		hsmCmdLen = 0;
	
	if ((pinText == NULL) || (accNo == NULL) || (pinBlockByLMK == NULL) || (pinTextLength < 0) || (pinCryptogramLen < 0))
	{
		UnionUserErrLog("in RacalCmdBA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BA",2);
	hsmCmdLen = 2;
	
	// 填充PIN明文
	if ((pinCryptogramLen >= sizeof(tmpBuf)) || (pinTextLength >= sizeof(tmpBuf)))
	{
		UnionUserErrLog("in RacalCmdBA:: pinCryptogramLen = [%d] or pinTextLength = [%d] too long!\n",pinCryptogramLen,pinTextLength);
		return(errCodeParameter);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	memset(tmpBuf,'F',pinCryptogramLen);
	memcpy(tmpBuf,pinText,pinTextLength);

	// 填充后的PIN明文
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBA:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// LMK加密的PIN密文
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	NG指令，使用本地主密钥解密一个PIN密文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：LMK加密的PIN密文的长度
	pinCryptogram：LMK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	referenceNumber：用LMK18-19加密帐号得到的偏移值
	pinText：PIN明文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinText
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinText的长度
*/
int RacalCmdNG(int hsmSckHDL,PUnionSJL06 pSJL06,int pinCryptogramLen,char *pinCryptogram,\
		char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1],tmpBuf[100];
	int		hsmCmdLen = 0,i;
	
	if ((pinCryptogram == NULL) || (accNo == NULL) || (referenceNumber == NULL) || (pinText == NULL))
	{
		UnionUserErrLog("in RacalCmdNG:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"NG",2);
	hsmCmdLen = 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdNG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinCryptogram,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNG:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"NH",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNG:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 用LMK18-19加密帐号得到的偏移值
	memcpy(referenceNumber,hsmCmdBuf+4+pinCryptogramLen,12);
	// 填充的PIN明文
	memset(tmpBuf,0,sizeof(tmpBuf));
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
	return(i);
}

/*
函数功能：
	DA指令，用IBM方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDA(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,\
		char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByTPK == NULL) || (pinValidData == NULL) || (pinOffset == NULL) || \
		(decimalizationTable == NULL) || (tpk == NULL) || (pvk == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdDA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DA",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
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
		UnionUserErrLog("in RacalCmdDA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
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
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdDA:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	EA指令，用IBM方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEA(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,\
		char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByZPK == NULL) || (pinValidData == NULL) || (pinOffset == NULL) || \
		(zpk == NULL) || (pvk == NULL) || (decimalizationTable == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdEA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EA",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
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
		UnionUserErrLog("in RacalCmdEA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
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
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdDE:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	DC指令，用VISA方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,\
		char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByTPK == NULL) || (pvv == NULL) || (tpk == NULL) || \
		(pvk == NULL) || (pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdDC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
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
		UnionUserErrLog("in RacalCmdDC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	// 终端PIN的4位VISA PVV
	memcpy(hsmCmdBuf+hsmCmdLen,pvv,4);
	hsmCmdLen += 4;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	EC指令，用VISA方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,\
		char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByZPK == NULL) || (pvv == NULL) || (zpk == NULL) || \
		(pvk == NULL) || (pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdEC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EC",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
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
		UnionUserErrLog("in RacalCmdEC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	// 终端PIN的4位VISA PVV
	memcpy(hsmCmdBuf+hsmCmdLen,pvv,4);
	hsmCmdLen += 4;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"ED",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	BC指令，用比较方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByTPK：TPK加密的PIN密文
	tpkLength：LMK加密的TPK
	tpk：LMK加密的TPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,\
		int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByTPK == NULL) || (pinByLMK == NULL) || (tpk == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdBC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
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
		UnionUserErrLog("in RacalCmdBC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 主机PIN密文，由LMK02-03加密
	memcpy(hsmCmdBuf+hsmCmdLen,pinByLMK,pinByLMKLength);
	hsmCmdLen += pinByLMKLength;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBC:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	BE指令，用比较方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByZPK：ZPK加密的PIN密文
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBE(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,\
		int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByZPK == NULL) || (pinByLMK == NULL) || (zpk == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdBE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BE",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
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
		UnionUserErrLog("in RacalCmdBE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 主机PIN密文，由LMK02-03加密
	memcpy(hsmCmdBuf+hsmCmdLen,pinByLMK,pinByLMKLength);
	hsmCmdLen += pinByLMKLength;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBE:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	CW指令，产生VISA卡校验值CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	cardValidDate：VISA卡的有效期
	cvkLength：CVK的长度
	cvk：CVK
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	cvv：生成的VISA卡的CVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无cvv
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回cvv的长度
*/
int RacalCmdCW(int hsmSckHDL,PUnionSJL06 pSJL06,char *cardValidDate,int cvkLength,char *cvk,\
		char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in RacalCmdCW:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CW",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCW:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
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
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdCW:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CX",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdCW:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 生成的VISA卡的CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	return(3);
}

/*
函数功能：
	CY指令，验证VISA卡的CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdCY(int hsmSckHDL,PUnionSJL06 pSJL06,char *cvv,char *cardValidDate,\
		int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in RacalCmdCY:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CY",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCY:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
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
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdCY:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CZ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdCY:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	EW指令，用私钥签名 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	indexOfVK：私钥索引号
	signDataLength：待签名数据的长度
	signData：待签名的数据
	vkLength：LMK加密的私钥长度
	vk：LMK加密的私钥
输出参数：
	signature：生成的签名
	errCode：错误代码
返回值：
	<0，函数执行失败，无signature
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回signature的长度

int RacalCmdEW(int hsmSckHDL,PUnionSJL06 pSJL06,char *indexOfVK,int signDataLength,\
		char *signData,int vkLength,char *vk,char *signature,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1],tmpBuf[10];
	int		hsmCmdLen = 0;
	
	if ((indexOfVK == NULL) || (signData == NULL) || (vk == NULL) || (signature == NULL))
	{
		UnionUserErrLog("in RacalCmdEW:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EW",2);
	hsmCmdLen = 2;
	// Hash Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gHashIdentifier,2);
	hsmCmdLen += 2;
	// Signature Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gSignatureIdentifier,2);
	hsmCmdLen += 2;
	// Pad Mode Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gPadModeIdentifier,2);
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
	// LMK加密的私钥长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLength);
	hsmCmdLen += 4;
	// LMK加密的私钥
	memcpy(hsmCmdBuf+hsmCmdLen,vk,vkLength);
	hsmCmdLen += vkLength;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEW:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EX",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEW:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 生成的签名长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	// 生成的签名
	memcpy(signature,hsmCmdBuf+4+4,atoi(tmpBuf));
	return(atoi(tmpBuf));
}


函数功能：
	EY指令，用公钥验证签名
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	macOfPK：公钥的MAC值
	signatureLength：待验证的签名的长度
	signature：待验证的签名
	signDataLength：待签名数据的长度
	signData：待签名的数据
	publicKeyLength：公钥的长度
	publicKey：公钥
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码

int RacalCmdEY(int hsmSckHDL,PUnionSJL06 pSJL06,char *macOfPK,int signatureLength,\
		char *signature,int signDataLength,char *signData,int publicKeyLength,\
		char *publicKey,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((macOfPK == NULL) || (signData == NULL) || (publicKey == NULL) || (signature == NULL))
	{
		UnionUserErrLog("in RacalCmdEY:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EY",2);
	hsmCmdLen = 2;
	// Hash Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gHashIdentifier,2);
	hsmCmdLen += 2;
	// Signature Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gSignatureIdentifier,2);
	hsmCmdLen += 2;
	// Pad Mode Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gPadModeIdentifier,2);
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
	
	UnionProgramerMemLog("in RacalCmdEY:: req =",hsmCmdBuf, hsmCmdLen);
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEY:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EZ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEY:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}
*/
// Mary add end, 2004-3-9

// Mary add begin, 2004-3-24
/*
函数功能：
	BU指令，产生一把密钥的校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	key：LMK加密的密钥密文
输出参数：
	checkValue：生成的密钥校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回checkValue的长度
*/
/****del by xusj 20110624****
int RacalCmdBU(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	int		keyTmp = 0;
	char		tmpBuf[100];
	
	if ((key == NULL) || (checkValue == NULL) || (pSJL06 == NULL))
	{
		UnionUserErrLog("in RacalCmdBU:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;

	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	// 加密密钥的LMK对指示符，即密钥类型代码的最后2位
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,tmpBuf)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	if (memcmp(tmpBuf,"000",3) == 0)
		memcpy(tmpBuf,"010",3);
	// Mary add end, 2004-4-15
	memcpy(hsmCmdBuf+hsmCmdLen,&tmpBuf[1],2);
	hsmCmdLen += 2;
	// 密钥长度标志
	switch (keyLength)
	{
		case	con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '2';
			break;
		default:
			UnionUserErrLog("in RacalCmdBU:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	// LMK加密的密钥密文
	if (keyTmp >= 10000)    
        {       
                memcpy(hsmCmdBuf+hsmCmdLen,"U",1);      
                hsmCmdLen ++;   
                memcpy(hsmCmdBuf+hsmCmdLen,key,strlen(key));    
                hsmCmdLen += strlen(key);       
        } 
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdBU:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	*
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// 密钥类型代码
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// 校验值类型标志
	hsmCmdBuf[hsmCmdLen] = '1';
	hsmCmdLen += 1;
	*

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BV",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdBU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(checkValue,hsmCmdBuf+4,16);
	return(16);
}
********del by xusj 20110624 end******/
int RacalCmdBU(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
                TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode)
{
        int             ret;
        char            hsmCmdBuf[1024+1];
        int             hsmCmdLen = 0;
	int		keyTmp = 0;
        char            tmpBuf[100];
        char            tmpBuf1[100];

        if ((key == NULL) || (checkValue == NULL) || (pSJL06 == NULL))
        {
                UnionUserErrLog("in RacalCmdBU:: wrong parameters!\n");
                return(errCodeParameter);
        }
        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
        // 命令字
        memcpy(hsmCmdBuf,"BU",2);
        hsmCmdLen = 2;
	
	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }
	
        // 加密密钥的LMK对指示符，即密钥类型代码的最后2位
        memset(tmpBuf,0,sizeof(tmpBuf));
        memset(tmpBuf1,0,sizeof(tmpBuf1));
        if ((ret = UnionTranslateHsmKeyTypeStringForBU(keyType,tmpBuf,tmpBuf1)) < 0)
        {
                UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyTypeStringForBU!\n");
                return(ret);
        }
        // Mary add begin, 2004-4-15
        if (memcmp(tmpBuf,"000",3) == 0)
                memcpy(tmpBuf,"010",3);
        // Mary add end, 2004-4-15
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
                        UnionUserErrLog("in RacalCmdBU:: unknown key length!\n");
                        return(errCodeParameter);
        }
        hsmCmdLen += 1;
        // LMK加密的密钥密文
	if (keyTmp >= 10000)    
        {       
                memcpy(hsmCmdBuf+hsmCmdLen,"U",1);      
                hsmCmdLen ++;   
                memcpy(hsmCmdBuf+hsmCmdLen,key,strlen(key));    
                hsmCmdLen += strlen(key);       
        } 
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
        	{
        	        UnionUserErrLog("in RacalCmdBU:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
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
        if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in RacalCmdBU:: UnionLongConnSJL06Cmd!\n");
                return(ret);
        }
        hsmCmdBuf[ret] = 0;
        if ((memcmp(hsmCmdBuf,"BV",2) != 0) || (ret < 4))
        {
                UnionUserErrLog("in RacalCmdBU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
                return(errSJL06Abnormal);
        }
        memcpy(errCode,hsmCmdBuf+2,2);
        if (memcmp(errCode,"00",2) != 0)
        {
                //UnionUserErrLog("in RacalCmdBU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
                return(0);
        }
        memcpy(checkValue,hsmCmdBuf+4,16);
        return(16);
}


// Mary add begin, 2004-3-24
/*
函数功能：
	BU指令，产生一把密钥的校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	key：LMK加密的密钥密文
输出参数：
	checkValue：生成的密钥校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回checkValue的长度
*/
int RacalCmdBUNormal(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	
	if ((key == NULL) || (checkValue == NULL) || (pSJL06 == NULL))
	{
		UnionUserErrLog("in RacalCmdBUNormal:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;
	// 加密密钥的LMK对指示符，即密钥类型代码的最后2位
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,tmpBuf)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	//if (memcmp(tmpBuf,"000",3) == 0)
	//	memcpy(tmpBuf,"010",3);
	// Mary add end, 2004-4-15
	memcpy(hsmCmdBuf+hsmCmdLen,&tmpBuf[1],2);
	hsmCmdLen += 2;
	// 密钥长度标志
	switch (keyLength)
	{
		case	con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '2';
			break;
		default:
			UnionUserErrLog("in RacalCmdBUNormal:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	// LMK加密的密钥密文
	if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
		return(ret);
	}
	hsmCmdLen += ret;
	/*
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// 密钥类型代码
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// 校验值类型标志
	hsmCmdBuf[hsmCmdLen] = '1';
	hsmCmdLen += 1;
	*/

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BV",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBUNormal:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdBUNormal:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(checkValue,hsmCmdBuf+4,16);
	return(16);
}

int RacalCmdBUSpecForZmk(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	int		keyTmp = 0;
	char		tmpBuf[100];
	
	if ((key == NULL) || (checkValue == NULL) || (pSJL06 == NULL))
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;
	
	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	// 加密密钥的LMK对指示符，即密钥类型代码的最后2位
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,tmpBuf)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,&tmpBuf[1],2);
	hsmCmdLen += 2;
	// 密钥长度标志
	switch (keyLength)
	{
		case	con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '2';
			break;
		default:
			UnionUserErrLog("in RacalCmdBUSpecForZmk:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	// LMK加密的密钥密文
	if (keyTmp >= 10000)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"U",1);
		hsmCmdLen ++;
		memcpy(hsmCmdBuf+hsmCmdLen,key,strlen(key));
		hsmCmdLen += strlen(key);
	}
	else
	{
		if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
			return(ret);
		}
		hsmCmdLen += ret;
	}
	/*
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// 密钥类型代码
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// 校验值类型标志
	hsmCmdBuf[hsmCmdLen] = '1';
	hsmCmdLen += 1;
	*/

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BV",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdBUSpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(checkValue,hsmCmdBuf+4,16);
	return(16);
}

/*
函数功能：
	A4指令，用几把密钥的密文成分合成一把密钥，并生成校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	partKeyNum：密钥成分的数量
	partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
输出参数：
	keyByLMK：合成的密钥密文，由LMK加密
	checkValue：合成的密钥的校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无keyByLMK和checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回keyByLMK和checkValue的总长度
*/
int RacalCmdA4(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	int		i;
	int		keyTmp = 0;
	
	if ((keyByLMK == NULL) || (checkValue == NULL) || (pSJL06 == NULL))
	{
		UnionUserErrLog("in RacalCmdA4:: wrong parameters!\n");
		return(errCodeParameter);
	}
	if (partKeyNum < 2 || partKeyNum > 9)
	{
		UnionUserErrLog("in RacalCmdA4:: partKeyNum [%d] must between 2 and 9!\n",partKeyNum);
		return(errCodeParameter);
	}
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"A4",2);
	hsmCmdLen = 2;
	// 密钥成分数量
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;

	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	// 密钥类型代码
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA4:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
		memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
	// Mary add end, 2004-4-15
	hsmCmdLen += ret;

	if (keyTmp >= 10000)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,"U",1);
		hsmCmdLen ++;

		for (i = 0; i < partKeyNum; i++)
		{
			memcpy(hsmCmdBuf+hsmCmdLen,"U",1);
                	hsmCmdLen ++;

			memcpy(hsmCmdBuf+hsmCmdLen,partKey[i],strlen(partKey[i]));
			hsmCmdLen += strlen(partKey[i]);
		}	
	}
	else
	{
		// 密钥长度标志
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA4:: UnionTranslateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
		
		for (i = 0; i < partKeyNum; i++)
		{
			// LMK加密的密钥密文
			if ((ret = UnionGenerateX917RacalKeyString(keyLength,partKey[i],hsmCmdBuf+hsmCmdLen)) < 0)
			{
				UnionUserErrLog("in RacalCmdA4:: UnionGenerateX917RacalKeyString for [%s][%d]\n",partKey[i],keyLength);
				return(ret);
			}
			hsmCmdLen += ret;
		}
	}	

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA4:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A5",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (keyLength)
	{
		case	con64BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+16,6);
			return(16+6);
		case	con128BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+1+32,6);
			return(32+6);
		case	con192BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			return(48+6);
		default:
			UnionUserErrLog("in RacalCmdA4:: unknown key length!\n");
			return(errCodeParameter);
	}
}
// Mary add end, 2004-3-24

/*
功能	由源帐号生成的PINBLOCK转换为由目的帐号生成的PINBLOCK，并受指定的密钥加密保护
说明	根据密钥长度确定算法

指令消息格式
消息头	mA	
命令码	2A	DF
ZPK	16H or
	1A+32H or
	1A+48H	被 LMK 06-07 加密的ZPK
PVK	16H or
	1A+32H or
	1A+48H	被 LMK 14-15加密的ZPK
Check length	2N	最大PIN长度
十进制数表	16N	16进制转换表
PIN Validation data	12A	User-defined data consisting of hexadecimal characters and
the character N, which indicates to the HSM where to insert
the last 5 digits of the account number.
PIN block	16 H	被ZPK采用ANSI X9.8标准加密
源账号	12 N	12位有效源账号
目标账号	12 N	12位有效目标账号
*/

int RacalCmdDF(int hsmSckHDL,PUnionSJL06 pSJL06,char *zpk,char *pvk,int maxPinLen,char *decimalizationTable,
		char *pinValidationData,char *pinBlock1,char *accNo1,char *accNo2,
		char *pinBlock2,char *pinOffset,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((zpk == NULL) || (pvk == NULL) || (decimalizationTable == NULL) || 
		(pinValidationData == NULL) || (pinBlock1 == NULL) || (accNo1 == NULL) || (accNo2 == NULL) ||
		(pinBlock2 == NULL) || (pinOffset == NULL) || (errCode == NULL))
	{
		UnionUserErrLog("in RacalCmdDF:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DF",2);
	hsmCmdLen = 2;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDF:: UnionGenerateX917RacalKeyString for [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(pvk),pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDF:: UnionGenerateX917RacalKeyString for [%s]\n",pvk);
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
	memcpy(hsmCmdBuf+hsmCmdLen,accNo1,12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo2,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDF:: UnionLongConnSJL06Cmd! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DG",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDF:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		memcpy(errCode,"00",2);
		memcpy(pinBlock2,hsmCmdBuf+2+2,16);
		memcpy(pinOffset,hsmCmdBuf+2+2+16,12);
	}
	return(0);
}

int RacalCmdA8(int hsmSckHDL,PUnionSJL06 pSJL06,
	TUnionDesKeyType keyType,char *key,char *zmk,
	char *keyByZMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		zmkLength,keyLen;
	int		offsetOfKeyByZMK,offsetOfCheckValue;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A8",2);
	hsmCmdLen = 2;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zmk),zmk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(key),key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	switch (strlen(key))
	{
		case	16:
			memcpy(hsmCmdBuf+hsmCmdLen,"Z",1);
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			break;
		default:
			UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: keylen [%d] error!\n",strlen(key));
			return(errCodeParameter);
	}
	hsmCmdLen++;
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA8:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A9",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA8:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA8:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (strlen(key))
	{
		case	16:
			offsetOfKeyByZMK = 4;
			offsetOfCheckValue = 4 + 16;
			keyLen = 16;
			break;
		case	32:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 32;
			keyLen = 32;
			break;
		case	48:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;		// Mary add, 2004-4-12
		default:
			UnionUserErrLog("in RacalCmdA8:: invalid keyLen[%d]\n",strlen(key));
			return(errCodeParameter);
	}
	memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	if (ret - offsetOfCheckValue > 16)
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,16);
	else
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,ret - offsetOfCheckValue);
	return(keyLen);
}

int RacalCmdA8_SM1_SFF33(int hsmSckHDL,PUnionSJL06 pSJL06, char *algFlag,
	TUnionDesKeyType keyType,char *key,char *zmk,
	char *keyByZMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		zmkLength,keyLen;
	int		offsetOfKeyByZMK,offsetOfCheckValue;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"S8",2);
	hsmCmdLen = 2;

	if (algFlag != NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, algFlag, 1);
		hsmCmdLen += 1;
	}

	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8_SM1_SFF33:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(zmk),zmk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8_SM1_SFF33:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	if ((ret = UnionGenerateX917RacalKeyString(strlen(key),key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8_SM1_SFF33:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	switch (strlen(key))
	{
		case	16:
			memcpy(hsmCmdBuf+hsmCmdLen,"Z",1);
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			break;
		default:
			UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: keylen [%d] error!\n",strlen(key));
			return(errCodeParameter);
	}
	hsmCmdLen++;
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA8_SM1_SFF33:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"S9",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA8_SM1_SFF33:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA8_SM1_SFF33:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (strlen(key))
	{
		case	16:
			offsetOfKeyByZMK = 4;
			offsetOfCheckValue = 4 + 16;
			keyLen = 16;
			break;
		case	32:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 32;
			keyLen = 32;
			break;
		case	48:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;		// Mary add, 2004-4-12
		default:
			UnionUserErrLog("in RacalCmdA8_SM1_SFF33:: invalid keyLen[%d]\n",strlen(key));
			return(errCodeParameter);
	}
	memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	if (ret - offsetOfCheckValue > 16)
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,16);
	else
		memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,ret - offsetOfCheckValue);
	return(keyLen);
}

// Mary add begin, 2008-9-16
/*
函数功能：
	33指令，私钥解密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	flag：用公钥加密时所采用的填充方式，
		'0'：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
		'1'：PKCS填充方式（一般情况下使用此方式）
	vkIndex：私钥索引，"00"－"20"
	cipherDataLen：密文数据的字节数
	cipherData：用于解密的密文数据
	sizeOfPlainData：plainData所在的存储空间大小
输出参数：
	plainData：解密得到的明文数据
	errCode：错误代码
返回值：
	<0，函数执行失败，无plainData
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回plainData的长度
*/
int SJL06Cmd33(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int cipherDataLen,char *cipherData,char *plainData,int sizeOfPlainData,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	
	if (vkIndex==NULL || cipherDataLen<=0 || cipherData==NULL || plainData==NULL || (flag!='0' && flag!='1') || sizeOfPlainData<=0)
	{
		UnionUserErrLog("in SJL06Cmd33:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"33",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = flag;
	hsmCmdLen++;
	memcpy(hsmCmdBuf+hsmCmdLen,vkIndex,2);
	hsmCmdLen += 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",cipherDataLen);
	hsmCmdLen += 4;
	if (hsmCmdLen + cipherDataLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in SJL06Cmd33:: cipherDataLen [%d] too long!\n",cipherDataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,cipherData,cipherDataLen);
	hsmCmdLen += cipherDataLen;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd33:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"34",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd33:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd33:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	if ((len = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd33:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	
	if (len > sizeOfPlainData)
	{
		UnionUserErrLog("in SJL06Cmd33:: sizeOfPlainData [%d] too small!\n",sizeOfPlainData);
		return(errCodeSmallBuffer);
	}
	
	memcpy(plainData,hsmCmdBuf+4+4,len);
	return(len);
}
// Mary add end, 2008-9-16

// lixiang add begin, 2009-01-19

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	pvkIndex：	指定的私钥，用于解密PIN数据密文
	fillMode：	“0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
			“1”：PKCS填充方式（一般情况下使用此方式）
	ZPK：		用于加密PIN的密钥。
	accNo：		用户主帐号
	lenOfPinBlock:	公钥加密的PIN密文的长度
	pinBlock：	经公钥加密的PIN数据密文
输出参数	
	lenOfPin：	密钥长度
	pinBlock1：	返回ZPK加密下的PIN密文
	pinBlock2：	返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
	lenOfUniqueID：	01-20
	UniqueID：	返回的经ASCII扩展后ID码明文
	errCode：	密码机返回码
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功

输入域		长度	类型	说　明
命令代码	2	A	值“H3”
私钥索引	2	N	指定的私钥，用于解密PIN数据密文
用公钥加密时所采用的填充方式	1	N	“0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
“1”：PKCS填充方式（一般情况下使用此方式）
ZPK	16/1A+32/1A+48	H	用于加密PIN的密钥
帐号（PAN）	12	N	用户主帐号
PIN密文	n	B	经公钥加密的PIN数据密文
	
输出域		长度	类型	说　明
响应代码	2	A	值“H4”
错误代码	2	H	０８：PIN明文长度错
请增加其它详细错误码
密码长度	2	N	04~20
PIN密文	32	H	返回ZPK加密下的PIN密文
PIN密文2	48	H	返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
ID码长度	2	N	01-20
ID码明文	N*2	H	返回的经ASCII扩展后ID码明文
*/
int RacalCmdH2 (int hsmSckHDL,PUnionSJL06 pSJL06, int pvkIndex, char fillMode, char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock,
		 char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID, char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	int		offset;
	int		lenOfID;
	
	if ((fillMode!='0' && fillMode!='1') || ZPK==NULL || accNo==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in RacalCmdH2:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"H2",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pvkIndex);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = fillMode;
	hsmCmdLen++;
	
	switch (len = strlen(ZPK))
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
			UnionUserErrLog("in RacalCmdH2:: invalid [%s]\n",ZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZPK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,strlen(accNo));
	hsmCmdLen += strlen(accNo);
	
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,lenOfPinBlock);
	hsmCmdLen += lenOfPinBlock;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdH2:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}	
	
	if ((memcmp(hsmCmdBuf,"H3",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdH2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdH2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	
	memcpy(lenOfPin,hsmCmdBuf+4,2);
	if ((offset = UnionConvertIntoLen(lenOfPin,2)) < 0)
	{
		UnionUserErrLog("in RacalCmdH2:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	memcpy(pinBlock1,hsmCmdBuf+4+2,32);
	memcpy(pinBlock2,hsmCmdBuf+4+2+32,48);
	memcpy(lenOfUniqueID,hsmCmdBuf+4+2+32+48,2);
	
	if ((offset = UnionConvertIntoLen(lenOfUniqueID,2)) < 0)
	{
		UnionUserErrLog("in RacalCmdH2:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	lenOfID = atoi(lenOfUniqueID)*2;
	if(lenOfID > strlen(hsmCmdBuf+4+2+32+48+2))
	{
		UnionUserErrLog("in RacalCmdH2:: lenOfUniqueID [%d] too small!\n",lenOfID);
		return(errCodeSmallBuffer);
	}
	memcpy(UniqueID,hsmCmdBuf+4+2+32+48+2,lenOfID);
	return(lenOfID);
}

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	pvkIndex：	指定的私钥，用于解密PIN数据密文
	fillMode：	“0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
			“1”：PKCS填充方式（一般情况下使用此方式）
	ZPK：		用于加密PIN的密钥。
	accNo：		用户主帐号
	lenOfPinBlock:	公钥加密的PIN密文的长度
	pinBlock：	经公钥加密的PIN数据密文
输出参数	
	lenOfPin：	密钥长度
	pinBlock1：	返回ZPK加密下的PIN密文
	pinBlock2：	返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
	lenOfUniqueID：	01-20
	UniqueID：	返回的经ASCII扩展后ID码明文
	errCode：	密码机返回码
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功

输入域		长度	类型	说　明
命令代码	2	A	值“H3”
私钥索引	2	N	指定的私钥，用于解密PIN数据密文
用公钥加密时所采用的填充方式	1	N	“0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
“1”：PKCS填充方式（一般情况下使用此方式）
ZPK	16/1A+32/1A+48	H	用于加密PIN的密钥
帐号（PAN）	12	N	用户主帐号
PIN密文	n	B	经公钥加密的PIN数据密文
	
输出域		长度	类型	说　明
响应代码	2	A	值“H4”
错误代码	2	H	０８：PIN明文长度错
请增加其它详细错误码
密码长度	2	N	04~20
PIN密文	32	H	返回ZPK加密下的PIN密文
PIN密文2	48	H	返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
ID码长度	2	N	01-20
ID码明文	N*2	H	返回的经ASCII扩展后ID码明文
*/
int RacalCmdN6 (int hsmSckHDL,PUnionSJL06 pSJL06, int pvkIndex, char fillMode, char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock,
		 char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID, char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	int		offset;
	int		lenOfID;
	
	if ((fillMode!='0' && fillMode!='1') || ZPK==NULL || accNo==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in RacalCmdN6:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"N6",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pvkIndex);
	hsmCmdLen += 2;
	hsmCmdBuf[hsmCmdLen] = fillMode;
	hsmCmdLen++;
	
	switch (len = strlen(ZPK))
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
			UnionUserErrLog("in RacalCmdN6:: invalid [%s]\n",ZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZPK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,strlen(accNo));
	hsmCmdLen += strlen(accNo);
	
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,lenOfPinBlock);
	hsmCmdLen += lenOfPinBlock;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdN6:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}	
	
	if ((memcmp(hsmCmdBuf,"N7",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdN6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdN6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	
	memcpy(lenOfPin,hsmCmdBuf+4,2);
	if ((offset = UnionConvertIntoLen(lenOfPin,2)) < 0)
	{
		UnionUserErrLog("in RacalCmdN6:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	memcpy(pinBlock1,hsmCmdBuf+4+2,32);
	memcpy(pinBlock2,hsmCmdBuf+4+2+32,48);
	memcpy(lenOfUniqueID,hsmCmdBuf+4+2+32+48,2);
	
	if ((offset = UnionConvertIntoLen(lenOfUniqueID,2)) < 0)
	{
		UnionUserErrLog("in RacalCmdN6:: signLen error!\n");
		return(errCodeSmallBuffer);
	}
	lenOfID = atoi(lenOfUniqueID)*2;
	if(lenOfID > strlen(hsmCmdBuf+4+2+32+48+2))
	{
		UnionUserErrLog("in RacalCmdN6:: lenOfUniqueID [%d] too small!\n",lenOfID);
		return(errCodeSmallBuffer);
	}
	memcpy(UniqueID,hsmCmdBuf+4+2+32+48+2,lenOfID);
	return(lenOfID);
}

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	OriKeyType：	源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
	OriKey：	源KEY，用于解密PIN的密钥。
	lenOfAcc：	帐号长度，4~20。
	accNo：		用户主帐号。
	DesKeyType：	’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
	DesKey：	目标KEY，用于加密PIN的密钥。
	pinBlock：	经公钥加密的PIN数据密文。
输出参数
	lenOfPinBlock1：密码长度。
	pinBlock1：	返回ZPK2/PVK加密下的PIN密文。
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功

输入域		长度	类型	说　明
命令代码	2	A	值“H５”
源KEY类型	１	N	０：ZPK
１：PVK（目标KEY类型不能为0）
源ZPK1/PVK1	16/1A+32/1A+48	H	用于解密PIN的密钥
帐号长度	2	N	4~20
帐号（PAN）	４～２０	N	用户主帐号
目的KEY类型	１	N	０：ZPK（只当源KEY类型为0时，目标KEY类型可为0）
１：PVK
目的ZPK２/PVK	16/1A+32/1A+48	H	用于加密PIN的密钥
PIN密文	32	H	经ZPK1加密的PIN数据密文

输出域		长度	类型	说　明
响应代码	2	A	值“H６”
错误代码	2	H	０８：PIN明文格式错
请增加其它详细错误码
密码长度	2	N	04~20
PIN密文	32	H	返回ZPK2/PVK加密下的PIN密文
*/
int RacalCmdH5 (int hsmSckHDL,PUnionSJL06 pSJL06, char *OriKey, char *lenOfAcc,	char *AccNo, 
	char DesKeyType, char *DesKey, char *pinBlock, char *lenOfPinBlock1,char *pinBlock1, char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		offset;
	int		len;
	int		accLen;
	
	if (OriKey==NULL || lenOfAcc==NULL || AccNo==NULL || (DesKeyType!='0' && DesKeyType!='1') || DesKey==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in RacalCmdH5:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"H5",2);
	hsmCmdLen = 2;
	
	switch (len = strlen(OriKey))
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
			UnionUserErrLog("in RacalCmdH5:: invalid [%s]\n",OriKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,OriKey,len);
	hsmCmdLen += len;
	
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAcc);	
	hsmCmdLen += 2;

	accLen = atoi(lenOfAcc);
	//memcpy(hsmCmdBuf+hsmCmdLen,AccNo,(int)lenOfAcc);
	//hsmCmdLen += (int)lenOfAcc;
	memcpy(hsmCmdBuf+hsmCmdLen,AccNo, accLen);
	hsmCmdLen += accLen;
	hsmCmdBuf[hsmCmdLen] = DesKeyType;
	hsmCmdLen++;

	switch (len = strlen(DesKey))
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
			UnionUserErrLog("in RacalCmdH5:: invalid [%s]\n",DesKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,DesKey,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,32);
	hsmCmdLen += 32;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdH5:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"H6",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdH5:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdH5::errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlock1,hsmCmdBuf+4,32);
	return(ret);
}

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	OriKeyType：	源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
	OriKey：	源KEY，用于解密PIN的密钥。
	lenOfAcc：	帐号长度，4~20。
	accNo：		用户主帐号。
	DesKeyType：	’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
	DesKey：	目标KEY，用于加密PIN的密钥。
	pinBlock：	经公钥加密的PIN数据密文。
输出参数
	lenOfPinBlock1：密码长度。
	pinBlock1：	返回ZPK2/PVK加密下的PIN密文。
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功

输入域		长度	类型	说　明
命令代码	2	A	值“H7”
源KEY类型	１	N	０：ZPK
１：PVK（目标KEY类型不能为0）
源ZPK1/PVK1	16/1A+32/1A+48	H	用于解密PIN的密钥
帐号长度	2	N	4~20
帐号（PAN）	４～２０	N	用户主帐号
目的KEY类型	１	N	０：ZPK（只当源KEY类型为0时，目标KEY类型可为0）
１：PVK
目的ZPK２/PVK	16/1A+32/1A+48	H	用于加密PIN的密钥
PIN密文	32	H	经ZPK1加密的PIN数据密文

输出域		长度	类型	说　明
响应代码	2	A	值“H8”
错误代码	2	H	０８：PIN明文格式错
请增加其它详细错误码
密码长度	2	N	04~20
PIN密文	32	H	返回ZPK2/PVK加密下的PIN密文
*/
int RacalCmdH7 (int hsmSckHDL,PUnionSJL06 pSJL06, char *OriKey, char *lenOfAcc,char *AccNo, char *DesKey,
		char *pinBlock, char *lenOfPinBlock1,char *pinBlock1, char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	int		accLen;
	
	if (OriKey==NULL || lenOfAcc==NULL || AccNo==NULL || DesKey==NULL || pinBlock==NULL)
	{
		UnionUserErrLog("in RacalCmdH7:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"H7",2);
	hsmCmdLen = 2;
	
	switch (len = strlen(OriKey))
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
			UnionUserErrLog("in RacalCmdH7:: invalid [%s]\n",OriKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,OriKey,len);
	hsmCmdLen += len;
	
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAcc);
	hsmCmdLen += 2;

	accLen = atoi(lenOfAcc);
	//memcpy(hsmCmdBuf+hsmCmdLen,AccNo,(int)lenOfAcc);
	//hsmCmdLen += (int)lenOfAcc;
	memcpy(hsmCmdBuf+hsmCmdLen, AccNo, accLen);
	hsmCmdLen += accLen;

	switch (len = strlen(DesKey))
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
			UnionUserErrLog("in RacalCmdH7:: invalid [%s]\n",DesKey);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,DesKey,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock,32);
	hsmCmdLen += 32;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdH7:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"H8",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdH7:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdH7::errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlock1,hsmCmdBuf+4,32);
	return(ret);
}

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	ZPK：		用于解密PIN的密钥。
	lenOfAcc：	帐号长度，4~20。
	accNo：		用户主帐号。
	pinBlockByZPK：	经ZPK加密的PIN数据密文。
	PVK：		PVK。
	pinBlockByPVK：	经PVK加密的PIN数据密文。
输出参数
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功
	
输入域		长度	类型	说　明
命令代码	2	A	值“H9”
ZPK	16/1A+32/1A+48	H	用于解密PIN的密钥
帐号长度	2	N	4~20
帐号（PAN）	４～２０	N	用户主帐号
PIN密文	32	H	经ZPK加密的PIN数据密文
PVK	16/1A+32/1A+48	H	
PIN密文	32	H	经PVK加密的PIN数据密文

输出域		长度	类型	说　明
响应代码	2	A	值“H:”
错误代码	2	H	00 成功
01 失败
请增加其它详细错误码

*/
int RacalCmdH9 (int hsmSckHDL,PUnionSJL06 pSJL06, char *ZPK, char *lenOfAcc, char *AccNo,
		char *pinBlockByZPK, char *PVK, char *pinBlockByPVK, char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		offset;
	int		len;
	int		accLen;
	
	if (ZPK==NULL || lenOfAcc==NULL || AccNo==NULL ||pinBlockByZPK==NULL || PVK==NULL || pinBlockByPVK==NULL)
	{
		UnionUserErrLog("in RacalCmdH9:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"H9",2);
	hsmCmdLen = 2;
	
	switch (len = strlen(ZPK))
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
			UnionUserErrLog("in RacalCmdH9:: invalid [%s]\n",ZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZPK,len);
	hsmCmdLen += len;
	
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",lenOfAcc);
	hsmCmdLen += 2;

	accLen = atoi(lenOfAcc);
	//memcpy(hsmCmdBuf+hsmCmdLen,AccNo,(int)lenOfAcc);
	//hsmCmdLen += (int)lenOfAcc;
	memcpy(hsmCmdBuf+hsmCmdLen, AccNo, accLen);
	hsmCmdLen += accLen;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,32);
	hsmCmdLen += 32;
	
	switch (len = strlen(PVK))
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
			UnionUserErrLog("in RacalCmdH9:: invalid [%s]\n",PVK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,PVK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByPVK,32);
	hsmCmdLen += 32;
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdH9:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"H:",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdH9:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdH9:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	return(ret);
}

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	ZPK：		用于加密PIN的密钥。
	accNo：		用户主帐号。
	pinBlockByZPK：	经ZPK加密的PIN数据密文。
	dataOfZAK：	产生ZAK密钥的因素之一。
输出参数	
	lenOfPin：	密码长度
	ZAK：		返回LMK26-27对对加密下的ZAK密钥。
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功
	
输入域		长度	类型	说　明
命令代码	2	A	值“H4”
ZPK	16/1A+32/1A+48	H	用于加密PIN的密钥
帐号（PAN）	12	N	用户主帐号
PIN密文	48	H	经ZPK加密的PIN数据密文
ZAK因子	32	H	产生ZAK密钥的因素之一

输出域		长度	类型	说　明
响应代码	2	A	值“H5”
错误代码	2	H	０８：PIN明文长度错
请增加其它详细错误码
密码长度	2	N	04~20
ZAK密钥	1A+32	H	返回LMK26-27对对加密下的ZAK密钥

*/
int RacalCmdH4 (int hsmSckHDL, PUnionSJL06 pSJL06, char *ZPK, char *AccNo, char *pinBlockByZPK,
	char *dataOfZAK, char *lenOfPin, char *ZAK, char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	
	if (ZPK==NULL || AccNo==NULL || pinBlockByZPK==NULL || dataOfZAK==NULL)
	{
		UnionUserErrLog("in RacalCmdH4:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"H4",2);
	hsmCmdLen = 2;
	switch (len = strlen(ZPK))
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
			UnionUserErrLog("in RacalCmdH4:: invalid [%s]\n",ZPK);
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
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdH4:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"H5",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdH4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdH4:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(lenOfPin,hsmCmdBuf+4,2);
	memcpy(ZAK,hsmCmdBuf+4+2+1,32);
	
	return(ret-4-2);
	
}

// lixiang add end, 2009-01-19
/*
功能
	把PinOffset转换为专用算法（FINSE算法）加密的密文
输  入  消  息  格  式

输  入  域	长  度	类  型	内      容
命   令	2	A	S1
PVK	16H或1A+32H或1A+48H	H	LMK对（14-15）下加密的PVK；用于解密offset。
PinOffset	12	N	Offset的值；采用左对齐的方式在右边填充字符“F”。
检查长度	2	N	最小的PIN长度。
账号	12	N	账号中去除校验位的最右12位。
十进制转换表	16	N	将十六进制转换为十进制的转换表。
PIN校验数据	12	A	用户定义的、包含十六进制字符和字符“N”的数据，用来指示HSM插入账号最后5位的位置。


输  出  消  息  格  式
输  出  域	长  度	类  型	内      容
应答码	2	A	S2
错误代码	2	H	00： 正确      其他：失败
结果	6	H	PIN密文
*/
int RacalCmdS1(int hsmSckHDL, PUnionSJL06 pSJL06, char *PVK, char *pinOffSet,int minPinLen,char *accNo,char *decimalizationTable,char *pinValidationData, char *pinBlock, char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	
	if (PVK==NULL || pinOffSet==NULL || minPinLen < 0|| accNo==NULL || decimalizationTable==NULL || pinValidationData==NULL)
	{
		UnionUserErrLog("in RacalCmdS1:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"S1",2);
	hsmCmdLen = 2;
	switch (len = strlen(PVK))
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
			UnionUserErrLog("in RacalCmdS1:: invalid [%s]\n",PVK);
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
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdS1:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"S2",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdS1:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdS1:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlock,hsmCmdBuf+4,6);
	return(ret-4);	
}

/*
功能：
	把PinBlock转换为专用算法（FINSE算法）加密的密文
输  入  消  息  格  式

输  入  域	长  度	类  型	内      容
命   令	2	A	S2
ZPK	16H或1A+32H或1A+48H	H	LMK对（14-15）下加密的PVK；用于解密offset。
检查长度	2	N	最小的PIN长度。
账号	12	N	账号中去除校验位的最右12位。
源PIN块	16H	H	源PIN块


输  出  消  息  格  式
输  出  域	长  度	类  型	内      容
应答码	2	A	S 3
错误代码	2	H	00： 正确      其他：失败
结果	6	H	PIN密文
*/
int RacalCmdS2(int hsmSckHDL, PUnionSJL06 pSJL06, char *ZPK, int minPinLen,char *accNo,char *OriPinBlock, char *pinBlock,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	
	if (ZPK==NULL || OriPinBlock==NULL || minPinLen < 0|| accNo==NULL )
	{
		UnionUserErrLog("in RacalCmdS2:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"S2",2);
	hsmCmdLen = 2;
	switch (len = strlen(ZPK))
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
			UnionUserErrLog("in RacalCmdS2:: invalid [%s]\n",ZPK);
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
		
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdS2:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"S3",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdS2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdS2:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlock,hsmCmdBuf+4,6);
	return(ret-4);	
}

/*
Field	Length&Type	Details
COMMAND MESSAGE
Message header	mA	(Subsequently returned to the Host unchanged).
Command code	2A	Value E0.
Message block number	1N	0: The only block.
1: The first block.
2: A middle block.
3: The last block.
Crypto flag	1N	0: DES encryption, 1: DES decryption
Algorithm:
Mode of operation	1N	1: ECB – Electronic Code Book
2: CBC – Cipher Block Chaining
3: CFB – Cipher Feed Back
4: OFB – Output Feed Back
5: PCBC
Key flag	1N	0: ZEK – Zone Encryption Key
ZEK	16H/1A+32H/1A+48H	ZEK encrypted under LMK pair 30-31.

Input data format	1N	0: Binary, 1: expanded Hex
Onput data format	1N	0: Binary, 1: expanded Hex
Pad  mode	1N	Present only when message block number is 0, or 3.
0: If data lengths are exact multiples of eight bytes, not padding; otherwise padding characters defined by the following field (Pad character)until data lengths are exact multiples of eight bytes.
1: If  data lengths are exact multiples of eight bytes ,Padding another eight bytes defined  by the following field( (Pad character)； otherwise  padding characters defined by the following field(Pad character),until data lengths are exact multiples of eight bytes.
Pad character	4H
	Present only when message block number is 0, or 3。 .e.g.
          Pad mode|Pad character|Pad count flag
ANSI X9.19  :  0         0000           0
ANSI X9.23  :  1         0000           1
PBOC MAC  :  1         8000           0 
etc.
Pad count flag	1N	Present only when message block number is 0, or 3. 
0：Last byte is not padding count 
1：Last byte is padding count,and the count is within the range of X’01’ to X’08’
IV	16H	Initialization value, present only when Algorithm:Mode of operation is 2, 3,4or 5.
Message length	3H	Actual Message length in bytes.
Binary: nB, 
Expanded Hex : n/2 n must be even.
Message block	nB	The clear/cipher text message block.
RESPONSE MESSAGE

Message header	mA  	Returned to the Host unchanged.
Response code	2A	Value E1.
Error code   	2N	00 : No Errors.
05: Invalid message block number
10 : ZEK/TEK parity error
12 : No keys loaded in user storage.
13 : LMK error. Report to supervisor.
15 : Error in input data.
21 : Invalid user storage index.
26: Invalid key scheme
80: Output error
Onput data format	1N	0: Binary, 1: expanded Hex
Message length	3H	The encrypted/decrypted message length in bytes.
Binary: nB, expanded Hex: n/2 n must be even.
Message block	nB	The encrypted/decrypted text message block.
Next Initial Vector	16H	The next initial vector. Present only in mode of operation: CBC, OFB, CFB, PCBC and Message block number is 1 or 2.
*/
int RacalCmdE0(int hsmSckHDL, PUnionSJL06 pSJL06, char *dataBlock, char *CrypToFlag,char *modeOfAlgorithm,
	char *ZEK, char *inDataType,char *outDataType,char *padMode,char *padChar,char *padCountFlag,char *IV,
	char *lenOfMSG,char *MSGBlock,char *datafmt,char *lenOfMSG1,char *MSGBlock1,char *IV1,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len,msglength;
	
	if (ZEK==NULL || dataBlock==NULL || CrypToFlag == NULL|| modeOfAlgorithm==NULL || inDataType == NULL||  \
		outDataType == NULL || padMode == NULL || padChar==NULL || padCountFlag == NULL ||  \
		lenOfMSG == NULL ||MSGBlock == NULL)
	{
		UnionUserErrLog("in RacalCmdE0:: parameter error!\n");
		return(errCodeParameter);
	}
	if (inDataType[0] != '0' && inDataType[0] != '1')
	{
		UnionUserErrLog("in RacalCmdE0:: inDataType [%c] parameter error!\n", inDataType[0]);
		return(errCodeParameter);
	}
	if (outDataType[0] != '0' && outDataType[0] != '1')
	{
		UnionUserErrLog("in RacalCmdE0:: outDataType parameter error!\n");
		return(errCodeParameter);
	}
	if ((strncmp(modeOfAlgorithm,"1",1) != 0)&& (strlen(IV) != 16))
	{
		UnionUserErrLog("in RacalCmdE0:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"E0",2);
	hsmCmdLen = 2;
	memcpy(hsmCmdBuf+hsmCmdLen,dataBlock,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,CrypToFlag,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,modeOfAlgorithm,1);
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
			UnionUserErrLog("in RacalCmdE0:: invalid [%s]\n",ZEK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,ZEK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,inDataType,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,outDataType,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,padMode,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,padChar,4);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf+hsmCmdLen,padCountFlag,1);
	hsmCmdLen += 1;
	if (strlen(IV) == 16)
	{
		memcpy(hsmCmdBuf+hsmCmdLen,IV,16);
		hsmCmdLen += 16;
	}
	memcpy(hsmCmdBuf+hsmCmdLen,lenOfMSG,3);
	hsmCmdLen += 3;
	
	msglength = UnionOxToD(lenOfMSG);
	
	if (inDataType[0] == '0')  //modify by hzh in 2011.5.4 增加对输入类型的判断
	{
		memcpy(hsmCmdBuf+hsmCmdLen,MSGBlock,msglength);
		hsmCmdLen += msglength;
	}
	else {
		memcpy(hsmCmdBuf+hsmCmdLen,MSGBlock,2*msglength);
		hsmCmdLen += 2*msglength;
	}
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdE0:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"E1",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdE0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdE0:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(datafmt,hsmCmdBuf+4,1);
	memcpy(lenOfMSG1,hsmCmdBuf+4+1,3);

	len = UnionOxToD(lenOfMSG1);
	
	if(outDataType[0] == '0')   //modify by hzh in 2011.5.4,增加对输出格式的判断
	{
		memcpy(MSGBlock1,hsmCmdBuf+4+1+3,len);
	}
	else {
		len = 2*len;
	memcpy(MSGBlock1,hsmCmdBuf+4+1+3,len);
	}
	if (memcmp(modeOfAlgorithm,"1",1) != 0 && (memcmp(dataBlock,"1",1) == 0 || memcmp(dataBlock,"2",1) == 0))
	{
		memcpy(IV1,hsmCmdBuf+4+1+3+len,16);
	}else
		strcpy(IV1,"");
	return(ret);
}

/*
采用离散MAK计算MAC

输  入  消  息  格  式

输入域	长度	类型	说　明
命令代码	2	A	值“G1”
ZAK	16H/1A+32H/1A+48H	N	被LMK26-27对加密
离散数据	16	H	用于计算离散密钥的数据
MAC数据长度	3H	H	用于计算MAC的数据
MAC数据	nB	B	MAC数据
							输    出    消    息
输出域	长度	类型	说　明
响应代码	2	A	值“G2”
错误代码	2	H	
MAC	16	H	返回的MAC值
计算流程：
1.	用ZAK密钥对离散数据采用IC卡标准离散算法进行离散，如果ZAK为64bit长，则直接对离散数据进行DES加密即可。
2.	用离散密钥对MAC数据采用Ansi9.19（ZAK双倍长以上）或9.9（ZAK单倍长）算法计算MAC；
*/
int RacalCmdG1(int hsmSckHDL, PUnionSJL06 pSJL06, char *ZAK, char *scatData,char *lenOfmac,char *macData, char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	int		lenmac;
	
	if (ZAK==NULL || scatData==NULL || lenOfmac == NULL|| macData==NULL )
	{
		UnionUserErrLog("in RacalCmdG1:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"G1",2);
	hsmCmdLen = 2;
	switch (len = strlen(ZAK))
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
			UnionUserErrLog("in RacalCmdG1:: invalid [%s]\n",ZAK);
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
		
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdG1:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"G2",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdG1:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdG1:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	return(ret-4);	
}


/*
指令功能：邮政PINBLOCK与ANSI9.8格式相互转换
输  入  消  息  格  式
输入域	长度	与类型		说　明
消息头	mA	
命令代码	2A	Value “ Y1 ”
原ZPK	16H or
1A+32H or 1A+48H
	在LMK对（06-07）下加密
目的ZPK	16H or
1A+32H or 1A+48H
	在LMK对（06-07）下加密
源PIN块	16H or 32H	源ZPK下加密的源PIN块。
源PINBLOCK格式	1N	1：ANSI9.8格式
2：邮政PIN专用格式和专用算法
目的PINBLOCK格式	1N	1：ANSI9.8格式
2：邮政PIN专用格式和专用算法
账号	12N	用户主帐号，仅用到最右12位，不含较验位;
输  出  消  息  格  式
消息头	mA	
响应代码	2A	Value “ Y2 ”
错误代码	2N	
PINBLOCK	16H或32H	当转化成ANSI9.8格式后，PINBLOCK是16H，当转化成邮政PIN格式后是32H。
注：1、邮政PINBLOCK为不带帐号加密。
2、当把ANSI9.8格式转换邮政专用格式时必须为不带帐号加密。
*/
int RacalCmdY1(int hsmSckHDL,PUnionSJL06 pSJL06,char *OriZPK,char *DesZPK,char *OriPinBlock,char *accNo,char *pinBlock,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[2048+1];
	int		hsmCmdLen = 0;
	int		len;
	
	if (OriZPK == NULL || DesZPK == NULL || OriPinBlock == NULL || accNo == NULL)
	{
		UnionUserErrLog("in RacalCmdY1:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"Y1",2);
	hsmCmdLen = 2;
	switch (len = strlen(OriZPK))
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
			UnionUserErrLog("in RacalCmdY1:: invalid [%s]\n",OriZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,OriZPK,len);
	hsmCmdLen += len;
	switch (len = strlen(DesZPK))
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
			UnionUserErrLog("in RacalCmdY1:: invalid [%s]\n",DesZPK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,DesZPK,len);
	hsmCmdLen += len;
	memcpy(hsmCmdBuf+hsmCmdLen,OriPinBlock,32);
	hsmCmdLen += 32;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,12);
	hsmCmdLen += 12;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdY1:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"Y2",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdY1:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdY1:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlock,hsmCmdBuf+4,16);
	return(ret-4);
}

/*
产生随机数指令
*/
int RacalCmdY3(int hsmSckHDL,PUnionSJL06 pSJL06,int min,int max,int Cnt,int isRepeat,char *RandNo,char *errCode)
{
	int 	ret = -1;
	const char *Cmd = "Y3";
	char hsmCmdBuf[1024] = {0};
	
	sprintf(hsmCmdBuf,"%s%03d%03d%03d%d",Cmd,min,max,Cnt,isRepeat);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,strlen(hsmCmdBuf),hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdY3:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if (memcmp(hsmCmdBuf,"Y4",2) != 0)
	{
		UnionUserErrLog("in RacalCmdY3:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdY3:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(RandNo,hsmCmdBuf+4,3);
	RandNo[3] = 0;
	return( ret -4 );	
	
}


/*一次性发送加密机数据块最大长度*/
#ifndef MAXMSGBLOCK 
#define MAXMSGBLOCK 512
#endif

// modified by Fenglk 2011-11-29
/*
int newRacalCmdE0(int hsmSckHDL, PUnionSJL06 pSJL06,int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,unsigned long *encryDataLen,int sizeOfEncryptedData)
*/
int newRacalCmdE0(int hsmSckHDL, PUnionSJL06 pSJL06,int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData)
// end of modification 2011-11-29
{
	int 	nLoop,i;
	int 	ret;
	int 	p_len =0;
	int	lastDataLen;
	
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
		UnionUserErrLog("in RacalCmdE0 : SizeOfBuffer[%d] > sizeOfData[%d]\n",ret,sizeOfEncryptedData);
		return (errCodeSmallBuffer);		
	}

	
	if ( MsgLen <= MAXMSGBLOCK )
	{
		return RacalCmdE0_f(hsmSckHDL,pSJL06,crytoFlag,blockFlag,encrypMode,zekLen,\
			Zek,datatype,IV,MsgLen,Msg,\
			encrypData,encryDataLen,sizeOfEncryptedData);
	}
	
	if ( MsgLen > MAXMSGBLOCK )
	{
		nLoop = MsgLen / MAXMSGBLOCK;	
		
		for ( i = 0; i < nLoop; i++ )
		{
				ret = RacalCmdE0_f(hsmSckHDL,pSJL06,crytoFlag,blockFlag,encrypMode,zekLen,\
					Zek,datatype,IV,MAXMSGBLOCK,Msg + i *MAXMSGBLOCK,\
					encrypData + i*MAXMSGBLOCK,(unsigned long *)&p_len,sizeOfEncryptedData);	
				if ( ret < 0  )
				{
					UnionUserErrLog("in RacalCmdE0 : RacalCmdE0_f error !!\n");
					return ( ret );		
				}
				*encryDataLen += p_len; 
		}
		
		lastDataLen = MsgLen % MAXMSGBLOCK;
		if ( lastDataLen != 0 )
		{
			ret = RacalCmdE0_f(hsmSckHDL,pSJL06,crytoFlag,blockFlag,encrypMode,zekLen,\
					Zek,datatype,IV,lastDataLen,Msg + i *MAXMSGBLOCK,\
					encrypData + i*MAXMSGBLOCK,(unsigned long *)&p_len,sizeOfEncryptedData);	
		
			if ( ret < 0  )
			{
				UnionUserErrLog("in RacalCmdE0 : RacalCmdE0_f error !!\n");
				return ( ret );		
			}
		
			*encryDataLen += p_len; 
		}
	}
	return *encryDataLen;
}


// modified by Fenglk 2011-11-29
/*
int RacalCmdE0_f(int hsmSckHDL, PUnionSJL06 pSJL06,int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,unsigned long *encryDataLen,int sizeOfEncryptedData)
*/
int RacalCmdE0_f(int hsmSckHDL, PUnionSJL06 pSJL06,int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData)
// end of modification 2011-11-29
{
	int 	offset,ret;
	char	iMsg_Len[10];
	char	hsmCmd[2+1];
	unsigned char hsmCmdBuf[1024 + 1];
	char	v_zek[60 + 1];
	
	char 	errCode[3] = {0};
	

	strcpy(hsmCmd,"E0");
		 	
	memset(hsmCmdBuf,0x00,sizeof(hsmCmdBuf));
	
	if( blockFlag < 0 ||  blockFlag > 3 )
	{
		UnionUserErrLog("in RacalCmdE0_f:: parameter error! blockFlag[%d]\n",blockFlag);
		return(errCodeParameter);
	}
	offset = 2;
	if( encrypMode < 1 ||encrypMode > 5 )
	{
		UnionUserErrLog("in RacalCmdE0_f:: parameter  encrypMode=[%d]error!\n",encrypMode);
		return(errCodeParameter);
	}
    	sprintf(hsmCmdBuf ,"%s%01d%01d%01d%01d",hsmCmd,blockFlag,crytoFlag,encrypMode,0);
	offset += 4;
	

	memset(v_zek,0,sizeof(v_zek));
	ret = UnionPutKeyIntoRacalKeyString(Zek,v_zek,sizeof(v_zek));
	if ( ret < 0 )
	{
		UnionUserErrLog("in RacalCmdE0_f:: UnionTranslateHsmKeyKeyScheme!\n");
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
		UnionUserErrLog("in RacalCmdE0_f:: parameter  datatype=[%d]error!\n",datatype);
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
/* 	if ((ret = UnionCommWithHsmSvr(hsmCmd,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
 	{
  		UnionUserErrLog("in RacalCmdE0:: UnionCommWithHsmSvr! ret = [%d]\n",ret);
  		return(ret);
 	}

	ret -= 4;
*/
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdE0_f:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"E1",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdE0_f:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdE0_f:: errCode hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(-1);
	}

	memset(iMsg_Len,0,sizeof(iMsg_Len));
	memcpy(iMsg_Len,hsmCmdBuf + 4 + 1,3);

	*encryDataLen = UnionOxToD(iMsg_Len);
	if ( datatype == 1 )
		*encryDataLen *= 2;
	if ( encrypData != NULL && ret < sizeOfEncryptedData )
		memcpy(encrypData,hsmCmdBuf + 8,*encryDataLen);

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
   	 errCode：密码机返回码
   返回值:
   	 <0：函数执行失败，值为失败的错误码
   	 >=0：函数执行成功
 */

int RacalCmdY4 (int hsmSckHDL,PUnionSJL06 pSJL06, char *kek, char *rootKey, \
  char *keyType, int discreteNum, char *discreteData1, char *discreteData2, \
    char *discreteData3, char *keyByKek, char *keyByLmk, char *checkValue,char *errCode)
{
	char hsmCmdBuf[2048];
	int hsmCmdLen = 0; 
	int ret;
	if ((kek == NULL) || (rootKey == NULL) || (discreteNum <= 0) || (discreteNum > 3) || (discreteData1 == NULL) || (keyByKek == NULL) || (errCode == NULL))
	{
		UnionUserErrLog("in RacalCmdY4:: parameter error!\n");
		return (errCodeParameter);
	}
	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf, "Y4", 2);      /*命令代码*/
	hsmCmdLen = 2;
	
	UnionNullLog("in RacalCmdY4 kek[%s]\n",kek);
	
	if ((ret = UnionPutKeyIntoRacalKeyString(kek, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*保护密钥*/
	{
		UnionUserErrLog("in RacalCmdY4:: kek UnionPutKeyIntoRacalKeyString [%s]!\n", kek);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(rootKey, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*根密钥*/
	{
		UnionUserErrLog("in RacalCmdY4:: rootKey UnionPutKeyIntoRacalKeyString [%s]!\n", rootKey);
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
			UnionUserErrLog("in RacalCmdY4:: parameter error!\n");
			return (errCodeParameter);
		}
		memcpy(hsmCmdBuf + hsmCmdLen, discreteData2, 16);
		hsmCmdLen += 16;
		if (discreteNum == 3)
		{
			if (discreteData3 == NULL)
			{
				UnionUserErrLog("in RacalCmdY4:: parameter error!\n");
				return (errCodeParameter);
			}
			memcpy(hsmCmdBuf + hsmCmdLen, discreteData3, 16);   
			hsmCmdLen += 16;
		}
	}
	/*向密码机发送请求报文*/
	
/*******************************************************/
	UnionNullLog("in RacalCmdY4 hsmCmdBuf[%s]\n",hsmCmdBuf);
/*******************************************************/

	if((ret = UnionLongConnSJL06Cmd(hsmSckHDL, pSJL06, hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdY4:: UnionLongConnSJL06Cmd!\n");
		return(hsmCmdLen);
	}
	hsmCmdBuf[ret] = '\0';
	/*判断响应码*/
	if ((memcmp(hsmCmdBuf, "Y5", 2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdY4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode, hsmCmdBuf + 2, 2);
	memcpy(keyByLmk, hsmCmdBuf + 4, 32);  /*返回keyByLmk*/
	memcpy(keyByKek, hsmCmdBuf + 36, 32);  /*返回keyByKek*/
	memcpy(checkValue,hsmCmdBuf+36+32,16);
	
/*******************************************************/
	UnionNullLog("\n");
	UnionNullLog("discreteData[%s]\t\t",discreteData1);
	hsmCmdBuf[36] = 0;
	UnionNullLog("ByLmk[%s]\t\t",hsmCmdBuf+4);
	UnionNullLog("ByKEK[%s]\n",keyByKek);
	UnionNullLog("\n");
/*******************************************************/

	
	return (0);
}

/*
函数功能：
	A5指令，输入密钥明文的几个成份合成最终的密钥，同时用相应的LMK密钥对加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	partKeyNum：密钥成分的数量
	partKey：存放各个密钥成分的数组缓冲
输出参数：
	keyByLMK：合成的密钥密文，由LMK加密
	checkValue：合成的密钥的校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无keyByLMK和checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回keyByLMK和checkValue的总长度
*/
int RacalCmdA5(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	int		i;
	
	if ((keyByLMK == NULL) || (checkValue == NULL) || (pSJL06 == NULL))
	{
		UnionUserErrLog("in RacalCmdA5:: wrong parameters!\n");
		return(errCodeParameter);
	}
	if (partKeyNum < 2 || partKeyNum > 9)
	{
		UnionUserErrLog("in RacalCmdA5:: partKeyNum [%d] must between 2 and 9!\n",partKeyNum);
		return(errCodeParameter);
	}
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"A5",2);
	hsmCmdLen = 2;
	// 密钥成分数量
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// 密钥类型代码
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA5:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// 密钥长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA5:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	for (i = 0; i < partKeyNum; i++)
	{
		// 密钥明文
		/*
		if ((ret = UnionGenerateX917RacalKeyString(keyLength,partKey[i],hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA5:: UnionGenerateX917RacalKeyString for [%s][%d]\n",partKey[i],keyLength);
			return(ret);
		}
		hsmCmdLen += ret;
		*/
		UnionLog("in RacalCmdA5:: partKey%d[%s]\n",i,partKey[i]);
		memcpy(hsmCmdBuf+hsmCmdLen,partKey[i],strlen(partKey[i]));
		hsmCmdLen += strlen(partKey[i]);
	}	

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA5:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A6",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA5:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA5:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (keyLength)
	{
		case	con64BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+16,16);
			return(16+16);
		case	con128BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+1+32,16);
			return(32+16);
		case	con192BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+1+48,16);
			return(48+16);
		default:
			UnionUserErrLog("in RacalCmdA5:: unknown key length!\n");
			return(errCodeParameter);
	}
}

int SJL06Cmd30(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *pkIndex,char *pk,int dataLen,char *data,char *encData,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[4048+1];
	int		hsmCmdLen = 0;
	int		len;
	
	if ((pk == NULL) || (dataLen <= 0) || (data == NULL) || ((flag != '0') && (flag != '1')))
	{
		UnionUserErrLog("in SJL06Cmd30:: parameter error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
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
		UnionUserErrLog("in SJL06Cmd30:: dataLen [%d] too long!\n",dataLen/2);
		return(errCodeSmallBuffer);
	}
	aschex_to_bcdhex(data,dataLen,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += dataLen/2;
	if ((pkIndex == NULL) || (strlen(pkIndex) ==0))
	{
		aschex_to_bcdhex(pk,strlen(pk),hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += strlen(pk)/2;
	}

	UnionProgramerMemLog("in SJL06Cmd30:: req =",hsmCmdBuf,hsmCmdLen);
	
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in SJL06Cmd30:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"31",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in SJL06Cmd30:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in SJL06Cmd30:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
		
	if ((len = UnionConvertIntoLen(hsmCmdBuf+4,4)) < 0)
	{
		UnionUserErrLog("in SJL06Cmd30:: signLen error!\n");
		return(len);
	}
	
	bcdhex_to_aschex(hsmCmdBuf+4+4,len,encData);
	return(len*2);
}
