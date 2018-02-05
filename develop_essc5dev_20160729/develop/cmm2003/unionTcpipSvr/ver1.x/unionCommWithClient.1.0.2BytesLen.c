// Author:	Wolfgang Wang
// Date:	2011/1/23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionREC.h"

#include "unionCommWithClient.h"
#include "commManager.h"
#include "UnionLog.h"

#ifdef _client_use_ebcdic_
#include "ebcdicAscii.h"
#endif

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
int UnionRecvInternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout)
{
	return(UnionRecvPackFromClient(sckHDL,buf,sizeOfBuf,timeout));
}

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
int UnionRecvExternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout)
{
	return(UnionRecvPackFromClient(sckHDL,buf,sizeOfBuf,timeout));
}

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
int UnionRecvPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout)
{
	int	ret;
	
	if ((ret = UnionReceiveDataFromSocketWith2BytesLenWithTimeout(sckHDL,buf,sizeOfBuf,timeout)) < 0)
	{
		UnionUserErrLog("in UnionRecvPackFromClient:: UnionReceiveDataFromSocketWith2BytesLenWithTimeout!\n");
		return(ret);
	}
	UnionIncreaseClientPackNum();	// ���ͻ��˰���Ŀ��1
#ifdef _client_use_ebcdic_
	// �ͻ���ʹ��ebcdic��ͨѶ
	UnionEbcdicToAscii(buf,buf,ret);
#endif
#ifndef _noSpierInfo_
	UnionSendRequestInfoToTransSpier(ret,buf);	// ��������Ϣ
#else
	UnionNullLog("[%04d][%s]\n",ret,buf);
#endif		
	return(ret);
}

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
int UnionSendInternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len)
{
	return(UnionSendPackToClient(sckHDL,buf,len));
}

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
int UnionSendExternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len)
{
	return(UnionSendPackToClient(sckHDL,buf,len));
}

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
int UnionSendPackToClient(int sckHDL,unsigned char *buf, unsigned int len)
{
	int	ret;
	
#ifndef _noSpierInfo_
	UnionSendResponseInfoToTransSpier(len,buf); // ��������Ϣ
#else
	UnionNullLog("[%04d][%s]\n",len,buf);
#endif	
#ifdef _client_use_ebcdic_	// ��������ת��
	UnionAsciiToEbcdic(buf,buf,len);
#endif
	if ((ret = UnionSendToSocketWithTowBytesLen(sckHDL,buf,len)) < 0)
	{
		UnionUserErrLog("in UnionSendPackToClient:: UnionSendToSocketWithTowBytesLen!\n");
		return(ret);
	}
	UnionIncreaseClientPackNum();	// ���ͻ��˰���Ŀ��1
	return(ret);
}
