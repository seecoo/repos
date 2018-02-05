//	Author: ChenJiaMei
//	Date: 2008-8-8

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionRec0.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionComplexDBObjectSql.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBUniqueKey.h"
#include "unionComplexDBRecordValue.h"
#include "unionComplexDBRecordParents.h"
#include "unionComplexDBRecordChild.h"
#include "unionComplexDBRecord.h"

int UnionConnectDatabase()
{
	return(0);
}

int UnionCloseDatabase()
{
	return(0);
}

/*
���ܣ�����һ�������ʵ��
���������
	idOfObject������ID
	record������ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertObjectRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord)
{
	int	iRet,i,j,iLen,iRecLen;
	char	caBuf[4096],caPrimaryKey[4096],caNameStr[512],caKeyStr[4096],caRecord[4096+1];
        char    caBufCpy[512];
	char	caForeignPrimaryKey[4096];
	TUnionObject		sObject;
	TUnionRec		sRecord;
	
	if (idOfObject == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	UnionProgramerLog("in UnionInsertObjectRecord:: [%04d][%s]\n",lenOfRecord,record);
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// ��ȡ������ֵ
	memset(&sRecord,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sRecord)) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecord,record);
		return(iRet);
	}
	
	// ��鸳ֵ�Ƿ�Ϸ�
	for (i=0;i<sObject.fldNum;i++)		// Ϊû�ж������ȱʡֵ
	{
		if ((iRet=UnionReadRecFldFromRecStr(record,lenOfRecord,sObject.fldDefGrp[i].name,caBuf,sizeof(caBuf))) < 0)
		{
			if (iRet != errCodeEsscMDL_FldNotExists)
			{
				UnionUserErrLog("in UnionInsertObjectRecord:: UnionReadRecFldFromRecStr [%s] from [%d] [%s]\n",sObject.fldDefGrp[i].name,lenOfRecord,record);
				return(iRet);
			}
			strcpy(sRecord.fldName[sRecord.fldNum],sObject.fldDefGrp[i].name);
			strcpy(sRecord.fldValue[sRecord.fldNum],sObject.fldDefGrp[i].defaultValue);
			sRecord.fldNum++;
			//UnionAuditLog("in UnionInsertObjectRecord:: UnionReadRecFldFromRecStr return < 0!\n");
		}
	}

	/* ������ɾ���˶� 2009/5/12	
	if (sRecord.fldNum > sObject.fldNum)
	{
		UnionUserErrLog("in UnionInsertObjectRecord::  field number[%d] of record[%s] > field number[%d] pre-defined of object [%s]\n",sRecord.fldNum,record,sObject.fldNum,idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNumberError));
	}
	*/

	for (i=0;i<sRecord.fldNum;i++)
	{
		for (j=0;j<sObject.fldNum;j++)
		{
			if (strcmp(sRecord.fldName[i],sObject.fldDefGrp[j].name) == 0)
				break;
		}
		if (j == sObject.fldNum)
			continue;
		if (!UnionIsValidObjectFieldValue(sObject.fldDefGrp[j],sRecord.fldValue[i],strlen(sRecord.fldValue[i])))
		{
			UnionUserErrLog("in UnionInsertObjectRecord::  field [%s] value[%s] is invalid!\n",sRecord.fldName[i],sRecord.fldValue[i]);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldValueIsInvalid));
		}
	}
	
	// ����¼�ṹд�ɼ�¼��
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=UnionPutRecIntoRecStr(&sRecord,caRecord,sizeof(caRecord));
	if (iRecLen < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionPutRecIntoRecStr fail! return=[%d]\n",iRecLen);
		return(iRecLen);
	}
	
	// ���ؼ����Ƿ��ظ�
	memset(caPrimaryKey,0,sizeof(caPrimaryKey));
	// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
	iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.primaryKey),caRecord,iRecLen,caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldValueStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
		return(iRet);
	}
	iRet=UnionObjectRecPaimaryKeyIsRepeat(idOfObject,caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionObjectRecPaimaryKeyIsRepeat fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: primary key [%s] is repeat!\n",caPrimaryKey);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_PrimaryKeyIsRepeat));
	}
	
	// ���Ψһֵ�Ƿ��ظ�
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// �γ������鴮"����1.����2.����3��"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldValueStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		iRet=UnionObjectRecUniqueKeyIsRepeat(idOfObject,caNameStr,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionObjectRecUniqueKeyIsRepeat [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		if (iRet > 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: unique key [%s] is repeat!\n",caKeyStr);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_UniqueKeyIsRepeat));
		}
	}
	
	// ���ⲿ����ǼǱ���¼�����ü�¼
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// �����ⲿ��ֵ�п�����Ψһֵ������ͨ�����ҵõ��ⲿ����Ĵ洢λ�õõ��ⲿ����ļ�ֵ
		iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
		if (iRet < 0)
		{
			if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
			{
				UnionUserErrLog("in UnionInsertObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				return(iRet);
			}
			// �γ������鴮"����1.����2.����3��"
			memset(caNameStr,0,sizeof(caNameStr));
			iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].foreignFldGrp),caNameStr);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
				return(iRet);
			}
			iRet=UnionGetObjectRecStoreStationByUniqueKey(sObject.foreignFldGrp[i].objectName,caNameStr,caKeyStr,caBuf,sizeof(caBuf));
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionInsertObjectRecord:: UnionGetObjectRecStoreStationByUniqueKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				return(iRet);
			}
		}
		
		memset(caForeignPrimaryKey,0,sizeof(caForeignPrimaryKey));
		iRet=UnionGetRecordPrimaryKeyFromStorePosition(caBuf,strlen(caBuf),caForeignPrimaryKey);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionGetRecordPrimaryKeyFromStorePosition for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
		
		iRet=UnionAddObjectRecChildFile(sObject.foreignFldGrp[i].objectName,caForeignPrimaryKey,idOfObject,caPrimaryKey);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionAddObjectRecChildFile for [%s] [%s] [%s] [%s] fail! return=[%d]\n",sObject.foreignFldGrp[i].objectName,caKeyStr,idOfObject,caPrimaryKey,iRet);
			return(iRet);
		}
	}
	
	// ��������¼
	iRet=UnionInsertObjectRecordValue(idOfObject,caPrimaryKey,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionInsertObjectRecordValue fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// �ڱ�����ʵ���ڲ������ļ��еǼ�ʵ�������ü�¼
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// �γ������鴮"����1.����2.����3��"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		iRet=UnionAddObjectRecParentsFile(idOfObject,caPrimaryKey,caNameStr,sObject.foreignFldGrp[i].objectName,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionAddObjectRecParentsFile for [%s] [%s] [%s] fail! return=[%d]\n",caNameStr,sObject.foreignFldGrp[i].objectName,caKeyStr,iRet);
			return(iRet);
		}
	}
	
	// �ڹؼ��������еǼǱ���¼
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecValueFileName(idOfObject,caPrimaryKey,caBuf);

        // add by chenliang, 2008-09-16
        memset(caBufCpy, 0, sizeof(caBufCpy));
        UnionFilterMainWorkingDirStr(caBuf, caBufCpy);
        // add end

        // modify by chenliang, 2008-09-16
        // iRet=UnionAddObjectPaimaryKey(idOfObject,caPrimaryKey,caBuf);
	iRet=UnionAddObjectPaimaryKey(idOfObject,caPrimaryKey,caBufCpy);
        // modify end

	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionAddObjectPaimaryKey for [%s] [%s] fail! return=[%d]\n",caPrimaryKey,caBuf,iRet);
		return(iRet);
	}
	
	// ��Ψһֵ�����еǼǱ���¼
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// �γ������鴮"����1.����2.����3��"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldValueStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}

                // modify by chenliang, 2008-09-17
		// iRet=UnionAddObjectUniqueKey(idOfObject,caNameStr,caKeyStr,caBuf);
                iRet=UnionAddObjectUniqueKey(idOfObject,caNameStr,caKeyStr,caBufCpy);
                // modify end
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionAddObjectUniqueKey for [%s] [%s] [%s] fail! return=[%d]\n",idOfObject,caNameStr,caKeyStr,iRet);
			return(iRet);
		}
	}
	
	return(0);
}

/*
���ܣ����ݹؼ���ɾ��һ�������ʵ��
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey)
{
	int	iRet,i,j,iRecLen;
	char	caPrimaryKey[512],caRecord[4096],caNameStr[512],caKeyStr[4096];
	char	caBuf[2048],caForeignPrimaryKey[4096];
	TUnionObject		sObject;
	TUnionRec		sCondition;
	
	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// ��ȡ����ɾ������
	memset(&sCondition,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(primaryKey,strlen(primaryKey),&sCondition)) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionReadRecFromRecStr from [%s] fail! return=[%d]\n",primaryKey,iRet);
		return(iRet);
	}
	
	// ��������Ƿ�Ϸ�
	for (i=0;i<sCondition.fldNum;i++)
	{
		for (j=0;j<sObject.fldNum;j++)
		{
			if (strcmp(sCondition.fldName[i],sObject.fldDefGrp[j].name) == 0)
				break;
		}
		if (j == sObject.fldNum)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord::  field [%s] in delete condition does not found!\n",sCondition.fldName[i]);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
		}
		if (!UnionIsValidObjectFieldValue(sObject.fldDefGrp[j],sCondition.fldValue[i],strlen(sCondition.fldValue[i])))
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord::  field [%s] value[%s] is invalid!\n",sCondition.fldName[i],sCondition.fldValue[i]);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldValueIsInvalid));
		}
	}
	
	// �γɹؼ��ֵ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
	memset(caPrimaryKey,0,sizeof(caPrimaryKey));
	iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.primaryKey),primaryKey,strlen(primaryKey),caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// ���Ҫɾ���ļ�¼�Ƿ�������¼����
	iRet=UnionExistForeignObjectRecordDef(idOfObject,caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionExistForeignObjectRecordDef for [%s] [%s] fail! return=[%d]\n",idOfObject,caPrimaryKey,iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: record [%s] of table [%s] is referenced by other objects records already!\n",caPrimaryKey,idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_RecordIsReferenced));
	}
	
	// ����Ҫɾ���ļ�¼
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,primaryKey,caRecord,sizeof(caRecord));
	if (iRecLen < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionSelectUniqueObjectRecordByPrimaryKey for [%s] [%s] fail! return=[%d]\n",idOfObject,primaryKey,iRecLen);
		return(iRecLen);
	}
	
	// ɾ�����ⲿ�����¼������
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		
		// �����ⲿ��ֵ�п�����Ψһֵ������ͨ�����ҵõ��ⲿ����Ĵ洢λ�õõ��ⲿ����ļ�ֵ
		iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
		if (iRet < 0)
		{
			if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
			{
				UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				return(iRet);
			}
			// �γ������鴮"����1.����2.����3��"
			memset(caNameStr,0,sizeof(caNameStr));
			iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].foreignFldGrp),caNameStr);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
				return(iRet);
			}
			iRet=UnionGetObjectRecStoreStationByUniqueKey(sObject.foreignFldGrp[i].objectName,caNameStr,caKeyStr,caBuf,sizeof(caBuf));
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionGetObjectRecStoreStationByUniqueKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				return(iRet);
			}
		}
		memset(caForeignPrimaryKey,0,sizeof(caForeignPrimaryKey));
		iRet=UnionGetRecordPrimaryKeyFromStorePosition(caBuf,strlen(caBuf),caForeignPrimaryKey);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionGetRecordPrimaryKeyFromStorePosition for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
		
		iRet=UnionDeleteObjectRecChildFile(sObject.foreignFldGrp[i].objectName,caForeignPrimaryKey,idOfObject,caPrimaryKey);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteObjectRecChildFile for [%s] [%s] [%s] [%s] fail! return=[%d]\n",sObject.foreignFldGrp[i].objectName,caKeyStr,idOfObject,caPrimaryKey,iRet);
			return(iRet);
		}
	}
	
	// ɾ���ؼ��������еǼǵı���¼
	iRet=UnionDeleteObjectPaimaryKey(idOfObject,caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteObjectPaimaryKey for [%s] [%s] fail! return=[%d]\n",idOfObject,caPrimaryKey,iRet);
		return(iRet);
	}

	// ɾ��Ψһֵ�����еǼǵı���¼
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// �γ������鴮"����1.����2.����3��"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		
		iRet=UnionDeleteObjectUniqueKey(idOfObject,caNameStr,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteObjectUniqueKey for [%s] [%s] [%s] fail! return=[%d]\n",idOfObject,caNameStr,caKeyStr,iRet);
			return(iRet);
		}
	}
	
	// Mary add begin, 2008-9-4
	// ��Windows�£�ֻ��Ŀ¼Ϊ�գ����ܽ�Ŀ¼ɾ�����������ӱ���
	// ɾ��������ڲ������ļ�
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecParentsListFileName(idOfObject,caPrimaryKey,caBuf);
	if ((iRet = UnionExistsFile(caBuf)) != 0)
	{
		iRet=UnionDeleteRecFile(caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
	}
	
	// ɾ�������ֵ�洢�ļ�
	memset(caBuf,0,sizeof(caBuf));
	iRet=UnionDeleteObjectRecordValue(idOfObject,caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteObjectRecordValue for [%s] fail! return=[%d]\n",caPrimaryKey,iRet);
		return(iRet);
	}
	// Mary add end, 2008-9-4
	
	// ɾ������¼
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecDirName(idOfObject,caPrimaryKey,caBuf);
	iRet=UnionRemoveDir(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionRemoveDir for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ��ж�һ������Ƿ���Ҫ�޸�
���������
        fldGrp��������ṹ
	record����¼������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
����ֵ��
        1:      ��Ҫ�޸�
        0��     ����Ҫ�޸�
*/
int UnionIsForeignKeyShouldUpdate(PUnionObjectFldGrp fldGrp,char *record)
{
        int     i;
        int     len = strlen(record);
        char	caValue[4096];
        int     iRet;

        if (fldGrp == NULL || record == NULL)
	{
		UnionUserErrLog("in UnionIsForeignKeyShouldUpdate:: parameter is error!\n");
		return(0);
	}
        /*
        UnionLog("In UnionIsForeignKeyShouldUpdate, the record is: [%s].\n", record);

        for (i=0;i<fldGrp->fldNum;i++)
        {
                UnionLog("In UnionIsForeignKeyShouldUpdate, the [%d] is: [%s].\n", i, fldGrp->fldNameGrp[i]);
        } */

        for (i=0;i<fldGrp->fldNum;i++)
        {
                iRet = UnionReadRecFldFromRecStr(record, len, fldGrp->fldNameGrp[i], caValue,sizeof(caValue));
                if (iRet > 0)
                {
                        UnionProgramerLog("In UnionIsForeignKeyShouldUpdate, fld [%s] should be update.\n", fldGrp->fldNameGrp[i]);
                        return 1;
                }
        }
        
        return 0;
}

/*
���ܣ��޸�һ�������ʵ��
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionUpdateUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	int	iRet,i,j;
	char	caPrimaryKey[512];
	TUnionObject		sObject;
	TUnionRec		sUpdateRec;
	TUnionObjectFldGrp	sTmpFldGrp;
        
        char    caRecord[4096], caKeyStr[4096], caRec[4096], tmp[512];
        char    tmpCaDRecord[2048 + 512];
        char    caBuf[2048], caNameStr[512], caForeignPrimaryKey[4096];
        int     sizeOfCaRecord  = sizeof(caRecord);
        int     iRecLen;
        int     start = 0, len;
        char    *pos;
        char    *caRecordPos;

	if (idOfObject == NULL || primaryKey == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
        
	// ��ȡ�޸ĵ�ʵ������
	memset(&sUpdateRec,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sUpdateRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadRecFromRecStr from [%04d][%s] fail! return=[%d]\n",lenOfRecord,record,iRet);
		return(iRet);
	}

	// ����޸ĵ����Ƿ�Ϸ�
	for (i=0;i<sUpdateRec.fldNum;i++)
	{
		memset(&sTmpFldGrp,0,sizeof(TUnionObjectFldGrp));
		sTmpFldGrp.fldNum=1;
		strcpy(sTmpFldGrp.fldNameGrp[0],sUpdateRec.fldName[i]);
		// ����Ƿ�ؼ���
		/* deleted by Wolfgang Wang, 2009/5/19
		iRet=UnionIsChildFldGrpOfFldGrp(&sTmpFldGrp,&(sObject.primaryKey));
		if (iRet == 1)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: primary key [%s] can not update!\n",sTmpFldGrp.fldNameGrp[0]);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldCanNotUpdate));
		}
		// ����Ƿ�Ψһֵ
		for (j=0;j<sObject.uniqueFldGrpNum;j++)
		{
			iRet=UnionIsChildFldGrpOfFldGrp(&sTmpFldGrp,&(sObject.uniqueFldGrp[j]));
			if (iRet == 1)
			{
				UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: unique key [%s] can not update!\n",sTmpFldGrp.fldNameGrp[0]);
				return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldCanNotUpdate));
			}
		}
		end of deleting by Wolfgang Wang,2009/5/19
		*/
                // del by chenliang, 2008-9-12
                // �����û��޸� �ⲿ�ؼ����еı�������
                /*
		// ����Ƿ��ⲿ�ؼ����еı�������
		for (j=0;j<sObject.foreignFldGrpNum;j++)
		{
			iRet=UnionIsChildFldGrpOfFldGrp(&sTmpFldGrp,&(sObject.foreignFldGrp[j].localFldGrp));
			if (iRet == 1)
			{
				UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: foreign key [%s] can not update!\n",sTmpFldGrp.fldNameGrp[0]);
				return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldCanNotUpdate));
			}
		} */
                // del end
	}
        
	// ����޸ĵ���ֵ�Ƿ�Ϸ�
	for (i=0;i<sUpdateRec.fldNum;i++)
	{
		for (j=0;j<sObject.fldNum;j++)
		{
			if (strcmp(sUpdateRec.fldName[i],sObject.fldDefGrp[j].name) == 0)
				break;
		}
		if (j == sObject.fldNum)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord::  field [%s] in update record does not found!\n",sUpdateRec.fldName[i]);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
		}
		if (!UnionIsValidObjectFieldValue(sObject.fldDefGrp[j],sUpdateRec.fldValue[i],strlen(sUpdateRec.fldValue[i])))
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord::  field [%s] value[%s] is invalid!\n",sUpdateRec.fldName[i],sUpdateRec.fldValue[i]);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldValueIsInvalid));
		}
	}

	// �γɹؼ��ֵ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
	memset(caPrimaryKey,0,sizeof(caPrimaryKey));
	iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.primaryKey),primaryKey,strlen(primaryKey),caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for primary key [%s] fail! return=[%d]\n",primaryKey,iRet);
		return(iRet);
	}
        
        // add by chenliang. 2008-9-12
        // ��ȡ���ⲿ�����¼������
        memset(caRec, 0, sizeof(caRec));
        iRecLen=UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,primaryKey,caRec,sizeof(caRec));
	if (iRecLen < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionSelectUniqueObjectRecordByPrimaryKey for [%s] [%s] fail! return=[%d]\n",idOfObject,primaryKey,iRecLen);
		return(iRecLen);
	}
        memset(caRecord, 0, sizeOfCaRecord);
        caRecordPos = caRecord;
       
        for (i=0;i<sUpdateRec.fldNum;i++)
        {
                pos = strstr(caRec, sUpdateRec.fldName[i]);
                start = pos - caRec;
                if(start >= 0)
                {
                        pos = strchr(caRec + start, '|');

                        memset(tmp, 0, sizeof(tmp));
                        strncpy(tmp, caRec + start, pos - (caRec + start) + 1);
                        len = sprintf(caRecordPos, "%s", tmp);
                        caRecordPos += len;
                }
        }
        
        // ɾ�� �ⲿ�����¼������
        memset(tmpCaDRecord, 0, sizeof(tmpCaDRecord));
        sprintf(tmpCaDRecord, "%s%s", primaryKey, caRecord);

        UnionProgramerLog("in UnionUpdateUniqueObjectRecord, object: [%s], foreignFldGrpNum: [%d], \ntmpCaDRecord: [%s].\n", idOfObject, sObject.foreignFldGrpNum, tmpCaDRecord);
        
        for (i=0;i<sObject.foreignFldGrpNum;i++)
        {
                if (UnionIsForeignKeyShouldUpdate(&(sObject.foreignFldGrp[i].localFldGrp), tmpCaDRecord) == 1)
                {
                        // �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		        memset(caKeyStr,0,sizeof(caKeyStr));
                        // modify by chenliang, 2008-10-08
                        iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),tmpCaDRecord,strlen(tmpCaDRecord),caKeyStr);
                        // modify by chenliang, 2008-10-08
                        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			        return(iRet);
		        }

                        // �����ⲿ��ֵ�п�����Ψһֵ������ͨ�����ҵõ��ⲿ����Ĵ洢λ�õõ��ⲿ����ļ�ֵ
		        iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
		        if (iRet < 0)
                        {
                                if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
			        {
				        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				        return(iRet);
			        }

                                // �γ������鴮"����1.����2.����3��"
		        	memset(caNameStr,0,sizeof(caNameStr));
			        iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].foreignFldGrp),caNameStr);
			        if (iRet < 0)
			        {
				        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
				        return(iRet);
		        	}
			        iRet=UnionGetObjectRecStoreStationByUniqueKey(sObject.foreignFldGrp[i].objectName,caNameStr,caKeyStr,caBuf,sizeof(caBuf));
			        if (iRet < 0)
			        {
			        	UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByUniqueKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
			        	return(iRet);
			        }
                        }

                        memset(caForeignPrimaryKey,0,sizeof(caForeignPrimaryKey));
		        iRet=UnionGetRecordPrimaryKeyFromStorePosition(caBuf,strlen(caBuf),caForeignPrimaryKey);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetRecordPrimaryKeyFromStorePosition for [%s] fail! return=[%d]\n",caBuf,iRet);
			        return(iRet);
		        }
		
		        iRet=UnionDeleteObjectRecChildFile(sObject.foreignFldGrp[i].objectName,caForeignPrimaryKey,idOfObject,caPrimaryKey);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionDeleteObjectRecChildFile for [%s] [%s] [%s] [%s] fail! return=[%d]\n",sObject.foreignFldGrp[i].objectName,caKeyStr,idOfObject,caPrimaryKey,iRet);
			        return(iRet);
		        }
                }
        }
        // add end

        UnionProgramerLog("in UnionUpdateUniqueObjectRecord, caPrimaryKey: [%s], record: [%s].\n", caPrimaryKey, record);
        
	iRet=UnionUpdateObjectRecordValue(idOfObject,caPrimaryKey,record,lenOfRecord);
	if (iRet < 0)
	{
                UnionLog("In UnionUpdateUniqueObjectRecord::UnionUpdateObjectRecordValue failed.\n");
                // add by chenliang. 2008-9-12
                // �ָ� �ⲿ����ǼǱ���¼�����ü�¼
                for (i=0;i<sObject.foreignFldGrpNum;i++)
                {
                        if (UnionIsForeignKeyShouldUpdate(&(sObject.foreignFldGrp[i].localFldGrp), tmpCaDRecord) == 1)
                        {
                                memset(caKeyStr,0,sizeof(caKeyStr));
                                // modify by chenliang, 2008-10-08
                                // iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,strlen(caRecord),caKeyStr);
                                iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),tmpCaDRecord,strlen(tmpCaDRecord),caKeyStr);
                                // modify end.
                                if (iRet < 0)
                                {
                                        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
                                        return(iRet);
                                }

                                // �����ⲿ��ֵ�п�����Ψһֵ������ͨ�����ҵõ��ⲿ����Ĵ洢λ�õõ��ⲿ����ļ�ֵ
                                iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
                                if (iRet < 0)
                                {
                                        if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
                                        {
                                                UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
                                                return(iRet);
                                        }
                                        // �γ������鴮"����1.����2.����3��"
                                        memset(caNameStr,0,sizeof(caNameStr));
                                        iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].foreignFldGrp),caNameStr);
                                        if (iRet < 0)
                                        {
                                                UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
                                                return(iRet);
                                        }
                                        iRet=UnionGetObjectRecStoreStationByUniqueKey(sObject.foreignFldGrp[i].objectName,caNameStr,caKeyStr,caBuf,sizeof(caBuf));
                                        if (iRet < 0)
                                        {
                                                UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByUniqueKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
                                                return(iRet);
                                        }
                                        memset(caForeignPrimaryKey,0,sizeof(caForeignPrimaryKey));
		                        iRet=UnionGetRecordPrimaryKeyFromStorePosition(caBuf,strlen(caBuf),caForeignPrimaryKey);
		                        if (iRet < 0)
		                        {
			                        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetRecordPrimaryKeyFromStorePosition for [%s] fail! return=[%d]\n",caBuf,iRet);
			                        return(iRet);
		                        }
                                        iRet=UnionAddObjectRecChildFile(sObject.foreignFldGrp[i].objectName,caForeignPrimaryKey,idOfObject,caPrimaryKey);
		                        if (iRet < 0)
		                        {
			                        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionAddObjectRecChildFile for [%s] [%s] [%s] [%s] fail! return=[%d]\n",sObject.foreignFldGrp[i].objectName,caKeyStr,idOfObject,caPrimaryKey,iRet);
			                        return(iRet);
		                        }
                                }
                        }
                }
                // add end
                
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionUpdateObjectRecordValue fail! idOfObject=[%s] caPrimaryKey=[%s] record=[%s], return=[%d]\n",idOfObject,caPrimaryKey,record,iRet);
		return(iRet);
	}
	
	// add by chenliang, 2008-9-12
        // ɾ�� ������ʵ���ڲ������ļ��еǼ�ʵ�������ü�¼
        for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// �γ������鴮"����1.����2.����3��"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caKeyStr,0,sizeof(caKeyStr));
                if (UnionIsForeignKeyShouldUpdate(&(sObject.foreignFldGrp[i].localFldGrp), tmpCaDRecord) == 1)
                {
                        // modify by chenliang, 2008-10-08
		        // iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,iRecLen,caKeyStr);
                        iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),tmpCaDRecord,strlen(tmpCaDRecord),caKeyStr);
                        // modify end.
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			        return(iRet);
		        }
		        iRet=UnionDelObjectRecParentsFile(idOfObject,caPrimaryKey,caNameStr,sObject.foreignFldGrp[i].objectName,caKeyStr);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionDelObjectRecParentsFile for [%s] [%s] [%s] fail! return=[%d]\n",caNameStr,sObject.foreignFldGrp[i].objectName,caKeyStr,iRet);
			        return(iRet);
		        }
                }
	}
        // �� ���ⲿ����ǼǱ���¼�����ü�¼
        memset(caRecord, 0, sizeOfCaRecord);
        strcpy(caRecord, record);

        sprintf(tmpCaDRecord, "%s%s", primaryKey, caRecord);
	
        for (i=0;i<sObject.foreignFldGrpNum;i++)
        {
                if (UnionIsForeignKeyShouldUpdate(&(sObject.foreignFldGrp[i].localFldGrp), tmpCaDRecord) == 1)
                {
                        memset(caKeyStr,0,sizeof(caKeyStr));
                        // modify by chenliang, 2008-10-08
                        // iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,strlen(caRecord),caKeyStr);
                        iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),tmpCaDRecord,strlen(tmpCaDRecord),caKeyStr);
                        // modify end.
                        if (iRet < 0)
                        {
                                UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
                                return(iRet);
                        }

                        // �����ⲿ��ֵ�п�����Ψһֵ������ͨ�����ҵõ��ⲿ����Ĵ洢λ�õõ��ⲿ����ļ�ֵ
                        iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
                        if (iRet < 0)
                        {
                                if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
                                {
                                        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
                                        return(iRet);
                                }
                                // �γ������鴮"����1.����2.����3��"
                                memset(caNameStr,0,sizeof(caNameStr));
                                iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].foreignFldGrp),caNameStr);
                                if (iRet < 0)
                                {
                                        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
                                        return(iRet);
                                }
                                iRet=UnionGetObjectRecStoreStationByUniqueKey(sObject.foreignFldGrp[i].objectName,caNameStr,caKeyStr,caBuf,sizeof(caBuf));
                                if (iRet < 0)
                                {
                                        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByUniqueKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
                                        return(iRet);
                                }
                        }
                        memset(caForeignPrimaryKey,0,sizeof(caForeignPrimaryKey));
		        iRet=UnionGetRecordPrimaryKeyFromStorePosition(caBuf,strlen(caBuf),caForeignPrimaryKey);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetRecordPrimaryKeyFromStorePosition for [%s] fail! return=[%d]\n",caBuf,iRet);
			        return(iRet);
		        }
                        iRet=UnionAddObjectRecChildFile(sObject.foreignFldGrp[i].objectName,caForeignPrimaryKey,idOfObject,caPrimaryKey);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionAddObjectRecChildFile for [%s] [%s] [%s] [%s] fail! return=[%d]\n",sObject.foreignFldGrp[i].objectName,caKeyStr,idOfObject,caPrimaryKey,iRet);
			        return(iRet);
		        }
                }
        }
        
        // �޸ı�����ʵ���ڲ������ļ��еǼ�ʵ�������ü�¼
        // �ڱ�����ʵ���ڲ������ļ��еǼ�ʵ�������ü�¼
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
                if (UnionIsForeignKeyShouldUpdate(&(sObject.foreignFldGrp[i].localFldGrp), tmpCaDRecord) == 1)
                {
		        // �γ������鴮"����1.����2.����3��"
		        memset(caNameStr,0,sizeof(caNameStr));
		        iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caNameStr);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			        return(iRet);
		        }
		        // �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		        memset(caKeyStr,0,sizeof(caKeyStr));
		        // iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,iRecLen,caKeyStr);   tmpCaDRecord
                        iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),tmpCaDRecord,strlen(tmpCaDRecord),caKeyStr);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			        return(iRet);
		        }
		        iRet=UnionAddObjectRecParentsFile(idOfObject,caPrimaryKey,caNameStr,sObject.foreignFldGrp[i].objectName,caKeyStr);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionAddObjectRecParentsFile for [%s] [%s] [%s] fail! return=[%d]\n",caNameStr,sObject.foreignFldGrp[i].objectName,caKeyStr,iRet);
			        return(iRet);
		        }
                }
	}
        // add end

	return(0);
}

/*
���ܣ����ݹؼ��ֲ�ѯһ�������ʵ����ֻѡ��ָ������
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
	fldGrp		Ҫѡ�������嵥��ÿ����֮����,�ָ���
	lenOfFldGrp	���嵥�ĳ���
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectObjectFldGrpByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *fldGrp,int lenOfFldGrp,char *record,int sizeOfRecord)
{
	char	recStr[4096+1];
	int	lenOfRecStr;
	int	ret;
	
	//UnionDebugLog("in UnionSelectObjectFldGrpByPrimaryKey:: fldGrp = [%04d][%s]\n",lenOfFldGrp,fldGrp);
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,primaryKey,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionSelectUniqueObjectRecordByPrimaryKey [%s]\n",idOfObject);
		return(lenOfRecStr);
	}
	//UnionDebugLog("in UnionSelectObjectFldGrpByPrimaryKey:: fldGrp = [%04d][%s]\n",lenOfFldGrp,fldGrp);
	if ((ret = UnionReadFldGrpFromRecStr(recStr,lenOfRecStr,fldGrp,lenOfFldGrp,record,sizeOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionReadFldGrpFromRecStr fldList = [%s] from [%s] on [%s]! ret = [%d]\n",fldGrp,recStr,idOfObject,ret);
		return(ret);
	}
	return(ret);
}

/*
���ܣ����ݹؼ��ֶ�ȡָ������
���������
	idOfObject��	����ID
	primaryKey��	����ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	fldName		����
	sizeOfBuf	���ܲ�ѯ��Ĵ�С
���������
	fldValue	��ֵ
����ֵ��
	>=0��		�ɹ���������ֵ�ĳ���
	<0��		ʧ�ܣ�������
*/
int UnionSelectSpecFldOfObjectByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *fldName,char *fldValue,int sizeOfBuf)
{
	int	ret;
	char	recStr[8192*2+1];
	int	lenOfRecStr;
	
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,primaryKey,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecFldOfObjectByPrimaryKey:: UnionSelectUniqueObjectRecordByPrimaryKey primaryKey = [%s] tableName = [%s]\n",primaryKey,idOfObject);
		return(lenOfRecStr);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecFldOfObjectByPrimaryKey:: UnionReadRecFldFromRecStr [%s] from [%s] of tableName = [%s]\n",fldName,recStr,idOfObject);
		return(ret);
	}
	return(ret);
}
		
/*
���ܣ����ݹؼ��ֶ�ȡָ����������
���������
	idOfObject��	����ID
	primaryKey��	����ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	fldName		����
���������
	fldValue	��ֵ
����ֵ��
	>=0��		�ɹ���������ֵ�ĳ���
	<0��		ʧ�ܣ�������
*/
int UnionSelectSpecIntFldOfObjectByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *fldName,int *fldValue)
{
	int	ret;
	char	recStr[8192*2+1];
	int	lenOfRecStr;
	
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,primaryKey,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecIntFldOfObjectByPrimaryKey:: UnionSelectUniqueObjectRecordByPrimaryKey primaryKey = [%s] tableName = [%s]\n",primaryKey,idOfObject);
		return(lenOfRecStr);
	}
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue)) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecIntFldOfObjectByPrimaryKey:: UnionReadIntTypeRecFldFromRecStr [%s] from [%s] of tableName = [%s]\n",fldName,recStr,idOfObject);
		return(ret);
	}
	return(ret);
}

/*
���ܣ����ݹؼ��ֲ�ѯһ�������ʵ��
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniqueObjectRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	int	iRet;
	char	caPrimaryKey[4096];
	TUnionObject		sObject;
	
	//UnionProgramerLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: [%s] [%s] [%d]\n",idOfObject,primaryKey,sizeOfRecord);
	if (idOfObject == NULL || primaryKey == NULL || record == NULL || sizeOfRecord <= 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// �γɹؼ��ֵ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
	memset(caPrimaryKey,0,sizeof(caPrimaryKey));
	iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.primaryKey),primaryKey,strlen(primaryKey),caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionFormFieldValueStrFromFldGrp for primary key [%s] fail! return=[%d]\n",primaryKey,iRet);
		return(iRet);
	}
	
	iRet=UnionSelectObjectRecordValue(idOfObject,caPrimaryKey,record,sizeOfRecord);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionSelectObjectRecordValue for [%s] [%s] fail! return=[%d]\n",idOfObject,caPrimaryKey,iRet);
		return(iRet);
	}
	
	return(iRet);
}

/*
���ܣ�����Ψһֵ��ѯһ�������ʵ��
���������
	idOfObject������ID
	uniqueKey������ʵ����Ψһֵ����ʽΪ"Ψһֵ��1=��ֵ|Ψһֵ��2=��ֵ|��Ψһֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniqueObjectRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord)
{
	int	iRet,i;
	char	caPrimaryKey[4096],caNameStr[512],caValueStr[4096],caBuf[4096];
	TUnionObject		sObject;
	TUnionObjectFldGrp	sUniqueGrp;
	char	caFileName[512+1];
	
	if (idOfObject == NULL || uniqueKey == NULL || record == NULL || sizeOfRecord <= 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// �γ�������ṹ
	memset(&sUniqueGrp,0,sizeof(TUnionObjectFldGrp));	// Mary add, 20081031
	iRet=UnionFormFldGrpFromRecordStr(uniqueKey,strlen(uniqueKey),&sUniqueGrp);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFormFldGrpFromRecordStr for [%s] fail! return=[%d]\n",uniqueKey,iRet);
		return(iRet);
	}
	
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// ����������Ƿ��Ѷ���
		iRet=UnionFldGrpIsRepeat(&sUniqueGrp,&(sObject.uniqueFldGrp[i]));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFldGrpIsRepeat for unique key [%s] and unique group [%d] in object fail! return=[%d]\n",uniqueKey,i,iRet);
			return(iRet);
		}
		if (iRet == 0)
			continue;
		
		// �γ������鴮"����1.����2.����3��"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// �γ�����ֵ��"��ֵ1.��ֵ2.��ֵ3��"
		memset(caValueStr,0,sizeof(caValueStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.uniqueFldGrp[i]),uniqueKey,strlen(uniqueKey),caValueStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFormFieldValueStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// �õ�ʵ���Ĵ洢λ��
		iRet=UnionGetObjectRecStoreStationByUniqueKey(idOfObject,caNameStr,caValueStr,caBuf,sizeof(caBuf));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionGetObjectRecStoreStationByUniqueKey for [%s] [%s] [%s] fail! return=[%d]\n",idOfObject,caNameStr,caValueStr,iRet);
			return(iRet);
		}
		break;
	}
	
	if (i == sObject.uniqueFldGrpNum)	// ʵ��δ�ҵ�
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: record of object [%s] is not found! unique key=[%s] return=[%d]\n",idOfObject,uniqueKey,iRet);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
	}
	
	// ��ȡ��ֵ�ļ��еļ�¼
	// 2008/10/22���������޸�
	UnionPatchMainDirToObjectValueFileName(caBuf,caFileName);
	iRet=UnionReadRecStrFromFile(caFileName,record,sizeOfRecord);
	// 2008/10/22���������޸Ľ���
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionReadRecStrFromFile from [%s] fail! return=[%d]\n",caFileName,iRet);
		return(iRet);
	}
	
	return(iRet);
}

/*
���ܣ�����ɾ��һ�������ʵ��
���������
	idOfObject������ID
	condition��ɾ����������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	��
����ֵ��
	>0���ɹ������ر�ɾ����ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchDeleteObjectRecord(TUnionIDOfObject idOfObject,char *condition)
{
	int	iRet,iLen,iRecLen,iDeleteFlag=0,iDeleteNum=0;
	char	caFileName[512],caValueFile[512],caBuf[512],caPrimaryKey[4096];
        char    caTmpValueFile[512];
	char	caRecord[4096];
	TUnionObject		sObject;
	TUnionRecFileHDL	hFileHdl;
	
	if (idOfObject == NULL || condition == NULL)
	{
		UnionUserErrLog("in UnionBatchDeleteObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionBatchDeleteObjectRecord:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	while(1)
	{                
		hFileHdl=UnionOpenRecFileHDL(caFileName);
		if (hFileHdl == NULL)
		{
			UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionOpenRecFileHDL fail!\n");
			return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
		}
		iDeleteFlag=0;
		while(1)
		{
			// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
			memset(caBuf,0,sizeof(caBuf));
			iLen=UnionReadNextRecFromFile(hFileHdl,caBuf,sizeof(caBuf));
			if (iLen < 0)
			{
				UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadNextRecFromFile fail! return=[%d]\n",iLen);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iLen);
			}
			if (iLen == 0)	// ���ļ�����
				break;
		
			// �õ�ֵ�洢�ļ�����
			memset(caValueFile,0,sizeof(caValueFile));
			iRet=UnionReadRecFldFromRecStr(caBuf,iLen,"fldGrp",caValueFile,sizeof(caValueFile));
			if (iRet < 0)
			{
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
				return(iRet);
			}
			// ������¼��ֵ
			memset(caRecord,0,sizeof(caRecord));
                        memset(caTmpValueFile, 0, sizeof(caTmpValueFile));

			// UnionGetObjectRecValueFileName(idOfObject,caValueFile,caFileName);
                        // Ϊ�˱��⽫ԭ�����ļ������ǵ�
                        UnionGetObjectRecValueFileName(idOfObject,caValueFile,caTmpValueFile);

			// iRecLen=UnionReadRecStrFromFile(caFileName,caRecord,sizeof(caRecord));
                        iRecLen=UnionReadRecStrFromFile(caTmpValueFile,caRecord,sizeof(caRecord));
                        if (iRecLen < 0)
			{
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadRecStrFromFile fail! return=[%d]\n",iRecLen);
				return(iRecLen);
			}
			// �жϸü�¼�Ƿ��������
			iRet=UnionIsRecStrFitSpecConditon(caRecord,iRecLen,condition,strlen(condition));
			if (iRet < 0)
			{
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionIsRecStrFitSpecConditon fail! return=[%d]\n",iRet);
				return(iRet);
			}
			if (iRet == 0)
				continue;
			else
			{
				iDeleteFlag=1;
				break;
			}
		}
		// ����ɾ��һ��ʵ��ʱҪɾ����ֵ�ļ��еļ�¼�������ȹرռ�ֵ�ļ���ɾ����������´�
		UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
		if (iDeleteFlag == 0)
			break;
		
		// �γ�����ֵ��"��ֵ��1=��ֵ|��ֵ��2=��ֵ|��ֵ��3=��ֵ|��"
		memset(caPrimaryKey,0,sizeof(caPrimaryKey));
		iRet=UnionFormFieldRecordStrFromFldGrp(&(sObject.primaryKey),caRecord,iRecLen,caPrimaryKey,sizeof(caPrimaryKey));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionFormFieldRecordStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
			return(iRet);
		}
		// ɾ���ü�¼
		iRet=UnionDeleteUniqueObjectRecord(idOfObject,caPrimaryKey);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionDeleteUniqueObjectRecord for [%s] fail! return=[%d]\n",caPrimaryKey,iRet);
			return(iRet);
		}
		iDeleteNum++;
	}	
	
	return(iDeleteNum);
}

/*
���ܣ������޸�һ�������ʵ��
���������
	idOfObject������ID
	condition���޸���������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ|"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	>=0���ɹ������ر��޸ĵ�ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchUpdateObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord)
{
	int	iRet,iLen,iRecLen,iUpdateNum=0;
	char	caFileName[512],caValueFile[512],caBuf[512],caPrimaryKey[4096],caTmpFile[512];
	char	caRecord[4096];
	TUnionObject		sObject;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || condition == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionBatchUpdateObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionBatchUpdateObjectRecord:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caBuf,0,sizeof(caBuf));
		iLen=UnionReadNextRecFromFile(hFileHdl,caBuf,sizeof(caBuf));
		if (iLen < 0)
		{
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionReadNextRecFromFile fail! return=[%d]\n",iLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iLen);
		}
		if (iLen == 0)	// ���ļ�����
			break;
		
		// �õ�ֵ�洢�ļ�����
		memset(caValueFile,0,sizeof(caValueFile));
		iRet=UnionReadRecFldFromRecStr(caBuf,iLen,"fldGrp",caValueFile,sizeof(caValueFile));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
			return(iRet);
		}
		// ������¼��ֵ
		memset(caTmpFile,0,sizeof(caTmpFile));
		//UnionGetObjectRecValueFileName(idOfObject,caValueFile,caFileName);		Mary modify, 20081212
		UnionGetObjectRecValueFileName(idOfObject,caValueFile,caTmpFile);
		memset(caRecord,0,sizeof(caRecord));
		//iRecLen=UnionReadRecStrFromFile(caFileName,caRecord,sizeof(caRecord));	Mary modify, 20081212
		iRecLen=UnionReadRecStrFromFile(caTmpFile,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionReadRecStrFromFile fail! return=[%d]\n",iRecLen);
			return(iRecLen);
		}
		// �жϸü�¼�Ƿ��������
		iRet=UnionIsRecStrFitSpecConditon(caRecord,iRecLen,condition,strlen(condition));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionIsRecStrFitSpecConditon fail! return=[%d]\n",iRet);
			return(iRet);
		}
		if (iRet == 0)
			continue;
		
		// �γ�����ֵ��"��ֵ��1=��ֵ|��ֵ��2=��ֵ|��ֵ��3=��ֵ|��"
		memset(caPrimaryKey,0,sizeof(caPrimaryKey));
		iRet=UnionFormFieldRecordStrFromFldGrp(&(sObject.primaryKey),caRecord,iRecLen,caPrimaryKey,sizeof(caPrimaryKey));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionFormFieldRecordStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
			return(iRet);
		}
		// �޸ĸü�¼
		iRet=UnionUpdateUniqueObjectRecord(idOfObject,caPrimaryKey,record,lenOfRecord);
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionUpdateUniqueObjectRecord for [%s] fail! return=[%d]\n",caPrimaryKey,iRet);
			return(iRet);
		}
		iUpdateNum++;
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);	
	
	return(iUpdateNum);
}

/*
���ܣ�������ѯһ�������ʵ��
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	fileName���洢�˲�ѯ������ʵ����ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ���ļ�¼����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchSelectObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName)
{
	int	numOfPerPage=0,currPage=0,isUseNewMech=1;
	int	beginNum=0,endNum=0;
	char	tmpStr[20+1];
	int	iRet,iLen,iRecLen,iSelectNum=0;
	char	caFileName[512],caValueFile[512],caBuf[512],caPrimaryKey[4096],caTmpFile[512];
	char	caRecord[4096];
	TUnionObject		sObject;
	TUnionRecFileHDL	hFileHdl,hWFileHdl;

        TUnionRecFileHDL	fp = NULL;

        //UnionLog("in UnionBatchSelectObjectRecord : start ~~~~~\n");

	if (idOfObject == NULL || condition == NULL || fileName == NULL)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
        //UnionLog("in UnionBatchSelectObjectRecord : 2222222222222 ~~~~~\n");
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
        //UnionLog("in UnionBatchSelectObjectRecord : 33333333333333333 ~~~~~\n");
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
        //UnionLog("in UnionBatchSelectObjectRecord : 4444444444444444444 ~~~~~\n");
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		//UnionUserErrLog("in UnionBatchSelectObjectRecord:: [%s] does not exists!\n",caFileName);
		//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
		UnionAuditLog("in UnionBatchSelectObjectRecord:: [%s] does not exists!\n",caFileName);
		return(0);
	}

        //UnionLog("in UnionBatchSelectObjectRecord : 5555555555555555555 ~~~~~\n");

	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
        //UnionLog("in UnionBatchSelectObjectRecord : 6666666666666666666666 ~~~~~\n");
        // 2008-09-04 add by chenliang
        // ���������ʱ�ļ�ԭ�������ݣ������ڽ�����ʾ����
        if((iRet = UnionExistsFile(fileName)) != 0)
        {
        	/* Mary delete, 20081211
                fp = fopen(fileName, "w");
                fclose(fp);
                */
                // Mary add begin, 20081211
                fp=UnionOpenRecFileHDLForWrite(fileName);
                UnionCloseRecFileHDLOfFileName(fp,fileName);
                // Mary add end, 20081211
        }
        // end
        //UnionLog("in UnionBatchSelectObjectRecord : 7777777777777777777777777 ~~~~~\n");
        // Mary add begin, 2008-10-27
        hWFileHdl=UnionOpenRecFileHDLForWrite(fileName);
	if (hWFileHdl == NULL)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionOpenRecFileHDLForWrite fail!\n");
		UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
        // Mary add end, 2008-10-27
        //UnionLog("in UnionBatchSelectObjectRecord : 88888888888888888888888 ~~~~~\n");

	// add by xusj begin, 2009-07-27
	memset(tmpStr, 0, sizeof tmpStr);
	if (UnionReadRecFldFromRecStr(condition,strlen(condition),"currentPage",tmpStr,sizeof(tmpStr)) < 0)
	{
		isUseNewMech=0;
	}
	else
	{
		currPage = atoi(tmpStr);
	}
	memset(tmpStr, 0, sizeof tmpStr);
	if (UnionReadRecFldFromRecStr(condition,strlen(condition),"numOfPerPage",tmpStr,sizeof(tmpStr)) < 0)
	{
		isUseNewMech=0;
	}
	else
	{
		numOfPerPage = atoi(tmpStr);
	}
	if (isUseNewMech)
	{
		beginNum = (currPage-1)*numOfPerPage;
		endNum = beginNum + numOfPerPage;
	}
	UnionDeleteRecFldFromRecStr(condition,"currentPage");
	UnionDeleteRecFldFromRecStr(condition,"numOfPerPage");
	// add by xusj end, 2009-07-27

	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caBuf,0,sizeof(caBuf));
                //UnionLog("in UnionBatchSelectObjectRecord : UnionReadNextRecFromFile start ~~~~~\n");
		iLen=UnionReadNextRecFromFile(hFileHdl,caBuf,sizeof(caBuf));
		if (iLen < 0)
		{
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionReadNextRecFromFile fail! return=[%d]\n",iLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
			return(iLen);
		}
                //UnionLog("in UnionBatchSelectObjectRecord : UnionReadNextRecFromFile OK ~~~~~\n");
		if (iLen == 0)	// ���ļ�����
			break;
		
		// �õ�ֵ�洢�ļ�����
		memset(caValueFile,0,sizeof(caValueFile));
                //UnionLog("in UnionBatchSelectObjectRecord : UnionReadRecFldFromRecStr start ~~~~~\n");
		iRet=UnionReadRecFldFromRecStr(caBuf,iLen,"fldGrp",caValueFile,sizeof(caValueFile));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
			return(iRet);
		}

                //UnionLog("in UnionBatchSelectObjectRecord : UnionReadRecFldFromRecStr get str[%s] ~~~~~\n",caValueFile);
		// ������¼��ֵ
		memset(caRecord,0,sizeof(caRecord));
		//UnionGetObjectRecValueFileName(idOfObject,caValueFile,caFileName);		Mary modify, 20081212
		memset(caTmpFile,0,sizeof(caTmpFile));
		UnionGetObjectRecValueFileName(idOfObject,caValueFile,caTmpFile);
		//iRecLen=UnionReadRecStrFromFile(caFileName,caRecord,sizeof(caRecord));	Mary modify, 20081212
		iRecLen=UnionReadRecStrFromFile(caTmpFile,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionReadRecStrFromFile fail! return=[%d]\n",iRecLen);
			return(iRecLen);
		}
		// �жϸü�¼�Ƿ��������
                // UnionLog("in UnionBatchSelectObjectRecord, the condition: [%s].\n", condition);
                // UnionLog("in UnionBatchSelectObjectRecord : UnionIsRecStrFitSpecConditon start ~~~~~\n");
		iRet=UnionIsRecStrFitSpecConditon(caRecord,iRecLen,condition,strlen(condition));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionIsRecStrFitSpecConditon fail! return=[%d]\n",iRet);
			return(iRet);
		}
		if (iRet == 0)			// ����������
			continue;

                // UnionLog("in UnionBatchSelectObjectRecord, current record: [%s] is fit [%s], iSelectNum is: [%d].\n", caRecord, condition, iSelectNum);
		
		// д�ļ�
		/* Mary delete, 2008-10-27
		iRet=UnionAppendRecStrToFile(fileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl);
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
		*/
		// Mary add begin, 2008-10-27
                //UnionLog("in UnionBatchSelectObjectRecord : UnionAppendRecStrToFileUseFileHDL start ~~~~~\n");
		// add by xusj begin, 2009-07-27
		if (isUseNewMech) 
		{
			if ( (iSelectNum>=beginNum) && (iSelectNum<endNum) )
			{
				iRet=UnionAppendRecStrToFileUseFileHDL(hWFileHdl,caRecord,iRecLen);
				if (iRet < 0)
				{
					UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
					UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
					UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionAppendRecStrToFileUseFileHDL for [%s] fail! return=[%d]\n",caRecord,iRet);
					return(iRet);
				}
			}
		}
		else
		{
			iRet=UnionAppendRecStrToFileUseFileHDL(hWFileHdl,caRecord,iRecLen);
			if (iRet < 0)
			{
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
				UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionAppendRecStrToFileUseFileHDL for [%s] fail! return=[%d]\n",caRecord,iRet);
				return(iRet);
			}
		}
		// add by xusj end, 2009-07-27
		// Mary add end, 2008-10-27
		iSelectNum++;
	}
	// add by xusj begin, 2009-07-27
	if (isUseNewMech)
	{
		memset(caRecord, 0, sizeof caRecord);
		sprintf(caRecord, "%s=%d", "totalRecNum", iSelectNum);
		iRet=UnionAppendRecStrToFileUseFileHDL(hWFileHdl,caRecord,strlen(caRecord));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionAppendRecStrToFileUseFileHDL for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	// add by xusj end, 2009-07-27
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	UnionCloseRecFileHDLOfFileName(hWFileHdl,fileName);
	UnionProgramerLog("in UnionBatchSelectObjectRecord : OK ~~~~~\n");
	return(iSelectNum);
}


/*
���ܣ�ƴװһ���ؼ���������
���������
	idOfObject��	Ҫ���Ķ���ID	
	primaryKeyValueList	�ؼ�ֵ�ִ�,������ֵ��.�ֿ�
	lenOfValueList		�ؼ�ֵ�ִ�����
	sizeOfBuf	�ؼ����嵥�Ĵ�С
���������
	recStr		�ؼ���������
����ֵ��
	0��	�ؼ����������ĳ���
	<0��	ʧ�ܣ����ش�����
*/
int UnionFormObjectPrimarKeyCondition(TUnionIDOfObject idOfObject,char *primaryKeyValueList,int lenOfValueList,char *recStr,int sizeOfBuf)
{
	char	primaryKeyList[512+1];
	int	len;
	int	fldNum;
	char	fldGrp[16][128+1];
	char	valueGrp[16][128+1];
	int	ret;
	int	offset = 0;
	int	index;
	char	caNewValue[4096];
	int	iNewValueLen;
		
	memset(primaryKeyList,0,sizeof(primaryKeyList));
	if ((len = UnionReadObjectPrimarKeyList(idOfObject,primaryKeyList,sizeof(primaryKeyList))) < 0)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionReadObjectPrimarKeyList [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(len));
	}
	//UnionDebugLog("in UnionFormObjectPrimarKeyCondition:: primaryKeyList = [%04d][%s]\n",len,primaryKeyList);
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(primaryKeyList,len,',',fldGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionSeprateVarStrIntoVarGrp [%s] of [%s]!\n",primaryKeyList,idOfObject);
		return(UnionSetUserDefinedErrorCode(fldNum));
	}
	//UnionDebugLog("in UnionFormObjectPrimarKeyCondition:: primaryKeyValueList = [%04d][%s]\n",lenOfValueList,primaryKeyValueList);
	if ((ret = UnionSeprateVarStrIntoVarGrp(primaryKeyValueList,lenOfValueList,'.',valueGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionSeprateVarStrIntoVarGrp [%s] of [%s]!\n",primaryKeyValueList,idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	if (fldNum != ret)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: fldNum [%d] of primaryKey in [%s] != that [%d] in [%s] of [%s]!\n",fldNum,primaryKeyList,ret,primaryKeyValueList,idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	for (index = 0; index < fldNum; index++)
	{
		// Mary add begin, 20081112
		memset(caNewValue,0,sizeof(caNewValue));
		iNewValueLen=UnionTranslateRecFldStrIntoStr(valueGrp[index],strlen(valueGrp[index]),caNewValue,sizeof(caNewValue));
		if (iNewValueLen <= 0)
		{
			UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionTranslateStrIntoRecFldStr for [%s] fail! return=[%d]\n",valueGrp[index],iNewValueLen);
			return(iNewValueLen);
		}
		if (strlen(valueGrp[index]) != iNewValueLen)
			UnionAuditLog("in UnionFormObjectPrimarKeyCondition:: caValue=[%s] iValueLen=[%d] caNewValue=[%s] iNewValueLen=[%d]\n",valueGrp[index],strlen(valueGrp[index]),caNewValue,iNewValueLen);
		if ((ret = UnionPutRecFldIntoRecStr(fldGrp[index],caNewValue,iNewValueLen,recStr+offset,sizeOfBuf-offset)) < 0)
		// Mary add end, 20081112
		//if ((ret = UnionPutRecFldIntoRecStr(fldGrp[index],valueGrp[index],strlen(valueGrp[index]),recStr+offset,sizeOfBuf-offset)) < 0)	Mary delete, 20081112
		{
			UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionPutRecFldIntoRecStr [%s] of [%s]!\n",primaryKeyValueList,idOfObject);
			return(UnionSetUserDefinedErrorCode(ret));
		}
		offset += ret;
	}
	//UnionDebugLog("in UnionFormObjectPrimarKeyCondition:: primaryKey = [%s]\n",recStr);
	return(offset);
}

/*
���ܣ�
	��һ����¼��ƴװ�ؼ���
���������
	idOfObject��		����ID
	record��		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��		record�ĳ���
	sizeOfPrimaryKey	�ؼ�ֵ�������С
���������
	primaryKey		�ؼ�ֵ������.�ֿ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormPrimaryKeyStrOfSpecObjectRec(TUnionIDOfObject idOfObject,char *record,int lenOfRecord,char *primaryKey,int sizeOfPrimaryKey)
{
	TUnionObject		sObject;
	int			iRet;
	int			fldNum;
	int			index;
	char			tmpBuf[4096+1];
	int			offset = 0;
	
	if (idOfObject == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	for (index = 0; index < sObject.primaryKey.fldNum; index++)
	{
		memset(&tmpBuf,0,sizeof(tmpBuf));
		if ((iRet = UnionReadRecFldFromRecStr(record,lenOfRecord,sObject.primaryKey.fldNameGrp[index],tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: UnionReadRecFldFromRecStr [%s] from [%s]\n",sObject.primaryKey.fldNameGrp[index],record);
			return(iRet);
		}
		if (offset + 1 + iRet >= sizeOfPrimaryKey)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: smallBuffer!\n");
			return(errCodeSmallBuffer);
		}
		if (index != 0)
		{
			memcpy(primaryKey+offset,".",1);
			offset++;
		}
		memcpy(primaryKey+offset,tmpBuf,iRet);
		offset += iRet;
	}
	return(offset);
}
