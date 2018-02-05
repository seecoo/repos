//      Author:         zhouxw
//      Copyright:      Union Tech. Guangzhou
//      Date:           2015-09-11

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
#include "unionDataSync.h"
#include "unionRecFile.h"
#include "asymmetricKeyDB.h"

int UnionReadSpecFieldValueFromMsg(const char *msg, const char *fieldName, char *fieldValue)
{
	int	lenOfFieldName = 0;
	int	lenOfFieldValue = 0;
	char 	tmpBuf[16];
	char	*ptr = NULL;
	int	offset = 0;

	if(msg == NULL || fieldName == NULL || fieldValue == NULL || strlen(msg) == 0 || strlen(fieldName) == 0)
	{
		UnionUserErrLog("in UnionReadSpecFieldValueFromMsg:: wrong parameter\n");
		return(errCodeParameter);
	}
	
	while((ptr = strstr(msg + offset, fieldName)) != NULL)
	{
		lenOfFieldName = strlen(fieldName);
		memcpy(tmpBuf, ptr+lenOfFieldName, 4);
		tmpBuf[4] = 0;
		if(UnionIsDigitString(tmpBuf))
			break;
		offset += ptr - msg + 1 + lenOfFieldName;
	}
	
	lenOfFieldValue = atoi(tmpBuf);
	if(lenOfFieldValue == 0)
		fieldValue[0] = 0;
	else
	{
		memcpy(fieldValue, ptr+lenOfFieldName+4, lenOfFieldValue);
		fieldValue[lenOfFieldValue] = 0;
	}
	return(0);
}

int UnionFormSymmetricKeyFromMsg(PUnionSymmetricKeyDB p, char *msg)
{
	int		ret = 0;
	char		tmpBuf[16];
	
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyName", p->keyName)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyName", ret);
		return(ret);
	}
	
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyGroup", p->keyGroup)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyGroup", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "algorithmID", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "algorithmID", ret);
		return(ret);
	}
	p->algorithmID = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyType", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyType", ret);
		return(ret);
	}
	p->keyType = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyLen", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyLen", ret);
		return(ret);
	}
	p->keyLen = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "inputFlag", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "inputFlag", ret);
		return(ret);
	}
	p->inputFlag = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "outputFlag", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "outputFlag", ret);
		return(ret);
	}
	p->outputFlag = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "effectiveDays", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "effectiveDays", ret);
		return(ret);
	}
	p->effectiveDays = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "status", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "status", ret);
		return(ret);
	}
	p->status = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldVersionKeyIsUsed", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "oldVersionKeyIsUsed", ret);
		return(ret);
	}
	p->oldVersionKeyIsUsed = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "activeDate", p->activeDate)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "activeDate", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "checkValue", p->checkValue)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "checkValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldCheckValue", p->oldCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "oldCheckValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyUpdateTime", p->keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyUpdateTime", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyApplyPlatform", p->keyApplyPlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyApplyPlatform", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyDistributePlatform", p->keyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyDistributePlatform", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyUpdateTime", p->keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyUpdateTime", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "creatorType", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "creatorType", ret);
		return(ret);
	}
	p->creatorType = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "creator", p->creator)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "creator", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "createTime", p->createTime)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "createTime", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "usingUnit", p->usingUnit)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "usingUnit", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "remark", p->remark)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "remark", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "lmkProtectMode", p->keyValue[0].lmkProtectMode)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "lmkProtectMode", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyValue", p->keyValue[0].keyValue)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "keyValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldKeyValue", p->keyValue[0].oldKeyValue)) < 0)
	{
		UnionUserErrLog("in UnionFormSymmetricKeyFromMsg:: UnionFormSymmetricKeyFromMsg[%s], ret[%d]\n", "oldKeyValue", ret);
		return(ret);
	}
	return(0);
}

int UnionFormAsymmetricKeyFromMsg(PUnionAsymmetricKeyDB p, char *msg)
{
	int		ret = 0;
	char		tmpBuf[16];
	
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyName", p->keyName)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyName", ret);
		return(ret);
	}
	
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyGroup", p->keyGroup)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyGroup", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "algorithmID", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "algorithmID", ret);
		return(ret);
	}
	p->algorithmID = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyType", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyType", ret);
		return(ret);
	}
	p->keyType = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyLen", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyLen", ret);
		return(ret);
	}
	p->keyLen = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "pkExponent", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "pkExponent", ret);
		return(ret);
	}
	p->pkExponent = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "inputFlag", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "inputFlag", ret);
		return(ret);
	}
	p->inputFlag = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "outputFlag", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "outputFlag", ret);
		return(ret);
	}
	p->outputFlag = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "effectiveDays", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "effectiveDays", ret);
		return(ret);
	}
	p->effectiveDays = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "status", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "status", ret);
		return(ret);
	}
	p->status = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "vkStoreLocation", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "vkStoreLocation", ret);
		return(ret);
	}
	p->vkStoreLocation = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "hsmGroupID", p->hsmGroupID)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "hsmGroupID", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "vkIndex", p->vkIndex)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "vkIndex", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldVersionKeyIsUsed", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "oldVersionKeyIsUsed", ret);
		return(ret);
	}
	p->oldVersionKeyIsUsed = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "pkValue", p->pkValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "pkValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "pkCheckValue", p->pkCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "pkCheckValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "vkValue", p->vkValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "vkValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "vkCheckValue", p->vkCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "vkCheckValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldPKValue", p->oldPKValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "oldPKValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldPKCheckValue", p->oldPKCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "oldPKCheckValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldVKValue", p->oldVKValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "oldVKValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "oldVKCheckValue", p->oldVKCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "oldVKCheckValue", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "activeDate", p->activeDate)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "activeDate", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyUpdateTime", p->keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyUpdateTime", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyApplyPlatform", p->keyApplyPlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyApplyPlatform", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyDistributePlatform", p->keyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyDistributePlatform", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "keyUpdateTime", p->keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "keyUpdateTime", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "creatorType", tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "creatorType", ret);
		return(ret);
	}
	p->creatorType = atoi(tmpBuf);
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "creator", p->creator)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "creator", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "createTime", p->createTime)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "createTime", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "usingUnit", p->usingUnit)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "usingUnit", ret);
		return(ret);
	}
	if((ret = UnionReadSpecFieldValueFromMsg(msg, "remark", p->remark)) < 0)
	{
		UnionUserErrLog("in UnionFormAsymmetricKeyFromMsg:: UnionFormAsymmetricKeyFromMsg[%s], ret[%d]\n", "remark", ret);
		return(ret);
	}
	return(0);
}

int UnionUpdateKey(char *parameter)
{
	int		ret = 0;
	char		sql[512];
	int		len = 0;

	char		*keyMsg = NULL;

	TUnionSymmetricKeyDB	symmetricKeyDB;
	TUnionSymmetricKeyDB	localSymmetricKeyDB;
	TUnionAsymmetricKeyDB	asymmetricKeyDB;
	TUnionAsymmetricKeyDB	localASymmetricKeyDB;
	
	memset(&symmetricKeyDB, 0, sizeof(TUnionSymmetricKeyDB));
	memset(&localSymmetricKeyDB, 0, sizeof(TUnionSymmetricKeyDB));
	memset(&asymmetricKeyDB, 0, sizeof(TUnionAsymmetricKeyDB));
	memset(&localASymmetricKeyDB, 0, sizeof(TUnionAsymmetricKeyDB));

	if(memcmp(parameter, "symmetricKeyDB", 14) == 0)
	{
		keyMsg = parameter + 14;
		if((ret = UnionReadSpecFieldValueFromMsg(keyMsg, "keyName", symmetricKeyDB.keyName)) < 0)
		{
			UnionUserErrLog("in UnionUpdateKey:: UnionReadSpecFieldValueFromMsg[%s], ret[%d]\n", "keyName", ret);
			return(ret);
		}
		
		if((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName, 0, &localSymmetricKeyDB)) < 0)
		{
			//密钥不存在，增加密钥后结束
			if(ret == errCodeKeyCacheMDL_WrongKeyName)
			{
				if((ret = UnionFormSymmetricKeyFromMsg(&symmetricKeyDB, keyMsg)) < 0)
				{
					UnionUserErrLog("in UnionUpdateKey:: UnionFormSymmetricKeyFromMsg ret[%d]\n", ret);
					return(ret);
				}
				symmetricKeyDB.status = conSymmetricKeyStatusOfInitial;
				if((ret = UnionCreateSymmetricKeyDBWithoutSqlSyn(&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionUpdateKey:: UnionCreateSymmetricKeyDBWithoutSqlSyn keyName[%s] ret[%d]\n", symmetricKeyDB.keyName, ret);
					return(ret);
				}
				//根据lmkProtectMode是否存在选择更新密钥值
				if((symmetricKeyDB.keyValue[0].lmkProtectMode != NULL) && (strlen(symmetricKeyDB.keyValue[0].lmkProtectMode) > 0))
				{
					if((ret = UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn(&symmetricKeyDB)) < 0)
					{	
						UnionUserErrLog("in UnionUpdateKey:: UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn keyName[%s] ret[%d]\n", symmetricKeyDB.keyName, ret);
						return(ret);
					}
				}
			}
			else
			{
				UnionUserErrLog("in UnionUpdateKey:: UnionReadSymmetricKeyDBRec[%s] ret[%d]\n", symmetricKeyDB.keyName,ret);
				return(ret);
			}
		}	
		//密钥已存在
		else
		{
			//读取校验值
			if((ret = UnionReadSpecFieldValueFromMsg(keyMsg, "checkValue", symmetricKeyDB.checkValue)) < 0)
			{
				UnionUserErrLog("in UnionUpdateKey:: UnionReadSpecFieldValueFromMsg[%s] ret[%d]\n", "checkValue", ret);
				return(ret);
			}
			if(strcmp(symmetricKeyDB.checkValue, localSymmetricKeyDB.checkValue) != 0)
			{
				//读取密钥更新日期
				if((ret = UnionReadSpecFieldValueFromMsg(keyMsg, "keyUpdateTime", symmetricKeyDB.keyUpdateTime)) < 0)
				{
					UnionUserErrLog("in UnionUpdateKey:: UnionReadSpecFieldValueFromMsg[%s] ret[%d]\n", "keyUpdateTime", ret);
					return(ret);
				}
				if(strcmp(symmetricKeyDB.keyUpdateTime, localSymmetricKeyDB.keyUpdateTime) > 0)
				{
					if((ret = UnionFormSymmetricKeyFromMsg(&symmetricKeyDB, keyMsg)) < 0)
					{
						UnionUserErrLog("in UnionUpdateKey:: UnionFormSymmetricKeyFromMsg ret[%d]\n", ret);
						return(ret);
					}
					if((symmetricKeyDB.keyValue[0].lmkProtectMode != NULL) && (strlen(symmetricKeyDB.keyValue[0].lmkProtectMode) > 0))
					{
						//远程密钥的旧密钥值不存在
						if(strlen(symmetricKeyDB.keyValue[0].oldKeyValue) == 0)
						{
							len = snprintf(sql, sizeof(sql), "delete from %s where keyName = '%s';",
								defTableNameOfSymmetricKeyValue, symmetricKeyDB.keyName);
							if((ret = UnionGetSqlForSymmetricKeyDBJnl(conSymmetricKeyDBOperationDelete, &localSymmetricKeyDB, sql+len, sizeof(sql)-len)) < 0)
							{
								UnionUserErrLog("in UnionUpdateKey:: UnionGetSqlForSymmetricKeyDBJnl\n");
								return(ret);
							}
							if((ret = UnionExecRealDBSql2(0, sql)) < 0)
							{
								UnionUserErrLog("in UnionUpdateKey:: UnionExecRealDBSql[%s]\n", sql);
								return(ret);	
							}
						}
						//更新密钥
						if((ret = UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn(&symmetricKeyDB)) < 0)
						{
							UnionUserErrLog("in UnionUpdateKey:: UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn keyName[%s] ret[%d]\n", symmetricKeyDB.keyName, ret);
							return(ret);
						}
					}
				}
			}
		}
	}
	else if(memcmp(parameter, "asymmetricKeyDB", 15) == 0)
	{
		keyMsg = parameter + 15;
		if((ret = UnionReadSpecFieldValueFromMsg(keyMsg, "keyName", asymmetricKeyDB.keyName)) < 0)
		{
			UnionUserErrLog("in UnionUpdateKey:: UnionReadSpecFieldValueFromMsg[%s], ret[%d]\n", "keyName", ret);
			return(ret);
		}
	
		if((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 0, &localASymmetricKeyDB)) < 0)
		{
			//密钥不存在，增加密钥后结束
			if(ret == errCodeKeyCacheMDL_WrongKeyName)
			{
				if((ret = UnionFormAsymmetricKeyFromMsg(&asymmetricKeyDB, keyMsg)) < 0)
				{
					UnionUserErrLog("in UnionUpdateKey:: UnionFormAsymmetricKeyFromMsg ret[%d]\n", ret);
					return(ret);
				}
				asymmetricKeyDB.status = conAsymmetricKeyStatusOfInitial;
				if((ret = UnionCreateAsymmetricKeyDBWithoutSqlSyn(&asymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionUpdateKey:: UnionCreateAsymmetricKeyDBWithoutSqlSyn keyName[%s] ret[%d]\n", symmetricKeyDB.keyName, ret);
					return(ret);
				}
				// 更新密钥
				if((ret = UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn(&asymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionUpdateKey:: UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn keyName[%s] ret[%d]\n", asymmetricKeyDB.keyName, ret);
					return(ret);
				}
			}
			else
			{
				UnionUserErrLog("in UnionUpdateKey:: UnionReadAsymmetricKeyDBRec[%s] ret[%d]\n", asymmetricKeyDB.keyName,ret);
				return(ret);
			}
		}	
		//密钥已存在
		else
		{
			if((ret = UnionReadSpecFieldValueFromMsg(keyMsg, "pkValue", asymmetricKeyDB.pkValue)) < 0)
			{
				UnionUserErrLog("in UnionUpdateKey:: UnionReadSpecFieldValueFromMsg[%s] ret[%d]\n", "pkValue", ret);
				return(ret);
			}
			if(strcmp(asymmetricKeyDB.pkValue, localASymmetricKeyDB.pkValue) != 0)
			{
				//读取密钥更新日期
				if((ret = UnionReadSpecFieldValueFromMsg(keyMsg, "keyUpdateTime", asymmetricKeyDB.keyUpdateTime)) < 0)
				{
					UnionUserErrLog("in UnionUpdateKey:: UnionReadSpecFieldValueFromMsg[%s] ret[%d]\n", "keyUpdateTime", ret);
					return(ret);
				}
				if(strcmp(asymmetricKeyDB.keyUpdateTime, localASymmetricKeyDB.keyUpdateTime) > 0)
				{
					if((ret = UnionFormAsymmetricKeyFromMsg(&asymmetricKeyDB, keyMsg)) < 0)
					{
						UnionUserErrLog("in UnionUpdateKey:: UnionFormSymmetricKeyFromMsg ret[%d]\n", ret);
						return(ret);
					}
					//更新密钥
					if((ret = UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn(&asymmetricKeyDB)) < 0)
					{
						UnionUserErrLog("in UnionUpdateKey:: UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn keyName[%s] ret[%d]\n", asymmetricKeyDB.keyName, ret);
						return(ret);
					}
				}
			}
		}
	}

	return(0);
}
