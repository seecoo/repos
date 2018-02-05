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
#include "unionTASSCmd.h"
#include "commWithHsmSvr.h"

// ʹ�������ZMK����
extern int gunionIsUseSpecZmkType;

//������ǩ��ָ��(����)
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
		UnionUserErrLog("in UnionTASSHsmCmdK5:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


/*
   ��������:
   TJָ���Կ����
   �������:
encryMode: 1H �㷨��ʶ 1���������� 2���������� 3����֤ǩ�� 4��������Կ 5��������Կ
pkLength: ���빫Կ����
pk: nB/1A+3H DER����Ĺ�Կ��Կ��HSM��ȫ�洢���ڵ�λ��
signLength: ����֤ǩ��ֵ���ȣ�encryMode = 3ʱ�������
sign: nB ����֤ǩ��ֵ��encryMode = 3ʱ�������
keyLength: 1H ����/������Կ���ȣ�0 = ��������Կ��1 = ˫������Կ��encryMode = 4 �� encryMode = 5ʱ�������
inputDataLength: �ӽ������ݳ���
inputData: nB/1A+3H ���ӽ������ݻ���ӽ�����Կ��HSM��ȫ�洢���ڵ�����
�������:
outputData: nB �ӽ���/�ӽ�����Կ����
����ֵ:
>0	ouputData�ĳ���
<0	ʧ��
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

	// �������ͨѶ
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

