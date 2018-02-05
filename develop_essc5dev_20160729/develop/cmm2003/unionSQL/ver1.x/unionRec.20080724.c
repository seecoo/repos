//---------------------------------------------------------------------------


//#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"

#include "unionRec0.h"
#include "unionCalcuOperator.h"

/*
功能	将一个字符串拆成记录域,允许重复包括一个域
输入参数
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	prec		读出的记录
返回值
	>=0		记录包括的域数目
	<0		错误码
*/
int UnionReadRecFromRecStrRepeatPermitted(char *recStr,int lenOfRecStr,PUnionRec prec)
{
	int	lenOfFldValue,lenOfFldName;
	int	offset = 0;
	int	thisFldIsError = 0;
	
	if ((recStr == NULL) || (lenOfRecStr < 0) || (prec == NULL))
	{
		UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: null pointer or lenOfRecStr = [%d]!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	memset(prec,0,sizeof(*prec));
	prec->fldNum = 0;
	if (lenOfRecStr == 0)
	{
		return(prec->fldNum);
	}

	for (;;)
	{
		if (prec->fldNum == conMaxNumOfFldPerRec)	// 记录域已满
			return(prec->fldNum);
		if (offset >= lenOfRecStr)			// 串读完了
			return(prec->fldNum);
		// 读域名称
		thisFldIsError = 0;
		for (lenOfFldName = 0;;)
		{
			if (offset >= lenOfRecStr)	// 串已读完
				return(prec->fldNum);
			if (recStr[offset] == '=')	// 
			{
				offset++;
				break;	// 域名称定义已完
			}
			if (((recStr[offset] >= 'a') && (recStr[offset] <= 'z')) || 
				((recStr[offset] >= '0') && (recStr[offset] <= '9')) ||
				((recStr[offset] >= 'A') && (recStr[offset] <= 'Z')) ||
                                (recStr[offset] == '.') || (recStr[offset] == '[') ||
                                (recStr[offset] == ']'))
			{
				if (lenOfFldName >= sizeof(prec->fldName[prec->fldNum]) - 1)
				{
					UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: the fldName of fldIndex [%d] is too long!\n",prec->fldNum);
					return(errCodeSQLRecFldNameTooLong);
				}
				prec->fldName[prec->fldNum][lenOfFldName] = recStr[offset];
				lenOfFldName++;
				offset++;
				continue;
			}
			else
			{
				if (recStr[offset] == '|')
				{
					thisFldIsError = 1;
					offset++;
					break;
				}

                                UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted the invalid sign is: [%c] in [%s]\n", recStr[offset],recStr);
				UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
				return(errCodeSQLInvalidFldName);
			}
		}
		if (lenOfFldName == 0)	// 未定义域名
		{
			UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
			//return(errCodeSQLNullFldName);
			continue;
		}
		if (thisFldIsError)
			continue;
		// 读域值
		for (lenOfFldValue = 0;;)
		{
			if (offset >= lenOfRecStr)	// 串已读完
				break;
			if (recStr[offset] == '|')	// 
			{
				offset++;
				break;	// 域值已读完
			}
			if (lenOfFldValue >= sizeof(prec->fldValue[prec->fldNum]) - 1)
				UnionAuditLog("in UnionReadRecFromRecStrRepeatPermitted:: the fldValue of fldIndex [%d] is too long!\n",prec->fldNum);
			else
			{
				prec->fldValue[prec->fldNum][lenOfFldValue] = recStr[offset];
				lenOfFldValue++;
			}
			offset++;
			continue;
		}
		prec->fldNum++;	
	}
}
/*
功能	将一个字符串拆成记录域
输入参数
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	prec		读出的记录
返回值
	>=0		记录包括的域数目
	<0		错误码
*/
int UnionReadRecFromRecStr(char *recStr,int lenOfRecStr,PUnionRec prec)
{
	int	lenOfFldValue,lenOfFldName;
	int	offset = 0;
	int	thisFldIsError = 0;
	int	index;
	
	if ((recStr == NULL) || (lenOfRecStr < 0) || (prec == NULL))
	{
		UnionUserErrLog("in UnionReadRecFromRecStr:: null pointer or lenOfRecStr = [%d]!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	memset(prec,0,sizeof(*prec));
	prec->fldNum = 0;
	if (lenOfRecStr == 0)
	{
		return(prec->fldNum);
	}

	for (;;)
	{
		if (prec->fldNum == conMaxNumOfFldPerRec)	// 记录域已满
			return(prec->fldNum);
		if (offset >= lenOfRecStr)			// 串读完了
			return(prec->fldNum);
		// 读域名称
		thisFldIsError = 0;
		for (lenOfFldName = 0;;)
		{
			if (offset >= lenOfRecStr)	// 串已读完
				return(prec->fldNum);
			if (recStr[offset] == '=')	// 
			{
				offset++;
				break;	// 域名称定义已完
			}
			if (((recStr[offset] >= 'a') && (recStr[offset] <= 'z')) || 
				((recStr[offset] >= '0') && (recStr[offset] <= '9')) ||
				((recStr[offset] >= 'A') && (recStr[offset] <= 'Z')) ||
                                (recStr[offset] == '.') || (recStr[offset] == '[') ||
                                (recStr[offset] == ']'))
			{
				if (lenOfFldName >= sizeof(prec->fldName[prec->fldNum]) - 1)
				{
					UnionUserErrLog("in UnionReadRecFromRecStr:: the fldName of fldIndex [%d] is too long!\n",prec->fldNum);
					return(errCodeSQLRecFldNameTooLong);
				}
				prec->fldName[prec->fldNum][lenOfFldName] = recStr[offset];
				lenOfFldName++;
				offset++;
				continue;
			}
			else
			{
				if (recStr[offset] == '|')
				{
					thisFldIsError = 1;
					offset++;
					break;
				}

                                UnionUserErrLog("in UnionReadRecFromRecStr the invalid sign is: [%c] in [%s]\n", recStr[offset],recStr);
				UnionUserErrLog("in UnionReadRecFromRecStr:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
				return(errCodeSQLInvalidFldName);
			}
		}
		if (lenOfFldName == 0)	// 未定义域名
		{
			UnionUserErrLog("in UnionReadRecFromRecStr:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
			//return(errCodeSQLNullFldName);
			continue;
		}
		if (thisFldIsError)
			continue;
		// 读域值
		for (lenOfFldValue = 0;;)
		{
			if (offset >= lenOfRecStr)	// 串已读完
				break;
			if (recStr[offset] == '|')	// 
			{
				offset++;
				break;	// 域值已读完
			}
			if (lenOfFldValue >= sizeof(prec->fldValue[prec->fldNum]) - 1)
				UnionAuditLog("in UnionReadRecFromRecStr:: the fldValue of fldIndex [%d] is too long!\n",prec->fldNum);
			else
			{
				prec->fldValue[prec->fldNum][lenOfFldValue] = recStr[offset];
				lenOfFldValue++;
			}
			offset++;
			continue;
		}
		// 2009/6/5，王纯军增加
		for (index = 0; index < prec->fldNum; index++)
		{
			if (strcmp(prec->fldName[index],prec->fldName[prec->fldNum]) == 0)	// 这个域已定义
				break;
		}
		if (index == prec->fldNum)	// 未定义
			prec->fldNum++;
		// 2009/6/5，王纯军增加结束
		//prec->fldNum++;	2009/6/5，王纯军删除
	}
}

/*
功能	将一个记录域写入到记录字符串
输入参数
	prec		记录
	sizeOfRecStr	记录串缓冲的大小
输出参数
	recStr		打成的记录串
返回值
	>=0		记录串的长度
	<0		错误码
*/
int UnionPutRecIntoRecStr(PUnionRec prec,char *recStr,int sizeOfRecStr)
{
	int	lenOfFldValue,lenOfFldName;
	int	offset = 0;
	int	index;
	int	thisFldLen;
	
	if ((recStr == NULL) || (sizeOfRecStr < 0) || (prec == NULL))
	{
		UnionUserErrLog("in UnionPutRecIntoRecStr:: null pointer or sizeOfRecStr = [%d]!\n",sizeOfRecStr);
		return(errCodeParameter);
	}
	for (index = 0; (index < prec->fldNum) && (index < conMaxNumOfFldPerRec); index++)
	{
		lenOfFldValue = strlen(prec->fldValue[index]);
		lenOfFldName = strlen(prec->fldName[index]);
		if ((thisFldLen = lenOfFldName + lenOfFldValue + 2) + offset >= sizeOfRecStr)
		{
			UnionUserErrLog("in UnionPutRecIntoRecStr:: sizeOfRecStr [%d] < expected [%d] when put fldIndex [%d]\n",sizeOfRecStr,thisFldLen + offset,index);
			return(errCodeSmallBuffer);
		}
		sprintf(recStr+offset,"%s=%s|",prec->fldName[index],prec->fldValue[index]);
		offset += thisFldLen;
	}
	if (prec->fldNum > conMaxNumOfFldPerRec)
	{
		UnionAuditLog("in UnionPutRecIntoRecStr:: fldNum [%d] too much, and [%d] fld not put into the recStr!\n",prec->fldNum,prec->fldNum-conMaxNumOfFldPerRec);
	}
	return(offset);
}

/*
功能	比较一个记录串是否满足条件
输入参数
	recStr		记录串
	lenOfRecStr	记录串长度
	conStr		条件串
	lenOfConStr	条件串长度
输出参数
	无
返回值
	>0		满足
	=0		不满足
	<0		函数出错
*/
int UnionIsRecStrFitSpecConditon(char *recStr,int lenOfRecStr,char *conStr,int lenOfConStr)
{
	int		ret;
	TUnionRec	conRec;
	int		index = 0;
	int		compType = conCalcuOperatorAnd;
	
	if ((lenOfConStr == 0) || (conStr == NULL))	// 未定义查询条件
		return(1);
			
	// 读取记录条件串
	memset(&conRec,0,sizeof(conRec));
	if ((ret = UnionReadRecFromRecStr(conStr,lenOfConStr,&conRec)) < 0)
	{
		UnionUserErrLog("in UnionIsRecStrFitSpecConditon:: UnionReadRecFromRecStr [%d] [%s]\n",lenOfConStr,conStr);
		return(ret);
	}
	
	// 读取条件符
	if (strcmp(conRec.fldName[0],"operator") == 0)
	{
		if ((compType = UnionConvertCalcuOperatorStrTagIntoIntTag(conRec.fldValue[0])) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitSpecConditon:: UnionConvertCalcuOperatorStrTagIntoIntTag [%s] ret = [%d]\n",conRec.fldValue[0],compType);
			return(compType);
		}
		index = 1;
	}
	
	switch (compType)
	{
		case	conCalcuOperatorAnd:
			return(UnionIsRecStrFitAndCondition(&conRec,index,recStr,lenOfRecStr));
		case	conCalcuOperatorOr:
			return(UnionIsRecStrFitOrCondition(&conRec,index,recStr,lenOfRecStr));
		case	conCalcuOperatorNot:
			return(UnionIsRecStrFitNotCondition(&conRec,index,recStr,lenOfRecStr));
		default:
			UnionUserErrLog("in UnionIsRecStrFitSpecConditon:: [%s] not valid relation operator!\n",conRec.fldValue[0]);
			return(errCodeCDPMDL_InvalidRelationCalcuOperator);
	}		
}

/*
功能	比较一个记录串是否满足所有与条件
输入参数
	pconRec		与条件
	index		从条件的该域开始比较
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	无
返回值
	>0		满足
	=0		不满足
	<0		函数出错
*/
int UnionIsRecStrFitAndCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr)
{
	int		index;
	char		fldValue[1024+1];
	int		lenOfFldValue;
	
	if ((pconRec == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	for (index = startIndex; index < pconRec->fldNum; index++)
	{
		memset(fldValue,0,sizeof(fldValue));
		if ((lenOfFldValue = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,pconRec->fldName[index],fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitAndCondition:: UnionReadRecFldFromRecStr [%s] from [%d] [%s]\n",pconRec->fldName[index],lenOfRecStr,recStr);
			return(lenOfFldValue);
		}
		if (strcmp(fldValue,pconRec->fldValue[index]) != 0)	// 比较结果
			return(0);
	}
	return(1);
}

/*
功能	比较一个记录串是否满足所有或条件
输入参数
	pconRec		条件
	index		从条件的该域开始比较
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	无
返回值
	>0		满足
	=0		不满足
	<0		函数出错
*/
int UnionIsRecStrFitOrCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr)
{
	int		index;
	char		fldValue[1024+1];
	int		lenOfFldValue;
	
	if ((pconRec == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	for (index = startIndex; index < pconRec->fldNum; index++)
	{
		memset(fldValue,0,sizeof(fldValue));
		if ((lenOfFldValue = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,pconRec->fldName[index],fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitOrCondition:: UnionReadRecFldFromRecStr [%s] from [%d] [%s]\n",pconRec->fldName[index],lenOfRecStr,recStr);
			return(lenOfFldValue);
		}
		if (strcmp(fldValue,pconRec->fldValue[index]) == 0)	// 比较结果
			return(1);
	}
	return(0);
}

/*
功能	比较一个记录串是否满足所有否条件
输入参数
	pconRec		条件
	index		从条件的该域开始比较
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	无
返回值
	>0		满足
	=0		不满足
	<0		函数出错
*/
int UnionIsRecStrFitNotCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr)
{
	int		index;
	char		fldValue[1024+1];
	int		lenOfFldValue;
	
	if ((pconRec == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	for (index = startIndex; index < pconRec->fldNum; index++)
	{
		memset(fldValue,0,sizeof(fldValue));
		if ((lenOfFldValue = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,pconRec->fldName[index],fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitNotCondition:: UnionReadRecFldFromRecStr [%s] from [%d] [%s]\n",pconRec->fldName[index],lenOfRecStr,recStr);
			return(lenOfFldValue);
		}
		if (strcmp(fldValue,pconRec->fldValue[index]) == 0)	// 比较结果
			return(0);
	}
	return(1);
}

//---------------------------------------------------------------------------

//#pragma package(smart_init)
