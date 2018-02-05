#ifndef _dataSyncIni_
#define _dataSyncIni_

#ifndef _unionFileTransfer_2_x_
#define _unionFileTransfer_2_x_
#endif

#define DB_SYNC_NUM	10

typedef enum
{
	conSQL,			// ��SQL
	conCache,		// ��Cache
	conSQLAndCache,		// SQL+Cache
	conSymmetricKeyMsg	// �Գ���Կ��Ϣ		//add by zhouxw 20150910
}TUnionDataSyncDataType;
typedef TUnionDataSyncDataType		*PUnionDataSyncDataType;

typedef enum
{
	conNotWaitResult,	// ���ȴ�ִ�н��
	conWaitResult		// �ȴ�ִ�н��
}TUnionDataSyncResultFlag;
typedef TUnionDataSyncResultFlag		*PUnionDataSyncResultFlag;

typedef struct
{
	int	localResourcesNeedSync;	// ������Դ�Ƿ�ͬ��������HSM
	int	highCachedNeedSync;	// ����ٻ����Ƿ�ͬ��������memcached
	char	sendIPAddr[64];		// ͬ��ƽ̨����IP��ַ
	int	sendPort;		// ͬ��ƽ̨���ն˿�
//	int	fileRecvPort;		// ͬ��ƽ̨�ļ����ն˿�
	int	socket_no;
}TUnionDataSyncAddr;
typedef TUnionDataSyncAddr	*PUnionDataSyncAddr;

typedef struct
{
	int	localPort;		// ����ƽ̨���ն˿�
	int	syncNum;
	TUnionDataSyncAddr	syncAddr[DB_SYNC_NUM];		
}TUnionDataSyncIni;
typedef TUnionDataSyncIni		*PUnionDataSyncIni;

void UnionSetDataSyncIsWaitResult();

void UnionSetDataSyncIsNotWaitResult();

int UnionDataSyncIsWaitResult();

/*
����	��ȡunionDBSync.ini��Ϣ
�������
	��
�������
	��
����ֵ
	=0		��ȷ
	<0		����
*/
int UnionReloadDataSyncIni();

/*
����	����msg����Ϣ����
�������
	msg		������Ϣ����
	lenOfMsg	��Ϣ����
	dataType	��������
	resultFlag	�����ʶ
�������
	��
����ֵ
	>=		�ɹ�
	<0		ʧ��
*/
int UnionDataSyncSendMsg(unsigned char *msg,int lenOfMsg,TUnionDataSyncDataType dataType,TUnionDataSyncResultFlag resultFlag);

/*
����	����Ϣ����ȡmsg
�������
	��
�������
	msg		��ȡ������Ϣ
	sizeOfMsg	��ȡ������Ϣ����
����ֵ
	>=		�ɹ�
	<0		ʧ��
*/
int UnionDataSyncRecvMsg(unsigned char *msg,int sizeOfMsg);

/*
����	������ʧ�ܼ�¼д�뵽�ļ�
�������
	psyncAddr	
	msg		ʧ�ܵļ�¼��Ϣ
	sizeOfMsg	��Ϣ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionDataSyncWriteFile(PUnionDataSyncAddr psyncAddr,unsigned char *msg,int lenOfMsg);

int UnionDataSyncSendToSocket(int closeSocket,unsigned char *msg,int lenOfMsg);

int UnionDataSyncCloseSocket();

int UnionGetDataSyncLocalPort();

PUnionDataSyncIni getDataSyncIniPtr();

#endif

