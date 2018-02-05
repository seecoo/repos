//	Author: ChenJiaMei
//	Date: 2008-8-7

#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectSql.h"
#include "unionComplexDBObjectDef.h"
#include "unionComplexDBObjectChildDef.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBUniqueKey.h"
#include "unionComplexDBObject.h"

/*
���ܣ�����Ԥ�ȶ����SQL�ļ�����һ������
���������
	idOfObject������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObject(TUnionIDOfObject idOfObject)
{
	int	iRet,i;
	TUnionObject		sObject;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCreateObject:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ���������ļ��Ƿ����
	iRet=UnionExistsObjectDef(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObject:: UnionExistsObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionCreateObject:: object [%s] defined file already exists!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// �Ӷ��󴴽��ļ��ж�ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionCreateObjectDefFromCreateSqlFile(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObject:: UnionCreateObjectDefFromCreateSqlFile fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	iRet=UnionCreateObjectAnyway(&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObject:: UnionCreateObjectAnyway fail! return=[%d]\n",iRet);
		return(iRet);
	}
	return(iRet);
}

/*
���ܣ�ɾ��һ������
���������
	idOfObject�������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionDropObject(TUnionIDOfObject idOfObject)
{
	int	iRet,i;
	char	caBuf[512];
	TUnionObject		sObject;
	TUnionChildObjectGrp	sChildObjGrp;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionDropObject:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ���������ļ��Ƿ����
	iRet=UnionExistsObjectDef(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionExistsObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionDropObject:: object [%s] defined file does not exists!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// ��������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionReadObjectDef for [%s] fail! return=[%d]\n",idOfObject,iRet);
		return(iRet);
	}
	
	// ����Ƿ��ж���ʵ��
	iRet=UnionGetObjectRecordNumber(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionGetObjectRecordNumber fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObject:: object [%s] has [%d] record already!\n",idOfObject,iRet);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_RecordExistAlready));
	}
	
	// ����Ƿ������������������˱��������
	iRet=UnionExistForeignObjectInfo(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionExistForeignObjectInfo fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObject:: object [%s] has been referenced by [%d] foreign objects already!\n",idOfObject,iRet);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectIsReferenced));
	}
	
	// ɾ�������¼�Ĵ洢Ŀ¼
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecMainDirName(idOfObject,caBuf);
	iRet=UnionRemoveDir(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionRemoveDir for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// ɾ�������������������������ù�ϵ
	memset(&sChildObjGrp,0,sizeof(TUnionChildObjectGrp));
	sChildObjGrp.childNum=sObject.foreignFldGrpNum;
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		strcpy(sChildObjGrp.childObjectGrp[i].objectName,idOfObject);
		memcpy(&(sChildObjGrp.childObjectGrp[i].localFldGrp),&(sObject.foreignFldGrp[i].foreignFldGrp),sizeof(TUnionObjectFldGrp));
		memcpy(&(sChildObjGrp.childObjectGrp[i].foreignFldGrp),&(sObject.foreignFldGrp[i].localFldGrp),sizeof(TUnionObjectFldGrp));
		iRet=UnionCancelRegisterOfForeignObjectInfo(sObject.foreignFldGrp[i].objectName,sChildObjGrp);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObject:: UnionCancelRegisterOfForeignObjectInfo fail! return=[%d]\n",iRet);
			return(iRet);
		}
	}
	
	// ɾ��������Ķ����ļ�
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// ɾ����������ⲿ�ؼ��ֶ����嵥�ļ�
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectChildrenDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ�
	����һ����������Ѵ��ڣ���ɾ��
���������
	prec	����
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObjectAnyway(PUnionObject prec)
{
	int	iRet,i;
	char	caBuf[512];
	TUnionObject		sObject;
	TUnionChildObjectGrp	sChildObjGrp;
	
	if (prec == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	if ((iRet = UnionBatchDeleteObjectRecord(prec->name,"")) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionBatchDeleteObjectRecord! ret = %d!\n",iRet);
		//return(iRet);
	}
	if ((iRet = UnionDropObjectAnyway(prec->name)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionDropObjectAnyway! ret = %d!\n",iRet);
		//return(iRet);
	}
	
	// ���������Ƿ�Ϸ�
	iRet=UnionIsValidObjectDef(*prec);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionIsValidObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: object [%s] is invalid!\n",prec->name);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectIsInvalid));
	}
	
	// ���ⲿ����Ǽ��������
	memset(&sChildObjGrp,0,sizeof(TUnionChildObjectGrp));
	// sChildObjGrp.childNum=prec->foreignFldGrpNum;
        sChildObjGrp.childNum=1;       // modify by chenliang, 2008-09-11

	for (i=0;i<prec->foreignFldGrpNum;i++)
	{
		strcpy(sChildObjGrp.childObjectGrp[i].objectName,prec->name);
		memcpy(&(sChildObjGrp.childObjectGrp[i].localFldGrp),&(prec->foreignFldGrp[i].foreignFldGrp),sizeof(TUnionObjectFldGrp));
		memcpy(&(sChildObjGrp.childObjectGrp[i].foreignFldGrp),&(prec->foreignFldGrp[i].localFldGrp),sizeof(TUnionObjectFldGrp));
		iRet=UnionRegisterForeignObjectInfo(prec->foreignFldGrp[i].objectName,sChildObjGrp);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionRegisterForeignObjectInfo fail! return=[%d]\n",iRet);
			return(iRet);
		}
	}
	
	// ����������Ĵ洢Ŀ¼
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefDirName(prec->name,caBuf);
	if (!UnionExistsDir(caBuf))
	{
		iRet=UnionCreateDir(caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateDir for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
	}
	
	// �����������ļ�
	iRet=UnionStoreObjectDefAnyway(prec);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionStoreObjectDefAnyway fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// �����ⲿ���ö���Ǽ��ļ�
	iRet=UnionCreateObjectForeignObjDefFile(prec->name);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateObjectForeignObjDefFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// ��������ʵ���Ĵ洢Ŀ¼
	memset(caBuf,0,sizeof(caBuf));
	UnionGetAllObjectRecDirName(caBuf);
	if (!UnionExistsDir(caBuf))
	{
		iRet=UnionCreateDir(caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateDir for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
	}
	
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecMainDirName(prec->name,caBuf);
	if (!UnionExistsDir(caBuf))
	{
		iRet=UnionCreateDir(caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateDir for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
	}
	
	// ������ֵ�ļ���Ψһֵ�ļ�
	iRet=UnionCreateObjectPrimaryKeyIndexFile(prec->name);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateObjectPrimaryKeyIndexFile for [%s] fail! return=[%d]\n",prec->name,iRet);
		return(iRet);
	}
	
	for (i=0;i<prec->uniqueFldGrpNum;i++)
	{
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(prec->uniqueFldGrp[i]),caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionFormFieldNameStrFromFldGrp for [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}

		iRet=UnionCreateObjectUniqueKeyIndexFile(prec->name,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateObjectUniqueKeyIndexFile for [%s] fail! return=[%d]\n",prec->name,iRet);
			return(iRet);
		}
	}
	
	return(0);
}

/*
���ܣ�ɾ��һ������
���������
	idOfObject�������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionDropObjectAnyway(TUnionIDOfObject idOfObject)
{
	int	iRet,i;
	char	caBuf[512];
	TUnionObject		sObject;
	TUnionChildObjectGrp	sChildObjGrp;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ���������ļ��Ƿ����
	iRet=UnionExistsObjectDef(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionExistsObjectDef fail! return=[%d]\n",iRet);
		//return(iRet);
	}
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: object [%s] defined file does not exists!\n",idOfObject);
		//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// ��������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionReadObjectDef for [%s] fail! return=[%d]\n",idOfObject,iRet);
		//return(iRet);
	}
	
	// ����Ƿ��ж���ʵ��
	iRet=UnionGetObjectRecordNumber(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionGetObjectRecordNumber fail! return=[%d]\n",iRet);
		//return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: object [%s] has [%d] record already!\n",idOfObject,iRet);
		//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_RecordExistAlready));
	}
	
	// ����Ƿ������������������˱��������
	iRet=UnionExistForeignObjectInfo(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionExistForeignObjectInfo fail! return=[%d]\n",iRet);
		//return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: object [%s] has been referenced by [%d] foreign objects already!\n",idOfObject,iRet);
		//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectIsReferenced));
	}
	
	// ɾ�������¼�Ĵ洢Ŀ¼
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecMainDirName(idOfObject,caBuf);
	iRet=UnionRemoveDir(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionRemoveDir for [%s] fail! return=[%d]\n",caBuf,iRet);
		//return(iRet);
	}
	
	// ɾ�������������������������ù�ϵ
	memset(&sChildObjGrp,0,sizeof(TUnionChildObjectGrp));
	sChildObjGrp.childNum=sObject.foreignFldGrpNum;
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		strcpy(sChildObjGrp.childObjectGrp[i].objectName,idOfObject);
		memcpy(&(sChildObjGrp.childObjectGrp[i].localFldGrp),&(sObject.foreignFldGrp[i].foreignFldGrp),sizeof(TUnionObjectFldGrp));
		memcpy(&(sChildObjGrp.childObjectGrp[i].foreignFldGrp),&(sObject.foreignFldGrp[i].localFldGrp),sizeof(TUnionObjectFldGrp));
		iRet=UnionCancelRegisterOfForeignObjectInfo(sObject.foreignFldGrp[i].objectName,sChildObjGrp);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObject:: UnionCancelRegisterOfForeignObjectInfo fail! return=[%d]\n",iRet);
			//return(iRet);
		}
	}
	
	// ɾ��������Ķ����ļ�
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		//return(iRet);
	}
	
	// ɾ����������ⲿ�ؼ��ֶ����嵥�ļ�
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectChildrenDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		//return(iRet);
	}
	
	return(0);
}
