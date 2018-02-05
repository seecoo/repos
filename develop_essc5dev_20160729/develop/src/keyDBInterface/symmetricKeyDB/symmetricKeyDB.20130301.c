//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-07-24

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionREC.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionErrCode.h"

#include "symmetricKeyDB.h"
#include "symmetricKeyDBJnl.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "unionHighCachedAPI.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"
#include "unionHsmCmd.h"

/* 
功能：	转换对称密钥名称
参数：	keyName[in|out]		密钥名称
	sizeofBuf[in]		缓冲大小
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionTransformSymmetricKeyName(char *keyName,int sizeofBuf)
{
	int	ret;
	int	len;
	char	unitID[32];
	char	newKeyName[128];
	char	*ptr = NULL;
	PUnionXMLPackageHead    pxmlPackageHead;
	
	pxmlPackageHead = UnionGetXMLPackageHead();
	
	if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// 阿里云
	{
		if (pxmlPackageHead->isUI)
		{
			if ((ret = UnionReadRequestXMLPackageValue("body/usingUnit",unitID,sizeof(unitID))) <= 0)
			{
				return(0);
			}
			unitID[ret] = 0;
		}
		else
		{
			if ((ret = UnionReadRequestXMLPackageValue("head/sysID",unitID,sizeof(unitID))) < 0)
			{
				UnionUserErrLog("in UnionTransformSymmetricKeyName:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
				return(ret);
			}
			if ((ptr = strchr(unitID,':')) == NULL)
			{
				UnionUserErrLog("in UnionTransformSymmetricKeyName:: sysID[%s] error!\n",unitID);
				UnionSetResponseRemark("系统ID[%s]格式错误",unitID);
				return(errCodeParameter);
			}
			ptr += 1;
			snprintf(unitID,sizeof(unitID),"%s",ptr);
		}
		
		// 密钥名称包含了单位编号
		len = snprintf(newKeyName,sizeof(newKeyName),"%s:",unitID);
		if (memcmp(keyName,newKeyName,len) == 0)
			return(0);
		
		len = snprintf(newKeyName,sizeof(newKeyName),"%s:%s",unitID,keyName);
		memcpy(keyName,newKeyName,len);
		keyName[len] = 0;
	}
	return(0);
}

/* 
功能：	获取对称密钥库在高速缓存中键名
参数：	keyName[in]		密钥名称
	highCachedKey[out]	键名
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGetKeyOfHighCachedForSymmetricKeyDB(char *keyName,char *highCachedKey)
{
	return(sprintf(highCachedKey,"%s:%s",defTableNameOfSymmetricKeyDB,keyName));
}

// 读取LMK保护方式表的保护密钥
int UnionReadProtectKeyFormLmkProtectMode(TUnionSymmetricAlgorithmID algorithmID,char *lmkProtectMode,char *keyValue,int sizeOfKey,char *checkValue,int sizeOfCV)
{
	int	ret;
	char	fieldNameOfKey[32];
	char	fieldNameOfCV[32];
	char	sql[512];
	
	switch(algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES算法
			snprintf(fieldNameOfKey,sizeof(fieldNameOfKey),"%s","desProtectKey");
			snprintf(fieldNameOfCV,sizeof(fieldNameOfKey),"%s","desProtectCV");
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
			snprintf(fieldNameOfKey,sizeof(fieldNameOfKey),"%s","sm4ProtectKey");
			snprintf(fieldNameOfCV,sizeof(fieldNameOfKey),"%s","sm4ProtectCV");
			break;
		default:
			UnionUserErrLog("in UnionReadProtectKeyFormLmkProtectMode:: algorithmID[%d] error!\n",algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	snprintf(sql,sizeof(sql),"select %s,%s from lmkProtectMode where modeID = '%s'",fieldNameOfKey,fieldNameOfCV,lmkProtectMode);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadProtectKeyFormLmkProtectMode:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionLog("in UnionReadProtectKeyFormLmkProtectMode:: select lmkProtectMode not find,sql[%s]!\n",sql);
		return(errCodeParameter);
	}
	UnionLocateXMLPackage("detail", 1);
	if ((ret = UnionReadXMLPackageValue(fieldNameOfKey, keyValue, sizeOfKey)) < 0)
	{
		UnionUserErrLog("in UnionReadProtectKeyFormLmkProtectMode:: UnionReadXMLPackageValue[%s]!\n",fieldNameOfKey);
		return(ret);
	}
	if ((ret = UnionReadXMLPackageValue(fieldNameOfCV, checkValue, sizeOfCV)) < 0)
	{
		UnionUserErrLog("in UnionReadProtectKeyFormLmkProtectMode:: UnionReadXMLPackageValue[%s]!\n",fieldNameOfCV);
		return(ret);
	}
	return(0);
}

// 获取对称密钥值
PUnionSymmetricKeyValue UnionGetSymmetricKeyValue(PUnionSymmetricKeyDB psymmetricKeyDB,char *lmkProtectMode)
{
	int	i;
	int	ret;
	int	len;
	int	num;
	char	sql[512];
	char	hsmCmdVersionID[32];
	char	zmk1[64];
	char	zmk2[64];
	char	keyByZMK[64];
	char	lmk[64];
	char	localCheckValue[32];
	char	checkValue[32];
	char	checkValue1[32];
	char	checkValue2[32];
	char	highCachedKey[256];
	char	keyDBStr[4096];
	
	TUnionHsmGroupRec	hsmGroupRec;
	
	for (i = 0; i < maxNumOfSymmetricKeyValue; i++)
	{
		if (strlen(psymmetricKeyDB->keyValue[i].lmkProtectMode) == 0)
		{
			if (i == 0)
			{
				UnionUserErrLog("in UnionGetSymmetricKeyValue:: %s no key value!\n",psymmetricKeyDB->keyName);
				return(NULL);
			}
			break;
		}
		
		if (strcmp(psymmetricKeyDB->keyValue[i].lmkProtectMode,lmkProtectMode) == 0)
			return(&psymmetricKeyDB->keyValue[i]);
	}
	num = i;
	
	// 没找到，转换密钥值
	// 使用第一个密钥值，查找一个可用的密码机组
	snprintf(sql,sizeof(sql),"select * from hsmGroup where lmkProtectMode = '%s' and enabled = 1",psymmetricKeyDB->keyValue[0].lmkProtectMode);
	if ((ret = UnionSelectRealDBRecord(sql,1,1)) < 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(NULL);
	}
	else if (ret == 0)
	{
		UnionLog("in UnionGetSymmetricKeyValue:: select hsmGroup not find,sql[%s]!\n",sql);
		return(NULL);
	}

	UnionLocateXMLPackage("detail", 1);
	
	snprintf(hsmGroupRec.lmkProtectMode,sizeof(hsmGroupRec.lmkProtectMode),"%s",psymmetricKeyDB->keyValue[0].lmkProtectMode);
	if ((ret = UnionReadXMLPackageValue("hsmGroupID", hsmGroupRec.hsmGroupID, sizeof(hsmGroupRec.hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
		return(NULL);
	}
	
	if ((ret = UnionReadXMLPackageValue("hsmCmdVersionID", hsmCmdVersionID, sizeof(hsmCmdVersionID))) < 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionReadXMLPackageValue[%s]!\n","hsmCmdVersionID");
		return(NULL);
	}
	//hsmCmdVersionID[ret] = 0;
	hsmGroupRec.hsmCmdVersionID = atoi(hsmCmdVersionID);
	
	// 读取ZMK1
	if ((ret = UnionReadProtectKeyFormLmkProtectMode(psymmetricKeyDB->algorithmID,hsmGroupRec.lmkProtectMode,zmk1,sizeof(zmk1),checkValue1,sizeof(checkValue1))) < 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionReadProtectKeyFormLmkProtectMode zmk1!\n");
		return(NULL);
	}
	// 读取ZMK2
	if ((ret = UnionReadProtectKeyFormLmkProtectMode(psymmetricKeyDB->algorithmID,lmkProtectMode,zmk2,sizeof(zmk2),checkValue2,sizeof(checkValue2))) < 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionReadProtectKeyFormLmkProtectMode zmk2!\n");
		return(NULL);
	}
	if (strcasecmp(checkValue1,checkValue2) != 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: checkValue1[%s] != checkValue2[%s]\n",checkValue1,checkValue2);
		return(NULL);
	}
	
	switch(psymmetricKeyDB->algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES算法
			// LMK->ZMK1
			for (i = 0; i < 2; i++)
			{
				UnionSetUseSpecHsmGroupForOneCmd(hsmGroupRec.hsmGroupID);
				if (i == 0)
				{
					snprintf(lmk,sizeof(lmk),"%s",psymmetricKeyDB->keyValue[0].keyValue);
					snprintf(localCheckValue,sizeof(localCheckValue),"%s",psymmetricKeyDB->checkValue);
				}
				else
				{
					snprintf(lmk,sizeof(lmk),"%s",psymmetricKeyDB->keyValue[0].oldKeyValue);
					snprintf(localCheckValue,sizeof(localCheckValue),"%s",psymmetricKeyDB->oldCheckValue);
				}

				if (strlen(lmk) == 0)
					continue;

				switch(hsmGroupRec.hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdA8(psymmetricKeyDB->keyType,lmk,zmk1,keyByZMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionHsmCmdA8 ret[%d]!\n",ret);
							return(NULL);
						}
						break;
					default:
						UnionUserErrLog("in UnionGetSymmetricKeyValue::  hsmGroupRec.hsmCmdVersionID[%d] error!\n",hsmGroupRec.hsmCmdVersionID);
						return(NULL);
				}

				// 检查校验值
				if (strcasecmp(localCheckValue,checkValue) != 0)
				{
					UnionUserErrLog("in UnionGetSymmetricKeyValue:: localCheckValue[%s] != checkValue[%s]\n",localCheckValue,checkValue);
					return(NULL);
				}
				
				// ZMK2->LMK
				switch(hsmGroupRec.hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,psymmetricKeyDB->keyType,zmk2,keyByZMK,psymmetricKeyDB->keyValue[num].keyValue,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionHsmCmdA6! ret[%d]\n",ret);
							return(NULL);
						}
						break;
					default:
						UnionUserErrLog("in UnionGetSymmetricKeyValue::  hsmGroupRec.hsmCmdVersionID[%d] error!\n",hsmGroupRec.hsmCmdVersionID);
						return(NULL);
				}

				// 检查校验值
				if (strcasecmp(localCheckValue,checkValue) != 0)
				{
					UnionUserErrLog("in UnionGetSymmetricKeyValue:: localCheckValue[%s] != checkValue[%s]\n",localCheckValue,checkValue);
					return(NULL);
				}
			}
			
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
			// LMK->ZMK1
			for (i = 0; i < 2; i++)
			{
				UnionSetUseSpecHsmGroupForOneCmd(hsmGroupRec.hsmGroupID);
				if (i == 0)
				{
					snprintf(lmk,sizeof(lmk),"%s",psymmetricKeyDB->keyValue[0].keyValue);
					snprintf(localCheckValue,sizeof(localCheckValue),"%s",psymmetricKeyDB->checkValue);
				}
				else
				{
					snprintf(lmk,sizeof(lmk),"%s",psymmetricKeyDB->keyValue[0].oldKeyValue);
					snprintf(localCheckValue,sizeof(localCheckValue),"%s",psymmetricKeyDB->oldCheckValue);
				}
				if (strlen(lmk) == 0)
					continue;

				switch(hsmGroupRec.hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdST(psymmetricKeyDB->keyType,lmk,zmk1,keyByZMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionHsmCmdST! ret[%d]\n",ret);
							return(NULL);
						}
						break;
					default:
						UnionUserErrLog("in UnionGetSymmetricKeyValue::  hsmGroupRec.hsmCmdVersionID[%d] error!\n",hsmGroupRec.hsmCmdVersionID);
						return(NULL);
				}

				// 检查校验值
				if (strcasecmp(localCheckValue,checkValue) != 0)
				{
					UnionUserErrLog("in UnionGetSymmetricKeyValue:: localCheckValue[%s] != checkValue1[%s]\n",localCheckValue,checkValue);
					return(NULL);
				}

				// ZMK2->LMK
				switch(hsmGroupRec.hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(psymmetricKeyDB->keyType,zmk2,keyByZMK,psymmetricKeyDB->keyValue[num].keyValue,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionHsmCmdSV! ret[%d]\n",ret);
							return(NULL);
						}
						break;
					default:
						UnionUserErrLog("in UnionGetSymmetricKeyValue::  hsmGroupRec.hsmCmdVersionID[%d] error!\n",hsmGroupRec.hsmCmdVersionID);
						return(NULL);
				}

				// 检查校验值
				if (strcasecmp(localCheckValue,checkValue) != 0)
				{
					UnionUserErrLog("in UnionGetSymmetricKeyValue:: localCheckValue[%s] != checkValue1[%s]\n",localCheckValue,checkValue);
					return(NULL);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionGetSymmetricKeyValue:: psymmetricKeyDB->algorithmID[%d] error!\n",psymmetricKeyDB->algorithmID);
			return(NULL);
	}

	len = sprintf(sql,"insert into %s(keyName,lmkProtectMode,keyValue,oldKeyValue) values ('%s','%s','%s','%s')",
		defTableNameOfSymmetricKeyValue,psymmetricKeyDB->keyName,lmkProtectMode,psymmetricKeyDB->keyValue[num].keyValue,psymmetricKeyDB->keyValue[num].oldKeyValue);
	sql[len] = 0;
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		return(NULL);
	}

	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	// 保存到高速缓存
	if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionGetSymmetricKeyValue:: UnionSymmetricKeyDBStructToString!\n");
		return(NULL);
	}
	keyDBStr[len] = 0;
	
	if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
		UnionProgramerLog("in UnionGetSymmetricKeyValue:: UnionSetHighCachedValue[%s] error!\n",highCachedKey);
	else
		UnionProgramerLog("in UnionGetSymmetricKeyValue:: UnionSetHighCachedValue highCachedKey[%s] lenOfStr[%d]\n",highCachedKey,len);
	
	return(&psymmetricKeyDB->keyValue[num]);
}

// 判断旧密钥是否允许使用
int UnionOldVersionSymmetricKeyIsUsed(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	time_t	now;
	time_t	keyUpdateTime;
	long	snap;
	int	windowPeriod = 0;
	
	if (psymmetricKeyDB == NULL)
	{
		UnionUserErrLog("in UnionOldVersionSymmetricKeyIsUsed:: null pointer!\n");
		return(0);
	}
	
	// 旧密钥不允许使用
	if (!psymmetricKeyDB->oldVersionKeyIsUsed)
		return(0);
	
	// 转换密钥更新时间
	keyUpdateTime = UnionTranslateStringTimeToTime(psymmetricKeyDB->keyUpdateTime);
	
	// 读取密钥窗口期
	if ((windowPeriod = UnionReadIntTypeRECVar("windowPeriodOfSymmetricKey")) <= 0)
		windowPeriod = 0;
	
	time(&now);
	// 计算时间差
	if (((snap = now - keyUpdateTime) > 0) && (snap < windowPeriod))
		return(1);
	else
		return(0);
}

// 把密钥类型内部枚举值转换为外部可识别
char *UnionConvertSymmetricKeyKeyTypeToExternal(TUnionSymmetricKeyType keyType)
{
	switch(keyType)
	{
		case	conZPK: return("ZPK");
		case	conZAK: return("ZAK"); 
		case	conZMK:	return("ZMK");
		case	conTMK: return("TMK");
		case	conTPK: return("TPK"); 
		case	conTAK: return("TAK");
		case	conPVK: return("PVK");
		case	conCVK: return("CVK");
		case	conZEK: return("ZEK");
		case	conWWK: return("WWK");
		case	conBDK: return("BDK");
		case	conEDK: return("EDK");
		case	conSelfDefinedKey: return("USER");
		case	conMKAC:    return("MK-AC");
		case	conMKSMI:   return("MK-SMI");	// add by leipp 20151023
		case	conMKSMC:   return("MK-SMC");
		case	conKMCSeed: return("KMC-Seed");
		case	conKMUSeed: return("KMU-Seed");
		case	conMDKSeed: return("MDK-Seed");
		default:
			UnionUserErrLog("in UnionIsValidSymmetricKeyType:: keyType[%d]!\n",keyType);
			UnionSetResponseRemark("密钥类型不正确");
			return(NULL);
		
	}
}

// 把密钥类型名称转为内部枚举值
TUnionSymmetricKeyType UnionConvertSymmetricKeyKeyType(char *keyTypeName)
{
	if (strcasecmp(keyTypeName,"ZPK") == 0)
		return(conZPK);
	if (strcasecmp(keyTypeName,"ZAK") == 0)
		return(conZAK);
	if (strcasecmp(keyTypeName,"ZMK") == 0)
		return(conZMK);
	if (strcasecmp(keyTypeName,"ZEK") == 0)
		return(conZEK);
	if (strcasecmp(keyTypeName,"TPK") == 0)
		return(conTPK);
	if (strcasecmp(keyTypeName,"TAK") == 0)
		return(conTAK);
	if (strcasecmp(keyTypeName,"TMK") == 0)
		return(conTMK);
	if (strcasecmp(keyTypeName,"PVK") == 0)
		return(conPVK);
	if (strcasecmp(keyTypeName,"CVK") == 0)
		return(conCVK);
	if (strcasecmp(keyTypeName,"WWK") == 0)
		return(conWWK);
	if (strcasecmp(keyTypeName,"BDK") == 0)
		return(conBDK);
	if (strcasecmp(keyTypeName,"EDK") == 0)
		return(conEDK);
	if (strcasecmp(keyTypeName,"USER") == 0)
		return(conSelfDefinedKey);
	if (strcasecmp(keyTypeName,"MK-AC") == 0)
		return(conMKAC);
	if (strcasecmp(keyTypeName,"MK-SMI") == 0)	// add by leipp 201501023
		return(conMKSMI);
	if (strcasecmp(keyTypeName,"MK-SMC") == 0)
		return(conMKSMC);
	if (strcasecmp(keyTypeName,"KMC-Seed") == 0)
		return(conKMCSeed);
	if (strcasecmp(keyTypeName,"KMU-Seed") == 0)
		return(conKMUSeed);
	if (strcasecmp(keyTypeName,"MDK-Seed") == 0)
		return(conMDKSeed);
	return(errCodeParameter);
}

// 把密钥长度转换为内部枚举值
TUnionSymmetricKeyLength UnionConvertSymmetricKeyDBKeyLen(int keyLen)
{
	switch (keyLen)
	{
		case    16:
		case    64:
			return(con64BitsSymmetricKey);
		case    32:
		case    128:
			return(con128BitsSymmetricKey);
		case    48:
		case    192:
			return(con192BitsSymmetricKey);
		default:
			UnionUserErrLog("in UnionConvertSymmetricKeyDBKeyLen:: keyLen[%d] is error!\n",keyLen);
			return(errCodeEssc_KeyLength);
	}
}

// 判断是否是合法的对称密钥类型
int UnionIsValidSymmetricKeyType(TUnionSymmetricKeyType keyType)
{
	switch (keyType)
	{
		case	conZPK:
		case	conZAK:
		case	conZMK:
		case	conTMK:
		case	conTPK:
		case	conTAK:
		case	conPVK:
		case	conCVK:
		case	conZEK:
		case	conWWK:
		case	conBDK:
		case	conEDK:
		case	conSelfDefinedKey:
		case	conMKAC:
		case	conMKSMI:	// add by leipp 20151023
		case	conMKSMC:
		case	conKMCSeed:
		case	conKMUSeed:
		case	conMDKSeed:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidSymmetricKeyType:: keyType[%d]!\n",keyType);
			UnionSetResponseRemark("密钥类型不正确");
			return(0);
	}
}

// 判断是否是合法的对称密钥长度
int UnionIsValidSymmetricKeyLength(TUnionSymmetricKeyLength keyLen)
{
	switch (keyLen)
	{
		case	con64BitsSymmetricKey:
		case	con128BitsSymmetricKey:
		case	con192BitsSymmetricKey:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidSymmetricKeyLength:: keyLen[%d]!\n",keyLen);
			UnionSetResponseRemark("密钥长度不正确");
			return(0);
	}
}

// 把对称密钥长度转为DES密钥长度
TUnionDesKeyLength UnionConvertSymmetricKeyKeyLen(TUnionSymmetricKeyLength keyLen)
{
	switch (keyLen)
	{
		case	con64BitsSymmetricKey:
			return(con64BitsDesKey);
		case	con128BitsSymmetricKey:
			return(con128BitsDesKey);
		case	con192BitsSymmetricKey:
			return(con192BitsDesKey);
		default:
			return(con128BitsDesKey);
	}
}

// 把算法标识名称转为内部枚举值
TUnionSymmetricAlgorithmID UnionConvertSymmetricKeyAlgorithmID(char *algorithmID)
{
	if (strcasecmp(algorithmID,"DES") == 0)
		return(conSymmetricAlgorithmIDOfDES);
	if (strcasecmp(algorithmID,"SM4") == 0)
		return(conSymmetricAlgorithmIDOfSM4);
	if (strcasecmp(algorithmID,"SM1") == 0)
		return(conSymmetricAlgorithmIDOfSM1);
	return(errCodeParameter);
}

// 把算法内部枚举值称转为标识名
int UnionConvertSymmetricKeyStringAlgorithmID(TUnionSymmetricAlgorithmID algorithmID,char *strAlgorithmID)
{
	switch (algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:
			sprintf(strAlgorithmID,"%s","DES");
			return(0);
		case	conSymmetricAlgorithmIDOfSM4:
			sprintf(strAlgorithmID,"%s","SM4");
			return(0);
		case	conSymmetricAlgorithmIDOfSM1:
			sprintf(strAlgorithmID,"%s","SM1");
			return(0);
		default:
			UnionUserErrLog("in UnionIsValidSymmetricKeyAlgorithmID:: algorithmID[%d]!\n",algorithmID);
			UnionSetResponseRemark("算法标识不正确");
			return(errCodeParameter);
	}
}

// 判断是否是合法的对称密钥算法
int UnionIsValidSymmetricKeyAlgorithmID(TUnionSymmetricAlgorithmID algorithmID)
{
	switch (algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:
		case	conSymmetricAlgorithmIDOfSM4:
		case	conSymmetricAlgorithmIDOfSM1:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidSymmetricKeyAlgorithmID:: algorithmID[%d]!\n",algorithmID);
			UnionSetResponseRemark("算法标识不正确");
			return(0);
	}
}

// 判断是否是合法的对称密钥名称
int UnionIsValidSymmetricKeyName(char *keyName)
{
	int	i;
	int	len;
	int	pointNum = 0;
	
	len = strlen(keyName);
	for (i = 0; i < len; i++)
	{
		if (keyName[i] == '.')
			pointNum++;
	}
	
	if (pointNum != 2)
	{
		// 20150626 zhangyd delete
		// UnionSetResponseRemark("密钥名称格式不正确");
		return(0);
	}
	else
	{
		if (keyName[len-1] == '.')
		{
			// 20150626 zhangyd delete
			// UnionSetResponseRemark("密钥名称格式不正确");
			return(0);
		}
		else
			return(1);
	}
}

// 从密钥名称中获取节点名称
int UnionAnalysisSymmetricKeyName(char *keyName,char *appID,char *keyNode,char *keyType)
{
	int	len;
	int	i,j;
 
	if (!UnionIsValidSymmetricKeyName(keyName))
		return(errCodeParameter);
		
	for (i = 0,j = 0,len = strlen(keyName); i < len; i++,j++)
	{  
		if (keyName[i] == '.')
		{
			appID[j] = 0;
			break;
		}  
		else
			appID[j] = keyName[i];
	}

	for (j = 0,++i; i < len; i++,j++)
	{
		if (keyName[i] == '.')
		{
			keyNode[j] = 0;
			break;
		}
		else
			keyNode[j] = keyName[i];
	}

	++i;
	memcpy(keyType,keyName+i,len - i);
	keyType[len-i] = 0;
	return(0);
}

// 从PBOC密钥名称中获取节点名称
int UnionAnalysisPBOCSymmetricKeyName(char *keyName, int version, char *appID, char *keyNode, char *keyType)
{
	int			ret;
	char			keyOwner[64];

	ret = UnionAnalysisSymmetricKeyName(keyName, appID, keyOwner, keyType);
	if(ret >= 0 && version > 0)
	{
		sprintf(keyNode, "%s-%03d", keyOwner, version);
	}
	else if(ret >= 0 && version == 0)
	{
		sprintf(keyNode, "%s", keyOwner);	
	}

	return(ret);
}

// 判断是否是合法的对称密钥
int UnionIsValidSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	if (psymmetricKeyDB == NULL)
	{
		UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: null pointer!\n");
		return(0);
	}

	if (!UnionIsValidSymmetricKeyName(psymmetricKeyDB->keyName))
	{
		UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyName error!\n");
		return(0);
	}
	if (!UnionIsValidSymmetricKeyLength(psymmetricKeyDB->keyLen))
	{
		UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyLength error!\n");
		return(0);
	}
	if (!UnionIsValidSymmetricKeyType(psymmetricKeyDB->keyType))
	{
		UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyType error!\n");
		return(0);
	}
	if (!UnionIsValidSymmetricKeyAlgorithmID(psymmetricKeyDB->algorithmID))
	{
		UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyAlgorithmID error!\n");
		return(0);
	}

	return(1);
}

/* 
功能：	创建一个对称密钥容器
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCreateSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[4096];
	
	// 转换密钥名称
	if ((ret = UnionTransformSymmetricKeyName(psymmetricKeyDB->keyName,sizeof(psymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDB:: UnionTransformSymmetricKeyName[%s]!\n",psymmetricKeyDB->keyName);
		return(ret);
	}
	
	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	UnionGetFullSystemDateTime(psymmetricKeyDB->createTime);

	// 保存到高速缓存
	if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDB:: UnionSymmetricKeyDBStructToString!\n");
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionCreateSymmetricKeyDB:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);

	UnionLog("in UnionCreateSymmetricKeyDB::update cache ok!\n");
	
	len = snprintf(sql,sizeof(sql),"insert into %s(keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,creatorType,creator,createTime,usingUnit,keyApplyPlatform,keyDistributePlatform,remark)values('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s');",
		defTableNameOfSymmetricKeyDB,
		psymmetricKeyDB->keyName,
		psymmetricKeyDB->keyGroup,
		psymmetricKeyDB->algorithmID,
		psymmetricKeyDB->keyType,
		psymmetricKeyDB->keyLen,
		psymmetricKeyDB->inputFlag,
		psymmetricKeyDB->outputFlag,
		psymmetricKeyDB->effectiveDays,
		psymmetricKeyDB->status,
		psymmetricKeyDB->oldVersionKeyIsUsed,
		psymmetricKeyDB->creatorType,
		psymmetricKeyDB->creator,
		psymmetricKeyDB->createTime,
		psymmetricKeyDB->usingUnit,
		psymmetricKeyDB->keyApplyPlatform,
                psymmetricKeyDB->keyDistributePlatform,
		psymmetricKeyDB->remark);
	
	if ((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationInsert,psymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDB:: UnionGetSqlForSymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDB:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	
	// 登录对称密钥更新日志
	// UnionInsertSymmetricKeyDBJnl(conSymmetricKeyDBOperationInsert,psymmetricKeyDB);
	
	return(0);
}

/* 
功能：	创建一个对称密钥容器,sql语句不同步
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCreateSymmetricKeyDBWithoutSqlSyn(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[4096];
	
	// 转换密钥名称
	if ((ret = UnionTransformSymmetricKeyName(psymmetricKeyDB->keyName,sizeof(psymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDBWithoutSqlSyn:: UnionTransformSymmetricKeyName[%s]!\n",psymmetricKeyDB->keyName);
		return(ret);
	}
	
	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	UnionGetFullSystemDateTime(psymmetricKeyDB->createTime);

	// 保存到高速缓存
	if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDBWithoutSqlSyn:: UnionSymmetricKeyDBStructToString!\n");
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionCreateSymmetricKeyDBWithoutSqlSyn:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);

	UnionLog("in UnionCreateSymmetricKeyDBWithoutSqlSyn::update cache ok!\n");
	
	len = snprintf(sql,sizeof(sql),"insert into %s(keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,creatorType,creator,createTime,usingUnit,keyApplyPlatform,keyDistributePlatform,remark)values('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s');",
		defTableNameOfSymmetricKeyDB,
		psymmetricKeyDB->keyName,
		psymmetricKeyDB->keyGroup,
		psymmetricKeyDB->algorithmID,
		psymmetricKeyDB->keyType,
		psymmetricKeyDB->keyLen,
		psymmetricKeyDB->inputFlag,
		psymmetricKeyDB->outputFlag,
		psymmetricKeyDB->effectiveDays,
		psymmetricKeyDB->status,
		psymmetricKeyDB->oldVersionKeyIsUsed,
		psymmetricKeyDB->creatorType,
		psymmetricKeyDB->creator,
		psymmetricKeyDB->createTime,
		psymmetricKeyDB->usingUnit,
		psymmetricKeyDB->keyApplyPlatform,
                psymmetricKeyDB->keyDistributePlatform,
		psymmetricKeyDB->remark);
	
	if ((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationInsert,psymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDBWithoutSqlSyn:: UnionGetSqlForSymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	
	if ((ret = UnionExecRealDBSql2(0, sql)) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDBWithoutSqlSyn:: UnionExecRealDBSql2 ret = [%d] sql = [%s]!\n",ret,sql);
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	
	// 登录对称密钥更新日志
	// UnionInsertSymmetricKeyDBJnl(conSymmetricKeyDBOperationInsert,psymmetricKeyDB);
	
	return(0);
}

/* 
功能：	更新一个对称密钥容器状态
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateSymmetricKeyDBStatus(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	ret;
	char	sql[81920];
	char	highCachedKey[256];
	int	len = 0;
	
	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	// 高速缓存中删除
	if ((ret = UnionDeleteHighCachedValue(highCachedKey)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBStatus:: UnionDeleteHighCachedValue[%s] error!\n",highCachedKey);
		return(ret);
	}
	
	//len = snprintf(sql,sizeof(sql),"update %s set status = %d where keyName = '%s'",defTableNameOfSymmetricKeyDB,psymmetricKeyDB->status,psymmetricKeyDB->keyName);
	len = snprintf(sql,sizeof(sql),"update %s set status = %d where keyName = '%s';",defTableNameOfSymmetricKeyDB,psymmetricKeyDB->status,psymmetricKeyDB->keyName);

	if ((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationDelete,psymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBStatus:: UnionGetSqlForSymmetricKeyDBJnl!\n");
		return(ret);
	}

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBStatus:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBStatus:: count[0] sql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	// 登录对称密钥更新日志
	// UnionInsertSymmetricKeyDBJnl(conSymmetricKeyDBOperationDelete,psymmetricKeyDB);

	return(0);
}

/* 
功能：	删除一个对称密钥容器
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionDropSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	
	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	// 高速缓存中删除
	if ((ret = UnionDeleteHighCachedValue(highCachedKey)) < 0)
	{
		UnionUserErrLog("in UnionDropSymmetricKeyDB:: UnionDeleteHighCachedValue[%s] error!\n",highCachedKey);
		return(ret);
	}
	
	/*
	len = sprintf(sql,"delete from %s where keyName = '%s'",defTableNameOfSymmetricKeyValue,psymmetricKeyDB->keyName);
	sql[len] = 0;
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDropSymmetricKeyDB:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	len = sprintf(sql,"delete from %s where keyName = '%s'",defTableNameOfSymmetricKeyDB,psymmetricKeyDB->keyName);
	sql[len] = 0;
	*/
	len = snprintf(sql,sizeof(sql),"delete from %s where keyName = '%s';",defTableNameOfSymmetricKeyValue,psymmetricKeyDB->keyName);
	len += snprintf(sql + len,sizeof(sql) - len,"delete from %s where keyName = '%s';",defTableNameOfSymmetricKeyDB,psymmetricKeyDB->keyName);
	if ((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationDelete,psymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionDropSymmetricKeyDB:: UnionGetSqlForSymmetricKeyDBJnl!\n");
		return(ret);
	}
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDropSymmetricKeyDB:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 登录对称密钥更新日志
	// UnionInsertSymmetricKeyDBJnl(conSymmetricKeyDBOperationDelete,psymmetricKeyDB);

	return(0);
}

/* 
功能：	把对称密钥数据结构转换成字符串
参数：	psymmetricKeyDB[in]	密钥容器信息
	keyDBStr[out]		密钥字符串
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionSymmetricKeyDBStructToString(PUnionSymmetricKeyDB psymmetricKeyDB,char *keyDBStr)
{
	//int	ret;
	//char	buf[32];
	int	len = 0;
	TUnionSymmetricKeyDBStr symmetricKeyDBStr;
	
	if ((keyDBStr == NULL) || (psymmetricKeyDB == NULL))
	{
		UnionUserErrLog("in UnionSymmetricKeyDBStructToString:: keyDBStr or psymmetricKeyDB is null!\n");
		return(errCodeParameter);
	}

	if(UnionIsSymmetricKeyDBRecInCachedOnStructFormat()) // cached 直接保存struct数据
	{
		len = sizeof(TUnionSymmetricKeyDB);
		memcpy(keyDBStr, (char *)psymmetricKeyDB, len);
	}
	else	// cached 保存字符流数据
	{
		len = sizeof(TUnionSymmetricKeyDBStr);
		UnionSymmetricKeyDBStructToStringStruct(&symmetricKeyDBStr, psymmetricKeyDB);
		memcpy(keyDBStr, (char *)&symmetricKeyDBStr, len);
	}

	return(len);
}

/* 
功能：	把对称密钥字符串转换成数据结构
参数：	keyDBStr[in]		密钥字符串
	psymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionSymmetricKeyDBStringToStruct(char *keyDBStr,PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	len = 0;
	TUnionSymmetricKeyDBStr symmetricKeyDBStr;
	
	if ((keyDBStr == NULL) || (psymmetricKeyDB == NULL))
	{
		UnionUserErrLog("in UnionSymmetricKeyDBStringToStruct:: keyDBStr or psymmetricKeyDB is null!\n");
		return(errCodeParameter);
	}
		
	if(UnionIsSymmetricKeyDBRecInCachedOnStructFormat()) // cached 直接保存struct数据
	{
		len = sizeof(TUnionSymmetricKeyDB);
		memcpy(psymmetricKeyDB, keyDBStr, len);
	}
	else    // cached 保存字符流数据
        {
                len = sizeof(TUnionSymmetricKeyDBStr);
                memcpy(&symmetricKeyDBStr, keyDBStr, len);
                UnionSymmetricKeyDBStringSturctToStruct(psymmetricKeyDB, &symmetricKeyDBStr);
        }

	return(0);
}

/*
int gunionCurrentNum = 0;
int UnionReadSymmetricKeyDB_CallBack(void *arg ,int nCount,char** pValue,char** pName)
{
	int			i;
	PUnionSymmetricKeyDB	psymmetricKeyDB = (PUnionSymmetricKeyDB)arg;
	
	for (i = 0; i < nCount; i++)
	{
		if (gunionCurrentNum == 0)
		{
			if (i == 1)
			{
				snprintf(psymmetricKeyDB->keyGroup,sizeof(psymmetricKeyDB->keyGroup),"%s",pValue[i]);
				continue;
			}
			else if (i == 2)
			{
				psymmetricKeyDB->algorithmID = atoi(pValue[i]);
				continue;
			}
			else if (i == 3)
			{
				psymmetricKeyDB->keyType = atoi(pValue[i]);
				continue;
			}
			else if (i == 4)
			{
				psymmetricKeyDB->keyLen = atoi(pValue[i]);
				continue;
			}
			else if (i == 5)
			{
				psymmetricKeyDB->inputFlag = atoi(pValue[i]);
				continue;
			}
			else if (i == 6)
			{
				psymmetricKeyDB->outputFlag = atoi(pValue[i]);
				continue;
			}
			else if (i == 7)
			{
				psymmetricKeyDB->effectiveDays = atoi(pValue[i]);
				continue;
			}
			else if (i == 8)
			{
				psymmetricKeyDB->status = atoi(pValue[i]);
				continue;
			}
			else if (i == 9)
			{
				psymmetricKeyDB->oldVersionKeyIsUsed = atoi(pValue[i]);
				continue;
			}
			else if (i == 10)
			{
				snprintf(psymmetricKeyDB->activeDate,sizeof(psymmetricKeyDB->activeDate),"%s",pValue[i]);
				continue;
			}
			else if (i == 11)
			{
				snprintf(psymmetricKeyDB->checkValue,sizeof(psymmetricKeyDB->checkValue),"%s",pValue[i]);
				continue;
			}
			else if (i == 12)
			{
				snprintf(psymmetricKeyDB->oldCheckValue,sizeof(psymmetricKeyDB->oldCheckValue),"%s",pValue[i]);
				continue;
			}
			else if (i == 13)
			{
				snprintf(psymmetricKeyDB->keyUpdateTime,sizeof(psymmetricKeyDB->keyUpdateTime),"%s",pValue[i]);
				continue;
			}
			else if (i == 14)
			{
				snprintf(psymmetricKeyDB->keyApplyPlatform,sizeof(psymmetricKeyDB->keyApplyPlatform),"%s",pValue[i]);
				continue;
			}
			else if (i == 15)
			{
				snprintf(psymmetricKeyDB->keyDistributePlatform,sizeof(psymmetricKeyDB->keyDistributePlatform),"%s",pValue[i]);
				continue;
			}
			else if (i == 16)
			{
				psymmetricKeyDB->creatorType = atoi(pValue[i]);
				continue;
			}
			else if (i == 17)
			{
				snprintf(psymmetricKeyDB->creator,sizeof(psymmetricKeyDB->creator),"%s",pValue[i]);
				continue;
			}
			else if (i == 18)
			{
				snprintf(psymmetricKeyDB->createTime,sizeof(psymmetricKeyDB->createTime),"%s",pValue[i]);
				continue;
			}
			else if (i == 19)
			{
				snprintf(psymmetricKeyDB->usingUnit,sizeof(psymmetricKeyDB->usingUnit),"%s",pValue[i]);
				continue;
			}
			else if (i == 20)
			{
				snprintf(psymmetricKeyDB->remark,sizeof(psymmetricKeyDB->remark),"%s",pValue[i]);
				continue;
			}
		}

		if (i == 21)
		{
			snprintf(psymmetricKeyDB->keyValue[gunionCurrentNum].lmkProtectMode,sizeof(psymmetricKeyDB->keyValue[gunionCurrentNum].lmkProtectMode),"%s",pValue[i]);
			continue;
		}
		else if (i == 22)
		{
			snprintf(psymmetricKeyDB->keyValue[gunionCurrentNum].keyValue,sizeof(psymmetricKeyDB->keyValue[gunionCurrentNum].keyValue),"%s",pValue[i]);
			continue;
		}
		else if (i == 23)
		{
			snprintf(psymmetricKeyDB->keyValue[gunionCurrentNum].oldKeyValue,sizeof(psymmetricKeyDB->keyValue[gunionCurrentNum].oldKeyValue),"%s",pValue[i]);
			continue;
		}
	}
	gunionCurrentNum ++;
	return(0);	
}
*/

/* 
功能：	读取一个对称密钥容器
参数：	keyName[in]		密钥名称
	isCheckKey[in]		是否检查密钥的可用性，1检查，0不检查
	psymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadSymmetricKeyDBRec(char *keyName,int isCheckKey,PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int		ret;
	int		len;
	int		i,num;
	int		isUseHighCached;
	char		sql[1024];
	char		tmpBuf[128];
	char		highCachedKey[256];
	char		keyDBStr[4096];
	char		tmpKeyName[136];
	
	snprintf(tmpKeyName,sizeof(tmpKeyName),"%s",keyName);
	snprintf(psymmetricKeyDB->keyName,sizeof(psymmetricKeyDB->keyName),"%s",tmpKeyName);
	
	// 转换密钥名称
	if ((ret = UnionTransformSymmetricKeyName(psymmetricKeyDB->keyName,sizeof(psymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDB:: UnionTransformSymmetricKeyName[%s]!\n",psymmetricKeyDB->keyName);
		return(ret);
	}

	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		isUseHighCached = 0;
	else
	{
		isUseHighCached = 1;	
		len = UnionGetKeyOfHighCachedForSymmetricKeyDB(keyName,highCachedKey);
		highCachedKey[len] = 0;
	}
	
	if (!isUseHighCached ||
		((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr,sizeof(keyDBStr))) <= 0))
	{
		// 设置通用查询不使用高速缓存
		if (isUseHighCached)
			UnionSetIsNotUseHighCached();
	
		// 从对称密钥库表中读取
		//len = sprintf(sql,"select * from %s where keyName = '%s'",defTableNameOfSymmetricKeyDB,psymmetricKeyDB->keyName);
		/*
		len = sprintf(sql,"select %s.*,lmkProtectMode,keyValue,oldKeyValue from %s left join %s on %s.keyName = %s.keyName where %s.keyName = '%s'",
			defTableNameOfSymmetricKeyDB,
			defTableNameOfSymmetricKeyDB,
			defTableNameOfSymmetricKeyValue,
			defTableNameOfSymmetricKeyDB,
			defTableNameOfSymmetricKeyValue,
			defTableNameOfSymmetricKeyDB,
			psymmetricKeyDB->keyName);
		sql[len] = 0;
		*/
		snprintf(sql,sizeof(sql),"select %s.keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,activeDate,checkValue,oldCheckValue,keyUpdateTime,keyApplyPlatform,keyDistributePlatform,creatorType,creator,createTime,usingUnit,%s.remark,lmkProtectMode,keyValue,oldKeyValue from %s left join %s on %s.keyName = %s.keyName where %s.keyName = '%s'",
			defTableNameOfSymmetricKeyDB,
			defTableNameOfSymmetricKeyDB,
			defTableNameOfSymmetricKeyDB,
			defTableNameOfSymmetricKeyValue,
			defTableNameOfSymmetricKeyDB,
			defTableNameOfSymmetricKeyValue,
			defTableNameOfSymmetricKeyDB,
			psymmetricKeyDB->keyName);
		
		if ((num = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionExecRealDBSql[%s]!\n",sql);
			return(num);
		}
		else if (num == 0)
		{
			UnionLog("in UnionReadSymmetricKeyDBRec:: keyName[%s] not find!\n",keyName);
			return(errCodeKeyCacheMDL_WrongKeyName);
		}

		for (i = 0; i < num; i++)
		{
			UnionLocateXMLPackage("detail", i + 1);
		
			if (i == 0)
			{
				if ((ret = UnionReadXMLPackageValue("keyGroup", psymmetricKeyDB->keyGroup, sizeof(psymmetricKeyDB->keyGroup))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyGroup");
					return(ret);
				}
		
				if ((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","algorithmID");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->algorithmID = atoi(tmpBuf);
			
				if ((ret = UnionReadXMLPackageValue("keyType", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyType");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->keyType = atoi(tmpBuf);
			
				if ((ret = UnionReadXMLPackageValue("keyLen", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyLen");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->keyLen = atoi(tmpBuf);
			
				if ((ret = UnionReadXMLPackageValue("inputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","inputFlag");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->inputFlag = atoi(tmpBuf);
			
				if ((ret = UnionReadXMLPackageValue("outputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","outputFlag");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->outputFlag = atoi(tmpBuf);
			
				if ((ret = UnionReadXMLPackageValue("effectiveDays", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","effectiveDays");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->effectiveDays = atoi(tmpBuf);
			
				if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","status");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->status = atoi(tmpBuf);
		
				if ((ret = UnionReadXMLPackageValue("oldVersionKeyIsUsed", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldVersionKeyIsUsed");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->oldVersionKeyIsUsed = atoi(tmpBuf);
				
				if ((ret = UnionReadXMLPackageValue("checkValue", psymmetricKeyDB->checkValue, sizeof(psymmetricKeyDB->checkValue))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","checkValue");
					return(ret);
				}
		
				if ((ret = UnionReadXMLPackageValue("oldCheckValue", psymmetricKeyDB->oldCheckValue, sizeof(psymmetricKeyDB->oldCheckValue))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldCheckValue");
					return(ret);
				}
				if ((ret = UnionReadXMLPackageValue("keyUpdateTime", psymmetricKeyDB->keyUpdateTime, sizeof(psymmetricKeyDB->keyUpdateTime))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyUpdateTime");
					return(ret);
				}
		
				if ((ret = UnionReadXMLPackageValue("activeDate", psymmetricKeyDB->activeDate, sizeof(psymmetricKeyDB->activeDate))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","activeDate");
					return(ret);
				}
			
				if ((ret = UnionReadXMLPackageValue("keyApplyPlatform", psymmetricKeyDB->keyApplyPlatform, sizeof(psymmetricKeyDB->keyApplyPlatform))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyApplyPlatform");
					return(ret);
				}
			
				if ((ret = UnionReadXMLPackageValue("keyDistributePlatform", psymmetricKeyDB->keyDistributePlatform, sizeof(psymmetricKeyDB->keyDistributePlatform))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyDistributePlatform");
					return(ret);
				}
			
				if ((ret = UnionReadXMLPackageValue("creatorType", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","creatorType");
					return(ret);
				}
				tmpBuf[ret] = 0;
				psymmetricKeyDB->creatorType = atoi(tmpBuf);
			
				if ((ret = UnionReadXMLPackageValue("creator", psymmetricKeyDB->creator, sizeof(psymmetricKeyDB->creator))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","creator");
					return(ret);
				}
			
				if ((ret = UnionReadXMLPackageValue("createTime", psymmetricKeyDB->createTime, sizeof(psymmetricKeyDB->createTime))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","createTime");
					return(ret);
				}
				
				if ((ret = UnionReadXMLPackageValue("usingUnit", psymmetricKeyDB->usingUnit, sizeof(psymmetricKeyDB->usingUnit))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","usingUnit");
					return(ret);
				}
				
				if ((ret = UnionReadXMLPackageValue("remark", psymmetricKeyDB->remark, sizeof(psymmetricKeyDB->remark))) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","remark");
					return(ret);
				}
			}

			if ((ret = UnionReadXMLPackageValue("lmkProtectMode", psymmetricKeyDB->keyValue[i].lmkProtectMode, sizeof(psymmetricKeyDB->keyValue[i].lmkProtectMode))) < 0)
			{
				UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","lmkProtectMode");
				return(ret);
			}
			if ((ret = UnionReadXMLPackageValue("keyValue", psymmetricKeyDB->keyValue[i].keyValue, sizeof(psymmetricKeyDB->keyValue[i].keyValue))) < 0)
			{
				UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyValue");
				return(ret);
			}
			if ((ret = UnionReadXMLPackageValue("oldKeyValue", psymmetricKeyDB->keyValue[i].oldKeyValue, sizeof(psymmetricKeyDB->keyValue[i].oldKeyValue))) < 0)
			{
				UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldKeyValue");
				return(ret);
			}
		}

		/*
		gunionCurrentNum = 0;
		if ((num = UnionSelectRealDBRecord2(sql,0,0,UnionReadSymmetricKeyDB_CallBack,(void *)psymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionExecRealDBSql[%s]!\n",sql);
			return(num);
		}
		else if (num == 0)
		{
			UnionLog("in UnionReadSymmetricKeyDBRec:: keyName[%s] not find!\n",keyName);
			return(errCodeKeyCacheMDL_WrongKeyName);
		}
		*/

		// 20150626 zhangyd add
		psymmetricKeyDB->keyValue[i].lmkProtectMode[0] = 0;
		
		if (isUseHighCached)
		{
			if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB,keyDBStr)) < 0)
			{
				UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionSymmetricKeyDBStructToString!\n");
				return(len);
			}
			keyDBStr[len] = 0;
			
			// 保存到高速缓存
			if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
				UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionSetHighCachedValue error!\n");
		}
	}
	else
	{
		if ((ret = UnionSymmetricKeyDBStringToStruct(keyDBStr,psymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionSymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr);
			return(ret);
		}
		UnionProgramerLog("in UnionReadSymmetricKeyDBRec:: keyName[%s] highCached!\n", psymmetricKeyDB->keyName);
	}
	
	if (isCheckKey)
	{
		// 检查密钥使用权限
		if ((ret = UnionCheckSymmetricKeyPrivilege(psymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionCheckSymmetricKeyPrivilege keyName[%s]!\n",psymmetricKeyDB->keyName);
			return(ret);
		}

		// 检查密钥有效性
		if ((ret = UnionCheckSymmetricKeyStatus(psymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionCheckSymmetricKeyStatus keyName[%s]!\n",psymmetricKeyDB->keyName);
			return(ret);
		}
	}
	return(0);
}

/* 
功能：	生成对称密钥
参数：	psymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGenerateSymmetricKeyDBRec(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[4096];
	
	// 转换密钥名称
	if ((ret = UnionTransformSymmetricKeyName(psymmetricKeyDB->keyName,sizeof(psymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionGenerateSymmetricKeyDBRec:: UnionTransformSymmetricKeyName[%s]!\n",psymmetricKeyDB->keyName);
		return(ret);
	}

	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	UnionGetFullSystemDateTime(psymmetricKeyDB->createTime);
	snprintf(psymmetricKeyDB->keyUpdateTime,sizeof(psymmetricKeyDB->keyUpdateTime),"%s",psymmetricKeyDB->createTime);		
	
	// 保存到高速缓存
	if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSymmetricKeyDBRec:: UnionSymmetricKeyDBStructToString!\n");
		return(len);
	}
	keyDBStr[len] = 0;

	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionGenerateSymmetricKeyDBRec:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);

	UnionLog("in UnionGenerateSymmetricKeyDBRec::update cache ok!\n");
	
	len = snprintf(sql,sizeof(sql),"insert into %s(keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,activeDate,checkValue,keyUpdateTime,keyApplyPlatform,keyDistributePlatform,creatorType,creator,createTime,usingUnit,remark)"
		"values('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s','%s','%s','%s',%d,'%s','%s','%s','%s');",
		defTableNameOfSymmetricKeyDB,
		psymmetricKeyDB->keyName,
		psymmetricKeyDB->keyGroup,
		psymmetricKeyDB->algorithmID,
		psymmetricKeyDB->keyType,
		psymmetricKeyDB->keyLen,
		psymmetricKeyDB->inputFlag,
		psymmetricKeyDB->outputFlag,
		psymmetricKeyDB->effectiveDays,
		psymmetricKeyDB->status,
		psymmetricKeyDB->oldVersionKeyIsUsed,
		psymmetricKeyDB->activeDate,
		psymmetricKeyDB->checkValue,
		psymmetricKeyDB->keyUpdateTime,
		psymmetricKeyDB->keyApplyPlatform,
		psymmetricKeyDB->keyDistributePlatform,
		psymmetricKeyDB->creatorType,
		psymmetricKeyDB->creator,
		psymmetricKeyDB->createTime,
		psymmetricKeyDB->usingUnit,
		psymmetricKeyDB->remark);
	/*
	sql[len] = 0;
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSymmetricKeyDBRec:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	*/
	//len = snprintf(sql,sizeof(sql),"insert into %s(keyName,lmkProtectMode,keyValue)"
	len += snprintf(sql + len,sizeof(sql) - len,"insert into %s(keyName,lmkProtectMode,keyValue)"
		"values('%s','%s','%s');",
		defTableNameOfSymmetricKeyValue,
		psymmetricKeyDB->keyName,
		psymmetricKeyDB->keyValue[0].lmkProtectMode,
		psymmetricKeyDB->keyValue[0].keyValue);
	
	if ((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationInsert,psymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSymmetricKeyDBRec:: UnionGetSqlForSymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSymmetricKeyDBRec:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}

	// 登录对称密钥更新日志
	// UnionInsertSymmetricKeyDBJnl(conSymmetricKeyDBOperationInsert,psymmetricKeyDB);

	return(0);
}

/* 
功能：	更新对称密钥
参数：	psymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateSymmetricKeyDBKeyValue(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	i;
	int	ret;
	int 	tmp;
	int	len = 0;;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[4096];
	char	sysDateTime[32];
	int	status = 0;
	int     numOfValue = 1;
	
	char		keyDBStr_backup[4096+1];		//add by lusj 20151221
	PUnionSymmetricKeyDB psymmetricKeyDB_backup=NULL;	//add by lusj 20151221
	

	if (psymmetricKeyDB->inputFlag == 2)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: inputFlag[%d] keyName is forbid update!\n",psymmetricKeyDB->inputFlag);
		return(errCodeEsscMDLKeyOperationNotPermitted);
	}
	
	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	//add begin by lusj 201512121 从memcached获取密钥信息.并保存
	psymmetricKeyDB_backup = (PUnionSymmetricKeyDB)malloc(sizeof(TUnionSymmetricKeyDB));
	if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr_backup,sizeof(keyDBStr_backup))) <= 0)
		UnionLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionGetHighCachedValue failed !\n");

	if ((ret = UnionSymmetricKeyDBStringToStruct(keyDBStr_backup,psymmetricKeyDB_backup)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionSymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr);
		return(ret);
	}	
	//UnionLog("in UnionUpdateSymmetricKeyDBKeyValue:: key[%s] oldvalue[%d][%s] !\n",psymmetricKeyDB_backup->keyName,(int)strlen(psymmetricKeyDB_backup->keyValue[0].keyValue),psymmetricKeyDB_backup->keyValue[0].keyValue);
	
	//add end  by lusj 201512121

	UnionGetFullSystemDateTime(sysDateTime);
	sysDateTime[14] = 0;

	snprintf(psymmetricKeyDB->keyUpdateTime,sizeof(psymmetricKeyDB->keyUpdateTime),"%s",sysDateTime);	

	 for (i = 1; i < maxNumOfSymmetricKeyValue; i++)
        {
                //memset(&psymmetricKeyDB->keyValue[i],0,sizeof(TUnionSymmetricKeyValue));
                if (strlen(psymmetricKeyDB->keyValue[i].lmkProtectMode) > 0)
                        numOfValue ++;

                psymmetricKeyDB->keyValue[i].lmkProtectMode[0] = 0;
        }

	status = psymmetricKeyDB->status;
        if (psymmetricKeyDB->status == conSymmetricKeyStatusOfInitial)  // 初始化状态
        {
                psymmetricKeyDB->status = conSymmetricKeyStatusOfEnabled;
                memcpy(psymmetricKeyDB->activeDate,sysDateTime,8);
		psymmetricKeyDB->activeDate[8] = 0;	// add by leipp 20151201
                numOfValue = 0;
        }

	// 保存到高速缓存
	if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionSymmetricKeyDBStructToString!\n");
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	UnionLog("in UnionUpdateSymmetricKeyDBKeyValue::update cache ok!\n");

	len = 0;
	if (numOfValue == 1 && (strlen(psymmetricKeyDB->keyValue[0].oldKeyValue) > 0))
        {
                len = snprintf(sql,sizeof(sql),"update %s set lmkProtectMode = '%s',keyValue = '%s',oldKeyValue = '%s' where keyName = '%s';",
                        defTableNameOfSymmetricKeyValue,
                        psymmetricKeyDB->keyValue[0].lmkProtectMode,
                        psymmetricKeyDB->keyValue[0].keyValue,
                        psymmetricKeyDB->keyValue[0].oldKeyValue,
                        psymmetricKeyDB->keyName);
        }
        else
        {
                if (numOfValue > 1)
                {
                        len = snprintf(sql,sizeof(sql),"delete from %s where keyName = '%s';",
                                defTableNameOfSymmetricKeyValue,
                                psymmetricKeyDB->keyName);
                }
                len += snprintf(sql + len,sizeof(sql) - len,"insert into %s(keyName,lmkProtectMode,keyValue,oldKeyValue)"
                        "values('%s','%s','%s','%s');",
                        defTableNameOfSymmetricKeyValue,
                        psymmetricKeyDB->keyName,
                        psymmetricKeyDB->keyValue[0].lmkProtectMode,
                        psymmetricKeyDB->keyValue[0].keyValue,
                        psymmetricKeyDB->keyValue[0].oldKeyValue);
        }

	// modify by leipp 20151022 增加修改密钥算法标识
        if (status == conSymmetricKeyStatusOfInitial)   // 初始化状态
        {
                len += snprintf(sql + len,sizeof(sql) - len,"update %s set algorithmID = %d,status = %d,activeDate = '%s',checkValue = '%s',keyUpdateTime='%s' where keyName = '%s';",
                        defTableNameOfSymmetricKeyDB,
			psymmetricKeyDB->algorithmID,
                        psymmetricKeyDB->status,
                        psymmetricKeyDB->activeDate,
                        psymmetricKeyDB->checkValue,
                        psymmetricKeyDB->keyUpdateTime,
                        psymmetricKeyDB->keyName);
        }
        else
        {
                len += snprintf(sql + len,sizeof(sql) - len,"update %s set algorithmID = %d,checkValue = '%s',oldCheckValue = '%s',keyUpdateTime='%s' where keyName = '%s';",
                        defTableNameOfSymmetricKeyDB,
			psymmetricKeyDB->algorithmID,
                        psymmetricKeyDB->checkValue,
                        psymmetricKeyDB->oldCheckValue,
                        psymmetricKeyDB->keyUpdateTime,
                        psymmetricKeyDB->keyName);
        }
	// modify end 20151022

        if ((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationUpdate,psymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
        {
                UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionGetSqlForSymmetricKeyDBJnl!\n");
                UnionDeleteHighCachedValue(highCachedKey);
                return(ret);
        }
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionExecRealDBSql update ret = [%d] sql = [%s]!\n",ret,sql);
		//UnionDeleteHighCachedValue(highCachedKey);

		
		// modify begin  by lusj  20151222  数据库更新失败根据情况进行memcached操作
		if(strlen(psymmetricKeyDB_backup->keyValue[0].keyValue) > 0)
		{	
			//原有的memcached存在密钥，则还原之前的密钥值
			if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB_backup,keyDBStr_backup)) < 0)
			{
				UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionSymmetricKeyDBStructToString!\n");
				return(len);
			}
			keyDBStr_backup[len] = 0;

			for(i = 0; i < 3; i++)
			{
				if ((tmp = UnionSetHighCachedValue(highCachedKey,keyDBStr_backup,len,0)) < 0)
					UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValue:: UnionSetHighCachedValue Of old_memcached_Value[%s] loop[%d] error!\n",highCachedKey,i+1);
				else
					break;
			}
		}
		else
			UnionDeleteHighCachedValue(highCachedKey);//原有的memcached不存在密钥值，则del操作
	
		// modify end  by lusj  20151222
	
		free(psymmetricKeyDB_backup);// add   by lusj  20151221	
		return(ret);
	}

	// 登录对称密钥更新日志
	// UnionInsertSymmetricKeyDBJnl(conSymmetricKeyDBOperationUpdate,psymmetricKeyDB);
	free(psymmetricKeyDB_backup);	// add   by lusj  20151221
	return(0);
}

/* 
功能：	更新对称密钥
参数：	psymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	i;
	int	ret;
	int 	tmp;
	int	len = 0;;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[4096];
	char	sysDateTime[32];
	int	status = 0;
	int     numOfValue = 1;
	
	char		keyDBStr_backup[4096+1];		//add by lusj 20151221
	PUnionSymmetricKeyDB psymmetricKeyDB_backup=NULL;	//add by lusj 20151221
	

	if (psymmetricKeyDB->inputFlag == 2)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: inputFlag[%d] keyName is forbid update!\n",psymmetricKeyDB->inputFlag);
		return(errCodeEsscMDLKeyOperationNotPermitted);
	}
	
	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(psymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	//add begin by lusj 201512121 从memcached获取密钥信息.并保存
	psymmetricKeyDB_backup = (PUnionSymmetricKeyDB)malloc(sizeof(TUnionSymmetricKeyDB));
	if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr_backup,sizeof(keyDBStr_backup))) <= 0)
		UnionLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionGetHighCachedValue failed !\n");

	if ((ret = UnionSymmetricKeyDBStringToStruct(keyDBStr_backup,psymmetricKeyDB_backup)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr);
		return(ret);
	}	
	//UnionLog("in UnionUpdateSymmetricKeyDBKeyValue:: key[%s] oldvalue[%d][%s] !\n",psymmetricKeyDB_backup->keyName,(int)strlen(psymmetricKeyDB_backup->keyValue[0].keyValue),psymmetricKeyDB_backup->keyValue[0].keyValue);
	
	//add end  by lusj 201512121

	UnionGetFullSystemDateTime(sysDateTime);
	sysDateTime[14] = 0;

	snprintf(psymmetricKeyDB->keyUpdateTime,sizeof(psymmetricKeyDB->keyUpdateTime),"%s",sysDateTime);	

	 for (i = 1; i < maxNumOfSymmetricKeyValue; i++)
        {
                //memset(&psymmetricKeyDB->keyValue[i],0,sizeof(TUnionSymmetricKeyValue));
                if (strlen(psymmetricKeyDB->keyValue[i].lmkProtectMode) > 0)
                        numOfValue ++;

                psymmetricKeyDB->keyValue[i].lmkProtectMode[0] = 0;
        }

	status = psymmetricKeyDB->status;
        if (psymmetricKeyDB->status == conSymmetricKeyStatusOfInitial)  // 初始化状态
        {
                psymmetricKeyDB->status = conSymmetricKeyStatusOfEnabled;
                memcpy(psymmetricKeyDB->activeDate,sysDateTime,8);
		psymmetricKeyDB->activeDate[8] = 0;	// add by leipp 20151201
                numOfValue = 0;
        }

	// 保存到高速缓存
	if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSymmetricKeyDBStructToString!\n");
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	UnionLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn::update cache ok!\n");

	len = 0;
	if (numOfValue == 1 && (strlen(psymmetricKeyDB->keyValue[0].oldKeyValue) > 0))
        {
                len = snprintf(sql,sizeof(sql),"update %s set lmkProtectMode = '%s',keyValue = '%s',oldKeyValue = '%s' where keyName = '%s';",
                        defTableNameOfSymmetricKeyValue,
                        psymmetricKeyDB->keyValue[0].lmkProtectMode,
                        psymmetricKeyDB->keyValue[0].keyValue,
                        psymmetricKeyDB->keyValue[0].oldKeyValue,
                        psymmetricKeyDB->keyName);
        }
        else
        {
                if (numOfValue > 1)
                {
                        len = snprintf(sql,sizeof(sql),"delete from %s where keyName = '%s';",
                                defTableNameOfSymmetricKeyValue,
                                psymmetricKeyDB->keyName);
                }
                len += snprintf(sql + len,sizeof(sql) - len,"insert into %s(keyName,lmkProtectMode,keyValue,oldKeyValue)"
                        "values('%s','%s','%s','%s');",
                        defTableNameOfSymmetricKeyValue,
                        psymmetricKeyDB->keyName,
                        psymmetricKeyDB->keyValue[0].lmkProtectMode,
                        psymmetricKeyDB->keyValue[0].keyValue,
                        psymmetricKeyDB->keyValue[0].oldKeyValue);
        }

	// modify by leipp 20151022 增加修改密钥算法标识
        if (status == conSymmetricKeyStatusOfInitial)   // 初始化状态
        {
                len += snprintf(sql + len,sizeof(sql) - len,"update %s set algorithmID = %d,status = %d,activeDate = '%s',checkValue = '%s',keyUpdateTime='%s' where keyName = '%s';",
                        defTableNameOfSymmetricKeyDB,
			psymmetricKeyDB->algorithmID,
                        psymmetricKeyDB->status,
                        psymmetricKeyDB->activeDate,
                        psymmetricKeyDB->checkValue,
                        psymmetricKeyDB->keyUpdateTime,
                        psymmetricKeyDB->keyName);
        }
        else
        {
                len += snprintf(sql + len,sizeof(sql) - len,"update %s set algorithmID = %d,checkValue = '%s',oldCheckValue = '%s',keyUpdateTime='%s' where keyName = '%s';",
                        defTableNameOfSymmetricKeyDB,
			psymmetricKeyDB->algorithmID,
                        psymmetricKeyDB->checkValue,
                        psymmetricKeyDB->oldCheckValue,
                        psymmetricKeyDB->keyUpdateTime,
                        psymmetricKeyDB->keyName);
        }
	// modify end 20151022

        if ((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationUpdate,psymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
        {
                UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionGetSqlForSymmetricKeyDBJnl!\n");
                UnionDeleteHighCachedValue(highCachedKey);
                return(ret);
        }
	
	if ((ret = UnionExecRealDBSql2(0, sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionExecRealDBSql2 update ret = [%d] sql = [%s]!\n",ret,sql);
		//UnionDeleteHighCachedValue(highCachedKey);

		
		// modify begin  by lusj  20151222  数据库更新失败根据情况进行memcached操作
		if(strlen(psymmetricKeyDB_backup->keyValue[0].keyValue) > 0)
		{	
			//原有的memcached存在密钥，则还原之前的密钥值
			if ((len = UnionSymmetricKeyDBStructToString(psymmetricKeyDB_backup,keyDBStr_backup)) < 0)
			{
				UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSymmetricKeyDBStructToString!\n");
				return(len);
			}
			keyDBStr_backup[len] = 0;

			for(i = 0; i < 3; i++)
			{
				if ((tmp = UnionSetHighCachedValue(highCachedKey,keyDBStr_backup,len,0)) < 0)
					UnionUserErrLog("in UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSetHighCachedValue Of old_memcached_Value[%s] loop[%d] error!\n",highCachedKey,i+1);
				else
					break;
			}
		}
		else
			UnionDeleteHighCachedValue(highCachedKey);//原有的memcached不存在密钥值，则del操作
	
		// modify end  by lusj  20151222
	
		free(psymmetricKeyDB_backup);// add   by lusj  20151221	
		return(ret);
	}

	// 登录对称密钥更新日志
	// UnionInsertSymmetricKeyDBJnl(conSymmetricKeyDBOperationUpdate,psymmetricKeyDB);
	free(psymmetricKeyDB_backup);	// add   by lusj  20151221
	return(0);
}

/*
功能：	检查对称密钥使用权限
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckSymmetricKeyPrivilege(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	ret;
	char	unitID[32];
	char	*ptr = NULL;

	if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",unitID,sizeof(unitID))) < 0)
		{
			UnionUserErrLog("in UnionTransformSymmetricKeyName:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		if ((ptr = strchr(unitID,':')) == NULL)
		{
			UnionUserErrLog("in UnionTransformSymmetricKeyName:: sysID[%s] error!\n",unitID);
			UnionSetResponseRemark("系统ID[%s]格式错误",unitID);
			return(errCodeParameter);
		}
		ptr += 1;
		snprintf(unitID,sizeof(unitID),"%s",ptr);

		if (strcmp(unitID,psymmetricKeyDB->usingUnit) != 0)
		{
			UnionUserErrLog("in UnionCheckAsymmetricKeyPrivilege:: 请求单位ID[%s] != 密钥的使用单位[%s]\n",unitID,psymmetricKeyDB->usingUnit);
			return(errCodeHsmCmdMDL_KeyNotPermitted);
		}
	}

	return 0;
}

/* 
功能：	检查对称密钥状态
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckSymmetricKeyStatus(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	ret;
	char	systemDate[32];
	char	keyUpdateDate[32];
	char	effectiveDate[32];
	
	if (psymmetricKeyDB->status != conSymmetricKeyStatusOfEnabled)
	{
		UnionUserErrLog("in UnionCheckSymmetricKeyStatus:: status[%d] != [%d]!\n",psymmetricKeyDB->status,conSymmetricKeyStatusOfEnabled);
		UnionSetResponseRemark("密钥[%s]未启用",psymmetricKeyDB->keyName);
		return(errCodeEsscMDL_KeyStatusDisabled);
	}
	
	// 检查密钥生效日期
	UnionGetFullSystemDate(systemDate);
	systemDate[8] = 0;
	if ((strlen(psymmetricKeyDB->activeDate) == 0) || (strcasecmp(psymmetricKeyDB->activeDate,systemDate) > 0))
	{
		UnionUserErrLog("in UnionCheckSymmetricKeyStatus:: 生效日期[%s] > 系统日期[%s]!\n",psymmetricKeyDB->activeDate,systemDate);
		UnionSetResponseRemark("密钥[%s]未生效",psymmetricKeyDB->keyName);
		return(errCodeEsscMDL_KeyStatusNotActive);
	}

	// 检查有效时间
	if (psymmetricKeyDB->effectiveDays > 0)
	{
		memcpy(keyUpdateDate,psymmetricKeyDB->keyUpdateTime,8);
		keyUpdateDate[8] = 0;
		
		// 计算失效日期
		if ((ret = UnionDecideDateAfterSpecDate(keyUpdateDate,psymmetricKeyDB->effectiveDays,effectiveDate)) < 0)
		{
			UnionUserErrLog("in UnionCheckSymmetricKeyStatus:: UnionDecideDateAfterSpecDate[%s]!\n",keyUpdateDate);
			return(ret);
		}
		effectiveDate[8] = 0;
		if (strcasecmp(effectiveDate,systemDate) < 0)
		{
			UnionUserErrLog("in UnionCheckSymmetricKeyStatus:: 密钥[%s]已失效,更新日期[%.8s],有效天数[%d]!\n",psymmetricKeyDB->keyName,keyUpdateDate,psymmetricKeyDB->effectiveDays);
			UnionSetResponseRemark("密钥[%s]已失效,更新日期[%.8s],有效天数[%d]",psymmetricKeyDB->keyName,keyUpdateDate,psymmetricKeyDB->effectiveDays);
			return(errCodeEsscMDL_KeyStatusNotActive);
		}
	}
	return(0);
}

/* 
功能：	拼接对称密钥的创建者
参数：	sysID[in]	系统ID
	appID[in]	应用ID
	creator[out]	创建者
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionPieceSymmetricKeyCreator(char *sysID,char *appID,char *creator)
{
	return(sprintf(creator,"%s:%s",sysID,appID));
}

/*
功能：	打印密钥
参数：	phsmGroupRec:		对称密钥容器
	psymmetricKeyDB：	密钥信息
	keyValuePrintFormat： 	打印格式
	numOfComponent： 	分量数量
	hsmIP： 		加密机IP
返回：
	psymmetricKeyDB:	密钥值和校验值
*/
int UnionGenerateAndPrintSymmetricKey(PUnionHsmGroupRec phsmGroupRec,PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValuePrintFormat,int isCheckAlonePrint,int numOfComponent,char *hsmIP,int appendPrintNum,char appendPrintParam[][80])
{
	int				ret = 0;
	int				len = 0;
	char				*ptr = NULL;
	int				i = 0;
	int				j = 0;
	char				checkValuePrintFormat[512];
	int				valuePrintNum = 0;
	char				component[20][50];
	char				componentCheckValue[20][32];
	char				keyValue[64];
	char				checkValue[32];
	char				valuePrintParam[20][80];
	char				currentDateTime[32];
	char				tmpCheckValue[32];
	char				tmpBuf[512];
	char				keyName[160];
	time_t				stime;
	struct				tm *curTime;


	PUnionSymmetricKeyValue 	psymmetricKeyValue = NULL;

	// 获取校验值格式
	if ((ptr = strchr(keyValuePrintFormat,';')) != NULL)
	{
		len = sprintf(checkValuePrintFormat,"%s",ptr+1);
		checkValuePrintFormat[len] = 0;
		keyValuePrintFormat[strlen(keyValuePrintFormat) - len - 1] = 0;
		UnionFilterHeadAndTailBlank(checkValuePrintFormat);

		if (strlen(checkValuePrintFormat) == 0)
		{
			UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: checkValuePrintFormat can not be null!\n");
			return(errCodeParameter);
		}
	}
	else
	{
		UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: ';' not found in keyValuePrintFormat[%s],get checkValuePrintFormat failed!\n",keyValuePrintFormat);
		return(errCodeParameter);
	}

	if (numOfComponent <= 0 || numOfComponent > 9)
	{
		UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: numOfComponent[%d] error!\n",numOfComponent);
		return(errCodeParameter);
	}

	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(psymmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		snprintf(psymmetricKeyValue->oldKeyValue,sizeof(psymmetricKeyValue->oldKeyValue),"%s",psymmetricKeyValue->keyValue);
		snprintf(psymmetricKeyDB->oldCheckValue,sizeof(psymmetricKeyDB->oldCheckValue),"%s",psymmetricKeyDB->checkValue);
	}
	else
		psymmetricKeyValue = &psymmetricKeyDB->keyValue[0];

	snprintf(psymmetricKeyValue->lmkProtectMode,sizeof(psymmetricKeyValue->lmkProtectMode),"%s",phsmGroupRec->lmkProtectMode);


	time(&stime);
	curTime = localtime(&stime);
	strftime(currentDateTime,sizeof(currentDateTime),"%F %T",curTime);

	// 打印指定数量的密钥成分，并生成对应校验值
	for (i = 0; i < numOfComponent; i++)
	{
		// 打印密钥值
		// 设置密钥打印格式
		// 设置使用指定加密机
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case	conHsmCmdVerRacalStandardHsmCmd:
			case	conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdPA(keyValuePrintFormat)) < 0)
				{
					UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdPA[%s]!\n",keyValuePrintFormat);
					return(ret);
				}
				break;
			default:
				UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("非法的加密机指令类型");
				return(errCodeParameter);
		}

		valuePrintNum = 4 + appendPrintNum;
		for (j = 0; j < valuePrintNum + 1; j++)
			memset(valuePrintParam[j],0,sizeof(valuePrintParam[j]));

		// 密钥名称
		if (psymmetricKeyDB->algorithmID == conSymmetricAlgorithmIDOfDES)
			len = sprintf(keyName,"%s    (DES算法)",psymmetricKeyDB->keyName);
		else
			len = sprintf(keyName,"%s    (SM4算法)",psymmetricKeyDB->keyName);
		keyName[len] = 0;

		if (strlen(keyName) >= 80)
		{
			UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: keyName[%s][%zu] >= 80\n",keyName,strlen(keyName));
			UnionSetResponseRemark("打印的密钥名称行太长");
			return(errCodeParameter);
		}

		// 密钥名称
		snprintf(valuePrintParam[0],sizeof(valuePrintParam[0]),"%s", keyName);
		// 分量数量
		snprintf(valuePrintParam[1],sizeof(valuePrintParam[1]), "%d", numOfComponent);
		// 分量序列号
		snprintf(valuePrintParam[2],sizeof(valuePrintParam[2]),"%02d",i+1);
		// 生成日期
		snprintf(valuePrintParam[3],sizeof(valuePrintParam[3]),"%s", currentDateTime);

		// 设置附件打印域
		for (j = 0; j < appendPrintNum; j++)
		{
			len = sprintf(valuePrintParam[4+j],"%s",appendPrintParam[j]);
			valuePrintParam[4+j][len] = 0;
		}

		// 设置正常zmk
		if (numOfComponent == 1)
			UnionSetIsUseNormalZmkType();

		// 设置使用指定加密机
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

		// 生成并打印密钥
		switch (psymmetricKeyDB->algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:
				// DES
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA2(psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),valuePrintNum,valuePrintParam,component[i])) < 0)
						{
							UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdA2!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:
				// SM4
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdWJ(psymmetricKeyDB->keyType,valuePrintNum,valuePrintParam,component[i])) < 0)
						{
								UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdWJ!\n");
								return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: algorithmID[%d] is invalid\n",psymmetricKeyDB->algorithmID);
				UnionSetResponseRemark("非法的算法标识");
				return(errCodeParameter);
		}

		// 设置正常zmk
		if (numOfComponent == 1)
			UnionSetIsUseNormalZmkType();

		// 设置使用指定加密机
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

		// 为每个密钥成份生成校验值
		switch (psymmetricKeyDB->algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:

				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdBU(0,psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),component[i],componentCheckValue[i])) < 0)
						{
								UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdBU DES psymmetricKeyDB->algorithmID[%d]!\n",psymmetricKeyDB->algorithmID);
								return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:

				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdBU(1,psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),component[i],componentCheckValue[i])) < 0)
						{
								UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdBU SM4 psymmetricKeyDB->algorithmID[%d]!\n",psymmetricKeyDB->algorithmID);
								return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: algorithmID[%d] is invalid\n",psymmetricKeyDB->algorithmID);
				UnionSetResponseRemark("非法的算法标识");
				return(errCodeParameter);
		}

		if (!isCheckAlonePrint)
		{
			// 设置使用指定加密机
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

			// 设置校验值打印格式
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdPA(checkValuePrintFormat)) < 0)
					{
							UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdPA[%s]!\n",checkValuePrintFormat);
							return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}

			valuePrintNum = 2;
			for (j = 0; j < valuePrintNum + 1; j++)
				memset(valuePrintParam[j],0,sizeof(valuePrintParam[j]));

			// 成分校验值
			snprintf(valuePrintParam[0],sizeof(valuePrintParam[0]),"%s","校验值:");
			snprintf(valuePrintParam[1],sizeof(valuePrintParam[1]),"%s",componentCheckValue[i]);
		}

		// 将各个密钥成分合成一把密钥，并生成合成密钥的校验值
		if ((i == numOfComponent -1) && i >= 1) 
		{
			// 设置使用指定密码机
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

			// 合成密钥
			switch (psymmetricKeyDB->algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:

					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA4(0,psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),numOfComponent,component,keyValue,checkValue)) < 0)
							{
								UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdA4 DES psymmetricKeyDB->algorithmID[%d]!\n",psymmetricKeyDB->algorithmID);
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:

					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA4(1,psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),numOfComponent,component,keyValue,checkValue)) < 0)
							{
								UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdA4 SM4 psymmetricKeyDB->algorithmID[%d]!\n",psymmetricKeyDB->algorithmID);
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: algorithmID[%d] is invalid\n",psymmetricKeyDB->algorithmID);
					UnionSetResponseRemark("非法的算法标识");
					return(errCodeParameter);
			}

			if (psymmetricKeyDB->algorithmID == conSymmetricAlgorithmIDOfDES)
			{
				// 为合成密钥重新生成校验值
				// 设置使用指定加密机
				UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

				UnionSetIsUseNormalZmkType();
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdBU(0,psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),keyValue,tmpCheckValue)) < 0)
						{
								UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdBU [DES]!\n");
								return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}

				if (memcmp(tmpCheckValue,checkValue,strlen(checkValue)) != 0)
				{
					UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey::  checkValue HsmCmdA4[%s]  != HsmCmdBU[%s] error!\n",checkValue,tmpCheckValue);
					UnionSetResponseRemark("BU指令生成校验值错误");
					return(errCodeEssc_CheckValue);
				}

				snprintf(checkValue,sizeof(checkValue),"%s",tmpCheckValue);
			}

			// 总校验值
			valuePrintNum = 4;
			snprintf(valuePrintParam[2],sizeof(valuePrintParam[2]),"总校验值:");
			snprintf(valuePrintParam[3],sizeof(valuePrintParam[3]),"%s",checkValue);
		}

		// 是否独立打印校验值
		if (!isCheckAlonePrint)
		{
			// 打印校验值
			// 设置使用指定加密机
			UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);
			if (psymmetricKeyDB->algorithmID == conSymmetricAlgorithmIDOfDES)
			{
				// DES
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA2(psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),valuePrintNum,valuePrintParam,tmpBuf)) < 0)
						{
							UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdA2!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
			}
			else
			{
				// SM4  
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdWJ(psymmetricKeyDB->keyType,valuePrintNum,valuePrintParam,tmpBuf)) < 0)
						{
							UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdWJ!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
			}
		}
	}

	// 单独打印校验值	
	if (isCheckAlonePrint)
	{
		// 设置使用指定加密机
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

		// 设置校验值打印格式
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case	conHsmCmdVerRacalStandardHsmCmd:
			case	conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdPA(checkValuePrintFormat)) < 0)
				{
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdPA[%s]!\n",checkValuePrintFormat);
						return(ret);
				}
				break;
			default:
				UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("非法的加密机指令类型");
				return(errCodeParameter);
		}

		valuePrintNum = 2 + numOfComponent + 1;
		for (j = 0; j < valuePrintNum + numOfComponent; j++)
			memset(valuePrintParam[j],0,sizeof(valuePrintParam[j]));

		// 密钥名称
		snprintf(valuePrintParam[0],sizeof(valuePrintParam[0]),"%s",keyName);
		// 分量数量
		//snprintf(valuePrintParam[1], "%d", numOfComponent);
		// 生成日期
		snprintf(valuePrintParam[1],sizeof(valuePrintParam[1]),"%s",currentDateTime);

		// 设置成份校验值
		for (j = 0; j < numOfComponent; j++)
		{
			sprintf(valuePrintParam[2+j],"成份%d校验值 :%s",j+1,componentCheckValue[j]);
		}

		// 设置总校验值
		sprintf(valuePrintParam[2+j],"合成后校验值:%s", checkValue);

		// 打印校验值
		// 设置使用指定加密机
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);
		if (psymmetricKeyDB->algorithmID == conSymmetricAlgorithmIDOfDES)
		{
			// DES
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdA2(psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),valuePrintNum,valuePrintParam,tmpBuf)) < 0)
					{
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdA2!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
		}
		else
		{
			// SM4  
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdWJ(psymmetricKeyDB->keyType,valuePrintNum,valuePrintParam,tmpBuf)) < 0)
					{
						UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: UnionHsmCmdWJ!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionGenerateAndPrintSymmetricKey:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
		}
	}

	snprintf(psymmetricKeyValue->keyValue,sizeof(psymmetricKeyValue->keyValue),"%s",keyValue);
	snprintf(psymmetricKeyDB->checkValue,sizeof(psymmetricKeyValue->keyValue),"%s",checkValue);
	return 0;
}

int UnionSymmetricKeyDBStructToStringStruct(PUnionSymmetricKeyDBStr psymmetricKeyDBStr, const PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int ret = 0;

	strcpy(psymmetricKeyDBStr->keyName, 	psymmetricKeyDB->keyName);
	strcpy(psymmetricKeyDBStr->keyGroup, 	psymmetricKeyDB->keyGroup);
	ret = sprintf(psymmetricKeyDBStr->algorithmID, "%d", psymmetricKeyDB->algorithmID);
	ret = sprintf(psymmetricKeyDBStr->keyType, 	 "%d", psymmetricKeyDB->keyType);
	ret = sprintf(psymmetricKeyDBStr->keyLen, 	 "%d", psymmetricKeyDB->keyLen);
	ret = sprintf(psymmetricKeyDBStr->inputFlag,	 "%d", psymmetricKeyDB->inputFlag);
	ret = sprintf(psymmetricKeyDBStr->outputFlag,	 "%d", psymmetricKeyDB->outputFlag);
	ret = sprintf(psymmetricKeyDBStr->effectiveDays,	 "%d", psymmetricKeyDB->effectiveDays);
	ret = sprintf(psymmetricKeyDBStr->status,	 "%d", psymmetricKeyDB->status);
	ret = sprintf(psymmetricKeyDBStr->oldVersionKeyIsUsed,	 "%d", psymmetricKeyDB->oldVersionKeyIsUsed);
	strcpy(psymmetricKeyDBStr->activeDate,	psymmetricKeyDB->activeDate);
	memcpy(&psymmetricKeyDBStr->keyValue,	psymmetricKeyDB->keyValue, sizeof(psymmetricKeyDB->keyValue));
	strcpy(psymmetricKeyDBStr->checkValue,	psymmetricKeyDB->checkValue);
	strcpy(psymmetricKeyDBStr->oldCheckValue,	psymmetricKeyDB->oldCheckValue);
	strcpy(psymmetricKeyDBStr->keyUpdateTime,	psymmetricKeyDB->keyUpdateTime);
	strcpy(psymmetricKeyDBStr->keyApplyPlatform,	psymmetricKeyDB->keyApplyPlatform);
	strcpy(psymmetricKeyDBStr->keyDistributePlatform, psymmetricKeyDB->keyDistributePlatform);
	ret = sprintf(psymmetricKeyDBStr->creatorType,	"%d",	psymmetricKeyDB->creatorType);
	strcpy(psymmetricKeyDBStr->creator,		psymmetricKeyDB->creator);
	strcpy(psymmetricKeyDBStr->createTime,		psymmetricKeyDB->createTime);
	strcpy(psymmetricKeyDBStr->usingUnit,		psymmetricKeyDB->usingUnit);
	strcpy(psymmetricKeyDBStr->remark,		psymmetricKeyDB->remark);

	return ret;
}

int UnionSymmetricKeyDBStringSturctToStruct(PUnionSymmetricKeyDB psymmetricKeyDB, const  PUnionSymmetricKeyDBStr psymmetricKeyDBStr)
{
	int	ret = 0;
	
	strcpy(psymmetricKeyDB->keyName, 	psymmetricKeyDBStr->keyName);
	strcpy(psymmetricKeyDB->keyGroup, 	psymmetricKeyDBStr->keyGroup);
	psymmetricKeyDB->algorithmID	 = atoi(psymmetricKeyDBStr->algorithmID);
	psymmetricKeyDB->keyType	 = atoi(psymmetricKeyDBStr->keyType);
	psymmetricKeyDB->keyLen	 	 = atoi(psymmetricKeyDBStr->keyLen);
	psymmetricKeyDB->inputFlag	 = atoi(psymmetricKeyDBStr->inputFlag);
	psymmetricKeyDB->outputFlag	 = atoi(psymmetricKeyDBStr->outputFlag);
	psymmetricKeyDB->effectiveDays	 = atoi(psymmetricKeyDBStr->effectiveDays);
	psymmetricKeyDB->status		 = atoi(psymmetricKeyDBStr->status);
	psymmetricKeyDB->oldVersionKeyIsUsed	 = atoi(psymmetricKeyDBStr->oldVersionKeyIsUsed);
	strcpy(psymmetricKeyDB->activeDate,	psymmetricKeyDBStr->activeDate);
	memcpy(&psymmetricKeyDB->keyValue,	psymmetricKeyDBStr->keyValue, sizeof(psymmetricKeyDBStr->keyValue));
	strcpy(psymmetricKeyDB->checkValue,	psymmetricKeyDBStr->checkValue);
	strcpy(psymmetricKeyDB->oldCheckValue,	psymmetricKeyDBStr->oldCheckValue);
	strcpy(psymmetricKeyDB->keyUpdateTime,	psymmetricKeyDBStr->keyUpdateTime);
	strcpy(psymmetricKeyDB->keyApplyPlatform,	psymmetricKeyDBStr->keyApplyPlatform);
	strcpy(psymmetricKeyDB->keyDistributePlatform, psymmetricKeyDBStr->keyDistributePlatform);
	psymmetricKeyDB->creatorType =  atoi(psymmetricKeyDBStr->creatorType);
	strcpy(psymmetricKeyDB->creator,		psymmetricKeyDBStr->creator);
	strcpy(psymmetricKeyDB->createTime,		psymmetricKeyDBStr->createTime);
	strcpy(psymmetricKeyDB->usingUnit,		psymmetricKeyDBStr->usingUnit);
	strcpy(psymmetricKeyDB->remark,		psymmetricKeyDBStr->remark);

	UnionProgramerLog("in UnionSymmetricKeyDBStringSturctToStruct:: \nkeyName=[%s:%s], keyGroup=[%s:%s], algorithmID=[%d:%s],keyType=[%d:%s], keyLen=[%d:%s], inputFlag=[%d:%s], outputFlag=[%d:%s],effectiveDays=[%d:%s],status=[%d:%s],oldVersionKeyIsUsed=[%d:%s],activeDate=[%s:%s], checkValue=[%s:%s],oldCheckValue=[%s:%s],keyUpdateTime=[%s:%s], keyApplyPlatform=[%s:%s],keyDistributePlatform=[%s:%s], creatorType=[%d:%s], creator=[%s:%s], createTime=[%s:%s],usingUnit=[%s:%s],remark=[%s:%s]\n" , 
	psymmetricKeyDB->keyName,psymmetricKeyDBStr->keyName, 
	psymmetricKeyDB->keyGroup, psymmetricKeyDBStr->keyGroup,
	psymmetricKeyDB->algorithmID, psymmetricKeyDBStr->algorithmID,
	psymmetricKeyDB->keyType,  psymmetricKeyDBStr->keyType, 
	psymmetricKeyDB->keyLen, psymmetricKeyDBStr->keyLen,
 	psymmetricKeyDB->inputFlag, psymmetricKeyDBStr->inputFlag,
	psymmetricKeyDB->outputFlag ,psymmetricKeyDBStr->outputFlag,
	psymmetricKeyDB->effectiveDays, psymmetricKeyDBStr->effectiveDays,
	psymmetricKeyDB->status, psymmetricKeyDBStr->status,
	psymmetricKeyDB->oldVersionKeyIsUsed,psymmetricKeyDBStr->oldVersionKeyIsUsed,
	psymmetricKeyDB->activeDate,psymmetricKeyDBStr->activeDate,
	psymmetricKeyDB->checkValue,	psymmetricKeyDBStr->checkValue,
	psymmetricKeyDB->oldCheckValue,	psymmetricKeyDBStr->oldCheckValue,
	psymmetricKeyDB->keyUpdateTime,	psymmetricKeyDBStr->keyUpdateTime,
	psymmetricKeyDB->keyApplyPlatform,	psymmetricKeyDBStr->keyApplyPlatform,
	psymmetricKeyDB->keyDistributePlatform, psymmetricKeyDBStr->keyDistributePlatform,
	psymmetricKeyDB->creatorType,	psymmetricKeyDBStr->creatorType,
	psymmetricKeyDB->creator,		psymmetricKeyDBStr->creator,
	psymmetricKeyDB->createTime,		psymmetricKeyDBStr->createTime,
	psymmetricKeyDB->usingUnit,	psymmetricKeyDBStr->usingUnit,
	psymmetricKeyDB->remark,		psymmetricKeyDBStr->remark);

	return ret;
}

int UnionIsSymmetricKeyDBRecInCachedOnStructFormat()
{
	int		ret;
	if ((ret = UnionReadIntTypeRECVar("isKeyDBRecInCachedOnStructFormat")) < 0)
	{
		return(1);
	}
	return(ret);
}

// 初始化密钥结构体
void UnionInitSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	i = 0;

	psymmetricKeyDB->keyName[0] = 0;	
	psymmetricKeyDB->keyGroup[0] = 0;	
	psymmetricKeyDB->activeDate[0] = 0;	

	for (i = 0; i < maxNumOfSymmetricKeyValue; i++)
	{
		psymmetricKeyDB->keyValue[i].lmkProtectMode[0] = 0;
		psymmetricKeyDB->keyValue[i].keyValue[0] = 0;
		psymmetricKeyDB->keyValue[i].oldKeyValue[0] = 0;
	}

	psymmetricKeyDB->checkValue[0] = 0;	
	psymmetricKeyDB->oldCheckValue[0] = 0;	
	psymmetricKeyDB->keyUpdateTime[0] = 0;	
	psymmetricKeyDB->keyApplyPlatform[0] = 0;	
	psymmetricKeyDB->keyDistributePlatform[0] = 0;	
	psymmetricKeyDB->creator[0] = 0;	
	psymmetricKeyDB->createTime[0] = 0;	
	psymmetricKeyDB->usingUnit[0] = 0;	
	psymmetricKeyDB->remark[0] = 0;	
}

int UnionIsUseCheckSymmetricKeyDB(TUnionSymmetricKeyStatus status,int flag)
{
	// flag = 0,表示外部app应用
	// flag = 1,表示内部app应用，内部使用密钥无需检查状态
	if (!flag)
	{
		switch (status)
		{
			case    conSymmetricKeyStatusOfInitial:         // 初始化状态
				break;
			case    conSymmetricKeyStatusOfEnabled:         // 启用状态
				break;
			default:
				UnionUserErrLog("in UnionIsUseCheckSymmetricKeyDB:: symmetricKeyDB status[%d] error!\n",status);
				UnionSetResponseRemark("非法密钥状态");
				return(errCodeEsscMDL_KeyStatusDisabled);
		}
	}

	return 0;
}
