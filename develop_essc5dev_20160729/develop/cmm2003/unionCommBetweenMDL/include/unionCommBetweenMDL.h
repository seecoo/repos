// Wolfgang Wang
// 2005/12/29

#ifndef	_commBetweenMDL_
#define _commBetweenMDL_

#include "unionMDLID.h"
#include "unionErrCode.h"
#include "unionMsgBuf6.x.h"

typedef struct
{
	TUnionModuleID	requesterMDLID;	// ��������ģ���ID
	TUnionModuleID	dealerMDLID;	// ���������ģ���ID
} TUnionHeaderOfMDLComm;
typedef TUnionHeaderOfMDLComm		*PUnionHeaderOfMDLComm;

int UnionConnectCommBetweenMDLs();

int UnionDisconnectCommBetweenMDLs();

// ��������ָ����ģ��
/* ���������
	recvMDLID������ģ���ID
	msg,lenOfMsg,�ֱ�Ϊ��Ϣ����Ϣ����
   ���������
	pmsgHeader���ǽ��շ��͸���Ϣʱ���ɵ���Ϣͷ�Ļ���
	ΪNULLʱ�����������Ϣͷ
   ����ֵ��
   	>=0����ȷ
   	��ֵ��������
*/
int UnionSendRequestToSpecifiedModuleWithNewMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader);

// ��������ָ����ģ��
/* ���������
	recvMDLID������ģ���ID
	msg,lenOfMsg,�ֱ�Ϊ��Ϣ����Ϣ����
   ���������
	��
   ����ֵ��
   	>=0����ȷ
   	��ֵ��������
*/
int UnionSendRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg);

// ����Ӧ����ָ����ģ��
/* ���������
	recvMDLID������ģ���ID
	msg,lenOfMsg,�ֱ�Ϊ��Ϣ����Ϣ����
	poriMsgHeader,�Ǹ���Ӧ��Ӧ��������Ϣͷ��ΪNULLʱ���޶�Ӧ��������Ϣͷ
   ���������
   	��
   ����ֵ��
   	>=0����ȷ
   	��ֵ��������
*/
int UnionSendResponseToApplyModuleWithOriMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader poriMsgHeader);

// ����Ӧ����ָ����ģ��
/* ���������
	recvMDLID������ģ���ID
	msg,lenOfMsg,�ֱ�Ϊ��Ϣ����Ϣ����
   ���������
   	��
   ����ֵ��
   	>=0����ȷ
   	��ֵ��������
*/
int UnionSendResponseToApplyModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg);

// ��ȡ����ָ��ģ�������
/* ���������
	recvMDLID������ģ���ID
	sizeOfBuf,�ǽ��ջ���Ĵ�С
   ���������
	msg,Ϊ���յ�����Ϣ
	prequesterMDLID,���ύ�����ģ��ID
	pmsgHeader,��������Ϣ����Ϣͷ�������ʼ��ΪNULL���򲻶�����Ϣͷ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadRequestToSpecifiedModuleWithMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID,PUnionMessageHeader pmsgHeader);

// ��ȡ����ָ��ģ�������
/* ���������
	recvMDLID������ģ���ID
	sizeOfBuf,�ǽ��ջ���Ĵ�С
   ���������
	msg,Ϊ���յ�����Ϣ
	prequesterMDLID,���ύ�����ģ��ID
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID);

// ��ȡ����ָ��ģ�������
/* ���������
	��
	sizeOfBuf,�ǽ��ջ���Ĵ�С
   ���������
	msg,Ϊ���յ�����Ϣ
	prequesterMDLID,���ύ�����ģ��ID
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadFirstRequestSendToThisModule(unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID);

// ��ȡ������ģ�����Ӧ
/* ���������
	timeout,�Ƕ���Ӧ�ĳ�ʱֵ
	sizeOfBuf,�ǽ��ջ���Ĵ�С
   ���������
	msg,Ϊ���յ�����Ϣ
	pmsgHeader,��������Ϣ����Ϣͷ�������ʼ��ΪNULL���򲻶�����Ϣͷ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadResponseToThisModuleWithMsgHeader(unsigned char *msg,int sizeOfBuf,int timeout,PUnionMessageHeader pmsgHeader);

// ��ȡ������ģ�����Ӧ
/* ���������
	timeout,�Ƕ���Ӧ�ĳ�ʱֵ
	sizeOfBuf,�ǽ��ջ���Ĵ�С
   ���������
	msg,Ϊ���յ�����Ϣ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadResponseToThisModule(unsigned char *msg,int sizeOfBuf,int timeout);

// ��ָ����ģ����н���
/* ���������
	recvMDLID,ָ����ģ��
	reqStr,lenOfReqStr,�ֱ�����������󳤶�
	sizeOfBuf,�ǽ��ջ���Ĵ�С
	timeout,�Ƕ���Ӧ�ĳ�ʱֵ
   ���������
	resStr,Ϊ���յ�����Ϣ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionExchangeWithSpecModule(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

// ��ָ����ģ����н���
/* ���������
	recvMDLID,ָ����ģ��
	reqStr,lenOfReqStr,�ֱ�����������󳤶�
	sizeOfBuf,�ǽ��ջ���Ĵ�С
	timeout,�Ƕ���Ӧ�ĳ�ʱֵ
   ���������
	resStr,Ϊ���յ�����Ϣ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionExchangeWithSpecModuleVerifyReqAndRes(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

int UnionExchangeWithSpecModuleVerifyReqAndResCompress(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

int UnionSendRequestToSpecifiedModuleWithOriMsgHeaderWithTime(TUnionModuleID recvMDLID,int oriMDLID, char *msg,int lenOfMsg,PUnionMessageHeader poriMsgHeader,int timeout);

//add by hzh in 2011.8.18i
int UnionSendRequestToSpecifiedModuleWithNewMsgHeaderWithTime(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader,int timeout);

#endif
