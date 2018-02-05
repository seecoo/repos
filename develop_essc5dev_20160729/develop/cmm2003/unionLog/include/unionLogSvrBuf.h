// Author:	Wolfgang Wang
// Date:	2007/5/29

// 5.x����ʹ��

#ifndef _UnionLogSvrBuf_
#define _UnionLogSvrBuf_

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define conLogSvrInfoFlag	123456
#define conMDLNameOfLogSvrBuf	"UnionLogSvrBufMDL"


typedef struct
{
	int		len;		// �����Ϣ�ĳ���
	char		name[128+1];	// ���40���ַ���log�ļ�����������׺������·��
} TUnionLogSvrInfoHeader;
typedef TUnionLogSvrInfoHeader	*PUnionLogSvrInfoHeader;

typedef struct
{
	int			maxSizeOfMsg;
	int			maxNumOfMsg;
	unsigned char		*pmsgBuf;		// ���СΪ��
} TUnionLogSvrBufHDL;
typedef TUnionLogSvrBufHDL	*PUnionLogSvrBufHDL;

#define conSizeOfEachLogSvrInfo	(sizeof(TUnionLogSvrInfoHeader) + sizeof(unsigned char) * pgunionLogSvrBufHDL->maxSizeOfMsg)

int UnionReconnectLogSvrBufMDLAnyway();

int UnionIsLogSvrBufMDLConnected();

int UnionGetFileNameOfLogSvrBufDef(char *fileName);

int UnionGetTotalNumOfLogSvrBufMDL();

int UnionInitLogSvrBufDef(PUnionLogSvrBufHDL pdef);

int UnionConnectLogSvrBufMDL();
	
int UnionDisconnectLogSvrBufMDL();

int UnionRemoveLogSvrBufMDL();

int UnionReloadLogSvrBufDef();

int UnionBufferLogSvrInfo(char *msg,int lenOfMsg);

int UnionReadFirstLogSvrInfoUntilSuccess(unsigned char *msg,int sizeOfMsg,char *logFileName);

int UnionPrintAvailablLogSvrBufPosToFile(FILE *fp);

int UnionPrintInavailabeLogSvrBufPosToFile(FILE *fp);

int UnionPrintLogSvrBufStatusToFile(FILE *fp);

int UnionPrintLogSvrBufToFile(FILE *fp);

int UnionResetAllLogSvrInfoPos();

#endif
