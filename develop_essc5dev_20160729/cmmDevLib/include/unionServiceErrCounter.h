//	Author:		Wolfgang Wang
//	Date:		2009/9/30
//	Version:	1.0

#ifndef _unionServiceErrCounter_
#define _unionServiceErrCounter_

#include "unionErrCode.h"
#include "time.h"

typedef struct
{
	char	idOfApp[2+1];		// Ӧ�ô���
	char	serviceCode[3+1];	// �������
	int	errCode;		// �������
	long	times;			// ���������ֵĴ���
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
	long	totalNum;	//��������
	long	failNum;	//ʧ�ܴ���
	float	failRate;	//ʧ����
	time_t	startTime;	//��ʼʱ��
	char	startTimeStr[14+1];	//��ʼʱ��
	time_t	finishTime;	//����ʱ��
} TUnionSecuSvrStatusCounter;
typedef TUnionSecuSvrStatusCounter	*PUnionSecuSvrStatusCounter;
int UnionIncreaseSecuSvrStatusCounterByErrCode(char *idOfApp,char *serviceCode,int errCode);

#endif
