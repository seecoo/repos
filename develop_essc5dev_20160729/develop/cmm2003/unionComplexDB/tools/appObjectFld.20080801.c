//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "unionComplexDBRecord.h"
#include "unionRecFile.h"
#include "UnionLog.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index;
	char	*ptr;
	char	objectName[128+1];
	char	newFldStr[1024+1];
	
	if ((ret = UnionConnectDatabase()) < 0)
	{
		printf("UnionConnectDatabase failure! ret = [%d]\n",ret);
		return(ret);
	}
loop1:
	ptr = UnionInput("\n\n��������������(exit�˳�)::");
	strcpy(objectName,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
	ptr = UnionInput("\n\n��������չ����(exit�˳�)::");
	strcpy(newFldStr,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}

	if ((ret = UnionBatchExpendFieldToObjectRecord(objectName,newFldStr,strlen(newFldStr))) < 0)
		printf("UnionBatchExpendFieldToObjectRecord [%s] failure! ret = [%d]\n",objectName,ret);
	else
		printf("UnionBatchExpendFieldToObjectRecord [%s] OK!\n",objectName);
	goto loop1;
}

/*
���ܣ�
	����Ϊ���м�¼������չ��
���������
	idOfObject��	����ID
	fldStr��	�������嵥
	lenOfFldStr	�������嵥�ĳ���
���������
	fileName���洢�˲�ѯ������ʵ����ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ���ļ�¼����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchExpendFieldToObjectRecord(char *idOfObject,char *fldStr,int lenOfFldStr)
{
	int			iRet,iLen,iRecLen,iSelectNum=0;
	char			caFileName[512],caValueFile[512],caBuf[512],caPrimaryKey[512];
	char			caRecord[4096+1];
	TUnionObject		sObject;
	TUnionRecFileHDL	hFileHdl;
	int			newLen;
	
	if (idOfObject == NULL || fldStr == NULL || lenOfFldStr == 0)
	{
		UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}

	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caBuf,0,sizeof(caBuf));
		iLen=UnionReadNextRecFromFile(hFileHdl,caBuf,sizeof(caBuf));
		if (iLen < 0)
		{
			UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionReadNextRecFromFile fail! return=[%d]\n",iLen);
			UnionCloseRecFileHDL(hFileHdl);
			return(iLen);
		}
		if (iLen == 0)	// ���ļ�����
			break;
		
		// �õ�ֵ�洢�ļ�����
		memset(caValueFile,0,sizeof(caValueFile));
		iRet=UnionReadRecFldFromRecStr(caBuf,iLen,"fldGrp",caValueFile,sizeof(caValueFile));
		if (iRet < 0)
		{
			UnionCloseRecFileHDL(hFileHdl);
			UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionReadRecFldFromRecStr [%s] from [%s] fail! return=[%d]\n","fldGrp",caBuf,iRet);
			return(iRet);
		}
		// ƴװ�ؼ��ֲ�ѯ����
		memset(caPrimaryKey,0,sizeof(caPrimaryKey));
		if ((newLen = UnionFormObjectPrimarKeyCondition(idOfObject,caValueFile,iRet,caPrimaryKey,sizeof(caPrimaryKey))) < 0)
		{
			UnionCloseRecFileHDL(hFileHdl);
			UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionFormObjectPrimarKeyCondition fail! return=[%d]\n",newLen);
			return(newLen);
		}
		memset(caRecord,0,sizeof(caRecord));
		if ((iRecLen = UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,caPrimaryKey,caRecord,sizeof(caRecord))) < 0)
		{
			UnionCloseRecFileHDL(hFileHdl);
			UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionSelectUniqueObjectRecordByPrimaryKey fail! return=[%d]\n",iRecLen);
			return(iRecLen);
		}
		if ((newLen = iRecLen + lenOfFldStr) >= sizeof(caRecord))
		{
			//UnionCloseRecFileHDL(hFileHdl);
			UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: caRecord [%s] too long!\n",caRecord);
			continue;
		}
		memcpy(caRecord+iRecLen,fldStr,lenOfFldStr);
		// ��ö����ֵ�ļ�����
		memset(caPrimaryKey,0,sizeof(caPrimaryKey));
		if ((iRet = UnionFormPrimaryKeyStrOfSpecObjectRec(idOfObject,caRecord,newLen,caPrimaryKey,sizeof(caPrimaryKey))) < 0)
		{
			//UnionCloseRecFileHDL(hFileHdl);
			UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionFormPrimaryKeyStrOfSpecObjectRec from [%s]!\n",caRecord);
			continue;
		}
		memset(caFileName,0,sizeof(caFileName));
		UnionGetObjectRecValueFileName(idOfObject,caPrimaryKey,caFileName);
		if ((iRet = UnionWriteRecStrIntoFile(caFileName,caRecord,newLen)) < 0)
		{
			UnionCloseRecFileHDL(hFileHdl);
			UnionUserErrLog("in UnionBatchExpendFieldToObjectRecord:: UnionWriteRecStrIntoFile [%s] to [%s],fail! return=[%d]\n",caRecord,caFileName,iRecLen);
			return(iRet);
		}
		iSelectNum++;
	}
	UnionCloseRecFileHDL(hFileHdl);	
	
	return(iSelectNum);
}
