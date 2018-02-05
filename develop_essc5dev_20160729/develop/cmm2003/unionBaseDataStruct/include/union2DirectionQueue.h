// Wolfgang Wang
// 2008/1/17

#ifndef _union2DirectionQueue_
#define _union2DirectionQueue_

typedef struct TUnion2DirectionQueueRec
{
	char				*key;		// 主键
	char				lastUsedTime[14+1];	// 最近一次使用时间
	struct TUnion2DirectionQueueRec	*previous;	// 前一个记录
	struct TUnion2DirectionQueueRec	*next;		// 后一个记录
	unsigned char			*reserved;	// 保留值
	int				lenOfData;	// 保留数据的长度
} TUnion2DirectionQueueRec;
typedef TUnion2DirectionQueueRec	*PUnion2DirectionQueueRec;

// 显示所有数据
/*
输入参数
	prec		数据数目
	resID		资源号
输出参数
	无
返回值
	>=0		锁数目
	<0		错误码
*/
int UnionSpierAllRecIn2DirectionQueue(PUnion2DirectionQueueRec prec,int resID);

// 初始化一个新的记录
/*
输入参数
	key		关键字
	reserved	保留字
	sizeOfReserved	保留字长度
输出参数
	无
返回值
	成功		新记录指针
	失败		空指针
*/
PUnion2DirectionQueueRec UnionNew2DirectionQueueRec(char *key,unsigned char *reserved,int sizeOfReserved);

// 释放一个记录
/*
输入参数
	prec		记录指针
输出参数
	无
返回值
	无
*/
void UnionFree2DirectionQueueRec(PUnion2DirectionQueueRec prec);

// 查找一个值对应的记录
/*
输入参数
	prec		查找的起始位置指针
	key		关键字
输出参数
	无
返回值
	成功		记录指针
	失败		空指针
*/
PUnion2DirectionQueueRec UnionFind2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key);

// 将一个值插入表中
/*
输入参数
	prec		插入的起始位置的指针
	key		关键字
	reserved	保留字
	sizeOfReserved	保留字长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,出错码
*/
int UnionInsert2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key,unsigned char *reserved,int sizeOfReserved);		

// 将一个值从表中删除
/*
输入参数
	prec		记录指针
	key		关键字
输出参数
	无
返回值
	成功		>=0
	失败		<0,出错码
*/
int UnionDelete2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key);

// 显示表中的所有记录
/*
输入参数
	prec		记录指针
	fp		将所有记录显示到该文件
输出参数
	无
返回值
	成功		>=0
	失败		<0,出错码
*/
int UnionPrintAll2DirectionQueueRecToFile(PUnion2DirectionQueueRec prec,FILE *fp);

// 删除表中的所有记录
/*
输入参数
	prec		记录指针
输出参数
	无
返回值
	成功		>=0,删除的记录数
	失败		<0,出错码
*/
int UnionDeleteAll2DirectionQueueRec(PUnion2DirectionQueueRec prec);

#endif
