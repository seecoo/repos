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
����	�ж�һ����¼���Ƿ��������
�������
	pfldCon		һ����Ƚ�����
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		��������
	=0		����������
	<0		�������
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
	if (pfldCon->fldRelation == conFldQueryConditionLike)	// ����������
	{
		if (strstr(fldValue,pfldCon->fldValue) == NULL)
			return(0);
		else
			return(1);
	}
	compResult = strcmp(pfldCon->fldValue,fldValue);
	if (pfldCon->fldRelation == conFldQueryConditionEqual)	// ���=����
	{
		if (compResult == 0)
			return(1);
		else
			return(0);
	}
	if (pfldCon->fldRelation == conFldQueryConditionLarger)	// ���>����
	{
		if (compResult > 0)
			return(1);
		else
			return(0);
	}
	if (pfldCon->fldRelation == conFldQueryConditionSmaller)	// ���<����
	{
		if (compResult < 0)
			return(1);
		else
			return(0);
	}
	if (pfldCon->fldRelation == conFldQueryConditionNotEqual)	// ���<����
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
����	�ж�һ����¼���Ƿ��������
�������
	pcon		�Ƚ�����
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		��������
	=0		����������
	<0		�������
*/
int UnionIsRecStrFitQueryCondition(PUnionQueryCondition pcon,char *recStr,int lenOfRecStr)
{
	int	ret;
	
	if (pcon == NULL)	// �����ǿ�
		return(1);
	// �ȱȽϱ�����
	if ((ret = UnionIsRecStrFitFldCondition(&(pcon->condition),recStr,lenOfRecStr)) < 0) // ִ�г���
	{
		UnionUserErrLog("in UnionIsRecStrFitQueryCondition:: UnionIsRecStrFitFldCondition thisConditon [%s] [%s] [%d]!\n",
			pcon->condition.fldName,pcon->condition.fldValue,pcon->condition.fldRelation);
		return(ret);
	}
	// ���㱾����
	if (ret > 0)
	{
		// ����뱾���������ϵ�����������ıȽϽ��
		if ((ret = UnionIsRecStrFitQueryCondition(pcon->pandList,recStr,lenOfRecStr)) < 0)	// ִ�г���
		{
			UnionUserErrLog("in UnionIsRecStrFitQueryCondition:: UnionIsRecStrFitFldCondition pandList!\n");
			return(ret);
		}
		if (ret > 0)	// ���ϵ����������
			return(ret);
	}
	// �����㱾�������뱾���������ϵ������
	// �Ƚ��뱾�����б��ǻ��ϵ����������
	if (pcon->porList == NULL)	// û�л�����
		return(0);
	return(UnionIsRecStrFitQueryCondition(pcon->porList,recStr,lenOfRecStr));
}
