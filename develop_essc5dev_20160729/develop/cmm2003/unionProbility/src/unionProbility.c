// Wolfgang Wang
// 2008/1/17

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "probility.h"
#include "UnionLog.h"

// ��ʼ��һ���µļ�¼
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

// ����һ��ֵ��Ӧ�ļ�¼
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

// ���ҷ�ֵ
// �����ҵ��ķ�ֵ
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

// ��һ��ֵ�������
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

// ����һ����¼���ֵĸ���
void UnionPercentOfProbilityRec(PUnionProbilityRec precList,long totalEchoNum)
{
	if ((precList == NULL) || (totalEchoNum <= 0))
		return;
	precList->percent = precList->echoNum / (double)totalEchoNum;
	UnionPercentOfProbilityRec(precList->left,totalEchoNum);
	UnionPercentOfProbilityRec(precList->right,totalEchoNum);
}

// �ͷ�һ����¼
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

// ��ʾһ����¼
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

// ��һ��ֵ���ӵ����ʱ���
// ����ֵ��
// ���ر��еļ�¼��	
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

// ��ʼ��һ���µĸ��ʱ�
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

// ɾ�����ʱ�
void UnionDeleteProbilityGrp(PUnionProbilityGrp pgrp)
{
	if (pgrp == NULL)
		return;
	UnionDeleteProbilityRec(pgrp->precList);
	free(pgrp);
	pgrp = NULL;
	return;
}

// ��ʼͳ�Ƹ���
void UnionPercentAllRecOfProbilityGrp(PUnionProbilityGrp pgrp)
{
	if (pgrp == NULL)
		return;
	UnionPercentOfProbilityRec(pgrp->precList,pgrp->totalEchoNum);
	return;
}

// ��ȡĳ����¼
PUnionProbilityRec UnionFindRecInProbilityGrp(PUnionProbilityGrp pgrp,long key)
{
	if (pgrp == NULL)
	{
		UnionUserErrLog("in UnionFindRecInProbilityGrp:: pgrp is null!\n");
		return(NULL);
	}
	return(UnionFindProbilityRec(pgrp->precList,key));
}

// ���ҷ�ֵ������ֵ���ҵ��ķ�ֵ
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
	fprintf(tmpFp,"\n������ϸ��::\n");
	fprintf(tmpFp,"%12s %12s %12s\n","��ֵ","���ִ���","�ٷֱ�");
	UnionPrintProbilityRecToFile(pgrp->precList,tmpFp);
	fprintf(tmpFp,"�ܳ��ִ���::%ld ��ֵ����::%ld\n\n\n",pgrp->totalEchoNum,pgrp->keyNum);
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

	fprintf(tmpFp,"\n��ֵ�ο���::\n");
	fprintf(tmpFp,"%12s %12s\n","�ɹ���","��ֵ");
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

	fprintf(outFp,"\n����ͳ�Ʊ�::\n");
	fprintf(outFp,"%19s %4s\n","��ֵ��Χ","����");
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

// ������Сֵ
long UnionFindMinKeyOfProbilityRec(PUnionProbilityRec precList)
{
	long	levelKey;
	
	if (precList == NULL)
		return(-1);
	if (precList->left != NULL)
		return(UnionFindMinKeyOfProbilityRec(precList->left));
	return(precList->key);
}

// �������ֵ
long UnionFindMaxKeyOfProbilityRec(PUnionProbilityRec precList)
{
	long	levelKey;
	
	if (precList->right != NULL)
		return(UnionFindMaxKeyOfProbilityRec(precList->right));
	return(precList->key);
}	
