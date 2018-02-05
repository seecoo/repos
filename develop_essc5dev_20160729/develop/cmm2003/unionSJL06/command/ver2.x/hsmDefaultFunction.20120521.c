//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionHsm.h"
#include "hsmDefaultFunction.h"
#include "unionHsmGroup.h"
#include "unionHsmCmd.h"
#include "commWithHsmSvr.h"

/*
����
	�ж�ָ��IP�ļ��ܻ��Ƿ�����
�������
	��
�������
	hsmGrpID	���
����ֵ
	1		����
	<=0		�쳣
*/
int UnionIsHsmNormal(char *ipAddr,char *hsmGrpID)
{
	int		ret;
	TUnionHsm	hsm;
	memset(&hsm, 0, sizeof hsm);

	ret = UnionReadHsmRec(ipAddr,&hsm);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionIsHsmNormal:: UnionReadHsmRec err!! ipAddr=[%s]\n",ipAddr);
		return(ret);
	}

	strcpy(hsmGrpID, hsm.hsmGrpID);
	if (hsm.hsmStatusID == conHsmStatusOnlineSJL06)
		return 1;
	else
		return 0;
}

/*
����
	��ȡһ����������еĿ��������
�������
	hsmGrpID	��������
�������
	hsmGrp		������ṹ��
����ֵ
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionFindHsmOfSpecHsmGrp(char *hsmGrpID,TUnionHsm hsmRec)
{
	int		ret;
	TUnionHsm	hsmRecGrp[1];

	if ((ret = UnionFindAllWorkingHsmOfSpecHsmGrp(hsmGrpID,hsmRecGrp,1)) < 0)
	{
		UnionUserErrLog("in UnionFindHsmOfSpecHsmGrp:: UnionFindAllWorkingHsmOfSpecHsmGrp hsmGrpID=[%s]\n",hsmGrpID);
		return(ret);
	}
	memcpy(&hsmRec,&hsmRecGrp[0],sizeof(TUnionHsm));
	return(0);
}

/*
����
	��ȡһ����������е����п��������
�������
	hsmGrpID	��������
	maxNum		���ܷ��ص���������̨��
�������
	hsmRecGrp	���������ɵ�����
����ֵ
	>=0		ʵ�ʶ�ȡ�������̨��
	<0		������
*/
int UnionFindAllWorkingHsmOfSpecHsmGrp(char *hsmGrpID, TUnionHsm hsmRecGrp[], int maxNum)
{
	int		num;
	char		condition[256];

	memset(condition,0,sizeof(condition));
	sprintf(condition,"%s=%s|%s=%d|%s=%d|",conHsmGroupFldNameHsmGrpID,hsmGrpID,conHsmGroupFldNameActive,conHsmStatusOnlineSJL06, conHsmFldNameHsmStatusID, conHsmStatusOnlineSJL06);
	if ( (num = UnionBatchReadHsmRec(condition, hsmRecGrp, maxNum)) < 0)
	{
		UnionUserErrLog("in UnionFindAllWorkingHsmOfSpecHsmGrp:: UnionBatchReadHsmRec hsmGrpID=[%s]\n",hsmGrpID);
		return(num);
	}
	return(num);
}

/*
����
	��ȡ��������ڵĴ�ӡ�����
�������
	hsmGrpID	�������ID
�������
	hsmRec		������ṹ��
����ֵ
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionFindDefaultPrinterHsmOfHsmGroup(char *hsmGrpID,PUnionHsm hsmRec)
{
	int		num;
	char		condition[256];
	TUnionHsm	hsmRecGrp[1];

	memset(condition,0,sizeof(condition));
	sprintf(condition,"%s=%s|%s=%d|%s=%d|%s=%d|",conHsmGroupFldNameHsmGrpID,hsmGrpID,conHsmGroupFldNameActive,conHsmStatusOnlineSJL06, conHsmFldNameHsmStatusID, conHsmStatusOnlineSJL06,conHsmFldNameHsmFunction,conHsmFunctionPrintKey);
	
	memset(&hsmRecGrp,0,sizeof(hsmRecGrp));
	if ( (num = UnionBatchReadHsmRec(condition, hsmRecGrp, 1)) <= 0)
	{
		UnionUserErrLog("in UnionFindDefaultPrinterHsmOfHsmGroup:: UnionBatchReadHsmRec condition=[%s]\n",condition);
		return(errCodeHsmCmdMDL_HsmNotAvailable);
	}
	memcpy(hsmRec,&hsmRecGrp[0],sizeof(TUnionHsm));
	return(0);
}

// ʹ��ָ�����ܻ����ڵ����������ӡһ����Կ,
/*
����
	ʹ��ָ�����ܻ����ڵ����������ӡһ����Կ
�������
	hsmGrpID		�������ID
	keyType			��Կ����
	keyLen			��Կ����
	printFormat		��Կ�Ĵ�ӡ��ʽ
	checkValueFormat	��ԿУ��ֵ�Ĵ�ӡ��ʽ
	numOfComponent		����������
	keyName			��Կ����
�������
	value			��Կֵ
	checkValue		��ԿУ��ֵ
����ֵ
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionPrintDesKeyUsingSpecHsmGroup(char *hsmGrpID, int keyType, int keyLen, char *printFormat, char *checkValueFormat, int numOfComponent, char *keyName, char *value, char *checkValue)
{
	int	ret;
	int	i;
	char	errCode[2+1];
	char	thisDate[20];
	char	component[20][50];
	char	componentCheckValue[20][16+1];
	char	valuePrintParam[20][80];
	char	checkValuePrintParam[20][80];
	TUnionHsm	hsmRec;

	// ��ȡ��������ڵĴ�ӡ�����
	memset(&hsmRec,0,sizeof(hsmRec));
	if ((ret = UnionFindDefaultPrinterHsmOfHsmGroup(hsmGrpID,&hsmRec)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionFindDefaultPrinterHsmOfHsmGroup hsmGrpID = [%s]!\n",hsmGrpID);
		return(ret);
	}

	// ʹ��ָ���ļ��ܻ�
	UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

	// ������Կ���ĳɷֵĴ�ӡ��ʽ
	memset(errCode, 0, sizeof(errCode));
	if ((ret = UnionHsmCmdPA(printFormat)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdPA!\n");
		return(ret);
	}

	// ����ӡ��Կ�ŷ��ʽ�е�ÿ������и�ֵ
	strcpy(valuePrintParam[0], keyName);
	sprintf(valuePrintParam[2], "%02d", numOfComponent);
	memset(thisDate, 0, sizeof(thisDate));
	UnionGetFullSystemDate(thisDate);
	strcpy(valuePrintParam[3], thisDate);

	// ��ӡÿ�����������ĳɷ�
	for (i = 0; i < numOfComponent; i++)
	{
		// ���ɲ���ӡÿ������
		sprintf(valuePrintParam[1], "%02d", i+1);
		memset(component[i], 0, sizeof(component[i]));
		memset(errCode, 0, sizeof(errCode));
		if (numOfComponent == 1)
		{
			// ʹ��ָ���ļ��ܻ�
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);
			UnionSetIsUseNormalZmkType();
			
			if ((ret = UnionHsmCmdA2(keyType, keyLen,  4, valuePrintParam, component[i])) < 0)
			{
				UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA2SpecForZmk!\n");
				return(ret);
			}

			// ʹ��ָ���ļ��ܻ�
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

			// Ϊÿ����Կ�ɷ�����У��ֵ
			memset(componentCheckValue[i], 0, sizeof(componentCheckValue[i]));
			memset(errCode, 0, sizeof(errCode));
			if ((ret = UnionHsmCmdBU(0, keyType, keyLen, component[i], componentCheckValue[i])) < 0)
			{
				UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdBUSpecForZmk!\n");
				continue;
			}
		
		}
		else
		{
			// ʹ��ָ���ļ��ܻ�
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

			if ((ret = UnionHsmCmdA2(keyType, keyLen, 4, valuePrintParam, component[i])) < 0)
			{
				UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA2!\n");
				return ret;
			}

			// ʹ��ָ���ļ��ܻ�
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

			// Ϊÿ����Կ�ɷ�����У��ֵ
			memset(componentCheckValue[i], 0, sizeof(componentCheckValue[i]));
			memset(errCode, 0, sizeof(errCode));
			if ((ret = UnionHsmCmdBU(0, keyType, keyLen, component[i], componentCheckValue[i])) < 0)
			{
				UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdBU!\n");
				continue;
			}
		
		}
	}

	// ��������Կ�ɷֺϳ�һ����Կ�������ɺϳ���Կ��У��ֵ
	if (numOfComponent == 1)
	{
		strcpy(checkValue, componentCheckValue[0]);
		strcpy(value, component[0]);
	}
	else
	{
		// ʹ��ָ���ļ��ܻ�
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

		memset(errCode, 0, sizeof(errCode));
		if ((ret=UnionHsmCmdA4(0, keyType, keyLen, numOfComponent, component, value, checkValue)) < 0)
		{
			UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA4!\n");
			return ret;
		}
		
	}

	// ʹ��ָ���ļ��ܻ�
	UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

	// ������ԿУ��ֵ�Ĵ�ӡ��ʽ
	memset(errCode, 0, sizeof(errCode));
	if ((ret = UnionHsmCmdPA(checkValueFormat)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdPA!\n");
		return ret;
	}

	// ����ӡ��Կ�ŷ��ʽ�е�ÿ������и�ֵ
	sprintf(checkValuePrintParam[0], "��Կ���ƣ�%s", keyName);
	sprintf(checkValuePrintParam[1], "�ɷ�������%02d", numOfComponent);
	sprintf(checkValuePrintParam[2], "�������ڣ�%s", thisDate);
	for (i = 0; i < numOfComponent; i++)
	{
		sprintf(checkValuePrintParam[i + 3], "�ɷ�%02d��У��ֵ��%s", i+1, componentCheckValue[i]);
	}
	sprintf(checkValuePrintParam[3+numOfComponent], "��Կ��У��ֵ��%s", checkValue);

	// ʹ��ָ���ļ��ܻ�
	UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

	if ((ret = UnionHsmCmdA2(keyType, keyLen, 3+numOfComponent+1, checkValuePrintParam, component[i])) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA2!\n");
		return ret;
	}
	
	return 0;
}

