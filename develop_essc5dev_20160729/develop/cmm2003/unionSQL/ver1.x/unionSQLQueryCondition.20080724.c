// Wolfgang Wang
// 2008/7/24

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "UnionStr.h"
#include "UnionLog.h"
#include "unionSQLQueryCondition.h"
#include "unionErrCode.h"

/*
功能	判断一个记录串是否符合条件
输入参数
	pfldCon		一个域比较条件
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	无
返回值
	>0		符合条件
	=0		不符合条件
	<0		错误代码
*/
int UnionIsRecStrFitFldCondition(PUnionFldQueryCondition pfldCon,char *recStr,int lenOfRecStr)
{
	char	fldValue[1024+1];
	int	lenOfFldValue;
	int	compResult;
	
	if ((pfldCon == NULL) || (recStr == NULL) || (lenOfRecStr <= 0))
	{
		UnionUserErrLog("in UnionIsRecStrFitFldCondition:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(fldValue,0,sizeof(fldValue));
	if ((lenOfFldValue = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,pfldCon->fldName,fldValue,sizeof(fldValue))) < 0)
	{
		UnionUserErrLog("in UnionIsRecStrFitFldCondition:: UnionReadRecFldFromRecStr [%s] from [%s]\n",pfldCon->fldName,recStr);
		return(lenOfFldValue);
	}
	if (pfldCon->fldRelation == conFldQueryConditionLike)	// 检查包含条件
	{
		if (strstr(fldValue,pfldCon->fldValue) == NULL)
			return(0);
		else
			return(1);
	}
	compResult = strcmp(pfldCon->fldValue,fldValue);
	if (pfldCon->fldRelation == conFldQueryConditionEqual)	// 检查=条件
	{
		if (compResult == 0)
			return(1);
		else
			return(0);
	}
	if (pfldCon->fldRelation == conFldQueryConditionLarger)	// 检查>条件
	{
		if (compResult > 0)
			return(1);
		else
			return(0);
	}
	if (pfldCon->fldRelation == conFldQueryConditionSmaller)	// 检查<条件
	{
		if (compResult < 0)
			return(1);
		else
			return(0);
	}
	if (pfldCon->fldRelation == conFldQueryConditionNotEqual)	// 检查<条件
	{
		if (compResult != 0)
			return(1);
		else
			return(0);
	}
	UnionUserErrLog("in UnionIsRecStrFitFldCondition:: invalid condition [%d]\n",pfldCon->fldRelation);
	return(errCodeSQLInvalidFldCompareCondition);
}

/*
功能	判断一个记录串是否符合条件
输入参数
	pcon		比较条件
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	无
返回值
	>0		符合条件
	=0		不符合条件
	<0		错误代码
*/
int UnionIsRecStrFitQueryCondition(PUnionQueryCondition pcon,char *recStr,int lenOfRecStr)
{
	int	ret;
	
	if (pcon == NULL)	// 条件是空
		return(1);
	// 先比较本条件
	if ((ret = UnionIsRecStrFitFldCondition(&(pcon->condition),recStr,lenOfRecStr)) < 0) // 执行出错
	{
		UnionUserErrLog("in UnionIsRecStrFitQueryCondition:: UnionIsRecStrFitFldCondition thisConditon [%s] [%s] [%d]!\n",
			pcon->condition.fldName,pcon->condition.fldValue,pcon->condition.fldRelation);
		return(ret);
	}
	// 满足本条件
	if (ret > 0)
	{
		// 获得与本条件是与关系的其它条件的比较结果
		if ((ret = UnionIsRecStrFitQueryCondition(pcon->pandList,recStr,lenOfRecStr)) < 0)	// 执行出错
		{
			UnionUserErrLog("in UnionIsRecStrFitQueryCondition:: UnionIsRecStrFitFldCondition pandList!\n");
			return(ret);
		}
		if (ret > 0)	// 与关系的条件满足
			return(ret);
	}
	// 不满足本条件及与本条件是与关系的条件
	// 比较与本条件列表是或关系的其它条件
	if (pcon->porList == NULL)	// 没有或条件
		return(0);
	return(UnionIsRecStrFitQueryCondition(pcon->porList,recStr,lenOfRecStr));
}
