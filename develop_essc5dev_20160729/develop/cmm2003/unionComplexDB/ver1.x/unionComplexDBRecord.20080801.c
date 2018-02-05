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
功能：插入一个对象的实例
输入参数：
	idOfObject：对象ID
	record：对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 读取对象域赋值
	memset(&sRecord,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sRecord)) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecord,record);
		return(iRet);
	}
	
	// 检查赋值是否合法
	for (i=0;i<sObject.fldNum;i++)		// 为没有定义的域赋缺省值
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

	/* 王纯军删除此段 2009/5/12	
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
	
	// 将记录结构写成记录串
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=UnionPutRecIntoRecStr(&sRecord,caRecord,sizeof(caRecord));
	if (iRecLen < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionPutRecIntoRecStr fail! return=[%d]\n",iRecLen);
		return(iRecLen);
	}
	
	// 检查关键字是否重复
	memset(caPrimaryKey,0,sizeof(caPrimaryKey));
	// 形成域名值串"域值1.域值2.域值3…"
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
	
	// 检查唯一值是否重复
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// 形成域名组串"域名1.域名2.域名3…"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// 形成域名值串"域值1.域值2.域值3…"
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
	
	// 在外部对象登记本记录的引用记录
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// 形成域名值串"域值1.域值2.域值3…"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// 由于外部键值有可能是唯一值，所以通过查找得到外部对象的存储位置得到外部对象的键值
		iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
		if (iRet < 0)
		{
			if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
			{
				UnionUserErrLog("in UnionInsertObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				return(iRet);
			}
			// 形成域名组串"域名1.域名2.域名3…"
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
	
	// 创建本记录
	iRet=UnionInsertObjectRecordValue(idOfObject,caPrimaryKey,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionInsertObjectRecordValue fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 在本对象实例内部引用文件中登记实例的引用记录
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// 形成域名组串"域名1.域名2.域名3…"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// 形成域名值串"域值1.域值2.域值3…"
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
	
	// 在关键字索引中登记本记录
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
	
	// 在唯一值索引中登记本记录
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// 形成域名组串"域名1.域名2.域名3…"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// 形成域名值串"域值1.域值2.域值3…"
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
功能：根据关键字删除一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
	
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 读取对象删除条件
	memset(&sCondition,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(primaryKey,strlen(primaryKey),&sCondition)) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionReadRecFromRecStr from [%s] fail! return=[%d]\n",primaryKey,iRet);
		return(iRet);
	}
	
	// 检查条件是否合法
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
	
	// 形成关键字的域名值串"域值1.域值2.域值3…"
	memset(caPrimaryKey,0,sizeof(caPrimaryKey));
	iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.primaryKey),primaryKey,strlen(primaryKey),caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 检查要删除的记录是否被其它记录引用
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
	
	// 查找要删除的记录
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,primaryKey,caRecord,sizeof(caRecord));
	if (iRecLen < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionSelectUniqueObjectRecordByPrimaryKey for [%s] [%s] fail! return=[%d]\n",idOfObject,primaryKey,iRecLen);
		return(iRecLen);
	}
	
	// 删除对外部对象记录的引用
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// 形成域名值串"域值1.域值2.域值3…"
		memset(caKeyStr,0,sizeof(caKeyStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caRecord,iRecLen,caKeyStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		
		// 由于外部键值有可能是唯一值，所以通过查找得到外部对象的存储位置得到外部对象的键值
		iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
		if (iRet < 0)
		{
			if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
			{
				UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				return(iRet);
			}
			// 形成域名组串"域名1.域名2.域名3…"
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
	
	// 删除关键字索引中登记的本记录
	iRet=UnionDeleteObjectPaimaryKey(idOfObject,caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteObjectPaimaryKey for [%s] [%s] fail! return=[%d]\n",idOfObject,caPrimaryKey,iRet);
		return(iRet);
	}

	// 删除唯一值索引中登记的本记录
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// 形成域名组串"域名1.域名2.域名3…"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		
		// 形成域名值串"域值1.域值2.域值3…"
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
	// 在Windows下，只有目录为空，才能将目录删除，所以增加本段
	// 删除对象的内部引用文件
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
	
	// 删除对象的值存储文件
	memset(caBuf,0,sizeof(caBuf));
	iRet=UnionDeleteObjectRecordValue(idOfObject,caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteObjectRecordValue for [%s] fail! return=[%d]\n",caPrimaryKey,iRet);
		return(iRet);
	}
	// Mary add end, 2008-9-4
	
	// 删除本记录
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
功能：判断一个外键是否需要修改
输入参数：
        fldGrp：域名组结构
	record：记录串，格式为"域1=域值|域2=域值|域3=域值|…"
返回值：
        1:      需要修改
        0：     不需要修改
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
功能：修改一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	record：要修改的实例的值，格式为"域1=域值|域2=域值|…|域N=域值"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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

	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
        
	// 读取修改的实例的域
	memset(&sUpdateRec,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sUpdateRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadRecFromRecStr from [%04d][%s] fail! return=[%d]\n",lenOfRecord,record,iRet);
		return(iRet);
	}

	// 检查修改的域是否合法
	for (i=0;i<sUpdateRec.fldNum;i++)
	{
		memset(&sTmpFldGrp,0,sizeof(TUnionObjectFldGrp));
		sTmpFldGrp.fldNum=1;
		strcpy(sTmpFldGrp.fldNameGrp[0],sUpdateRec.fldName[i]);
		// 检查是否关键字
		/* deleted by Wolfgang Wang, 2009/5/19
		iRet=UnionIsChildFldGrpOfFldGrp(&sTmpFldGrp,&(sObject.primaryKey));
		if (iRet == 1)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: primary key [%s] can not update!\n",sTmpFldGrp.fldNameGrp[0]);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldCanNotUpdate));
		}
		// 检查是否唯一值
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
                // 允许用户修改 外部关键字中的本地域组
                /*
		// 检查是否外部关键字中的本地域组
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
        
	// 检查修改的域值是否合法
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

	// 形成关键字的域名值串"域值1.域值2.域值3…"
	memset(caPrimaryKey,0,sizeof(caPrimaryKey));
	iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.primaryKey),primaryKey,strlen(primaryKey),caPrimaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for primary key [%s] fail! return=[%d]\n",primaryKey,iRet);
		return(iRet);
	}
        
        // add by chenliang. 2008-9-12
        // 获取现外部对象记录的引用
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
        
        // 删除 外部对象记录的引用
        memset(tmpCaDRecord, 0, sizeof(tmpCaDRecord));
        sprintf(tmpCaDRecord, "%s%s", primaryKey, caRecord);

        UnionProgramerLog("in UnionUpdateUniqueObjectRecord, object: [%s], foreignFldGrpNum: [%d], \ntmpCaDRecord: [%s].\n", idOfObject, sObject.foreignFldGrpNum, tmpCaDRecord);
        
        for (i=0;i<sObject.foreignFldGrpNum;i++)
        {
                if (UnionIsForeignKeyShouldUpdate(&(sObject.foreignFldGrp[i].localFldGrp), tmpCaDRecord) == 1)
                {
                        // 形成域名值串"域值1.域值2.域值3…"
		        memset(caKeyStr,0,sizeof(caKeyStr));
                        // modify by chenliang, 2008-10-08
                        iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),tmpCaDRecord,strlen(tmpCaDRecord),caKeyStr);
                        // modify by chenliang, 2008-10-08
                        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldValueStrFromFldGrp for foreign key [%d] fail! return=[%d]\n",i,iRet);
			        return(iRet);
		        }

                        // 由于外部键值有可能是唯一值，所以通过查找得到外部对象的存储位置得到外部对象的键值
		        iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
		        if (iRet < 0)
                        {
                                if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
			        {
				        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
				        return(iRet);
			        }

                                // 形成域名组串"域名1.域名2.域名3…"
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
                // 恢复 外部对象登记本记录的引用记录
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

                                // 由于外部键值有可能是唯一值，所以通过查找得到外部对象的存储位置得到外部对象的键值
                                iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
                                if (iRet < 0)
                                {
                                        if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
                                        {
                                                UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
                                                return(iRet);
                                        }
                                        // 形成域名组串"域名1.域名2.域名3…"
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
        // 删除 本对象实例内部引用文件中登记实例的引用记录
        for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		// 形成域名组串"域名1.域名2.域名3…"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// 形成域名值串"域值1.域值2.域值3…"
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
        // 在 新外部对象登记本记录的引用记录
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

                        // 由于外部键值有可能是唯一值，所以通过查找得到外部对象的存储位置得到外部对象的键值
                        iRet=UnionGetObjectRecStoreStationByPaimaryKey(sObject.foreignFldGrp[i].objectName,caKeyStr,caBuf,sizeof(caBuf));
                        if (iRet < 0)
                        {
                                if (iRet != errCodeObjectMDL_ObjectRecordNotExist)
                                {
                                        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionGetObjectRecStoreStationByPaimaryKey for [%s] fail! return=[%d]\n",caKeyStr,iRet);
                                        return(iRet);
                                }
                                // 形成域名组串"域名1.域名2.域名3…"
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
        
        // 修改本对象实例内部引用文件中登记实例的引用记录
        // 在本对象实例内部引用文件中登记实例的引用记录
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
                if (UnionIsForeignKeyShouldUpdate(&(sObject.foreignFldGrp[i].localFldGrp), tmpCaDRecord) == 1)
                {
		        // 形成域名组串"域名1.域名2.域名3…"
		        memset(caNameStr,0,sizeof(caNameStr));
		        iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.foreignFldGrp[i].localFldGrp),caNameStr);
		        if (iRet < 0)
		        {
			        UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			        return(iRet);
		        }
		        // 形成域名值串"域值1.域值2.域值3…"
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
功能：根据关键字查询一个对象的实例（只选择指定的域）
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
	fldGrp		要选择的域的清单，每个域之间以,分隔开
	lenOfFldGrp	域清单的长度
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
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
功能：根据关键字读取指定的域
输入参数：
	idOfObject：	对象ID
	primaryKey：	对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	fldName		域名
	sizeOfBuf	接受查询域的大小
输出参数：
	fldValue	域值
返回值：
	>=0：		成功，返回域值的长度
	<0：		失败，错误码
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
功能：根据关键字读取指定的整型域
输入参数：
	idOfObject：	对象ID
	primaryKey：	对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	fldName		域名
输出参数：
	fldValue	域值
返回值：
	>=0：		成功，返回域值的长度
	<0：		失败，错误码
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
功能：根据关键字查询一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
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
	
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 形成关键字的域名值串"域值1.域值2.域值3…"
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
功能：根据唯一值查询一个对象的实例
输入参数：
	idOfObject：对象ID
	uniqueKey：对象实例的唯一值，格式为"唯一值域1=域值|唯一值域2=域值|…唯一值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
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
	
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 形成域名组结构
	memset(&sUniqueGrp,0,sizeof(TUnionObjectFldGrp));	// Mary add, 20081031
	iRet=UnionFormFldGrpFromRecordStr(uniqueKey,strlen(uniqueKey),&sUniqueGrp);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFormFldGrpFromRecordStr for [%s] fail! return=[%d]\n",uniqueKey,iRet);
		return(iRet);
	}
	
	for (i=0;i<sObject.uniqueFldGrpNum;i++)
	{
		// 检查域名组是否已定义
		iRet=UnionFldGrpIsRepeat(&sUniqueGrp,&(sObject.uniqueFldGrp[i]));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFldGrpIsRepeat for unique key [%s] and unique group [%d] in object fail! return=[%d]\n",uniqueKey,i,iRet);
			return(iRet);
		}
		if (iRet == 0)
			continue;
		
		// 形成域名组串"域名1.域名2.域名3…"
		memset(caNameStr,0,sizeof(caNameStr));
		iRet=UnionFormFieldNameStrFromFldGrp(&(sObject.uniqueFldGrp[i]),caNameStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFormFieldNameStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// 形成域名值串"域值1.域值2.域值3…"
		memset(caValueStr,0,sizeof(caValueStr));
		iRet=UnionFormFieldValueStrFromFldGrp(&(sObject.uniqueFldGrp[i]),uniqueKey,strlen(uniqueKey),caValueStr);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionFormFieldValueStrFromFldGrp for unique key [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		// 得到实例的存储位置
		iRet=UnionGetObjectRecStoreStationByUniqueKey(idOfObject,caNameStr,caValueStr,caBuf,sizeof(caBuf));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionGetObjectRecStoreStationByUniqueKey for [%s] [%s] [%s] fail! return=[%d]\n",idOfObject,caNameStr,caValueStr,iRet);
			return(iRet);
		}
		break;
	}
	
	if (i == sObject.uniqueFldGrpNum)	// 实例未找到
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: record of object [%s] is not found! unique key=[%s] return=[%d]\n",idOfObject,uniqueKey,iRet);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
	}
	
	// 读取域值文件中的记录
	// 2008/10/22，王纯军修改
	UnionPatchMainDirToObjectValueFileName(caBuf,caFileName);
	iRet=UnionReadRecStrFromFile(caFileName,record,sizeOfRecord);
	// 2008/10/22，王纯军修改结束
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionReadRecStrFromFile from [%s] fail! return=[%d]\n",caFileName,iRet);
		return(iRet);
	}
	
	return(iRet);
}

/*
功能：批量删除一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：删除条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	无
返回值：
	>0：成功，返回被删除的实例数目
	<0：失败，错误码
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
	
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
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
			// 读取文件中的一行，已经去掉了注释行
			memset(caBuf,0,sizeof(caBuf));
			iLen=UnionReadNextRecFromFile(hFileHdl,caBuf,sizeof(caBuf));
			if (iLen < 0)
			{
				UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadNextRecFromFile fail! return=[%d]\n",iLen);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iLen);
			}
			if (iLen == 0)	// 读文件结束
				break;
		
			// 得到值存储文件名称
			memset(caValueFile,0,sizeof(caValueFile));
			iRet=UnionReadRecFldFromRecStr(caBuf,iLen,"fldGrp",caValueFile,sizeof(caValueFile));
			if (iRet < 0)
			{
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
				return(iRet);
			}
			// 读出记录的值
			memset(caRecord,0,sizeof(caRecord));
                        memset(caTmpValueFile, 0, sizeof(caTmpValueFile));

			// UnionGetObjectRecValueFileName(idOfObject,caValueFile,caFileName);
                        // 为了避免将原来的文件名覆盖掉
                        UnionGetObjectRecValueFileName(idOfObject,caValueFile,caTmpValueFile);

			// iRecLen=UnionReadRecStrFromFile(caFileName,caRecord,sizeof(caRecord));
                        iRecLen=UnionReadRecStrFromFile(caTmpValueFile,caRecord,sizeof(caRecord));
                        if (iRecLen < 0)
			{
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionReadRecStrFromFile fail! return=[%d]\n",iRecLen);
				return(iRecLen);
			}
			// 判断该记录是否符合条件
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
		// 由于删除一个实例时要删除键值文件中的记录，所以先关闭键值文件，删除完后再重新打开
		UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
		if (iDeleteFlag == 0)
			break;
		
		// 形成域名值串"键值域1=域值|键值域2=域值|键值域3=域值|…"
		memset(caPrimaryKey,0,sizeof(caPrimaryKey));
		iRet=UnionFormFieldRecordStrFromFldGrp(&(sObject.primaryKey),caRecord,iRecLen,caPrimaryKey,sizeof(caPrimaryKey));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionFormFieldRecordStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
			return(iRet);
		}
		// 删除该记录
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
功能：批量修改一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：修改条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
	record：要修改的实例的值，格式为"域1=域值|域2=域值|…|域N=域值|"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	>=0：成功，返回被修改的实例数目
	<0：失败，错误码
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
	
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
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
		// 读取文件中的一行，已经去掉了注释行
		memset(caBuf,0,sizeof(caBuf));
		iLen=UnionReadNextRecFromFile(hFileHdl,caBuf,sizeof(caBuf));
		if (iLen < 0)
		{
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionReadNextRecFromFile fail! return=[%d]\n",iLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iLen);
		}
		if (iLen == 0)	// 读文件结束
			break;
		
		// 得到值存储文件名称
		memset(caValueFile,0,sizeof(caValueFile));
		iRet=UnionReadRecFldFromRecStr(caBuf,iLen,"fldGrp",caValueFile,sizeof(caValueFile));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
			return(iRet);
		}
		// 读出记录的值
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
		// 判断该记录是否符合条件
		iRet=UnionIsRecStrFitSpecConditon(caRecord,iRecLen,condition,strlen(condition));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionIsRecStrFitSpecConditon fail! return=[%d]\n",iRet);
			return(iRet);
		}
		if (iRet == 0)
			continue;
		
		// 形成域名值串"键值域1=域值|键值域2=域值|键值域3=域值|…"
		memset(caPrimaryKey,0,sizeof(caPrimaryKey));
		iRet=UnionFormFieldRecordStrFromFldGrp(&(sObject.primaryKey),caRecord,iRecLen,caPrimaryKey,sizeof(caPrimaryKey));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionFormFieldRecordStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
			return(iRet);
		}
		// 修改该记录
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
功能：批量查询一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	fileName：存储了查询出来的实例的值的文件名
返回值：
	>=0：成功，返回查询出的记录的数目
	<0：失败，错误码
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
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
        //UnionLog("in UnionBatchSelectObjectRecord : 33333333333333333 ~~~~~\n");
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
        //UnionLog("in UnionBatchSelectObjectRecord : 4444444444444444444 ~~~~~\n");
	// 检查对象的键值文件是否存在
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
        // 用于清空临时文件原来的数据，以免在界面显示混乱
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
		// 读取文件中的一行，已经去掉了注释行
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
		if (iLen == 0)	// 读文件结束
			break;
		
		// 得到值存储文件名称
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
		// 读出记录的值
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
		// 判断该记录是否符合条件
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
		if (iRet == 0)			// 不符合条件
			continue;

                // UnionLog("in UnionBatchSelectObjectRecord, current record: [%s] is fit [%s], iSelectNum is: [%d].\n", caRecord, condition, iSelectNum);
		
		// 写文件
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
功能：拼装一个关键字条件串
输入参数：
	idOfObject：	要读的对象ID	
	primaryKeyValueList	关键值字串,两个域值以.分开
	lenOfValueList		关键值字串长度
	sizeOfBuf	关键字清单的大小
输出参数：
	recStr		关键字条件串
返回值：
	0：	关键字条件串的长度
	<0：	失败，返回错误码
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
功能：
	从一个记录串拼装关键字
输入参数：
	idOfObject：		对象ID
	record：		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：		record的长度
	sizeOfPrimaryKey	关键值串缓冲大小
输出参数：
	primaryKey		关键值串，以.分开
返回值：
	0：成功
	<0：失败，错误码
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
	
	// 读取对象定义
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
