// Author:	Wolfgang Wang
// Date:	2006/3/2

#ifndef _racalRsaCmd_
#define _racalRsaCmd_

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
int RacalCmdEI(int hsmSckHDL,PUnionSJL06 pSJL06,char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,char *errCode);

// �洢˽Կ
/* �������
	vkIndex		˽Կ��������
	lenOfVK		˽Կ����
	vk		˽Կ
   �������
   	errCode		�����������
*/
int RacalCmdEK(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,int lenOfVK,unsigned char *vk,char *errCode);

// ���ɹ�Կ��MACֵ
int RacalCmdEO(int hsmSckHDL,PUnionSJL06 pSJL06,char *pkEncoding,int isNullPK,int lenOfPK,char *pk,char *authData,char *mac,char *errCode);

// ��һ��LMK���ܵ���Կת��Ϊ��Կ����
int RacalCmdGK(int hsmSckHDL,PUnionSJL06 pSJL06,char *encyFlag,char *padMode,char *lmkType,char *desKeyByLMK,
		char *checkValue,char *pkMac,char *pk,char *authData,
		char *initValue,char *desKeyByPK,int sizeOfBuf,char *errCode);

/* 
ʮһ��	����ժҪ

*/
int RacalCmdGM(int hsmSckHDL,PUnionSJL06 pSJL06,char *method,int lenOfData,char *data,
		char *hashResult,int sizeOfBuf,char *errCode);

// ����ǩ����ָ��
int RacalCmdEW(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,int lenOfData,char *data,
		int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign,char *errCode);

// ��֤ǩ����ָ��
int RacalCmdEY(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData,char *errCode);

// ���ɹ�Կ��MACֵ
// ���30�����ܻ�����Կ���ǰ���׼ASN.1��ʽ�����DER��λ��
int RacalCmdEO_For30Hsm(int hsmSckHDL,PUnionSJL06 pSJL06,char *pkEncoding,int isNullPK,
	int lenOfPK,char *pk,char *authData,char *mac,char *errCode);
		
// ��֤ǩ����ָ��
// ���30�����ܻ�����Կ���ǰ���׼ASN.1��ʽ�����DER��λ��
int RacalCmdEY_For30Hsm(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData,char *errCode);

/*
	��һ����Կ���ܵ���Կת��ΪLMK����
*/
int RacalCmdGI(int hsmSckHDL,PUnionSJL06 pSJL06, char *encyFlag, char *padMode, 
	char *lmkType, int keyLength, int lenOfDesKeyByPK,
	char *desKeyByPK, int vkIndex, int lenOfVK, unsigned char *vk,
	char *initValue, char *desKeyByLMK, char *checkValue, char *errCode);
		
#endif

