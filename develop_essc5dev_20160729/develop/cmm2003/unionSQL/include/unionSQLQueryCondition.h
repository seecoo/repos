// Wolfgang Wang
// 2008/7/24


#ifndef _unionSQLQueryCondition_
#define _unionSQLQueryCondition_

#define conFldQueryConditionLarger	1	// 大于该值
#define conFldQueryConditionEqual	2	// 等于该值
#define conFldQueryConditionSmaller	3	// 小于该值
#define conFldQueryConditionNotEqual	4	// 不等于该值
#define conFldQueryConditionLike	5	// 包含该值

typedef struct
{
	char	fldName[40+1];		// 域名称
	char	fldValue[256+1];	// 域的值
	int	fldRelation;		// 比较条件
} TUnionFldQueryCondition;
typedef TUnionFldQueryCondition	*PUnionFldQueryCondition;

typedef struct TFldQueryConditionList
{
	TUnionFldQueryCondition		condition;	// 本条件值
	struct TFldQueryConditionList	*pandList;	// 与本条件值是与关系的条件值
	struct TFldQueryConditionList	*porList;	// 与本条件值是或关系的条件值
} TUnionQueryCondition;
typedef TUnionQueryCondition		*PUnionQueryCondition;

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
int UnionIsRecStrFitFldCondition(PUnionFldQueryCondition pfldCon,char *recStr,int lenOfRecStr);

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
int UnionIsRecStrFitQueryCondition(PUnionQueryCondition pcon,char *recStr,int lenOfRecStr);

#endif
