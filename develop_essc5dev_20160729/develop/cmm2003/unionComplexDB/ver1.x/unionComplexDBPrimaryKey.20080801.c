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
#include "unionComplexDBPrimaryKey.h"

/*
���ܣ����һ������ļ�ֵ�ļ��Ƿ����
���������
	idOfObject������ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];

	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectPrimaryKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ������ļ�ֵ�ļ��Ƿ����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
���ܣ�����һ������ļ�ֵ�ļ�
���������
	idOfObject������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionCreateObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectPrimaryKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectPrimaryKeyIndexFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// ��������ļ�ֵ�ļ�
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectPrimaryKeyIndexFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
���ܣ���һ������ļ�ֵ�ļ�������һ���¼�¼�ļ�ֵ
���������
	idOfObject������ID
	primaryKey���¼�¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	storePosition���¼�¼�Ĵ洢λ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition)
{
	char	caFileName[512],caRecord[4096];
	int	iRet,iRecLen=0;

	if (idOfObject == NULL || primaryKey == NULL || storePosition == NULL)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	memset(caRecord,0,sizeof(caRecord));
	iRet=iRet=UnionPutRecFldIntoRecStr("fldGrp",primaryKey,strlen(primaryKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionPutRecFldIntoRecStr for primary [%s] fail!\n",primaryKey);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=iRet=UnionPutRecFldIntoRecStr("fileName",storePosition,strlen(storePosition),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionPutRecFldIntoRecStr for storePosition [%s] fail!\n",storePosition);
		return(iRet);
	}
	iRecLen += iRet;

        // modify by chenliang, 2008-10-10
	// iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
        // iRet = UnionInsertRecStrToFileInAscOrder(caFileName,caRecord,iRecLen);	// deleted by Wolfgang Wang, 2008/10/27
        // modify end.
        iRet = UnionInsertRecStrToFileInOrderByFld(caFileName,caRecord,iRecLen,"fldGrp",1);	// added by Wolfgang Wang, 2008/10/27
	if (iRet < 0)
	{
		// UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
                UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionInsertRecStrToFileInOrderByFld for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ���һ������ļ�ֵ�ļ���ɾ��һ����¼�ļ�ֵ
���������
	idOfObject������ID
	primaryKey��Ҫɾ����¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512],caBuf[4096];
	int	iRet;

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// ������ļ�ֵ�ļ����Ƿ��иü�¼
	iRet=UnionObjectRecPaimaryKeyIsRepeat(idOfObject,primaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: UnionObjectRecPaimaryKeyIsRepeat fail! reutnr=[%d]\n",iRet);
		return(iRet);
	}
	else if (iRet == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: [%s] does not exists!\n",primaryKey);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	memset(caBuf,0,sizeof(caBuf));
	sprintf(caBuf,"fldGrp=%s",primaryKey);

	//iRet = UnionDeleteRecStrFromFile(caFileName,caBuf,strlen(caBuf));	// 2008/10/27,wolfgang deleted
	iRet = UnionDeleteRecStrFromFileByFld(caFileName,caBuf,strlen(caBuf),"fldGrp");	// 2008/10/27,wolfgang added
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: UnionDeleteRecStrFromFile for primary key [%s] fail! return=[%d]\n",primaryKey,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ����ݶ���ʵ���ļ�ֵ���ʵ���Ĵ洢λ��
���������
	idOfObject������ID
	primaryKey����¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	sizeofStorePosition��storePosition�Ĵ洢�ռ��С
���������
	storePosition����¼�Ĵ洢λ��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionGetObjectRecStoreStationByPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition,int sizeofStorePosition)
{
	char	caFileName[512],caRecord[4096];
	int	iRet,iRecLen=0,iLen;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || primaryKey == NULL || storePosition == NULL || sizeofStorePosition <= 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
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
		if (iLen != strlen(primaryKey))
			continue;			
		// Mary add end, 20081125
		
		if (strncmp(&caRecord[7],primaryKey,strlen(primaryKey)) == 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			iRet=UnionReadRecFldFromRecStr(caRecord,iRecLen,"fileName",storePosition,sizeofStorePosition);
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
				return(iRet);
			}
			return(0);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: primaryKey [%s] is not found!\n",primaryKey);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
}

/*
���ܣ��ж϶���ʵ���ļ�ֵ�Ƿ��ظ�
���������
	idOfObject������ID
	primaryKey����¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ�������
*/
int UnionObjectRecPaimaryKeyIsRepeat(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512],caRecord[4096];
	int	iRet,iRecLen=0,iLen=0;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		if ((iRet = UnionExistAndCreateFile(caFileName,"")) < 0)
		{
			UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: [%s] does not exists!\n",caFileName);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
		}
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// ���ļ�����
			break;
		
		
		// Mary add begin, 20081027
		for (iLen=0;iLen<iRecLen;iLen++)
		{
			if (caRecord[iLen+7] == '|')
				break;
		}
		if (iLen != strlen(primaryKey))
			continue;			
		// Mary add end, 20081027
		
		if (strncmp(&caRecord[7],primaryKey,strlen(primaryKey)) == 0)
		{
			UnionProgramerLog("in UnionObjectRecPaimaryKeyIsRepeat:: primaryKey is repeat! primaryKey = [%s] record=[%s]\n",primaryKey,caRecord);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(1);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	return(0);
}

/*
���ܣ���ö����ʵ����Ŀ
���������
	idOfObject������ID
���������
	��
����ֵ��
	>=0��ʵ����
	<0��ʧ�ܣ�������
*/
int UnionGetObjectRecordNumber(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionGetObjectRecordNumber:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionGetObjectRecordNumber:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// ��ö���ļ�ֵ�ļ��еļ�¼��
	iRet=UnionGetTotalRecNumInFile(caFileName);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionGetObjectRecordNumber:: UnionGetTotalRecNumInFile fail! return=[%d]\n",iRet);
		return(iRet);
	}

	return(iRet);
}

// Mary add begin, 20081112
/*
���ܣ���ȡָ��������м�¼
���������
	tableName��	����
	maxRecNum	������Ի��������¼����
���������
	recKey		�ؼ��֣���һ���ؼ����ɶ���򹹳ɣ�ʹ��.�ֿ�������
����ֵ��
	>=0��		��¼��
	<0��		�������
*/
int UnionReadAllRecOfSpecTBL(char *tableName,char recKey[][128+1],int maxRecNum)
{
	char	caFileName[512];
	int	iRet;
        char 	tmpBuf[128+1];
        int	iTmpRet;
	int	i;
	
	if (tableName == NULL || maxRecNum < 0)
	{
		UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	if (maxRecNum == 0)
	{
		UnionAuditLog("in UnionReadAllRecOfSpecTBL:: maxRecNum is 0!\n");
		return(0);
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(tableName,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// ��ü�ֵ�ļ������м�¼�ļ�ֵ
	iRet=UnionReadAllRecFldInFile(caFileName,"fldGrp",recKey,maxRecNum);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: UnionReadAllRecFldInFile fail! return=[%d]\n",iRet);
		return(iRet);
	}

        // Begin Add By HuangBaoxin 2008/11/14
        for (i = 0; i<iRet; i++)
        {
                memset(tmpBuf, 0, sizeof(tmpBuf));
                iTmpRet = UnionTranslateRecFldStrIntoStr(recKey[i], strlen(recKey[i]), tmpBuf, sizeof(tmpBuf));
                if (iTmpRet < 0)
                {
                	UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: UnionTranslateRecFldStrIntoStr fail! return=[%d]\n",iTmpRet);
                	return(iTmpRet);		
                }
                memset(recKey[i], 0, sizeof(recKey[i]));
                memcpy(recKey[i], tmpBuf, iTmpRet);
        }
        // End Add By HuangBaoxin 2008/11/14
        
	return(iRet);
}
// Mary add end, 20081112

/*
function:
        �ж��Ƿ��Ƕ����еĹؼ��ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               �ǹؼ��ֶ�
        0               ���ǹؼ��ֶ�
*/
int UnionIsPrimaryKeyFld(TUnionIDOfObject idOfObject, char *fldName)
{
        int             ret;
        TUnionObject    obj;
        //int             i;

        if(NULL == fldName || strlen(fldName) == 0)
                return 0;

        memset(&obj,0,sizeof(obj));
        // ���ݶ���������ȡ�ö���Ķ���
        if ((ret = UnionReadObjectDef(idOfObject,&obj)) < 0)
        {
                UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadObjectDef from [%s] errCode = [%d]\n",idOfObject,ret);
                return(ret);
        }
        /*
        for(i = 0; i < obj.primaryKey.fldNum; i++)
                if(strcmp(obj.primaryKey.fldNameGrp[i], fldName) == 0)
                        return 1;

        return 0;
        */
        return(UnionIsPrimaryKeyFldOfObject(&obj,fldName));
}

/*
function:
        �ж��Ƿ��Ƕ����еĹؼ��ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               �ǹؼ��ֶ�
        0               ���ǹؼ��ֶ�
*/
int UnionIsPrimaryKeyFldOfObject(PUnionObject pobject, char *fldName)
{
        int             ret;
        int             i;

        if (NULL == fldName || pobject == 0)
                return 0;

        for(i = 0; i < pobject->primaryKey.fldNum; i++)
                if(strcmp(pobject->primaryKey.fldNameGrp[i], fldName) == 0)
                        return 1;

        return 0;
}

/*
function:
        �ж��Ƿ��Ƕ����е�Ψһֵ�ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               ��Ψһֵ�ֶ�
        0               ����Ψһֵ�ֶ�
*/
int UnionIsUniqueFld(TUnionIDOfObject idOfObject, char *fldName)
{
        int             ret;
        TUnionObject    obj;

        if(NULL == fldName || strlen(fldName) == 0)
                return 0;

        memset(&obj,0,sizeof(obj));
        // ���ݶ���������ȡ�ö���Ķ���
        if ((ret = UnionReadObjectDef(idOfObject,&obj)) < 0)
        {
                UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadObjectDef from [%s] errCode = [%d]\n",idOfObject,ret);
                return(ret);
        }

        return(UnionIsUniqueFldOfObject(&obj,fldName));
}

/*
function:
        �ж��Ƿ��Ƕ����е�Ψһ�ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               ��Ψһֵ�ֶ�
        0               ����Ψһֵ�ֶ�
*/
int UnionIsUniqueFldOfObject(PUnionObject pobject, char *fldName)
{
        int             ret;
        int             index1,index2;
        PUnionObjectFldGrp	pfldGrp;

        if (NULL == fldName || pobject == 0)
                return 0;

        for(index1 = 0; index1 < pobject->uniqueFldGrpNum; index1++)
        {
        	pfldGrp = &(pobject->uniqueFldGrp[index1]);
        	for (index2 = 0; index2 < pfldGrp->fldNum; index2++)
        	{
                	if(strcmp(pfldGrp->fldNameGrp[index2], fldName) == 0)
                        	return(1);
		}
	}
        return(0);
}
