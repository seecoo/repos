// Wolfgang Wang
// 2008/7/24


#ifndef _unionSQLQueryCondition_
#define _unionSQLQueryCondition_

#define conFldQueryConditionLarger	1	// ���ڸ�ֵ
#define conFldQueryConditionEqual	2	// ���ڸ�ֵ
#define conFldQueryConditionSmaller	3	// С�ڸ�ֵ
#define conFldQueryConditionNotEqual	4	// �����ڸ�ֵ
#define conFldQueryConditionLike	5	// ������ֵ

typedef struct
{
	char	fldName[40+1];		// ������
	char	fldValue[256+1];	// ���ֵ
	int	fldRelation;		// �Ƚ�����
} TUnionFldQueryCondition;
typedef TUnionFldQueryCondition	*PUnionFldQueryCondition;

typedef struct TFldQueryConditionList
{
	TUnionFldQueryCondition		condition;	// ������ֵ
	struct TFldQueryConditionList	*pandList;	// �뱾����ֵ�����ϵ������ֵ
	struct TFldQueryConditionList	*porList;	// �뱾����ֵ�ǻ��ϵ������ֵ
} TUnionQueryCondition;
typedef TUnionQueryCondition		*PUnionQueryCondition;

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
int UnionIsRecStrFitFldCondition(PUnionFldQueryCondition pfldCon,char *recStr,int lenOfRecStr);

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
int UnionIsRecStrFitQueryCondition(PUnionQueryCondition pcon,char *recStr,int lenOfRecStr);

#endif
