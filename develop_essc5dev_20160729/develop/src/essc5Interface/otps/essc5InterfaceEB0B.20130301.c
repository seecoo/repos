/*
 * Copyright (c) 2011 Union Tech.
 * All rights reserved.
 *
 * Created on: 2016-01-27
 *   @Author: chenwd
 *   @Version 1.0
 */

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
/*
功能：指令WH离散卡片密钥并以安全报文方式导出（SM4）
输入参数：
securityMech		1A	安全机制		R：SM4加密和MAC
mode			1H	模式标志		0-仅加密 1-加密并计算MAC 2-加密并使用加密后数据计算校验值
id			1N	方案ID			密钥加密模式：PBOC3.0模式 0 = CBC模式，强制填充X80 1 = 带长度指引的ECB 2 = ECB模式，外带填充数据
mkType			3H	根密钥类型		109= MK-AC 209= MK-SMI 309= MK-SMC  509= MK-DN
mk			32H	根密钥	
mkIndex			1A+3H	根密钥索引	
mkCheckValue		16H	根密钥校验值		索引方式没有该域
mkDvsNum		1N	离散次数		指定对根密钥离散的次数（1-3次）
mkDvsData		n*16H	离散数据		离散卡片密钥的数据，其中n代表离散次数
pkType			1N	保护密钥类型		加密保护导出的卡片密钥的密钥类型 0=TK（传输密钥） 1=DK-SMC（使用MK-SMC实时离散生成的子密钥）
pk			32H	保护密钥	
pkIndex			1A+3H	保护密钥		保护密钥索引
pkCheckValue		16H	保护密钥校验值		保护密钥为索引方式没有该域
pkDvsNum		1N	保护密钥离散次数	仅当“保护密钥类型”为1时有 从MK-SMC离散得到DK-SMC的离散次数 范围为0-3
pkDvsData		n*16H	保护密钥离散数据	仅当“保护密钥类型”为1时有 保护密钥的离散数据，其中n为“保护密钥离散次数”
proKeyFlag		1A	过程密钥标识		是否对保护密钥进行过程计算,当需要计算过程密钥时,使用过程密钥保护数据. Y:计算过程密钥 N:不计算过程密钥 可选项:当没有该域时缺省为N
proFactor		32H	过程因子		计算过程密钥的数据	可选项:仅当过程密钥标志为Y时有 使用保护密钥的离散子密钥对此数据进行过程计算得到过程密钥。
ivCbc			32H	IV-CBC			仅当“方案ID”为0时有
encryptFillData		N*16H	加密填充数据		仅当“方案ID”为2时有和密钥明文一起进行加密
encryptFillOffset	4H	加密填充数据偏移量	仅当“方案ID”为2时有将密钥明文插入到加密填充数据的位置数值必须在0到加密填充数据长度之间
ivMac			32H	IV-MAC			仅当“模式标志”为1时有
macData			N*16H	MAC填充数据		仅当“模式标志”为1时有 和密钥密文一起进行MAC计算的数据
macOffset		4H	偏移量			仅当“模式标志”为1时有将密钥密文插入到MAC填充数据的位置数值必须在0到MAC填充数据长度之间

输出参数：
mac			16H	MAC值			仅当“模式标志”为1时有
criperDataLen		4H	密文数据长度		密文数据长度(必须是16的倍数，并等于前缀长度、后缀长度、密钥长度的和)
criperData		N*16H	密文数据		输出的密文数据
checkValue		16H	加密数据的校验值	仅当“模式标志”为2时有此项
*/
int UnionDealServiceCodeEB0B(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,mkIndex = -1,mkDvsNum = 1,len = 0,pkIndex = -1,pkDvsNum = 0,encryptFillOffset = 0,macOffset = 0, criperDataLen = 0;
	char	securityMech[32];
	char	mode[32];
	char	mk[64];
	char	id[32];
	char	mkCheckValue[32];
	char	mkType[32];
	char	mkDvsData[64];
	char	pk[64];
	char	pkCheckValue[32];
	char	pkType[32];
	char	pkDvsData[64];
	char	proKeyFlag[32];
	char	proFactor[64];
	char	ivCbc[64];
	char	encryptFillData[1056];
	char	ivMac[64];
	char	macData[1056];
	char	mac[64];
	char	tmpBuf[64];
	char	criperData[1056];
	char	checkValue[32];
	
	//安全机制
	if ((ret = UnionReadRequestXMLPackageValue("body/securityMech",securityMech,sizeof(securityMech))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/securityMech");
		return(ret);
	}
	securityMech[ret] = 0;

	//模式标志
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	mode[ret] = 0;	

	//方案ID
	if ((ret = UnionReadRequestXMLPackageValue("body/id",id,sizeof(id))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/id");
		return(ret);
	}
	id[ret] = 0;	
			
	//根密钥
	memset(mk,0,sizeof(mk));
	if ((ret = UnionReadRequestXMLPackageValue("body/mk",mk,sizeof(mk))) < 0)
	{
		//根密钥索引		4A
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}
	else
	{
		//根密钥校验值	
		if ((ret = UnionReadRequestXMLPackageValue("body/mkCheckValue",mkCheckValue,sizeof(mkCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkCheckValue");
			return(ret);
		}
		mkCheckValue[ret] = 0;
	}

	//根密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/mkType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkType");
		return(ret);
	}
	tmpBuf[ret] = 0;

	memset(mkType,0,sizeof(mkType));
	UnionTranslateDesKeyTypeTo3CharFormat(UnionConvertSymmetricKeyKeyType(tmpBuf), mkType);

	//离散次数
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	mkDvsNum = atoi(tmpBuf);
	
	if(mkDvsNum < 1 || mkDvsNum > 3)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: the mkDvsNum not 1 or 2 or 3!\n");
		return errCodePackageDefMDL_InvalidMaxFldNum;
	}
	
	//离散数据	
	if ((len = UnionReadRequestXMLPackageValue("body/mkDvsData",mkDvsData,sizeof(mkDvsData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData");
		return(ret);
	}
	mkDvsData[len] = 0;
	
	if((len/16) != mkDvsNum)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue mkDvsData[%s] len[%d] != %d*16!\n",mkDvsData,len,mkDvsNum);
		return(-1);
	}	
	
	//保护密钥
	memset(pk,0,sizeof(pk));
	memset(pkCheckValue,0,sizeof(pkCheckValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/pk",pk,sizeof(pk))) < 0)
	{
		//保护密钥索引
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}
	else
	{
		//保护密钥校验值
		if ((ret = UnionReadRequestXMLPackageValue("body/pkCheckValue",pkCheckValue,sizeof(pkCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkCheckValue");
			return(ret);
		}
		pkCheckValue[ret] = 0;
	}

	//保护密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/pkType",pkType,sizeof(pkType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkType");
		return(ret);
	}
	pkType[ret] = 0;

	if(strcmp(pkType,"1") == 0)
	{
		//保护密钥离散次数
		if ((ret = UnionReadRequestXMLPackageValue("body/pkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkDvsNum");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pkDvsNum = atoi(tmpBuf);
		
		if(pkDvsNum < 0 || pkDvsNum > 3)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: the pkDvsNum not 1 or 2 or 3!\n");
			return errCodePackageDefMDL_InvalidMaxFldNum;
		}
		
		if(pkDvsNum > 0)
		{
			//保护密钥离散数据	
			if ((len = UnionReadRequestXMLPackageValue("body/pkDvsData",pkDvsData,sizeof(pkDvsData))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkDvsData");
				return(len);
			}
			pkDvsData[len] = 0;
			
			if((len/16) != pkDvsNum)
			{
				UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue mkDvsData[%s] len[%d] != %d*16!\n",pkDvsData,len,pkDvsNum);
				return(-1);
			}
		}
	}

	//过程密钥标识
	memset(proKeyFlag,0,sizeof(proKeyFlag));
	if ((ret = UnionReadRequestXMLPackageValue("body/proKeyFlag",proKeyFlag,sizeof(proKeyFlag))) < 0)
	{
		strcpy(proKeyFlag,"N");
	}
	else if((strcmp(proKeyFlag,"Y") != 0)  && (strcmp(proKeyFlag,"N") != 0))
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s][%s] != Y or != N !\n","body/proKeyFlag",proKeyFlag);
		return -1;
	}
	
	memset(proFactor,0,sizeof(proFactor));
	if(strcmp(proKeyFlag,"Y") == 0)
	{
		//过程因子
		if ((ret = UnionReadRequestXMLPackageValue("body/proFactor",proFactor,sizeof(proFactor))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/proFactor");
			return(ret);
		}
	}
	
	memset(ivCbc,0,sizeof(ivCbc));
	memset(encryptFillData,0,sizeof(encryptFillData));
	if(strcmp(id,"0") == 0)
	{
		//ivCbc	
		if ((ret = UnionReadRequestXMLPackageValue("body/ivCbc",ivCbc,sizeof(ivCbc))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/ivCbc");
			return(ret);
		}
	}
	else if(strcmp(id,"2") == 0)
	{
		//加密填充数据
		if ((ret = UnionReadRequestXMLPackageValue("body/encryptFillData",encryptFillData,sizeof(encryptFillData))) > 0)
		{
			//加密填充数据偏移量
			if ((ret = UnionReadRequestXMLPackageValue("body/encryptFillOffset",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/encryptFillOffset");
				return(ret);
			}
			tmpBuf[ret] = 0;
			encryptFillOffset = atoi(tmpBuf);
		}
	}
				
	memset(ivMac,0,sizeof(ivMac));
	memset(macData,0,sizeof(macData));
	if(strcmp(mode,"1") == 0)
	{
		//IV-MAC	
		if ((ret = UnionReadRequestXMLPackageValue("body/ivMac",ivMac,sizeof(ivMac))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/ivMac");
			return(ret);
		}
		
		//MAC填充数据	
		if ((ret = UnionReadRequestXMLPackageValue("body/macData",macData,sizeof(macData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/macData");
			return(ret);
		}
		
		//偏移量
		if ((ret = UnionReadRequestXMLPackageValue("body/macOffset",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/macOffset");
			return(ret);
		}
		tmpBuf[ret] = 0;
		macOffset = atoi(tmpBuf);
	}
	
	memset(mac,0,sizeof(mac));
	memset(criperData,0,sizeof(criperData));
	memset(checkValue,0,sizeof(checkValue));
	if ((ret = UnionHsmCmdWH(securityMech, mode, id, mkType, mk, mkIndex, mkCheckValue, mkDvsNum, mkDvsData, pkType, pk, pkIndex, pkDvsNum, pkDvsData, pkCheckValue, proKeyFlag, proFactor, ivCbc, strlen(encryptFillData), encryptFillData, encryptFillOffset, ivMac, strlen(macData), macData, macOffset, mac, &criperDataLen, criperData, checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionHsmCmdWH err!\n");
		return(ret);
	}

	if(strcmp(mode,"1") == 0)
	{
		// 返回MAC值
		if ((ret = UnionSetResponseXMLPackageValue("body/mac",mac)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue newSeed[%s]!\n",mac);
			return(ret);
		}
	}
	else if(strcmp(mode,"2") == 0)
	{
		// 返回加密数据的校验值
		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue checkValue[%s]!\n",checkValue);
			return(ret);
		}
	}

        len = sprintf(tmpBuf,"%d",criperDataLen);
        tmpBuf[len] = 0;
        // 返回密文数据长度	
        if ((ret = UnionSetResponseXMLPackageValue("body/criperDataLen",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue criperDataLen[%s]!\n",tmpBuf);
                return(ret);
        }

	// 返回密文数据
	if ((ret = UnionSetResponseXMLPackageValue("body/criperData",criperData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue criperData[%s]!\n",criperData);
		return(ret);
	}
	
	return(0);
}
