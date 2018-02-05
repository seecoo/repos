//	Author:		������
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
���ܣ�	ת���ǶԳ���Կ����
������	keyName[in|out]		��Կ����
	sizeofBuf[in]		�����С
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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

	if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// ������
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
                                UnionSetResponseRemark("ϵͳID[%s]��ʽ����",unitID);
                                return(errCodeParameter);
                        }
                        ptr += 1;
                        snprintf(unitID,sizeof(unitID),"%s",ptr);
		}
		
		// ��Կ���ư����˵�λ���
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
���ܣ�	��ȡ�ǶԳ���Կ���ڸ��ٻ����м���
������	keyName[in]		��Կ����
	highCachedKey[out]	����
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetKeyOfHighCachedForAsymmetricKeyDB(char *keyName,char *highCachedKey)
{
	return(sprintf(highCachedKey,"%s:%s",defTableNameOfAsymmetricKeyDB,keyName));
}

// �жϾ���Կ�Ƿ�����ʹ��
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
	
	// ����Կ������ʹ��
	if (!pasymmetricKeyDB->oldVersionKeyIsUsed)
	{
		UnionUserErrLog("in UnionOldVersionAsymmetricKeyIsUsed:: [%s] oldVersionKeyIsUsed [%d]!\n", pasymmetricKeyDB->keyName, pasymmetricKeyDB->oldVersionKeyIsUsed);
		return(0);
	}
	
	// ת����Կ����ʱ��
	keyUpdateTime = UnionTranslateStringTimeToTime(pasymmetricKeyDB->keyUpdateTime);
	
	// ��ȡ��Կ������
	if ((windowPeriod = UnionReadIntTypeRECVar("windowPeriodOfAsymmetricKey")) <= 0)
	{
		UnionUserErrLog("in UnionOldVersionAsymmetricKeyIsUsed:: UnionReadIntTypeRECVar read [windowPeriodOfAsymmetricKey] [%d]\n", windowPeriod);
		windowPeriod = 0;
	}
	
	time(&now);
	UnionAuditLog("in UnionOldVersionAsymmetricKeyIsUsed:: [%s] snap [%d]\n", pasymmetricKeyDB->keyName, (int)(now - keyUpdateTime));
	// ����ʱ���
	if (((snap = now - keyUpdateTime) > 0) && (snap < windowPeriod))
		return(1);
	else
		return(0);
}

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ����
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
			UnionSetResponseRemark("��Կ���Ͳ���ȷ");
			return(0);
	}
}

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ����
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
			UnionSetResponseRemark("��Կ���Ȳ���ȷ");
			return(0);
	}
}

// ���㷨��ʶ����תΪ�ڲ�ö��ֵ
TUnionAsymmetricAlgorithmID UnionConvertAsymmetricKeyAlgorithmID(char *algorithmID)
{
	UnionToUpperCase(algorithmID);
	if (strcmp(algorithmID,"RSA") == 0)
		return(conAsymmetricAlgorithmIDOfRSA);
	if (strcmp(algorithmID,"SM2") == 0)
		return(conAsymmetricAlgorithmIDOfSM2);
	return(errCodeParameter);
}

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ�㷨
int UnionIsValidAsymmetricKeyAlgorithmID(TUnionAsymmetricAlgorithmID algorithmID)
{
	switch (algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:
		case	conAsymmetricAlgorithmIDOfSM2:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidAsymmetricKeyAlgorithmID:: algorithmID[%d]!\n",algorithmID);
			UnionSetResponseRemark("�㷨��ʶ����ȷ");
			return(0);
	}
}

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ����
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
		UnionSetResponseRemark("��Կ���Ƹ�ʽ����ȷ");
		return(0);
	}
	else
	{
		if (keyName[len-1] == '.')
		{
			UnionSetResponseRemark("��Կ���Ƹ�ʽ����ȷ");
			return(0);
		}
		else
			return(1);
	}
}

// ����Կ�����л�ȡ��������
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

// ��PBOC��Կ�����л�ȡ��������
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

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ
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
���ܣ�	����һ���ǶԳ���Կ����
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCreateAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[8192*2];
	
	// ת����Կ����
	if ((ret = UnionTransformAsymmetricKeyName(pasymmetricKeyDB->keyName,sizeof(pasymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDB:: UnionTransformAsymmetricKeyName[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}

	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	UnionGetFullSystemDateTime(pasymmetricKeyDB->createTime);
	pasymmetricKeyDB->createTime[14] = 0;
	
	// ���浽���ٻ���
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDB:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// ����3��
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
	
	// ��¼�ǶԳ���Կ������־
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB);
	
	return(0);
}

/* 
���ܣ�	����һ���ǶԳ���Կ����,sql��䲻ͬ��
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCreateAsymmetricKeyDBWithoutSqlSyn(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[8192*2];
	
	// ת����Կ����
	if ((ret = UnionTransformAsymmetricKeyName(pasymmetricKeyDB->keyName,sizeof(pasymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDBWithoutSqlSyn:: UnionTransformAsymmetricKeyName[%s]!\n",pasymmetricKeyDB->keyName);
		return(ret);
	}

	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	UnionGetFullSystemDateTime(pasymmetricKeyDB->createTime);
	pasymmetricKeyDB->createTime[14] = 0;
	
	// ���浽���ٻ���
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionCreateAsymmetricKeyDBWithoutSqlSyn:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// ����3��
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
	
	// ��¼�ǶԳ���Կ������־
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB);
	
	return(0);
}

/* 
���ܣ�	ɾ��һ���ǶԳ���Կ����
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionDropAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	
	len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(pasymmetricKeyDB->keyName,highCachedKey);
	highCachedKey[len] = 0;
	
	// ���ٻ�����ɾ��
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

	// ��¼�ǶԳ���Կ������־
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationDelete,pasymmetricKeyDB);

	return(0);
}

/* 
���ܣ�	�ѷǶԳ���Կ���ݽṹת�����ַ���
������	pasymmetricKeyDB[in]	��Կ������Ϣ
	keyDBStr[out]		��Կ�ַ���
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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

	if(UnionIsAsymmetricKeyDBRecInCachedOnStructFormat()) // cached ֱ�ӱ���struct����
	{
		len = sizeof(TUnionAsymmetricKeyDB);
		memcpy(keyDBStr, (char *)pasymmetricKeyDB, len);
	}
	else	// cached �����ַ�������
	{
		len = sizeof(TUnionAsymmetricKeyDBStr);
		UnionAsymmetricKeyDBStructToStringStruct(&asymmetricKeyDBStr, pasymmetricKeyDB);
		memcpy(keyDBStr, (char *)&asymmetricKeyDBStr, len);
	}
	return(len);
}

/* 
���ܣ�	�ѷǶԳ���Կ�ַ���ת�������ݽṹ
������	keyDBStr[in]		��Կ�ַ���
	pasymmetricKeyDB[out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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

	if(UnionIsAsymmetricKeyDBRecInCachedOnStructFormat()) // cached ֱ�ӱ���struct����
	{
		len = sizeof(TUnionAsymmetricKeyDB);
		memcpy(pasymmetricKeyDB, keyDBStr, len);
	}
	else    // cached �����ַ�������
        {
                len = sizeof(TUnionAsymmetricKeyDBStr);
                memcpy(&asymmetricKeyDBStr, keyDBStr, len);
                UnionAsymmetricKeyDBStringSturctToStruct(pasymmetricKeyDB, &asymmetricKeyDBStr);
        }
	return(0);
}

/* 
���ܣ�	��ȡһ���ǶԳ���Կ����
������	keyName[in]		��Կ����
	isCheckKey[in]		�Ƿ�����Կ�Ŀ����ԣ�1��飬0�����		
	pasymmetricKeyDB[out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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

	// ת����Կ����
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
		// ����ͨ�ò�ѯ��ʹ�ø��ٻ���
		if (isUseHighCached)
			UnionSetIsNotUseHighCached();
	
		// �ӷǶԳ���Կ����ж�ȡ
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
			
			// ���浽���ٻ���
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
		// �����Կʹ��Ȩ��
		if ((ret = UnionCheckAsymmetricKeyPrivilege(pasymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionCheckAsymmetricKeyPrivilege keyName[%s]!\n",pasymmetricKeyDB->keyName);
			return(ret);
		}

		// �����Կ��Ч��
		if ((ret = UnionCheckAsymmetricKeyStatus(pasymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionReadAsymmetricKeyDBRec:: UnionCheckAsymmetricKeyStatus keyName[%s]!\n",keyName);
			return(ret);
		}
	}
	return(0);
}

/* 
���ܣ�	���ɷǶԳ���Կ
������	pasymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGenerateAsymmetricKeyDBRec(PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	i;
	int	ret;
	int	len;
	char	sql[81920];
	char	highCachedKey[256];
	char	keyDBStr[8192*2];
	
	// ת����Կ����
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

	// ���浽���ٻ���
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAsymmetricKeyDBRec:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;

	// ����3��
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

	// ��¼�ǶԳ���Կ������־
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationInsert,pasymmetricKeyDB);

	return(0);
}

/* 
���ܣ�	���·ǶԳ���Կ
������	pasymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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

	//add begin by lusj 201512122 ��memcached��ȡ��Կ��Ϣ
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

	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// ��ʼ��״̬
	{
		status = conAsymmetricKeyStatusOfEnabled;	// leipp modify  20141106
		memcpy(pasymmetricKeyDB->activeDate,pasymmetricKeyDB->keyUpdateTime,8);
		pasymmetricKeyDB->activeDate[8] = 0;
	}
	
	// ���浽���ٻ���
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// ����3��
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValue:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// ��ʼ��״̬
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
		// ��Կ��Ч��������Կֵ���µ�ͬʱ�����޸�
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
	
		// modify begin  by lusj  20151222	���ݿ����ʧ�ܸ����������memcached����
		if(strlen(pasymmetricKeyDB_backup->pkValue) > 0)
		{	//ԭ�е�memcached������Կ����ԭ֮ǰ����Կֵ
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
			UnionDeleteHighCachedValue(highCachedKey);//ԭ�е�memcached��������Կֵ����del����
		// modify end  by lusj	20151222

		free(pasymmetricKeyDB_backup);	
		
		return(ret);
	}

	// ��¼�ǶԳ���Կ������־
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationUpdate,pasymmetricKeyDB);
	free(pasymmetricKeyDB_backup);
	return(0);
}

/* 
���ܣ�	���·ǶԳ���Կ, ��ͬ��sql���
������	pasymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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

	//add begin by lusj 201512122 ��memcached��ȡ��Կ��Ϣ
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

	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// ��ʼ��״̬
	{
		status = conAsymmetricKeyStatusOfEnabled;	// leipp modify  20141106
		memcpy(pasymmetricKeyDB->activeDate,pasymmetricKeyDB->keyUpdateTime,8);
		pasymmetricKeyDB->activeDate[8] = 0;
	}
	
	// ���浽���ٻ���
	if ((len = UnionAsymmetricKeyDBStructToString(pasymmetricKeyDB,keyDBStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionAsymmetricKeyDBStructToString!\n"); 
		return(len);
	}
	keyDBStr[len] = 0;
	
	// ����3��
	for(i = 0; i < 3; i++)
	{
		if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
			UnionUserErrLog("in UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn:: UnionSetHighCachedValue[%s] loop[%d] error!\n",highCachedKey,i+1);
		else
			break;
	}
	
	if (ret < 0)
		return(ret);
	
	if (pasymmetricKeyDB->status == conAsymmetricKeyStatusOfInitial)	// ��ʼ��״̬
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
	
		// modify begin  by lusj  20151222	���ݿ����ʧ�ܸ����������memcached����
		if(strlen(pasymmetricKeyDB_backup->pkValue) > 0)
		{	//ԭ�е�memcached������Կ����ԭ֮ǰ����Կֵ
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
			UnionDeleteHighCachedValue(highCachedKey);//ԭ�е�memcached��������Կֵ����del����
		// modify end  by lusj	20151222

		free(pasymmetricKeyDB_backup);	
		
		return(ret);
	}

	// ��¼�ǶԳ���Կ������־
	// UnionInsertAsymmetricKeyDBJnl(conAsymmetricKeyDBOperationUpdate,pasymmetricKeyDB);
	free(pasymmetricKeyDB_backup);
	return(0);
}

/* 
���ܣ�	���ǶԳ���Կ״̬
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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
		UnionSetResponseRemark("��Կ[%s]δ����",pasymmetricKeyDB->keyName);
		return(errCodeEsscMDL_KeyStatusDisabled);
	}
	
	// �����Կ��Ч����
	UnionGetFullSystemDate(systemDate);
	systemDate[8] = 0;
	if ((strlen(pasymmetricKeyDB->activeDate) == 0) || (strcmp(pasymmetricKeyDB->activeDate,systemDate) > 0))
	{
		UnionUserErrLog("in UnionCheckAsymmetricKeyStatus:: ��Ч����[%s] > ϵͳ����[%s]!\n",pasymmetricKeyDB->activeDate,systemDate);
		UnionSetResponseRemark("��Կ[%s]δ��Ч",pasymmetricKeyDB->keyName);
		return(errCodeEsscMDL_KeyStatusNotActive);
	}

	// �����Чʱ��
	if (pasymmetricKeyDB->effectiveDays > 0)
	{
		// ����ʧЧ����
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
			UnionUserErrLog("in UnionCheckAsymmetricKeyStatus:: ��Կ[%s]��ʧЧ,��Ч����[%s],��Ч����[%d],ʧЧ����[%s],��ǰ����[%s]!\n",pasymmetricKeyDB->keyName,pasymmetricKeyDB->activeDate,pasymmetricKeyDB->effectiveDays,effectiveDate,systemDate);
			UnionSetResponseRemark("��Կ[%s]��ʧЧ,��Ч����[%s],��Ч����[%d]",pasymmetricKeyDB->keyName,pasymmetricKeyDB->activeDate,pasymmetricKeyDB->effectiveDays);
			return(errCodeEsscMDL_KeyStatusNotActive);
		}
	}
	return(0);
}

/*
���ܣ�	���ǶԳ���Կʹ��Ȩ��
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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
			UnionSetResponseRemark("ϵͳID[%s]��ʽ����",unitID);
			return(errCodeParameter);
		}
		ptr += 1;
		snprintf(unitID,sizeof(unitID),"%s",ptr);

		if (strcmp(unitID,pasymmetricKeyDB->usingUnit) != 0)
		{
			UnionUserErrLog("in UnionCheckAsymmetricKeyPrivilege:: ����λID[%s] != ��Կ��ʹ�õ�λ[%s]\n",unitID,pasymmetricKeyDB->usingUnit);
			return(errCodeHsmCmdMDL_KeyNotPermitted);
		}
	}

	return 0;
}


/* 
���ܣ�	ƴ�ӷǶԳ���Կ�Ĵ�����
������	sysID[in]	ϵͳID
	appID[in]	Ӧ��ID
	creator[out]	������
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
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

// ɾ��֤��
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

// ��ʼ����Կ�ṹ��
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
