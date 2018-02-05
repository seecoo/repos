//	Author: Wolfgang Wang
//	Date: 2008-10-23

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
#include "unionComplexDBTrigger.h"
#include "unionComplexDBOperationDef.h"
#include "unionComplexDBUnionQuery.h"
#include "unionComplexDBRecord.h"
#include "unionDatabaseCmd.h"

/*
���ܣ�
	ִ�����ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecInStr(char *triggerStr,int lenOfTriggerStr,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr)
{
	int	ret;
	char	resName[128+1];
	char	oriCondition[1024+1];
	char	desCondition[1024+1];
	int	operationID;
	int	lenOfDesCondition;
	char	fldGrp[1024+1];
	int	lenOfFldGrp;

	// ��ȡһ�����ϲ�ѯ����
	memset(oriCondition,0,sizeof(oriCondition));
	memset(fldGrp,0,sizeof(fldGrp));
	memset(resName,0,sizeof(resName));
	if ((ret = UnionReadComplexDBSelectOperationDefFromStr(triggerStr,lenOfTriggerStr,resName,sizeof(resName),&operationID,oriCondition,sizeof(oriCondition),fldGrp,sizeof(fldGrp))) < 0)
	{
		UnionUserErrLog("in UnionExcuteUnionSelectSpecRecInStr:: UnionReadComplexDBSelectOperationDefFromStr from [%s]\n",triggerStr);
		return(ret);
	}
	lenOfFldGrp = strlen(fldGrp);
	// ת����ò�������
	memset(desCondition,0,sizeof(desCondition));
	if ((lenOfDesCondition = UnionFormConditionFromTriggerDefinedStr(oriCondition,strlen(oriCondition),recStr,lenOfRecStr,desCondition,sizeof(desCondition))) < 0)
	{
		UnionUserErrLog("in UnionExcuteUnionSelectSpecRecInStr:: UnionFormConditionFromTriggerDefinedStr from [%s] and from [%s]\n",triggerStr,recStr);
		return(lenOfDesCondition);
	}
	UnionDebugLog("in UnionSelectObjectFldGrpByPrimaryKey:: fldGrp = [%04d][%s]\n",lenOfFldGrp,fldGrp);
	if ((ret = UnionSelectObjectFldGrpByPrimaryKey(resName,desCondition,fldGrp,lenOfFldGrp,resStr,sizeOfResStr)) < 0)
	{
		if (ret == errCodeObjectMDL_ObjectDefinitionNotExists)
			return(UnionGenerateNullValueRecStr(fldGrp,lenOfFldGrp,resStr,sizeOfResStr));
		UnionUserErrLog("in UnionExcuteUnionSelectSpecRecInStr:: UnionSelectObjectFldGrpByPrimaryKey [%s] by [%s] on [%s]\n",fldGrp,desCondition,resName);
		return(ret);
	}
	return(ret);
}

/*
���ܣ�
	ִ��һ���ļ��е����ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecInFile(char *fileName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr)
{
	int	ret;
	char	lineStr[2048+1];
	FILE	*fp;
	int	lineLen;
	int	offset = 0;

	if (UnionIsOperationOnTrigger())	// ��ǰ���������ϲ�ѯ�����Ķ���
		return(0);
	
	/* Mary delete, 20081211
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionExcuteUnionSelectSpecRecInFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	*/
	// Mary add begin, 20081211
	if ((fp = UnionOpenRecFileHDL(fileName)) == NULL)
	{
		UnionUserErrLog("in UnionExcuteUnionSelectSpecRecInFile:: UnionOpenRecFileHDL for [%s] fail!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// Mary add end, 20081211
	
	// ���ж����ϲ�ѯ����
	while (!feof(fp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionUserErrLog("in UnionExcuteUnionSelectSpecRecInFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			ret = lineLen;
			goto errExit;
		}
		if (lineLen == 0)	// ����
			continue;
		// ִ�����ϲ�ѯ����
		if ((ret = UnionExcuteUnionSelectSpecRecInStr(lineStr,lineLen,recStr,lenOfRecStr,resStr+offset,sizeOfResStr-offset)) < 0)
		{
			UnionUserErrLog("in UnionExcuteUnionSelectSpecRecInFile:: UnionExcuteUnionSelectSpecRecInStr from [%s] in [%s]\n",lineStr,fileName);
			goto errExit;
		}
		offset += ret;
	}
	ret = offset;
errExit:
	//fclose(fp);	Mary modify, 20081211
	UnionCloseRecFileHDLOfFileName(fp,fileName);
	return(ret);
}

/*
���ܣ�
	ִ��һ��������ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecOnObject(char *resName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr)
{
	int	ret;
	char	fileName[512+1];
	int	len;
	char	condition[1024+1];

	if (lenOfRecStr >= sizeof(condition))
	{
		UnionUserErrLog("in UnionExcuteUnionSelectSpecRecOnObject:: lenOfRecStr too long!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	memset(condition,0,sizeof(condition));
	memcpy(condition,recStr,lenOfRecStr);
	if ((len = UnionSelectUniqueObjectRecordByPrimaryKey(resName,recStr,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionExcuteUnionSelectSpecRecOnObject:: UnionSelectUniqueObjectRecordByPrimaryKey! primaryKey = [%s]\n",recStr);
		return(len);
	}
	// �ж����ϲ�ѯ������Ƿ����
	if ((ret = UnionGetObjectTriggerFileName(resName,"select","union",fileName)) <= 0)
	{
		return(len);
	}
	if ((ret = UnionExcuteUnionSelectSpecRecInFile(fileName,condition,lenOfRecStr,resStr+len,sizeOfResStr-len)) < 0)
	{
		UnionUserErrLog("in UnionExcuteUnionSelectSpecRecOnObject:: UnionExcuteUnionSelectSpecRecInFile!\n");
		ret = 0;
		//return(ret);
	}
	return(len+ret);
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
int UnionBatchSelectUnionObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName)
{
	int	iRet,iLen,iRecLen,iSelectNum=0;
	char	caFileName[512],caValueFile[512],caBuf[512],caPrimaryKey[512];
	char	caRecord[8192];
	TUnionObject		sObject;
	TUnionRecFileHDL	hFileHdl;
	int	lenOfCon;
	
        FILE    *fp = NULL;
	
	if (idOfObject == NULL || condition == NULL || fileName == NULL)
	{
		UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// ��ö���ļ�ֵ�ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// ������ļ�ֵ�ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}

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
        
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caBuf,0,sizeof(caBuf));
		iLen=UnionReadNextRecFromFile(hFileHdl,caBuf,sizeof(caBuf));
		if (iLen < 0)
		{
			UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionReadNextRecFromFile fail! return=[%d]\n",iLen);
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
			UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
			return(iRet);
		}
		// ƴװ�ؼ��ֲ�ѯ����
		memset(caPrimaryKey,0,sizeof(caPrimaryKey));
		if ((lenOfCon = UnionFormObjectPrimarKeyCondition(idOfObject,caValueFile,iRet,caPrimaryKey,sizeof(caPrimaryKey))) < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionFormObjectPrimarKeyCondition fail! return=[%d]\n",lenOfCon);
			return(lenOfCon);
		}
		
		if ((iRecLen = UnionExcuteUnionSelectSpecRecOnObject(idOfObject,caPrimaryKey,lenOfCon,caRecord,sizeof(caRecord))) < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionExcuteUnionSelectSpecRecOnObject primaryKey = [%s] from [%s] fail! return=[%d]\n",caPrimaryKey,caValueFile,iRecLen);
			return(iRecLen);
		}
		// �жϸü�¼�Ƿ��������
		iRet=UnionIsRecStrFitSpecConditon(caRecord,iRecLen,condition,strlen(condition));
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionIsRecStrFitSpecConditon fail! return=[%d]\n",iRet);
			return(iRet);
		}
		if (iRet == 0)			// ����������
			continue;
		
		// д�ļ�
		iRet=UnionAppendRecStrToFile(fileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			UnionUserErrLog("in UnionBatchSelectUnionObjectRecord:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
		iSelectNum++;
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);	
	
	return(iSelectNum);
}
