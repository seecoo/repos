// Wolfgang Wang
// 2010-8-19

#ifndef _sjl05HsmApiSample_
#define _sjl05HsmApiSample_

/*
����
	�����������֮�������
�������
	ipAddr		�����IP��ַ
	port		������˿�
�������
	��
����ֵ
	>=0		�ɹ������������
	<0		�������
*/
int ConnectHsmSJL05(char *ipAddr,int port);

/*
����
	�ر��������֮�������
�������
	��
�������
	��
����ֵ
	>=0		�ɹ������������
	<0		�������
*/
int CloseHsmSJL05();

/*
����
	ִ�������ָ��
�������
	reqStr		���������
	lenOfReqStr	��������󴮳���
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		�������Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int ExcuteHsmSJL05Cmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

#endif

