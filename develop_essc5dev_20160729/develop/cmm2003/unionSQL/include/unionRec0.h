//---------------------------------------------------------------------------

#ifndef unionRecH
#define unionRecH
//---------------------------------------------------------------------------
// 定义一个记录
#define conMaxNumOfFldPerRec	64
typedef struct
{
	int		fldNum;
	char		fldName[conMaxNumOfFldPerRec][40+1];
	char		fldValue[conMaxNumOfFldPerRec][4096+1];
} TUnionRec;
typedef TUnionRec *PUnionRec;

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
int UnionReadRecFromRecStrRepeatPermitted(char *recStr,int lenOfRecStr,PUnionRec prec);

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
int UnionReadRecFromRecStr(char *recStr,int lenOfRecStr,PUnionRec prec);

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
int UnionPutRecIntoRecStr(PUnionRec prec,char *recStr,int sizeOfRecStr);

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
int UnionIsRecStrFitSpecConditon(char *recStr,int lenOfRecStr,char *conStr,int lenOfConStr);

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
int UnionIsRecStrFitAndCondition(PUnionRec pconRec,int index,char *recStr,int lenOfRecStr);

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
int UnionIsRecStrFitOrCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr);

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
int UnionIsRecStrFitNotCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr);

#endif
