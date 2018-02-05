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
#include "simuMngSvrLocally.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBTrigger.h"
#include "unionComplexDBOperationDef.h"
#include "unionDatabaseCmd.h"

#ifndef conMenuDefinedQueryConditionFldSeperator
#define conMenuDefinedQueryConditionFldSeperator	','     // �˵�����Ĳ�ѯ�����еķָ���
#endif
#ifndef conMenuDefinedQueryConditionReadThisFld
#define conMenuDefinedQueryConditionReadThisFld		"this."   // �����ȡ��ǰ��¼�ĸ���
#endif

int gunionIsOperationOnTrigger = 0;

/*
����	�򿪴����������Ķ���
�������
	��
�������
	��
����ֵ
	��
*/
void UnionOpenOperationOnTrigger()
{
	gunionIsOperationOnTrigger = 1;
}

/*
����	�رմ����������Ķ���
�������
	��
�������
	��
����ֵ
	��
*/
void UnionCloseOperationOnTrigger()
{
	gunionIsOperationOnTrigger = 0;
}

/*
����	�ж��Ƿ��Ǵ����������Ķ���
�������
	��
�������
	��
����ֵ
	1	��
	0	����
*/
int UnionIsOperationOnTrigger()
{
	return(gunionIsOperationOnTrigger);
}

//---------------------------------------------------------------------------
/*
����	���ݶ����trigger�������Լ�����������ƴ��trigger��Ҫ�Ĳ�������
�������
	oriCondition		trigger����ĸ�ֵ����
	lenOfOriCondition	trigger����ĸ�ֵ��������
	recStr			����ʹ�õ�ʵ������
	lenOfRecStr		����ʹ�õ�ʵ�������ĳ���
	sizeOfDesCondition	Ŀ����������Ĵ�С
�������
	desCondition		Ŀ������
����ֵ
	>= 0			Ŀ�������ĳ���
	<0			�������
*/
int UnionFormConditionFromTriggerDefinedStr(char *oriCondition,int lenOfOriCondition,char *recStr,int lenOfRecStr,char *desCondition,int sizeOfDesCondition)
{
	int		index;
	int		offset;
	int		ret;
	TUnionRec	recAsignDef;
	TUnionRec	recValue;
	int		asignFldNum;
	char		fldNameInRecStr[128+1];
	
	if ((oriCondition == NULL) || (desCondition == NULL) || (lenOfOriCondition < 0) || (recStr == NULL) || (lenOfRecStr < 0))
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: parameter error!\n");
		return(errCodeParameter);
	}
	UnionDebugLog("in UnionFormConditionFromTriggerDefinedStr::\noriCondition=[%04d][%s]\n",lenOfOriCondition,oriCondition);
	UnionDebugNullLog("recStr=[%04d][%s]\n",lenOfRecStr,recStr);
	if (lenOfOriCondition == 0)	// Դ����Ϊ��
	{
		strcpy(desCondition,"");
		return(0);
	}
	// ��Դ�����е���ָ���ת��Ϊͨ�÷ָ���
	if ((ret = UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator(oriCondition,lenOfOriCondition,conMenuDefinedQueryConditionFldSeperator,oriCondition,lenOfOriCondition+1)) < 0)
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator!\n");
		return(ret);
	}
	// ��Դ�����ж�ȡ��ֵ����
	memset(&recAsignDef,0,sizeof(recAsignDef));
	if ((asignFldNum = UnionReadRecFromRecStr(oriCondition,lenOfOriCondition,&recAsignDef)) < 0)
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionReadRecFromRecStr!\n");
		return(asignFldNum);
	}
	// ���ݸ�ֵ������ֵ
	memset(&recValue,0,sizeof(recValue));
	for (index = 0; index < recAsignDef.fldNum; index++)
	{
		strcpy(recValue.fldName[index],recAsignDef.fldName[index]);
		if (strstr(recAsignDef.fldValue[index],conMenuDefinedQueryConditionReadThisFld) == NULL)	// ������˹̶�ֵ
		{
			strcpy(recValue.fldValue[index],recAsignDef.fldValue[index]);
			continue;
		}
		// �������ȡ�ɱ�ֵ
		if (strlen(recAsignDef.fldValue[index]+strlen(conMenuDefinedQueryConditionReadThisFld)) >= sizeof(fldNameInRecStr))
		{
			UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: fld alais name [%s] too long!\n",recAsignDef.fldValue[index]+strlen(conMenuDefinedQueryConditionReadThisFld));
			return(errCodeParameter);
		}
                // �Ӽ�¼���ڶ���ֵ����
		strcpy(fldNameInRecStr,recAsignDef.fldValue[index]+strlen(conMenuDefinedQueryConditionReadThisFld));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldNameInRecStr,recValue.fldValue[index],sizeof(recValue.fldValue[index]))) < 0)
		{
			UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionReadRecFldFromRecStr [%s]\n",fldNameInRecStr);
			return(ret);
		}
	}
	recValue.fldNum = recAsignDef.fldNum;
	if ((ret = UnionPutRecIntoRecStr(&recValue,desCondition,sizeOfDesCondition)) < 0)
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionPutRecIntoRecStr!\n");
		return(ret);
	}
        desCondition[ret] = 0;
	UnionDebugLog("in UnionFormConditionFromTriggerDefinedStr::\ndesCondition=[%04d][%s]\n",ret,desCondition);
	return(ret);
}

/*
���ܣ�
	ִ�д���������
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
	fileRecved	�Ƿ��յ����ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationInStr(char *triggerStr,int lenOfTriggerStr,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	resName[128+1];
	char	oriCondition[1024+1];
	char	desCondition[1024+1];
	int	operationID;
	int	lenOfDesCondition;

	if (UnionIsOperationOnTrigger())	// ��ǰ�����Ǵ����������Ķ���
		return(0);
		
	// ��ȡһ�д���������
	memset(oriCondition,0,sizeof(oriCondition));
	memset(resName,0,sizeof(resName));
	if ((ret = UnionReadComplexDBOperationDefFromStr(triggerStr,lenOfTriggerStr,resName,sizeof(resName),&operationID,oriCondition,sizeof(oriCondition))) < 0)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInStr:: UnionReadComplexDBOperationDefFromStr from [%s]\n",triggerStr);
		return(ret);
	}
	// ת����ò�������
	memset(desCondition,0,sizeof(desCondition));
	if ((lenOfDesCondition = UnionFormConditionFromTriggerDefinedStr(oriCondition,strlen(oriCondition),recStr,lenOfRecStr,desCondition,sizeof(desCondition))) < 0)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInStr:: UnionFormConditionFromTriggerDefinedStr from [%s] and from [%s]\n",triggerStr,recStr);
		return(lenOfDesCondition);
	}
	// ִ�в���
	UnionOpenOperationOnTrigger();
	if ((ret = UnionExcuteDBSvrOperation(resName,operationID,desCondition,lenOfDesCondition,resStr,sizeOfResStr,fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInStr:: UnionExcuteDBSvrOperation [%s] on [%s]\n",desCondition,resName);
		UnionCloseOperationOnTrigger();
		return(ret);
	}
	UnionCloseOperationOnTrigger();
	return(ret);
}

/*
���ܣ�
	ִ��һ���ļ��еĴ���������
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
	fileRecved	�Ƿ��յ����ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationInFile(char *fileName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	lineStr[2048+1];
	FILE	*fp;
	int	lineLen;
	int	offset = 0;

	if (UnionIsOperationOnTrigger())	// ��ǰ�����Ǵ����������Ķ���
		return(0);
	
	// UnionLog("in UnionExcuteTriggerOperationInFile, fileName: [%s], recStr: [%s].\n", fileName, recStr);
	/* Mary delete, 20081211
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionExcuteTriggerOperationInFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	*/
	// Mary add begin, 20081211
	if ((fp = UnionOpenRecFileHDL(fileName)) == NULL)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInFile:: UnionOpenRecFileHDL for [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// Mary add end, 20081211
	
	// ���ж�����������
	while (!feof(fp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionUserErrLog("in UnionExcuteTriggerOperationInFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			ret = lineLen;
			goto errExit;
		}
		if (lineLen == 0)	// ����
			continue;
		// ִ�д���������
		if ((ret = UnionExcuteTriggerOperationInStr(lineStr,lineLen,recStr,lenOfRecStr,resStr+offset,sizeOfResStr-offset,fileRecved)) < 0)
		{
			UnionUserErrLog("in UnionExcuteTriggerOperationInFile:: UnionExcuteTriggerOperationInStr from [%s] in [%s]\n",lineStr,fileName);
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
	ִ��һ����Ĵ���������
���������
	resName		��������
	operationTag	������ʶ
	beforeOrAfter	�ǲ���֮ǰִ�л���֮��ִ��
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
	fileRecved	�Ƿ��յ����ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationOnObject(char *resName,char *operationTag,char *beforeOrAfter,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	fileName[512+1];
		
	// �жϴ�����������Ƿ����
	if ((ret = UnionGetObjectTriggerFileName(resName,operationTag,beforeOrAfter,fileName)) <= 0)
		return(ret);
	
	// UnionLog("in UnionExcuteTriggerOperationOnObject, fileName is: [%s].\n", fileName);
	
	return(UnionExcuteTriggerOperationInFile(fileName,recStr,lenOfRecStr,recStr,sizeOfResStr,fileRecved));
}

/*
���ܣ�
	ִ��һ����Ĵ���������
���������
	resName		��������
	operationID	������ʶ
	isBeforeOperation	�ǲ���֮ǰִ�л���֮��ִ�У�1 ��ʾ����֮ǰ
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr)
{
	char	operationTag[64+1];
	char	beforeOrAfter[20];
	char	resStr[4096+1];
	int	fileRecved;
	
	if (UnionIsOperationOnTrigger())	// ��ǰ�����Ǵ����������Ķ���
		return(0);
	if (isBeforeOperation)
		strcpy(beforeOrAfter,"before");
	else
		strcpy(beforeOrAfter,"after");
	
	// UnionLog("in UnionExcuteTriggerOperationOnTable, beforeOrAfter: [%s].\n", beforeOrAfter);
	
	switch (operationID)
	{
		case	conDatabaseCmdInsert:	// �����¼
			strcpy(operationTag,"insert");
			break;
		case	conDatabaseCmdDelete:	// ɾ����¼
			strcpy(operationTag,"delete");
			break;
		case	conDatabaseCmdConditionDelete:	// ����ɾ����¼
			strcpy(operationTag,"deleteAll");
			break;
		case	conDatabaseCmdUpdate:	// �޸ļ�¼
			strcpy(operationTag,"update");
			break;
		case	conDatabaseCmdConditionUpdate:	// �����޸ļ�¼
			strcpy(operationTag,"updateAll");
			break;
		default:
			return(0);
	}
	
	// UnionLog("in UnionExcuteTriggerOperationOnTable, operationTag is: [%s].\n", operationTag);
	memset(resStr,0,sizeof(resStr));
	return(UnionExcuteTriggerOperationOnObject(resName,operationTag,beforeOrAfter,recStr,lenOfRecStr,resStr,sizeof(resStr),&fileRecved));
}
