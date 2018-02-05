// Wolfgang Wang
// 2005-08-18

#ifndef _unionCommConf_
#define _unionCommConf_

#define conConfFileNameOfComm		"unionComm.Conf"
#define conMDLNameOfCommConfTBL		"unionCommConfTBLMDL"
	
#include "unionErrCode.h"

#ifdef _maxNumOfCommConf_2000_
#define conMaxNumOfConn		2000
#else
#ifdef _maxNumOfCommConf_1000_
#define conMaxNumOfConn		1000
#else
#ifdef _maxNumOfCommConf_500_
#define conMaxNumOfConn		500
#else
#ifdef _maxNumOfCommConf_200_
#define conMaxNumOfConn		200
#else
#define conMaxNumOfConn		40
#endif
#endif
#endif
#endif

typedef enum
{
	conCommClient,
	conCommServer
} TUnionCommProcType;

typedef enum
{
	conCommShortConn,
	conCommLongConn
} TUnionCommConnType;

typedef struct
{
	TUnionCommProcType	procType;		// 通讯配置的类型
	TUnionCommConnType	connType;		// 连接类型	
	char			ipAddr[15+1];		// 通讯另一方的IP地址
	int			port;			// 通讯使用的端口
	char			remark[10+1];		// 十个字符的说明

	long			connNum;		// 通讯连接的状态
	long			lastWorkingTime;	// 上次工作时间
	long			totalNum;		// 发送的包的总数量
} TUnionCommConf;
typedef TUnionCommConf	*PUnionCommConf;

typedef struct
{
	//int			num;
	TUnionCommConf		rec[conMaxNumOfConn];
} TUnionCommConfTBL;
typedef TUnionCommConfTBL	*PUnionCommConfTBL;

int UnionIsCommConfNormal(PUnionCommConf pcommConf);
int UnionSetCommConfOK(PUnionCommConf pcommConf);
int UnionSetCommConfAbnormal(PUnionCommConf pcommConf);
int UnionIncreaseCommConfWorkingTimes(PUnionCommConf pcommConf);

int UnionIsValidCommProcType(TUnionCommProcType commType);
int UnionIsValidCommConnType(TUnionCommConnType commType);
		
int UnionGetNameOfCommConfTBL(char *fileName);

int UnionIsCommConfTBLConnected();

int UnionConnectCommConfTBL();

int UnionDisconnectCommConfTBL();

int UnionRemoveCommConfTBL();

int UnionReloadCommConfTBL();

int UnionPrintCommConfToFile(PUnionCommConf punionCommConf,FILE *fp);
int UnionPrintCommConfTBLToFile(FILE *fp);
int UnionPrintCommConfOfIPAddrToFile(char *ipAddr,FILE *fp);
int UnionPrintCommConfOfRemarkToFile(char *remark,FILE *fp);
int UnionPrintCommConfOfPortToFile(int port,FILE *fp);
int UnionPrintAllServerCommConfToFile(FILE *fp);
int UnionPrintAllClientCommConfToFile(FILE *fp);

int UnionPrintCommConfTBLToSpecFile(char *fileName);
int UnionPrintCommConfOfIPAddrToSpecFile(char *ipAddr,char *fileName);
int UnionPrintCommConfOfRemarkToSpecFile(char *remark,char *fileName);
int UnionPrintCommConfOfPortToSpecFile(int port,char *fileName);
int UnionPrintAllServerCommConfToSpecFile(char *fileName);
int UnionPrintAllClientCommConfToSpecFile(char *fileName);

PUnionCommConf UnionFindServerCommConf(char *ipAddr,int port);
PUnionCommConf UnionFindClientCommConf(char *ipAddr,int port);
//UnionFindClientCommConfWithType add by hzh in 2012.2.22
PUnionCommConf UnionFindClientCommConfWithType(char *ipAddr,int port,TUnionCommConnType connType); 

PUnionCommConf UnionAddCommConf(char *ipAddr,int port,TUnionCommProcType procType,TUnionCommConnType connType,char *remark);
PUnionCommConf UnionAddServerCommConf(char *ipAddr,int port,TUnionCommConnType connType,char *remark);
PUnionCommConf UnionAddClientCommConf(char *ipAddr,int port,TUnionCommConnType connType,char *remark);

int UnionDeleteSpecifiedCommConf(char *ipAddr,int port,TUnionCommProcType procType);
int UnionDeleteIdleCommConf(long idleTime);
int UnionDeleteSpecifiedCommConfByRemark(char *remark);

// 2006/08/08
int UnionGetMaxNumOfCommConfTBL();

// 2006/08/08
PUnionCommConf UnionGetCurrentCommConfGrp();

int UnionResetAllCommConf();

int UnionResetSpecifiedCommConf(char *ipAddr,int port,TUnionCommProcType procType);

// wangk add 2009-9-25
int UnionPrintCommConfInRecStrFormatToFile(PUnionCommConf punionCommConf,FILE *fp);

int UnionPrintCommConfTBLInRecStrFormatToFile(FILE *fp);

int UnionPrintCommConfTBLInRecStrFormatToSpecFile(char *fileName);
// wangk add end 2009-9-25

#endif
