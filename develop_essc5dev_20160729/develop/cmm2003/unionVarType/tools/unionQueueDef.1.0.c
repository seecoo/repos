// author	Wolfgang Wang
// date		2010-5-5

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"

#include "unionQueueDef.h"

/*
函数功能
        从队列中读下一条数据
输入参数
       pqueue		队列指针 
	sizeOfData	数据缓冲大小
输出参数
       无
返回值
       >=0	成功，返回数据长度
       <0	失败，错误码
*/
int UnionReadNextDataFromQueue(PUnionQueue pqueue,unsigned char *data,int sizeOfData)
{
	int	dataLen = 0;
	
	if ((pqueue == NULL) || (data == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	if (pqueue->currentItem == NULL)
	{
		UnionLog("in UnionReadNextDataFromQueue:: all data read!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNoDataFoundInQueue));
	}
	if (pqueue->currentItem->dataLen >= sizeOfData)
	{
		UnionUserErrLog("in UnionReadNextDataFromQueue:: dataLen = [%d] > sizeOfData [%d]\n",pqueue->currentItem->dataLen,sizeOfData);
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	memcpy(data,pqueue->currentItem->data,dataLen = pqueue->currentItem->dataLen);
	pqueue->currentItem = pqueue->currentItem->next;
	return(dataLen);
}

/*
函数功能
        为读打开一个队列
输入参数
       pqueue		队列指针 
输出参数
       无
返回值
       >=0	成功
       <0	失败，错误码
*/
int UnionOpenQueueForRead(PUnionQueue pqueue)
{
	if (pqueue == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	pqueue->currentItem = pqueue->firstItem;
	return(0);
}

/*
函数功能
        申请一个队列项
输入参数
       dataLen 
       data 
输出参数
       无
返回值
       成功：队列项指针；失败：空指针
*/
PUnionQueueItem UnionNewQueueItem(int dataLen,unsigned char *data)
{
	PUnionQueueItem		pitem;
	
	if ((pitem = (PUnionQueueItem)malloc(sizeof(*pitem))) == NULL)
	{
		UnionUserErrLog("in UnionNewQueueItem:: malloc!\n");
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	
	memset(pitem,0,sizeof(*pitem));
	
	if ((data == NULL) || (dataLen <= 0))
		return(pitem);

	// 赋值队列项数据	
	if ((pitem->data = (unsigned char *)malloc(dataLen+1)) == NULL)
	{
		UnionUserErrLog("in UnionNewQueueItem:: malloc data!\n");
		free(pitem);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	memcpy(pitem->data,data,dataLen);
	pitem->data[dataLen] = 0;
	pitem->dataLen = dataLen;
	return(pitem);
}

/*
函数功能
        释放一个队列项
输入参数
       pqueueItem 
输出参数
       无
返回值
       >=0，数据项的长度；<0，出错，错误代码
*/
int UnionFreeQueueItem(PUnionQueueItem pqueueItem)
{
	if (pqueueItem == NULL)
		return(errCodeNullPointer);
	// 释放本队列项之后的项	
	UnionFreeQueueItem(pqueueItem->next);
	// 释放队列项数据
	if (pqueueItem->data != NULL)
		free(pqueueItem->data);
	// 释放本队列项
	free(pqueueItem);
	return(0);
}

/*
函数功能
        申请一个队列
输入参数
       无
输出参数
       无
返回值
       成功：队列指针；失败：空指针
*/
PUnionQueue UnionNewQueue()
{
	PUnionQueue	pqueue;

	if ((pqueue = (PUnionQueue)malloc(sizeof(*pqueue))) == NULL)
	{
		UnionUserErrLog("in UnionNewQueue:: malloc!\n");
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	
	memset(pqueue,0,sizeof(*pqueue));
	
	return(pqueue);
}

/*
函数功能
        释放一个队列
输入参数
       pqueue 
输出参数
       无
返回值
       >=0,队列中的数据项数目；<0,错误代码
*/
int UnionFreeQueue(PUnionQueue pqueue)
{
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// 释放第一个队列项
	UnionFreeQueueItem(pqueue->firstItem);
	// 释放队列
	free(pqueue);
	return(0);
}

/*
函数功能
        以字符串形式打印一个队列项数据
输入参数
       pqueueItem 	队列项指针
       fp		文件指针
输出参数
       无
返回值
       >=0，数据项的长度；<0，出错，错误代码
*/
int UnionPrintQueueItemDataAsStringToFp(PUnionQueueItem pqueueItem,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pqueueItem == NULL)
		return(0);
	if (fp != NULL)
		outFp = fp;
	// 打印本队列项
	fprintf(outFp,"%s\n",pqueueItem->data);
	// 打印本队列项之后的项	
	return(UnionPrintQueueItemDataAsStringToFp(pqueueItem->next,outFp));
}

/*
函数功能
        以字符串形式打印一个队列中的数据
输入参数
       pqueue 	队列指针
       fp	文件指针
输出参数
       无
返回值
       >=0,成功
       <0,错误代码
*/
int UnionPrintQueueDataAsStringToFp(PUnionQueue pqueue,FILE *fp)
{
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// 打印第一个队列项
	return(UnionPrintQueueItemDataAsStringToFp(pqueue->firstItem,fp));
}

/*
函数功能
        以字符串形式打印一个队列中的数据到指定文件
输入参数
       pqueue 	队列指针
       fileName	文件名称
输出参数
       无
返回值
       >=0,成功
       <0,错误代码
*/
int UnionPrintQueueDataAsStringToSpecFile(PUnionQueue pqueue,char *fileName)
{
	FILE			*fp;
	int			ret;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintQueueDataAsStringToSpecFile:: fopen [%s]!\n",fileName);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	ret = UnionPrintQueueDataAsStringToFp(pqueue,fp);
	fclose(fp);
	return(ret);
}


/*
函数功能
        将数据放入到队列尾部
输入参数
       pqueue 
       dataLen 
       data 
输出参数
       无
返回值
       >=0,队列中的数据项数目；<0,错误代码
*/
int UnionAppendDataToQueue(PUnionQueue pqueue,int dataLen,unsigned char *data)
{
	PUnionQueueItem pqueueItem;
	
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// 申请一个新项
	if ((pqueueItem = UnionNewQueueItem(dataLen,data)) == NULL)
	{
		UnionUserErrLog("in UnionAppendDataToQueue:: UnionNewQueueItem!\n");
		return(UnionGetUserDefinedErrorCode());
	}
	// 新项的前一个项指向队列的最后一个项
	pqueueItem->previous = pqueue->lastItem;
	
	// 将原来的最后一个项的下一个项，指向新项
	if (pqueue->lastItem != NULL)
		pqueue->lastItem->next = pqueueItem;
	// 将新项置为最后一个项
	pqueue->lastItem = pqueueItem;
	
	// 队列是空
	if (pqueue->firstItem == NULL)	
		pqueue->firstItem = pqueueItem;
	
	// 队列中项数目增1
	pqueue->itemNum += 1;
	
	return(0);
}

/*
函数功能
        将数据放入到队列头部
输入参数
       pqueue 
       dataLen 
       data 
输出参数
       无
返回值
       >=0,队列中的数据项数目；<0,错误代码
*/
int UnionPutDataIntoQueue(PUnionQueue pqueue,int dataLen,unsigned char *data)
{
	PUnionQueueItem pqueueItem;
	
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// 申请一个新项
	if ((pqueueItem = UnionNewQueueItem(dataLen,data)) == NULL)
	{
		UnionUserErrLog("in UnionPutDataIntoQueue:: UnionNewQueueItem!\n");
		return(UnionGetUserDefinedErrorCode());
	}
	// 新项的后一个项指向队列的第一个项
	pqueueItem->next = pqueue->firstItem;
	
	// 将原来的第一个项的前一项，指向新项
	if (pqueue->firstItem != NULL)
		pqueue->firstItem->previous = pqueueItem;
	// 将新项置为第一个项
	pqueue->firstItem = pqueueItem;
	
	// 队列是空
	if (pqueue->lastItem == NULL)	
		pqueue->lastItem = pqueueItem;
	
	// 队列中项数目增1
	pqueue->itemNum += 1;
	
	return(0);
}

