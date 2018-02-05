// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionQueueDef_
#define _unionQueueDef_

typedef struct TUnionQueueItem
{
	unsigned char		*data;		// 数据
	int			dataLen;	// 数据长度
	struct TUnionQueueItem	*previous;	// 前一个项
	struct TUnionQueueItem	*next;		// 后一个项
} TUnionQueueItem;
typedef TUnionQueueItem		*PUnionQueueItem;

typedef struct
{
	int			itemNum;	// 项数目
	PUnionQueueItem		firstItem;	// 第一个项
	PUnionQueueItem		lastItem;	// 最后一个项
	PUnionQueueItem		currentItem;	// 当前项
} TUnionQueue;
typedef TUnionQueue		*PUnionQueue;

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
int UnionReadNextDataFromQueue(PUnionQueue pqueue,unsigned char *data,int sizeOfData);

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
int UnionOpenQueueForRead(PUnionQueue pqueue);

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
int UnionPrintQueueItemDataAsStringToFp(PUnionQueueItem pqueueItem,FILE *fp);

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
int UnionPrintQueueDataAsStringToSpecFile(PUnionQueue pqueue,char *fileName);

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
int UnionPrintQueueDataAsStringToFp(PUnionQueue pqueue,FILE *fp);

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
PUnionQueueItem UnionNewQueueItem(int dataLen,unsigned char *data);

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
int UnionFreeQueueItem(PUnionQueueItem pqueueItem);

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
PUnionQueue UnionNewQueue();

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
int UnionFreeQueue(PUnionQueue pqueue);

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
int UnionAppendDataToQueue(PUnionQueue pqueue,int dataLen,unsigned char *data);

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
int UnionPutDataIntoQueue(PUnionQueue pqueue,int dataLen,unsigned char *data);

#endif
