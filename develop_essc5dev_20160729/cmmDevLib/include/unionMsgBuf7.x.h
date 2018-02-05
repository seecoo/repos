//	Author:	Wolfgang Wang
//	Date:	2006/8/1


#ifndef _UnionMsgBuf7_x_        
#define _UnionMsgBuf7_x_ 

#include <time.h>

#define conMDLNameOfMsgBuf	"UnionMsgBufMDL"

#define	gAvailableMsgBufPos	100
//#define gOccupiedMsgBufPos	101

#define defMaxMsgBufGroupNum 	50

typedef struct
{
	long 	statusOfPos;	// 值固定为：gAvailableMsgBufPos
	char	indexOfPos[4];	// 值为位置的索引，直接存放索引位置的值，即将int型内存拷贝至该缓冲
} TUnionFreePosOfMsgBuf;

typedef struct
{
	long 	typeOfMsg;	// 值为用户赋的消息类型
	char	indexOfPos[4];	// 值为位置的索引，直接存放索引位置的值，即将int型内存拷贝至该缓冲
} TUnionOccupiedPosOfMsgBuf;

typedef struct
{
	long	maxSizeOfMsg;		// 一条消息的最大长度, 必须为64的倍数
	int	maxNumOfMsg;		// 消息的最大数量
} TUnionMsgBufGroup;

typedef struct
{
	// 以下参数由用户在初始化消息缓冲时置
	//long	maxSizeOfMsg;		// 一条消息的最大长度, 必须为64的倍数
	//int	maxNumOfMsg;		// 消息的最大数量
	int	groupNumOfMsg;		// 消息组数量
	
	TUnionMsgBufGroup	msgGrp[defMaxMsgBufGroupNum];

	int	maxStayTime;		// 一条消息在缓冲存在的最大时间，秒计
	int	isNoWaitWhenReading;	// 是否采用非阻塞方式读消息
	int	intervalWhenReading;	// 定义读等待时间，在非阻塞方式下，微秒计
	
	// 以下参数初始化时由初始化函数置
	int	userID;			// 用户为消息缓冲分配的ID号
	int	queueIDOfFreePos;	// 缓冲空闲位置的队列，该队列中的消息为：TUnionFreePosOfMsgBuf
					// 对应的用户ID为userID
	int	queueIDOfOccupiedPos;
					// 缓冲已占位置的队列，该队列中的消息为：TUnionOccupiedPosOfMsgBuf
					// 对应的用户ID为userID + 1
} TUnionMsgBufHDL;
typedef TUnionMsgBufHDL		*PUnionMsgBufHDL;

typedef struct
{
	long	type;		// 消息的标识号,不可以为<=0的数
	int	provider;	// 消息提供者的进程号
	time_t	time;		// 置放消息的时间
	int	len;		// 消息的长度
	long	msgIndex;	// 消息的索引号，由本模块分配
	int	dealer;		// 消息的处理者
	short	locked;		// 锁
} TUnionMessageHeader;
typedef TUnionMessageHeader	*PUnionMessageHeader;

// 设置组ID,从0开始计数
void UnionSetGroupIDOfMsgBuf(int groupID);

// 根据索引得到本条消息的偏移量
long UnionGetOffsetOfMsgBufByIndex(int index);

long UnionGetMaxStayTimeOfMsg();

int UnionGetMaxNumOfMsg();

int UnionGetAllMaxNumOfMsg();

int UnionIsMsgBufMDLConnected();

int UnionGetFileNameOfMsgBufHDL(char *fileName);

int UnionInitMsgBufHDL(PUnionMsgBufHDL pdef);

int UnionConnectMsgBufMDL();

int UnionReloadMsgBufDef();

int UnionPrintAvailablMsgBufPosToFile(FILE *fp);

int UnionPrintMsgBufToFile(FILE *fp);

int UnionDisconnectMsgBufMDL();

int UnionRemoveMsgBufMDL();

int UnionPrintMsgBufStatusToFile(FILE *fp);

// 将index对应的消息位置置为可用
int UnionFreeMsgBufPos(int index);

// 获得可用的消息位置，返回值为所获位置
int UnionGetAvailableMsgBufPos();

// 获得可用位置的数量
int UnionGetNumOfFreeMsgBufPos();

// 获得已占位置的数量
int UnionGetNumOfOccupiedMsgBufPos();

// 将index对应的消息位置置为有消息
// typeOfMsg为消息类型
int UnionOccupyMsgBufPos(int index,long typeOfMsg);

// 获得第一个有消息的缓冲位置，返回值为该位置
// *typeOfMsg，返回占据该位置的消息的类型
int UnionGetOccupiedMsgBufPos(long *typeOfMsg);

// 获得第一个有指定消息的缓冲位置，返回值为该位置
// typeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgType(long typeOfMsg);

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// typeOfMsg，是消息类型
int UnionBufferMessage(unsigned char * Msg,int LenOfMsg,long typeOfMsg);

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// typeOfMsg，是消息类型
// poriHeader，原始的消息头
int UnionBufferMessageWithOriginHeader(unsigned char * Msg,int LenOfMsg,long typeOfMsg,PUnionMessageHeader poriHeader);

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// typeOfMsg，是消息类型
// pnewHeader，新的消息头，输出参数
int UnionBufferMessageWithNewHeader(unsigned char * Msg,int LenOfMsg,long typeOfMsg,PUnionMessageHeader pnewHeader);

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是第一条消息的类型
// 返回值为消息的长度
int UnionReadFirstMsg(unsigned char * Msg,int LenOfMsgBuf,long *typeOfMsg);

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionReadFirstMsgOfTypeUntilSuccess(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg);

// 获得第一个有指定消息的缓冲位置，返回值为该位置
// typeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(long typeOfMsg);

// 删除指定的响应消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionDeleteSpecifiedMsg(long typeOfMsg);

// 将所有位置置为可用
int UnionResetAllMsgIndexAvailable();

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// waitTime，是消息可以等待处理的最大时间值，超过该值的消息将直接丢弃
// 返回值为消息的长度
int UnionReadFirstMsgOfTypeUntilSuccess_FilterOutdateMsg(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg,long waitTime);

// 初始化所有位置置为可用
int UnionInitAllMsgIndex();

int UnionFreeRubbishMsg();

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// poutputHeader是消息的提供者信息
// 返回值为消息的长度
int UnionReadFirstMsgOfTypeUntilSuccessWithHeader(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg,PUnionMessageHeader poutputHeader);

long UnionGetTotalNumOfMsgBufMDL();

int UnionModifyMsgMaxStayTimeInBuf(long time);

int UnionPrintInavailabeMsgBufPosToSpecFile(char *fileName);

int UnionPrintMsgBufStatusToSpecFile(char *fileName);

PUnionMessageHeader UnionGetMessageHeaderOfIndexInMsgGrp(int index);

int UnionGetMaxMsgNumOfMsgBufMDL();

// 2007/04/12 增加
// 设置指令的消息索引丢失
int UnionSetSpecMsgBufIndexLosted(int index);

// 2007/04/12 增加
// 修复索引
int UnionRepairSpecMsgBufIndex(int index);

// 2007/4/12,增加
int UnionRepairAllMsgBufIndex();

// wangk add 2009-9-24
int UnionPrintMsgBufStatusInRecStrFormatToSpecFile(char *fileName);

int UnionPrintMsgBufStatusInRecStrFormatToFile(FILE *fp);
// wangk add end 2009-9-24

// add by wangk 2009-12-09
int UnionPrintAvailabeMsgBufPosInRecStrToFile(FILE *fp);

int UnionPrintAvailabeMsgBufPosInRecStrFormatToSpecFile(char *fileName);

int UnionPrintInavailabeMsgBufPosInRecStrToFile(FILE *fp);

int UnionPrintInavailablMsgBufPosInRecStrFormatToSpecFile(char *fileName);
// add end wangk 2009-12-09

// 2013/10/11, 增加
PUnionMsgBufHDL UnionGetCurrentMsgBufHDLGrp();
// 2013/10/11, 增加
unsigned char  *UnionGetCurrentMsgBuf();

int UnionPrintInavailabeMsgBufPosToFile(FILE *fp);

// 2007/04/12 修改
int UnionResetMsgIndexOfSpecTypeAvailable(long type,int index);

int UnionGetMaxSizeOfMsg();

#endif
