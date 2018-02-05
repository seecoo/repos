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
#include "unionTASSCmd.h"
#include "commWithHsmSvr.h"

// 使用特殊的ZMK类型
extern int gunionIsUseSpecZmkType;

//生成验签的指令(国密)
int UnionTASSHsmCmdK5(int vkIndex, char *hashID, int lenOfUsrID, char *usrID, int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal)
{
	int	ret;
	char	hsmCmdBuf[8192*2];
	int	offset = 0;

	UnionSetBCDPrintTypeForHSMReqCmd();

	memcpy(hsmCmdBuf + offset,"K5",2);
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
		UnionUserErrLog("in UnionTASSHsmCmdK5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


/*
   函数功能:
   TJ指令，公钥解密
   输入参数:
encryMode: 1H 算法标识 1－解密数据 2－加密数据 3－验证签名 4－加密密钥 5－解密密钥
pkLength: 送入公钥长度
pk: nB/1A+3H DER编码的公钥或公钥在HSM安全存储区内的位置
signLength: 待验证签名值长度，encryMode = 3时该域存在
sign: nB 待验证签名值，encryMode = 3时该域存在
keyLength: 1H 待加/解密密钥长度，0 = 单长度密钥，1 = 双长度密钥；encryMode = 4 或 encryMode = 5时该域存在
inputDataLength: 加解密数据长度
inputData: nB/1A+3H 待加解密数据或待加解密密钥在HSM安全存储区内的索引
输出参数:
outputData: nB 加解密/加解密密钥数据
返回值:
>0	ouputData的长度
<0	失败
 */

int UnionTASSHsmCmdTJ(char encryMode, int pkLength, char *pk, int signLength,
		char *sign, char keyLength, int inputDataLength, char *inputData, char *outputData)
{
	int		ret;
	char		tmpBuf[32];
	char		hsmCmdBuf[8192];
	int		hsmCmdLen = 0;

	if ((pk == NULL) || (inputDataLength <= 0))
	{
		UnionUserErrLog("in UnionTASSHsmCmdTJ:: parameters error!\n");
		return(errCodeParameter);
	}

	UnionSetBCDPrintTypeForHSMCmd();

	memcpy(hsmCmdBuf, "TJ", 2);
	hsmCmdLen += 2;

	hsmCmdBuf[hsmCmdLen] = encryMode;
	hsmCmdLen++;

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", pkLength);
	hsmCmdLen += 4;

	memcpy(hsmCmdBuf + hsmCmdLen, pk, pkLength);
	hsmCmdLen += pkLength;

	memcpy(hsmCmdBuf + hsmCmdLen, "01", 2);
	hsmCmdLen += 2;

	if (encryMode == '3')
	{
		if (sign == NULL)
		{
			UnionUserErrLog("in UnionTASSHsmCmdTJ:: parameters error!\n");
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf + hsmCmdLen, "%04d", signLength);
		hsmCmdLen += 4;
		memcpy(hsmCmdBuf + hsmCmdLen, sign, signLength);
		hsmCmdLen += signLength;
	}

	if ((encryMode == '4') || (encryMode == '5'))
	{
		memcpy(hsmCmdBuf + hsmCmdLen, "000", 3);
		hsmCmdLen += 3;
		hsmCmdBuf[hsmCmdLen] = keyLength;
		hsmCmdLen++;
	}

	sprintf(hsmCmdBuf + hsmCmdLen, "%04d", inputDataLength);
	hsmCmdLen += 4;
	memcpy(hsmCmdBuf + hsmCmdLen, inputData, inputDataLength);
	hsmCmdLen += inputDataLength;
	hsmCmdBuf[hsmCmdLen] = 0;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionTASSHsmCmdTJ:: UnionDirectHsmCmd!\n");
		return(ret);
	}

	hsmCmdBuf[ret] = 0;

	memcpy(tmpBuf, hsmCmdBuf + 4, 4);
	tmpBuf[4] = 0;
	ret = atoi(tmpBuf);
	memcpy(outputData, hsmCmdBuf + 4 + 4, ret);
	outputData[ret] = 0;

	return(ret);
}

