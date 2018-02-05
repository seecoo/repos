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
功能
	初始化一个变量
输入参数：
	varName		变量名称
	len		变量值
	varValue	变量值
输出参数
	无
返回值
	成功	变量指针
	失败	NULL
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
功能
	释放一个变量
输入参数：
	ptr		变量指针
输出参数
	无
返回值
	无
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
功能
	登记一个变量
输入参数：
	pvarList	变量列表
	varName		变量名称
	len		变量值
	varValue	变量值
输出参数
	无
返回值
	>=0		变量数据
	<0		出错代码
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
功能
	破坏一个变量列表
输入参数：
	pvarList	变量列表
输出参数
	无
返回值
	无
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
功能
	创建一个变量列表
输入参数：
	varListName	列表名称
输出参数
	无
返回值
	成功	变量列表指针
	失败	NULL
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
功能
	读取一个变量
输入参数：
	pvarList	变量列表
	varName		变量名称
	sizeOfBuf	变量缓冲大小
输出参数
	varValue	变量值
返回值
	>=0		变量数据
	<0		出错代码
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
功能
	将一个变量列表写到日志中
输入参数：
	pvarList	变量列表
	title		标题
输出参数
	无
返回值
	无
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


