//	Author: ChenJiaMei
//	Date: 2008-8-7

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
#include "unionComplexDBRecordParents.h"

/*
���ܣ����һ������ʵ�����ڲ������ļ��Ƿ����
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
int UnionExistsObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ������ʵ�����ڲ������ļ��Ƿ����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfObject,primaryKey,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
���ܣ�����һ������ʵ�����ڲ������ļ�
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
int UnionCreateObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ʵ�����ڲ������ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfObject,primaryKey,caFileName);
	
	// ������ʵ�����ڲ������ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectRecParentsFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// ��������ʵ�����ڲ������ļ�
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectRecParentsFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
���ܣ���һ������ʵ�����ڲ������ļ���ɾ��һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	localFldGrp��������ı����������������ʽΪ"��������1.��������2.��������3����������N"��
		��������������˳���뱾�������ļ��ж���ı�������˳��һ��
	idOfForeignObject���ⲿ�����ID
	foreignPrimaryKey�����������õ��ⲿ����ʵ���Ĺؼ�����ֵ��
		��ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDelObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0;
	
	if (idOfLocalObject == NULL || localPrimaryKey == NULL || localFldGrp == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ʵ�����ڲ������ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfLocalObject,localPrimaryKey,caFileName);
	
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=0;
	iRet=UnionPutRecFldIntoRecStr("localfldGrp",localFldGrp,strlen(localFldGrp),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionPutRecFldIntoRecStr for local field group [%s] fail!\n",localFldGrp);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignObject",idOfForeignObject,strlen(idOfForeignObject),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign object name [%s] fail!\n",idOfForeignObject);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignPrimaryKey",foreignPrimaryKey,strlen(foreignPrimaryKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign field value [%s] fail!\n",foreignPrimaryKey);
		return(iRet);
	}
	iRecLen += iRet;

        UnionProgramerLog("in UnionDelObjectRecParentsFile, try to delete file: [%s] record: [%s].\n", caFileName, caRecord);
        
	// iRet = UnionDeleteRecStrFromFile(caFileName, caRecord, iRecLen);
        iRet = UnionDeleteRecStrFromFileByFld(caFileName, caRecord, iRecLen, "foreignPrimaryKey");
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ���һ������ʵ�����ڲ������ļ�������һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	localFldGrp��������ı����������������ʽΪ"��������1.��������2.��������3����������N"��
		��������������˳���뱾�������ļ��ж���ı�������˳��һ��
	idOfForeignObject���ⲿ�����ID
	foreignPrimaryKey�����������õ��ⲿ����ʵ���Ĺؼ�����ֵ��
		��ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0;
	
	if (idOfLocalObject == NULL || localPrimaryKey == NULL || localFldGrp == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ʵ�����ڲ������ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfLocalObject,localPrimaryKey,caFileName);
	
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=0;
	iRet=UnionPutRecFldIntoRecStr("localfldGrp",localFldGrp,strlen(localFldGrp),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionPutRecFldIntoRecStr for local field group [%s] fail!\n",localFldGrp);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignObject",idOfForeignObject,strlen(idOfForeignObject),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign object name [%s] fail!\n",idOfForeignObject);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignPrimaryKey",foreignPrimaryKey,strlen(foreignPrimaryKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign field value [%s] fail!\n",foreignPrimaryKey);
		return(iRet);
	}
	iRecLen += iRet;

        UnionProgramerLog("in UnionAddObjectRecParentsFile, try to add record: [%s] to file: [%s].\n", caRecord, caFileName);
        
	iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ�ȡ��һ��ʵ�����õĶ���ʵ���Ĺؼ���
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	localFldGrp�������������ƣ���ʽΪ"��1������.��2������.��3�����ơ���N������"��
		�������˳����������ļ����ⲿ�ؼ��ֵı�����������˳��һ��
	idOfForeignObject���ⲿ���������
	sizeOfPrimaryKey��foreignPrimaryKey�Ĵ洢�ռ��С
���������
	foreignPrimaryKey�����������õ��ⲿ����ʵ���Ĺؼ�����ֵ��
		��ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
����ֵ��
	>=0���ɹ�������foreignPrimaryKey�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionObtainForeignObjectRecPrimaryKey(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey,int sizeOfPrimaryKey)
{
	char	caFileName[512],caRecord[1024],caBuf[1024];
	int	iRet,iRecLen=0,iBufLen=0;
	TUnionRecFileHDL	hFileHdl;

	if (idOfLocalObject == NULL || localPrimaryKey == NULL || localFldGrp == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL || sizeOfPrimaryKey <= 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ʵ�����ڲ������ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfLocalObject,localPrimaryKey,caFileName);
	
	// ������ʵ�����ڲ������ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	memset(caBuf,0,sizeof(caBuf));
	iBufLen=0;
	iRet=UnionPutRecFldIntoRecStr("localfldGrp",localFldGrp,strlen(localFldGrp),&caBuf[iBufLen],sizeof(caBuf)-iBufLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionPutRecFldIntoRecStr for local field group [%s] fail!\n",localFldGrp);
		return(iRet);
	}
	iBufLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignObject",idOfForeignObject,strlen(idOfForeignObject),&caBuf[iBufLen],sizeof(caBuf)-iBufLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionPutRecFldIntoRecStr for foreign object name [%s] fail!\n",idOfForeignObject);
		return(iRet);
	}
	iBufLen += iRet;
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// ���ļ�����
			break;
		
		if (strncmp(caRecord,caBuf,iBufLen) == 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			iRet=UnionReadRecFldFromRecStr(caRecord,iRecLen,"foreignPrimaryKey",foreignPrimaryKey,sizeOfPrimaryKey);
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionReadRecFldFromRecStr for foreignPrimaryKey fail! return=[%d]\n",iRet);
				return(iRet);
			}
			return(0);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: idOfForeignObject [%s] is not found!\n",idOfForeignObject);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
}
