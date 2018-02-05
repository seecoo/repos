//	Author:	Wolfgang Wang
//	Date:	2001/10/22

// 2002/8/17，Wolfgang Wang
//	由2.x版升级为3.x版
//	3.x版与2.x版的差异在于，对于消息不再区分请求与响应，而只有一组消息函数。

// 2003/05/29,Wolfgang Wang
/*
	对gMaxLargestSizeOfMsg和gMaxSizeOfMsgBuf进行了重定义
*/

#ifndef _UnionMsgBuf3_x
#define _UnionMsgBuf3_x

#ifdef _UnionUseMaxMsgBufModel_	// 新增加，2003/05/29
#define gMaxSizeOfMsgBuf	256000	//以K为单位计
#define gMaxLargestSizeOfMsg	1280000	// 以字节计
#else				// 此段为原有定义
#define gMaxSizeOfMsgBuf	10000
#define gMaxLargestSizeOfMsg	1024
#endif

#define	gAvailableMsgBufPos	100
#define gOccupiedMsgBufPos	101


typedef struct
{
	long 	StatusOfPos;	// 值固定为：gAvailableMsgBufPos
	char	IndexOfPos[4];	// 值为位置的索引，直接存放索引位置的值，即将int型内存拷贝至该缓冲
} TUnionFreePosOfMsgBuf;

typedef struct
{
	long 	TypeOfMsg;	// 值为用户赋的消息类型
	char	IndexOfPos[4];	// 值为位置的索引，直接存放索引位置的值，即将int型内存拷贝至该缓冲
} TUnionOccupiedPosOfMsgBuf;

typedef struct
{
	// 以下参数由用户在初始化消息缓冲时置
	int	SizeOfBuf;		// 消息缓冲区的大小，以KiloBytes计
	int	LargestSizeOfMsg;	// 一条消息的最大长度, 必须为64的倍数
	int	UserID;			// 用户为消息缓冲分配的ID号
	
	// 以下参数初始化时由初始化函数置
	int	QueueIDOfFreePos;	// 缓冲空闲位置的队列，该队列中的消息为：TUnionFreePosOfMsgBuf
					// 对应的用户ID为UserID
	int	QueueIDOfOccupiedPos;
					// 缓冲已占位置的队列，该队列中的消息为：TUnionOccupiedPosOfMsgBuf
					// 对应的用户ID为UserID + 1
	int	ShareMemoryID;		// 存放消息缓冲定义和消息的共享内存
					// 对应的用户ID为UserID
	
	int	NumOfMsg;
} TUnionMsgBufHDL;
typedef TUnionMsgBufHDL		*PUnionMsgBufHDL;


PUnionMsgBufHDL UnionConnectMsgBufMDL(int UserID,int SizeOfMsgBuf,int LargestSizeOfMsg);

int UnionDisconnectMsgBufMDL(PUnionMsgBufHDL);

int UnionRemoveMsgBufMDL(int UserID,int SizeOfMsgBuf);

// 将Index对应的消息位置置为可用
int UnionFreeMsgBufPos(PUnionMsgBufHDL,int Index);

// 获得可用的消息位置，返回值为所获位置
int UnionGetAvailableMsgBufPos(PUnionMsgBufHDL);

// 将Index对应的消息位置置为有消息
// TypeOfMsg为消息类型
int UnionOccupyMsgBufPos(PUnionMsgBufHDL,int Index,long TypeOfMsg);

// 获得第一个有消息的缓冲位置，返回值为该位置
// *TypeOfMsg，返回占据该位置的消息的类型
int UnionGetOccupiedMsgBufPos(PUnionMsgBufHDL,long *TypeOfMsg);

// 获得第一个有指定消息的缓冲位置，返回值为该位置
// TypeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgType(PUnionMsgBufHDL,long TypeOfMsg);

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// TypeOfMsg，是消息类型
int UnionBufferMsg(PUnionMsgBufHDL,unsigned char * Msg,int LenOfMsg,long TypeOfMsg);

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是第一条消息的类型
// 返回值为消息的长度
int UnionReadFirstMsg(PUnionMsgBufHDL,unsigned char * Msg,int LenOfMsgBuf,long *TypeOfMsg);

// 读取第一条类型指定的消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionReadSpecifiedMsg(PUnionMsgBufHDL,unsigned char * Msg,int LenOfMsgBuf,long TypeOfMsg);

// 获得可用位置的数量
int UnionGetNumOfFreeMsgBufPos(PUnionMsgBufHDL);
// 获得已占位置的数量
int UnionGetNumOfOccupiedMsgBufPos(PUnionMsgBufHDL);

// 获得第一个有指定消息的缓冲位置，返回值为该位置,以非等待方式
// TypeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(PUnionMsgBufHDL,long TypeOfMsg);
// 删除指定的响应消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为删除的消息的数目
int UnionDeleteSpecifiedMsg(PUnionMsgBufHDL,long TypeOfMsg);

int UnionPrintStatusOfMsgBufMDL(PUnionMsgBufHDL pMsgBufHDL);

#endif

