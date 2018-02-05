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
��������
        �Ӷ����ж���һ������
�������
       pqueue		����ָ�� 
	sizeOfData	���ݻ����С
�������
       ��
����ֵ
       >=0	�ɹ����������ݳ���
       <0	ʧ�ܣ�������
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
��������
        Ϊ����һ������
�������
       pqueue		����ָ�� 
�������
       ��
����ֵ
       >=0	�ɹ�
       <0	ʧ�ܣ�������
*/
int UnionOpenQueueForRead(PUnionQueue pqueue)
{
	if (pqueue == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	pqueue->currentItem = pqueue->firstItem;
	return(0);
}

/*
��������
        ����һ��������
�������
       dataLen 
       data 
�������
       ��
����ֵ
       �ɹ���������ָ�룻ʧ�ܣ���ָ��
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

	// ��ֵ����������	
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
��������
        �ͷ�һ��������
�������
       pqueueItem 
�������
       ��
����ֵ
       >=0��������ĳ��ȣ�<0�������������
*/
int UnionFreeQueueItem(PUnionQueueItem pqueueItem)
{
	if (pqueueItem == NULL)
		return(errCodeNullPointer);
	// �ͷű�������֮�����	
	UnionFreeQueueItem(pqueueItem->next);
	// �ͷŶ���������
	if (pqueueItem->data != NULL)
		free(pqueueItem->data);
	// �ͷű�������
	free(pqueueItem);
	return(0);
}

/*
��������
        ����һ������
�������
       ��
�������
       ��
����ֵ
       �ɹ�������ָ�룻ʧ�ܣ���ָ��
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
��������
        �ͷ�һ������
�������
       pqueue 
�������
       ��
����ֵ
       >=0,�����е���������Ŀ��<0,�������
*/
int UnionFreeQueue(PUnionQueue pqueue)
{
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// �ͷŵ�һ��������
	UnionFreeQueueItem(pqueue->firstItem);
	// �ͷŶ���
	free(pqueue);
	return(0);
}

/*
��������
        ���ַ�����ʽ��ӡһ������������
�������
       pqueueItem 	������ָ��
       fp		�ļ�ָ��
�������
       ��
����ֵ
       >=0��������ĳ��ȣ�<0�������������
*/
int UnionPrintQueueItemDataAsStringToFp(PUnionQueueItem pqueueItem,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pqueueItem == NULL)
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ��ӡ��������
	fprintf(outFp,"%s\n",pqueueItem->data);
	// ��ӡ��������֮�����	
	return(UnionPrintQueueItemDataAsStringToFp(pqueueItem->next,outFp));
}

/*
��������
        ���ַ�����ʽ��ӡһ�������е�����
�������
       pqueue 	����ָ��
       fp	�ļ�ָ��
�������
       ��
����ֵ
       >=0,�ɹ�
       <0,�������
*/
int UnionPrintQueueDataAsStringToFp(PUnionQueue pqueue,FILE *fp)
{
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// ��ӡ��һ��������
	return(UnionPrintQueueItemDataAsStringToFp(pqueue->firstItem,fp));
}

/*
��������
        ���ַ�����ʽ��ӡһ�������е����ݵ�ָ���ļ�
�������
       pqueue 	����ָ��
       fileName	�ļ�����
�������
       ��
����ֵ
       >=0,�ɹ�
       <0,�������
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
��������
        �����ݷ��뵽����β��
�������
       pqueue 
       dataLen 
       data 
�������
       ��
����ֵ
       >=0,�����е���������Ŀ��<0,�������
*/
int UnionAppendDataToQueue(PUnionQueue pqueue,int dataLen,unsigned char *data)
{
	PUnionQueueItem pqueueItem;
	
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// ����һ������
	if ((pqueueItem = UnionNewQueueItem(dataLen,data)) == NULL)
	{
		UnionUserErrLog("in UnionAppendDataToQueue:: UnionNewQueueItem!\n");
		return(UnionGetUserDefinedErrorCode());
	}
	// �����ǰһ����ָ����е����һ����
	pqueueItem->previous = pqueue->lastItem;
	
	// ��ԭ�������һ�������һ���ָ������
	if (pqueue->lastItem != NULL)
		pqueue->lastItem->next = pqueueItem;
	// ��������Ϊ���һ����
	pqueue->lastItem = pqueueItem;
	
	// �����ǿ�
	if (pqueue->firstItem == NULL)	
		pqueue->firstItem = pqueueItem;
	
	// ����������Ŀ��1
	pqueue->itemNum += 1;
	
	return(0);
}

/*
��������
        �����ݷ��뵽����ͷ��
�������
       pqueue 
       dataLen 
       data 
�������
       ��
����ֵ
       >=0,�����е���������Ŀ��<0,�������
*/
int UnionPutDataIntoQueue(PUnionQueue pqueue,int dataLen,unsigned char *data)
{
	PUnionQueueItem pqueueItem;
	
	if (pqueue == NULL)
		return(errCodeNullPointer);
	
	// ����һ������
	if ((pqueueItem = UnionNewQueueItem(dataLen,data)) == NULL)
	{
		UnionUserErrLog("in UnionPutDataIntoQueue:: UnionNewQueueItem!\n");
		return(UnionGetUserDefinedErrorCode());
	}
	// ����ĺ�һ����ָ����еĵ�һ����
	pqueueItem->next = pqueue->firstItem;
	
	// ��ԭ���ĵ�һ�����ǰһ�ָ������
	if (pqueue->firstItem != NULL)
		pqueue->firstItem->previous = pqueueItem;
	// ��������Ϊ��һ����
	pqueue->firstItem = pqueueItem;
	
	// �����ǿ�
	if (pqueue->lastItem == NULL)	
		pqueue->lastItem = pqueueItem;
	
	// ����������Ŀ��1
	pqueue->itemNum += 1;
	
	return(0);
}

