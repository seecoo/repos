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
	long 	statusOfPos;	// ֵ�̶�Ϊ��gAvailableMsgBufPos
	char	indexOfPos[4];	// ֵΪλ�õ�������ֱ�Ӵ������λ�õ�ֵ������int���ڴ濽�����û���
} TUnionFreePosOfMsgBuf;

typedef struct
{
	long 	typeOfMsg;	// ֵΪ�û�������Ϣ����
	char	indexOfPos[4];	// ֵΪλ�õ�������ֱ�Ӵ������λ�õ�ֵ������int���ڴ濽�����û���
} TUnionOccupiedPosOfMsgBuf;

typedef struct
{
	long	maxSizeOfMsg;		// һ����Ϣ����󳤶�, ����Ϊ64�ı���
	int	maxNumOfMsg;		// ��Ϣ���������
} TUnionMsgBufGroup;

typedef struct
{
	// ���²������û��ڳ�ʼ����Ϣ����ʱ��
	//long	maxSizeOfMsg;		// һ����Ϣ����󳤶�, ����Ϊ64�ı���
	//int	maxNumOfMsg;		// ��Ϣ���������
	int	groupNumOfMsg;		// ��Ϣ������
	
	TUnionMsgBufGroup	msgGrp[defMaxMsgBufGroupNum];

	int	maxStayTime;		// һ����Ϣ�ڻ�����ڵ����ʱ�䣬���
	int	isNoWaitWhenReading;	// �Ƿ���÷�������ʽ����Ϣ
	int	intervalWhenReading;	// ������ȴ�ʱ�䣬�ڷ�������ʽ�£�΢���
	
	// ���²�����ʼ��ʱ�ɳ�ʼ��������
	int	userID;			// �û�Ϊ��Ϣ��������ID��
	int	queueIDOfFreePos;	// �������λ�õĶ��У��ö����е���ϢΪ��TUnionFreePosOfMsgBuf
					// ��Ӧ���û�IDΪuserID
	int	queueIDOfOccupiedPos;
					// ������ռλ�õĶ��У��ö����е���ϢΪ��TUnionOccupiedPosOfMsgBuf
					// ��Ӧ���û�IDΪuserID + 1
} TUnionMsgBufHDL;
typedef TUnionMsgBufHDL		*PUnionMsgBufHDL;

typedef struct
{
	long	type;		// ��Ϣ�ı�ʶ��,������Ϊ<=0����
	int	provider;	// ��Ϣ�ṩ�ߵĽ��̺�
	time_t	time;		// �÷���Ϣ��ʱ��
	int	len;		// ��Ϣ�ĳ���
	long	msgIndex;	// ��Ϣ�������ţ��ɱ�ģ�����
	int	dealer;		// ��Ϣ�Ĵ�����
	short	locked;		// ��
} TUnionMessageHeader;
typedef TUnionMessageHeader	*PUnionMessageHeader;

// ������ID,��0��ʼ����
void UnionSetGroupIDOfMsgBuf(int groupID);

// ���������õ�������Ϣ��ƫ����
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

// ��index��Ӧ����Ϣλ����Ϊ����
int UnionFreeMsgBufPos(int index);

// ��ÿ��õ���Ϣλ�ã�����ֵΪ����λ��
int UnionGetAvailableMsgBufPos();

// ��ÿ���λ�õ�����
int UnionGetNumOfFreeMsgBufPos();

// �����ռλ�õ�����
int UnionGetNumOfOccupiedMsgBufPos();

// ��index��Ӧ����Ϣλ����Ϊ����Ϣ
// typeOfMsgΪ��Ϣ����
int UnionOccupyMsgBufPos(int index,long typeOfMsg);

// ��õ�һ������Ϣ�Ļ���λ�ã�����ֵΪ��λ��
// *typeOfMsg������ռ�ݸ�λ�õ���Ϣ������
int UnionGetOccupiedMsgBufPos(long *typeOfMsg);

// ��õ�һ����ָ����Ϣ�Ļ���λ�ã�����ֵΪ��λ��
// typeOfMsg��ָ������Ϣ����
int UnionGetOccupiedMsgBufPosOfMsgType(long typeOfMsg);

// MsgΪҪ�������Ϣ
// LenOfMsg����Ҫ�������Ϣ�ĳ���
// typeOfMsg������Ϣ����
int UnionBufferMessage(unsigned char * Msg,int LenOfMsg,long typeOfMsg);

// MsgΪҪ�������Ϣ
// LenOfMsg����Ҫ�������Ϣ�ĳ���
// typeOfMsg������Ϣ����
// poriHeader��ԭʼ����Ϣͷ
int UnionBufferMessageWithOriginHeader(unsigned char * Msg,int LenOfMsg,long typeOfMsg,PUnionMessageHeader poriHeader);

// MsgΪҪ�������Ϣ
// LenOfMsg����Ҫ�������Ϣ�ĳ���
// typeOfMsg������Ϣ����
// pnewHeader���µ���Ϣͷ���������
int UnionBufferMessageWithNewHeader(unsigned char * Msg,int LenOfMsg,long typeOfMsg,PUnionMessageHeader pnewHeader);

// ��ȡ��һ����Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// typeOfMsg���ǵ�һ����Ϣ������
// ����ֵΪ��Ϣ�ĳ���
int UnionReadFirstMsg(unsigned char * Msg,int LenOfMsgBuf,long *typeOfMsg);

// ��ȡ��һ����Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// typeOfMsg����ָ����Ϣ������
// ����ֵΪ��Ϣ�ĳ���
int UnionReadFirstMsgOfTypeUntilSuccess(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg);

// ��õ�һ����ָ����Ϣ�Ļ���λ�ã�����ֵΪ��λ��
// typeOfMsg��ָ������Ϣ����
int UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(long typeOfMsg);

// ɾ��ָ������Ӧ��Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// typeOfMsg����ָ����Ϣ������
// ����ֵΪ��Ϣ�ĳ���
int UnionDeleteSpecifiedMsg(long typeOfMsg);

// ������λ����Ϊ����
int UnionResetAllMsgIndexAvailable();

// ��ȡ��һ����Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// typeOfMsg����ָ����Ϣ������
// waitTime������Ϣ���Եȴ���������ʱ��ֵ��������ֵ����Ϣ��ֱ�Ӷ���
// ����ֵΪ��Ϣ�ĳ���
int UnionReadFirstMsgOfTypeUntilSuccess_FilterOutdateMsg(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg,long waitTime);

// ��ʼ������λ����Ϊ����
int UnionInitAllMsgIndex();

int UnionFreeRubbishMsg();

// ��ȡ��һ����Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// typeOfMsg����ָ����Ϣ������
// poutputHeader����Ϣ���ṩ����Ϣ
// ����ֵΪ��Ϣ�ĳ���
int UnionReadFirstMsgOfTypeUntilSuccessWithHeader(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg,PUnionMessageHeader poutputHeader);

long UnionGetTotalNumOfMsgBufMDL();

int UnionModifyMsgMaxStayTimeInBuf(long time);

int UnionPrintInavailabeMsgBufPosToSpecFile(char *fileName);

int UnionPrintMsgBufStatusToSpecFile(char *fileName);

PUnionMessageHeader UnionGetMessageHeaderOfIndexInMsgGrp(int index);

int UnionGetMaxMsgNumOfMsgBufMDL();

// 2007/04/12 ����
// ����ָ�����Ϣ������ʧ
int UnionSetSpecMsgBufIndexLosted(int index);

// 2007/04/12 ����
// �޸�����
int UnionRepairSpecMsgBufIndex(int index);

// 2007/4/12,����
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

// 2013/10/11, ����
PUnionMsgBufHDL UnionGetCurrentMsgBufHDLGrp();
// 2013/10/11, ����
unsigned char  *UnionGetCurrentMsgBuf();

int UnionPrintInavailabeMsgBufPosToFile(FILE *fp);

// 2007/04/12 �޸�
int UnionResetMsgIndexOfSpecTypeAvailable(long type,int index);

int UnionGetMaxSizeOfMsg();

#endif
