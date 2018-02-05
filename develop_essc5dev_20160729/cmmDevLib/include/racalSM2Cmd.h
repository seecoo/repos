// Author:	Wolfgang Wang
// Date:	2006/3/2

#ifndef _racalSM2Cmd_
#define _racalSM2Cmd_

#include "sjl06.h"

// ����һ��RSA��Կ
/* �������
	type������	0��ֻ����ǩ��
			1��ֻ������Կ����
			2��ǩ���͹���
	length��ģ��	0320/512/1024/2048
	pkEncoding	��Կ�ı��뷽ʽ
	lenOfPKExponent	��Կexponent�ĳ���	��ѡ����
	pkExponent	��Կ��pkExponent	��ѡ����
	exportNullPK	1�������PK��0�����뷽ʽ��PK
   	sizeOfPK	���չ�Կ�Ļ����С
   	sizeOfVK	����˽Կ�Ļ����С
   �������
   	pk		��Կ
   	lenOfVK		˽Կ���ĳ���
   	vk		˽Կ
   	errCode		�����������
*/
int RacalCmdK1(int hsmSckHDL,PUnionSJL06 pSJL06,char type,int length,char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,char *errCode);

// ʹ��SM3�㷨����HASH����
/* �������
	algorithmID	�㷨��ʶ��3-SM3
	lenOfData	���ݳ���
	hashData	��Hash������
   	sizeOfBuf	����Hash����Ļ����С
   �������
   	hashValue	Hash���
   	errCode		�����������
*/
int RacalCmdM7(int hsmSckHDL,PUnionSJL06 pSJL06,char *algorithmID,int lenOfData,char *hashData,char *hashValue,int sizeOfBuf,char *errCode);

// ����ǩ����ָ��
int RacalCmdK3(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign,char *errCode);

//������ǩ��ָ��
int RacalCmdK4(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,char *hashID,int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal,char *errCode);

#endif

