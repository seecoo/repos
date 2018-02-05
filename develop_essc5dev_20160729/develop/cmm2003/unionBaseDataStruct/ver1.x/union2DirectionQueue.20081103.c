// Wolfgang Wang
// 2008/11/03

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "union2DirectionQueue.h"

int UnionSpierSpecRecOfSpecRes(char fldValue[][1024+1],int fldNum,int resID);

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
PUnion2DirectionQueueRec UnionNew2DirectionQueueRec(char *key,unsigned char *reserved,int sizeOfReserved)
{
	PUnion2DirectionQueueRec	prec;
	int				keyLen;
	
	if ((key == NULL) || ((keyLen = strlen(key)) == 0))
	{
		UnionUserErrLog("in UnionNew2DirectionQueueRec:: parameter error!\n");
		return(NULL);
	}
	if ((prec = (PUnion2DirectionQueueRec)malloc(sizeof(*prec))) == NULL)
	{
		UnionSystemErrLog("in UnionNew2DirectionQueueRec:: malloc!\n");
		return(NULL);
	}
	prec->previous = prec->next = NULL;
	if ((prec->key = (char *)malloc(sizeof(char) * (keyLen+1))) == NULL)
	{
		UnionSystemErrLog("in UnionNew2DirectionQueueRec:: malloc!\n");
		free(prec);
		return(NULL);
	}
	memset(prec->key,0,keyLen+1);
	memcpy(prec->key,key,keyLen);
	if ((reserved != NULL) || (sizeOfReserved > 0))
	{	
		if ((prec->reserved = (unsigned char *)malloc(sizeof(unsigned char) * sizeOfReserved+1)) == NULL)
		{
			UnionSystemErrLog("in UnionNew2DirectionQueueRec:: malloc!\n");
			free(prec->key);
			free(prec);
			return(NULL);
		}
		memset(prec->reserved,0,sizeOfReserved+1);
		memcpy(prec->reserved,reserved,sizeOfReserved);
		prec->lenOfData = sizeOfReserved;
	}
	else
	{
		prec->reserved = NULL;
		prec->lenOfData = 0;
	}
	memset(prec->lastUsedTime,0,sizeof(prec->lastUsedTime));
	UnionGetFullSystemDateTime(prec->lastUsedTime);
	return(prec);
}

// 释放一个记录
/*
输入参数
	prec		记录指针
输出参数
	无
返回值
	无
*/
void UnionFree2DirectionQueueRec(PUnion2DirectionQueueRec prec)
{
	if (prec == NULL)
		return;
	if (prec->previous != NULL)
		prec->previous = prec->next;
	free(prec->key);
	if (prec->reserved)
		free(prec->reserved);
	free(prec);
	prec = NULL;
	return;
}	

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
PUnion2DirectionQueueRec UnionFind2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key)
{
	int	ret;
	
	if ((prec == NULL) || (key == NULL))
	{
		UnionProgramerLog("in UnionFind2DirectionQueueRec:: [%s] not exists!\n",key);
		return(NULL);
	}
	if ((ret = strcmp(key,prec->key)) == 0)
	{
		memset(prec->lastUsedTime,0,sizeof(prec->lastUsedTime));
		UnionGetFullSystemDateTime(prec->lastUsedTime);
		UnionProgramerLog("in UnionFind2DirectionQueueRec:: [%s] exists!\n",key);
		return(prec);
	}
	return(UnionFind2DirectionQueueRec(prec->next,key));
}

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
int UnionInsert2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key,unsigned char *reserved,int sizeOfReserved)
{
	PUnion2DirectionQueueRec	ptmpRec;
	
	if (prec == NULL)
	{
		UnionUserErrLog("in UnionInsert2DirectionQueueRec:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((ptmpRec = UnionFind2DirectionQueueRec(prec,key)) != NULL)
	{
		UnionUserErrLog("in UnionInsert2DirectionQueueRec:: [%s] already exists!\n",key);
		return(errCodeKeyWordAlreadyExists);
	}
	if ((ptmpRec = UnionNew2DirectionQueueRec(key,reserved,sizeOfReserved)) == NULL)
	{
		UnionUserErrLog("in UnionInsert2DirectionQueueRec:: UnionNew2DirectionQueueRec!\n");
		return(errCodeUseOSErrCode);
	}
	ptmpRec->previous = prec;
	ptmpRec->next = prec->next;
	if (ptmpRec->next != NULL)
		ptmpRec->next->previous = ptmpRec;
	prec->next = ptmpRec;
	return(0);
}		

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
int UnionDelete2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key)
{
	PUnion2DirectionQueueRec	ptmpRec;
	
	if ((ptmpRec = UnionFind2DirectionQueueRec(prec,key)) == NULL)
	{
		UnionUserErrLog("in UnionDelete2DirectionQueueRec:: [%s] not exists!\n",key);
		return(errCodeKeyWordNotExists);
	}
	if (prec == ptmpRec)
	{
		return(errCodeKeyWordIsMyself);
	}
	printf("key = [%s]\n",ptmpRec->key);
	if (ptmpRec->previous != NULL)
		ptmpRec->previous->next = ptmpRec->next;
	if (ptmpRec->next != NULL)
		ptmpRec->next->previous = ptmpRec->previous;
	UnionFree2DirectionQueueRec(ptmpRec);
	return(0);
}

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
int UnionPrintAll2DirectionQueueRecToFile(PUnion2DirectionQueueRec prec,FILE *fp)
{
	PUnion2DirectionQueueRec	ptmpRec;
	FILE				*tmpFp = stdout;
	int				num = 0;
	
	ptmpRec = prec;
	if (fp != NULL)
		tmpFp = fp;
	while (ptmpRec != NULL)
	{
		fprintf(tmpFp,"%s %40s",ptmpRec->lastUsedTime,ptmpRec->key);
		if (ptmpRec->reserved != NULL)
			fprintf(tmpFp," %04d %s\n",ptmpRec->lenOfData,ptmpRec->reserved);
		else
			fprintf(tmpFp,"\n");
		ptmpRec = ptmpRec->next;
		num++;
	}
	if (num > 0)
		fprintf(tmpFp,"total num = [%d]\n",num);
	return(num);
}

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
int UnionSpierAllRecIn2DirectionQueue(PUnion2DirectionQueueRec prec,int resID __attribute__((unused)))
{
	PUnion2DirectionQueueRec	ptmpRec;
	int				num = 0;
	char				fldValue[20][1024+1];
	int				fldIndex;
	int				i;

	ptmpRec = prec;
	while (ptmpRec != NULL)
	{
		fldIndex = 0;
		strcpy(fldValue[fldIndex],ptmpRec->lastUsedTime);
		fldIndex++;
		strcpy(fldValue[fldIndex],ptmpRec->key);
		fldIndex++;
		memset(fldValue[fldIndex],0,sizeof(fldValue[fldIndex]));
		if (ptmpRec->reserved != NULL)
		{
			if (ptmpRec->lenOfData >= (int)sizeof(fldValue[fldIndex]))
			{
				memcpy(fldValue[fldIndex],ptmpRec->reserved,sizeof(fldValue[fldIndex])-4);
				memset(fldValue[fldIndex]+sizeof(fldValue[fldIndex])-4,'.',3);
			}
			else
				memcpy(fldValue[fldIndex],ptmpRec->reserved,ptmpRec->lenOfData);
		}
#ifndef _openSpierAPI_
		for (i = 0; i <= fldIndex; i++)
			fprintf(stdout,"%s|",fldValue[i]);
		fprintf(stdout,"\n");
#else
		UnionSpierSpecRecOfSpecRes(fldValue,fldIndex,resID);
#endif
		ptmpRec = ptmpRec->next;
		num++;
	}
	return(num);
}

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
int UnionDeleteAll2DirectionQueueRec(PUnion2DirectionQueueRec prec)
{
	PUnion2DirectionQueueRec	ptmpRec,pcurRec;
	int				num = 0;
	
	ptmpRec = prec;
	while (ptmpRec != NULL)
	{
		pcurRec = ptmpRec;
		ptmpRec = ptmpRec->next;
		UnionFree2DirectionQueueRec(pcurRec);
		pcurRec = NULL;
		num++;
	}
	return(num);
}
