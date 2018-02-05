//	Wolfgang Wang
//	2005/7/18

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sjl06.h"
#include "sjl06Cmd.h"
#include "unionErrCode.h"
#include "UnionLog.h"


/* 打印域	1：密钥名称
		2：分量序号
		3：分量数量
		4：生成日期
*/
extern char	pgPrintFormat[];
extern char	pgPrintFormatForCheckValue[];
/*
char	pgPrintFormat[] 
	= ">L>001密钥名称：>011^0>L>001分量序号：>011^1>L>001分量数量：>011^2>L>001生成日期：>011^3>L>001密钥明文：>011^P>L>L";
char	pgPrintFormatForCheckValue[] 
	= ">L>003^0>L>003^1>L>003^2>L>003^3>L>003^4>L>003^5>L>003^6>L>003^7>L>003^8>L>003^9>L>L"; //003^10>L>003^11>L>003^12>L>003^13>L>L";
*/

int UnionGenerateKeyComponent(PUnionSJL06 psjl06,PUnionDesKey pdesKey,int componentNum,int numOfPar,char par[][80])
{
	va_list 	args;
	char		errCode[2+1];
	char		component[20][50],checkValue[20][16+1];
	int		ret;
	int		i;
	char		valuePrintParam[20][80];
	int		valuePrintNum;
	char		checkValuePrintParam[20][80];
	char		thisDate[20];
		
	if ((psjl06 == NULL) || (pdesKey == NULL) || (componentNum <= 0) || (componentNum >= 20) || (par == NULL))
	{
		printf("null pointer! or component [%d] error\n",componentNum);
		return(errCodeParameter);
	}
	
	// 设置密钥明文成分的打印格式
	memset(errCode,0,sizeof(errCode));
	if (((ret = RacalCmdPA(-1,psjl06,pgPrintFormat,errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
	{
		UnionUserErrLog("in UnionGenerateKeyComponent:: [%s] set print format [%s]\n",pdesKey->fullName,errCode);
		if (ret < 0)
			return(ret);
		else
			return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
	}

	// 给打印密钥信封格式中的每个域进行赋值
	strcpy(valuePrintParam[0],pdesKey->fullName);
	sprintf(valuePrintParam[2],"%02d",componentNum);
	memset(thisDate,0,sizeof(thisDate));
	UnionGetFullSystemDate(thisDate);
	strcpy(valuePrintParam[3],thisDate);

	for (i = 0,valuePrintNum = 4; i < numOfPar; i++,valuePrintNum++)
	{
		memset(valuePrintParam[valuePrintNum],0,sizeof(valuePrintParam[valuePrintNum]));
		strcpy(valuePrintParam[valuePrintNum],par[i]);
	}		
	
	UnionLog("in UnionGenerateKeyComponent:: valuePrintNum = [%d]\n",valuePrintNum);
	for (i = 0; i < valuePrintNum; i++)
	{
		UnionNullLog("[%02d] [%s]\n",i,valuePrintParam[i]);
	}
	// 打印每个分量的明文成分
	for (i = 0; (i < componentNum) && (i < 20); i++)
	{
		// 生成并打印每个分量
		sprintf(valuePrintParam[1],"%02d",i+1);
		memset(component[i],0,sizeof(component[i]));
		memset(errCode,0,sizeof(errCode));
		if (componentNum == 1)
		{
			if (((ret = RacalCmdA2SpecForZmk(-1,psjl06,pdesKey->type,pdesKey->length,valuePrintNum,valuePrintParam,component[i],errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
			{
				UnionUserErrLog("in UnionGenerateKeyComponent:: [%s] the [%02dth] component of [%d],errCode = [%s]\n",
						pdesKey->fullName,i+1,componentNum,errCode);
				if (ret < 0)
					return(ret);
				else
					return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
			}
			// 为每个密钥成分生成校验值
			memset(checkValue[i],0,sizeof(checkValue[i]));
			memset(errCode,0,sizeof(errCode));
			if ((ret=RacalCmdBUSpecForZmk(-1,psjl06,pdesKey->type,pdesKey->length,component[i],checkValue[i],errCode) < 0) || (strncmp(errCode,"00",2) != 0))
			{
				UnionUserErrLog("in UnionGenerateKeyComponent:: [%s] the [%02dth] component of [%d],errCode = [%s]\n",
						pdesKey->fullName,i+1,componentNum,errCode);
				// 2008/3/12，修改
				continue;	// 2008/3/12增加
				/* 2008/3/12删除
				if (ret < 0)
					return(ret);
				else
					return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
				*/
			}
		}
		else
		{
			if (((ret = RacalCmdA2(-1,psjl06,pdesKey->type,pdesKey->length,valuePrintNum,valuePrintParam,component[i],errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
			{
				UnionUserErrLog("in UnionGenerateKeyComponent:: [%s] the [%02dth] component of [%d],errCode = [%s]\n",
						pdesKey->fullName,i+1,componentNum,errCode);
				if (ret < 0)
					return(ret);
				else
					return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
			}
			// 为每个密钥成分生成校验值
			memset(checkValue[i],0,sizeof(checkValue[i]));
			memset(errCode,0,sizeof(errCode));
			if ((ret=RacalCmdBU(-1,psjl06,pdesKey->type,pdesKey->length,component[i],checkValue[i],errCode) < 0) || (strncmp(errCode,"00",2) != 0))
			{
				UnionUserErrLog("in UnionGenerateKeyComponent:: [%s] the [%02dth] component of [%d],errCode = [%s]\n",
						pdesKey->fullName,i+1,componentNum,errCode);
				// 2008/3/12，修改
				continue;	// 2008/3/12增加
				/* 2008/3/12删除
				if (ret < 0)
					return(ret);
				else
					return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
				*/
			}
		}
		UnionSuccessLog("in UnionGenerateKeyComponent:: [%s] the [%02dth] component  of [%d]\n",pdesKey->fullName,i+1,componentNum);
	}
	
	// 将各个密钥成分合成一把密钥，并生成合成密钥的校验值
	if (componentNum == 1)
	{
		strcpy(pdesKey->checkValue,checkValue[0]);
		strcpy(pdesKey->value,component[0]);
		//return(0);
	}
	else
	{
		memset(errCode,0,sizeof(errCode));
		memset(pdesKey->value,0,sizeof(pdesKey->value));
		memset(pdesKey->checkValue,0,sizeof(pdesKey->checkValue));
		if (( (ret=RacalCmdA4(-1,psjl06,pdesKey->type,pdesKey->length,componentNum,component,
			pdesKey->value,pdesKey->checkValue,errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
		{
			UnionUserErrLog("in UnionGenerateKeyComponent:: [%s]\n",pdesKey->fullName);
			if (ret < 0)
				return(ret);
			else
				return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
		}
	}
	
	// 设置密钥校验值的打印格式
	memset(errCode,0,sizeof(errCode));
	//UnionNullLog("pgPrintFormatForCheckValue = [%s]\n",pgPrintFormatForCheckValue);
	if (((ret = RacalCmdPA(-1,psjl06,pgPrintFormatForCheckValue,errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
	{
		UnionUserErrLog("in UnionGenerateKeyComponent:: [%s] set print format [%s]\n",pdesKey->fullName,errCode);
		if (ret < 0)
			return(ret);
		else
			return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
	}

	// 给打印密钥信封格式中的每个域进行赋值
	sprintf(checkValuePrintParam[0],"密钥名称：%s",pdesKey->fullName);
	sprintf(checkValuePrintParam[1],"成分数量：%02d",componentNum);
	sprintf(checkValuePrintParam[2],"生成日期：%s",thisDate);
	for (i = 0; i < componentNum; i++)
	{
		sprintf(checkValuePrintParam[i+3],"成分%02d的校验值：%s",i+1,checkValue[i]);
	}
	sprintf(checkValuePrintParam[3+componentNum],"密钥的校验值：%s",pdesKey->checkValue);

	if (((ret = RacalCmdA2(-1,psjl06,pdesKey->type,pdesKey->length,3+componentNum+1,checkValuePrintParam,component[i],errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
	{
		UnionUserErrLog("in UnionGenerateKeyComponent:: [%s] printCheckValue!\n",pdesKey->fullName);
		if (ret < 0)
			return(ret);
		else
			return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
	}
	UnionSuccessLog("in UnionGenerateKeyComponent:: [%s]\n",pdesKey->fullName);
	return(0); 
}

