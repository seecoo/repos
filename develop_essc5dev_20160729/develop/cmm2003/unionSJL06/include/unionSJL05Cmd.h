
#ifndef _UnionSJL05Cmd_
#define	_UnionSJL05Cmd_ 

#include "unionDesKey.h"

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

int UnionSJL05CmdX0(char mode, char *mediaType, char keyType,int compNum, int keyLen, int derivateNum, char derivateData[][32 + 1], int printNum, char printFld[][32], char *value, char *checkvalue);

/*
������	����	����	��ע
������	2	A	��A2��
Key type	3	H	��Կ����
Key scheme ��LMK��	1	A	��LMK���ܷ�ʽ��־
Print Field 0	n	A	��ӡ�ֶ�0����������;��
Delimiter	1	A	ֵΪ����������ӡ�ֶν�����
Print Field 1	n	A	��ӡ�ֶ�1����������;��
��	��	��	��
Last Print Field 	n	A	���һ����ӡ�ֶΣ���������;��
��Ϣβ	Nt	A	
�����	����	����	��ע
��Ϣͷ	Nh	A	��������ͬ
��Ӧ����	2	A	��A3��
�������	2	N	����Ϊ��00��
			��13����LMK��
��15�����������ݴ���
��17������Ȩ��֤ʧ��
��18������ʽû�ж�����ӡ������
Component	16H/
1A+32H/
1A+48H	H          	���ֵ�LMK���ܺ����Կ����
*/

int UnionSJL05CmdA2(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component);

/*
   �������ܣ�
   A4ָ�����Կ��������������������ܺ�ϳ�һ����Կ�����ö�ӦLMK���ܺ󷵻ء�
   ���������
   keyType����Կ������
   keyLength����Կ�ĳ���
   partKeyNum����Կ�ɷֵ�����
   partKey����Ÿ�����Կ�ɷֵ����黺�壬ΪLMK���ܵ���Կ����
   ���������
   keyByLMK���ϳɵ���Կ���ģ���LMK����
   checkValue���ϳɵ���Կ��У��ֵ

 */
int UnionSJL05CmdA4(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue);

int UnionSJL05CmdA8(TUnionDesKeyType keyType,char *key,char *zmk,char *keyByZMK,char *checkValue);

int UnionSJL05CmdBU(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue);

int UnionSJL05CmdE0(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

int UnionSJL05CmdE0_f(int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

// ����һ��RSA��Կ
/* �������
   mode��ģʽ	��0����������Կ��������HSM�ڣ�ֻ�����Կ
							��1����������Կ��������HSM�ڣ������Կ��LMK������˽Կ
							��2����������Կ�������Կ��LMK������˽Կ
							��3����������Կ��������HSM�ڣ��������˽Կ
							��4��-������Կ������Կ���뵽IC������
   length��ģ��	0320/512/1024/2048
   pkEncoding	��Կ�ı��뷽ʽ
   index	������Կ�洢��HSM�ڵ������š�Mode!=0ʱ�����򲻴��ڣ��磺G000
   lenOfPKExponent	��Կexponent�ĳ���	��ѡ����
   pkExponent	��Կ��pkExponent	��ѡ����
   sizeOfPK	���չ�Կ�Ļ����С
   sizeOfVK	����˽Կ�Ļ����С
   �������
   pk		��Կ
   lenOfVK		˽Կ���ĳ���
   vk		˽Կ
 */
int UnionSJL05CmdEI(char mode,int length,char *pkEncoding,char *index,
		int lenOfPKExponent,unsigned char *pkExponent,char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

/*
�������ܣ�
	EWָ���˽Կǩ�� 
���������
	hashID:	HASH��ʶ
	signID:	ǩ����ʶ
	padModeID: ���ģʽ��ʶ	
	indexOfVK��˽Կ������
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	vkLength��LMK���ܵ�˽Կ����
	vk��LMK���ܵ�˽Կ
���������
	signature�����ɵ�ǩ��
����ֵ��
	<0������ִ��ʧ�ܣ���signature
	0�����ܻ�ִ��ָ��ʧ��
	>0���ɹ�������signature�ĳ���
*/

int UnionSJL05CmdEW(char *hashID,char *signID,char *padModeID,char *indexOfVK,int signDataLength,char *signData,int vkLength,char *vk,char *signature);

/*
   �������ܣ�
   UAָ�����ת��RSA˽Կ��LMK��KEK,LMK������RSA˽ԿΪREF�ṹ,KEKʹ��Mode������㷨����REF�ṹ��RSA˽Կ��ÿ��Ԫ�ء�
   ���������
	keyType����Կ����
	key: ����Կ����HSM�ڰ�ȫ����ʧ�洢������Կ�Լ���LMK���ܵ���������
	vkLength: ˽Կ���ݵ����ݳ���
	vk: nB ��LMK������RSA˽Կ��REF�ṹ��
	���������
	vkByKey: key���ܵ�vk���ݴ�
 */

int UnionSJL05CmdUA(TUnionDesKeyType keyType,char *key, int vkLength, char *vk, char *vkByKey);

/*
   �������ܣ�
   UCָ�ʹ��ָ����Ӧ������Կ����2����ɢ�õ���ƬӦ������Կ��
   ʹ��ָ��������Կ����Կ���м��ܱ������������MAC���㡣
   ���������
   
	mk: ����Կ
	mkType: ����Կ����109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
	mkDvsNum: ����Կ��ɢ����, 1-3��
	mkDvsFlag: ����Կ��ɢ�㷨 1��������׼
	mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

	pkType: ������Կ���� 0=TK(������Կ)1=DK-SMC(ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ)
	pk: ������Կ
	pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
	pkDvsFlag: ������Կ��ɢ�㷨 1��������׼
	pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������

���������
criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
criperData: nB �������������
checkValue: У��ֵ
 */
int UnionSJL05CmdUC(char algoType, char *mkType, char *mk,
		int mkDvsNum, char *mkDvsFlag, char *mkDvsData, char *pkType, char *pk,
		int pkDvsNum, char *pkDvsFlag, char *pkDvsData, int *criperDataLen, char *criperData, char *checkValue);

// ����һ��SM2��Կ
/* �������
   	type������
	1��ǩ����2�����ܣ�3��ǩ���ͼ���
   	length������ �̶�256
   	index,	��Կ����
   	keyPasswd,	��Կ����
   	sizeOfPK	���չ�Կ�Ļ����С
   	sizeOfVK	����˽Կ�Ļ����С
   	�������
	pk		��Կ
   	lenOfVK		˽Կ���ĳ���
   	vk		˽Կ
   	vkByLmk		��˽Կ����
 */
int UnionSJL05CmdUO(char type,int length,	char *index, char *keyPasswd, char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,unsigned char *vkByLmk);

// ����ǩ����ָ��
int UnionSJL05CmdUQ(char *hashID, int lenOfUsrID, char *usrID, int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign);

// �����㷨���������Կ
int UnionSJL05CmdWI(char mode, char *mediaType, char keyType,int compNum, int keyLen, int derivateNum, char derivateData[][32 + 1], int printNum,	char printFld[][32], char *value, char *checkvalue);

// ������ԿУ��ֵ
int UnionSJL05CmdBS(int SM4Mode, TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue);

//������ǩ��ָ��
int UnionSJL05CmdUS(int vkIndex, char *hashID, int lenOfUsrID, char *usrID, int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal);

// �����㷨ǩ��ָ��
int UnionSJL05CmdTI(int pkLength, char *pk, int inputDataLength, unsigned char *inputData, unsigned char *outputData);

//SM2˽Կת����
/*�������
  algoType	�㷨��ʶ
  vkByLmk		˽Կ���ģ�LMK(36,37)����
  keyType		��Կ����
  key		����Կ�����µĴ���(SM4)��Կ
  encMode	�����㷨ģʽ
  iv		CBCģʽʱ����  DES/3DES ��8�ֽ�   SM4��16�ֽ�
  
  �������
  vk		�����µ�SM2˽Կ����
 */
int UnionSJL05CmdUY(char algoType, unsigned char *vkByLmk, TUnionDesKeyType keyType,char *key,char encMode,char *iv,char *vk);


// �����ӡ��ʽ
int UnionSJL05CmdPA(char *format);

// ��������ӡһ����Կ���� �����㷨
int UnionSJL05CmdWM(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component);

#endif

