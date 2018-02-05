//	Author:		张永定
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
功能
	判断指定IP的加密机是否正常
输入参数
	无
输入出数
	hsmGrpID	组号
返回值
	1		正常
	<=0		异常
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
功能
	获取一个密码机组中的可用密码机
输入参数
	hsmGrpID	密码机组号
输入出数
	hsmGrp		密码机结构体
返回值
	>=0 成功
	<0 失败
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
功能
	获取一个密码机组中的所有可用密码机
输入参数
	hsmGrpID	密码机组号
	maxNum		所能返回的最大密码机台数
输入出数
	hsmRecGrp	密码机所组成的数组
返回值
	>=0		实际读取的密码机台数
	<0		错误码
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
功能
	获取密码机组内的打印密码机
输入参数
	hsmGrpID	密码机组ID
输入出数
	hsmRec		密码机结构体
返回值
	>=0 成功
	<0 失败
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

// 使用指定加密机组内的密码机，打印一把密钥,
/*
功能
	使用指定加密机组内的密码机，打印一把密钥
输入参数
	hsmGrpID		密码机组ID
	keyType			密钥类型
	keyLen			密钥长度
	printFormat		密钥的打印格式
	checkValueFormat	密钥校验值的打印格式
	numOfComponent		分量的数量
	keyName			密钥名称
输入出数
	value			密钥值
	checkValue		密钥校验值
返回值
	>=0 成功
	<0 失败
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

	// 获取密码机组内的打印密码机
	memset(&hsmRec,0,sizeof(hsmRec));
	if ((ret = UnionFindDefaultPrinterHsmOfHsmGroup(hsmGrpID,&hsmRec)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionFindDefaultPrinterHsmOfHsmGroup hsmGrpID = [%s]!\n",hsmGrpID);
		return(ret);
	}

	// 使用指定的加密机
	UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

	// 设置密钥明文成分的打印格式
	memset(errCode, 0, sizeof(errCode));
	if ((ret = UnionHsmCmdPA(printFormat)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdPA!\n");
		return(ret);
	}

	// 给打印密钥信封格式中的每个域进行赋值
	strcpy(valuePrintParam[0], keyName);
	sprintf(valuePrintParam[2], "%02d", numOfComponent);
	memset(thisDate, 0, sizeof(thisDate));
	UnionGetFullSystemDate(thisDate);
	strcpy(valuePrintParam[3], thisDate);

	// 打印每个分量的明文成分
	for (i = 0; i < numOfComponent; i++)
	{
		// 生成并打印每个分量
		sprintf(valuePrintParam[1], "%02d", i+1);
		memset(component[i], 0, sizeof(component[i]));
		memset(errCode, 0, sizeof(errCode));
		if (numOfComponent == 1)
		{
			// 使用指定的加密机
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);
			UnionSetIsUseNormalZmkType();
			
			if ((ret = UnionHsmCmdA2(keyType, keyLen,  4, valuePrintParam, component[i])) < 0)
			{
				UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA2SpecForZmk!\n");
				return(ret);
			}

			// 使用指定的加密机
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

			// 为每个密钥成分生成校验值
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
			// 使用指定的加密机
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

			if ((ret = UnionHsmCmdA2(keyType, keyLen, 4, valuePrintParam, component[i])) < 0)
			{
				UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA2!\n");
				return ret;
			}

			// 使用指定的加密机
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

			// 为每个密钥成分生成校验值
			memset(componentCheckValue[i], 0, sizeof(componentCheckValue[i]));
			memset(errCode, 0, sizeof(errCode));
			if ((ret = UnionHsmCmdBU(0, keyType, keyLen, component[i], componentCheckValue[i])) < 0)
			{
				UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdBU!\n");
				continue;
			}
		
		}
	}

	// 将各个密钥成分合成一把密钥，并生成合成密钥的校验值
	if (numOfComponent == 1)
	{
		strcpy(checkValue, componentCheckValue[0]);
		strcpy(value, component[0]);
	}
	else
	{
		// 使用指定的加密机
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

		memset(errCode, 0, sizeof(errCode));
		if ((ret=UnionHsmCmdA4(0, keyType, keyLen, numOfComponent, component, value, checkValue)) < 0)
		{
			UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA4!\n");
			return ret;
		}
		
	}

	// 使用指定的加密机
	UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

	// 设置密钥校验值的打印格式
	memset(errCode, 0, sizeof(errCode));
	if ((ret = UnionHsmCmdPA(checkValueFormat)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdPA!\n");
		return ret;
	}

	// 给打印密钥信封格式中的每个域进行赋值
	sprintf(checkValuePrintParam[0], "密钥名称：%s", keyName);
	sprintf(checkValuePrintParam[1], "成分数量：%02d", numOfComponent);
	sprintf(checkValuePrintParam[2], "生成日期：%s", thisDate);
	for (i = 0; i < numOfComponent; i++)
	{
		sprintf(checkValuePrintParam[i + 3], "成分%02d的校验值：%s", i+1, componentCheckValue[i]);
	}
	sprintf(checkValuePrintParam[3+numOfComponent], "密钥的校验值：%s", checkValue);

	// 使用指定的加密机
	UnionSetUseSpecHsmIPAddrForOneCmd(hsmRec.ipAddr);

	if ((ret = UnionHsmCmdA2(keyType, keyLen, 3+numOfComponent+1, checkValuePrintParam, component[i])) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyUsingSpecHsmGroup:: UnionHsmCmdA2!\n");
		return ret;
	}
	
	return 0;
}

