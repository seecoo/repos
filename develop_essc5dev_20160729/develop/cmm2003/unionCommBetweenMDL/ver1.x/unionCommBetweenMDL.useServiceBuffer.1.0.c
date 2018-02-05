// Wolfgang Wang
// 2010-12-26

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "unionREC.h"
#include "unionMsgBuf6.x.h"
#include "unionServiceBuffer.h"
#include "UnionLog.h"

unsigned long	gunionServiceNum = 0;	// ��������

int UnionConnectCommBetweenMDLs()
{
	return(UnionConnectServiceBuffer());
}

int UnionDisconnectCommBetweenMDLs()
{
	return(UnionDisconnectServiceBuffer());
}

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
int UnionSendRequestToSpecifiedModuleWithNewMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader)
{
	unsigned char		buf[8192+256];
	TUnionMessageHeader	msgHeader;
	int			offset = 0;
	int			ret;
		
	if ((lenOfMsg + sizeof(msgHeader) > sizeof(buf)) || (lenOfMsg < 0))
	{
		UnionUserErrLog("in UnionSendRequestToSpecifiedModuleWithNewMsgHeader:: lenOfMsg [%d] error!\n",lenOfMsg);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	// ��ͷ���и�ֵ
	msgHeader.len = lenOfMsg;
	time(&(msgHeader.time));
	msgHeader.provider = getpid();
	msgHeader.dealer = 0;
	msgHeader.msgIndex = ++gunionServiceNum; 
	msgHeader.type = recvMDLID;
	memcpy(buf,&msgHeader,sizeof(msgHeader));
	offset = sizeof(msgHeader);
	// �����Ľ��и�ֵ
	memcpy(buf+offset,msg,lenOfMsg);
	offset += lenOfMsg;
	
	if ((ret = UnionSendRequestOfSpecService(recvMDLID,buf,offset)) < 0)
	{
		UnionUserErrLog("in UnionSendRequestToSpecifiedModuleWithNewMsgHeader:: UnionSendRequestOfSpecService recvMDLID = [%ld]\n",recvMDLID);
		return(ret);
	}
	if (pmsgHeader != NULL)
		memcpy(pmsgHeader,&msgHeader,sizeof(*pmsgHeader));
	return(ret);
}

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
int UnionSendRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg)
{
	return(UnionSendRequestToSpecifiedModuleWithNewMsgHeader(recvMDLID,msg,lenOfMsg,NULL));
}

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
int UnionSendResponseToApplyModuleWithOriMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader poriMsgHeader)
{
	unsigned char		buf[8192+256];
	int			ret;
	int			offset = 0;
	
	if (poriMsgHeader == NULL)
	{
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: poriMsgHeader is NULL!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	if ((sizeof(*poriMsgHeader) + lenOfMsg >= sizeof(buf)) || (lenOfMsg < 0))
	{
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: lenOfMsg = [%d] error!\n",lenOfMsg);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	// �жϽ��ܽ����Ƿ񻹴���
	if (kill(poriMsgHeader->provider,0) != 0)
	{
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: the process [%d] of mdl [%ld] not exists!\n",poriMsgHeader->provider,recvMDLID);
		return(UnionSetUserDefinedErrorCode(errCodeEsscMDL_ProcNotExistsAnymore));
	}
	poriMsgHeader->dealer = getpid();
	memcpy(buf,poriMsgHeader,sizeof(*poriMsgHeader));
	offset = sizeof(*poriMsgHeader);
	memcpy(buf+offset,msg,lenOfMsg);
	offset += lenOfMsg;
	if ((ret = UnionSendResponseOfSpecService(poriMsgHeader->type,buf,offset,poriMsgHeader->provider)) < 0)
	{
		msg[lenOfMsg] = 0;
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: UnionSendResponseOfSpecService\nmessage=[%04d][%s]\n",lenOfMsg,msg);
		return(ret);
	}
	return(ret);
}

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
int UnionSendResponseToApplyModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg)
{
	return(UnionSendResponseToApplyModuleWithOriMsgHeader(recvMDLID,msg,lenOfMsg,NULL));
}

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
int UnionReadRequestToSpecifiedModuleWithMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID,PUnionMessageHeader pmsgHeader)
{
	unsigned char		buf[8192+256];
	int			ret;
	int			len;

loop:	
	if ((ret = UnionReadRequestOfSpecService(recvMDLID,buf,sizeof(buf))) < 0)
	{
		UnionUserErrLog("in UnionReadRequestToSpecifiedModuleWithMsgHeader:: UnionReadRequestOfSpecService [%ld]\n",recvMDLID);
		return(ret);
	}
	if ((len = ret - sizeof(*pmsgHeader)) > sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadRequestToSpecifiedModuleWithMsgHeader:: sizeOfBuf [%d] too small\n",sizeOfBuf);
		//return(errCodeSmallBuffer);
		goto loop;
	}
	if (len < 0)
	{
		UnionUserErrLog("in UnionReadRequestToSpecifiedModuleWithMsgHeader:: msgLen [%d] too short\n",ret);
		//return(errCodeEsscMDL_MsgForThisModuleTooShort);
		goto loop;
	}
	if (pmsgHeader != NULL)
		memcpy(pmsgHeader,buf,sizeof(*pmsgHeader));
	memcpy(msg,buf+sizeof(*pmsgHeader),len);
	if (pmsgHeader->type != recvMDLID)
	{
		msg[len] = 0;
		UnionUserErrLog("in UnionReadRequestToSpecifiedModule:: dealerMDLID [%ld] != expected [%ld]\nmessage=[%04d][%s]\n",
					 pmsgHeader->type,recvMDLID,len,msg);
		goto loop;
	}
	if (prequesterMDLID != NULL)
		*prequesterMDLID = pmsgHeader->type;
	return(len);
}

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
int UnionReadRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID)
{
	return(UnionReadRequestToSpecifiedModuleWithMsgHeader(recvMDLID,msg,sizeOfBuf,prequesterMDLID,NULL));
}

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
int UnionReadFirstRequestSendToThisModule(unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID)
{
	return(UnionReadRequestToSpecifiedModuleWithMsgHeader(UnionGetMyFixedMDLID(),msg,sizeOfBuf,prequesterMDLID,NULL));
}

// ��ȡ������ģ�����Ӧ
/* ���������
	timeout,�Ƕ���Ӧ�ĳ�ʱֵ
	sizeOfBuf,�ǽ��ջ���Ĵ�С
	pmsgHeader,��ԭʼ������Ϣ����Ϣͷ
   ���������
	msg,Ϊ���յ�����Ϣ
	pmsgHeader,��������Ϣ����Ϣͷ�������ʼ��ΪNULL���򲻶�����Ϣͷ
   ����ֵ��
   	>=0����Ϣ�ĳ���
   	��ֵ��������
*/
int UnionReadResponseToThisModuleWithMsgHeader(unsigned char *msg,int sizeOfBuf,int timeout,PUnionMessageHeader pmsgHeader)
{
	unsigned char		buf[8192+256];
	int			ret;
	int			len;
	int			recvMDLID;
	
	if (pmsgHeader == NULL)
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: pmsgHeader is NULL!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	if ((ret = UnionReadResponseOfSpecService(pmsgHeader->type,buf,sizeof(buf),pmsgHeader->provider,timeout)) < 0)
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: UnionReadResponseOfSpecService read msg of MDLID [%ld]\n",pmsgHeader->provider);
		return(ret);
	}
	if ((len = ret - sizeof(*pmsgHeader)) > sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: sizeOfBuf [%d] too small\n",sizeOfBuf);
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	if (len < 0)
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: msgLen [%d] too short\n",ret);
		return(UnionSetUserDefinedErrorCode(errCodeEsscMDL_MsgForThisModuleTooShort));
	}
	memcpy(msg,buf+sizeof(*pmsgHeader),len);
	memcpy(pmsgHeader,buf,sizeof(*pmsgHeader));
	return(len);
}

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
int UnionReadResponseToThisModule(unsigned char *msg,int sizeOfBuf,int timeout)
{
	return(UnionReadResponseToThisModuleWithMsgHeader(msg,sizeOfBuf,timeout,NULL));
}

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
int UnionExchangeWithSpecModule(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout)
{
	int			ret;

	if ((ret = UnionSendRequestToSpecifiedModule(recvMDLID,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModule:: UnionSendRequestToSpecifiedModule!\n");
		return(ret);
	}
	if ((ret = UnionReadResponseToThisModule(resStr,sizeOfResStr,timeout)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModule:: UnionReadResponseToThisModule!\n");
		return(ret);
	}
	return(ret);
}

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
int UnionExchangeWithSpecModuleVerifyReqAndRes(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout)
{
	int			ret;
	TUnionMessageHeader	reqMsgHeader;
	TUnionMessageHeader	resMsgHeader;
	time_t			start,finish;

	UnionResetServiceBufferLeftTimeout(timeout);
	
	if ((ret = UnionSendRequestToSpecifiedModuleWithNewMsgHeader(recvMDLID,reqStr,lenOfReqStr,&reqMsgHeader)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes:: UnionSendRequestToSpecifiedModuleWithNewMsgHeader!\n");
		return(ret);
	}
	time(&start);
loop:
	memcpy(&resMsgHeader,&reqMsgHeader,sizeof(resMsgHeader));
	if ((ret = UnionReadResponseToThisModuleWithMsgHeader(resStr,sizeOfResStr,-1,&resMsgHeader)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes:: UnionReadResponseToThisModuleWithMsgHeader!\n");
		return(ret);
	}
	if ((reqMsgHeader.time != resMsgHeader.time) || (reqMsgHeader.msgIndex != resMsgHeader.msgIndex) ||
		(reqMsgHeader.provider != resMsgHeader.provider) || (reqMsgHeader.dealer != 0) || 
		(resMsgHeader.dealer == 0))
	{
		UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes::\nReq=[%08ld-%012ld-%012ld-%07d-%07d]\nRes=[%08ld-%012ld-%012ld-%07d-%07d]\n",
						reqMsgHeader.time-start,reqMsgHeader.msgIndex,reqMsgHeader.type,reqMsgHeader.provider,reqMsgHeader.dealer,
						resMsgHeader.time-start,resMsgHeader.msgIndex,resMsgHeader.type,resMsgHeader.provider,resMsgHeader.dealer);
		time(&finish);
		if (finish - start > timeout)
			return(errCodeEsscMDL_ReqAndResNotIsIndentified);
		else
			goto loop;
	}
	return(ret);
}

