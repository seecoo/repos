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
#include "unionComplexDBCommon.h"
#include "unionComplexDBUniqueKey.h"

/*
���ܣ����һ�������ĳΨһֵ�����ļ��Ƿ����
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName)
{
	char	caFileName[512];

	if (idOfObject == NULL || fldGrpName == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectUniqueKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// �������ĳΨһֵ�����ļ��Ƿ����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
���ܣ�����һ�������ĳΨһֵ�����ļ�
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionCreateObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL || fldGrpName == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectUniqueKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö����ĳΨһֵ�����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// �������ĳΨһֵ�����ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectUniqueKeyIndexFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// ���������ĳΨһֵ�����ļ�
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectUniqueKeyIndexFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
���ܣ���һ�������ĳΨһֵ�����ļ�������һ���¼�¼��Ψһֵ
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey���¼�¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	storePosition���¼�¼�Ĵ洢λ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL || storePosition == NULL)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö����ĳΨһֵ�����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// �������ĳΨһֵ�����ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	memset(caRecord,0,sizeof(caRecord));
	iRet=iRet=UnionPutRecFldIntoRecStr("fldGrp",uniqueKey,strlen(uniqueKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: UnionPutRecFldIntoRecStr for uniqueKey [%s] fail!\n",uniqueKey);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=iRet=UnionPutRecFldIntoRecStr("fileName",storePosition,strlen(storePosition),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: UnionPutRecFldIntoRecStr for storePosition [%s] fail!\n",storePosition);
		return(iRet);
	}
	iRecLen += iRet;
	
	iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ���һ�������ĳΨһֵ�����ļ���ɾ��һ����¼��Ψһֵ
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey��Ҫɾ����¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey)
{
	char	caFileName[512],caBuf[1024];
	int	iRet;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö����ĳΨһֵ�����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// �������ĳΨһֵ�����ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// �������ĳΨһֵ�����ļ����Ƿ��иü�¼
	iRet=UnionObjectRecUniqueKeyIsRepeat(idOfObject,fldGrpName,uniqueKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: UnionObjectRecUniqueKeyIsRepeat fail! reutnr=[%d]\n",iRet);
		return(iRet);
	}
	else if (iRet == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: [%s] does not exists!\n",uniqueKey);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	memset(caBuf,0,sizeof(caBuf));
	sprintf(caBuf,"fldGrp=%s",uniqueKey);
	//iRet = UnionDeleteRecStrFromFile(caFileName,caBuf,strlen(caBuf));	// Mary delete, 20081105
	iRet = UnionDeleteRecStrFromFileByFld(caFileName,caBuf,strlen(caBuf),"fldGrp");	// Mary add, 20081105
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: UnionDeleteRecStrFromFileByFld for unique key [%s] fail! return=[%d]\n",uniqueKey,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ����ݶ���ʵ����Ψһֵ���ʵ���Ĵ洢λ��
���������
	idOfObject������ID
	fldGrpName��Ψһֵ���������ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey����¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	sizeofStorePosition��storePosition�Ĵ洢�ռ��С
���������
	storePosition����¼�Ĵ洢λ��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionGetObjectRecStoreStationByUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition,int sizeofStorePosition)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0,iLen;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL || storePosition == NULL || sizeofStorePosition <= 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// ��ö����ĳΨһֵ�����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// �������ĳΨһֵ�����ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// ���ļ�����
			break;
		
		// Mary add begin, 20081125
		for (iLen=0;iLen<iRecLen;iLen++)
		{
			if (caRecord[iLen+7] == '|')
				break;
		}
		if (iLen != strlen(uniqueKey))
			continue;			
		// Mary add end, 20081125
		
		if (strncmp(&caRecord[7],uniqueKey,strlen(uniqueKey)) == 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			iRet=UnionReadRecFldFromRecStr(caRecord,iRecLen,"fileName",storePosition,sizeofStorePosition);
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
				return(iRet);
			}
			return(0);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: uniqueKey [%s] is not found!\n",uniqueKey);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
}

/*
���ܣ��ж϶���ʵ����Ψһֵ�Ƿ��ظ�
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey����¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ�������
*/
int UnionObjectRecUniqueKeyIsRepeat(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0,iLen;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL)
	{
		UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö����ĳΨһֵ�����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// �������ĳΨһֵ�����ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// ���ļ�����
			break;
		
		// Mary add begin, 20081201
		for (iLen=0;iLen<iRecLen;iLen++)
		{
			if (caRecord[iLen+7] == '|')
				break;
		}
		if (iLen != strlen(uniqueKey))
			continue;			
		// Mary add end, 20081201
		
		if (strncmp(&caRecord[7],uniqueKey,strlen(uniqueKey)) == 0)
		{
			UnionAuditLog("in UnionObjectRecUniqueKeyIsRepeat:: uniqueKey is repeat! record=[%s]\n",caRecord);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(1);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	return(0);
}
