//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/23

#ifndef _unionSckCommCli_
#define _unionSckCommCli_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define errSocketError		-12345

#define conMDLNameOfCommSvrDef	"UnionCommSvrDefMDL"

typedef enum
{
	conSckSvrAbnormal,
	conSckSvrNormal,
	conSckSvrMaintaining,
	conSckSvrColdBackup
} TUnionSckSvrStatus;
typedef struct
{
	char			ipAddr[15+1];		// IP��ַ
	int			port;			// �˿ں�
	int			isPrimaryServer;	// �Ƿ�����ͨѶ����ͨ��
	unsigned long		activeConnNum;		// ��������
	unsigned long		successTimes;		// �������ݵĳɹ�����
	unsigned long		failTimes;		// �������ݵ�ʧ�ܴ���
#ifdef _UnionSckCli_2_x_
	unsigned long		activeNum;		// ��ǰ���ڴ���Ĵ���
#endif
	TUnionSckSvrStatus	status;
} TUnionSckSvrDef;
typedef TUnionSckSvrDef	*PUnionSckSvrDef;

typedef enum
{
	conCommSvrHotBackupWorkingMode,
	conCommSvrBalanceWorkingMode,
	conCommSvrMaintainWorkingMode
} TUnionCommSvrWorkingMode;
typedef struct
{
	TUnionSckSvrDef			sckSvrDef[2];
	TUnionCommSvrWorkingMode	workingMode;
	int				timeout;
} TUnionCommSvrDef;
typedef TUnionCommSvrDef		*PUnionCommSvrDef;

typedef struct
{
	PUnionSckSvrDef		psckSvrDef;
	int			sckHDL;
} TUnionSckSvrConn;
typedef TUnionSckSvrConn	*PUnionSckSvrConn;

typedef struct
{
	PUnionCommSvrDef		pcommSvrDef;
	PUnionSckSvrConn		psckCommConn[2];
	TUnionCommSvrWorkingMode 	workingMode;
} TUnionCommSvrConn;
typedef TUnionCommSvrConn	*PUnionCommSvrConn;

int UnionSetBackupSckSvrAsPrimary(PUnionCommSvrDef pcommSvrDef);
PUnionSckSvrDef UnionSelectPrimarySckSvr(PUnionCommSvrDef pcommSvrDef);
PUnionSckSvrDef UnionSelectIdleSckSvr(PUnionCommSvrDef pcommSvrDef);

int UnionGetNameOfCommSvrDef(char *tmpBuf);
int UnionSetCommSvrAsHotBackupWorkingMode();
int UnionSetCommSvrAsBalanceWorkingMode();
int UnionSetCommSvrAsMaintainWorkingMode();
int UnionReloadCommSvrDef();
int UnionIsConnectedCommSvrDefMDL();
int UnionConnectCommSvrDefMDL();
int UnionDisconnectCommSvrDefMDL();
int UnionRemoveCommSvrDefMDL();
PUnionSckSvrConn UnionCreateSckSvrConn(PUnionSckSvrDef psckSvrDef);
int UnionCloseSckSvrConn(PUnionSckSvrConn psckSvrConn);
PUnionCommSvrConn UnionConnectCommSvr();
int UnionDisconnectCommSvr(PUnionCommSvrConn psckCommSvrConn);
int UnionCommSvrConnService(PUnionCommSvrConn psckCommSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr);
int UnionCommSvrConnHotBackupService(PUnionCommSvrConn psckCommSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr);
int UnionPrintCommSvrDef();

int UnionCommWithSckSvr(PUnionSckSvrConn psckSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr);

int UnionZerolizeSckSvrCommTimes(char *ipAddr);
int UnionSetSckSvrStatus(char *ipAddr,TUnionSckSvrStatus status);
int UnionReversePrimaryAndSecondarySckSvr();

int UnionSetCommSvrStatus(TUnionCommSvrWorkingMode workingMode);

int UnionGetTimeoutOfCommSvr();
int UnionSetTimeoutOfCommSvr(int timeout);

int UnionCheckCommSvr();

PUnionSckSvrDef UnionSelectSpecifiedSckSvr(char *ipAddr);
PUnionSckSvrDef UnionSelectSpecifiedSckSvrOfIndex(int index);

int UnionCommSvrConnBalanceService(PUnionCommSvrConn psckCommSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr);

#endif
