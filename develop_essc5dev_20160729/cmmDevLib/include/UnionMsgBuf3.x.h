//	Author:	Wolfgang Wang
//	Date:	2001/10/22

// 2002/8/17��Wolfgang Wang
//	��2.x������Ϊ3.x��
//	3.x����2.x��Ĳ������ڣ�������Ϣ����������������Ӧ����ֻ��һ����Ϣ������

// 2003/05/29,Wolfgang Wang
/*
	��gMaxLargestSizeOfMsg��gMaxSizeOfMsgBuf�������ض���
*/

#ifndef _UnionMsgBuf3_x
#define _UnionMsgBuf3_x

#ifdef _UnionUseMaxMsgBufModel_	// �����ӣ�2003/05/29
#define gMaxSizeOfMsgBuf	256000	//��KΪ��λ��
#define gMaxLargestSizeOfMsg	1280000	// ���ֽڼ�
#else				// �˶�Ϊԭ�ж���
#define gMaxSizeOfMsgBuf	10000
#define gMaxLargestSizeOfMsg	1024
#endif

#define	gAvailableMsgBufPos	100
#define gOccupiedMsgBufPos	101


typedef struct
{
	long 	StatusOfPos;	// ֵ�̶�Ϊ��gAvailableMsgBufPos
	char	IndexOfPos[4];	// ֵΪλ�õ�������ֱ�Ӵ������λ�õ�ֵ������int���ڴ濽�����û���
} TUnionFreePosOfMsgBuf;

typedef struct
{
	long 	TypeOfMsg;	// ֵΪ�û�������Ϣ����
	char	IndexOfPos[4];	// ֵΪλ�õ�������ֱ�Ӵ������λ�õ�ֵ������int���ڴ濽�����û���
} TUnionOccupiedPosOfMsgBuf;

typedef struct
{
	// ���²������û��ڳ�ʼ����Ϣ����ʱ��
	int	SizeOfBuf;		// ��Ϣ�������Ĵ�С����KiloBytes��
	int	LargestSizeOfMsg;	// һ����Ϣ����󳤶�, ����Ϊ64�ı���
	int	UserID;			// �û�Ϊ��Ϣ��������ID��
	
	// ���²�����ʼ��ʱ�ɳ�ʼ��������
	int	QueueIDOfFreePos;	// �������λ�õĶ��У��ö����е���ϢΪ��TUnionFreePosOfMsgBuf
					// ��Ӧ���û�IDΪUserID
	int	QueueIDOfOccupiedPos;
					// ������ռλ�õĶ��У��ö����е���ϢΪ��TUnionOccupiedPosOfMsgBuf
					// ��Ӧ���û�IDΪUserID + 1
	int	ShareMemoryID;		// �����Ϣ���嶨�����Ϣ�Ĺ����ڴ�
					// ��Ӧ���û�IDΪUserID
	
	int	NumOfMsg;
} TUnionMsgBufHDL;
typedef TUnionMsgBufHDL		*PUnionMsgBufHDL;


PUnionMsgBufHDL UnionConnectMsgBufMDL(int UserID,int SizeOfMsgBuf,int LargestSizeOfMsg);

int UnionDisconnectMsgBufMDL(PUnionMsgBufHDL);

int UnionRemoveMsgBufMDL(int UserID,int SizeOfMsgBuf);

// ��Index��Ӧ����Ϣλ����Ϊ����
int UnionFreeMsgBufPos(PUnionMsgBufHDL,int Index);

// ��ÿ��õ���Ϣλ�ã�����ֵΪ����λ��
int UnionGetAvailableMsgBufPos(PUnionMsgBufHDL);

// ��Index��Ӧ����Ϣλ����Ϊ����Ϣ
// TypeOfMsgΪ��Ϣ����
int UnionOccupyMsgBufPos(PUnionMsgBufHDL,int Index,long TypeOfMsg);

// ��õ�һ������Ϣ�Ļ���λ�ã�����ֵΪ��λ��
// *TypeOfMsg������ռ�ݸ�λ�õ���Ϣ������
int UnionGetOccupiedMsgBufPos(PUnionMsgBufHDL,long *TypeOfMsg);

// ��õ�һ����ָ����Ϣ�Ļ���λ�ã�����ֵΪ��λ��
// TypeOfMsg��ָ������Ϣ����
int UnionGetOccupiedMsgBufPosOfMsgType(PUnionMsgBufHDL,long TypeOfMsg);

// MsgΪҪ�������Ϣ
// LenOfMsg����Ҫ�������Ϣ�ĳ���
// TypeOfMsg������Ϣ����
int UnionBufferMsg(PUnionMsgBufHDL,unsigned char * Msg,int LenOfMsg,long TypeOfMsg);

// ��ȡ��һ����Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// TypeOfMsg���ǵ�һ����Ϣ������
// ����ֵΪ��Ϣ�ĳ���
int UnionReadFirstMsg(PUnionMsgBufHDL,unsigned char * Msg,int LenOfMsgBuf,long *TypeOfMsg);

// ��ȡ��һ������ָ������Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// TypeOfMsg����ָ����Ϣ������
// ����ֵΪ��Ϣ�ĳ���
int UnionReadSpecifiedMsg(PUnionMsgBufHDL,unsigned char * Msg,int LenOfMsgBuf,long TypeOfMsg);

// ��ÿ���λ�õ�����
int UnionGetNumOfFreeMsgBufPos(PUnionMsgBufHDL);
// �����ռλ�õ�����
int UnionGetNumOfOccupiedMsgBufPos(PUnionMsgBufHDL);

// ��õ�һ����ָ����Ϣ�Ļ���λ�ã�����ֵΪ��λ��,�Էǵȴ���ʽ
// TypeOfMsg��ָ������Ϣ����
int UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(PUnionMsgBufHDL,long TypeOfMsg);
// ɾ��ָ������Ӧ��Ϣ
// LenOfMsgBuf����Msg����Ĵ�С
// TypeOfMsg����ָ����Ϣ������
// ����ֵΪɾ������Ϣ����Ŀ
int UnionDeleteSpecifiedMsg(PUnionMsgBufHDL,long TypeOfMsg);

int UnionPrintStatusOfMsgBufMDL(PUnionMsgBufHDL pMsgBufHDL);

#endif

