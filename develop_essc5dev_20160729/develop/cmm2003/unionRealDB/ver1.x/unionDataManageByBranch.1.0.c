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
功能：
	读取操作员的授权
输入参数:
	idOfOperator 	用户定义的，用于唯一识别一条记录的名称
输出参数:
	branchID	操作员所属机构
	dataReadAuth	操作员操作权限
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionReadAuthDefOfOperator(char *idOfOperator,char *branchID,int *dataReadAuth)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192*4+1];
	char	condition[1024+1];

	// 拼关键字
	memset(condition,0,sizeof(condition));
	ret = UnionPutRecFldIntoRecStr("idOfOperator",idOfOperator,strlen(idOfOperator),condition+lenOfRecStr,sizeof(condition)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionPutRecFldIntoRecStr [idOfOperator] failure!\n");
		return(ret);
	}
	lenOfRecStr += ret;

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameOperator,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
		return(lenOfRecStr);
	}

	//读机构号
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"branch",branchID,40+1)) < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionReadRecFldFromRecStr!\n");
		return(ret);
	}

	//读授权代码
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"dataReadAuth",dataReadAuth)) < 0)
	{
		UnionUserErrLog("in UnionReadAuthDefOfOperator:: UnionReadIntTypeRecFldFromRecStr!\n");
		return(ret);
	}

	return(ret);
}

/*
功能：
	判断是否分机构管理数据
输入参数：
	无
输出参数：
	无
返回值：
	1：		是
	0：		否
*/
int UnionIsManageDataByBranch()
{
	if (UnionReadIntTypeRECVar("isManageDataByBranch") > 0)
		return(1);
	else
		return(0);
}

/*
功能：
	读机构域的名称
输入参数：
	idOfObject：	对象ID
输出参数：
	branchFldName	机构域的名称
返回值：
	>=0：		成功，串长度
	<0：		失败，错误码
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
功能：
	自动赋加查询条件
输入参数：
	idOfObject：对象ID
	oriConLen:	源条件的长度
	condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
	sizeOfBuf	条件串缓冲的大小
输出参数：
	condition	赋加了自动查询域的条件串
返回值：
	>=0：		成功，串长度
	<0：		失败，错误码
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
	// 如果是导入导出数据工具则直接返回
	if (UnionGetIsOutPutInPutDataTools())
		return(oriConLen);

	if (!UnionIsManageDataByBranch())	// 本平台不对数据进行分机构管理
		return(oriConLen);
		
	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** reading operator authorization!\n");

	// 读表中是否赋加了该域的值
	memset(branchFldName,0,sizeof(branchFldName));
	UnionReadBranchFldNameOfSpecTBL(tblName,branchFldName);
	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** branchFldName = [%s]\n",branchFldName);
	if (strlen(branchFldName) == 0)	// 该表未定义机构域，即该表的数据不分机构管理
		return(oriConLen);	

	// 读操作员的操作授权定义
	memset(branchID,0,sizeof(branchID));
	if ((ret = UnionReadAuthDefOfOperator(UnionGetCurrentOperationTellerNo(),branchID,&readAllDataPermitted)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendBranchIDToQueryCondition:: UnionReadAuthDefOfOperator [%s]!\n",UnionGetCurrentOperationTellerNo());
		return(ret);
	}


	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** readAllDataPermitted = [%d]\n",readAllDataPermitted);
	if (readAllDataPermitted)	// 本操作员可以读所有数据
		return(oriConLen);
	
	//UnionLog("in UnionAutoAppendBranchIDToQueryCondition:: *** manage data by branch!\n");
	
	// 将机构号自动赋加到查询条件中
	//if (oriConLen > 0)
	if (UnionGetIsAddKeyWordAND())
	{
		strcpy(condition," AND ");
		offset = 5;
	}
	memset(realBranchFldName,0,sizeof(realBranchFldName));
	UnionChargeFieldNameToDBFieldName(branchFldName,realBranchFldName);
	// 2011-12-31 张永定修改
	//sprintf(condition+offset,"%s = '%s'",realBranchFldName,branchID);
	sprintf(condition+offset,"%s LIKE '%%%s%%'",realBranchFldName,branchID);
	return(ret+strlen(condition));
}

/*
功能：
	自动赋加数据串
输入参数：
	idOfObject：	对象ID
	record		记录串，"域1=域值|域2=域值|域3=域值|…域N=域值|"
	lenOfRecord	记录串长度
输出参数：
	record		赋加了自动域的记录串
返回值：
	>=0：		成功，串长度
	<0：		失败，错误码
*/
int UnionAutoAppendFldOfSpecTBL(char *idOfObject,char *record,int lenOfRecord)
{
	int			ret;
	TUnionAutoFldDef	autoFldDefGrp[20];
	int			num;
	char			condition[128+1];
	int			index;
	
	if (!UnionIsManageDataByBranch())	// 本平台不对数据进行分机构管理
		return(lenOfRecord);
	
	// 读取自动赋加的赋定义
	sprintf(condition,"%s=%s|",conAutoFldDefFldNameDesResName,idOfObject);
	if ((num = UnionBatchReadAutoFldDefRec(condition,autoFldDefGrp,20)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldOfSpecTBL:: UnionBatchReadAutoFldDefRec!\n");
		return(num);
	}
	
	// 根据自动赋加的赋定义，赋加自动赋加域
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
功能：
	自动赋加数据串
输入参数：
	pautoFldDef	自动赋加域定义
	record		记录串，"域1=域值|域2=域值|域3=域值|…域N=域值|"
	lenOfRecord	记录串长度
输出参数：
	record		赋加了自动域的记录串
返回值：
	>=0：		成功，串长度
	<0：		失败，错误码
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
	
	// 折分目标表关键字
	if ((desNum = UnionSeprateVarStrIntoVarGrp(pautoFldDef->desConFldList,strlen(pautoFldDef->desConFldList),',',desFldNameGrp,10)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionSeprateVarStrIntoVarGrp [%s]!\n",pautoFldDef->desConFldList);
		return(desNum);
	}
	// 折分源表关键字
	if ((oriNum = UnionSeprateVarStrIntoVarGrp(pautoFldDef->oriConFldList,strlen(pautoFldDef->oriConFldList),',',oriFldNameGrp,10)) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionSeprateVarStrIntoVarGrp [%s]!\n",pautoFldDef->oriConFldList);
		return(oriNum);
	}
	// 拼装源表的关键字
	memset(condition,0,sizeof(condition));
	for (index = 0; index < desNum; index++)
	{
		// 从数据串中读取数据
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
		// 将条件串并入到源表的查询条件中
		if (index < oriNum)
			oriFldName = oriFldNameGrp[index];
		else	// 源表串定义不完整，取目标表中域名定义
			oriFldName = desFldNameGrp[index];
		if ((ret = UnionPutRecFldIntoRecStr(oriFldName,fldValue,lenOfFldValue,condition+lenOfCondition,sizeof(condition)-lenOfCondition)) < 0)
		{
			UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionPutRecFldIntoRecStr [%s] into [%s]\n",oriFldName,condition);
			return(ret);
		}
		lenOfCondition += ret;
	}
	// 从源表中读取指定域
	memset(fldValue,0,sizeof(fldValue));
	if ((ret = UnionSelectSpecFldOfObjectByPrimaryKey(pautoFldDef->oriResName,condition,pautoFldDef->oriFldName,fldValue,sizeof(fldValue))) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendFldByAutoFldDef:: UnionSelectSpecFldOfObjectByPrimaryKey [%s] from [%s] on [%s]\n",pautoFldDef->oriFldName,pautoFldDef->oriResName,condition);
		return(ret);
	}

	/**modify by xusj begin 20100302***/
	// 如果域存在于串中，则先删除
	len = UnionDeleteRecFldFromRecStr(record, pautoFldDef->desFldName);
	if (len == 0)
		len = lenOfRecord;

	// 将指定域加入到目标串中
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
