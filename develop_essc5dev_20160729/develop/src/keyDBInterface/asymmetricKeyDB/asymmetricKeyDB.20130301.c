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

#include "asymmetricKeyDB.h"
#include "asymmetricKeyDBJnl.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "unionHighCachedAPI.h"

/* 
功能：	转换非对称密钥名称
参数：	keyName[in|out]		密钥名称
	sizeofBuf[in]		缓冲大小
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionTransformAsymmetricKeyName(char *keyName,int sizeofBuf)
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
				UnionUserErrLog("in UnionTransformAsymmetricKeyName:: UnionReadRequestXMLPackageValue[%s]!\n","head/unitID");
				return(ret);
			}
			if ((ptr = strchr(unitID,':')) == NULL)
                        {
                                UnionUserErrLog("in UnionTransformAsymmetricKeyName:: sysID[%s] error!\n",unitID);
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
功能：	获取非对称密钥库在高速缓存中键名
参数：	keyName[in]		密钥名称
	highCachedKey[out]	键名
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGetKeyOfHighCachedForAsymmetricKeyDB(char *keyName,char *highCachedKey)
{
	return(sprintf(highCachedKey,"%s:%s",defTableNameOfAsymmetricKeyDB,keyName));
}

// 判断旧密钥是否允许使用
int UnionOldVersionAsymmetricKeyIsUsed(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	time_t	now;
	time_t	keyUpdateTime;
	long	snap;
	int	windowPeriod = 0;
	
	if (pasymmetricKeyDB == NULL)
	{
		UnionUserErrLog("in UnionOldVersionAsymmetricKeyIsUsed:: null pointer!\n");
		return(0);
	}
	
	// 旧密钥不允许使用
	if (!pasymmetricKeyDB->oldVersionKeyIsUsed)
	{
		UnionUserErrLog("in UnionOldVersionAsymmetricKeyIsUsed:: [%s] oldVersionKeyIsUsed [%d]!\n", pasymmetricKeyDB->keyName, pasymmetricKeyDB->oldVersionKeyIsUsed);
		return(0);
	}
	
	// 转换密钥更新时间
	keyUpdateTime = UnionTranslateStringTimeToTime(pasymmetricKeyDB->keyUpdateTime);
	
	// 读取密钥窗口期
	if ((windowPeriod = UnionReadIntTypeRECVar("windowPeriodOfAsymmetricKey")) <= 0)
	{
		UnionUserErrLog("in UnionOldVersionAsymmetricKeyIsUsed:: UnionReadIntTypeRECVar read [windowPeriodOfAsymmetricKey] [%d]\n", windowPeriod);
		windowPeriod = 0;
	}
	
	time(&now);
	UnionAuditLog("in UnionOldVersionAsymmetricKeyIsUsed:: [%s] snap [%d]\n", pasymmetricKeyDB->keyName, (int)(now - keyUpdateTime));
	// 计算时间差
	if (((snap = now - keyUpdateTime) > 0) && (snap < windowPeriod))
		return(1);
	else
		return(0);
}

// 判断是否是合法的非对称密钥类型
int UnionIsValidAsymmetricKeyType(TUnionAsymmetricKeyType keyType)
{
	switch (keyType)
	{
		case	conAsymmetricKeyTypeOfSignature:
		case	conAsymmetricKeyTypeOfEncryption:
		case	conAsymmetricKeyTypeOfSignatureAndEncryption:
		case	conAsymmetricKeyTypeOfConsult:
		case	conAsymmetricKeyTypeOfSignatureAndEncryptionAndKeyMng:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidAsymmetricKeyType:: keyType[%d]!\n",keyType);
			UnionSetResponseRemark("密钥类型不正确");
			return(0);
	}
}

// 判断是否是合法的非对称密钥长度
int UnionIsValidAsymmetricKeyLength(TUnionAsymmetricKeyLength keyLen)
{
	switch (keyLen)
	{
		case	con256BitsAsymmetricKey:
		case	con512BitsAsymmetricKey:
		case	con1024BitsAsymmetricKey:
		case	con1152BitsAsymmetricKey:
		case	con1408BitsAsymmetricKey:
		case	con1984BitsAsymmetricKey:
		case	con2048BitsAsymmetricKey:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidAsymmetricKeyLength:: keyLen[%d]!\n",keyLen);
			UnionSetResponseRemark("密钥长度不正确");
			return(0);
	}
}

// 把算法标识名称转为内部枚举值
TUnionAsymmetricAlgorithmID UnionConvertAsymmetricKeyAlgorithmID(char *algorithmID)
{
	UnionToUpperCase(algorithmID);
	if (strcmp(algorithmID,"RSA") == 0)
		return(conAsymmetricAlgorithmIDOfRSA);
	if (strcmp(algorithmID,"SM2") == 0)
		return(conAsymmetricAlgorithmIDOfSM2);
	return(errCodeParameter);
}

// 判断是否是合法的非对称密钥算法
int UnionIsValidAsymmetricKeyAlgorithmID(TUnionAsymmetricAlgorithmID algorithmID)
{
	switch (algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:
		case	conAsymmetricAlgorithmIDOfSM2:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidAsymmetricKeyAlgorithmID:: algorithmID[%d]!\n",algorithmID);
			UnionSetResponseRemark("算法标识不正确");
			return(0);
	}
}

// 判断是否是合法的非对称密钥名称
int UnionIsValidAsymmetricKeyName(char *keyName)
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
		UnionSetResponseRemark("密钥名称格式不正确");
		return(0);
	}
	else
	{
		if (keyName[len-1] == '.')
		{
			UnionSetResponseRemark("密钥名称格式不正确");
			return(0);
		}
		else
			return(1);
	}
}

// 从密钥名称中获取属主名称
int UnionAnalysisAsymmetricKeyName(char *keyName,char *appID,char *ownerName,char *keyType)
{
	int	len;
	int	i,j;
 
	if (!UnionIsValidAsymmetricKeyName(keyName))
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
			ownerName[j] = 0;
			break;
		}
		else
			ownerName[j] = keyName[i];
	}

	++i;
	memcpy(keyType,keyName+i,len - i);
	keyType[len-i] = 0;
	return(0);
}

// 从PBOC密钥名称中获取属主名称
int UnionAnalysisPBOCAsymmetricKeyName(char *keyName, int version, char *appID, char *ownerName, char *keyType)
{
	int			ret;
	char			keyOwner[64];

	ret = UnionAnalysisAsymmetricKeyName(keyName, appID, keyOwner, keyType);
	if(ret >= 0 && version > 0)
	{
		sprintf(ownerName, "%s-%03d", keyOwner, version);
	}

	return(ret);
}

// 判断是否是合法的非对称密钥
int UnionIsValidAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	if (pasymmetricKeyDB == NULL)
	{
		UnionUserErrLog("in UnionIsValidAsymmetricKeyDB:: null pointer!\n");
		return(0);
	}

	if (!UnionIsValidAsymmetricKeyName(pasymmetricKeyDB->keyName))
	{
		UnionUserErrLog("in UnionIsValidAsymmetricKeyDB:: UnionIsValidAsymmetricKeyName error!\n");
		return(0);
	}
	if (!UnionIsValidAsymmetricKeyLength(pasymmetricKeyDB->keyLen))
	{
		UnionUserErrLog("in UnionIsValidAsymmetricKeyDB:: UnionIsValidAsymmetricKeyLength error!\n");
		return(0);
	}
	if (!UnionIsValidAsymmetricKeyType(pasymmetricKeyDB->keyType))
	{
		UnionUserErrLog("in UnionIsValidAsymmetricKeyDB:: UnionIsValidAsymmetricKeyType error!\n");
		return(0);
	}
	if (!UnionIsValidAsymmetricKeyAlgorithmID(pasymmetricKeyDB->algorithmID))
	{
		UnionUserErrLog("in UnionIsValidAsymmetricKeyDB:: UnionIsValidAsymmetricKeyAlgorithmID error!\n");
		return(0);
	}

	return(1);
}

/* 
功能：	创建一个非对称密钥容器
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCreateAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[8192*2];
	
	// 转换密钥名称
	if ((ret = UnionTransformAsymmetricKeyName(pasymmetricKeyDB->keyName,sizeof(pasymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDB:: UnionTransformAsymmetricKeyName[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}

	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	UnionGetFullSystemDateTime(pasymmetricKeyDB->createTime);
	pasymmetricKeyDB->createTime[14] = 0;
	
	// 保存到高速缓存
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDB:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionCreateAsymmetricKeyDB:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	len = snprintf(sql,sizeof(sql),"insert into %s(keyName,keyGroup,algorithmID,keyType,pkExponent,keyLen,inputFlag,outputFlag,effectiveDays,status,vkStoreLocation,hsmGroupID,vkIndex,oldVersionKeyIsUsed,creatorType,creator,createTime,usingUnit,remark)"
		"values('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s',%d,%d,'%s','%s','%s','%s');",
		defTableNameOfAsymmetricKeyDB,
		pasymmetricKeyDB->keyName,
		pasymmetricKeyDB->keyGroup,
		pasymmetricKeyDB->algorithmID,
		pasymmetricKeyDB->keyType,
		pasymmetricKeyDB->pkExponent,
		pasymmetricKeyDB->keyLen,
		pasymmetricKeyDB->inputFlag,
		pasymmetricKeyDB->outputFlag,
		pasymmetricKeyDB->effectiveDays,
		pasymmetricKeyDB->status,
		pasymmetricKeyDB->vkStoreLocation,
		pasymmetricKeyDB->hsmGroupID,
		pasymmetricKeyDB->vkIndex,
		pasymmetricKeyDB->oldVersionKeyIsUsed,
		pasymmetricKeyDB->creatorType,
		pasymmetricKeyDB->creator,
		pasymmetricKeyDB->createTime,
		pasymmetricKeyDB->usingUnit,
		pasymmetricKeyDB->remark);
	
	if ((ret = UnionGetSqlForAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDB:: UnionGetSqlForAsymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDB:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	
	// 登录非对称密钥更新日志
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB);
	
	return(0);
}

/* 
功能：	创建一个非对称密钥容器,sql语句不同步
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCreateAsymmetricKeyDBWithoutSqlSyn(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[8192*2];
	
	// 转换密钥名称
	if ((ret = UnionTransformAsymmetricKeyName(pasymmetricKeyDB->keyName,sizeof(pasymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDBWithoutSqlSyn:: UnionTransformAsymmetricKeyName[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}

	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	UnionGetFullSystemDateTime(pasymmetricKeyDB->createTime);
	pasymmetricKeyDB->createTime[14] = 0;
	
	// 保存到高速缓存
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDBWithoutSqlSyn:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionCreateAsymmetricKeyDBWithoutSqlSyn:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	len = snprintf(sql,sizeof(sql),"insert into %s(keyName,keyGroup,algorithmID,keyType,pkExponent,keyLen,inputFlag,outputFlag,effectiveDays,status,vkStoreLocation,hsmGroupID,vkIndex,oldVersionKeyIsUsed,creatorType,creator,createTime,usingUnit,remark)"
		"values('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s',%d,%d,'%s','%s','%s','%s');",
		defTableNameOfAsymmetricKeyDB,
		pasymmetricKeyDB->keyName,
		pasymmetricKeyDB->keyGroup,
		pasymmetricKeyDB->algorithmID,
		pasymmetricKeyDB->keyType,
		pasymmetricKeyDB->pkExponent,
		pasymmetricKeyDB->keyLen,
		pasymmetricKeyDB->inputFlag,
		pasymmetricKeyDB->outputFlag,
		pasymmetricKeyDB->effectiveDays,
		pasymmetricKeyDB->status,
		pasymmetricKeyDB->vkStoreLocation,
		pasymmetricKeyDB->hsmGroupID,
		pasymmetricKeyDB->vkIndex,
		pasymmetricKeyDB->oldVersionKeyIsUsed,
		pasymmetricKeyDB->creatorType,
		pasymmetricKeyDB->creator,
		pasymmetricKeyDB->createTime,
		pasymmetricKeyDB->usingUnit,
		pasymmetricKeyDB->remark);
	
	if ((ret = UnionGetSqlForAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDBWithoutSqlSyn:: UnionGetSqlForAsymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}

	if ((ret = UnionExecRealDBSql2(0, sql)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDBWithoutSqlSyn:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	
	// 登录非对称密钥更新日志
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB);
	
	return(0);
}

/* 
功能：	删除一个非对称密钥容器
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionDropAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	
	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	// 高速缓存中删除
	if ((ret = UnionDeleteHighCachedValue(highCachedKey)) < 0)
	{
		UnionUserErrLog("in UnionDropAsymmetricKeyDB:: UnionDeleteHighCachedValue[%s] error!\n",highCachedKey);
		return(ret);
	}
	
	len = snprintf(sql,sizeof(sql),"delete from %s where keyName = '%s';",defTableNameOfAsymmetricKeyDB,pasymmetricKeyDB->keyName);

	if ((ret = UnionGetSqlForAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationDelete,pasymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionDropAsymmetricKeyDB:: UnionGetSqlForAsymmetricKeyDBJnl!\n");
		return(ret);
	}

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDropAsymmetricKeyDB:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 登录非对称密钥更新日志
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationDelete,pasymmetricKeyDB);

	return(0);
}

/* 
功能：	把非对称密钥数据结构转换成字符串
参数：	pasymmetricKeyDB[in]	密钥容器信息
	keyDBStr[out]		密钥字符串
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionAsymmetricKeyDBStructToString(PUnionAsymmetricKeyDB pasymmetricKeyDB,char *keyDBStr)
{
	int	len = 0;
	
	TUnionAsymmetricKeyDBStr asymmetricKeyDBStr;
	
	if ((keyDBStr == NULL) || (pasymmetricKeyDB == NULL))
	{
		UnionUserErrLog("in UnionAsymmetricKeyDBStructToString:: keyDBStr or pasymmetricKeyDB is null!\n");
		return(errCodeParameter);
	}

	if(UnionIsAsymmetricKeyDBRecInCachedOnStructFormat()) // cached 直接保存struct数据
	{
		len = sizeof(TUnionAsymmetricKeyDB);
		memcpy(keyDBStr, (char *)pasymmetricKeyDB, len);
	}
	else	// cached 保存字符流数据
	{
		len = sizeof(TUnionAsymmetricKeyDBStr);
		UnionAsymmetricKeyDBStructToStringStruct(&asymmetricKeyDBStr, pasymmetricKeyDB);
		memcpy(keyDBStr, (char *)&asymmetricKeyDBStr, len);
	}
	return(len);
}

/* 
功能：	把非对称密钥字符串转换成数据结构
参数：	keyDBStr[in]		密钥字符串
	pasymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionAsymmetricKeyDBStringToStruct(char *keyDBStr,PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	len;
	TUnionAsymmetricKeyDBStr asymmetricKeyDBStr;
	
	if ((keyDBStr == NULL) || (pasymmetricKeyDB == NULL))
	{
		UnionUserErrLog("in UnionAsymmetricKeyDBStringToStruct:: keyDBStr or pasymmetricKeyDB is null!\n");
		return(errCodeParameter);
	}

	if(UnionIsAsymmetricKeyDBRecInCachedOnStructFormat()) // cached 直接保存struct数据
	{
		len = sizeof(TUnionAsymmetricKeyDB);
		memcpy(pasymmetricKeyDB, keyDBStr, len);
	}
	else    // cached 保存字符流数据
        {
                len = sizeof(TUnionAsymmetricKeyDBStr);
                memcpy(&asymmetricKeyDBStr, keyDBStr, len);
                UnionAsymmetricKeyDBStringSturctToStruct(pasymmetricKeyDB, &asymmetricKeyDBStr);
        }
	return(0);
}

/* 
功能：	读取一个非对称密钥容器
参数：	keyName[in]		密钥名称
	isCheckKey[in]		是否检查密钥的可用性，1检查，0不检查		
	pasymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadAsymmetricKeyDBRec(char *keyName,int isCheckKey,PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int		ret;
	int		len;
	int		isUseHighCached;
	char		sql[1024];
	char		tmpBuf[128];
	char		highCachedKey[256];
	char		keyDBStr[8192];
	char		tmpKeyName[136];

	UnionInitASymmetricKeyDB(pasymmetricKeyDB);
	
	snprintf(tmpKeyName,sizeof(tmpKeyName),"%s",keyName);
	snprintf(pasymmetricKeyDB->keyName,sizeof(pasymmetricKeyDB->keyName),"%s",tmpKeyName);

	// 转换密钥名称
	if ((ret = UnionTransformAsymmetricKeyName(pasymmetricKeyDB->keyName,sizeof(pasymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionTransformAsymmetricKeyName[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}

	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		isUseHighCached = 0;
	else
	{
		isUseHighCached = 1;	
		len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
		highCachedKey[len] = 0;
	}
	
	if (!isUseHighCached ||
		((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr,sizeof(keyDBStr))) <= 0))
	{
		// 设置通用查询不使用高速缓存
		if (isUseHighCached)
			UnionSetIsNotUseHighCached();
	
		// 从非对称密钥库表中读取
		len = sprintf(sql,"select * from %s where keyName = '%s'",defTableNameOfAsymmetricKeyDB,pasymmetricKeyDB->keyName);
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionLog("in UnionReadAsymmetricKeyDBRec:: keyName[%s] not find!\n",keyName);
			return(errCodeKeyCacheMDL_WrongKeyName);
		}

		UnionLocateXMLPackage("detail", 1);		
		
		if ((ret = UnionReadXMLPackageValue("keyGroup", pasymmetricKeyDB->keyGroup, sizeof(pasymmetricKeyDB->keyGroup))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyGroup");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","algorithmID");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->algorithmID = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("keyType", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyType");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->keyType = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("pkExponent", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","pkExponent");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->pkExponent = atoi(tmpBuf);

		if ((ret = UnionReadXMLPackageValue("keyLen", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyLen");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->keyLen = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("inputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","inputFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->inputFlag = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("outputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","outputFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->outputFlag = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("effectiveDays", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","effectiveDays");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->effectiveDays = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","status");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->status = atoi(tmpBuf);

		if ((ret = UnionReadXMLPackageValue("vkStoreLocation", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","vkStoreLocation");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->vkStoreLocation = atoi(tmpBuf);

		if ((ret = UnionReadXMLPackageValue("oldVersionKeyIsUsed", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldVersionKeyIsUsed");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->oldVersionKeyIsUsed = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("pkValue", pasymmetricKeyDB->pkValue, sizeof(pasymmetricKeyDB->pkValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","pkValue");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("pkCheckValue", pasymmetricKeyDB->pkCheckValue, sizeof(pasymmetricKeyDB->pkCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","pkCheckValue");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("vkValue", pasymmetricKeyDB->vkValue, sizeof(pasymmetricKeyDB->vkValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","vkValue");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("vkCheckValue", pasymmetricKeyDB->vkCheckValue, sizeof(pasymmetricKeyDB->vkCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","vkCheckValue");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("oldPKValue", pasymmetricKeyDB->oldPKValue, sizeof(pasymmetricKeyDB->oldPKValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldPKValue");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("oldPKCheckValue", pasymmetricKeyDB->oldPKCheckValue, sizeof(pasymmetricKeyDB->oldPKCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldPKCheckValue");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("oldVKValue", pasymmetricKeyDB->oldVKValue, sizeof(pasymmetricKeyDB->oldVKValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldVKValue");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("oldVKCheckValue", pasymmetricKeyDB->oldVKCheckValue, sizeof(pasymmetricKeyDB->oldVKCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","oldVKCheckValue");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("activeDate", pasymmetricKeyDB->activeDate, sizeof(pasymmetricKeyDB->activeDate))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","activeDate");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("keyUpdateTime", pasymmetricKeyDB->keyUpdateTime, sizeof(pasymmetricKeyDB->keyUpdateTime))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyUpdateTime");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("keyApplyPlatform", pasymmetricKeyDB->keyApplyPlatform, sizeof(pasymmetricKeyDB->keyApplyPlatform))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyApplyPlatform");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("keyDistributePlatform", pasymmetricKeyDB->keyDistributePlatform, sizeof(pasymmetricKeyDB->keyDistributePlatform))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","keyDistributePlatform");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("creatorType", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","creatorType");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pasymmetricKeyDB->creatorType = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("hsmGroupID", pasymmetricKeyDB->hsmGroupID, sizeof(pasymmetricKeyDB->hsmGroupID))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("vkIndex", pasymmetricKeyDB->vkIndex, sizeof(pasymmetricKeyDB->vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","vkIndex");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("creator", pasymmetricKeyDB->creator, sizeof(pasymmetricKeyDB->creator))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","creator");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("createTime", pasymmetricKeyDB->createTime, sizeof(pasymmetricKeyDB->createTime))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","createTime");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("createTime", pasymmetricKeyDB->createTime, sizeof(pasymmetricKeyDB->createTime))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","createTime");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("createTime", pasymmetricKeyDB->createTime, sizeof(pasymmetricKeyDB->createTime))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","createTime");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("usingUnit", pasymmetricKeyDB->usingUnit, sizeof(pasymmetricKeyDB->usingUnit))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","usingUnit");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("remark", pasymmetricKeyDB->remark, sizeof(pasymmetricKeyDB->remark))) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionReadXMLPackageValue[%s]!\n","remark");
			return(ret);
		}

		if (isUseHighCached)
		{
			if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
			{
				UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionAsymmetricKeyDBStructToString keyDBStr[%s]!\n",keyDBStr); 
				return(len);
			}
			keyDBStr[len] = 0;
			
			// 保存到高速缓存
			if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
				UnionProgramerLog("in UnionReadAsymmetricKeyDBRec:: UnionSetHighCachedValue error!\n");
			else if (ret > 0)
				UnionProgramerLog("in UnionReadAsymmetricKeyDBRec:: highCachedKey[%s] keyDBStr[%s]\n",highCachedKey,keyDBStr);
		}
	}
	else
	{
		UnionLog("in UnionReadAsymmetricKeyDBRec:: keyDBStr[%s] highCachedKey[%s]!\n",keyDBStr,highCachedKey);
		if ((ret = UnionAsymmetricKeyDBStringToStruct(keyDBStr,pasymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionAsymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr); 
			return(ret);
		}
		UnionLog("in UnionReadAsymmetricKeyDBRec:: keyName[%s] keyDBStr[%s] highCached!\n",keyName,keyDBStr);
	}
	
	if (isCheckKey)
	{
		// 检查密钥使用权限
		if ((ret = UnionCheckAsymmetricKeyPrivilege(pasymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionCheckAsymmetricKeyPrivilege keyName[%s]!\n",pasymmetricKeyDB->keyName);
			return(ret);
		}

		// 检查密钥有效性
		if ((ret = UnionCheckAsymmetricKeyStatus(pasymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionCheckAsymmetricKeyStatus keyName[%s]!\n",keyName);
			return(ret);
		}
	}
	return(0);
}

/* 
功能：	生成非对称密钥
参数：	pasymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGenerateAsymmetricKeyDBRec(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[8192*2];
	
	// 转换密钥名称
	if ((ret = UnionTransformAsymmetricKeyName(pasymmetricKeyDB->keyName,sizeof(pasymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAsymmetricKeyDBRec:: UnionTransformAsymmetricKeyName[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}

	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	UnionGetFullSystemDateTime(pasymmetricKeyDB->createTime);
	pasymmetricKeyDB->createTime[14] = 0;
	snprintf(pasymmetricKeyDB->keyUpdateTime,sizeof(pasymmetricKeyDB->keyUpdateTime),"%s",pasymmetricKeyDB->createTime);

	// 保存到高速缓存
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAsymmetricKeyDBRec:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;

	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionGenerateAsymmetricKeyDBRec:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	len = snprintf(sql,sizeof(sql),"insert into %s(keyName,keyGroup,algorithmID,keyType,pkExponent,keyLen,inputFlag,outputFlag,effectiveDays,status,vkStoreLocation,hsmGroupID,vkIndex,oldVersionKeyIsUsed,pkValue,pkCheckValue,vkValue,vkCheckValue,activeDate,keyUpdateTime,keyApplyPlatform,keyDistributePlatform,creatorType,creator,createTime,usingUnit,remark)"
		"values('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s',%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s','%s','%s','%s');",
		defTableNameOfAsymmetricKeyDB,
		pasymmetricKeyDB->keyName,
		pasymmetricKeyDB->keyGroup,
		pasymmetricKeyDB->algorithmID,
		pasymmetricKeyDB->keyType,
		pasymmetricKeyDB->pkExponent,
		pasymmetricKeyDB->keyLen,
		pasymmetricKeyDB->inputFlag,
		pasymmetricKeyDB->outputFlag,
		pasymmetricKeyDB->effectiveDays,
		pasymmetricKeyDB->status,
		pasymmetricKeyDB->vkStoreLocation,
		pasymmetricKeyDB->hsmGroupID,
		pasymmetricKeyDB->vkIndex,
		pasymmetricKeyDB->oldVersionKeyIsUsed,
		pasymmetricKeyDB->pkValue,
		pasymmetricKeyDB->pkCheckValue,
		pasymmetricKeyDB->vkValue,
		pasymmetricKeyDB->vkCheckValue,
		pasymmetricKeyDB->activeDate,
		pasymmetricKeyDB->keyUpdateTime,
		pasymmetricKeyDB->keyApplyPlatform,
		pasymmetricKeyDB->keyDistributePlatform,
		pasymmetricKeyDB->creatorType,
		pasymmetricKeyDB->creator,
		pasymmetricKeyDB->createTime,
		pasymmetricKeyDB->usingUnit,
		pasymmetricKeyDB->remark);

	if ((ret = UnionGetSqlForAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAsymmetricKeyDBRec:: UnionGetSqlForAsymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAsymmetricKeyDBRec:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}

	// 登录非对称密钥更新日志
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB);

	return(0);
}

/* 
功能：	更新非对称密钥
参数：	pasymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateAsymmetricKeyDBKeyValue(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[512];
	char	keyDBStr[8192*2];
	int	status = 0;

	int     tmp;
	char		keyDBStr_backup[8192*2];//add by lusj 20151222
	PUnionAsymmetricKeyDB pasymmetricKeyDB_backup=NULL;//add by lusj 20151222

		
	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	//add begin by lusj 201512122 从memcached获取密钥信息
	pasymmetricKeyDB_backup = (PUnionAsymmetricKeyDB)malloc(sizeof(TUnionAsymmetricKeyDB));

	if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr_backup,sizeof(keyDBStr_backup))) <= 0)
		UnionLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionGetHighCachedValue failed !\n");

	if ((ret = UnionAsymmetricKeyDBStringToStruct(keyDBStr_backup,pasymmetricKeyDB_backup)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionAsymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr);
		return(ret);
	}
	
	//UnionLog("in UnionUpdateAsymmetricKeyDBKeyValue:: key[%s] Old_PKvalue[%d][%s] !\n",pasymmetricKeyDB_backup->keyName,(int)strlen(pasymmetricKeyDB_backup->pkValue),pasymmetricKeyDB_backup->pkValue);

	//add end  by lusj 201512122
	

	UnionGetFullSystemDateTime(pasymmetricKeyDB->keyUpdateTime);
	pasymmetricKeyDB->keyUpdateTime[14] = 0;	

	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// 初始化状态
	{
		status = conAsymmetricKeyStatusOfEnabled;	// leipp modify  20141106
		memcpy(pasymmetricKeyDB->activeDate,pasymmetricKeyDB->keyUpdateTime,8);
		pasymmetricKeyDB->activeDate[8] = 0;
	}
	
	// 保存到高速缓存
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// 初始化状态
	{
		len = snprintf(sql,sizeof(sql),"update %s set pkValue = '%s',pkCheckValue = '%s',vkValue = '%s',vkCheckValue = '%s',oldPKValue = '%s',oldPKCheckValue = '%s',oldVKValue = '%s',oldVKCheckValue = '%s',"
			"keyUpdateTime = '%s',status = %d,activeDate = '%s' where keyName = '%s';",
			defTableNameOfAsymmetricKeyDB,
			pasymmetricKeyDB->pkValue,
			pasymmetricKeyDB->pkCheckValue,
			pasymmetricKeyDB->vkValue,
			pasymmetricKeyDB->vkCheckValue,
			pasymmetricKeyDB->oldPKValue,
			pasymmetricKeyDB->oldPKCheckValue,
			pasymmetricKeyDB->oldVKValue,
			pasymmetricKeyDB->oldVKCheckValue,
			pasymmetricKeyDB->keyUpdateTime,
			status,
			pasymmetricKeyDB->activeDate,
			pasymmetricKeyDB->keyName);
	}
	else
	{	
		// modify by zhouxw 20160720
		// 密钥有效天数在密钥值更新的同时允许修改
		len = snprintf(sql,sizeof(sql),"update %s set pkValue = '%s',pkCheckValue = '%s',vkValue = '%s',vkCheckValue = '%s',oldPKValue = '%s',oldPKCheckValue = '%s',oldVKValue = '%s',oldVKCheckValue = '%s',"
			"keyUpdateTime = '%s',activeDate = '%s', effectiveDays = %d where keyName = '%s';",
			defTableNameOfAsymmetricKeyDB,
			pasymmetricKeyDB->pkValue,
			pasymmetricKeyDB->pkCheckValue,
			pasymmetricKeyDB->vkValue,
			pasymmetricKeyDB->vkCheckValue,
			pasymmetricKeyDB->oldPKValue,
			pasymmetricKeyDB->oldPKCheckValue,
			pasymmetricKeyDB->oldVKValue,
			pasymmetricKeyDB->oldVKCheckValue,
			pasymmetricKeyDB->keyUpdateTime,
			pasymmetricKeyDB->activeDate,
			pasymmetricKeyDB->effectiveDays,
			pasymmetricKeyDB->keyName);
	}
	if ((ret = UnionGetSqlForAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationUpdate,pasymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionGetSqlForAsymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		//UnionDeleteHighCachedValue(highCachedKey);
	
		// modify begin  by lusj  20151222	数据库更新失败根据情况进行memcached操作
		if(strlen(pasymmetricKeyDB_backup->pkValue) > 0)
		{	//原有的memcached存在密钥，则还原之前的密钥值
			if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB_backup,keyDBStr_backup)) < 0)
			{
				UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionAsymmetricKeyDBStructToString!\n");
				return(len);
			}
			keyDBStr_backup[len] = 0;

			for(i = 0; i < 3; i++)
			{
				if ((tmp = UnionSetHighCachedValue(highCachedKey,keyDBStr_backup,len,0)) < 0)
					UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionSetHighCachedValue Of old_memcached_Value[%s] loop[%d] error!\n",highCachedKey,i+1);
				else
					break;
			}
		}
		else
			UnionDeleteHighCachedValue(highCachedKey);//原有的memcached不存在密钥值，则del操作
		// modify end  by lusj	20151222

		free(pasymmetricKeyDB_backup);	
		
		return(ret);
	}

	// 登录非对称密钥更新日志
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationUpdate,pasymmetricKeyDB);
	free(pasymmetricKeyDB_backup);
	return(0);
}

/* 
功能：	更新非对称密钥, 不同步sql语句
参数：	pasymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[512];
	char	keyDBStr[8192*2];
	int	status = 0;

	int     tmp;
	char		keyDBStr_backup[8192*2];//add by lusj 20151222
	PUnionAsymmetricKeyDB pasymmetricKeyDB_backup=NULL;//add by lusj 20151222

		
	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;

	//add begin by lusj 201512122 从memcached获取密钥信息
	pasymmetricKeyDB_backup = (PUnionAsymmetricKeyDB)malloc(sizeof(TUnionAsymmetricKeyDB));

	if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr_backup,sizeof(keyDBStr_backup))) <= 0)
		UnionLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionGetHighCachedValue failed !\n");

	if ((ret = UnionAsymmetricKeyDBStringToStruct(keyDBStr_backup,pasymmetricKeyDB_backup)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionAsymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr);
		return(ret);
	}
	
	//UnionLog("in UnionUpdateAsymmetricKeyDBKeyValue:: key[%s] Old_PKvalue[%d][%s] !\n",pasymmetricKeyDB_backup->keyName,(int)strlen(pasymmetricKeyDB_backup->pkValue),pasymmetricKeyDB_backup->pkValue);

	//add end  by lusj 201512122
	

	UnionGetFullSystemDateTime(pasymmetricKeyDB->keyUpdateTime);
	pasymmetricKeyDB->keyUpdateTime[14] = 0;	

	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// 初始化状态
	{
		status = conAsymmetricKeyStatusOfEnabled;	// leipp modify  20141106
		memcpy(pasymmetricKeyDB->activeDate,pasymmetricKeyDB->keyUpdateTime,8);
		pasymmetricKeyDB->activeDate[8] = 0;
	}
	
	// 保存到高速缓存
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// 尝试3次
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// 初始化状态
	{
		len = snprintf(sql,sizeof(sql),"update %s set pkValue = '%s',pkCheckValue = '%s',vkValue = '%s',vkCheckValue = '%s',oldPKValue = '%s',oldPKCheckValue = '%s',oldVKValue = '%s',oldVKCheckValue = '%s',"
			"keyUpdateTime = '%s',status = %d,activeDate = '%s' where keyName = '%s';",
			defTableNameOfAsymmetricKeyDB,
			pasymmetricKeyDB->pkValue,
			pasymmetricKeyDB->pkCheckValue,
			pasymmetricKeyDB->vkValue,
			pasymmetricKeyDB->vkCheckValue,
			pasymmetricKeyDB->oldPKValue,
			pasymmetricKeyDB->oldPKCheckValue,
			pasymmetricKeyDB->oldVKValue,
			pasymmetricKeyDB->oldVKCheckValue,
			pasymmetricKeyDB->keyUpdateTime,
			status,
			pasymmetricKeyDB->activeDate,
			pasymmetricKeyDB->keyName);
	}
	else
	{	
		len = snprintf(sql,sizeof(sql),"update %s set pkValue = '%s',pkCheckValue = '%s',vkValue = '%s',vkCheckValue = '%s',oldPKValue = '%s',oldPKCheckValue = '%s',oldVKValue = '%s',oldVKCheckValue = '%s',"
			"keyUpdateTime = '%s',activeDate = '%s' where keyName = '%s';",
			defTableNameOfAsymmetricKeyDB,
			pasymmetricKeyDB->pkValue,
			pasymmetricKeyDB->pkCheckValue,
			pasymmetricKeyDB->vkValue,
			pasymmetricKeyDB->vkCheckValue,
			pasymmetricKeyDB->oldPKValue,
			pasymmetricKeyDB->oldPKCheckValue,
			pasymmetricKeyDB->oldVKValue,
			pasymmetricKeyDB->oldVKCheckValue,
			pasymmetricKeyDB->keyUpdateTime,
			pasymmetricKeyDB->activeDate,
			pasymmetricKeyDB->keyName);
	}
	if ((ret = UnionGetSqlForAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationUpdate,pasymmetricKeyDB,sql + len,sizeof(sql) - len)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionGetSqlForAsymmetricKeyDBJnl!\n");
		UnionDeleteHighCachedValue(highCachedKey);
		return(ret);
	}
	if ((ret = UnionExecRealDBSql2(0, sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		//UnionDeleteHighCachedValue(highCachedKey);
	
		// modify begin  by lusj  20151222	数据库更新失败根据情况进行memcached操作
		if(strlen(pasymmetricKeyDB_backup->pkValue) > 0)
		{	//原有的memcached存在密钥，则还原之前的密钥值
			if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB_backup,keyDBStr_backup)) < 0)
			{
				UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionAsymmetricKeyDBStructToString!\n");
				return(len);
			}
			keyDBStr_backup[len] = 0;

			for(i = 0; i < 3; i++)
			{
				if ((tmp = UnionSetHighCachedValue(highCachedKey,keyDBStr_backup,len,0)) < 0)
					UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSetHighCachedValue Of old_memcached_Value[%s] loop[%d] error!\n",highCachedKey,i+1);
				else
					break;
			}
		}
		else
			UnionDeleteHighCachedValue(highCachedKey);//原有的memcached不存在密钥值，则del操作
		// modify end  by lusj	20151222

		free(pasymmetricKeyDB_backup);	
		
		return(ret);
	}

	// 登录非对称密钥更新日志
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationUpdate,pasymmetricKeyDB);
	free(pasymmetricKeyDB_backup);
	return(0);
}

/* 
功能：	检查非对称密钥状态
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckAsymmetricKeyStatus(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	ret;
	char	systemDate[32];
	char	effectiveDate[32];
	char	keyUpdateDate[32];
	
	if (pasymmetricKeyDB->status != conAsymmetricKeyStatusOfEnabled)
	{
		UnionUserErrLog("in UnionCheckAsymmetricKeyStatus:: status[%d] != [%d]!\n",pasymmetricKeyDB->status,conAsymmetricKeyStatusOfEnabled);
		UnionSetResponseRemark("密钥[%s]未启用",pasymmetricKeyDB->keyName);
		return(errCodeEsscMDL_KeyStatusDisabled);
	}
	
	// 检查密钥生效日期
	UnionGetFullSystemDate(systemDate);
	systemDate[8] = 0;
	if ((strlen(pasymmetricKeyDB->activeDate) == 0) || (strcmp(pasymmetricKeyDB->activeDate,systemDate) > 0))
	{
		UnionUserErrLog("in UnionCheckAsymmetricKeyStatus:: 生效日期[%s] > 系统日期[%s]!\n",pasymmetricKeyDB->activeDate,systemDate);
		UnionSetResponseRemark("密钥[%s]未生效",pasymmetricKeyDB->keyName);
		return(errCodeEsscMDL_KeyStatusNotActive);
	}

	// 检查有效时间
	if (pasymmetricKeyDB->effectiveDays > 0)
	{
		// 计算失效日期
		// modify by liwj 20150706
		memcpy(keyUpdateDate,pasymmetricKeyDB->keyUpdateTime,8);
		keyUpdateDate[8] = 0;  
		//if ((ret = UnionDecideDateAfterSpecDate(pasymmetricKeyDB->activeDate,pasymmetricKeyDB->effectiveDays,effectiveDate)) < 0)
		if ((ret = UnionDecideDateAfterSpecDate(keyUpdateDate,pasymmetricKeyDB->effectiveDays,effectiveDate)) < 0)
		{
			//UnionUserErrLog("in UnionCheckAsymmetricKeyStatus:: UnionDecideDateAfterSpecDate[%s]!\n",pasymmetricKeyDB->activeDate);
			UnionUserErrLog("in UnionCheckAsymmetricKeyStatus:: UnionDecideDateAfterSpecDate[%s]!\n",keyUpdateDate);
			return(ret);
		}
		// end
		effectiveDate[8] = 0;
		if (strcmp(effectiveDate,systemDate) < 0)
		{
			UnionUserErrLog("in UnionCheckAsymmetricKeyStatus:: 密钥[%s]已失效,生效日期[%s],有效天数[%d],失效日期[%s],当前日期[%s]!\n",pasymmetricKeyDB->keyName,pasymmetricKeyDB->activeDate,pasymmetricKeyDB->effectiveDays,effectiveDate,systemDate);
			UnionSetResponseRemark("密钥[%s]已失效,生效日期[%s],有效天数[%d]",pasymmetricKeyDB->keyName,pasymmetricKeyDB->activeDate,pasymmetricKeyDB->effectiveDays);
			return(errCodeEsscMDL_KeyStatusNotActive);
		}
	}
	return(0);
}

/*
功能：	检查非对称密钥使用权限
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckAsymmetricKeyPrivilege(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	ret;
	char	unitID[32];
	char	*ptr = NULL;

	if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0) 
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",unitID,sizeof(unitID))) < 0)
		{
			UnionUserErrLog("in UnionCheckAsymmetricKeyPrivilege:: UnionReadRequestXMLPackageValue[%s]!\n","head/unitID");
			return(ret);
		}
		if ((ptr = strchr(unitID,':')) == NULL)
		{
			UnionUserErrLog("in UnionCheckAsymmetricKeyPrivilege:: sysID[%s] error!\n",unitID);
			UnionSetResponseRemark("系统ID[%s]格式错误",unitID);
			return(errCodeParameter);
		}
		ptr += 1;
		snprintf(unitID,sizeof(unitID),"%s",ptr);

		if (strcmp(unitID,pasymmetricKeyDB->usingUnit) != 0)
		{
			UnionUserErrLog("in UnionCheckAsymmetricKeyPrivilege:: 请求单位ID[%s] != 密钥的使用单位[%s]\n",unitID,pasymmetricKeyDB->usingUnit);
			return(errCodeHsmCmdMDL_KeyNotPermitted);
		}
	}

	return 0;
}


/* 
功能：	拼接非对称密钥的创建者
参数：	sysID[in]	系统ID
	appID[in]	应用ID
	creator[out]	创建者
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionPieceAsymmetricKeyCreator(char *sysID,char *appID,char *creator)
{
	return(sprintf(creator,"%s:%s",sysID,appID));
}

int UnionAsymmetricKeyDBStructToStringStruct(PUnionAsymmetricKeyDBStr pasymmetricKeyDBStr, const PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int ret = 0;

	strcpy(pasymmetricKeyDBStr->keyName, 	pasymmetricKeyDB->keyName);
	strcpy(pasymmetricKeyDBStr->keyGroup, 	pasymmetricKeyDB->keyGroup);
	sprintf(pasymmetricKeyDBStr->algorithmID, "%d", pasymmetricKeyDB->algorithmID);
	sprintf(pasymmetricKeyDBStr->keyType, 	 "%d", pasymmetricKeyDB->keyType);
	sprintf(pasymmetricKeyDBStr->keyLen, 	 "%d", pasymmetricKeyDB->keyLen);
	sprintf(pasymmetricKeyDBStr->inputFlag,	 "%d", pasymmetricKeyDB->inputFlag);
	sprintf(pasymmetricKeyDBStr->outputFlag,	 "%d", pasymmetricKeyDB->outputFlag);
	sprintf(pasymmetricKeyDBStr->effectiveDays,	 "%d", pasymmetricKeyDB->effectiveDays);
	sprintf(pasymmetricKeyDBStr->status,	 "%d", pasymmetricKeyDB->status);
	sprintf(pasymmetricKeyDBStr->vkStoreLocation,	 "%d", pasymmetricKeyDB->vkStoreLocation);
	strcpy(pasymmetricKeyDBStr->hsmGroupID,	pasymmetricKeyDB->hsmGroupID);
	strcpy(pasymmetricKeyDBStr->vkIndex,	pasymmetricKeyDB->vkIndex);
	sprintf(pasymmetricKeyDBStr->oldVersionKeyIsUsed,	 "%d", pasymmetricKeyDB->oldVersionKeyIsUsed);
	strcpy(pasymmetricKeyDBStr->pkValue,	pasymmetricKeyDB->pkValue);
	strcpy(pasymmetricKeyDBStr->pkCheckValue,	pasymmetricKeyDB->pkCheckValue);
	strcpy(pasymmetricKeyDBStr->vkValue,	pasymmetricKeyDB->vkValue);
	strcpy(pasymmetricKeyDBStr->vkCheckValue,	pasymmetricKeyDB->vkCheckValue);
	strcpy(pasymmetricKeyDBStr->oldPKValue,	pasymmetricKeyDB->oldPKValue);
	strcpy(pasymmetricKeyDBStr->oldPKCheckValue,	pasymmetricKeyDB->oldPKCheckValue);
	strcpy(pasymmetricKeyDBStr->oldVKValue,	pasymmetricKeyDB->oldVKValue);
	strcpy(pasymmetricKeyDBStr->oldVKCheckValue,	pasymmetricKeyDB->oldVKCheckValue);
	strcpy(pasymmetricKeyDBStr->activeDate,	pasymmetricKeyDB->activeDate);
	strcpy(pasymmetricKeyDBStr->keyUpdateTime,	pasymmetricKeyDB->keyUpdateTime);
	strcpy(pasymmetricKeyDBStr->keyApplyPlatform,	pasymmetricKeyDB->keyApplyPlatform);
	strcpy(pasymmetricKeyDBStr->keyDistributePlatform, pasymmetricKeyDB->keyDistributePlatform);
	sprintf(pasymmetricKeyDBStr->creatorType,	"%d",	pasymmetricKeyDB->creatorType);
	strcpy(pasymmetricKeyDBStr->creator,		pasymmetricKeyDB->creator);
	strcpy(pasymmetricKeyDBStr->createTime,		pasymmetricKeyDB->createTime);
	strcpy(pasymmetricKeyDBStr->usingUnit,		pasymmetricKeyDB->usingUnit);
	strcpy(pasymmetricKeyDBStr->remark,		pasymmetricKeyDB->remark);

	return ret;
}

int UnionAsymmetricKeyDBStringSturctToStruct(PUnionAsymmetricKeyDB pasymmetricKeyDB, const  PUnionAsymmetricKeyDBStr pasymmetricKeyDBStr)
{
	int	ret = 0;

	strcpy(pasymmetricKeyDB->keyName, 	pasymmetricKeyDBStr->keyName);
	strcpy(pasymmetricKeyDB->keyGroup, 	pasymmetricKeyDBStr->keyGroup);
	pasymmetricKeyDB->algorithmID	 = atoi(pasymmetricKeyDBStr->algorithmID);
	pasymmetricKeyDB->keyType	 = atoi(pasymmetricKeyDBStr->keyType);
	pasymmetricKeyDB->keyLen	 	 = atoi(pasymmetricKeyDBStr->keyLen);
	pasymmetricKeyDB->pkExponent	 	 = atoi(pasymmetricKeyDBStr->pkExponent);
	pasymmetricKeyDB->inputFlag	 = atoi(pasymmetricKeyDBStr->inputFlag);
	pasymmetricKeyDB->outputFlag	 = atoi(pasymmetricKeyDBStr->outputFlag);
	pasymmetricKeyDB->effectiveDays	 = atoi(pasymmetricKeyDBStr->effectiveDays);
	pasymmetricKeyDB->status		= atoi(pasymmetricKeyDBStr->status);
	pasymmetricKeyDB->vkStoreLocation	= atoi(pasymmetricKeyDBStr->vkStoreLocation);
	strcpy(pasymmetricKeyDB->hsmGroupID,	pasymmetricKeyDBStr->hsmGroupID);
	strcpy(pasymmetricKeyDB->vkIndex,	pasymmetricKeyDBStr->vkIndex);
	pasymmetricKeyDB->oldVersionKeyIsUsed	 = atoi(pasymmetricKeyDBStr->oldVersionKeyIsUsed);

	strcpy(pasymmetricKeyDB->pkValue,	pasymmetricKeyDBStr->pkValue);
	strcpy(pasymmetricKeyDB->pkCheckValue,	pasymmetricKeyDBStr->pkCheckValue);
	strcpy(pasymmetricKeyDB->vkValue,	pasymmetricKeyDBStr->vkValue);
	strcpy(pasymmetricKeyDB->vkCheckValue,	pasymmetricKeyDBStr->vkCheckValue);

	strcpy(pasymmetricKeyDB->oldPKValue,	pasymmetricKeyDBStr->oldPKValue);
	strcpy(pasymmetricKeyDB->oldPKCheckValue,	pasymmetricKeyDBStr->oldPKCheckValue);
	strcpy(pasymmetricKeyDB->oldVKValue,	pasymmetricKeyDBStr->oldVKValue);
	strcpy(pasymmetricKeyDB->oldVKCheckValue,	pasymmetricKeyDBStr->oldVKCheckValue);

	strcpy(pasymmetricKeyDB->activeDate,	pasymmetricKeyDBStr->activeDate);
	strcpy(pasymmetricKeyDB->keyUpdateTime,	pasymmetricKeyDBStr->keyUpdateTime);
	strcpy(pasymmetricKeyDB->keyApplyPlatform,	pasymmetricKeyDBStr->keyApplyPlatform);
	strcpy(pasymmetricKeyDB->keyDistributePlatform, pasymmetricKeyDBStr->keyDistributePlatform);
	pasymmetricKeyDB->creatorType =  atoi(pasymmetricKeyDBStr->creatorType);
	strcpy(pasymmetricKeyDB->creator,		pasymmetricKeyDBStr->creator);
	strcpy(pasymmetricKeyDB->createTime,		pasymmetricKeyDBStr->createTime);
	strcpy(pasymmetricKeyDB->usingUnit,		pasymmetricKeyDBStr->usingUnit);
	strcpy(pasymmetricKeyDB->remark,		pasymmetricKeyDBStr->remark);
	return ret;
}

int UnionIsAsymmetricKeyDBRecInCachedOnStructFormat()
{
	int		ret;
	if ((ret = UnionReadIntTypeRECVar("isKeyDBRecInCachedOnStructFormat")) < 0)
	{
		return(1);
	}
	return(ret);
}

// 删除证书
int UnionDropCertOfCnaps2(char *keyName)
{
	int 	ret = 0;
	int	len = 0;
	char	sql[256];
	
	len = sprintf(sql,"delete from certOfCnaps2 where keyName = '%s'",keyName);
	sql[len] = 0;
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDropCertOfCnaps2:: UnionExecRealDBSql ret[%d]  sql[%s]!\n",ret,sql);	
		return(ret);
	}
	return 0;
}

// 初始化密钥结构体
void UnionInitASymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{

		
	pasymmetricKeyDB->hsmGroupID[0]=0;
	pasymmetricKeyDB->vkIndex[0]=0;                       
       	pasymmetricKeyDB->pkValue[0]=0;                       
        pasymmetricKeyDB->pkCheckValue[0]=0;                   
        pasymmetricKeyDB->vkValue[0]=0;                         
        pasymmetricKeyDB->vkCheckValue[0]=0;                   
        pasymmetricKeyDB->oldPKValue[0]=0;                     
       	pasymmetricKeyDB->oldPKCheckValue[0]=0;                   
       	pasymmetricKeyDB->oldVKValue[0]=0;                     
        pasymmetricKeyDB->oldVKCheckValue[0]=0;                 
        pasymmetricKeyDB->activeDate[0]=0;                       

        pasymmetricKeyDB->keyUpdateTime[0]=0;                  
        pasymmetricKeyDB->keyApplyPlatform[0]=0;                   
       	pasymmetricKeyDB->keyDistributePlatform[0]=0;               

        pasymmetricKeyDB->usingUnit[0]=0;                     
        pasymmetricKeyDB->remark[0]=0;                         


}
