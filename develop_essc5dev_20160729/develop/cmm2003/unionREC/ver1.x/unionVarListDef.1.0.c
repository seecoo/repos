// author	Wolfgang Wang
// date		2010-5-5

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"

#include "unionVarListDef.h"

/*
����
	��ʼ��һ������
���������
	varName		��������
	len		����ֵ
	varValue	����ֵ
�������
	��
����ֵ
	�ɹ�	����ָ��
	ʧ��	NULL
*/
PUnionVarValue UnionInitOneVar(char *varName,int len,char *varValue)
{
	PUnionVarValue	ptr;
	int		nameLen;
	
	if ((ptr = (PUnionVarValue)malloc(sizeof(*ptr))) == NULL)
	{
		UnionSystemErrLog("in UnionInitOneVar:: malloc [%zu]!\n",sizeof(*ptr));
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	if ((ptr->varName = (char *)malloc(nameLen=strlen(varName)+1)) == NULL)
	{
		free(ptr);
		UnionSystemErrLog("in UnionInitOneVar:: malloc [%d]!\n",nameLen);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	memcpy(ptr->varName,varName,nameLen);
	ptr->varName[nameLen] = 0;
	if ((ptr->varValue = (char *)malloc(len+1)) == NULL)
	{
		free(ptr->varName);
		free(ptr);
		UnionSystemErrLog("in UnionInitOneVar:: malloc [%d]!\n",nameLen);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	memcpy(ptr->varValue,varValue,len);
	ptr->varValue[len] = 0;
	ptr->len = len;
	return(ptr);
}

/*
����
	�ͷ�һ������
���������
	ptr		����ָ��
�������
	��
����ֵ
	��
*/
void UnionFreeOneVar(PUnionVarValue ptr)
{
	if (ptr == NULL)
		return;
	free(ptr->varName);
	free(ptr->varValue);
	return;
}

/*
����
	�Ǽ�һ������
���������
	pvarList	�����б�
	varName		��������
	len		����ֵ
	varValue	����ֵ
�������
	��
����ֵ
	>=0		��������
	<0		�������
*/
int UnionAddOneVarToVarList(PUnionVarList pvarList,char *varName,int len,char *varValue)
{
	if (pvarList == NULL)
		return(errCodeNullPointer);
	
	if (pvarList->varNum >= conMaxNumOfVarPerList)
	{
		UnionUserErrLog("in UnionAddOneVarToVarList:: pvarList->varNum = [%d] is full!\n",pvarList->varNum);
		return(errCodeRECMDL_VarTBLIsFull);
	}
	
	if ((pvarList->varList[pvarList->varNum] = UnionInitOneVar(varName,len,varValue)) == NULL)
	{
		UnionUserErrLog("in UnionAddOneVarToVarList:: UnionInitOneVar!\n");
		return(UnionGetUserDefinedErrorCode());
	}
	pvarList->varNum += 1;
	return(pvarList->varNum);
}

/*
����
	�ƻ�һ�������б�
���������
	pvarList	�����б�
�������
	��
����ֵ
	��
*/
void UnionFreeVarList(PUnionVarList pvarList)
{
	int	index;
	
	if (pvarList == NULL)
		return;
		
	for (index = 0; index < pvarList->varNum; index++)
		UnionFreeOneVar(pvarList->varList[index]);
	free(pvarList);
	pvarList = NULL;
}

/*
����
	����һ�������б�
���������
	varListName	�б�����
�������
	��
����ֵ
	�ɹ�	�����б�ָ��
	ʧ��	NULL
*/
PUnionVarList UnionCreateVarList(char *varListName)
{
	PUnionVarList	ptr;
	
	if ((ptr = (PUnionVarList)malloc(sizeof(*ptr))) == NULL)
	{
		UnionSystemErrLog("in UnionCreateVarList:: malloc [%zu]!\n",sizeof(*ptr));
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	memset(ptr,0,sizeof(*ptr));
	if (strlen(varListName) >= sizeof(ptr->varListName))
		memcpy(ptr->varListName,varListName,sizeof(ptr->varListName)-1);
	else
		strcpy(ptr->varListName,varListName);
	return(ptr);
}

/*
����
	��ȡһ������
���������
	pvarList	�����б�
	varName		��������
	sizeOfBuf	���������С
�������
	varValue	����ֵ
����ֵ
	>=0		��������
	<0		�������
*/
int UnionReadOneVarByVarName(PUnionVarList pvarList,char *varName,char *varValue,int sizeOfBuf)
{
	int		index;
	PUnionVarValue	ptr;
	int		copyLen;
	
	if (pvarList == NULL)
	{
		UnionUserErrLog("in UnionReadOneVarByVarName:: pvarList is NULL!\n");
		return(errCodeNullPointer);
	}
	for (index = 0; index < pvarList->varNum; index++)
	{
		ptr = pvarList->varList[index];
		if (strcmp(ptr->varName,varName) != 0)
			continue;
		if (sizeOfBuf > ptr->len)
			copyLen = ptr->len;
		else
			copyLen = sizeOfBuf - 1;
		if (copyLen < 0)
			copyLen = 0;
		memcpy(varValue,ptr->varValue,copyLen);
		return(copyLen);
	}
	UnionUserErrLog("in UnionReadOneVarByVarName:: [%s] not found in [%s]!\n",varName,pvarList->varListName);
	return(UnionSetUserDefinedErrorCode(errCodeRECMDL_VarNotExists));
}

/*
����
	��һ�������б�д����־��
���������
	pvarList	�����б�
	title		����
�������
	��
����ֵ
	��
*/
void UnionLogVarList(PUnionVarList pvarList,char *title)
{
	int		index;
	PUnionVarValue	ptr;
	
	if (pvarList == NULL)
		return;

	UnionNullLog("%s varListName = [%s] varNum = [%d]\n",title,pvarList->varListName,pvarList->varNum);
	for (index = 0; index < pvarList->varNum; index++)
	{
		ptr = pvarList->varList[index];
		UnionNullLog("[%03d] [%30s] [%04d] [%s]\n",index,ptr->varName,ptr->len,ptr->varValue);
	}
	return;
}


