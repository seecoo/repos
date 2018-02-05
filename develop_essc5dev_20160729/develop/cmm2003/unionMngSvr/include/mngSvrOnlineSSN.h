// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2009/5/23
// Version:	1.0

#ifndef _mngSvrOnlineSSN_
#define _mngSvrOnlineSSN_

int UnionApplyMyMngSvrSSN();

void UnionRegisterMngSvrRequestService(int resID,int serviceID,char *tellerNo,int lenOfData,char *data);

void UnionRegisterMngSvrResponseService(int resID,int serviceID,int responseCode,int lenOfData,char *data,int fileRecved);

/*
���ܣ�
	ƴװ��ǰ��ˮ��ͷ
�������:
	sizeOfBuf	���ռ�¼�ִ��Ļ���Ĵ�С
�������:
	recStr	��¼�ִ�
����ֵ
	>=0		�ɹ�,ƴװ�ļ�¼���ĳ���
	<0		ʧ��,������
*/
int UnionFormHeaderStrOfCurrentMngSvrOnlineRec(char *recStr,int sizeOfBuf);

#endif

