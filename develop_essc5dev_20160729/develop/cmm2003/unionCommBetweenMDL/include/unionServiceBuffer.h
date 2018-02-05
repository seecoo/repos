// Wolfgang Wang
// 2005/12/29

#ifndef	_unionServiceBuffer_
#define _unionServiceBuffer_

#include "unionMsgBuf6.x.h"

// 2010-12-20,���ӣ�������
// ����һ�������������
typedef struct
{
	long	serviceID;		// �����ʶ
	char	tokenID[8];		// �������
} TUnionServiceToken;
typedef TUnionServiceToken		*PUnionServiceToken;

// ����һ��������Ϣ
typedef struct
{
	long	receiverID;		// �����߱�ʶ
	char	text[8192+1];		// ��Ϣ����
} TUnionServicePack;
typedef TUnionServicePack		*PUnionServicePack;

// ����һ������ͨ��
typedef struct
{
	long	serviceID;		// �����ʶ��
#ifdef _useSerivceChanelToken_
	int	userTokenQueueID;	// �û�ָ�������ƽ��̶��б�ʶ
	int	sysTokenQueueID;	// ���õ����ƶ��б�ʶ
#endif
	int	userRequestQueueID;	// �û�ָ�������󻺳����
	int	sysRequestQueueID;	// ���󻺳���е�OS��ʶ��
	int	userResponseQueueID;	// �û�ָ������Ӧ�������
	int	sysResponseQueueID;	// ��Ӧ������е�OS��ʶ��
} TUnionServiceChanel;
typedef TUnionServiceChanel		*PUnionServiceChanel;

// ���������
#define conMaxNumOfServiceChanel	8
typedef struct
{
	int				chanelNum;				// ͨ������
	TUnionServiceChanel		chanelGrp[conMaxNumOfServiceChanel];	// ͨ����
} TUnionServiceBuffer;
typedef TUnionServiceBuffer		*PUnionServiceBuffer;

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
void UnionResetServiceBufferLeftTimeout(int userTimeout);

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
void UnionSetReconnectServiceBuffer();

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
void UnionLogServiceBuffer(PUnionServiceBuffer pbuffer);

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
int UnionGetServiceBufferLeftTimeout(int userTimeout);

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
int UnionSetServiceBufferTimeout(int userTimeout);

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
int UnionClearServiceBufferTimeout();

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
int UnionConnectServiceBuffer();

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
int UnionConnectServiceBufferAnyway();

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
int UnionDisconnectServiceBuffer();

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
PUnionServiceChanel UnionFindChanelOfSpecService(long serviceID);

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
int UnionClearRequestToSpecToken(PUnionServiceChanel pchanel,int tokenID);
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
int UnionClearResponseOfSpecID(PUnionServiceChanel pchanel,int specID);

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
int UnionGetTokenOfSpecService(long serviceID,PUnionServiceChanel pchanel);
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
int UnionSetTokenOfSpecServiceAvailable(long serviceID,int tokenID);
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
int UnionSetTokenOfSpecChanelAvailable(PUnionServiceChanel pchanel,int tokenID);
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
int UnionSendRequestOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg);

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
int UnionSendResponseOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg,int responseID);

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
int UnionReadRequestOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf);

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
int UnionReadResponseOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf,long responseID,int timeout);

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
int UnionExcuteSpecService(long serviceID,long responseID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

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
void UnionServiceBufferDealTimeout();

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
int UnionGetTimeoutOfSpecServiceChanel(long chanelID);

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
int UnionGetMaxNumMsgOfSpecServiceChanel(long chanelID);

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
void UnionDisplayTitleOfServiceChanel(FILE *fp);

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
int UnionDisplayContentOfServiceChanel(PUnionServiceChanel pdef,FILE *fp);

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
int UnionPrintStatusOfServiceBufferToFp(FILE *fp);

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
int UnionPrintStatusOfServiceBufferToSpecFile(char *fileName);

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
int UnionGetStatusOfMsgQueue(int sysID,int *num,int *size,long *idleTime);

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
int UnionMaintainRequestOfServiceChanel(PUnionServiceChanel pchanel);

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
int UnionMaintainResponseOfServiceChanel(PUnionServiceChanel pchanel);

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
int UnionMaintainServiceChanel(PUnionServiceChanel pchanel);

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
int UnionMaintainServiceBuffer();

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
int UnionClearAllTimeoutServiceInfo(int queueID,int timeout,int maxNum,int isRequest);

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
int UnionClearAllTimeoutRequest(int queueID,int timeout,int maxNum);

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
int UnionClearAllTimeoutResponse(int queueID,int timeout,int maxNum);

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
int UnionClearServiceInfoOfAtLeastNum(int queueID,int atLeastNum,int timeout,int isRequest);

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
int UnionClearRequestOfAtLeastNum(int queueID,int atLeastNum,int timeout);

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
int UnionClearResponseOfAtLeastNum(int queueID,int atLeastNum,int timeout);

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
int UnionIsDirtyServiceInfo(char *buf,int len);
	
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
int UnionIsTimeoutServiceRequest(char *buf,int len,int timeout);

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
int UnionIsTimeoutServiceResponse(char *buf,int len,int timeout);

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
void UnionLogServiceInfo(char *title,char *buf,int len);

#endif
