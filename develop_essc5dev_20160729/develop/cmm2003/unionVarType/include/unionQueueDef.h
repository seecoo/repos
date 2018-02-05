// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionQueueDef_
#define _unionQueueDef_

typedef struct TUnionQueueItem
{
	unsigned char		*data;		// ����
	int			dataLen;	// ���ݳ���
	struct TUnionQueueItem	*previous;	// ǰһ����
	struct TUnionQueueItem	*next;		// ��һ����
} TUnionQueueItem;
typedef TUnionQueueItem		*PUnionQueueItem;

typedef struct
{
	int			itemNum;	// ����Ŀ
	PUnionQueueItem		firstItem;	// ��һ����
	PUnionQueueItem		lastItem;	// ���һ����
	PUnionQueueItem		currentItem;	// ��ǰ��
} TUnionQueue;
typedef TUnionQueue		*PUnionQueue;

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
int UnionReadNextDataFromQueue(PUnionQueue pqueue,unsigned char *data,int sizeOfData);

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
int UnionOpenQueueForRead(PUnionQueue pqueue);

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
int UnionPrintQueueItemDataAsStringToFp(PUnionQueueItem pqueueItem,FILE *fp);

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
int UnionPrintQueueDataAsStringToSpecFile(PUnionQueue pqueue,char *fileName);

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
int UnionPrintQueueDataAsStringToFp(PUnionQueue pqueue,FILE *fp);

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
PUnionQueueItem UnionNewQueueItem(int dataLen,unsigned char *data);

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
int UnionFreeQueueItem(PUnionQueueItem pqueueItem);

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
PUnionQueue UnionNewQueue();

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
int UnionFreeQueue(PUnionQueue pqueue);

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
int UnionAppendDataToQueue(PUnionQueue pqueue,int dataLen,unsigned char *data);

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
int UnionPutDataIntoQueue(PUnionQueue pqueue,int dataLen,unsigned char *data);

#endif
