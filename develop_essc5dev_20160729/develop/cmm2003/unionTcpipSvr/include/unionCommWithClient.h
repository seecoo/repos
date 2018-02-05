// author	Wolfgang Wang
// date		2010-10-25

#ifndef _unionCommWithClient_
#define _unionCommWithClient_

/*
����
	�ӿͻ��ˣ��������ݣ�������2���ֽڵĶ�����Ϊ����
	���յ������ݣ�����������λ
�������
	sckHDL		socket���
	sizeOfBuf	�������ݵĻ������Ĵ�С
	timeout		��ʱ
�������
	buf		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ���
	<0		�������
*/
int UnionRecvInternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout);

/*
����
	�ӿͻ��ˣ��������ݣ�������2���ֽڵĶ�����Ϊ����
	���յ������ݣ�����������λ
�������
	sckHDL		socket���
	sizeOfBuf	�������ݵĻ������Ĵ�С
	timeout		��ʱ
�������
	buf		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ���
	<0		�������
*/
int UnionRecvExternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout);

/*
����
	�ӿͻ��ˣ��������ݣ�������2���ֽڵĶ�����Ϊ����
	���յ������ݣ�����������λ
�������
	sckHDL		socket���
	sizeOfBuf	�������ݵĻ������Ĵ�С
	timeout		��ʱ
�������
	buf		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ���
	<0		�������
*/
int UnionRecvPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout);

/*
����
	��ͻ��˷������ݣ�������2���ֽڵĶ�����Ϊ����
�������
	sckHDL		socket���
	buf		Ҫ���͵����ݣ�����������λ
	len		���ݳ���
�������
	��
����ֵ
	>=0		ʵ�ʷ��͵����ݵĳ���
	<0		�������
*/
int UnionSendInternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len);

/*
����
	��ͻ��˷������ݣ�������2���ֽڵĶ�����Ϊ����
�������
	sckHDL		socket���
	buf		Ҫ���͵����ݣ�����������λ
	len		���ݳ���
�������
	��
����ֵ
	>=0		ʵ�ʷ��͵����ݵĳ���
	<0		�������
*/
int UnionSendExternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len);

/*
����
	��ͻ��˷������ݣ�������2���ֽڵĶ�����Ϊ����
�������
	sckHDL		socket���
	buf		Ҫ���͵����ݣ�����������λ
	len		���ݳ���
�������
	��
����ֵ
	>=0		ʵ�ʷ��͵����ݵĳ���
	<0		�������
*/
int UnionSendPackToClient(int sckHDL,unsigned char *buf, unsigned int len);

#endif
