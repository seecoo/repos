//	Author: ChenJiaMei
//	Date: 2008-8-6

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
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBRecordValue.h"
#include "unionRec0.h"
#include "unionDataImageInMemory.h"

/*
���ܣ����һ������ʵ����ֵ�ļ��Ƿ����
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectRecValueFile(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectRecValueFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ������ʵ����ֵ�ļ��Ƿ����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
���ܣ�����һ������ʵ����ֵ�ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	record���¼�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord���¼�¼�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL || primaryKey == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecordValue:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��������ʵ���Ĵ洢Ŀ¼
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecDirName(idOfObject,primaryKey,caFileName);
	if (!UnionExistsDir(caFileName))
	{
		iRet=UnionCreateDir(caFileName);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecordValue:: UnionCreateDir for [%s] fail! return=[%d]\n",caFileName,iRet);
			return(iRet);
		}
	}
	
	// ��ö���ʵ����ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// ������ʵ����ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecordValue:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	iRet=UnionAppendRecStrToFile(caFileName,record,lenOfRecord);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecordValue:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",record,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ����ݹؼ���ɾ��һ������ʵ����ֵ�ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];
	int	iRet;

	// ��ӳ����ɾ������,2009/1/8,����������
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{
		if ((iRet = UnionDeleteRecDataImageInMemory(idOfObject,primaryKey)) < 0)
		{
			UnionAuditLog("in UnionDeleteObjectRecordValue:: UnionDeleteRecDataImageInMemory [%s]\n",idOfObject,primaryKey);
			// ע��,����ʱ������,��Ϊ���ݿ�����ӳ����������
		}
	}
	// 2009/1/8,���������ӽ���
	
	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteObjectRecordValue:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ʵ����ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// ������ʵ����ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecordValue:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	iRet=UnionDeleteRecFile(caFileName);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecordValue:: UnionDeleteRecFile fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ��޸�һ������ʵ����ֵ�ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	record��Ҫ�޸ĵļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��Ҫ�޸ĵļ�¼�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionUpdateObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	char	caFileName[512],caRecStr[4096],caTmpVar[4096];
	int	iRet,iRecStrLen,i;
	TUnionRec	sOriRec,sNewRec;

	if (idOfObject == NULL || primaryKey == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ʵ����ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// ������ʵ����ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	// ��ԭʼ��¼
	memset(caRecStr,0,sizeof(caRecStr));
	iRecStrLen=UnionReadRecStrFromFile(caFileName,caRecStr,sizeof(caRecStr));
	if (iRecStrLen < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionReadRecStrFromFile fail! return=[%d]\n",iRecStrLen);
		return(iRecStrLen);
	}
	
	// ��ԭ��������뵽�ṹ��
	memset(&sOriRec,0,sizeof(sOriRec));
	if ((iRet = UnionReadRecFromRecStr(caRecStr,iRecStrLen,&sOriRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionReadRecFromRecStr from [%d] [%s]\n",iRecStrLen,caRecStr);
		return(iRet);
	}
	
	// ��Ҫ���ĵ�����뵽�ṹ��
	memset(&sNewRec,0,sizeof(sNewRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sNewRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecord,record);
		return(iRet);
	}
		
	// �����ĵ���д�뵽����
	memset(caRecStr,0,sizeof(caRecStr));	
	// deleted by Wolfgang Wang,2009/1/9
	/*
	if ((iRecStrLen = UnionPutRecIntoRecStr(&sNewRec,caRecStr,sizeof(caRecStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionPutRecIntoRecStr fail! return=[%d]\n",iRet);
		return(iRet);
	}
	*/
	
	// added by wolfgang wang 2009/1/9
	iRecStrLen = 0;
	for (i = 0; i < sNewRec.fldNum; i++)
	{
		//if (UnionIsPrimaryKeyFld(idOfObject, sNewRec.fldName[i]) == 1)
		if ((UnionIsPrimaryKeyFld(idOfObject, sNewRec.fldName[i]) == 1) || (UnionIsUniqueFld(idOfObject,sNewRec.fldName[i]) > 0))
			continue;
		if ((iRet = UnionPutRecFldIntoRecStr(sNewRec.fldName[i],sNewRec.fldValue[i],strlen(sNewRec.fldValue[i]),caRecStr+iRecStrLen,sizeof(caRecStr)-iRecStrLen)) < 0)
		{
			UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionPutRecFldIntoRecStr [%s]! return=[%d]\n",sNewRec.fldName[i],iRet);
			return(iRet);
		}
		iRecStrLen += iRet;
	}
	// end of addition of 2009/1/9
	
	// ��δ�ĵ������뵽��¼����
	for (i = 0; i < sOriRec.fldNum; i++)
	{
		if ((iRet = UnionReadRecFldFromRecStr(caRecStr,iRecStrLen,sOriRec.fldName[i],caTmpVar,sizeof(caTmpVar))) >= 0)
			continue;	// �Ǹ����˵���
		if ((iRet = UnionPutRecFldIntoRecStr(sOriRec.fldName[i],sOriRec.fldValue[i],strlen(sOriRec.fldValue[i]),caRecStr+iRecStrLen,sizeof(caRecStr)-iRecStrLen)) < 0)
		{
			UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionPutRecFldIntoRecStr [%s]! return=[%d]\n",sOriRec.fldName[i],iRet);
			return(iRet);
		}
		iRecStrLen += iRet;
	}
	
	if ((iRet = UnionWriteRecStrIntoFile(caFileName,caRecStr,iRecStrLen)) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionWriteRecStrIntoFile [%s] to [%s] fail! return=[%d]\n",caRecStr,caFileName,iRet);
		return(iRet);
	}
	
	// �޸�ӳ�����е�����,2009/1/8,����������
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{
		if ((iRet = UnionModifyRecDataImageInMemory(idOfObject,primaryKey,caRecStr,iRecStrLen)) < 0)
		{
			UnionAuditLog("in UnionUpdateObjectRecordValue:: UnionModifyRecDataImageInMemory [%s]\n",idOfObject,primaryKey);
			// ע��,����ʱ������,��Ϊ���ݿ��Բ���ӳ�����д���
			if (UnionExistRecDataImageInMemory(idOfObject,primaryKey))	// ӳ�����д�������
				UnionDeleteRecDataImageInMemory(idOfObject,primaryKey);	// �޸���ӳ�����е�����ʧ��,ɾ����ӳ�����е�����
		}
	}
	// 2009/1/8,���������ӽ���
	return(0);
}

/*
���ܣ����ݹؼ��ֲ�ѯһ������ʵ����ֵ�ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	sizeOfRecord��record�Ĵ洢�ռ��С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	char	caFileName[512];
	int	iRet;
	int	ret;
	
	if (idOfObject == NULL || primaryKey == NULL || record == NULL || sizeOfRecord <= 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordValue:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// �ȴ�ӳ�����ж�ȡ����,2009/1/8,����������
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{
		if ((iRet = UnionReadRecDataImageInMemory(idOfObject,primaryKey,record,sizeOfRecord)) >= 0)
			return(iRet);
	}
	// 2009/1/8,���������ӽ���

	// ��ö���ʵ����ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// ������ʵ����ֵ�ļ��Ƿ����
	iRet = UnionExistsFile(caFileName);
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordValue:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	iRet=UnionReadRecStrFromFile(caFileName,record,sizeOfRecord);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordValue:: UnionReadRecStrFromFile fail! return=[%d]\n",iRet);
		return(iRet);
	}

	// ���ص�ӳ����,2009/1/8,����������
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{	
		if (UnionExistRecDataImageInMemory(idOfObject,primaryKey))	// ӳ�����д�������
		{
			if ((ret = UnionModifyRecDataImageInMemory(idOfObject,primaryKey,record,iRet)) < 0)	// �޸�ʧ��
			{
				UnionAuditLog("in UnionSelectObjectRecordValue:: UnionModifyRecDataImageInMemory fail! return=[%d]\n",ret);
				UnionDeleteRecDataImageInMemory(idOfObject,primaryKey);	// �޸���ӳ�����е�����ʧ��,ɾ����ӳ�����е�����
			}
		}
		else
		{
			if ((ret = UnionInsertRecDataImageInMemory(idOfObject,primaryKey,record,iRet)) < 0)	// ����ʧ��
				UnionAuditLog("in UnionSelectObjectRecordValue:: UnionInsertRecDataImageInMemory fail! return=[%d]\n",ret);
		}
	}
	// 2009/1/8,���������ӽ���
	return(iRet);
}
