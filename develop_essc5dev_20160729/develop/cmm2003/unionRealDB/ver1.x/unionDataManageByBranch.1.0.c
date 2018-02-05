//	Author: Wolfgang Wang
//	Date: 2008-12-3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionAutoFldDef.h"
#include "unionREC.h"
#include "unionDataManageByBranch.h"
#include "unionDataTBLList.h"

#include "unionErrCode.h"

// #include "esscFldTagDef.h"

char *UnionGetCurrentOperationTellerNo();

int  gunionIsAddKeyWordAND = 0;
int  gunionIsOutPutInPutDataTools = 0;

int UnionSetIsAddKeyWordAND(int flag)
{
	gunionIsAddKeyWordAND = flag;
	return 0;
}

int UnionGetIsAddKeyWordAND()
{
	return gunionIsAddKeyWordAND;
}

void UnionSetDataManagementNotUnderBranch()
{
	gunionIsOutPutInPutDataTools = 1;
}

int UnionSetIsOutPutInPutDataTools(int flag)
{
	gunionIsOutPutInPutDataTools = flag;
	return 0;
}

int UnionGetIsOutPutInPutDataTools()
{
	return gunionIsOutPutInPutDataTools;
}

/*
���ܣ�
	��ȡ����Ա����Ȩ
�������:
	idOfOperator 	�û�����ģ�����Ψһʶ��һ����¼������
�������:
	branchID	����Ա��������
	dataReadAuth	����Ա����Ȩ��
����ֵ
	>=0		�ɹ�
	<0		ʧ��,������
*/
int UnionReadAuthDefOfOperator(char *idOfOperator,char *branchID,int *dataReadAuth)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	// ƴ�ؼ���
	memset(condition,0,sizeof(condition));
	ret = UnionPutRecFldIntoRecStr("idOfOperator",idOfOperator,strlen(idOfOperator),condition+lenOfRecStr,sizeof(condition)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionPutRecFldIntoRecStr [idOfOperator] failure!\n");
		return(ret);
	}
	lenOfRecStr += ret;

	//��ȡ��¼
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameOperator,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(lenOfRecStr);
	}

	//��������
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"branch",branchID,40+1)) < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionReadRecFldFromRecStr!\n");
		return(ret);
	}

	//����Ȩ����
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"dataReadAuth",dataReadAuth)) < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionReadIntTypeRecFldFromRecStr!\n");
		return(ret);
	}

	return(ret);
}

/*
���ܣ�
	�ж��Ƿ�ֻ�����������
���������
	��
���������
	��
����ֵ��
	1��		��
	0��		��
*/
int UnionIsManageDataByBranch()
{
	if (UnionReadIntTypeRECVar("isManageDataByBranch") > 0)
		return(1);
	else
		return(0);
}

/*
���ܣ�
	�������������
���������
	idOfObject��	����ID
���������
	branchFldName	�����������
����ֵ��
	>=0��		�ɹ���������
	<0��		ʧ�ܣ�������
*/
int UnionReadBranchFldNameOfSpecTBL(char *tblName,char *branchFldName)
{
	int	ret;
	char	condition[128+1];
	
	sprintf(condition,"tableName=%s|",tblName);
	if ((ret = UnionSelectSpecFldOfObjectByPrimaryKey("tableList",condition,"branchFldName",branchFldName,40+1)) < 0)
	{
		UnionUserErrLog("in UnionReadBranchFldNameOfSpecTBL:: UnionSelectSpecFldOfObjectByPrimaryKey [%s]\n",condition);
		return(ret);
	}
	return(ret);
}
	
/*
���ܣ�
	�Զ����Ӳ�ѯ����
���������
	idOfObject������ID
	oriConLen:	Դ�����ĳ���
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	sizeOfBuf	����������Ĵ�С
���������
	condition	�������Զ���ѯ���������
����ֵ��
	>=0��		�ɹ���������
	<0��		ʧ�ܣ�������
*/
int UnionAutoAppendBranchIDToQueryCondition(char *tblName,int oriConLen,char *condition,int sizeOfBuf)
{
	int	ret;
	char	branchID[40+1];
	int	readAllDataPermitted;
	char	branchFldName[40+1];
	char	realBranchFldName[40+1];
	int	offset = 0;
        
	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** entering!\n");
	// ����ǵ��뵼�����ݹ�����ֱ�ӷ���
	if (UnionGetIsOutPutInPutDataTools())
		return(oriConLen);

	if (!UnionIsManageDataByBranch())	// ��ƽ̨�������ݽ��зֻ�������
		return(oriConLen);
		
	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** reading operator authorization!\n");

	// �������Ƿ񸳼��˸����ֵ
	memset(branchFldName,0,sizeof(branchFldName));
	UnionReadBranchFldNameOfSpecTBL(tblName,branchFldName);
	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** branchFldName = [%s]\n",branchFldName);
	if (strlen(branchFldName) == 0)	// �ñ�δ��������򣬼��ñ�����ݲ��ֻ�������
		return(oriConLen);	

	// ������Ա�Ĳ�����Ȩ����
	memset(branchID,0,sizeof(branchID));
	if ((ret = UnionReadAuthDefOfOperator(UnionGetCurrentOperationTellerNo(),branchID,&readAllDataPermitted)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendBranchIDToQueryCondition:: UnionReadAuthDefOfOperator [%s]!\n",UnionGetCurrentOperationTellerNo());
		return(ret);
	}


	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** readAllDataPermitted = [%d]\n",readAllDataPermitted);
	if (readAllDataPermitted)	// ������Ա���Զ���������
		return(oriConLen);
	
	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** manage data by branch!\n");
	
	// ���������Զ����ӵ���ѯ������
	//if (oriConLen > 0)
	if (UnionGetIsAddKeyWordAND())
	{
		strcpy(condition," AND ");
		offset = 5;
	}
	memset(realBranchFldName,0,sizeof(realBranchFldName));
	UnionChargeFieldNameToDBFieldName(branchFldName,realBranchFldName);
	// 2011-12-31 �������޸�
	//sprintf(condition+offset,"%s = '%s'",realBranchFldName,branchID);
	sprintf(condition+offset,"%s LIKE '%%%s%%'",realBranchFldName,branchID);
	return(ret+strlen(condition));
}

/*
���ܣ�
	�Զ��������ݴ�
���������
	idOfObject��	����ID
	record		��¼����"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	lenOfRecord	��¼������
���������
	record		�������Զ���ļ�¼��
����ֵ��
	>=0��		�ɹ���������
	<0��		ʧ�ܣ�������
*/
int UnionAutoAppendFldOfSpecTBL(char *idOfObject,char *record,int lenOfRecord)
{
	int			ret;
	TUnionAutoFldDef	autoFldDefGrp[20];
	int			num;
	char			condition[128+1];
	int			index;
	
	if (!UnionIsManageDataByBranch())	// ��ƽ̨�������ݽ��зֻ�������
		return(lenOfRecord);
	
	// ��ȡ�Զ����ӵĸ�����
	sprintf(condition,"%s=%s|",conAutoFldDefFldNameDesResName,idOfObject);
	if ((num = UnionBatchReadAutoFldDefRec(condition,autoFldDefGrp,20)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldOfSpecTBL:: UnionBatchReadAutoFldDefRec!\n");
		return(num);
	}
	
	// �����Զ����ӵĸ����壬�����Զ�������
	for (index = 0; index < num; index++)
	{
		if ((ret = UnionAutoAppendFldByAutoFldDef(&(autoFldDefGrp[index]),record,lenOfRecord)) < 0)
		{
			UnionUserErrLog("in UnionAutoAppendFldOfSpecTBL:: UnionAutoAppendFldByAutoFldDef!\n");
			return(ret);
		}
		lenOfRecord = ret;
	}
	return(lenOfRecord);
}	

/*
���ܣ�
	�Զ��������ݴ�
���������
	pautoFldDef	�Զ���������
	record		��¼����"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	lenOfRecord	��¼������
���������
	record		�������Զ���ļ�¼��
����ֵ��
	>=0��		�ɹ���������
	<0��		ʧ�ܣ�������
*/
int UnionAutoAppendFldByAutoFldDef(PUnionAutoFldDef pautoFldDef,char *record,int lenOfRecord)
{
	int	ret,len;
	int	index;
	int	desNum,oriNum;
	char	oriFldNameGrp[10][128+1],desFldNameGrp[10][128+1];
	char	condition[512+1];
	int	lenOfCondition = 0;
	char	fldValue[256+1];
	int	lenOfFldValue;
	char	*oriFldName;
	
	// �۷�Ŀ���ؼ���
	if ((desNum = UnionSeprateVarStrIntoVarGrp(pautoFldDef->desConFldList,strlen(pautoFldDef->desConFldList),',',desFldNameGrp,10)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionSeprateVarStrIntoVarGrp [%s]!\n",pautoFldDef->desConFldList);
		return(desNum);
	}
	// �۷�Դ��ؼ���
	if ((oriNum = UnionSeprateVarStrIntoVarGrp(pautoFldDef->oriConFldList,strlen(pautoFldDef->oriConFldList),',',oriFldNameGrp,10)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionSeprateVarStrIntoVarGrp [%s]!\n",pautoFldDef->oriConFldList);
		return(oriNum);
	}
	// ƴװԴ��Ĺؼ���
	memset(condition,0,sizeof(condition));
	for (index = 0; index < desNum; index++)
	{
		// �����ݴ��ж�ȡ����
		memset(fldValue,0,sizeof(fldValue));
		if ((lenOfFldValue = UnionReadRecFldFromRecStr(record,lenOfRecord,desFldNameGrp[index],fldValue,sizeof(fldValue))) < 0)
		{
			if ((lenOfFldValue = UnionReadFldFromCurrentMngSvrClientReqStr(desFldNameGrp[index],fldValue,sizeof(fldValue))) < 0)
			{
				if ((ret = UnionReadSpecFldDefinedBySpecApp(desFldNameGrp[index],fldValue,sizeof(fldValue))) < 0)
				{
					UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionReadSpecFldDefinedBySpecApp [%s] from [%s]\n",desFldNameGrp[index],record);
					return(lenOfFldValue);
				}
				else
					lenOfFldValue = ret;
			}
		}
		// �����������뵽Դ��Ĳ�ѯ������
		if (index < oriNum)
			oriFldName = oriFldNameGrp[index];
		else	// Դ�����岻������ȡĿ�������������
			oriFldName = desFldNameGrp[index];
		if ((ret = UnionPutRecFldIntoRecStr(oriFldName,fldValue,lenOfFldValue,condition+lenOfCondition,sizeof(condition)-lenOfCondition)) < 0)
		{
			UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionPutRecFldIntoRecStr [%s] into [%s]\n",oriFldName,condition);
			return(ret);
		}
		lenOfCondition += ret;
	}
	// ��Դ���ж�ȡָ����
	memset(fldValue,0,sizeof(fldValue));
	if ((ret = UnionSelectSpecFldOfObjectByPrimaryKey(pautoFldDef->oriResName,condition,pautoFldDef->oriFldName,fldValue,sizeof(fldValue))) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionSelectSpecFldOfObjectByPrimaryKey [%s] from [%s] on [%s]\n",pautoFldDef->oriFldName,pautoFldDef->oriResName,condition);
		return(ret);
	}

	/**modify by xusj begin 20100302***/
	// ���������ڴ��У�����ɾ��
	len = UnionDeleteRecFldFromRecStr(record, pautoFldDef->desFldName);
	if (len == 0)
		len = lenOfRecord;

	// ��ָ������뵽Ŀ�괮��
	//if ((ret = UnionPutRecFldIntoRecStr(pautoFldDef->desFldName,fldValue,ret,record+lenOfRecord,lenOfRecord+ret+2)) < 0)
	if ((ret = UnionPutRecFldIntoRecStr(pautoFldDef->desFldName,fldValue,ret,record+len,len+ret+2)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionPutRecFldIntoRecStr [%s] of [%s] into [%s]\n",fldValue,pautoFldDef->desFldName,record);
		return(ret);
	}
	//return(lenOfRecord+ret);
	return(len+ret);
	/**modify by xusj end 20100302***/
}

int UnionIsAddKeyWordANDToCondition(char *condition)
{
	int	len=0;
	char	*ptr1=NULL;
	char	*ptr2=NULL;

	if ( (len=strlen(condition)) == 0 )
	{
		UnionSetIsAddKeyWordAND(0);	
		return 0;
	}

	if ( (ptr1=strstr(condition,"numOfPerPage")) == NULL )
	{
		UnionSetIsAddKeyWordAND(1);	
		return 0;
	}
	
	ptr2 = ptr1;
	ptr1 = strchr(ptr2, '|');
	if (ptr1 == NULL)
	{
		UnionSetIsAddKeyWordAND(0);	
		return 0;
	}

	if ( (ptr1-condition+1) < len )
		UnionSetIsAddKeyWordAND(1);	
	else
		UnionSetIsAddKeyWordAND(0);	

	return 0;
}
