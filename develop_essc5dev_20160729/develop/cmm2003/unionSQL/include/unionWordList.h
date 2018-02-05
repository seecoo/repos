// Wolfgang Wang
// 2008/7/24


#ifndef _unionWordList_
#define _unionWordList_

#define conFldQueryConditionLarger	1	// 大于该值
#define conFldQueryConditionEqual	2	// 等于该值
#define conFldQueryConditionSmaller	3	// 小于该值
#define conFldQueryConditionNotEqual	4	// 不等于该值
#define conFldQueryConditionLike	5	// 包含该值

typedef struct TWordList
{
	char			*word;
	struct	TWordList 	*next;
} TUnionWordList;
typedef TUnionWordList		*PUnionWordList;

/*
功能	初始化word串
输入参数
	word		值
输出参数
	无
返回值
	字符串指定
*/
PUnionWordList UnionNewWordList(char *word);

/*
功能	释放word串
输入参数
	plist		串
输出参数
	无
返回值
	无
*/
void UnionFreeWordList(PUnionWordList plist);


/*
功能	将一个字符串拆成字串
输入参数
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	无
返回值
	字符串指定
*/
PUnionWordList UnionConvertStrIntoWordList(char *recStr,int lenOfRecStr);

/*
功能	打印一个串
输入参数
	plist		串
	fileName	文件名称
输出参数
	无
返回值
	>=0		字数量
	<0		错误代码
*/
int UnionPrintWordListToFile(PUnionWordList plist,char *fileName);

#endif
