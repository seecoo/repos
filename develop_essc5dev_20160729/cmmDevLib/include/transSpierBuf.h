//	Author:		Wolfgang Wang
//	Date:		2003/09/26
//	Version:	4.0

#ifndef _UnionTransSpierBuf4_x_
#define _UnionTransSpierBuf4_x_

#include "time.h"

#define conTransSpierMsgFlag		123400

#define conMDLNameOfTransSpierBuf	"UnionTransSpierBufMDL"

#define conSizeOfEachTransSpierMsg	(sizeof(TUnionTransSpierMsgHeader) + sizeof(unsigned char) * pgunionTransSpierBufHDL->maxSizeOfMsg)

// add by liwj 20150410 �� .c �ļ��еĶ����ƶ���ͷ�ļ���
typedef struct
{
	int	used;
	char	ipAddr[15+1];
	int	port;
	int	sckHDL;
	time_t	lastConnFailTime;	// �ϴ�ͨѶʧ��ʱ��
} TUnionConn;
typedef TUnionConn	*PUnionConn;
// add end by liwj

typedef struct
{
	int			len;	// �����Ϣ�ĳ���
	int			resID;	// �����Ϣ������Դ
#ifdef _transSpier_2_x_
	char			applicationName[40+1];	// ��������
	int			pid;	// ���̺�
	char			clientIPAddr[15+1];	// �ͻ���IP��ַ
	int			clientPort;		// �˿ں�
	long			ssn;
	char			dateTime[14+1];		// ʱ��
#endif
} TUnionTransSpierMsgHeader;
typedef TUnionTransSpierMsgHeader	*PUnionTransSpierMsgHeader;

typedef struct
{
	int			maxSizeOfMsg;
	int			maxNumOfMsg;
	unsigned char		*pmsgBuf;		// ���СΪ��
} TUnionTransSpierBufHDL;
typedef TUnionTransSpierBufHDL	*PUnionTransSpierBufHDL;

int UnionPutTransSpierMsgHeaderIntoStr(PUnionTransSpierMsgHeader pheader,char *str,int sizeOfStr);

int UnionReconnectTransSpierBufMDLAnyway();

int UnionIsTransSpierBufMDLConnected();

int UnionGetFileNameOfTransSpierBufDef(char *fileName);

int UnionGetTotalNumOfTransSpierBufMDL();

int UnionInitTransSpierBufDef(PUnionTransSpierBufHDL pdef);

int UnionConnectTransSpierBufMDL();
	
int UnionDisconnectTransSpierBufMDL();

int UnionRemoveTransSpierBufMDL();

int UnionReloadTransSpierBufDef();

long UnionBufferTransSpierMsg(char *msg,int lenOfMsg,int resID);
	
int UnionReadFirstTransSpierMsgUntilSuccess(unsigned char *msg,int sizeOfMsg,int *resID);

int UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess(unsigned char *msg,int sizeOfMsg,int *resID,PUnionTransSpierMsgHeader poutputMsgHeader);

int UnionPrintAvailablTransSpierBufPosToFile(FILE *fp);

int UnionPrintInavailabeTransSpierBufPosToFile(FILE *fp);

int UnionPrintTransSpierBufStatusToFile(FILE *fp);

int UnionPrintTransSpierBufToFile(FILE *fp);

int UnionResetAllTransSpierMsgPos();

int UnionSendTransInfoToMonitor(int resID,char *data,int len);

// wangk add 2009-9-25
int UnionPrintTransSpierBufStatusInRecStrFormatToFile(FILE *fp);

int UnionPrintTransSpierBufStatusInRecStrFormatToSpecFile(char *fileName);
// wangk add end 2009-9-25

void UnionSetIntervalTimeOfLastConnFail(int seconds);

void UnionSetLongConnwithMonitor();

#endif

