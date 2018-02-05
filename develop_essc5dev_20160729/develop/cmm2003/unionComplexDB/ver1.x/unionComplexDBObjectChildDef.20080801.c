//	Author: ChenJiaMei
//	Date: 2008-7-31

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
#include "unionComplexDBObjectChildDef.h"

/*
���ܣ����һ��������ⲿ���ö���Ǽ��ļ��Ƿ����
���������
	idOfObject������ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectForeignObjDefFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];

	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectForeignObjDefFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��������ⲿ���ö���Ǽ��ļ��Ƿ����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
���ܣ�����һ��������ⲿ���ö���Ǽ��ļ�
���������
	idOfObject������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionCreateObjectForeignObjDefFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectForeignObjDefFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö�����ⲿ���ö���Ǽ��ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	// ��������ⲿ���ö���Ǽ��ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectForeignObjDefFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// ����������ⲿ���ö���Ǽ��ļ�
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectForeignObjDefFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
���ܣ�����Ƿ����������������˱�����
���������
	idOfObject������ID
���������
	��
����ֵ��
	>0���������������ñ�����
	0��û�������������ñ�����
	<0��ʧ�ܣ�������
*/
int UnionExistForeignObjectInfo(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iNum;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistForeignObjectInfo:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	iNum=UnionGetTotalRecNumInFile(caFileName);
	if (iNum < 0)
	{
		UnionUserErrLog("in UnionExistForeignObjectInfo:: UnionGetTotalRecNumInFile fail! return=[%d]\n",iNum);
		return(iNum);
	}
	
	return(iNum);
}

/*
���ܣ��Ǽ����ñ�������ⲿ�����������Ϣ
���������
	idOfObject��������ID
	foreignObjectGrp�����ñ�������ⲿ����ؼ��ֶ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionRegisterForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp)
{
	char	caFileName[512],caRecord[512],caBuf[512];
	int	iRet,i,iRecLen=0;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionRegisterForeignObjectInfo:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);

	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionAuditLog("in UnionRegisterForeignObjectInfo:: [%s] does not exists!\n",caFileName);
		if ((iRet = UnionExistAndCreateFile(caFileName,"")) < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: [%s] does not exists!\n",caFileName);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
		}
	}

	for (i=0;i<foreignObjectGrp.childNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=0;
		
		// ��������
                // UnionLog("In UnionRegisterForeignObjectInfo turn to UnionFormFieldNameStrFromFldGrp, and teh index is: [%d].\n", i);
                
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).localFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for localFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("localFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] localFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// �ⲿ��������
		iRet=UnionPutRecFldIntoRecStr("foreignObject",(foreignObjectGrp.childObjectGrp[i]).objectName,strlen((foreignObjectGrp.childObjectGrp[i]).objectName),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignObject fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// �ⲿ����
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).foreignFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for foreignFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("foreignFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		
		iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	
	return(0);
}

/*
���ܣ�ȡ�����ñ�������ⲿ����������Ϣ�ĵǼ�
���������
	idOfObject��������ID
	foreignObjectGrp�����ñ�������ⲿ����ؼ��ֶ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCancelRegisterOfForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp)
{
	char	caFileName[512],caRecord[512],caBuf[512];
	int	iRet,i,iRecLen=0;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	for (i=0;i<foreignObjectGrp.childNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=0;
		
		// ��������
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).localFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for localFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("localFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] localFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// �ⲿ��������
		iRet=UnionPutRecFldIntoRecStr("foreignObject",(foreignObjectGrp.childObjectGrp[i]).objectName,strlen((foreignObjectGrp.childObjectGrp[i]).objectName),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignObject fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// �ⲿ����
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).foreignFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for foreignFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("foreignFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		
		iRet = UnionDeleteRecStrFromFile(caFileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionDeleteRecStrFromFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	
	return(0);
}
