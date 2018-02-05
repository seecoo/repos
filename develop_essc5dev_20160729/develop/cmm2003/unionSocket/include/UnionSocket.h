// Author:	Wolfgang Wang
// Date:	2001/8/30

// Modification History
// 2002/3/6, Add a declaration of function UnionReceiveFromSocketUntilLen

// 2002/2/18, ��UnionTCPIPServer�����ˡ�2.x��ǰ�汾��������������2.x�汾����������������������Ϊһ������ָ�롣
// Ҫʹ��2.x�Ժ�İ汾��������ͷ�ļ����򣬱��붨��꣺_UnionSocket_2_x_

#ifndef _UnionSocket
#define _UnionSocket
  
#define SOCKET_CONNECT_TIMEOUT          50

int UnionGetLocalIpAddr(char *ip,int port,char *localIP,int sizeOfLocalIP);
int UnionGetLocalIpAddrEx(char *ip,int port,char *localIP,int sizeOfLocalIP);
int UnionGetHostByName(char *domain, char *ip);
int UnionCloseSocket(int SocketID);
int UnionCreateSocketClient(char *ip,int port);
int UnionSendToSocket(int SocketID,unsigned char *Buf,unsigned int SizeOfBuf);
int UnionReceiveFromSocket(int SocketID,unsigned char *Buf,unsigned int LenOfBuf);
int UnionReceiveFromSocketUntilLen(int SocketID,unsigned char *Buf,unsigned int LenOfBuf); // Added By Wolfgang Wang, 2002/3/6

int UnionNewTCPIPConnection(int scksvr);
int UnionInitializeTCPIPServer(int port);

#ifdef _UnionSocket_3_x_
int UnionTCPIPServer(int port,char *ServerName,int (*UnionTCPIPTaskServer)(),int (*UnionTaskActionBeforeExit)());
#else
#ifdef _UnionSocket_2_x_
int UnionTCPIPServer(int port,char *ServerName,int (*UnionTCPIPTaskServer)());
#else
int UnionTCPIPServer(int port,char *ServerName);
#endif
#endif

#define UnionErrOffsetOfSocketMdl			-10500
#define UnionErrNullIPAddressPassed			UnionErrOffsetOfSocketMdl - 1
#define	UnionErrMinusSocketPortPassed			UnionErrOffsetOfSocketMdl - 2
#define UnionErrCallingSocket				UnionErrOffsetOfSocketMdl - 3
#define UnionErrCallingConnect				UnionErrOffsetOfSocketMdl - 4
#define UnionErrCallingSetsockopt			UnionErrOffsetOfSocketMdl - 5
#define UnionErrCallingBind				UnionErrOffsetOfSocketMdl - 6
#define UnionErrCallingListen				UnionErrOffsetOfSocketMdl - 7
#define UnionErrCallingAccept				UnionErrOffsetOfSocketMdl - 8
#define UnionErrCallingMalloc				UnionErrOffsetOfSocketMdl - 9
#define UnionErrCallingSend				UnionErrOffsetOfSocketMdl - 10
#define UnionErrCallingRecv				UnionErrOffsetOfSocketMdl - 11

/*
����
	�������ݣ�������2���ֽڵĶ�����Ϊ����
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
int UnionReceiveDataFromSocketWith2BytesLenWithTimeout(int sckHDL,char *buf,int sizeOfBuf,int timeout);

/*
����
	�������ݣ�������2���ֽڵĶ�����Ϊ����
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
int UnionSendToSocketWithTowBytesLen(int fd,unsigned char *buf, unsigned int len);

/*
����
	�������ݣ�������2���ֽڵĶ�����Ϊ���ȣ������Ƴ���Ҳ�����ڽ��յ�������
�������
	sckHDL		socket���
	sizeOfBuf	�������ݵĻ������Ĵ�С
	timeout		��ʱ
�������
	buf		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ����������Ƶĳ���������
	<0		�������
*/
int UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout(int sckHDL,unsigned char *buf,int sizeOfBuf,int timeout);

/*
����
	��ָ���ķ���������һ������
�������
	ipAddr		��������IP��ַ����ָ���������127.0.0.1
	port		�������Ķ˿�
	reqStr		�������ݴ�
	lenOfReqStr	�������ݴ�����
	sizeOfBuf	�������ݵĻ������Ĵ�С
	timeout		��ʱ
�������
	resStr		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ������������Ƶĳ���������
	<0		�������
*/
int UnionCommWithSpecSvrWith2BytsLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf,int timeout);

/*
����
	��ָ���������Ͻ���һ������
�������
	sckHDL		����
	reqStr		�������ݴ�
	lenOfReqStr	�������ݴ�����
	sizeOfBuf	�������ݵĻ������Ĵ�С
	timeout		��ʱ
�������
	resStr		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ������������Ƶĳ���������
	<0		�������
*/
int UnionCommWithSpecSckHDLWith2BytsLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf,int timeout);

/*
����
	��ָ���������Ͻ���һ������
�������
	sckHDL		����
	reqStr		�������ݴ�
	lenOfReqStr	�������ݴ�����
	sizeOfBuf	�������ݵĻ������Ĵ�С
�������
	resStr		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ������������Ƶĳ���������
	<0		�������
*/
int UnionCommWithSpecSckHDLWithoutLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

/*
����
	��ָ���������Ͻ���һ������,���Ĳ���2����
�������
	sckHDL		����
	reqStr		�������ݴ�
	lenOfReqStr	�������ݴ�����
	sizeOfBuf	�������ݵĻ������Ĵ�С
�������
	resStr		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ������������Ƶĳ���������
	<0		�������
*/
int UnionCommWithSpecSckHDLInBitsFormatWithoutLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

/*
����
	��ָ���ķ���������һ������
�������
	ipAddr		��������IP��ַ����ָ���������127.0.0.1
	port		�������Ķ˿�
	reqStr		�������ݴ�
	lenOfReqStr	�������ݴ�����
	sizeOfBuf	�������ݵĻ������Ĵ�С
�������
	resStr		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ������������Ƶĳ���������
	<0		�������
*/
int UnionCommWithSpecSvrWithoutLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

/*
����
	��ָ���ķ���������һ������,���Ĳ��ö�������
�������
	ipAddr		��������IP��ַ����ָ���������127.0.0.1
	port		�������Ķ˿�
	reqStr		�������ݴ�
	lenOfReqStr	�������ݴ�����
	sizeOfBuf	�������ݵĻ������Ĵ�С
�������
	resStr		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ������������Ƶĳ���������
	<0		�������
*/
int UnionCommWithSpecSvrInBitsFormatWithoutLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

/*
// 2011-1-23,����������
����
	��һ�������Ͻ��ձ���,����ǰ�����2���ֽڵĳ��ȣ�2�ֽڳ�����2������
�������
	fd		����
	sizeOfBuf	�������ݵĻ������Ĵ�С
	maxWaitTime	���ȴ�ʱ��
�������
	buf		�������ݵĻ�����
����ֵ
	>=0		���յ����ݵĳ��ȣ������������Ƶĳ���������
	<0		�������
*/
int UnionReceiveFromSocketWith2BytesLen(int fd, unsigned char *buf,unsigned int sizeOfBuf,int maxWaitTime);

int UnionSetSockSendTimeout(int sckcli, int timeoutSecond);

#endif
