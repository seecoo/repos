// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2011-1-23

#ifndef _commManager_
#define _commManager_

/*
����
	��ȡ��ǰͨѶ���
�������
	��
�������
	��
����ֵ
	>=0		��ǰͨѶ���
	<0		ʧ�ܣ�������
*/
int UnionGetActiveTCIPSvrSckHDL();

/*
����
	���õ�ǰͨѶ���
�������
	sckHDL		��ǰͨѶ���
�������
	��
����ֵ
	��
*/
void UnionSetActiveTCPIPSvrSckHDL(int sckHDL);

/*
����
	������Ŀ��1
�������
	��
�������
	��
����ֵ
	��
*/
void UnionIncreaseClientPackNum();

/*
����
	�Ǽ�����
�������
	cli_addr	�ͻ�����Ϣ
	port		�ͻ��˶˿�
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ�ܣ�������
*/
int UnionRegisterTcpipConnTask(struct sockaddr_in *cli_addr,int port,int (*funName)());

#endif
