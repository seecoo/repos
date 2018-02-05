// Author:	Wolfgang Wang
// Date:	2003/10/09

// 2004-3-9���¼�÷����1.0�汾�Ļ������޸ģ�������ƴװ����ָ��ĺ���
// DG��CA��JE��JC��JG��JA��EE��DE��BA��NG��DA��EA��DC��EC��BC��BE��CW��CY��EW��EY

// 2006-4-9��������������
// MUָ��

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

/* 2007/5/14 ���� */
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
�ߡ�	ת��DES��Կ��������Կ���ܵ���Կ����
���ڷַ���Կ��

������	����	����	˵����
�������	2	A	ֵ"3B"
��Կ����	32	H	������Կ���ܵ�DES��Կ
����Կ	n	B	


�����	����	����	˵����
��Ӧ����	2	A	"3C"
�������	2	H	
���ֵ	16	H	DES��Կ����64bits��0
��Կ����	4	N	DES��Կ���ĵ��ֽ���
��Կ����	n	B	�ù�Կ���ܵ�DES��Կ
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
����	ת��DES��Կ���ӹ�Կ���ܵ�����Կ����
���ڽ�����Կ��

������	����	����	˵����
�������	2	A	ֵ"3A"
˽Կ����	2	N	"00"��"20"����������ڵ�˽Կ
��Կ����	4	N	DES��Կ���ĵ��ֽ���
��Կ����	n	B	�ù�Կ���ܵ�DES��Կ


�����	����	����	˵����
��Ӧ����	2	A	"3B"
�������	2	H	
DES��Կ	32	H	������Կ���ܵ�DES��Կ
���ֵ	16	H	DES��Կ����64bits��0
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
	//modify by hzh in 2012.01.13, ������ܻ������󣬹�Կȡ��ƫ�Ƶ�����. (��30818902818100..., ��߶���00,�ұ�ĩβ�ֽڶ�ʧ) 
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
/* 2007/5/14 ���ӽ��� */

/*
���ܣ����ɹ�Կ���ܵ�PINת����ANSI9.8��׼

������	����	����	˵����
�������	2	A	ֵ"43"
˽Կ����	2	N	ָ����˽Կ�����ڽ���PIN��������
��Կ����	1	N	1��TPK  2��ZPK
ZPK/TPK	16/1A+32/1A+48	H	���ڼ���PIN����Կ
PIN����	1	N	1��ANSI9.8
PAN	16	H	�û����ʺ�
PIN����	n	B	����Կ���ܵ�PIN��������


�����	����	����	˵����
��Ӧ����	2	A	ֵ"44"
�������	2	H	
PIN����	16	H	���ص�PIN����--ANSI9.8
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
2007/4/11,������������
50ָ��
���ܣ���EDK��Կ�ӽ������ݣ�����ǽ���״̬�����������Ȩ�²��ܴ������򱨴�

��  ��  ��  Ϣ  ��  ʽ

������		����	����	˵����
��Ϣͷ		m	A	
�������	2	A	ֵ"50"
Flag		1	N	0������
			1������
EDK		16 or 32
		1A+32 or 1A+48	
			H	LMK24-25����
DATA_length	4	N	���������ֽ�����8�ı�������Χ��0008-4096
DATA		n*2	H	�����ܻ���ܵ����ݣ���BCD���ʾ��

��  ��  ��  Ϣ  ��  ʽ

�����	����	����	˵����
��Ϣͷ		M	A	
��Ӧ����	2	A	ֵ"51"
�������	2	N	
DATA		n*2	H	������

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

// 20051206������������
// ��ZMK���ܵ���Կת��ΪLMK���ܵ�ZAK/ZEK
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
��  ��  ��  Ϣ  ��  ʽ
������	����	����	˵����
�������		2A	ֵΪ"TI"��
ԴKEY��־	1	N	1��TPK           2��ZPK
ԴKEY	16
1A+32
1A+48	H	��KEY��־Ϊ1ʱΪTPK��LMK�ԣ�14-15���¼��ܣ�
��KEY��־Ϊ2ʱΪZPK��LMK�ԣ�06-07���¼��ܡ�
Ŀ��KEY��־	1	N	1��TPK           2��ZPK
Ŀ��KEY	16
1A+32
1A+48	H	��KEY��־Ϊ1ʱΪTPK��LMK�ԣ�14-15���¼��ܣ�
��KEY��־Ϊ2ʱΪZPK��LMK�ԣ�06-07���¼��ܡ�
ԴPIN��	16	H	ԴZPK�¼��ܵ�ԴPIN�顣
ԴPIN��ʽ	1	N	1��ANSI9.8��ʽ    2��IBM��ʽ
ԴPAN	16	H	�û����ʺţ����õ�����12λ; ��ԴPIN��ʽΪ1ʱ�д���
Ŀ��PIN��ʽ	1	N	1��ANSI9.8��ʽ    2��IBM��ʽ
Ŀ��PAN	16	H	�û����ʺţ����õ�����12λ; ��ԴPIN��ʽΪ1ʱ�д���

��  ��  ��  Ϣ  ��  ʽ
�����	����	����	˵����
��Ӧ����	2	A	ֵ"TJ"
�������	2	H	
PIN����	16	H	TPK��ZPK�¼���
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

// ��һ��ZMK���ܵ���Կת��ΪLMK����
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
	if (keyByLmkStrLen >= ret)	// û��У��ֵ
		return(0);
	if (ret - keyByLmkStrLen > 16)
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,16);
	else
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,ret-keyByLmkStrLen-4);
	return(0);
}

// ��һ��ZMK���ܵ���Կת��ΪLMK����,֧�ֹ����㷨SM1��SFF33
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
	if (keyByLmkStrLen >= ret)	// û��У��ֵ
		return(0);
	if (ret - keyByLmkStrLen > 16)
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,16);
	else
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,ret-keyByLmkStrLen-4);
	return(0);
}

// ��������MAC��ָ��
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

// ��ZMK���ܵ�ZPKת��ΪLMK����
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

// ����ZAK/ZEK
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
			// 2008/07/11������������
			sprintf(tmpBuf,"MSCmdOf%sNotStandard",pSJL06->staticAttr.hsmGrpID);
			if (UnionReadIntTypeRECVar(tmpBuf) > 0)
			{
				hsmCmdBuf[hsmCmdLen] = 'X';
				hsmCmdLen++;
			}
			// 2008/07/11,���ӽ���
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
�������ܣ�
	DGָ���PVK����PIN��PVV(PIN Verification Value)��
	���õļ��ܱ�׼ΪVisa Method
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinLength��LMK���ܵ�PIN���ĳ���
	pin��LMK���ܵ�PIN����
	pvkLength��LMK���ܵ�PVK�Գ���
	pvk��LMK���ܵ�PVK��
	lenOfAccNo���ͻ��ʺų���
	accNo���ͻ��ʺ�
���������
	pvv��������PVV
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���PVV
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������PVV�ĳ���
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
	
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DG",2);
	hsmCmdLen = 2;
	// LMK���ܵ�PVK��
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PIN
	memcpy(hsmCmdBuf+hsmCmdLen,pin,pinLength);
	hsmCmdLen += pinLength;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI��0��6֮�䣩
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
	// ������PVV
	memcpy(pvv,hsmCmdBuf+4,4);
	return(4);
}

/*
�������ܣ�
	CAָ���һ��TPK���ܵ�PINת��Ϊ��ZPK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	tpkLength��LMK���ܵ�TPK����
	tpk��LMK���ܵ�TPK
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinFormat1��ת��ǰ��PIN��ʽ
	pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
	pinFormat2��ת�����PIN��ʽ
���������
	pinBlockByZPK��ת������ZPK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByZPK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByZPK�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CA",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN��󳤶�
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// ��TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// ת��ǰ��PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,2);
	hsmCmdLen += 2;
	// ת�����PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
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
	// ת����ZPK���ܵ�PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4+2,16);
	return(16);
}

/*
�������ܣ�
	JEָ���һ��ZPK���ܵ�PINת��Ϊ��LMK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinFormat��PIN��ʽ
	pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ��ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
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
	// ת����LMK���ܵ�PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
�������ܣ�
	JCָ���һ��TPK���ܵ�PINת��Ϊ��LMK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	tpkLength��LMK���ܵ�TPK����
	tpk��LMK���ܵ�TPK
	pinFormat��PIN��ʽ
	pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ��TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
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
	// ת����LMK���ܵ�PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
�������ܣ�
	JGָ���һ��LMK���ܵ�PINת��Ϊ��ZPK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinFormat��PIN��ʽ
	pinLength��LMK���ܵ�PIN���ĳ���
	pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByZPK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByZPK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByZPK�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JG",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// ��LMK���ܵ�PIN����
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
	// ת����ZPK���ܵ�PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4,16);
	return(16);
}

/*
�������ܣ�
	JAָ��������һ��PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinLength��Ҫ��������ɵ�PIN���ĵĳ���
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK�����������PIN�����ģ���LMK����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JA",2);
	hsmCmdLen = 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN����
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
	// ���������PIN�����ģ���LMK����
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
�������ܣ�
	EEָ���IBM��ʽ����һ��PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	minPINLength����СPIN����
	pinValidData���û��Զ�������
	decimalizationTable��ʮ����������ʮ��������ת����
	pinOffset��PIN Offset������룬�Ҳ�'F'
	pvkLength��LMK���ܵ�PVK����
	pvk��LMK���ܵ�PVK
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK�����������PIN�����ģ���LMK����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
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
	// ���������PIN�����ģ���LMK����
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
�������ܣ�
	DEָ���IBM��ʽ����һ��PIN��Offset
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	minPINLength����СPIN����
	pinValidData���û��Զ�������
	decimalizationTable��ʮ����������ʮ��������ת����
	pinLength��LMK���ܵ�PIN���ĳ���
	pinBlockByLMK����LMK���ܵ�PIN����
	pvkLength��LMK���ܵ�PVK����
	pvk��LMK���ܵ�PVK
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinOffset��PIN Offset������룬�Ҳ�'F'
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinOffset
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinOffset�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
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
	// ������PIN��Offset
	memcpy(pinOffset,hsmCmdBuf+4,12);
	return(12);
}

/*
�������ܣ�
	BAָ�ʹ�ñ�������Կ����һ��PIN����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinCryptogramLen�����ܺ��PIN���ĵĳ���
	pinTextLength��PIN���ĵĳ���
	pinText��PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BA",2);
	hsmCmdLen = 2;
	
	// ���PIN����
	if ((pinCryptogramLen >= sizeof(tmpBuf)) || (pinTextLength >= sizeof(tmpBuf)))
	{
		UnionUserErrLog("in RacalCmdBA:: pinCryptogramLen = [%d] or pinTextLength = [%d] too long!\n",pinCryptogramLen,pinTextLength);
		return(errCodeParameter);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	memset(tmpBuf,'F',pinCryptogramLen);
	memcpy(tmpBuf,pinText,pinTextLength);

	// �����PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	// 12λ�ͻ��ʺ�
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
	// LMK���ܵ�PIN����
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
�������ܣ�
	NGָ�ʹ�ñ�������Կ����һ��PIN����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinCryptogramLen��LMK���ܵ�PIN���ĵĳ���
	pinCryptogram��LMK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	referenceNumber����LMK18-19�����ʺŵõ���ƫ��ֵ
	pinText��PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinText
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinText�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"NG",2);
	hsmCmdLen = 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdNG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PIN����
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
	// ��LMK18-19�����ʺŵõ���ƫ��ֵ
	memcpy(referenceNumber,hsmCmdBuf+4+pinCryptogramLen,12);
	// ����PIN����
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
	// PIN����
	memcpy(pinText,tmpBuf,i);
	return(i);
}

/*
�������ܣ�
	DAָ���IBM��ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByTPK��TPK���ܵ�PIN����
	minPINLength����СPIN����
	pinValidData���û��Զ�������
	decimalizationTable��ʮ����������ʮ��������ת����
	pinOffset��IBM Offset������룬�Ҳ�'F'
	tpkLength��LMK���ܵ�TPK����
	tpk��LMK���ܵ�TPK
	pvkLength��LMK���ܵ�PVK����
	pvk��LMK���ܵ�PVK
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DA",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ���PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
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
�������ܣ�
	EAָ���IBM��ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByZPK��ZPK���ܵ�PIN����
	minPINLength����СPIN����
	pinValidData���û��Զ�������
	decimalizationTable��ʮ����������ʮ��������ת����
	pinOffset��IBM Offset������룬�Ҳ�'F'
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pvkLength��LMK���ܵ�TPK����
	pvk��LMK���ܵ�PVK
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EA",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ���PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// ��СPIN����
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// �û��Զ�������
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
�������ܣ�
	DCָ���VISA��ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByTPK��TPK���ܵ�PIN����
	pvv���ն�PIN��4λVISA PVV
	tpkLength��LMK���ܵ�TPK����
	tpk��LMK���ܵ�TPK
	pvkLength��LMK���ܵ�PVK����
	pvk��LMK���ܵ�PVK
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	// �ն�PIN��4λVISA PVV
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
�������ܣ�
	ECָ���VISA��ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByZPK��ZPK���ܵ�PIN����
	pvv���ն�PIN��4λVISA PVV
	zpkLength��LMK���ܵ�ZPK
	zpk��LMK���ܵ�ZPK
	pvkLength��LMK���ܵ�PVK����
	pvk��LMK���ܵ�PVK
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ�PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	// �ն�PIN��4λVISA PVV
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
�������ܣ�
	BCָ��ñȽϷ�ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByTPK��TPK���ܵ�PIN����
	tpkLength��LMK���ܵ�TPK
	tpk��LMK���ܵ�TPK
	pinByLMKLength������PIN���ĳ���
	pinByLMK������PIN���ģ���LMK02-03����
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BC",2);
	hsmCmdLen = 2;
	// LMK���ܵ�TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// ����PIN���ģ���LMK02-03����
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
�������ܣ�
	BEָ��ñȽϷ�ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByZPK��ZPK���ܵ�PIN����
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinByLMKLength������PIN���ĳ���
	pinByLMK������PIN���ģ���LMK02-03����
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BE",2);
	hsmCmdLen = 2;
	// LMK���ܵ�ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK���ܵ�PIN����
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN��ʽ
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12λ�ͻ��ʺ�
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// ����PIN���ģ���LMK02-03����
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
�������ܣ�
	CWָ�����VISA��У��ֵCVV 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	cardValidDate��VISA������Ч��
	cvkLength��CVK�ĳ���
	cvk��CVK
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	cvv�����ɵ�VISA����CVV
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���cvv
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������cvv�ĳ���
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CW",2);
	hsmCmdLen = 2;
	// LMK���ܵ�CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCW:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// �ͻ��ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA������Ч��
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA�������̴���
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
	// ���ɵ�VISA����CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	return(3);
}

/*
�������ܣ�
	CYָ���֤VISA����CVV 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	cvv������֤��VISA����CVV
	cardValidDate��VISA������Ч��
	cvkLength��CVK�ĳ���
	cvk��CVK
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
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
	// ������
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CY",2);
	hsmCmdLen = 2;
	// LMK���ܵ�CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCY:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ����֤��VISA����CVV
	memcpy(hsmCmdBuf+hsmCmdLen,cvv,3);
	hsmCmdLen += 3;
	// �ͻ��ʺ�
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// �ָ���
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA������Ч��
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA�������̴���
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
�������ܣ�
	EWָ���˽Կǩ�� 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	indexOfVK��˽Կ������
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	vkLength��LMK���ܵ�˽Կ����
	vk��LMK���ܵ�˽Կ
���������
	signature�����ɵ�ǩ��
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���signature
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������signature�ĳ���

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
	// ������
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
	// LMK���ܵ�˽Կ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLength);
	hsmCmdLen += 4;
	// LMK���ܵ�˽Կ
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
	// ���ɵ�ǩ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	// ���ɵ�ǩ��
	memcpy(signature,hsmCmdBuf+4+4,atoi(tmpBuf));
	return(atoi(tmpBuf));
}


�������ܣ�
	EYָ��ù�Կ��֤ǩ��
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	macOfPK����Կ��MACֵ
	signatureLength������֤��ǩ���ĳ���
	signature������֤��ǩ��
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	publicKeyLength����Կ�ĳ���
	publicKey����Կ
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������

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
	// ������
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
�������ܣ�
	BUָ�����һ����Կ��У��ֵ
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	key��LMK���ܵ���Կ����
���������
	checkValue�����ɵ���ԿУ��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������checkValue�ĳ���
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
	// ������
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;

	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	// ������Կ��LMK��ָʾ��������Կ���ʹ�������2λ
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
	// ��Կ���ȱ�־
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
	// LMK���ܵ���Կ����
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
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ��Կ���ʹ���
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// У��ֵ���ͱ�־
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
        // ������
        memcpy(hsmCmdBuf,"BU",2);
        hsmCmdLen = 2;
	
	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }
	
        // ������Կ��LMK��ָʾ��������Կ���ʹ�������2λ
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
                        UnionUserErrLog("in RacalCmdBU:: unknown key length!\n");
                        return(errCodeParameter);
        }
        hsmCmdLen += 1;
        // LMK���ܵ���Կ����
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
                // �ָ���
                hsmCmdBuf[hsmCmdLen] = ';';
                hsmCmdLen += 1;
                // ��Կ���ʹ���
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
�������ܣ�
	BUָ�����һ����Կ��У��ֵ
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	key��LMK���ܵ���Կ����
���������
	checkValue�����ɵ���ԿУ��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������checkValue�ĳ���
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
	// ������
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;
	// ������Կ��LMK��ָʾ��������Կ���ʹ�������2λ
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
	// ��Կ���ȱ�־
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
	// LMK���ܵ���Կ����
	if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
		return(ret);
	}
	hsmCmdLen += ret;
	/*
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ��Կ���ʹ���
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUNormal:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// У��ֵ���ͱ�־
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
	// ������
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;
	
	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	// ������Կ��LMK��ָʾ��������Կ���ʹ�������2λ
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,tmpBuf)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,&tmpBuf[1],2);
	hsmCmdLen += 2;
	// ��Կ���ȱ�־
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
	// LMK���ܵ���Կ����
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
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ��Կ���ʹ���
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܵ���Կ���ĳ��ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// У��ֵ���ͱ�־
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
�������ܣ�
	A4ָ��ü�����Կ�����ĳɷֺϳ�һ����Կ��������У��ֵ
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	partKeyNum����Կ�ɷֵ�����
	partKey����Ÿ�����Կ�ɷֵ����黺�壬ΪLMK���ܵ���Կ����
���������
	keyByLMK���ϳɵ���Կ���ģ���LMK����
	checkValue���ϳɵ���Կ��У��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���keyByLMK��checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������keyByLMK��checkValue���ܳ���
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
	// ������
	memcpy(hsmCmdBuf,"A4",2);
	hsmCmdLen = 2;
	// ��Կ�ɷ�����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;

	if (keyType >= 10000 && keyType < 20000)
        {
                keyTmp = keyType;
                keyType -= 10000;
        }

	// ��Կ���ʹ���
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
		// ��Կ���ȱ�־
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA4:: UnionTranslateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
		
		for (i = 0; i < partKeyNum; i++)
		{
			// LMK���ܵ���Կ����
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
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			// �ϳɵ���Կ��У��ֵ
			memcpy(checkValue,hsmCmdBuf+4+16,6);
			return(16+6);
		case	con128BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			// �ϳɵ���Կ��У��ֵ
			memcpy(checkValue,hsmCmdBuf+4+1+32,6);
			return(32+6);
		case	con192BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			// �ϳɵ���Կ��У��ֵ
			memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			return(48+6);
		default:
			UnionUserErrLog("in RacalCmdA4:: unknown key length!\n");
			return(errCodeParameter);
	}
}
// Mary add end, 2004-3-24

/*
����	��Դ�ʺ����ɵ�PINBLOCKת��Ϊ��Ŀ���ʺ����ɵ�PINBLOCK������ָ������Կ���ܱ���
˵��	������Կ����ȷ���㷨

ָ����Ϣ��ʽ
��Ϣͷ	mA	
������	2A	DF
ZPK	16H or
	1A+32H or
	1A+48H	�� LMK 06-07 ���ܵ�ZPK
PVK	16H or
	1A+32H or
	1A+48H	�� LMK 14-15���ܵ�ZPK
Check length	2N	���PIN����
ʮ��������	16N	16����ת����
PIN Validation data	12A	User-defined data consisting of hexadecimal characters and
the character N, which indicates to the HSM where to insert
the last 5 digits of the account number.
PIN block	16 H	��ZPK����ANSI X9.8��׼����
Դ�˺�	12 N	12λ��ЧԴ�˺�
Ŀ���˺�	12 N	12λ��ЧĿ���˺�
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
	// ������
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
�������ܣ�
	33ָ�˽Կ����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	flag���ù�Կ����ʱ�����õ���䷽ʽ��
		'0'��������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
		'1'��PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
	vkIndex��˽Կ������"00"��"20"
	cipherDataLen���������ݵ��ֽ���
	cipherData�����ڽ��ܵ���������
	sizeOfPlainData��plainData���ڵĴ洢�ռ��С
���������
	plainData�����ܵõ�����������
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���plainData
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������plainData�ĳ���
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
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	pvkIndex��	ָ����˽Կ�����ڽ���PIN��������
	fillMode��	��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
			��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
	ZPK��		���ڼ���PIN����Կ��
	accNo��		�û����ʺ�
	lenOfPinBlock:	��Կ���ܵ�PIN���ĵĳ���
	pinBlock��	����Կ���ܵ�PIN��������
�������	
	lenOfPin��	��Կ����
	pinBlock1��	����ZPK�����µ�PIN����
	pinBlock2��	����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
	lenOfUniqueID��	01-20
	UniqueID��	���صľ�ASCII��չ��ID������
	errCode��	�����������
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�

������		����	����	˵����
�������	2	A	ֵ��H3��
˽Կ����	2	N	ָ����˽Կ�����ڽ���PIN��������
�ù�Կ����ʱ�����õ���䷽ʽ	1	N	��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
ZPK	16/1A+32/1A+48	H	���ڼ���PIN����Կ
�ʺţ�PAN��	12	N	�û����ʺ�
PIN����	n	B	����Կ���ܵ�PIN��������
	
�����		����	����	˵����
��Ӧ����	2	A	ֵ��H4��
�������	2	H	������PIN���ĳ��ȴ�
������������ϸ������
���볤��	2	N	04~20
PIN����	32	H	����ZPK�����µ�PIN����
PIN����2	48	H	����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
ID�볤��	2	N	01-20
ID������	N*2	H	���صľ�ASCII��չ��ID������
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
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	pvkIndex��	ָ����˽Կ�����ڽ���PIN��������
	fillMode��	��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
			��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
	ZPK��		���ڼ���PIN����Կ��
	accNo��		�û����ʺ�
	lenOfPinBlock:	��Կ���ܵ�PIN���ĵĳ���
	pinBlock��	����Կ���ܵ�PIN��������
�������	
	lenOfPin��	��Կ����
	pinBlock1��	����ZPK�����µ�PIN����
	pinBlock2��	����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
	lenOfUniqueID��	01-20
	UniqueID��	���صľ�ASCII��չ��ID������
	errCode��	�����������
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�

������		����	����	˵����
�������	2	A	ֵ��H3��
˽Կ����	2	N	ָ����˽Կ�����ڽ���PIN��������
�ù�Կ����ʱ�����õ���䷽ʽ	1	N	��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
ZPK	16/1A+32/1A+48	H	���ڼ���PIN����Կ
�ʺţ�PAN��	12	N	�û����ʺ�
PIN����	n	B	����Կ���ܵ�PIN��������
	
�����		����	����	˵����
��Ӧ����	2	A	ֵ��H4��
�������	2	H	������PIN���ĳ��ȴ�
������������ϸ������
���볤��	2	N	04~20
PIN����	32	H	����ZPK�����µ�PIN����
PIN����2	48	H	����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
ID�볤��	2	N	01-20
ID������	N*2	H	���صľ�ASCII��չ��ID������
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
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	OriKeyType��	ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
	OriKey��	ԴKEY�����ڽ���PIN����Կ��
	lenOfAcc��	�ʺų��ȣ�4~20��
	accNo��		�û����ʺš�
	DesKeyType��	������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
	DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
	pinBlock��	����Կ���ܵ�PIN�������ġ�
�������
	lenOfPinBlock1�����볤�ȡ�
	pinBlock1��	����ZPK2/PVK�����µ�PIN���ġ�
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�

������		����	����	˵����
�������	2	A	ֵ��H����
ԴKEY����	��	N	����ZPK
����PVK��Ŀ��KEY���Ͳ���Ϊ0��
ԴZPK1/PVK1	16/1A+32/1A+48	H	���ڽ���PIN����Կ
�ʺų���	2	N	4~20
�ʺţ�PAN��	��������	N	�û����ʺ�
Ŀ��KEY����	��	N	����ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0��
����PVK
Ŀ��ZPK��/PVK	16/1A+32/1A+48	H	���ڼ���PIN����Կ
PIN����	32	H	��ZPK1���ܵ�PIN��������

�����		����	����	˵����
��Ӧ����	2	A	ֵ��H����
�������	2	H	������PIN���ĸ�ʽ��
������������ϸ������
���볤��	2	N	04~20
PIN����	32	H	����ZPK2/PVK�����µ�PIN����
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
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	OriKeyType��	ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
	OriKey��	ԴKEY�����ڽ���PIN����Կ��
	lenOfAcc��	�ʺų��ȣ�4~20��
	accNo��		�û����ʺš�
	DesKeyType��	������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
	DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
	pinBlock��	����Կ���ܵ�PIN�������ġ�
�������
	lenOfPinBlock1�����볤�ȡ�
	pinBlock1��	����ZPK2/PVK�����µ�PIN���ġ�
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�

������		����	����	˵����
�������	2	A	ֵ��H7��
ԴKEY����	��	N	����ZPK
����PVK��Ŀ��KEY���Ͳ���Ϊ0��
ԴZPK1/PVK1	16/1A+32/1A+48	H	���ڽ���PIN����Կ
�ʺų���	2	N	4~20
�ʺţ�PAN��	��������	N	�û����ʺ�
Ŀ��KEY����	��	N	����ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0��
����PVK
Ŀ��ZPK��/PVK	16/1A+32/1A+48	H	���ڼ���PIN����Կ
PIN����	32	H	��ZPK1���ܵ�PIN��������

�����		����	����	˵����
��Ӧ����	2	A	ֵ��H8��
�������	2	H	������PIN���ĸ�ʽ��
������������ϸ������
���볤��	2	N	04~20
PIN����	32	H	����ZPK2/PVK�����µ�PIN����
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
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	ZPK��		���ڽ���PIN����Կ��
	lenOfAcc��	�ʺų��ȣ�4~20��
	accNo��		�û����ʺš�
	pinBlockByZPK��	��ZPK���ܵ�PIN�������ġ�
	PVK��		PVK��
	pinBlockByPVK��	��PVK���ܵ�PIN�������ġ�
�������
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�
	
������		����	����	˵����
�������	2	A	ֵ��H9��
ZPK	16/1A+32/1A+48	H	���ڽ���PIN����Կ
�ʺų���	2	N	4~20
�ʺţ�PAN��	��������	N	�û����ʺ�
PIN����	32	H	��ZPK���ܵ�PIN��������
PVK	16/1A+32/1A+48	H	
PIN����	32	H	��PVK���ܵ�PIN��������

�����		����	����	˵����
��Ӧ����	2	A	ֵ��H:��
�������	2	H	00 �ɹ�
01 ʧ��
������������ϸ������

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
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	ZPK��		���ڼ���PIN����Կ��
	accNo��		�û����ʺš�
	pinBlockByZPK��	��ZPK���ܵ�PIN�������ġ�
	dataOfZAK��	����ZAK��Կ������֮һ��
�������	
	lenOfPin��	���볤��
	ZAK��		����LMK26-27�ԶԼ����µ�ZAK��Կ��
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�
	
������		����	����	˵����
�������	2	A	ֵ��H4��
ZPK	16/1A+32/1A+48	H	���ڼ���PIN����Կ
�ʺţ�PAN��	12	N	�û����ʺ�
PIN����	48	H	��ZPK���ܵ�PIN��������
ZAK����	32	H	����ZAK��Կ������֮һ

�����		����	����	˵����
��Ӧ����	2	A	ֵ��H5��
�������	2	H	������PIN���ĳ��ȴ�
������������ϸ������
���볤��	2	N	04~20
ZAK��Կ	1A+32	H	����LMK26-27�ԶԼ����µ�ZAK��Կ

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
����
	��PinOffsetת��Ϊר���㷨��FINSE�㷨�����ܵ�����
��  ��  ��  Ϣ  ��  ʽ

��  ��  ��	��  ��	��  ��	��      ��
��   ��	2	A	S1
PVK	16H��1A+32H��1A+48H	H	LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset��
PinOffset	12	N	Offset��ֵ�����������ķ�ʽ���ұ�����ַ���F����
��鳤��	2	N	��С��PIN���ȡ�
�˺�	12	N	�˺���ȥ��У��λ������12λ��
ʮ����ת����	16	N	��ʮ������ת��Ϊʮ���Ƶ�ת����
PINУ������	12	A	�û�����ġ�����ʮ�������ַ����ַ���N�������ݣ�����ָʾHSM�����˺����5λ��λ�á�


��  ��  ��  Ϣ  ��  ʽ
��  ��  ��	��  ��	��  ��	��      ��
Ӧ����	2	A	S2
�������	2	H	00�� ��ȷ      ������ʧ��
���	6	H	PIN����
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
���ܣ�
	��PinBlockת��Ϊר���㷨��FINSE�㷨�����ܵ�����
��  ��  ��  Ϣ  ��  ʽ

��  ��  ��	��  ��	��  ��	��      ��
��   ��	2	A	S2
ZPK	16H��1A+32H��1A+48H	H	LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset��
��鳤��	2	N	��С��PIN���ȡ�
�˺�	12	N	�˺���ȥ��У��λ������12λ��
ԴPIN��	16H	H	ԴPIN��


��  ��  ��  Ϣ  ��  ʽ
��  ��  ��	��  ��	��  ��	��      ��
Ӧ����	2	A	S 3
�������	2	H	00�� ��ȷ      ������ʧ��
���	6	H	PIN����
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
Mode of operation	1N	1: ECB �C Electronic Code Book
2: CBC �C Cipher Block Chaining
3: CFB �C Cipher Feed Back
4: OFB �C Output Feed Back
5: PCBC
Key flag	1N	0: ZEK �C Zone Encryption Key
ZEK	16H/1A+32H/1A+48H	ZEK encrypted under LMK pair 30-31.

Input data format	1N	0: Binary, 1: expanded Hex
Onput data format	1N	0: Binary, 1: expanded Hex
Pad  mode	1N	Present only when message block number is 0, or 3.
0: If data lengths are exact multiples of eight bytes, not padding; otherwise padding characters defined by the following field (Pad character)until data lengths are exact multiples of eight bytes.
1: If  data lengths are exact multiples of eight bytes ,Padding another eight bytes defined  by the following field( (Pad character)�� otherwise  padding characters defined by the following field(Pad character),until data lengths are exact multiples of eight bytes.
Pad character	4H
	Present only when message block number is 0, or 3�� .e.g.
          Pad mode|Pad character|Pad count flag
ANSI X9.19  :  0         0000           0
ANSI X9.23  :  1         0000           1
PBOC MAC  :  1         8000           0 
etc.
Pad count flag	1N	Present only when message block number is 0, or 3. 
0��Last byte is not padding count 
1��Last byte is padding count,and the count is within the range of X��01�� to X��08��
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
	
	if (inDataType[0] == '0')  //modify by hzh in 2011.5.4 ���Ӷ��������͵��ж�
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
	
	if(outDataType[0] == '0')   //modify by hzh in 2011.5.4,���Ӷ������ʽ���ж�
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
������ɢMAK����MAC

��  ��  ��  Ϣ  ��  ʽ

������	����	����	˵����
�������	2	A	ֵ��G1��
ZAK	16H/1A+32H/1A+48H	N	��LMK26-27�Լ���
��ɢ����	16	H	���ڼ�����ɢ��Կ������
MAC���ݳ���	3H	H	���ڼ���MAC������
MAC����	nB	B	MAC����
							��    ��    ��    Ϣ
�����	����	����	˵����
��Ӧ����	2	A	ֵ��G2��
�������	2	H	
MAC	16	H	���ص�MACֵ
�������̣�
1.	��ZAK��Կ����ɢ���ݲ���IC����׼��ɢ�㷨������ɢ�����ZAKΪ64bit������ֱ�Ӷ���ɢ���ݽ���DES���ܼ��ɡ�
2.	����ɢ��Կ��MAC���ݲ���Ansi9.19��ZAK˫�������ϣ���9.9��ZAK���������㷨����MAC��
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
ָ��ܣ�����PINBLOCK��ANSI9.8��ʽ�໥ת��
��  ��  ��  Ϣ  ��  ʽ
������	����	������		˵����
��Ϣͷ	mA	
�������	2A	Value �� Y1 ��
ԭZPK	16H or
1A+32H or 1A+48H
	��LMK�ԣ�06-07���¼���
Ŀ��ZPK	16H or
1A+32H or 1A+48H
	��LMK�ԣ�06-07���¼���
ԴPIN��	16H or 32H	ԴZPK�¼��ܵ�ԴPIN�顣
ԴPINBLOCK��ʽ	1N	1��ANSI9.8��ʽ
2������PINר�ø�ʽ��ר���㷨
Ŀ��PINBLOCK��ʽ	1N	1��ANSI9.8��ʽ
2������PINר�ø�ʽ��ר���㷨
�˺�	12N	�û����ʺţ����õ�����12λ����������λ;
��  ��  ��  Ϣ  ��  ʽ
��Ϣͷ	mA	
��Ӧ����	2A	Value �� Y2 ��
�������	2N	
PINBLOCK	16H��32H	��ת����ANSI9.8��ʽ��PINBLOCK��16H����ת��������PIN��ʽ����32H��
ע��1������PINBLOCKΪ�����ʺż��ܡ�
2������ANSI9.8��ʽת������ר�ø�ʽʱ����Ϊ�����ʺż��ܡ�
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
���������ָ��
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


/*һ���Է��ͼ��ܻ����ݿ���󳤶�*/
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

/* Y4ָ��,����:������Կ.ʹ�ø���Կ����ɢ����1������ɢ,�õ���ɢ��Կ;ʹ�ñ�����Կ(ZMK)���������LMK06-07�Լ������.
   �������: 
   	 kek��������ԿKEK 
   	 rootKey��Ӧ������Կ
   	 keyType����Կ���ͣ�001-ZPK��008-ZAK��
   	 discreteNum����ɢ����
   	 discreteData1����ɢ����1
   	 discreteData2����ɢ����2
   	 discreteData3����ɢ����3
   �������:
   	 keyByKek��ʹ��KEK���ܵ�key
   	 checkValue:У��ֵ
   	 errCode�������������
   ����ֵ:
   	 <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   	 >=0������ִ�гɹ�
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
	memcpy(hsmCmdBuf, "Y4", 2);      /*�������*/
	hsmCmdLen = 2;
	
	UnionNullLog("in RacalCmdY4 kek[%s]\n",kek);
	
	if ((ret = UnionPutKeyIntoRacalKeyString(kek, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*������Կ*/
	{
		UnionUserErrLog("in RacalCmdY4:: kek UnionPutKeyIntoRacalKeyString [%s]!\n", kek);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(rootKey, hsmCmdBuf + hsmCmdLen, sizeof(hsmCmdBuf) - 1 - hsmCmdLen)) < 0) /*����Կ*/
	{
		UnionUserErrLog("in RacalCmdY4:: rootKey UnionPutKeyIntoRacalKeyString [%s]!\n", rootKey);
		return(ret);
	}
	hsmCmdLen += ret;	
	memcpy(hsmCmdBuf + hsmCmdLen, keyType, 3);
	hsmCmdLen += 3;
	sprintf(hsmCmdBuf + hsmCmdLen, "%d", discreteNum); /*��ɢ����*/
	hsmCmdLen += 1;
	/*��ɢ����*/
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
	/*�����������������*/
	
/*******************************************************/
	UnionNullLog("in RacalCmdY4 hsmCmdBuf[%s]\n",hsmCmdBuf);
/*******************************************************/

	if((ret = UnionLongConnSJL06Cmd(hsmSckHDL, pSJL06, hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdY4:: UnionLongConnSJL06Cmd!\n");
		return(hsmCmdLen);
	}
	hsmCmdBuf[ret] = '\0';
	/*�ж���Ӧ��*/
	if ((memcmp(hsmCmdBuf, "Y5", 2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdY4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode, hsmCmdBuf + 2, 2);
	memcpy(keyByLmk, hsmCmdBuf + 4, 32);  /*����keyByLmk*/
	memcpy(keyByKek, hsmCmdBuf + 36, 32);  /*����keyByKek*/
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
�������ܣ�
	A5ָ�������Կ���ĵļ����ɷݺϳ����յ���Կ��ͬʱ����Ӧ��LMK��Կ�Լ���
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	partKeyNum����Կ�ɷֵ�����
	partKey����Ÿ�����Կ�ɷֵ����黺��
���������
	keyByLMK���ϳɵ���Կ���ģ���LMK����
	checkValue���ϳɵ���Կ��У��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���keyByLMK��checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������keyByLMK��checkValue���ܳ���
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
	// ������
	memcpy(hsmCmdBuf,"A5",2);
	hsmCmdLen = 2;
	// ��Կ�ɷ�����
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// ��Կ���ʹ���
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA5:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// ��Կ���ȱ�־
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA5:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	for (i = 0; i < partKeyNum; i++)
	{
		// ��Կ����
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
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			// �ϳɵ���Կ��У��ֵ
			memcpy(checkValue,hsmCmdBuf+4+16,16);
			return(16+16);
		case	con128BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			// �ϳɵ���Կ��У��ֵ
			memcpy(checkValue,hsmCmdBuf+4+1+32,16);
			return(32+16);
		case	con192BitsDesKey:
			// �ϳɵ���Կ��LMK����
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			// �ϳɵ���Կ��У��ֵ
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
