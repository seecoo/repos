// Wolfgang Wang
// 2008/1/17

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "probility.h"
#include "UnionLog.h"

// 初始化一个新的记录
PUnionProbilityRec UnionNewProbilityREC(long key)
{
	PUnionProbilityRec	prec;
	
	if ((prec = (PUnionProbilityRec)malloc(sizeof(*prec))) == NULL)
	{
		UnionSystemErrLog("in UnionNewProbilityREC:: malloc!\n");
		return(NULL);
	}
	prec->left = prec->right = NULL;
	prec->key = key;
	prec->percent = -1;
	prec->echoNum = 1;
	prec->parent = NULL;
	return(prec);
}

// 查找一个值对应的记录
PUnionProbilityRec UnionFindProbilityRec(PUnionProbilityRec precList,long key)
{
	if (precList == NULL)
		return(NULL);
	if (key == precList->key)
		return(precList);
	if (key < precList->key)
		return(UnionFindProbilityRec(precList->left,key));
	else
		return(UnionFindProbilityRec(precList->right,key));
}

// 查找阀值
// 返回找到的阀值
long UnionFindLevelKeyOfProbilityRec(PUnionProbilityRec precList,long minTotalEchoNum,long *currentTotalNum)
{
	long	levelKey;
	
	if (precList == NULL)
		return(-1);
	if ((levelKey = UnionFindLevelKeyOfProbilityRec(precList->left,minTotalEchoNum,currentTotalNum)) >= 0)
		return(levelKey);
	if ((*currentTotalNum += precList->echoNum) >= minTotalEchoNum)
		return(levelKey = precList->key);
	return(UnionFindLevelKeyOfProbilityRec(precList->right,minTotalEchoNum,currentTotalNum));
}

// 将一个值插入表中
PUnionProbilityRec UnionInsertProbilityRec(PUnionProbilityRec precList,long key,int *isNew)
{
	PUnionProbilityRec	prec;
	
	if (precList == NULL)
	{
		if ((prec = UnionNewProbilityREC(key)) == NULL)
		{
			UnionUserErrLog("in UnionInsertProbilityRec:: UnionNewProbilityREC!\n");
			return(NULL);
		}
		*isNew = 1;
		prec->parent = NULL;
		return(prec);
	}
	if (key == precList->key)
	{
		++precList->echoNum;
		*isNew = 0;
		return(precList);
	}
	if (key < precList->key)
	{
		prec = UnionInsertProbilityRec(precList->left,key,isNew);
		if (precList->left == NULL)
		{
			prec->parent = precList;
			precList->left = prec;
		}
	}
	else
	{
		prec = UnionInsertProbilityRec(precList->right,key,isNew);
		if (precList->right == NULL)
		{
			prec->parent = precList;
			precList->right = prec;
		}
	}
	return(prec);
}

// 计算一个记录出现的概率
void UnionPercentOfProbilityRec(PUnionProbilityRec precList,long totalEchoNum)
{
	if ((precList == NULL) || (totalEchoNum <= 0))
		return;
	precList->percent = precList->echoNum / (double)totalEchoNum;
	UnionPercentOfProbilityRec(precList->left,totalEchoNum);
	UnionPercentOfProbilityRec(precList->right,totalEchoNum);
}

// 释放一个记录
void UnionDeleteProbilityRec(PUnionProbilityRec precList)
{
	if (precList == NULL)
		return;
	if (precList->left != NULL)
		UnionDeleteProbilityRec(precList->left);
	if (precList->right != NULL)
		UnionDeleteProbilityRec(precList->right);
	free(precList);
	precList = NULL;
	return;
}	

// 显示一个记录
void UnionPrintProbilityRecToFile(PUnionProbilityRec precList,FILE *fp)
{
	FILE	*tmpFp;
	
	if (fp == NULL)
		tmpFp = stdout;
	else
		tmpFp = fp;
		
	if (precList == NULL)
	{
		//printf("precList = NULL!\n");
		return;
	}
	UnionPrintProbilityRecToFile(precList->left,fp);
	fprintf(tmpFp,"%12ld %12ld %12.4f\n",precList->key,precList->echoNum,precList->percent * 100);
	UnionPrintProbilityRecToFile(precList->right,fp);
	return;
}

// 将一个值增加到概率表中
// 返回值：
// 返回表中的记录数	
long UnionAddToProbilityGrp(PUnionProbilityGrp pgrp,long key)
{
	int			ret;
	int			isNew;
	PUnionProbilityRec	prec;
	
	if (pgrp == NULL)
	{
		UnionUserErrLog("in UnionAddToProbilityGrp:: pgrp is null!\n");
		return(-1);
	}
	if ((prec = UnionInsertProbilityRec(pgrp->precList,key,&isNew)) == NULL)
	{
		UnionUserErrLog("in UnionAddToProbilityGrp:: UnionInsertProbilityRec!\n");
		return(-1);
	}
	++pgrp->totalEchoNum;
	if (isNew)
		++pgrp->keyNum;
	if (pgrp->precList == NULL)
		pgrp->precList = prec;
	return(pgrp->keyNum);		
}

// 初始化一个新的概率表
PUnionProbilityGrp UnionNewProbilityGrp()
{
	PUnionProbilityGrp	pgrp;
	
	if ((pgrp = (PUnionProbilityGrp)malloc(sizeof(*pgrp))) == NULL)
	{
		UnionUserErrLog("in UnionNewProbilityGrp:: malloc!\n");
		return(NULL);
	}
	pgrp->totalEchoNum = 0;
	pgrp->keyNum = 0;
	pgrp->precList = NULL;
	return(pgrp);
}

// 删除概率表
void UnionDeleteProbilityGrp(PUnionProbilityGrp pgrp)
{
	if (pgrp == NULL)
		return;
	UnionDeleteProbilityRec(pgrp->precList);
	free(pgrp);
	pgrp = NULL;
	return;
}

// 开始统计概率
void UnionPercentAllRecOfProbilityGrp(PUnionProbilityGrp pgrp)
{
	if (pgrp == NULL)
		return;
	UnionPercentOfProbilityRec(pgrp->precList,pgrp->totalEchoNum);
	return;
}

// 获取某个记录
PUnionProbilityRec UnionFindRecInProbilityGrp(PUnionProbilityGrp pgrp,long key)
{
	if (pgrp == NULL)
	{
		UnionUserErrLog("in UnionFindRecInProbilityGrp:: pgrp is null!\n");
		return(NULL);
	}
	return(UnionFindProbilityRec(pgrp->precList,key));
}

// 查找阀值，返回值是找到的阀值
long UnionFindLevelKeyInProbilityGrp(PUnionProbilityGrp pgrp,double percent)
{
	double	percentNum;
	int	found;
	long	levelKey;
	long	currentTotalNum = 0;
	
	if ((pgrp == NULL) || (pgrp->totalEchoNum == 0))
	{
		UnionUserErrLog("in UnionFindLevelKeyInProbilityGrp:: pgrp is NULL!\n");
		return(-1);
	}
	percentNum = percent / 100 * pgrp->totalEchoNum;
	levelKey = UnionFindLevelKeyOfProbilityRec(pgrp->precList,(long)percentNum,&currentTotalNum);
	//if (levelKey < 0)
		//printf("percent = %4.2lf percentNum = %12.4lf %ld currentTotalNum = %ld\n",percent,percentNum,(long)percentNum,currentTotalNum);
	return(levelKey);
}

void UnionPrintProbilityGrpToFile(PUnionProbilityGrp pgrp,FILE *fp)
{
	FILE	*tmpFp;
	
	if (pgrp == NULL)
		return;
	if (fp == NULL)
		tmpFp = stdout;
	else
		tmpFp = fp;
	fprintf(tmpFp,"\n概率明细表::\n");
	fprintf(tmpFp,"%12s %12s %12s\n","键值","出现次数","百分比");
	UnionPrintProbilityRecToFile(pgrp->precList,tmpFp);
	fprintf(tmpFp,"总出现次数::%ld 键值总数::%ld\n\n\n",pgrp->totalEchoNum,pgrp->keyNum);
	return;
}

void UnionPrintAllLevelKeyOfProbilityGrpToFile(PUnionProbilityGrp pgrp,FILE *fp)
{
	FILE	*tmpFp;
	long	index;
	
	if (pgrp == NULL)
		return;
	if (fp == NULL)
		tmpFp = stdout;
	else
		tmpFp = fp;

	fprintf(tmpFp,"\n阀值参考表::\n");
	fprintf(tmpFp,"%12s %12s\n","成功率","阀值");
	for (index = 1; index < 10; index += 1)
		fprintf(tmpFp,"%12.2lf %12d\n",(double)index,UnionFindLevelKeyInProbilityGrp(pgrp,(double)index));
	for (index = 10; index < 90; index += 10)
		fprintf(tmpFp,"%12.2lf %12d\n",(double)index,UnionFindLevelKeyInProbilityGrp(pgrp,(double)index));
	for (index = 90; index < 100; index++)
		fprintf(tmpFp,"%12.2lf %12d\n",(double)index,UnionFindLevelKeyInProbilityGrp(pgrp,(double)index));
	for (index = 1; index < 100; index++)
		fprintf(tmpFp,"%12.2lf %12d\n",99+(double)index/(double)100,UnionFindLevelKeyInProbilityGrp(pgrp,99+(double)index/(double)100));
	fprintf(tmpFp,"\n\n");
	//fprintf(tmpFp,"%12.4lf %12d\n",100,UnionFindLevelKeyInProbilityGrp(pgrp,100));
	return;
}
	
int UnionGenerateProbilityReport(PUnionProbilityGrp pprobGrp,FILE *outFp)
{
	int		index;
	long		levelKey;
	long		lastKey = 0,currentKey;
	double		percent;
	double		lastPercentSum;
	
	if ((pprobGrp == NULL) || (outFp == NULL) || (pprobGrp->totalEchoNum == 0) || (pprobGrp->keyNum == 0))
		return;	

	fprintf(outFp,"\n概率统计表::\n");
	fprintf(outFp,"%19s %4s\n","键值范围","概率");
	lastPercentSum = 0.00;
	lastKey = UnionFindMinKeyOfProbilityRec(pprobGrp->precList);
	for (index = 1; index < 100; index++)
	{
		percent = (double)index;
		currentKey = UnionFindLevelKeyInProbilityGrp(pprobGrp,percent);
		if (currentKey - lastKey < 10)
			continue;
		if (currentKey > lastKey)
		{
			fprintf(outFp,"(%8ld,%8ld] %4.2lf%%\n",lastKey,currentKey,percent - lastPercentSum);
			lastPercentSum = percent;
			lastKey = currentKey;
		}
		continue;
	}
	fprintf(outFp,"(%8ld,%8ld] %4.2lf%%\n",lastKey,UnionFindMaxKeyOfProbilityRec(pprobGrp->precList),100-lastPercentSum);
	fprintf(outFp,"\n");
	return;
}

// 查找最小值
long UnionFindMinKeyOfProbilityRec(PUnionProbilityRec precList)
{
	long	levelKey;
	
	if (precList == NULL)
		return(-1);
	if (precList->left != NULL)
		return(UnionFindMinKeyOfProbilityRec(precList->left));
	return(precList->key);
}

// 查找最大值
long UnionFindMaxKeyOfProbilityRec(PUnionProbilityRec precList)
{
	long	levelKey;
	
	if (precList->right != NULL)
		return(UnionFindMaxKeyOfProbilityRec(precList->right));
	return(precList->key);
}	
