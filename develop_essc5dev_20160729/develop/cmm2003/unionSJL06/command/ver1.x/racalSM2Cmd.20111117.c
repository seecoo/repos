// Author:	Wolfgang Wang
// Date:	2006/3/2

#define _RacalCmdForNewRacal_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sjl06.h"
#include "sjl06Protocol.h"
#include "racalSM2Cmd.h"
#include "UnionStr.h"
#include "unionDesKey.h"
#include "unionSJL06API.h"
#include "unionErrCode.h"
#include "UnionLog.h"

// 生成一对SM2密钥
/* 输入参数
	type，类型	0，只用于签名
			1，只用于密钥管理
			2，签名和管理
	length，模数	0320/512/1024/2048
	pkEncoding	公钥的编码方式
	lenOfPKExponent	公钥exponent的长度	可选参数
	pkExponent	公钥的pkExponent	可选参数
	exportNullPK	1，输出裸PK，0，编码方式的PK
   	sizeOfPK	接收公钥的缓冲大小
   	sizeOfVK	接收私钥的缓冲大小
   输出参数
   	pk		公钥
   	lenOfVK		私钥串的长度
   	vk		私钥
   	errCode		密码机出错码
*/
int RacalCmdK1(int hsmSckHDL,PUnionSJL06 pSJL06,char type,int length,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,char *errCode)
{
	int			ret;
	char			hsmCmdBuf[8096+1];
	int			hsmCmdLen = 0;
	char			bcdPK[8096+1];
	
	memset(bcdPK,0,sizeof(bcdPK));
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"K1",2);
	hsmCmdLen = 2;
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",length);
	hsmCmdLen += 4;
	type = '3';
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	sprintf(hsmCmdBuf+hsmCmdLen,"%2d",99);
	hsmCmdLen += 2;	

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdK1:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"K2",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdK1:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdK1:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	*lenOfVK = UnionConvertIntoLen(hsmCmdBuf+4,4);
	if ((*lenOfVK <= 0) || (*lenOfVK + 4 + 4 > ret) || (*lenOfVK > sizeOfVK))
	{
		UnionUserErrLog("in RacalCmdK1:: lenOfVK [%d] error!\n",*lenOfVK);
		UnionMemLog("in RacalCmdK1:: hsmReturnStr::",(unsigned char *)hsmCmdBuf,ret);
		return(errCodeParameter);
	}
	memcpy(vk,hsmCmdBuf+4+4,*lenOfVK);
	memcpy(bcdPK,hsmCmdBuf+4+4+*lenOfVK,64);
	bcdhex_to_aschex(bcdPK,64,pk);				
	return(128);
}

// 使用SM3算法进行HASH计算
/* 输入参数
	algorithmID	算法标识，3-SM3
	lenOfData	数据长度
	hashData	做Hash的数据
   	sizeOfBuf	接收Hash结果的缓冲大小
   输出参数
   	hashValue	Hash结果
   	errCode		密码机出错码
*/
int RacalCmdM7(int hsmSckHDL,PUnionSJL06 pSJL06,char *algorithmID,int lenOfData,char *hashData,char *hashValue,int sizeOfBuf,char *errCode)
{
	int			ret;
	char			hsmCmdBuf[8096+1];
	int			hsmCmdLen = 0;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	
	// 指令代码
	memcpy(hsmCmdBuf,"M7",2);
	hsmCmdLen = 2;
	
	//模式标志
	memcpy(hsmCmdBuf + hsmCmdLen,"1",1);
	hsmCmdLen += 1;
	
	// 算法标识
	memcpy(hsmCmdBuf + hsmCmdLen,algorithmID,1);
	hsmCmdLen += 1;
	
	// 数据长度
	sprintf(hsmCmdBuf + hsmCmdLen,"%04d",lenOfData/2);
	hsmCmdLen += 4;	
	
	// 数据
	aschex_to_bcdhex(hashData,lenOfData,hsmCmdBuf + hsmCmdLen);
	hsmCmdLen += lenOfData/2;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdM7:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"M8",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdM7:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdM7:: errCode = [%s]!\n",errCode);
		return(0);
	}
	
	if (sizeOfBuf < 64+1)
	{
		UnionUserErrLog("in RacalCmdM7:: sizeOfBuf [%d] < 64 + 1!\n",sizeOfBuf);
		return(errCodeParameter);
	}

	bcdhex_to_aschex(hsmCmdBuf+4,32,hashValue);
	return(64);
}

// 生成签名的指令
int RacalCmdK3(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192*2+1];
	int		offset = 0;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	
	memcpy(hsmCmdBuf + offset,"K3",2);
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
	
	// 数据长度
	sprintf(hsmCmdBuf+offset,"%04d",lenOfData/2);
	offset += 4;

	// 数据
	aschex_to_bcdhex(data,lenOfData,hsmCmdBuf + offset);
	offset += lenOfData/2;

	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdK3:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdK3:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"K4",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdK3:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdK3:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	// 签名结果的R部分
	bcdhex_to_aschex(hsmCmdBuf + 4,32,sign);
	// 签名结果的S部分
	bcdhex_to_aschex(hsmCmdBuf + 4 + 32,32,sign+64);
	//return(lenOfSign);
	return(128);
}

//生成验签的指令
int RacalCmdK4(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,char *hashID,int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[8192*2+1];
	int		offset = 0;

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	
	memcpy(hsmCmdBuf + offset,"K4",2);
	offset += 2;
	if (vkIndex < 0)
	{
		// 密钥索引
		memcpy(hsmCmdBuf + offset,"99",2);
		offset += 2;
		// 外带公钥
		aschex_to_bcdhex(rCaPK,lengRCaPK,hsmCmdBuf + offset);
		offset += lengRCaPK/2;
		//memcpy(hsmCmdBuf + offset,rCaPK,lengRCaPK);
		//offset += lengRCaPK;
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
	//memcpy(hsmCmdBuf + offset,caCertDataSign,lenCaCertDataSign);
	//offset += lenCaCertDataSign;
		
	if (hashID == NULL)
		memcpy(hsmCmdBuf+offset,"01",2);
	else
		memcpy(hsmCmdBuf+offset,hashID,2);
	offset += 2;
	
	// 数据长度
	//sprintf(hsmCmdBuf+offset,"%04d",lenHashVal/2);
	sprintf(hsmCmdBuf+offset,"0032");
	//UnionLog("in RacalCmdK4:: UnionLongConnSJL06Cmd lenHashVal = [%d]!\n",lenHashVal/2);
	offset += 4;

	// 数据
	aschex_to_bcdhex(hashVal,lenHashVal,hsmCmdBuf + offset);
	offset += lenHashVal/2;
	//memcpy(hsmCmdBuf + offset,hashVal,lenHashVal);
	//offset += lenHashVal;
	UnionProgramerMemLog("in RacalCmdK4:: req =",(unsigned char *)hsmCmdBuf,offset);
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,offset,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdK4:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	UnionProgramerMemLog("in RacalCmdK4:: res =",(unsigned char *)hsmCmdBuf,ret);
	if ((memcmp(hsmCmdBuf,"K5",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdK4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in RacalCmdK4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	return(0);
}
