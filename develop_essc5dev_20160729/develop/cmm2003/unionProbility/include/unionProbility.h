// Wolfgang Wang
// 2008/1/17

#ifndef _probility_
#define _probility_

typedef struct tmpDef
{
	long			key;		// 概率计算的主值
	long			echoNum;	// 该值出现的次数
	double			percent;	// 该值出现的概率
	struct tmpDef		*left;		// 左节点
	struct tmpDef		*right;		// 右节点
	struct tmpDef		*parent;	// 父节点
} TUnionProbilityRec;
typedef TUnionProbilityRec	*PUnionProbilityRec;

typedef struct
{
	PUnionProbilityRec	precList;	// 中间记录
	long			totalEchoNum;	// 所有值出现的次数
	long			keyStep;	// 两个键值之间的步长
	long			keyNum;		// 记录数目
} TUnionProbilityGrp;
typedef TUnionProbilityGrp	*PUnionProbilityGrp;

// 初始化一个新的记录
PUnionProbilityRec UnionNewProbilityREC(long key);

// 查找一个值对应的记录
PUnionProbilityRec UnionFindProbilityRec(PUnionProbilityRec precList,long key);

// 查找阀值
// 返回找到的阀值
long UnionFindLevelKeyOfProbilityRec(PUnionProbilityRec precList,long minTotalEchoNum,long *currentTotalNum);

// 将一个值插入表中
PUnionProbilityRec UnionInsertProbilityRec(PUnionProbilityRec precList,long key,int *isNew);

// 计算一个记录出现的概率
void UnionPercentOfProbilityRec(PUnionProbilityRec precList,long totalEchoNum);

// 释放一个记录
void UnionDeleteProbilityRec(PUnionProbilityRec precList);

// 显示一个记录
void UnionPrintProbilityRecToFile(PUnionProbilityRec precList,FILE *fp);

// 将一个值增加到概率表中
// 返回值：
// 返回表中的记录数	
long UnionAddToProbilityGrp(PUnionProbilityGrp pgrp,long key);

// 初始化一个新的概率表
PUnionProbilityGrp UnionNewProbilityGrp();

// 删除概率表
void UnionDeleteProbilityGrp(PUnionProbilityGrp pgrp);

// 开始统计概率
void UnionPercentAllRecOfProbilityGrp(PUnionProbilityGrp pgrp);

// 获取某个记录
PUnionProbilityRec UnionFindRecInProbilityGrp(PUnionProbilityGrp pgrp,long key);

// 某个记录
PUnionProbilityRec UnionFindRecInProbilityGrp(PUnionProbilityGrp pgrp,long key);

// 打印概率明细表
void UnionPrintProbilityGrpToFile(PUnionProbilityGrp pgrp,FILE *fp);

// 打印概率统计表
int UnionGenerateProbilityReport(PUnionProbilityGrp pprobGrp,FILE *outFp);

// 查找最小值
long UnionFindMinKeyOfProbilityRec(PUnionProbilityRec precList);

// 查找最大值
long UnionFindMaxKeyOfProbilityRec(PUnionProbilityRec precList);

#endif
