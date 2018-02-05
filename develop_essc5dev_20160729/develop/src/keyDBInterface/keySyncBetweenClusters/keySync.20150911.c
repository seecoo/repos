//      Author:         zhouxw
//      Copyright:      Union Tech. Guangzhou
//      Date:           2015-09-11

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

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
#include "asymmetricKeyDB.h"

int UnionMsgAddIntField(char *msg, const char *fieldName, int fieldValue)
{
	char	tmpBuf[16];
	int	lenOfFieldValue = 0;
	int	lenOfFieldName = 0;
	char	strLenOfFieldValue[16];
	int	offset = 0;
	
	if(msg == NULL || fieldName == NULL)
	{
		UnionUserErrLog("in UnionMsgAddStrField:: NULL pointer\n");
		return(errCodeNullPointer);
	}
	
	lenOfFieldName = strlen(fieldName);
	memcpy(msg, fieldName, lenOfFieldName);
	offset += lenOfFieldName;
	snprintf(tmpBuf, sizeof(tmpBuf), "%d", fieldValue);
	lenOfFieldValue = strlen(tmpBuf);
	snprintf(strLenOfFieldValue, sizeof(strLenOfFieldValue), "%04d", lenOfFieldValue);
	memcpy(msg+offset, strLenOfFieldValue, 4);
	offset += 4;
	if(lenOfFieldValue == 0)
		return(offset);
	memcpy(msg+offset, tmpBuf, lenOfFieldValue);
	offset += lenOfFieldValue;
	return(offset);
}

int UnionMsgAddStrField(char *msg, const char *fieldName, const char *fieldValue)
{
	int	lenOfFieldName = 0;
	int	lenOfFieldValue = 0;
	int	offset = 0;
	char	strLenOfFieldValue[16];
	
	if(msg == NULL || fieldName == NULL || fieldValue == NULL)
	{
		UnionUserErrLog("in UnionMsgAddStrField:: NULL pointer\n");
		return(errCodeNullPointer);
	}
	
	lenOfFieldName = strlen(fieldName);
	memcpy(msg, fieldName, lenOfFieldName);
	offset += lenOfFieldName;
	lenOfFieldValue = strlen(fieldValue);
	snprintf(strLenOfFieldValue, sizeof(strLenOfFieldValue), "%04d", lenOfFieldValue);
	memcpy(msg+offset, strLenOfFieldValue, 4);
	offset += 4;
	if(lenOfFieldValue == 0)
		return(offset);
	memcpy(msg+offset, fieldValue, lenOfFieldValue);
	offset += lenOfFieldValue;
	return(offset);
}

int UnionFormMsgFromSymKeyStruct(PUnionSymmetricKeyDB p, char *msg)
{
	int		ret = 0;
	int		offset = 0;

	if(p == NULL || msg == NULL)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: Error parameter\n");
		return(errCodeNullPointer);
	}
	//keyName
	if((ret = UnionMsgAddStrField(msg+offset, "keyName", p->keyName)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyGroup
	if((ret = UnionMsgAddStrField(msg+offset, "keyGroup", p->keyGroup))< 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//algorithmID
	if((ret = UnionMsgAddIntField(msg+offset, "algorithmID", p->algorithmID)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyType
	if((ret = UnionMsgAddIntField(msg+offset, "keyType", p->keyType)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyLen
	if((ret = UnionMsgAddIntField(msg+offset, "keyLen", p->keyLen)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//inputFlag
	if((ret = UnionMsgAddIntField(msg+offset, "inputFlag", p->inputFlag)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//outputFlag
	if((ret = UnionMsgAddIntField(msg+offset, "outputFlag", p->outputFlag)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//effectiveDays
	if((ret = UnionMsgAddIntField(msg+offset, "effectiveDays", p->effectiveDays)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//status
	if((ret = UnionMsgAddIntField(msg+offset, "status", p->status)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldVersionKeyIsUsed
	if((ret = UnionMsgAddIntField(msg+offset, "oldVersionKeyIsUsed", p->oldVersionKeyIsUsed)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//activeDate
	if((ret = UnionMsgAddStrField(msg+offset, "activeDate", p->activeDate)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//checkValue
	if((ret = UnionMsgAddStrField(msg+offset, "checkValue", p->checkValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldCheckValue
	if((ret = UnionMsgAddStrField(msg+offset, "oldCheckValue", p->oldCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyUpdateTime
	if((ret = UnionMsgAddStrField(msg+offset, "keyUpdateTime", p->keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyApplyPlatform
	if((ret = UnionMsgAddStrField(msg+offset, "keyApplyPlatform", p->keyApplyPlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyDistributePlatform
	if((ret = UnionMsgAddStrField(msg+offset, "keyDistributePlatform", p->keyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//creatorType
	if((ret = UnionMsgAddIntField(msg+offset, "creatorType", p->creatorType)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//creator
	if((ret = UnionMsgAddStrField(msg+offset, "creator", p->creator)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//createTime
	if((ret = UnionMsgAddStrField(msg+offset, "createTime", p->createTime)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//usingUnit
	if((ret = UnionMsgAddStrField(msg+offset, "usingUnit", p->usingUnit)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//remark
	if((ret = UnionMsgAddStrField(msg+offset, "remark", p->remark)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//lmkProtectMode
	if((ret = UnionMsgAddStrField(msg+offset, "lmkProtectMode", p->keyValue[0].lmkProtectMode)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyValue
	if((ret = UnionMsgAddStrField(msg+offset, "keyValue", p->keyValue[0].keyValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldKeyValue
	if((ret = UnionMsgAddStrField(msg+offset, "oldKeyValue", p->keyValue[0].oldKeyValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromSymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	msg[offset] = 0;
	return(offset);
}

int UnionFormMsgFromAsymKeyStruct(PUnionAsymmetricKeyDB p, char *msg)
{
	int		ret = 0;
	int		offset = 0;

	if(p == NULL || msg == NULL)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: Error parameter\n");
		return(errCodeNullPointer);
	}
	//keyName
	if((ret = UnionMsgAddStrField(msg+offset, "keyName", p->keyName)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyGroup
	if((ret = UnionMsgAddStrField(msg+offset, "keyGroup", p->keyGroup))< 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//algorithmID
	if((ret = UnionMsgAddIntField(msg+offset, "algorithmID", p->algorithmID)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyType
	if((ret = UnionMsgAddIntField(msg+offset, "keyType", p->keyType)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyLen
	if((ret = UnionMsgAddIntField(msg+offset, "keyLen", p->keyLen)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//pkExponent
	if((ret = UnionMsgAddIntField(msg+offset, "pkExponent", p->pkExponent)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//inputFlag
	if((ret = UnionMsgAddIntField(msg+offset, "inputFlag", p->inputFlag)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//outputFlag
	if((ret = UnionMsgAddIntField(msg+offset, "outputFlag", p->outputFlag)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//effectiveDays
	if((ret = UnionMsgAddIntField(msg+offset, "effectiveDays", p->effectiveDays)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//status
	if((ret = UnionMsgAddIntField(msg+offset, "status", p->status)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//vkStoreLocation
	if((ret = UnionMsgAddIntField(msg+offset, "vkStoreLocation", p->vkStoreLocation)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//hsmGroupID
	if((ret = UnionMsgAddStrField(msg+offset, "hsmGroupID", p->hsmGroupID)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//vkIndex
	if((ret = UnionMsgAddStrField(msg+offset, "vkIndex", p->vkIndex)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldVersionKeyIsUsed
	if((ret = UnionMsgAddIntField(msg+offset, "oldVersionKeyIsUsed", p->oldVersionKeyIsUsed)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//pkValue
	if((ret = UnionMsgAddStrField(msg+offset, "pkValue", p->pkValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//pkCheckValue
	if((ret = UnionMsgAddStrField(msg+offset, "pkCheckValue", p->pkCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//vkValue
	if((ret = UnionMsgAddStrField(msg+offset, "vkValue", p->vkValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//vkCheckValue
	if((ret = UnionMsgAddStrField(msg+offset, "vkCheckValue", p->vkCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldPKValue
	if((ret = UnionMsgAddStrField(msg+offset, "oldPKValue", p->oldPKValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldPKCheckValue
	if((ret = UnionMsgAddStrField(msg+offset, "oldPKCheckValue", p->oldPKCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldVKValue
	if((ret = UnionMsgAddStrField(msg+offset, "oldVKValue", p->oldVKValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//oldVKCheckValue
	if((ret = UnionMsgAddStrField(msg+offset, "oldVKCheckValue", p->oldVKCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//activeDate
	if((ret = UnionMsgAddStrField(msg+offset, "activeDate", p->activeDate)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyUpdateTime
	if((ret = UnionMsgAddStrField(msg+offset, "keyUpdateTime", p->keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyApplyPlatform
	if((ret = UnionMsgAddStrField(msg+offset, "keyApplyPlatform", p->keyApplyPlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//keyDistributePlatform
	if((ret = UnionMsgAddStrField(msg+offset, "keyDistributePlatform", p->keyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//creatorType
	if((ret = UnionMsgAddIntField(msg+offset, "creatorType", p->creatorType)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddIntField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//creator
	if((ret = UnionMsgAddStrField(msg+offset, "creator", p->creator)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//createTime
	if((ret = UnionMsgAddStrField(msg+offset, "createTime", p->createTime)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//usingUnit
	if((ret = UnionMsgAddStrField(msg+offset, "usingUnit", p->usingUnit)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	//remark
	if((ret = UnionMsgAddStrField(msg+offset, "remark", p->remark)) < 0)
	{
		UnionUserErrLog("in UnionFormMsgFromAsymKeyStruct:: UnionMsgAddStrField ret = %d\n", ret);
		return(ret);
	}
	offset += ret;
	msg[offset] = 0;
	return(offset);
}

int UnionBackupFile(char *fileName, char *newFilePre)
{
	FILE		*fp = NULL;
	FILE		*fp2 = NULL;
	char		record[81920];
	char		newFileName[128];
	
	snprintf(newFileName, sizeof(newFileName), "%s_backup", newFilePre);
	
	if((fp = fopen(fileName, "r")) == NULL)
	{
		UnionUserErrLog("in UnionBackupFile:: fopen file[%s] error, errno[%d]\n", fileName, errno);
		return(errCodeUseOSErrCode);
	}

	if((fp2 = fopen(newFileName, "a")) == NULL)
	{
		UnionUserErrLog("in UnionBackupFile:: fopen file[%s] error, errno[%d]\n", newFileName, errno);
		return(errCodeUseOSErrCode);
	}
	
	while(!feof(fp))
	{
		if(fgets(record, sizeof(record), fp))
		{
			if(fputs(record, fp2) < 0)
			{
				UnionUserErrLog("in UnionBackupFile:: fputs err errno[%d]\n", errno);
				return(errCodeUseOSErrCode);
			}
		}
	}
	fclose(fp);
	fclose(fp2);
	return 0;
}

int UnionDealDaemonTask(char *parameter)
{
	int		ret = 0;
	char		msg[40960];
	int		lenOfMsg = 0;
	char		sysDate[16];
	
	//char		strDate[16];
	FILE		*fp = NULL;
	
	char		tmpFileName[128];
	char		record[81920];
	char		tmpRecord[81920];
	char		*ptr = NULL;
	char		keyName[128];
	char		fullFileName[128];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));

	// 设置临时文件名
        snprintf(tmpFileName, sizeof(tmpFileName), "%s/dataSync/dataSync_fail.tmp", getenv("UNIONREC"));

	UnionGetFullSystemDate(sysDate);
        sysDate[8] = 0;
	snprintf(fullFileName, sizeof(fullFileName), "%s/dataSync/dataSync_fail.%s.txt", getenv("UNIONREC"), sysDate);
	if(access(fullFileName, 0) == 0)
	{
		rename(fullFileName, tmpFileName);
		// 备份文件
		if((ret = UnionBackupFile(tmpFileName, fullFileName)) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionBackupFile error,ret[%d]\n", ret);
			return(ret);
		}
	}
	else
	{
		UnionGetDateBeforSpecDays(1, sysDate);
        	sysDate[8] = 0;
        	snprintf(fullFileName,sizeof(fullFileName),"%s/dataSync/dataSync_fail.%s.txt",getenv("UNIONREC"),sysDate);
		if(access(fullFileName, 0) == 0)
        	{
        	        rename(fullFileName, tmpFileName);
			if((ret = UnionBackupFile(tmpFileName, fullFileName)) < 0)
			{
				UnionUserErrLog("in UnionDealDaemonTask:: UnionBackupFile error,ret[%d]\n", ret);
				return(ret);
			}
        	}
		else
			return 0;
	}
	
	// 读取错误文件的信息，获取sql类消息的密钥表的密钥名，然后直接查询数据库获取最新密钥值到远程判断是否更新
	if((fp = fopen(tmpFileName, "r")) != NULL)
	{
		while(!feof(fp))
		{
			if(fgets(record, sizeof(record), fp) != NULL)
			{
				// 插入操作
				if((ptr = strstr(record, "00insert")) != NULL || (ptr = strstr(record, "01insert")) != NULL)
				{
					if((ptr = strstr(record, "symmetricKeyDB")) != NULL && (ptr = strstr(record, "values('")) != NULL)
					{
						strcpy(tmpRecord, ptr + 8);
						if((ptr = strchr(tmpRecord, '\'')) != NULL)
						{
							*ptr = 0;
							strcpy(keyName, tmpRecord);
						}
						strcpy(symmetricKeyDB.keyName, keyName);                                          
                                                if((ret = UnionReadSymmetricKeyDBRec(keyName, 0, &symmetricKeyDB)) >= 0)          
                                                {                                                                                 
                                                        memcpy(msg, "30symmetricKeyDB", 16);                                        
                                                        //拼装Msg                                                                 
                                                        if((lenOfMsg = UnionFormMsgFromSymKeyStruct(&symmetricKeyDB, msg+16)) < 0)
                                                        {                                                                         
                                                                UnionUserErrLog("in UnionDealDaemonTask:: UnionFormMsgFromSymKeyStruct[ret=%d]\n", lenOfMsg);
                                                                continue;                                                         
                                                        }                                                                         
                                                        lenOfMsg += 16;                                                           
                                                        msg[lenOfMsg] = 0;                                                        
                                                }                                                                                 
                                                else                                                                              
                                                        continue;
						
					}
					else if((ptr = strstr(record, "asymmetricKeyDB")) != NULL && (ptr = strstr(record, "values('")) != NULL)
					{
						strcpy(tmpRecord, ptr + 8);
						if((ptr = strchr(tmpRecord, '\'')) != NULL)
						{
							*ptr = 0;
							strcpy(keyName, tmpRecord);
						}
						strcpy(asymmetricKeyDB.keyName, keyName);                                         
                                                if((ret = UnionReadAsymmetricKeyDBRec(keyName, 0, &asymmetricKeyDB)) >= 0)        
                                                {                                                                                 
                                                        memcpy(msg, "30asymmetricKeyDB", 17);                                       
                                                        // 拼装Msg                                                                
                                                        if((lenOfMsg = UnionFormMsgFromAsymKeyStruct(&asymmetricKeyDB, msg+15)) < 0)
                                                        {                                                                         
                                                                UnionUserErrLog("in UnionDealDaemonTask:: UnionFormMsgFromAsymKeyStruct[ret=%d]\n",
lenOfMsg);
                                                                continue;                                                         
                                                        }                                                                         
                                                        lenOfMsg += 17;                                                           
                                                        msg[lenOfMsg] = 0;                                                        
                                                }                                                                                 
                                                else                                                                              
                                                        continue;
					}
					else                                                                                      
                                                continue;                                                                         
				}
				// 更新操作
				else if((ptr = strstr(record, "keyName = '")) != NULL)
				{
					strcpy(tmpRecord, ptr + 11);
					if((ptr = strchr(tmpRecord, '\'')) != NULL)
					{
						*ptr = 0;
						strcpy(keyName, tmpRecord);
					}
					if((ptr = strstr(record, "symmetricKeyDB")) != NULL)
					{
						strcpy(symmetricKeyDB.keyName, keyName);
						if((ret = UnionReadSymmetricKeyDBRec(keyName, 0, &symmetricKeyDB)) >= 0)
						{
							memcpy(msg, "30symmetricKeyDB", 16);
							//拼装Msg
							if((lenOfMsg = UnionFormMsgFromSymKeyStruct(&symmetricKeyDB, msg+16)) < 0)
							{
								UnionUserErrLog("in UnionDealDaemonTask:: UnionFormMsgFromSymKeyStruct[ret=%d]\n", lenOfMsg);
								continue;
							}
							lenOfMsg += 16;
							msg[lenOfMsg] = 0;
						}
						else
							continue;
					}
					else if((ptr = strstr(record, "asymmetricKeyDB")) != NULL)
					{
						strcpy(asymmetricKeyDB.keyName, keyName);
						if((ret = UnionReadAsymmetricKeyDBRec(keyName, 0, &asymmetricKeyDB)) >= 0)
						{
							memcpy(msg, "30asymmetricKeyDB", 17);
							// 拼装Msg
							if((lenOfMsg = UnionFormMsgFromAsymKeyStruct(&asymmetricKeyDB, msg+15)) < 0)
							{
								UnionUserErrLog("in UnionDealDaemonTask:: UnionFormMsgFromAsymKeyStruct[ret=%d]\n", lenOfMsg);	
								continue;
							}
							lenOfMsg += 17;
							msg[lenOfMsg] = 0;
						}
						else 
							continue;
					}
					else
						continue;	
				}
				else
					continue;
			}
			// 同步密钥信息
			UnionUserErrLog("msg: %s\n", msg);
			if((ret = UnionDataSyncSendToSocket(0, (unsigned char*)msg, lenOfMsg)) < 0)
			{
				UnionUserErrLog("in UnionDealDaemonTask:: UnionDataSyncSendToSocket msg[%s] ret[%d]\n", msg, ret);
				continue;
			}
		}
		if((ret = UnionDataSyncCloseSocket()) < 0)	
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionDataSyncCloseSocket ret[%d]\n", ret);
			return(ret);
		}
	}
	fclose(fp);
	
	return(0);
}
