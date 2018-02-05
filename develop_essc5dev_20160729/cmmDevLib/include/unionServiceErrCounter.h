//	Author:		Wolfgang Wang
//	Date:		2009/9/30
//	Version:	1.0

#ifndef _unionServiceErrCounter_
#define _unionServiceErrCounter_

#include "unionErrCode.h"
#include "time.h"

typedef struct
{
	char	idOfApp[2+1];		// 应用代码
	char	serviceCode[3+1];	// 服务代码
	int	errCode;		// 错误代码
	long	times;			// 错误代码出现的次数
} TUnionServiceErrCounter;
typedef TUnionServiceErrCounter		*PUnionServiceErrCounter;

#define conMaxNumOfServiceErrCounter	256
typedef struct
{
	time_t			startTime;
	char			startTimeStr[14+1];
	time_t			nowTime;
	int			counterNum;
	TUnionServiceErrCounter	errGrp[conMaxNumOfServiceErrCounter];
	long			successNum;
	long			failNum;
	//double			failPercent;
} TUnionServiceErrCounterGrp;
typedef TUnionServiceErrCounterGrp	*PUnionServiceErrCounterGrp;

int UnionConnectServiceErrCounter();

int UnionResetServiceErrCounter();

int UnionIncreaseServiceErrCode(char *idOfApp,char *serviceCode,int errCode);

int UnionSendAllSecuSvrServiceCountToMonitor(int resID);

// Add By HuangBaoxin, 2010-12-02
typedef struct
{
	long	totalNum;	//交易总数
	long	failNum;	//失败次数
	float	failRate;	//失败率
	time_t	startTime;	//开始时间
	char	startTimeStr[14+1];	//开始时间
	time_t	finishTime;	//结束时间
} TUnionSecuSvrStatusCounter;
typedef TUnionSecuSvrStatusCounter	*PUnionSecuSvrStatusCounter;
int UnionIncreaseSecuSvrStatusCounterByErrCode(char *idOfApp,char *serviceCode,int errCode);

#endif
