// Wolfgang Wang
// 2010-12-20

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionMsgBuf6.x.h"
#include "unionErrCode.h"
#include "unionServiceBuffer.h"

#define errCodeServiceBufferTimeout			-11000
#define errCodeServiceBufferNoChanelForSpecService	-12000
#define errCodeServiceBufferRequestTooShort		-13000

// ������񻺳���
TUnionServiceBuffer	gunionServiceBuffer;
PUnionServiceBuffer	pgunionServiceBuffer = &gunionServiceBuffer;
int			gunionServiceBufferConnected = 0;			// �������Ƿ�������
int			gunionIsServiceBufferTimeout = 0;			// ��ʶ�Ƿ����˳�ʱ�¼�
time_t			gunionCurrentServiceStartTime = 0;		// ��ǰ����ʼʱ��
int			gunionServiceBufferTimeout = 3;			// �������ĳ�ʱʱ��
int			gunionCurrentTokenID = -1;			// ��ǰ�����Ʊ�ʶ
int			gunionServiceReceiverWakeuped = 0;		// ��ʶ�Ƿ񱻻���

void UnionServiceBufferDealTimeout();

/*
����
	����������������ʶ
�������
	��
�������
	��
����ֵ
	��
*/
void UnionSetReconnectServiceBuffer()
{
	if ((errno == -22) || (errno == -35))
		gunionServiceBufferConnected = 0;
	return;
}		

/*
����
	�����񻺳�д�뵽��־�ļ�
�������
	���񻺳�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogServiceBuffer(PUnionServiceBuffer pbuffer)
{
	int			index;
	PUnionServiceChanel	pchanel;
	
	if (pbuffer == NULL)
		return;
	for (index = 0; index < pbuffer->chanelNum; index++)
	{
		pchanel = &(pbuffer->chanelGrp[index]);
#ifdef _useSerivceChanelToken_
		UnionNullLog("serverID=%08ld|tokenQueueID=%08d,%08x|requestQueueID=%08d,%08x|responseQueueID=%08d,%08x|\n",
			pchanel->serviceID,
			pchanel->userTokenQueueID,
			pchanel->sysTokenQueueID,
			pchanel->userRequestQueueID,
			pchanel->sysRequestQueueID,
			pchanel->userResponseQueueID,
			pchanel->sysResponseQueueID);
#else
		UnionNullLog("serverID=%08ld|requestQueueID=%08d,%08x|responseQueueID=%08d,%08x|\n",
			pchanel->serviceID,
			pchanel->userRequestQueueID,
			pchanel->sysRequestQueueID,
			pchanel->userResponseQueueID,
			pchanel->sysResponseQueueID);
#endif
	}
	UnionNullLog("chanelNum=%d\n",pbuffer->chanelNum);
	return;
}

/*
����
	���ó�ʱ
�������
	userTimeout	�û�ָ���ĳ�ʱ
�������
	��
����ֵ
	��
*/
void UnionResetServiceBufferLeftTimeout(int userTimeout)
{
	gunionIsServiceBufferTimeout = 0;
	time(&gunionCurrentServiceStartTime);
	if (userTimeout > 0)
		gunionServiceBufferTimeout = userTimeout;
	return;
}

/*
����
	��ȡʣ��ĳ�ʱ
�������
	userTimeout	�û�ָ���ĳ�ʱ
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ��
*/
int UnionGetServiceBufferLeftTimeout(int userTimeout)
{
	time_t		now;
	int		timeout;
	
	gunionIsServiceBufferTimeout = 0;
	time(&now);
	if (gunionCurrentServiceStartTime <= 0)
		gunionCurrentServiceStartTime = now;
	if (userTimeout > 0)
		gunionServiceBufferTimeout = userTimeout;
	timeout = gunionServiceBufferTimeout - (now - gunionCurrentServiceStartTime);
	if (timeout <= 0)
	{
		UnionUserErrLog("in UnionGetServiceBufferLeftTimeout:: timeout = [%d]!\n",timeout);
		gunionIsServiceBufferTimeout = 1;
		return(UnionSetUserDefinedErrorCode(errCodeServiceBufferTimeout));
	}
	return(timeout);
}

/*
����
	���ó�ʱ
�������
	userTimeout	�û�ָ���ĳ�ʱ
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ��
*/
int UnionSetServiceBufferTimeout(int userTimeout)
{
	int		timeout;

	if ((timeout = UnionGetServiceBufferLeftTimeout(userTimeout)) < 0)
	{
		UnionUserErrLog("in UnionSetServiceBufferTimeout:: UnionGetServiceBufferLeftTimeout!\n");
		return(timeout);
	}	
	alarm(timeout);
	signal(SIGALRM,UnionServiceBufferDealTimeout);
	return(timeout);
}

/*
����
	�����ʱ����
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ��
*/
int UnionClearServiceBufferTimeout()
{
	alarm(0);
	gunionIsServiceBufferTimeout = 0;
	return(0);
}

/*
����
	���ӷ��񻺳���,���������,��������
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�,����ͨ������
	<0	����,������
*/
int UnionConnectServiceBuffer()
{
	int	ret;
	
	if (gunionServiceBufferConnected)
		return(pgunionServiceBuffer->chanelNum);
	return(UnionConnectServiceBufferAnyway(pgunionServiceBuffer));
}

/*
����
	���ӷ��񻺳���
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�,����ͨ������
	<0	����,������
*/
int UnionConnectServiceBufferAnyway()
{
	int	ret;
	
	gunionServiceBufferConnected = 0;
	if ((ret = UnionInitServiceBuffer(pgunionServiceBuffer)) < 0)
	{
		UnionUserErrLog("in UnionConnectServiceBufferAnyway:: UnionInitServiceBuffer!\n");
		return(ret);
	}
	gunionServiceBufferConnected = 1;
	UnionLogServiceBuffer(pgunionServiceBuffer);
	return(ret);
}

/*
����
	�Ͽ�����񻺳���������
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	����,������
*/
int UnionDisconnectServiceBuffer()
{
	gunionServiceBufferConnected = 0;
	return(0);
}

/*
����
	��ȡָ�������Ӧ��ͨ��
�������
	serviceID	�����ʶ
�������
	��
����ֵ
	�ɹ�	����ͨ��ָ��
	ʧ��	��ָ��
*/
PUnionServiceChanel UnionFindChanelOfSpecService(long serviceID)
{
	int			index;
	int			ret;
		
	if ((ret = UnionConnectServiceBuffer()) < 0)
	{
		UnionUserErrLog("in UnionGetTokenOfSpecService:: UnionConnectServiceBuffer!\n");
		return(NULL);
	}
	// ����ָ������ͨ��
	for (index = 0; index < pgunionServiceBuffer->chanelNum; index++)
	{
		if (serviceID == pgunionServiceBuffer->chanelGrp[index].serviceID)
			return(&(pgunionServiceBuffer->chanelGrp[index]));
	}
	UnionLog("in UnionFindChanelOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
	UnionSetUserDefinedErrorCode(errCodeServiceBufferNoChanelForSpecService);
	return(NULL);
}

#ifdef _useSerivceChanelToken_
/*
����
	�������ָ�����Ƶ�����
�������
	pchanel		����ͨ��
	tokenID		��������
�������
	��
����ֵ
	>=0		���������
	<0		ʧ��,���ش�����
*/
int UnionClearRequestToSpecToken(PUnionServiceChanel pchanel,int tokenID)
{
	int			num = 0;
	TUnionServicePack	pack;
	int			ret;
	
	if ((pchanel == NULL) || (tokenID <= 0))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
		
	pack.receiverID = tokenID;
	for (;;)
	{
		if (msgrcv(pchanel->sysRequestQueueID,&pack,sizeof(pack.text),tokenID,IPC_NOWAIT) < 0)
			return(num);
		num++;
	}	
}
#endif

/*
����
	���ָ����Ӧ��ʶ����Ӧ
�������
	pchanel		����ͨ��
	specID		ָ������Ӧ��ʶ
�������
	��
����ֵ
	>=0		���������
	<0		ʧ��,���ش�����
*/
int UnionClearResponseOfSpecID(PUnionServiceChanel pchanel,int specID)
{
	int			num = 0;
	TUnionServicePack	pack;
	int			ret;
	
	if ((pchanel == NULL) || (specID <= 0))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
		
	pack.receiverID = specID;
	for (;;)
	{
		if (msgrcv(pchanel->sysResponseQueueID,&pack,sizeof(pack.text),specID,IPC_NOWAIT) < 0)
			return(num);
		num++;
	}	
}


#ifdef _useSerivceChanelToken_
/*
����
	��ȡ�ṩָ�����������
�������
	serviceID	�����ʶ
�������
	pchanel		����ͨ��
����ֵ
	>=0	���Ʊ�ʶ
	<0	����,������
*/
int UnionGetTokenOfSpecService(long serviceID,PUnionServiceChanel pchanel)
{
	int			ret;
	TUnionServiceToken	token;
	int			tokenID;
	PUnionServiceChanel	ptmpChanel;
	
	// ����ָ������ͨ��
	if ((ptmpChanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionGetTokenOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}

	memset(&token,0,sizeof(token));
	token.serviceID = serviceID;
	// ��ȡ���ó�ʱ
	if ((ret = UnionSetServiceBufferTimeout(-1)) < 0)
	{
		UnionUserErrLog("in UnionGetTokenOfSpecService:: UnionSetServiceBufferTimeout!\n");
		return(ret);
	}
	if ((ret = msgrcv(ptmpChanel->sysTokenQueueID,&token,sizeof(token.tokenID),serviceID,0)) < 0)
	{
		UnionClearServiceBufferTimeout();
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionGetTokenOfSpecService:: msgrcv from [%d][%d]!\n",ptmpChanel->userTokenQueueID,ptmpChanel->sysTokenQueueID);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionClearServiceBufferTimeout();
	memcpy(&tokenID,token.tokenID,sizeof(tokenID));
	if (pchanel != NULL)
		memcpy(pchanel,ptmpChanel,sizeof(*pchanel));
	return(tokenID);
}
#endif

#ifdef _useSerivceChanelToken_
/*
����
	��ָ����������ƿ���
�������
	serviceID	�����ʶ
	tokenID		���ƺ�
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��,������
*/
int UnionSetTokenOfSpecServiceAvailable(long serviceID,int tokenID)
{
	return(UnionSetTokenOfSpecChanelAvailable(UnionFindChanelOfSpecService(serviceID),tokenID));
}
#endif

#ifdef _useSerivceChanelToken_
/*
����
	��ָ����������ƿ���
�������
	pchanel		����ͨ��
	tokenID		���ƺ�
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��,������
*/
int UnionSetTokenOfSpecChanelAvailable(PUnionServiceChanel pchanel,int tokenID)
{
	int			index;
	TUnionServiceToken	token;
	int			ret;
	
	if (pchanel == NULL)
	{
		UnionUserErrLog("in UnionSetTokenOfSpecChanelAvailable:: pchanel is NULL!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	// �����ƿ���
	token.serviceID = pchanel->serviceID;
	memcpy(token.tokenID,&tokenID,sizeof(tokenID));
	if ((ret = msgsnd(pchanel->sysTokenQueueID,&token,sizeof(int),IPC_NOWAIT)) < 0)
	{
		UnionSystemErrLog("in UnionSetTokenOfSpecChanelAvailable:: msgsnd! [%d][%d]!\n",pchanel->userTokenQueueID,pchanel->sysTokenQueueID);
		UnionSetReconnectServiceBuffer();
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	return(ret);
}
#endif

// ����һ��ָ�����������
/* ���������
	serviceID	�����ʶ
	msg		��Ϣ
	lenOfMsg	��Ϣ����
   ���������
	tokenID		���Ʊ�ʶ
   ����ֵ��
   	>=0����ȷ
   	��ֵ��������
*/
int UnionSendRequestOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg)
{
	TUnionServicePack	pack;
	int			ret;
	PUnionServiceChanel	pchanel;
	int			tokenID;
	
	gunionCurrentTokenID = -1;
	if ((lenOfMsg > sizeof(pack.text)) || (lenOfMsg < 0))
	{
		UnionUserErrLog("in UnionSendRequestOfSpecService:: lenOfMsg [%d] error!\n",lenOfMsg);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	// �ҵ�����ͨ��
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionSendRequestOfSpecService:: UnionFindChanelOfSpecService [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}
	// �Ա��Ľ��и�ֵ
	memcpy(pack.text,msg,lenOfMsg);
	pack.receiverID = serviceID;
loop:
#ifdef _useSerivceChanelToken_		// ʹ������	
	// �Ȼ�����ƺ�
	memset(&chanel,0,sizeof(chanel));
	if ((tokenID = UnionGetTokenOfSpecService(serviceID,&chanel)) < 0)
	{
		UnionUserErrLog("in UnionSendRequestOfSpecService:: UnionGetTokenOfSpecService!\n");
		return(tokenID);
	}
	// �������ʶ��Ϊָ������
	pack.receiverID = tokenID;
#endif
	// �������������
	if ((ret = msgsnd(pchanel->sysRequestQueueID,&pack,lenOfMsg,IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionSendRequestOfSpecService:: msgsnd [%d][%d]!\n",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
#ifdef _useSerivceChanelToken_		// ʹ������	
		UnionSetTokenOfSpecChanelAvailable(pchanel,tokenID);	// �������ƿ���
#endif
		return(ret);
	}
#ifdef _useSerivceChanelToken_		// ʹ������	
	// ֪ͨ���ƽ�������
	if ((ret = UnionNoticeToken(tokenID)) < 0)	// �����Ѳ�����
	{
		UnionSystemErrLog("in UnionSendRequestOfSpecService:: UnionNoticeToken [%d]!\n",tokenID);
		// ����ղŵ�������Ϣ
		UnionClearRequestToSpecToken(pchanel,tokenID);
		goto loop;	// ����ʹ����һ���������·���
	}
	gunionCurrentTokenID = tokenID;
#endif
	return(ret);
}

// ����һ��ָ���������Ӧ
/* ���������
	serviceID	�����ʶ
	msg		��Ϣ
	lenOfMsg	��Ϣ����
	responseID	��Ӧ��ʶ
   ���������
   	��
   ����ֵ��
   	>=0����ȷ
   	��ֵ��������
*/
int UnionSendResponseOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg,int responseID)
{
	TUnionServicePack	pack;
	int			ret;
	int			tokenID;
	PUnionServiceChanel	pchanel;

	if ((lenOfMsg > sizeof(pack.text)) || (lenOfMsg < 0))
	{
		UnionUserErrLog("in UnionSendResponseOfSpecService:: lenOfMsg [%d] error!\n",lenOfMsg);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	// �����Ľ��и�ֵ
	memcpy(pack.text,msg,lenOfMsg);
	pack.receiverID = responseID;
	// ���ҳ�����ͨ��
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionSendResponseOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}
loop:	
	// ����Ӧ�������
	if ((ret = msgsnd(pchanel->sysResponseQueueID,&pack,lenOfMsg,IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionSendResponseOfSpecService:: msgsnd [%d][%d]!\n",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
#ifdef _useSerivceChanelToken_		// ʹ������	
		UnionSetTokenOfSpecChanelAvailable(pchanel,gunionCurrentTokenID);	// �õ�ǰ���ƿ���
#endif
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
#ifdef _useSerivceChanelToken_		// ʹ������	
	// ֪ͨ�������
	if ((ret = UnionNoticeResponseReceiver(responseID)) < 0)
	{
		UnionSystemErrLog("in UnionSendResponseOfSpecService:: UnionNoticeResponseReceiver [%d]!\n",responseID);
		// �����Ӧ������Ϣ
		UnionClearResponseOfSpecID(pchanel,responseID);
		UnionSetTokenOfSpecChanelAvailable(pchanel,gunionCurrentTokenID);	// �������ƿ���
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	// �������ƿ���
	if ((ret = UnionSetTokenOfSpecChanelAvailable(pchanel,gunionCurrentTokenID)) < 0)
	{
		UnionUserErrLog("in UnionSendResponseOfSpecService:: UnionSetTokenOfSpecChanelAvailable [%ld][%d]!\n",serviceID,gunionCurrentTokenID);
		return(UnionSetUserDefinedErrorCode(ret));
	}
#endif
	return(ret);
}

// ��ȡָ�����������
/* ���������
	serviceID	�����ʶ
	sizeOfBuf	���ջ���Ĵ�С
   ���������
	msg		Ϊ���յ�����Ϣ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadRequestOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf)
{
	int			ret;
	PUnionServiceChanel	pchanel;
	TUnionServicePack	pack;
	int			len;
	
loop:
	gunionCurrentTokenID = -1;
	// �Ȳ���ͨ��
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionReadRequestOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}

	pack.receiverID = serviceID;
#ifdef _useProcIDAsSerivceChanelToken_		// ʹ�ý��̺���Ϊ���Ʊ�ʶ	
	// �ȴ����յ���Ϣ֪ͨ
	for (;;)
	{
		gunionServiceReceiverWakeuped = 0;
		sleep(60);
		if (gunionServiceReceiverWakeuped)
			break;
	}
	// ��ȡ��Ϣ
	pack.receiverID = getpid();
	if ((ret = msgrcv(pchanel->sysRequestQueueID,&pack,sizeof(pack.text),getpid(),IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadRequestOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
		// �����ƿ���
		UnionSetTokenOfSpecChanelAvailable(pchanel,getpid());
		goto loop;
	}
#else
	if ((ret = msgrcv(pchanel->sysRequestQueueID,&pack,sizeof(pack.text),pack.receiverID,0)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadRequestOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
		goto loop;
	}
#endif
	len = ret;
	// ��������
	if (len >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadRequestOfSpecService:: sizeOfBuf [%d] too small to receive [%d]!\n",sizeOfBuf,len);
#ifdef _useSerivceChanelToken_		// ʹ������	
		// �����ƿ���
		UnionSetTokenOfSpecChanelAvailable(pchanel,getpid());
#endif
		goto loop;
	}
	memcpy(msg,pack.text,len);
	return(len);
}

// ��ȡָ���������Ӧ
/* ���������
	serviceID	�����ʶ
	responseID	��Ӧ��ʶ
	timeout		�Ƕ���Ӧ�ĳ�ʱֵ
	sizeOfBuf	�ǽ��ջ���Ĵ�С
   ���������
	msg		Ϊ���յ�����Ϣ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadResponseOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf,long responseID,int timeout)
{
	int			ret;
	PUnionServiceChanel	pchanel;
	TUnionServicePack	pack;
	int			len;
	int			leftTimeout;
	
	// �Ȳ���ͨ��
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}
loop:
#ifdef _useProcIDAsSerivceChanelToken_		// ʹ�ý��̺���Ϊ���Ʊ�ʶ	
	// �ȴ����յ���Ϣ֪ͨ
	if ((leftTimeout = UnionGetServiceBufferLeftTimeout(timeout)) < 0)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: UnionGetServiceBufferLeftTimeout!\n");
		return(leftTimeout);
	}
	for (;;)
	{
		gunionServiceReceiverWakeuped = 0;
		sleep(leftTimeout);
		if (gunionServiceReceiverWakeuped)
			break;
		if ((leftTimeout = UnionGetServiceBufferLeftTimeout(-1)) < 0)
		{
			UnionUserErrLog("in UnionReadResponseOfSpecService:: UnionGetServiceBufferLeftTimeout!\n");
			return(leftTimeout);
		}
	}
	// ��ȡ��Ϣ
	pack.receiverID = getpid();	
	if ((ret = msgrcv(pchanel->sysResponseQueueID,&pack,sizeof(pack.text),getpid(),IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadResponseOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
#else
	pack.receiverID = responseID;
	// ���ó�ʱ
	if ((ret = UnionSetServiceBufferTimeout(timeout)) < 0)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: UnionSetServiceBufferTimeout!\n");
		return(ret);
	}
	if ((ret = msgrcv(pchanel->sysResponseQueueID,&pack,sizeof(pack.text),responseID,0)) < 0)
	{
		UnionClearServiceBufferTimeout();
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadResponseOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionClearServiceBufferTimeout();	// �����ʱ
#endif
	len = ret;
	// ��������
	if (len >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: sizeOfBuf [%d] too small to receive [%d]!\n",sizeOfBuf,len);
		// �����ƿ���
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	memcpy(msg,pack.text,len);
	return(len);
}

// ִ��һ�η��񽻻�
/* ���������
	serviceID	ָ���ķ���
	responseID	��Ӧ��ʶ
	reqStr		����
	lenOfReqStr	���󳤶�
	sizeOfResStr	�ǽ��ջ���Ĵ�С
	timeout		�Ƕ���Ӧ�ĳ�ʱֵ
   ���������
	resStr		Ϊ���յ�����Ϣ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionExcuteSpecService(long serviceID,long responseID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout)
{
	int			ret;
				
	UnionSetServiceBufferTimeout(timeout);
	
	// ���ύ����
	if ((ret = UnionSendRequestOfSpecService(serviceID,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExcuteSpecService:: UnionSendRequestOfSpecService serviceID=[%ld]\n",serviceID);
		return(ret);
	}
	// ����ȡ��Ӧ
	if ((ret = UnionReadResponseOfSpecService(serviceID,resStr,sizeOfResStr,responseID,-1)) < 0)
	{
		UnionUserErrLog("in UnionExcuteSpecService:: UnionReadResponseOfSpecService serviceID=[%ld] responseID=[%ld]\n",serviceID,responseID);
		return(ret);
	}
	return(ret);
}

/*
����
	�ó�ʱ��ʶ
�������
	��
�������
	��
����ֵ
	��
*/
void UnionServiceBufferDealTimeout()
{
	gunionIsServiceBufferTimeout = 1;
	return;
}

/*
����
	���ָ������ͨ���ĳ�ʱ
�������
	chanelID	ͨ����
�������
	��
����ֵ
	>=0	ָ������ͨ���ĳ�ʱ
	<0	�������
*/
int UnionGetTimeoutOfSpecServiceChanel(long chanelID)
{
	char	varName[128+1];
	int	timeout;
	
	sprintf(varName,"timeoutOfChanel%ld",chanelID);
	if ((timeout = UnionReadIntTypeRECVar(varName)) <= 0)
		timeout = 2;
	return(timeout);
}

/*
����
	���ָ������ͨ�����Ի���������Ϣ��
�������
	chanelID	ͨ����
�������
	��
����ֵ
	>=0	ָ������ͨ�����Ի���������Ϣ��
	<0	�������
*/
int UnionGetMaxNumMsgOfSpecServiceChanel(long chanelID)
{
	char	varName[128+1];
	int	maxNumMsg;
	
	sprintf(varName,"maxNumMsgOfChanel%ld",chanelID);
	if ((maxNumMsg = UnionReadIntTypeRECVar(varName)) <= 0)
		maxNumMsg = 100;
	return(maxNumMsg);
}

/*
����
	��ʾ�ṹ�ı���
�������
	fp	������ݵ��ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionDisplayTitleOfServiceChanel(FILE *fp)
{
	FILE	*outFp;

	outFp = UnionGetValidFp(fp);
	// �ڴ����ӿͻ���Դ��
	fprintf(outFp,"%10s %4s %17s %6s %6s %8s\n","ͨ����ʶ","����","���б�ʶ","��Ϣ��","�ֽ���","����ʱ��");

	return;
}

/*
����
	��ʾ�ṹ������
�������
	fp	������ݵ��ļ�ָ��
	pdef	�ṹָ��
�������
	��
����ֵ
		�ɹ�	1
		ʧ��	0
*/
int UnionDisplayContentOfServiceChanel(PUnionServiceChanel pdef,FILE *fp)
{
	FILE			*outFp;
	int			msgNum;
	int			msgSize;
	long			ctime;
	int			ret;

	outFp = UnionGetValidFp(fp);
	// �ڴ����ӿͻ���Դ��
	if ((ret = UnionGetStatusOfMsgQueue(pdef->sysRequestQueueID,&msgNum,&msgSize,&ctime)) < 0)
	{
		msgNum = -1;
		msgSize = -1;
		ctime = -1;
	}
	fprintf(outFp,"%10ld ���� %08x-%08d %6d %6d %8d\n",
		pdef->serviceID,
		pdef->userRequestQueueID,pdef->sysRequestQueueID,
		msgNum,msgSize,ctime);
	if ((ret = UnionGetStatusOfMsgQueue(pdef->sysResponseQueueID,&msgNum,&msgSize,&ctime)) < 0)
	{
		msgNum = -1;
		msgSize = -1;
		ctime = -1;
	}
	fprintf(outFp,"%10ld ��Ӧ %08x-%08d %6d %6d %8d\n",
		pdef->serviceID,
		pdef->userResponseQueueID,pdef->sysResponseQueueID,
		msgNum,msgSize,ctime);
	
	return(1);
}

/*
����
	��ʾ����ͨ����״̬
�������
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionPrintStatusOfServiceBufferToFp(FILE *fp)
{
	PUnionServiceBuffer	pbus;
	int			index;
	int			realNum = 0;
	int			ret;
	
	if ((ret = UnionConnectServiceBuffer()) < 0)
	{
		UnionUserErrLog("in UnionDisplayStatusOfServiceBuffer:: ���ú���[UnionConnectServiceBuffer]����!");
		return(ret);
	}
	pbus = pgunionServiceBuffer;
	UnionDisplayTitleOfServiceChanel(fp);
	for (index = 0; index < pbus->chanelNum; index++)
	{
		if ((ret = UnionDisplayContentOfServiceChanel(&(pbus->chanelGrp[index]),fp)) <= 0)
			continue;
		realNum++;
	}
	fprintf(fp,"����ͨ������=%d\n",realNum);
	return(realNum);
}

/*
����
	��ʾ����ͨ����״̬
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionPrintStatusOfServiceBufferToSpecFile(char *fileName)
{
	FILE			*fp;
	int			ret;
		
	fp = UnionOpenFile(fileName);
	
	ret = UnionPrintStatusOfServiceBufferToFp(fp);
	
	UnionCloseFp(fp);
	
	return(ret);
}

/*
��������
       ��ȡһ����Ϣ���е�״̬
�������
       sysID		���еı�ʶ
�������
       num		ͨ���е�ǰ�ȴ��������Ϣ��Ŀ
       size		ͨ���е�ǰ�ȴ��������Ϣ���ܳ���
       idleTime		����ʱ��
����ֵ
       >=0		�ɹ�
       <0		ʧ��
*/
int UnionGetStatusOfMsgQueue(int sysID,int *num,int *size,long *idleTime)
{
	struct msqid_ds status;
	long		selTime;
	time_t		now;
		
	if (msgctl(sysID,IPC_STAT,&status) != 0)
	{
		UnionSystemErrLog("in UnionGetStatusOfMsgQueue:: ����[msgctl]��ȡ����[%d]��״̬����!",sysID);
		return(errCodeUseOSErrCode);
	}

	*num = status.msg_qnum;
	*size = status.msg_cbytes;
	if (status.msg_stime > status.msg_rtime)
		selTime = status.msg_stime;
	else
		selTime = status.msg_rtime;
	if (selTime < status.msg_ctime)
		selTime = status.msg_ctime;
	time(&now);
	*idleTime = now - selTime;
	return(0);
}

/*
����
	ά������ͨ�����������
�������
	pchanel
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionMaintainRequestOfServiceChanel(PUnionServiceChanel pchanel)
{
	int			num,size;
	long			idleTime;
	int			ret;
	int			realNum = 0;
	int			timeout,maxMsgNum;
	
	// ������		
	if (pchanel == NULL)
		return(errCodeNullPointer);
	
	// ���������е�״̬
	if ((ret = UnionGetStatusOfMsgQueue(pchanel->sysRequestQueueID,&num,&size,&idleTime)) < 0)
	{
		UnionUserErrLog("in UnionMaintainRequestOfServiceChanel:: ���ú���[UnionGetStatusOfMsgQueue]��ö���[%d]��״̬����!",pchanel->sysRequestQueueID);
		return(ret);
	}
	
	// �жϿ���ʱ��
	timeout = UnionGetTimeoutOfSpecServiceChanel(pchanel->serviceID);
	maxMsgNum = UnionGetMaxNumMsgOfSpecServiceChanel(pchanel->serviceID);
	if (idleTime > timeout)	// ����ʱ�䳬����Ԥ��ʱ��
	{
		if (num > 0)	// ����������Ϣ����ʱ����Ϣȫ��������Ϣ��Ҫ����
		{	
			if ((ret = UnionClearAllTimeoutRequest(pchanel->sysRequestQueueID,timeout,num)) < 0)
			{
				UnionUserErrLog("in UnionMaintainRequestOfServiceChanel:: ���ú���[UnionClearAllTimeoutRequest]�Ӷ���[%d-%d]�����������ʧ��!",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
				return(ret);
			}
			if (ret > 0)
				UnionNullLog("���Ӷ���[%d-%d]�������[%d]����ʱ[%d]����!",pchanel->userRequestQueueID,pchanel->sysRequestQueueID,ret,timeout);
			realNum += ret;
			//return(ret);
			num -= ret;	// �������м���Ҫ��������
		}
		else
			return(0);	// ��ʱ������û����Ϣ������Ҫ����
	}
	
	// ����ʱ���Ԥ�ڵĳ�ʱʱ��С
	if (num <= maxMsgNum)
		return(realNum);	// ��ʱ�����л������Ϣ��Ŀ�ȿ��Եȴ����������С������Ҫ����
		
	// �����л������Ϣ��Ŀ�ȿ��Եȴ�����Ϣ������,����������Ϣ
	if ((ret = UnionClearRequestOfAtLeastNum(pchanel->sysRequestQueueID,num-maxMsgNum,timeout)) < 0)
	{
		UnionUserErrLog("in UnionMaintainRequestOfServiceChanel:: ���ú���[UnionClearRequestOfAtLeastNum]�Ӷ���[%d-%d]�����������ʧ��!",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
		return(ret);
	}
	if (ret > 0)
		UnionNullLog("���Ӷ���[%d-%d]�������[%d]����������!��ʱʱ��=[%d]",pchanel->userRequestQueueID,pchanel->sysRequestQueueID,ret,timeout);
	return(ret+realNum);
}

/*
����
	ά������ͨ������Ӧ����
�������
	pchanel
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionMaintainResponseOfServiceChanel(PUnionServiceChanel pchanel)
{
	int			num,size;
	long			idleTime;
	int			ret;
	int			maxNum;
	int			realNum = 0;
	int			timeout,maxMsgNum;
	
	// ������		
	if (pchanel == NULL)
	{
		UnionUserErrLog("in UnionMaintainResponseOfServiceChanel","pchanel�����ǿ�ָ��!");
		return(errCodeNullPointer);
	}
		
	// �����Ӧ���е�״̬
	if ((ret = UnionGetStatusOfMsgQueue(pchanel->sysResponseQueueID,&num,&size,&idleTime)) < 0)
	{
		UnionUserErrLog("in UnionMaintainResponseOfServiceChanel:: ���ú���[UnionGetStatusOfMsgQueue]��ö���[%d]��״̬����!",pchanel->sysResponseQueueID);
		return(ret);
	}
	
	// �жϿ���ʱ��
	timeout = UnionGetTimeoutOfSpecServiceChanel(pchanel->serviceID);
	maxMsgNum = UnionGetMaxNumMsgOfSpecServiceChanel(pchanel->serviceID);
	if (idleTime > timeout)	// ����ʱ�䳬����Ԥ��ʱ��
	{
		if (num > 0)	// ����������Ϣ����ʱ����Ϣȫ��������Ϣ��Ҫ����
		{
			if ((ret = UnionClearAllTimeoutResponse(pchanel->sysResponseQueueID,timeout,num)) < 0)
			{
				UnionUserErrLog("in UnionMaintainResponseOfServiceChanel:: ���ú���[UnionClearAllTimeoutResponse]�Ӷ���[%d-%d]���������Ӧʧ��!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
				return(ret);
			}
			if (ret > 0)
				UnionNullLog("���Ӷ���[%d-%d]�������[%d]��������Ӧ!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID,ret);
			realNum += ret;
			num -= ret;	// ����Ӧ��Ϣ�м���Ҫ��������
		}
		else
			return(0);	// ��ʱ������û����Ϣ������Ҫ����
	}
	
	// ����ʱ���Ԥ�ڵĳ�ʱʱ��С
	maxNum = maxMsgNum / 10;	// ͬʱ�������Ӧֻ����ռ�����10%
	if (maxNum <= 0)
		maxNum = 5;
	if (num <= maxNum)
		return(realNum);	// ��ʱ�����л������Ϣ��Ŀ�ȿ��Եȴ����������С������Ҫ����
		
	// �����л������Ϣ��Ŀ�ȿ��Եȴ�����Ϣ������,����������Ϣ
	if ((ret = UnionClearResponseOfAtLeastNum(pchanel->sysResponseQueueID,num-maxNum,timeout)) < 0)
	{
		UnionUserErrLog("in UnionMaintainResponseOfServiceChanel:: ���ú���[UnionClearResponseOfAtLeastNum]�Ӷ���[%d-%d]���������Ӧʧ��!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
		return(ret);
	}
	if (ret > 0)
		UnionNullLog("���Ӷ���[%d-%d]�������[%d]��������Ӧ!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID,ret);
	return(ret+realNum);
}

/*
����
	ά������ͨ��
�������
	pchanel
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionMaintainServiceChanel(PUnionServiceChanel pchanel)
{
	int			ret;
	int			num = 0;
	
	if ((ret = UnionMaintainRequestOfServiceChanel(pchanel)) < 0)
	{
		UnionUserErrLog("in UnionDisplayStatusOfServiceBuffer:: ���ú���[UnionMaintainRequestOfServiceChanel]����!");
		return(ret);
	}
	num += ret;
	if ((ret = UnionMaintainResponseOfServiceChanel(pchanel)) < 0)
	{
		UnionUserErrLog("in UnionDisplayStatusOfServiceBuffer:: ���ú���[UnionMaintainResponseOfServiceChanel]����!");
		return(ret);
	}
	return(num+ret);
}

/*
����
	ά����������
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionMaintainServiceBuffer()
{
	PUnionServiceBuffer		pbus;
	int			index;
	int			realNum = 0;
	int			ret;
	int			msgNum = 0;
		
	if ((ret = UnionConnectServiceBuffer()) < 0)
	{
		UnionUserErrLog("in UnionMaintainServiceBuffer:: ���ú���[UnionConnectServiceBuffer]����!");
		return(ret);
	}
	pbus = pgunionServiceBuffer;
	for (index = 0; index < pbus->chanelNum; index++)
	{
		if ((ret = UnionMaintainServiceChanel(&(pbus->chanelGrp[index]))) <= 0)
			continue;
		realNum++;
		msgNum += ret;
	}
	if (realNum > 0)
		UnionNullLog("�޸��ķ���ͨ������=%d\n",realNum);
	return(msgNum);
}

/*
����
	��������е����г�ʱ������Ϣ
�������
	queueID		���к�
	timeout		��ʱ
	maxNum		�������
	isRequest	1:�ж�����0:�ж���Ӧ
�������
	��
����ֵ
	>=0	�ɹ�,������Ϣ����
	<0	ʧ�ܣ�������
*/
int UnionClearAllTimeoutServiceInfo(int queueID,int timeout,int maxNum,int isRequest)
{
	int				num = 0;
	TUnionServicePack		pack;
	int				ret;
	long				serviceID;
	int				len;
	int				isTimeout;
	int				retryTimes;
	
	for (retryTimes = 0; retryTimes < maxNum; retryTimes++)
	{
		// ��ȡһ����Ϣ
		if ((len = msgrcv(queueID,&pack,sizeof(pack.text),0,IPC_NOWAIT)) < 0)
			return(num);
		serviceID = pack.receiverID;
		if (UnionIsDirtyServiceInfo(pack.text,len))	// ��һ��������
		{
			pack.text[len] = 0;
			UnionNullLog("�����һ��������[%d][%s]",len,pack.text);
			num++;
			continue;
		}
		// �ж��Ƿ��ǳ�ʱ
		if (isRequest)
			isTimeout = UnionIsTimeoutServiceRequest(pack.text,len,timeout);
		else
			isTimeout = UnionIsTimeoutServiceResponse(pack.text,len,timeout);
		if (isTimeout)
		{
			// ��ʱ
			if (isRequest)
				UnionLogServiceInfo("��������³�ʱ����::",pack.text,len);
			else
				UnionLogServiceInfo("��������³�ʱ��Ӧ::",pack.text,len);
			num++;
			continue;
		}
		// ���ǳ�ʱ����
		// ���������·Żض���
		if ((ret = msgsnd(queueID,&pack,len,IPC_NOWAIT)) < 0)
		{
			UnionSystemErrLog("UnionClearAllTimeoutServiceInfo","����[msgsnd]����!���б�ʶ=[%d]!",queueID);
			return(errCodeUseOSErrCode);
		}
	}
	return(num);
}

/*
����
	��������е�������������
�������
	queueID	���к�
	timeout	��ʱ
	maxNum		�������
�������
	��
����ֵ
	>=0	�ɹ�,������Ϣ����
	<0	ʧ�ܣ�������
*/
int UnionClearAllTimeoutRequest(int queueID,int timeout,int maxNum)
{
	return(UnionClearAllTimeoutServiceInfo(queueID,timeout,maxNum,1));
}

/*
����
	��������е�����������Ӧ
�������
	queueID	���к�
	timeout	��ʱ
	maxNum		�������
�������
	��
����ֵ
	>=0	�ɹ�,������Ϣ����
	<0	ʧ�ܣ�������
*/
int UnionClearAllTimeoutResponse(int queueID,int timeout,int maxNum)
{
	return(UnionClearAllTimeoutServiceInfo(queueID,timeout,maxNum,0));
}

/*
����
	�Ӷ��������ָ����������Ϣ
�������
	queueID		���к�
	atLeastNum	Ҫ�������Ϣ����
	timeout		��ʱ
	isRequest	1:�ж�����0:�ж���Ӧ
�������
	��
����ֵ
	>=0	�ɹ�,������Ϣ����
	<0	ʧ�ܣ�������
*/
int UnionClearServiceInfoOfAtLeastNum(int queueID,int atLeastNum,int timeout,int isRequest)
{
	int				num = 0;
	TUnionServicePack	pack;
	int				ret;
	long				serviceID;
	int				len;
	
	// �������ʱ��Ϣ
	//if ((num = UnionClearAllTimeoutServiceInfo(queueID,timeout,atLeastNum,isRequest)) < 0)
	//	UnionUserErrLog("in UnionClearServiceInfoOfAtLeastNum:: ���ú���[UnionClearAllTimeoutServiceInfo]����!"));
	
	if (num >= atLeastNum)	// ����ĳ�ʱ��Ϣ����Ҫ�������Ŀ��
		return(num);
	
	// ��û������㹻����Ŀ
	for (;;)
	{
		// ��ȡһ����Ϣ
		if ((len = msgrcv(queueID,&pack,sizeof(pack.text),0,IPC_NOWAIT)) < 0)
			return(num);
		if (isRequest)
			UnionLogServiceInfo("��������³�������::",pack.text,len);
		else
			UnionLogServiceInfo("��������³�����Ӧ::",pack.text,len);
		num++;
		if (num >= atLeastNum)	// ����ĳ�ʱ��Ϣ����Ҫ�������Ŀ��
			return(num);
	}
}

/*
����
	��������������ָ����������Ϣ
�������
	queueID		���к�
	atLeastNum	Ҫ�������Ϣ����
	timeout		��ʱ
�������
	��
����ֵ
	>=0	�ɹ�,������Ϣ����
	<0	ʧ�ܣ�������
*/
int UnionClearRequestOfAtLeastNum(int queueID,int atLeastNum,int timeout)
{
	return(UnionClearServiceInfoOfAtLeastNum(queueID,atLeastNum,timeout,1));
}

/*
����
	����Ӧ���������ָ����������Ϣ
�������
	queueID		���к�
	atLeastNum	Ҫ�������Ϣ����
	timeout		��ʱ
�������
	��
����ֵ
	>=0	�ɹ�,������Ϣ����
	<0	ʧ�ܣ�������
*/
int UnionClearResponseOfAtLeastNum(int queueID,int atLeastNum,int timeout)
{
	return(UnionClearServiceInfoOfAtLeastNum(queueID,atLeastNum,timeout,0));
}

/*
����
	�ж��Ƿ�������������Ϣ
�������
	buf		���������ַ���
	len		�������󳤶�
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsDirtyServiceInfo(char *buf,int len)
{
	if (len < sizeof(TUnionMessageHeader))
		return(1);	// ������
	else
		return(0);
}
	
/*
����
	�ж��Ƿ��ǳ�ʱ�ķ�������
�������
	buf		���������ַ���
	len		�������󳤶�
	timeout		��ʱ
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsTimeoutServiceRequest(char *buf,int len,int timeout)
{
	time_t			nowSeconds;
	int			ret;
	long			waitTime;
	PUnionMessageHeader	pheader;
	
	if (UnionIsDirtyServiceInfo(buf,len))
		return(0);

	pheader = (PUnionMessageHeader)buf;
	time(&nowSeconds);
	waitTime = nowSeconds - pheader->time;
	UnionNullLog("waitTime = [%ld] timeout = [%ld] now = [%ld] provider = [%ld]!",waitTime,timeout,nowSeconds,pheader->time);
	if (waitTime > timeout)
		return(1);
	else
		return(0);
}

/*
����
	�ж��Ƿ��ǳ�ʱ�ķ�����Ӧ
�������
	buf		������Ӧ�ַ���
	len		������Ӧ����
	timeout		��ʱ
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsTimeoutServiceResponse(char *buf,int len,int timeout)
{
	return(UnionIsTimeoutServiceRequest(buf,len,timeout));
}

/*
����
	��һ��������Ϣд����־��
�������
	title		����
	buf		������Ӧ�ַ���
	len		������Ӧ����
�������
	��
����ֵ
	��
*/
void UnionLogServiceInfo(char *title,char *buf,int len)
{
	PUnionMessageHeader	pmsgHeader;
	
	if ((len < 0) || (buf == NULL))
		return;
	if (UnionIsDirtyServiceInfo(buf,len))
		return;
	if (title != NULL)
		UnionNullLog(title);
	pmsgHeader = (PUnionMessageHeader)buf;
	UnionNullLog("time=%ld,provider=%d,dealer=%d,msgIndex=%ld,type=%ld\n",
		pmsgHeader->time,pmsgHeader->provider,pmsgHeader->dealer,pmsgHeader->msgIndex,pmsgHeader->type);
	buf[len] = 0;
	UnionNullLog("data=[%04d][%s]",len-sizeof(*pmsgHeader),buf+sizeof(*pmsgHeader));
	return;
}
