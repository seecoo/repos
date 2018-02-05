//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _UnionHsmCmd_
#define _UnionHsmCmd_

#include "unionDesKey.h"

int UnionSetIsUseNormalZmkType();
int UnionSetUseSpecPVKI(int pvki);
int UnionGetUseSpecPVKI();

int UnionTranslateDesKeyTypeTo3CharFormat(TUnionDesKeyType desKeyType, char *keyType);

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
 */
int UnionHsmCmdEI(char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

// �洢˽Կ
/* �������
   vkIndex		˽Կ��������
   lenOfVK		˽Կ����
   vk		˽Կ
   �������
 */
int UnionHsmCmdEK(int vkIndex,int lenOfVK,unsigned char *vk);

// ���ɹ�Կ��MACֵ
int UnionHsmCmdEO(char *pkEncoding,int isNullPK,int lenOfPK,char *pk,char *authData,char *mac);

// ��֤��Կ��MACֵ
int UnionHsmCmdEQ(char *pk,char *authData,char *mac);

// ��һ��LMK���ܵ���Կת��Ϊ��Կ����
int UnionHsmCmdGK(char *encyFlag,char *padMode,char *lmkType,char *desKeyByLMK,
		char *checkValue,char *pkMac,char *pk,char *authData,
		char *initValue,char *desKeyByPK,int sizeOfBuf);

/* 
   ʮһ��	����ժҪ

 */
int UnionHsmCmdGM(char *method,int lenOfData,char *data,
		char *hashResult,int sizeOfBuf);

// ����ǩ����ָ��
int UnionHsmCmdEW(char *hashID,char *signID,char *padID,int lenOfData,char *data,
		int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign);

// ��֤ǩ����ָ��
int UnionHsmCmdEY(char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData);

/*
   ��һ����Կ���ܵ���Կת��ΪLMK����
 */
int UnionHsmCmdGI(char *encyFlag, char *padMode, 
		char *lmkType, int keyLength, int lenOfDesKeyByPK,
		char *desKeyByPK, int vkIndex, int lenOfVK, unsigned char *vk,
		char *initValue, char *desKeyByLMK, char *checkValue);

// ����һ��SM2��Կ
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
 */
int UnionHsmCmdK1(char type,int length,	char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

// ����ǩ����ָ��
int UnionHsmCmdK3(char *hashID,int lenOfUsrID, char *usrID,int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign);

//������ǩ��ָ��
int UnionHsmCmdK4(int vkIndex,char *hashID,int lenOfUsrID, char *usrID,int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal);

// ʹ��SM3�㷨����HASH����
/* �������
   algorithmID	�㷨��ʶ��3-SM3
   lenOfData	���ݳ���
   hashData	��Hash������
   sizeOfBuf	����Hash����Ļ����С
   �������
   hashValue	Hash���
 */
int UnionHsmCmdM7(char *algorithmID,int lenOfData,char *hashData,char *hashValue,int sizeOfBuf);

int UnionHsmCmd38(char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk);

int UnionHsmCmd30(char flag,char *pkIndex,char *pk,int dataLen,char *data,char *encData);

int UnionHsmCmdNC(char *version);

int UnionHsmCmd31(char *bmkIndex,char *bmkValue);

int UnionHsmCmd11(TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue);

int UnionHsmCmd2A(TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue);


int UnionHsmCmd34(char *vkIndex,int lenOfVK,char *pk,int sizeOfPK, char *vk, int sizeOfVk);

int UnionHsmCmd35(char *vkIndex,char *vkByMK,int lenOfVKByMK);

int UnionHsmCmd36(char *vkIndex,char *vkByMK,int sizeOfVKByMK);

/*
   ת��DES��Կ��������Կ���ܵ���Կ����
   ���ڷַ���Կ��
   �������
   pk ��Կ
   keyByMK ��Կ����
   �������
   checkValue ���ֵ 
   sizeOfBuf ��Կ����
   keyByPK ��Կ���ģ��ù�Կ���ܵ�DES��Կ��
 */
int UnionHsmCmd3B(char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue);

/*
   ת��DES��Կ���ӹ�Կ���ܵ�����Կ����
   ���ڽ�����Կ��
   �������
   vkIndex ˽Կ������"00"��"20"����������ڵ�˽Կ
   lenOfKeyByPK ��Կ���ȣ�DES��Կ���ĵ��ֽ���
   keyByPK �ù�Կ���ܵ�DES��Կ
   �������
   keyByMK ������Կ���ܵ�DES��Կ
   checkValue ���ֵ
 */
int UnionHsmCmd3A(int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue);

int UnionHsmCmd3E(char *bmk,char *pk,char *keyByBMK,char *checkValue,char *keyByPK);

int UnionHsmCmd3C(char flag,int lenOfData,char *data,char *hash);
int UnionHsmCmd3CForEEMG(char flag,int lenOfData,char *data,char *hash);

int UnionHsmCmd37(char flag,char *vkIndex,int lenOfVK,char *vk,int dataLen,char *data,char *signature,int sizeOfSignature);


int UnionHsmCmd40(char *vkIndex,char *bmk,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK);

int UnionHsmCmd41(char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK);

int UnionHsmCmd42(char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText);

/*
   ���ܣ����ɹ�Կ���ܵ�PINת����ANSI9.8��׼
   �������
   vkIndex ˽Կ������ָ����˽Կ�����ڽ���PIN��������
   type ��Կ���ͣ�1��TPK  2��ZPK
   zpk ���ڼ���PIN����Կ
   pinType PIN���ͣ�1��ANSI9.8
   pan �û����ʺ�
   pinByPK ����Կ���ܵ�PIN��������
   lenOfPinByPK ���ĳ���
   �������
   pinByZPK ���ص�PIN����--ANSI9.8
 */
int UnionHsmCmd43(char *vkIndex,int lenOfVK,char *vk,char type,char *zpk,char pinType,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK);

/*
   ���ܣ����ɹ�Կ���ܵ�PINת���ɾ����������㷨����
   �������
   vkIndex ˽Կ������ָ����˽Կ�����ڽ���PIN��������
   pinByPK ����Կ���ܵ�PIN��������
   lenOfPinByPK ���ĳ���
   �������
   hash ���ص�PIN����--ANSI9.8
 */
int UnionHsmCmd44(char *vkIndex,char *vkValue, int lenOfVKValue, unsigned char *pinByPK,int lenOfPinByPK,char *hash);

/*
   ���ܣ���EDK��Կ�ӽ������ݣ�����ǽ���״̬�����������Ȩ�²��ܴ������򱨴�
   �������
   flag 0������
   edk LMK24-25����
   lenOfData ���������ֽ�����8�ı�������Χ��0008-4096
   indata �����ܻ���ܵ����ݣ���BCD���ʾ��
   �������
   outdata ������
 */
int UnionHsmCmd50(char flag,char *edk,int lenOfData,char *indata,char *outdata,int sizeOfOutData);


/*
   ���ܣ���RC4�㷨�������ݼ�/���ܡ�����RC4�㷨�����ԣ����ܺͽ��ܶ��ǵ���ͬһָ�
   ������
   desKeyType      ��Կ���ͣ�֧��edk��zek
   inData          �������ݣ�binary
   inLen           �������ݳ���
   outData         �������
   sizeOfOutData   outDataָ��Ļ������ĳ���
   ���أ�
   <0 ����
   >0 �������ݳ���
   add by zhangsb 20140627
 */
int UnionHsmCmdRD(TUnionDesKeyType desKeyType, char *keyValue, char *inData, int inLen, char *outData, int sizeOfOutData);



// ��ZMK���ܵ���Կת��ΪLMK���ܵ�ZAK/ZEK
int UnionHsmCmdFK(char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue);

/*
   ��������:
   TIָ���Կ����
   �������:
encryMode: 1H �㷨��ʶ 1���������� 2���������� 3����֤ǩ�� 4��������Կ 5��������Կ
pkLength: ���빫Կ����
pk: nB/1A+3H DER����Ĺ�Կ��Կ��HSM��ȫ�洢���ڵ�λ��
signLength: ����֤ǩ��ֵ���ȣ�encryMode = 3ʱ�������
sign: nB ����֤ǩ��ֵ��encryMode = 3ʱ�������
keyLength: 1H ����/������Կ���ȣ�0 = ��������Կ��1 = ˫������Կ��encryMode = 4 �� encryMode = 5ʱ�������
inputDataLength: �ӽ������ݳ���
inputData: nB/1A+3H ���ӽ������ݻ���ӽ�����Կ��HSM��ȫ�洢���ڵ�����
�������:
outputData: nB �ӽ���/�ӽ�����Կ����
errCode: ������
����ֵ:
>0	ouputData�ĳ���
<0	ʧ��
 */

int UnionHsmCmdTI(char encryMode, int pkLength, char *pk, int signLength,
		char *sign, char keyLength, int inputDataLength, char *inputData, char *outputData);

// ��һ��ZMK���ܵ���Կת��ΪLMK����,֧�ֹ����㷨SM1��SFF33
int UnionHsmCmdA6(char *algFlag, TUnionDesKeyType keyType,char *zmk,char *keyByZmk, char *keyByLmk,char *checkValue);

// ��������MAC��ָ��
int UnionHsmCmdMU(char msgNo,int keyType,TUnionDesKeyLength keyLength,char *key,char msgType,char *iv,int msgLen,char *msg,char *mac);

// ��ZMK���ܵ�ZPKת��ΪLMK����
int UnionHsmCmdFA(char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue);

// ����ZAK/ZEK
int UnionHsmCmdFI(char keyFlag,char *zmk,int variant,char keyLenFlag,char *keyByZmk,char *keyByLmk,char *checkValue);

int UnionHsmCmdA0(int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue);

int UnionHsmCmd0A();

int UnionHsmCmdRA();

int UnionHsmCmdPA(char *format);

int UnionHsmCmdA2(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component);

int UnionHsmCmdCC(int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,int maxPinLen,char *pinFormat1,char *pinBlockByZPK1, char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2, int *pinLen);

int UnionHsmCmdMS(char msgNo,int keyType,TUnionDesKeyLength keyLength,char *key,char msgType,char *iv,int msgLen,char *msg,char *mac);

/*
   �������ܣ�
   DGָ���PVK����PIN��PVV(PIN Verification Value)��
   ���õļ��ܱ�׼ΪVisa Method
   ���������
   pinLength��LMK���ܵ�PIN���ĳ���
   pin��LMK���ܵ�PIN����
   pvkLength��LMK���ܵ�PVK�Գ���
   pvk��LMK���ܵ�PVK��
   lenOfAccNo���ͻ��ʺų���
   accNo���ͻ��ʺ�
   ���������
   pvv��������PVV

 */
int UnionHsmCmdDG(int pinLength,char *pin,int pvkLength,char *pvk,int lenOfAccNo,char *accNo,char *pvv);

/*
   �������ܣ�
   CAָ���һ��TPK���ܵ�PINת��Ϊ��ZPK����
   ���������
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinFormat1��ת��ǰ��PIN��ʽ
   pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   pinFormat2��ת�����PIN��ʽ
   ���������
   pinBlockByZPK��ת������ZPK���ܵ�PIN����
 */
int UnionHsmCmdCA(int tpkLength,char *tpk,int zpkLength,char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK);

/*
   �������ܣ�
   JEָ���һ��ZPK���ܵ�PINת��Ϊ��LMK����
   ���������
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinFormat��PIN��ʽ
   pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK��ת������LMK���ܵ�PIN����

 */
int UnionHsmCmdJE(int zpkLength,char *zpk,char *pinFormat,char *pinBlockByZPK,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   �������ܣ�
   JCָ���һ��TPK���ܵ�PINת��Ϊ��LMK����
   ���������
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   pinFormat��PIN��ʽ
   pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK��ת������LMK���ܵ�PIN����

 */
int UnionHsmCmdJC(int tpkLength,char *tpk,char *pinFormat,char *pinBlockByTPK,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   �������ܣ�
   JGָ���һ��LMK���ܵ�PINת��Ϊ��ZPK����
   ���������
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinFormat��PIN��ʽ
   pinLength��LMK���ܵ�PIN���ĳ���
   pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByZPK��ת������LMK���ܵ�PIN����

 */
int UnionHsmCmdJG(int zpkLength,char *zpk,char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,int lenOfAccNo,char *pinBlockByZPK);

/*
   �������ܣ�
   JAָ��������һ��PIN
   ���������
   pinLength��Ҫ��������ɵ�PIN���ĵĳ���
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK�����������PIN�����ģ���LMK����

 */
int UnionHsmCmdJA(int pinLength,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   �������ܣ�
   EEָ���IBM��ʽ����һ��PIN
   ���������
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinOffset��PIN Offset������룬�Ҳ�'F'
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK�����������PIN�����ģ���LMK����

 */
int UnionHsmCmdEE(int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   �������ܣ�
   DEָ���IBM��ʽ����һ��PIN��Offset
   ���������
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinLength��LMK���ܵ�PIN���ĳ���
   pinBlockByLMK����LMK���ܵ�PIN����
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinOffset��PIN Offset������룬�Ҳ�'F'

 */
int UnionHsmCmdDE(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset);

/*
   �������ܣ�
   BAָ�ʹ�ñ�������Կ����һ��PIN����
   ���������
   pinCryptogramLen�����ܺ��PIN���ĵĳ���
   pinTextLength��PIN���ĵĳ���
   pinText��PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK��LMK���ܵ�PIN����

 */
int UnionHsmCmdBA(int pinCryptogramLen,int pinTextLength,char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   �������ܣ�
   NGָ�ʹ�ñ�������Կ����һ��PIN����
   ���������
   pinCryptogramLen��LMK���ܵ�PIN���ĵĳ���
   pinCryptogram��LMK���ܵ�PIN����
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   referenceNumber����LMK18-19�����ʺŵõ���ƫ��ֵ
   pinText��PIN����

 */
int UnionHsmCmdNG(int pinCryptogramLen,char *pinCryptogram,char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText);

/*
   �������ܣ�
   DAָ���IBM��ʽ��֤�ն˵�PIN
   ���������
   pinBlockByTPK��TPK���ܵ�PIN����
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinOffset��IBM Offset������룬�Ҳ�'F'
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdDA(char *pinBlockByTPK,int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   �������ܣ�
   EAָ���IBM��ʽ��֤�������ĵ�PIN
   ���������
   pinBlockByZPK��ZPK���ܵ�PIN����
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinOffset��IBM Offset������룬�Ҳ�'F'
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pvkLength��LMK���ܵ�TPK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdEA(char *pinBlockByZPK,int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   �������ܣ�
   DCָ���VISA��ʽ��֤�ն˵�PIN
   ���������
   pinBlockByTPK��TPK���ܵ�PIN����
   pvv���ն�PIN��4λVISA PVV
   tpkLength��LMK���ܵ�TPK����
   tpk��LMK���ܵ�TPK
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdDC(char *pinBlockByTPK,char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   �������ܣ�
   ECָ���VISA��ʽ��֤�������ĵ�PIN
   ���������
   pinBlockByZPK��ZPK���ܵ�PIN����
   pvv���ն�PIN��4λVISA PVV
   zpkLength��LMK���ܵ�ZPK
   zpk��LMK���ܵ�ZPK
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������

 */
int UnionHsmCmdEC(char *pinBlockByZPK,char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   �������ܣ�
   BCָ��ñȽϷ�ʽ��֤�ն˵�PIN
   ���������
   pinBlockByTPK��TPK���ܵ�PIN����
   tpkLength��LMK���ܵ�TPK
   tpk��LMK���ܵ�TPK
   pinByLMKLength������PIN���ĳ���
   pinByLMK������PIN���ģ���LMK02-03����
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
 */
int UnionHsmCmdBC(char *pinBlockByTPK,int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   �������ܣ�
   BEָ��ñȽϷ�ʽ��֤�������ĵ�PIN
   ���������
   pinBlockByZPK��ZPK���ܵ�PIN����
   zpkLength��LMK���ܵ�ZPK����
   zpk��LMK���ܵ�ZPK
   pinByLMKLength������PIN���ĳ���
   pinByLMK������PIN���ģ���LMK02-03����
   pinFormat��PIN��ʽ
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
 */
int UnionHsmCmdBE(char *pinBlockByZPK,int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   �������ܣ�
   CWָ�����VISA��У��ֵCVV
   ���������
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   cvv�����ɵ�VISA����CVV
 */
int UnionHsmCmdCW(char *cardValidDate,int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *cvv);

/*
   �������ܣ�
   CYָ���֤VISA����CVV
   ���������
   cvv������֤��VISA����CVV
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
 */
int UnionHsmCmdCY(char *cvv,char *cardValidDate,int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode);

int UnionHsmCmdBU(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue);

/*
   �������ܣ�
   A4ָ��ü�����Կ�����ĳɷֺϳ�һ����Կ��������У��ֵ
   ���������
   keyType����Կ������
   keyLength����Կ�ĳ���
   partKeyNum����Կ�ɷֵ�����
   partKey����Ÿ�����Կ�ɷֵ����黺�壬ΪLMK���ܵ���Կ����
   ���������
   keyByLMK���ϳɵ���Կ���ģ���LMK����
   checkValue���ϳɵ���Կ��У��ֵ
 */
int UnionHsmCmdA4(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue);

/*
   ����    ��Դ�ʺ����ɵ�PINBLOCKת��Ϊ��Ŀ���ʺ����ɵ�PINBLOCK������ָ������Կ���ܱ���
   ˵��    ������Կ����ȷ���㷨
   �������
   zpk �� LMK 06-07 ���ܵ�ZPK
   pvk �� LMK 14-15���ܵ�ZPK
   maxPinLen ���PIN����
   decimalizationTable ʮ��������
   pinValidationData 
   pinBlock1 ��ZPK����ANSI X9.8��׼����
   accNo1 Դ�˺�
   accNo2 Ŀ���˺�
 */
int UnionHsmCmdDF(char *zpk,char *pvk,int maxPinLen,char *decimalizationTable,char *pinValidationData,char *pinBlock1,char *accNo1,char *accNo2,char *pinBlock2,char *pinOffset);

int UnionHsmCmdA8(TUnionDesKeyType keyType,char *key,char *zmk,char *keyByZMK,char *checkValue);

/*
   �������ܣ�
   33ָ�˽Կ����
   ���������
   flag���ù�Կ����ʱ�����õ���䷽ʽ��
   '0'��������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
   '1'��PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   vkIndex��˽Կ������"00"��"20"
   lenOfVK��˽Կ����
   vk��˽Կ����
   cipherDataLen���������ݵ��ֽ���
   cipherData�����ڽ��ܵ���������
   sizeOfPlainData��plainData���ڵĴ洢�ռ��С
   ���������
   plainData�����ܵõ�����������
 */
int UnionHsmCmd33(char flag,char *vkIndex,int lenOfVK,char *vk,int cipherDataLen,char *cipherData,char *plainData,int sizeOfPlainData);

/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   pvkIndex��      ָ����˽Կ�����ڽ���PIN��������
   fillMode��      ��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
   ��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   ZPK��	   ���ڼ���PIN����Կ��
   accNo��	 �û����ʺ�
lenOfPinBlock:  ��Կ���ܵ�PIN���ĵĳ���
pinBlock��      ����Կ���ܵ�PIN��������
�������
lenOfPin��      ��Կ����
pinBlock1��     ����ZPK�����µ�PIN����
pinBlock2��     ����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
lenOfUniqueID�� 01-20
UniqueID��      ���صľ�ASCII��չ��ID������
 */
int UnionHsmCmdH2 (int pvkIndex, char fillMode, char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID);

/*
����
        ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
   srcAlgorithmID  �ǶԳ���Կ�㷨��ʶ��0:�����㷨,  1:�����㷨
   pvkIndex��   ָ����˽Կ�����ڽ���PIN��������
   lenOfVK      ˽Կ����
   vkValue      ˽Կֵ
   fillMode��   ��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
                ��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   dstAlgorithmID  ZPK��Կ�㷨��ʶ,0:�����㷨,  1:�����㷨
   ZPK��        ���ڼ���PIN����Կ��
   accNo��      �û����ʺ�
   lenOfPinBlock:  ��Կ���ܵ�PIN���ĵĳ���
   pinBlock��      ����Կ���ܵ�PIN��������
�������
   lenOfPin��      ��Կ����
   pinBlock1��     ����ZPK�����µ�PIN����
   pinBlock2��     ����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
   lenOfUniqueID�� 01-20
   UniqueID��      ���صľ�ASCII��չ��ID������

 */
int UnionHsmCmdN6(int srcAlgorithmID,int pvkIndex, int lenOfVK,char *vkValue,char fillMode, int dstAlgorithmID,char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID);

/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   OriKeyType��    ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
   OriKey��	ԴKEY�����ڽ���PIN����Կ��
   lenOfAcc��      �ʺų��ȣ�4~20��
   accNo��	 �û����ʺš�
   DesKeyType��    ������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
   DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
   pinBlock��      ����Կ���ܵ�PIN�������ġ�
   �������
   lenOfPinBlock1�����볤�ȡ�
   pinBlock1��     ����ZPK2/PVK�����µ�PIN���ġ�
 */
int UnionHsmCmdH5(char *OriKey, int lenOfAcc, char *AccNo, char DesKeyType, char *DesKey, char *pinBlock, char *lenOfPinBlock1,char *pinBlock1);

/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   OriKeyType��    ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
   OriKey��	ԴKEY�����ڽ���PIN����Կ��
   lenOfAcc��      �ʺų��ȣ�4~20��
   accNo��	 �û����ʺš�
   DesKeyType��    ������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
   DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
   pinBlock��      ����Կ���ܵ�PIN�������ġ�
   �������
   lenOfPinBlock1�����볤�ȡ�
   pinBlock1��     ����ZPK2/PVK�����µ�PIN���ġ�
 */
int UnionHsmCmdH7(char *OriKey, int lenOfAcc,char *AccNo, char *DesKey, char *pinBlock, char *lenOfPinBlock1,char *pinBlock1);

/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   ZPK��	   ���ڽ���PIN����Կ��
   lenOfAcc��      �ʺų��ȣ�4~20��
   accNo��	 �û����ʺš�
   pinBlockByZPK�� ��ZPK���ܵ�PIN�������ġ�
   PVK��	   PVK��
   pinBlockByPVK�� ��PVK���ܵ�PIN�������ġ�
   �������
 */
int UnionHsmCmdH9(char *ZPK, int lenOfAcc, char *AccNo, char *pinBlockByZPK, char *PVK, char *pinBlockByPVK);

/*
   ����
   ��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
   �������
   ZPK��	   ���ڼ���PIN����Կ��
   accNo��	 �û����ʺš�
   pinBlockByZPK�� ��ZPK���ܵ�PIN�������ġ�
   dataOfZAK��     ����ZAK��Կ������֮һ��
   �������
   lenOfPin��      ���볤��
   ZAK��	   ����LMK26-27�ԶԼ����µ�ZAK��Կ��
 */
int UnionHsmCmdH4 (char *ZPK, char *AccNo, char *pinBlockByZPK, char *dataOfZAK, char *lenOfPin, char *ZAK);

/*
   ���ܣ���PinOffsetת��Ϊר���㷨��FINSE�㷨�����ܵ�����
   ������� 
   PVK LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset
   pinOffSet Offset��ֵ�����������ķ�ʽ���ұ�����ַ���F��
   minPinLen ��С��PIN����
   accNo �˺�
   decimalizationTable ʮ����ת����
   pinValidationData PINУ������
   �������
   pinBlock PIN����
 */
int UnionHsmCmdS1(char *PVK, char *pinOffSet,int minPinLen,char *accNo,char *decimalizationTable,char *pinValidationData, char *pinBlock);

/*
   ���ܣ���PinBlockת��Ϊר���㷨��FINSE�㷨�����ܵ�����
   �������
   ZPK LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset
   minPinLen ��С��PIN����
   accNo �˺�
   OriPinBlock ԴPIN��
   �������
   pinBlock PIN���� 
 */
int UnionHsmCmdS2(char *ZPK, int minPinLen,char *accNo,char *OriPinBlock, char *pinBlock);

/*
   ���ܣ�������ɢMAK����MAC
   �������
   ZAK ��LMK26-27�Լ���
   scatData ���ڼ�����ɢ��Կ������
   lenOfmac MAC���ݳ���
   macData MAC����
   �������
   mac ���ص�MACֵ
 */
int UnionHsmCmdG1(char *ZAK, char *scatData,char *lenOfmac,char *macData, char *mac);

/*
   ���������ָ��
 */
int UnionHsmCmdY3(int min,int max,int Cnt,int isRepeat,char *RandNo);

/*һ���Է��ͼ��ܻ����ݿ���󳤶�*/
int UnionHsmCmdE0(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

int UnionHsmCmdE0_f(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

/* Y4ָ��,����:������Կ.ʹ�ø���Կ����ɢ����1������ɢ,�õ���ɢ��Կ;ʹ�ñ�����Կ(ZMK)���������LMK06-07�Լ������.
   �������:
   kek��������ԿKEK
   rootKey��Ӧ������Կ
   keyType����Կ���ͣ�001-ZPK��008-ZAK��
   discreteNum����ɢ����
   discreteData1����ɢ����1
   discreteData2����ɢ����2
   discreteData3����ɢ����3
   �������:
   keyByKek��ʹ��KEK���ܵ�key
checkValue:У��ֵ
 */
int UnionHsmCmdY4 (char *kek, char *rootKey, char *keyType, int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char *keyByKek, char *keyByLmk, char *checkValue);

/*
   �������ܣ�
   A5ָ�������Կ���ĵļ����ɷݺϳ����յ���Կ��ͬʱ����Ӧ��LMK��Կ�Լ���
   ���������
   keyType����Կ������
   keyLength����Կ�ĳ���
   partKeyNum����Կ�ɷֵ�����
   partKey����Ÿ�����Կ�ɷֵ����黺��
   ���������
   keyByLMK���ϳɵ���Կ���ģ���LMK����
   checkValue���ϳɵ���Կ��У��ֵ
 */
int UnionHsmCmdA5(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue);

/*
   �������ܣ�
   U2ָ�ʹ��ָ����Ӧ������Կ����2����ɢ�õ���ƬӦ������Կ��
   ʹ��ָ��������Կ����Կ���м��ܱ������������MAC���㡣
   ���������
securityMech: ��ȫ����(S����DES���ܺ�MAC, T����DES���ܺ�MAC)
mode: ģʽ��־, 0-������ 1-���ܲ�����MAC
id: ����ID, 0=M/Chip4(CBCģʽ��ǿ�����X80) 1=VISA/PBOC(������ָ����ECB) 2=PBOC1.0ģʽ(ECBģʽ������������)

mk: ����Կ
mkType: ����Կ����109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
��ʹ�ü��ܻ�����Կʱ����������
mkIndex: ����Կ����, K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
mkDvsNum: ����Կ��ɢ����, 1-3��
mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

pkType: ������Կ���� 0=TK(������Կ)1=DK-SMC(ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ)
pk: ������Կ
pkIndex: ������Կ����
pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������

proKeyFlag:������Կ��ʶ��Y:���������Կ��N:�����������Կ����ѡ��:��û�и���ʱȱʡΪN
proFactor: ��������(16H),��ѡ��:����������Կ��־ΪYʱ��

ivCbc: IV-CBC,8H ����������ID��Ϊ0ʱ��

encryptFillDataLen: ����������ݳ���(4H),����������ID��Ϊ2ʱ�У�������1024��
����Կ����һ����м��ܵ����ݳ���
encryptFillData: ����������� nB ����������ID��Ϊ2ʱ��,����Կ����һ����м���
encryptFillOffset: �����������ƫ���� 4H ����������ID��Ϊ2ʱ��
����Կ���Ĳ��뵽����������ݵ�λ��, ��ֵ������0������������ݳ���֮��

ivMac: IV-MAC,16H ������ģʽ��־��Ϊ1ʱ��
macDataLen: MAC������ݳ��� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC��������ݳ���
macData: MAC������� nB ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC���������
macOffset: ƫ���� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ���Ĳ��뵽MAC������ݵ�λ��
��ֵ������0��MAC������ݳ���֮��
���������
mac: MACֵ 8B ������ģʽ��־��Ϊ1ʱ��
criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
criperData: nB �������������
checkValue: У��ֵ

 */
int UnionHsmCmdU2(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex, int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex, int pkDvsNum, char *pkDvsData, char *proKeyFlag, char *proFactor, char *ivCbc, int encryptFillDataLen, char *encryptFillData, int encryptFillOffset, char *ivMac, int macDataLen, char *macData, int macOffset, char *mac, int *criperDataLen, char *criperData, char *checkValue);

int UnionHsmCmdKW(char *mode, char *id, int mkIndex, char *mk, char *iv,int divNum, char *divData, int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,char *data1,char *ARPC);

// added 2011-11-08 �������ã���09KWָ����ͬ
int UnionHsmCmdKX(char *mode, char *id, int mkIndex, char *mk, char *iv,int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,char *data1,char *ARPC);

int UnionHsmCmdU0(char *mode, char *id, int mkIndex, char *mk, char *iv,char *pan, char *bh, char *atc, int lenOfPlainData,char *plainData, int *lenOfCiperData, char *ciperData);

/*
   �������ܣ�
   UGָ�����ת��RSA˽Կ��LMK��KEK,LMK������RSA˽ԿΪREF�ṹ,KEKʹ��Mode������㷨����REF�ṹ��RSA˽Կ��ÿ��Ԫ�ء�
   ���������
mode: ����˽Կÿ��Ԫ�صļ����㷨 "00"-DES_ECB, "01"-DES_ECB_LP, "02"-DES_ECB_P,
"10"-DES_CBC, "11"-DES_CBC_LP, "12"-DES_CBC_P
keyType����Կ����
key: ����Կ����HSM�ڰ�ȫ����ʧ�洢������Կ�Լ���LMK���ܵ���������
iv: ��ʼ����, modeΪCBC����ʱ��������ڡ�
vkLength: ˽Կ���ݵ����ݳ���
vk: nB ��LMK������RSA˽Կ��REF�ṹ��
���������
vkByKey: key���ܵ�vk���ݴ�
 */
int UnionHsmCmdUG(char *mode, TUnionDesKeyType keyType,char *key, char *iv,int vkLength, char *vk, char *vkByKey);

/*
   ��������:
   TKָ�˽Կ����
   �������:
encryMode: 1H �㷨��ʶ 1���������� 2���������� 3��ǩ�� 4��������Կ 5��������Կ
vkLength: ����˽Կ����
vk: nB/1A+3H DER�����˽Կ��˽Կ��HSM��ȫ�洢���ڵ�����
keyLength: 1H ����/������Կ���ȣ�0 = ��������Կ��1 = ˫������Կ��encryMode = 4 �� encryMode = 5ʱ�������
inputDataLength: �ӽ������ݳ���
inputData: nB/1A+3H ���ӽ������ݻ���ӽ�����Կ��HSM��ȫ�洢���ڵ�����
�������:
outputData: nB �ӽ���/�ӽ�����Կ����
 */
int UnionHsmCmdTK(char encryMode, int vkLength, char *vk, char keyLength,int inputDataLength, char *inputData, char *outputData);

/*
   ��������:
   ���������
   �������:
   lenOfRandomData ���������
   �������:
   randomData �����
 */
int UnionHsmCmdTE(int lenOfRandomData, char *randomData);

/*
   ��������:
   ������ɢ/������Կ
   �������:
   mode �㷨��־:
   "00" - EMV2000
   "01" - DES_CBC
   "02" - DES_ECB
   "10" - VISA
   "11" - EMV2000
   masterKeyType	������Կ����
   masterKey	������Կ
   desKeyType	����Կ����
   keyLengthFlag	����ɢ����Կ����	
   0 = ��������Կ
   1 = ˫������Կ
   data		��ɢ����
   masterKey	������Կ
   iv		����
   �������:
   key	����Կ����

 */

int UnionHsmCmdX1(char *mode, TUnionDesKeyType masterKeyType, char *masterKey,
		TUnionDesKeyType desKeyType, char *keyLengthFlag,char *data, char *iv, char *key);

/*
   ��������:
   ��һ����Կ���ܵ�����ת��Ϊ��һ����Կ����
   �������:
   srcKeyType			      Դ��Կ������
   srcKey	  1A+3H/16H/32H/48H       Դ��Կ
   srcModel				Դ�����㷨
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4��CBCģʽ��ǿ�����X80��
   "21"=VISA/PBOC��������ָ����ECB��
   dstKeyType			      Ŀ����Կ����
   dstKey	  1A+3H/16H/32H/48H       Ŀ����Կ
   dstModel				Ŀ�ļ����㷨
   "00"=DES_ECB
   "02"=DES_ECB_P
   lenOfSrcCiphertext		      Դ��Կ���ܵ��������ݳ���
   srcCiphertext   nB		      Դ��Կ���ܵ���������
   iv_cbc	  16H		     ��ʼ������Դ�����㷨ΪCBC����ʱ���������
   �������:
   dstCiphertext   nB		      Ŀ����Կ���ܵ���������
 */
int UnionHsmCmdY1(TUnionDesKeyType srcKeyType,char *srcKey,int srcKeyIdx,char *srcModel,TUnionDesKeyType dstKeyType,char *dstKey,int dstKeyIdx,char *dstModel,int lenOfSrcCiphertext,char *srcCiphertext,char *iv_cbc,char *dstCiphertext);

/*
   �������ܣ�
   UDָ������ն˿����������Ա����Ȩ״̬�²�����ʵ�ֶ���Կ�Ļָ��򱸷ݹ��ܡ�
   ���������
   algorithmFlag��	[1A]	�㷨��ʶ����D��-des�㷨����S��-SM4�㷨��Ĭ��Ϊ'D��
modeFlag:       [1N]		    ģʽ��ʶ��
1-�ָ���Կ 2-������Կ
mkIndex:	[1A+3H] ��Կλ�ã�      ����������Ƭ��Կ�ķ���������Կ��
ʹ��LMK��28-29��Ӧ���ּ��ܡ�
1A+3H��ʾʹ��K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ��
����ʱ��ʵ��1A+3Hȡ��Կ��
keyLocale       [32H/1A+32H]
key:	    [32H]		   ��Կ
����ģʽ��־Ϊ1ʱ�У�LMK04_05����


���������
encryKey:       [N*8B/N*16B/N*24B]      LMK�Լ��ܵ���Կ����(lmk04_05����)
encryKeyLen:			    ��Կ����
checkValue:     [16H]		   ����Կ����0�Ľ��
 */
int UnionHsmCmdUD(char *algorithmFlag,char *modeFlag,char *mkIndex,char *keyLocale,char *key,char *encryKey,int *pencryKeyLen,char *checkValue);

/*
   �������
   mode ����ģʽ��ʶ 0=��ɢ��Կ3DES����
   1=��ɢ��Կ3DES����
   2=��1DES_MAC����Կ����8�ֽڣ�(ANSI X9.19 MAC)
   3=��3DES_MAC(ANSI X9.19 MAC)
   4=��ɢ��ԿDES����
   5=��ɢ��ԿDES����
   6=������ԿDES����
   7=������ԿDES����
   id ����ID �ӽ����㷨ģʽ��
   01=ECB
   02=CBC
   03=CFB
   04=OFB
   mkType ����Կ���ͣ�������Ƭ��Կ�ķ�������Կ���ͣ�
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   lenOfData ���ݳ���
   data ����
   �������
   criperDataLen ���ݳ���  
   criperData ����   
 */
int UnionHsmCmdU1(char *mode, char *id, char *mkType, char *mk, int mkIndex,int mkDvsNum, char *mkDvsData, char *proFactor, int lenOfData,char *data, int *criperDataLen, char *criperData);

/*
   �������
   mode ģʽ��ʶ 1=����MAC
   2=У��MAC
   1=����C-MAC
   id ����ID
   0=3DESMAC��ʹ������Կ����3DESMAC��
   1=DESMAC��ʹ�ù�����Կ����DESMAC��
   2=TAC(ʹ������Կ����DESMAC)
   3=3DESMAC��ʹ��2����������Կ����3DESMAC��
   mkType ����Կ���� 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   macFillFlagMAC ��������ʶ
   1=ǿ�����0X80
   0=��ǿ�����0X80
   ivMac IV-MAC 
   lenOfData MAC�������ݳ���
   data MAC��������
   macFlagMAC ���ȱ�ʶ   
   1=4 BYTE
   2=8 BYTE
   �������
   checkMac
   mac MACֵ
 */
int UnionHsmCmdUB(char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,char *data, char *macFlag, char *checkMac, char *mac);

/*
   �������
   mode ģʽ��ʶ 
   2=У��MAC
   1=����C-MAC
   id ����ID
   0=3DESMAC��ʹ������Կ����3DESMAC��
   1=DESMAC��ʹ�ù�����Կ����DESMAC��
   2=TAC(ʹ������Կ����DESMAC)
   mk ����Կ
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor �������� 
   macFillFlag MAC��������ʶ
   1=ǿ�����0X80
   0=��ǿ�����0X80
   ivMac IV-MAC 
   lenOfData MAC�������ݳ���
   data MAC��������
   macFlag MAC���ȱ�ʶ
   1=4 BYTE
   2=8 BYTE
   �������
   checkMac
   mac MACֵ
 */
int UnionHsmCmdU3(char *mode, char *id, char *mk, int mkIndex,int mkDvsNum,char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,char *data, char *macFlag, char *checkMac, char *mac);

/*
   �������
   mode ģʽ��־
   1-���ܲ�������ԿУ��ֵ(checkvalue)
   id ����ID
   0=DES_ECB(ֱ�Ӽ��ܣ��������)
   1=DES_CBC(ֱ�Ӽ��ܣ��������)
   mkType ����Կ����
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   pkType ������Կ����
   0=TK(ZEK)��������Կ��
   1=ZMK
   pk ������Կ 
   ivCbc IV-CBC������������ID��Ϊ1ʱ��
   �������
   keyCheckValue ��ԿУ��ֵ
   keyValue ��������
 */
int UnionHsmCmdUK(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex,int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex,char *ivCbc,char *keyValue, char *keyCheckValue);

/*
   ���ܣ�����ȫ���ı�������Կ���뵽���ܻ��У�EMV 4.1/PBOC��
   �������
   mode ģʽ��־ 
   1-��֤MAC������
   id ����ID 
   0=M/Chip4��CBCģʽ��ǿ�����X80��
   1=VISA/PBOC��������ָ����ECB��
   mkFlag �洢��־ 
   1-����Կ�洢�����ܻ���
   mkIndex �洢����
   mkLengthFlag ��Կ������ָ��������Կ���ͣ���ȷ�����ܵ�LMK��
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN

   pkType ������Կ����
   0=TK��������Կ��
   1=DK-SMC��ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ��
   pk ������Կ 
   pkDvsNum ������Կ��ɢ����
   pkDvsData ������Կ��ɢ���� 
   ivCbc IV-CBC
   criperDataLen ���ĳ��� 
   criperData ��������
   mac MAC
   ivMac IV-MAC 
   macDataLen MAC������ݳ���
   macData MAC������� 
   macOffset ƫ����
   �������
   keyValue LMK��������Կ��ԿУ��ֵ
   checkValue ��ԿУ��ֵ
 */
int UnionHsmCmdU4(char *mode,char *id,char *mkFlag,int mkIndex,char *mkType,char *mkLengthFlag,char *pkType,char *pk,int pkIndex,int pkDvsNum,char *pkDvsData,char *proKeyFlag,char *ivCbc,int criperDataLen,char *criperData,char *mac,char *ivMac,int macDataLen,char *macData, int macOffset,char *keyValue,char *checkValue);

/*
   ����
   �ӽ�������
   ���������
   encryFlag��1�����ܣ�2������
   mode��01��DES_ECB��02��DES_CBC
   keyType����Կ���ͣ�000��ZMK��00A��ZAK
   keyLen����Կ���ȣ�0 = ��������Կ��1 = ˫������Կ
   key���ӽ�����Կ
   iv��CBCģʽʱʹ�õĳ�ʼ������
   lenOfData����Ҫ��ժҪ�����ݳ���
   pData����Ҫ��ժҪ������
   �������
   pCipherDataLen���������ݳ���
   pCipherData����������
 */
int UnionHsmCmdTG(char encryFlag,char *mode,char *keyType,char keyLen,char *key,char *iv,int lenOfData,char *pData,int *pCipherDataLen,char *pCipherData);

/*
   ��������:
   ��������keyA����ת����ΪkeyB����
   �������:
   srcKeyType			      Դ��Կ������
   srcKey	  1A+3H/16H/32H/48H       Դ��Կ
   srcModel				Դ�����㷨
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4��CBCģʽ��ǿ�����X80��
   "21"=VISA/PBOC��������ָ����ECB��
   srcIV	   16H		     ��ʼ������Դ�����㷨ΪCBC����ʱ���������
   dstKeyType			      Ŀ����Կ����
   dstKey	  1A+3H/16H/32H/48H       Ŀ����Կ
   dstModel				Ŀ�ļ����㷨
   "00"=DES_ECB
   "02"=DES_ECB_P
   dstIV	   16H		     ��ʼ������Ŀ�ļ����㷨ΪCBC����ʱ���������
   lenOfSrcCiphertext		      Դ��Կ���ܵ��������ݳ���
   srcCiphertext   nH		      Դ��Կ���ܵ���������

   �������:
   dstCiphertext   nH		      Ŀ����Կ���ܵ���������
 */
int UnionHsmCmdUE(char *srcKeyType,char *srcKey,int srcKeyIdx,char *srcModel,char *srcIV,char *dstKeyType,char *dstKey,int dstKeyIdx,char *dstModel,char *dstIV,int lenOfSrcCiphertext,char *srcCiphertext,char *dstCiphertext);

/*
   ����
   ����Կ��KEK����תΪLMK����
   ���������
   mode���㷨��ʶ
   00��DES_ECB
   01��DES_ECB_LP
   02��DES_ECB_P
   10��DES_CBC
   11��DES_CBC_LP
   12��DES_CBC_P
   kekType����Կ���ͣ�000��ZMK��
kekLen:��Կ���ȣ�0 = ��������Կ��1 = ˫������Կ
kek����Կ������Կ
iv��CBCģʽʱʹ�õĳ�ʼ������
keyType����Կ���ͣ�000��ZMK��
lenOfKeyCipherTextByKek����Ҫ��ժҪ�����ݳ���
keyCipherTextByKek����Ҫ��ժҪ������
�������
keyCipherTextByLmk����������
 */
int UnionHsmCmdTU(char *mode,char *kekType,char kekLen,char *kek,char *iv,char *keyType,char keyLen,int lenOfKeyCipherTextByKek,char *keyCipherTextByKek,int *lenOfKeyCipherTextByLmk,char *keyCipherTextByLmk);

int UnionHsmCmdVY(char *k1, int lenOfK1, char *k2, int lenOfK2,int isDis, char *disFac, int lenOfDisFac, int pinMaxLen, char *pinByZPK1, int lenOfPinByZPK1, char *pinFormatZPK1, int lenOfPinFormatZPK1, char *pinFormatMDKENC, int lenOfPinFormatMDKENC, char *accNo1, int lenOfAccNo1, char *accNo2, int lenOfAccNo2,char *pinSec, char *pinFormat, char *isWeaKey);

int UnionHsmCmdPE(char *pinByLmk,char *accNo,int fldNum,char fldGrp[][128+1],char *retValue,int sizeOfBuf);

/* �������
   algorithmID	�㷨��ʶ��0:ECB	1:CBC
   keyType		��Կ����
   zek		������Կ
   iv		iv
   keyByZek	��Կ����
   keyFlag		��Կ����X/U
   sizeOfBuf	����keyByLmk����Ļ����С
   �������
   keyByLmk	��Կ����
   checkValue	����ֵ
 */
int UnionHsmCmdM8(char *algorithmID,TUnionDesKeyType keyType,char *zek,char *iv,char *keyByZek,char *keyFlag,char *keyByLmk,char *checkValue,int sizeOfBuf);

// ������ԿSM4��Կ
/* �������
mode:
0��������Կ
1��������Կ����ZMK�¼���
keyType		��Կ����
zmk			����ģʽΪ1ʱ����ʾ��ZMK��
�������
keyByLMK		LMK��������Կ����ֵ
keyByZMK		ZMK��������Կ����ֵ
checkValue		��ԿУ��ֵ
 */
int UnionHsmCmdWI(char *mode, TUnionDesKeyType keyType, char *zmk, char *keyByLMK,char *keyByZMK, char *checkValue);

//����SM4��Կ����ӡһ���ɷ� 
/* �������
   keyType         ��Կ����
   fldNumi         ��ӡ�����
   fld             ��ӡ��
   �������
   component       �ɷ�
 */
int UnionHsmCmdWJ(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component);


/*
   ��������:
   WF::  ��һ����Կ���ܵ�����ת��Ϊ��һ����Կ����
   �������:
   algType      �㷨ת������
01:3DES�㷨תSM4
02:SM4�㷨ת3DES
03:SM4�㷨תSM4
srcKeyType             Դ��Կ������
srcKey                 Դ��Կ
srcKeyCheckValue       Դ��ԿУ��ֵ
srcModel               Դ�����㷨
"00"=ECB
"01"=ECB_LP
"02"=ECB_P
"10"=CBC
"11"=CBC_LP
"12"=CBC_P
dstKeyType                            Ŀ����Կ����
dstKey    1A+3H/16H/32H/48H             Ŀ����Կ
dstKeyCheckValue                        Ŀ����ԿУ��ֵ 
dstModel                                Ŀ�ļ����㷨
"00"=ECB
"02"=ECB_P
"10"=CBC
"12"=ECB_P
lenOfSrcCiphertext                    Դ��Կ���ܵ��������ݳ���
srcCiphertext   nB                    Դ��Կ���ܵ���������
iv_cbc    16H                ��ʼ������Դ�����㷨ΪCBC����ʱ���������
iv_cbcDstKey 16H                     Ŀ�ļ����㷨λcbc�����ʱ��������?

�������:
dstCiphertext   nB                    Ŀ����Կ���ܵ���������
����ֵ��
�ɹ���>=0  

 */
int UnionHsmCmdWF(char *algType, TUnionDesKeyType srcKeyType,char *srcKey, char* srcKeyCheckValue, int srcKeyIdx,char *srcModel, TUnionDesKeyType dstKeyType,char *dstKey, char* dstKeyCheckValue, int dstKeyIdx,char *dstModel,int lenOfSrcCiphertext,char *srcCiphertext, char *iv_cbc, char *iv_cbcDstKey, char *dstCiphertext);

/*
   RQCУ��
   1=ִ��ARQCУ���ARPC����
   2=����ִ��ARPC����
   id        ����ID   0=PBOC3.0(����SM4�㷨����)
   mkIndex           mk���������mkֵΪ����ʹ��������ʽ
   mk                ���ܼ�������Կ
   mkCheckValue      ���ܼ�������ԿУ��ֵ
   pan               ��ɢ��Ƭ��Կʹ�õ��ʺŻ����ʺ����к�
   atc               ʹ��ATC�����������Կ
   lenOfData         �������ݳ���
   data             Ҫ���ܵ�����
   ARQC             ARQC
   ARC              ARC
   �������:
   ARPC             ARPC
   ����ֵ��
   �ɹ���>=0
 */
int UnionHsmCmdWG(char *mode, char *id, int mkIndex, char *mk, char* mkCheckValue, int lenOfPan, char *pan, char *atc, int lenOfData, char *data, char *ARQC, char *ARC, char *ARPC);

/*
   ���ܣ� 
   RSA˽Կ��������תZPK��������
   ����:   
   vkeyLen	˽Կ���� ˽Կ�洢�ڼ��ܻ��ڲ�ʱ����˽Կ����ȡ0
   vkey	˽Կֻ��Ϊ�ⲿ�������ΪHSM�ڰ�ȫ�洢������Կ��
   dataPaddingMode 	?00=����� 01=PKCS#1 v1.5 02=OAEP
   dataLength  �ӽ������ݳ���	
   data �ӽ�������
   keyType	ZMKTYPE=0;ZPKTYPE=1;TPKTYPE=2 
   Key  �ö�Ӧ��LMK��Կ�Խ���
   Specialalgorithm	ר���㷨��ʶ
   ���:
   pin ����
 */
int UnionHsmCmd70(int vkeyLen, char *vkey, int vkIndex, int dataPaddingMode, int dataLength, char* data,  int keyType, char* key,char* Specialalgorithm, char *pin);

/*
   ���ܣ�
   ��zpk���ܵ��ַ���������ת��Ϊ��һzpk���ܵ��ַ���������
   ����:
   srcAlgorithmID Դ��Կ�㷨��ʶ��0�������,1�������
   srcZPK ��ǰ����PIN���ZPK��LMK�ԣ�06-07���¼���
   dstAlgorithmID Ŀ����Կ�㷨��ʶ��0�������,1�������
   dstZPK ��Ҫ����PIN���ZPK��LMK�ԣ�06-07���¼��ܡ�
   srcPin ԴZPK���ܵ��ַ���������
   srcFormat ԴPIN��ʽ  ,01-ANSI9.8
                         09-ũ���и�ʽ
   dstFormat Ŀ��PIN��ʽ,01-ANSI9.8
                         09-ũ���и�ʽ
   srcPan Դ12λ��Ч���˺�
   dstPan Ŀ��12λ ��Ч���˺�
   ���:
   pin ����PIN���� 
 */
int UnionHsmCmdH8(int srcAlgorithmID,char* srcZPK,int dstAlgorithmID, char* dstZPK, char *srcPin,char *srcFormat,char *dstFormat, char* srcPan, char* dstPan, int* lenOfPin, char* pin);

/*
   ���ܣ�
   SM1���ݼӽ���
   ����:
   algorithmID ���㷽ʽ 0��SM1 ECB����,1��SM1 ECB����, 2��SM1 CBC����, 3��SM1 CBC����.
   keyType ��Կ����
   keyValue ������Կ
   iv ����algorithmID����2��3ʱ�д���
   inData ���������� (algorithmID='1', '3' bcdhex)
   dataLen ���������ݳ��ȣ�����Ϊ16�ı���
   sizefEncryptDaga ����Ѵ���ݻ�������С 
   ���:
   encryptData �Ѵ������݄1�7(algorithmID='0','2' bcdhex)
   ��ֵ��
   �ɹ���>=0 Ϊ�Ѽ������ݳ���
   ʧ��: < 0
 */
int UnionHsmCmdM1(char *algorithmID,TUnionDesKeyType keyType, char *zek, char *iv, char *inData, int dataLen, char * encryptData, int sizefEncryptDaga);

/*
   ���ܣ��R8/R9���������

   �������: 
   genKeyLen 	ָ����������ĳ���
   genKeyType ����ĸ��Ӷ�		1�������� 2������ĸ 3������+��ĸ

   ���������
   outPutKey  �����Ŀ��� 

   ����ֵ��
   �����Ŀ���ĳ���
 */

int UnionHsmCmdR8(int genKeyLen,  char genKeyType, char* outPutkey);

// ��һ��ZMK���ܵ���Կת��ΪLMK����,SM4�㷨
int UnionHsmCmdSV(TUnionDesKeyType keyType, char *zmk, char *keyByZmk, char *keyByLmk, char *checkValue);
// ��һ��LMK���ܵ���Կת��ΪZMK����,SM4�㷨
int UnionHsmCmdST(TUnionDesKeyType keyType, char *key, char *zmk, char *keyByZMK, char *checkValue);

int UnionHsmCmdTA(int mode ,int keyLen,char *keyType,char *keyPlain,char *keybyLmk);
int UnionHsmCmdUI(char *mode, char *iv,int kekLen, char *Kek,char *keyType,int n ,
		int mLen, char *m,int eLen, char *e,int dLen, char *d,int pLen, char *p,int qLen, char *q,
		int dmp1Len, char *dmp1,int dmq1Len, char *dmq1,int coefLen, char *coef, unsigned char *keyByLmk,
		int *keyByLmkLen);
//	vkByKek��UGָ������Ӧ
//	vkByKey: key���ܵ�vk���ݴ�
int UnionHsmCmdUI2(char *mode,char *keyType,int keyLen,char *keyValue,char *iv,char *vkByKek,int vkByKeyLen,unsigned char *keyByLmk);
int UnionHsmCmdTY(int vkIdx,int keyLen,unsigned char *vkByLmk );

/*
   ���ܣ���WWK��Կ�ӽ�������
   �������
cryptoFlag: 	0:���� 1:����
wwk: 		LMK22-23����
encrypFlag:  	0: ECBģʽ 1: CBCģʽ
iv:  		��ʼ����(����CBCģʽ)
lenOfData: 	���������ֽ�����8�ı�����
data:		�����ܻ���ܵ�����
�������
outdata: 	������
 */
int UnionHsmCmdB8(char *wwk,int cryptoFlag,int encrypFlag,char *iv,int lenOfData,char *data,char *outdata,int sizeOfOutData);

// SM4�㷨�ӽ�������
/* �������
   encryptFlag          �ӽ��ܱ�ʶ
   1��������    
   2��������
mode:   
01 �CSM4-ECB
02 --SM4-CBC
keyType                 ֧�ֵ���Կ����ΪZEK/ZMK
lenOfKey                        1 = ˫������Կ
Key             ��S����ʾSM4��Կ,����Կ��LMK���ܵ���������
checkValue                      KeyУ��ֵ
iv                      ��ʼ������ Mode=02�ͣ�������ڡ�
lenOfDate                       �ӽ������ݳ��ȣ����ȱ�����16�ı���
data                    ���ӽ�������
�������
value                   �ӽ��ܺ������
 */     
int UnionHsmCmdWA(int encryptFlag, char *mode, char *keyType, int lenOfKey, char *Key, char *checkValue, char *iv, int lenOfDate, unsigned char *data, char *value); 

//��SM2˽Կ��SM2����
/*��4�����
  vkIndex                       ��Կ����
  lenOfVK                       �����Կ����
  VK                    �����Կ
  lenOfCiphertext               ���ĳ���
  ciphertext            ����
  �������
  lenOfData             ���ݳ���
  data                  ����
 */
int UnionHsmCmdK6(int vkIndex,int lenOfVK,char *VK,int lenOfCiphertext,unsigned char *ciphertext,int *lenOfData,char *data);

//��SM2��Կ��SCE����
/*�������
  pkIndex                       ��Կ����
  lenOfPK                       ��Կ���ĳ���
  PK                    ��Կ����
  lenOfData             ���ݳ���
  data                  ����
  �������
  lenOfCiphertext               ���ĳ���
  ciphertext            ����
 */
int UnionHsmCmdK5(int pkIndex,int lenOfPK,char *PK,int lenOfData,char *data,int *lenOfCiphertext,unsigned char *ciphertext);

// ����SM2��Կ��
/* �������
   index                ��Կ����
   lenOfVK      ��Կ�����ֽ���
   VK           ��Կ����
   �������
   ��
 */
int UnionHsmCmdK2(int index, int lenOfVK, unsigned char *VK);

/*
   �������ܣ�
   WH��ɢ��Ƭ��Կ���԰�ȫ���ķ�ʽ������PBOC3.0��
   ���������
securityMech: ��ȫ����(R��SM4���ܺ�MAC)
mode: ģʽ��־, 0-������ 1-���ܲ�����MAC
id: ����ID, 0=PBOC3.0ģʽ��CBCģʽ��ǿ�����X80�� 1= PBOC3.0ģʽ��������ָ����ECB�� 2=PBOC3.0ģʽ(ECBģʽ������������)

mk: ����Կ
mkType: ����Կ����109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
��ʹ�ü��ܻ�����Կʱ����������
mkIndex: ����Կ����, K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
mkCheckValue: ����ԿУ��ֵ
mkDvsNum: ����Կ��ɢ����, 1-3��
mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

pkType: ������Կ���� 0=TK(������Կ)1=DK-SMC(ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ)
pk: ������Կ
pkIndex: ������Կ����
pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������
pkCheckValue: ������ԿУ��ֵ

proKeyFlag:������Կ��ʶ��Y:���������Կ��N:�����������Կ����ѡ��:��û�и���ʱȱʡΪN
proFactor: ��������(32H),��ѡ��:����������Կ��־ΪYʱ��

ivCbc: IV-CBC,32H ����������ID��Ϊ0ʱ��

encryptFillDataLen: ����������ݳ���(4H),����������ID��Ϊ2ʱ�У�������1024��
����Կ����һ����м��ܵ����ݳ���
encryptFillData: ����������� nB ����������ID��Ϊ2ʱ��,����Կ����һ����м���
encryptFillOffset: �����������ƫ���� 4H ����������ID��Ϊ2ʱ��
����Կ���Ĳ��뵽����������ݵ�λ��, ��ֵ������0������������ݳ���֮��

ivMac: IV-MAC,32H ������ģʽ��־��Ϊ1ʱ��
macDataLen: MAC������ݳ��� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC��������ݳ���
macData: MAC������� nB ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC���������
macOffset: ƫ���� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ���Ĳ��뵽MAC������ݵ�λ��
��ֵ������0��MAC������ݳ���֮��
���������
mac: MACֵ 16H ������ģʽ��־��Ϊ1ʱ��
criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
criperData: nB �������������
checkValue: 16H У��ֵ

 */
int UnionHsmCmdWH(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex, char *mkCheckValue, int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex, int pkDvsNum, char *pkDvsData, char *pkCheckValue, char *proKeyFlag, char *proFactor, char *ivCbc, int encryptFillDataLen, char *encryptFillData, int encryptFillOffset, char *ivMac, int macDataLen, char *macData, int macOffset, char *mac, int *criperDataLen, char *criperData, char *checkValue);

/*
   ����:	FB:FC)���ɶ�̬��

   ����
   mk	��Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ���������Ϊ�����������Կ������
   mkType ��Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��109��309�е�һ��
   pkDvsNum ������Կ��ɢ����	1N	0~2��
   pkDvsData1 ��ɢ����1	32H	��������ɢ����������0ʱ����
   pkDvsData2 ��ɢ����2	32H	��������ɢ����������1ʱ����

   seedLen    ������Կ���ĳ���	4N	16��������
   seedKeyCiper ������Կ����	32H	�ɸ���Կ��ɢ�����Կ����
   algorithm 1��SM3�㷨
   otpKeyLen ��̬���볤��	2N	Ĭ��Ϊ06
   challengeValLen	��սֵ����	2N	00~~99
   challengeValue ��սֵ nA
   timeChangeCycle ʱ�ӱ仯���� 
   ���
   otpKey	OTP��̬����	6N	6λOTP��̬����
 */

int UnionHsmCmdFB(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int seedLen, char* seedKeyCiper, int otpKeyLen,int algorithm,  int challengeValLen, char* challengeValue, int timeChangeCycle, char* otpKey);

//�������	2A	ֵ��FF��
//������Կ����Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ��K+�����ţ��������Ϊ�����������Կ������
//����Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��309�е�һ��
//����ǰ������Կ��ɢ����	1N	1~2��
//����ǰ������ɢ����	n*32H	��������Կ��ɢ����Ϊ1~2ʱ��
//����n��������Կ��ɢ����
//���κ󱣻���Կ��ɢ����	1N	1~2��
//���κ󱣻���ɢ����	n*32H	��������Կ��ɢ����Ϊ1~2ʱ��
//����n��������Կ��ɢ����
//����ǰ�������ĳ���	4N	16��������
//����ǰ��������	n*2H	�ɸ���Կ��ɢ�����Կ������PKCS#5��䣩
//�����㷨	1N	1�� SM3
//����ǰ����У��ֵ	16H	�����㷨Ϊ1ʱ������sm3(����)ǰ8�ֽ��Ƿ�һ�¡�
//���κ�����ӳ���	4N	�����ҽ�ȡ���ٸ��ֽ�
//����Ҫ�س���	4N	
//����Ҫ��	n*2H	
//���Ӳ��뵽����Ҫ����λ��	4N	��0,���������Ϊ������+����Ҫ��

//�����	����	˵����
//��Ӧ����	2A	ֵ��FG��
//�������	2N	
//���κ���������ĳ���	4N	
//���κ����������	n*2H	�ɱ��κ�ı�����Կ������PKCS#5��䣩
//���κ�������У��ֵ	16H	

int UnionHsmCmdFF(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int newPkDvsNum, char* newPkDvsData1, char* newPkDvsData2,  int seedLen, char* seedKeyCiper, int algorithm, char *seedCheckValue,  int newSeedLen, int tranElementLen, char *tranElement, int tranOffset, char* seed, char* newSeedCheckValue);

/*
   ���� F8:?/F9)��������������Ӳ�ʹ���ض���Կ������

   ����	��
   mk		����Կ	32H/1A+32H/1A+3H	
   mkIndex 	����Կ����
   mkType	?��Կ����	3H	�����ܲ�ʹ������ʱ���ڴ������롣����֧��00A, 309�е�һ��
   mkDvsNum	������Կ��ɢ����	1N	0~2��
   mkDvsData1	��ɢ����1	32H	��������ɢ����������0ʱ����
   mkDvsData2	��ɢ����2	32H	��������ɢ����������1ʱ����
   seedLen		���ӳ���	2N	Ĭ��Ϊ16��С�ڻ����16

   ���	:
   seedKey	��������	32H	�ɸ���Կ��ɢ��ı�����Կ�������
   seedKeyCheckValue ����У��ֵ	16H	��ԿУ��ֵ������SM4�㷨��

 */

int UnionHsmCmdF8(char *mk, int mkIndex, char*mkType, int mkDvsNum, char* mkDvsData1, char* mkDvsData2 ,int seedLen,int seedAlgorithm, char *seedKey, char *seedKeyCheckValue);

/*
   �������	2	ֵ��D��
   ����Կ	32H/1A+32H/1A+3H	������ɢ�õ����������Կ�ĸ���Կ��K+�����ţ��������Ϊ�����������Կ������
   ����Կ����	3H	�����ܲ�ʹ������ʱ���ڴ�������֧��00A��309�е�һ��
   ������Կ��ɢ����	1N	1~2��
   ��ɢ����1	32H	
   ��ɢ����2	32H	��������ɢ����������1ʱ����
   ������Կ����	32H	�ɸ���Կ��ɢ�����Կ����
   ���ɶ�̬�����㷨	1N	1��SM3�㷨��2��SM4�㷨
   ��̬�����	2N	Ĭ��Ϊ06
   ��սֵ����	2N	00~~99
   ��սֵ	nA	
   ʱ��ֵT0	16N	
   ʱ�ӱ仯����Tc	3N	��Ϊ��λ��1����Ϊ��060��1�룺001
   ʱ�Ӵ���	2N	��n���ٸ�ʱ����������֤����
   ����֤��̬����	nN	

   ��Ӧ����	2	ֵ��E��
   �������	2	
   ʱ��ƫ�Ʒ���	1A	��ǰƫ�ƣ�+
   ���ƫ�ƣ�-
   ʱ��ƫ��ֵ	2N	ƫ���������ڣ�02
 */

int UnionHsmCmdFD(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int seedLen, char* seedKeyCiper, int algorithm, int otpKeyLen, int challengeValLen, char* challengeValue,int timeChangeCycle, int timeValue,int timeWindow, char* otpKey, char* clockSkew,  int* clockSkewValue);

/*
   �������	"AE"
keyValue1:	LMK��(14-15)�¼��ܵĵ�ǰTMK��TPK��PVK
keyValue2:	LMK��(14-15)�¼��ܵĴ洢TMK��TPK��PVK	

���أ�
keyByLMK:	��ǰ��Կ�¼��ܵĴ洢��Կ	
checkValue:	У��ֵ
 */
int UnionHsmCmdAE(char *keyValue1,char *keyValue2,char *keyByLMK,char *checkValue);

/*
   ����		"AG"	
tmk:		LMK��(14-15)�¼��ܵ�TMK
tak:		LMK��(16-17)�¼��ܵ�TAK

���ֵ:
keyByTMK:	TMK�¼��ܵ�TAK
checkValue:	У��ֵ
 */
int UnionHsmCmdAG(char *tmk,char *tak,char *keyByTMK,char *checkValue);
int UnionHsmCmd4H(char *vkIndex,int lenOfRandom,char *random,char *zpk,char *pan,char *pinByPK,char *pinByZPK);
/*
   ����:�����й�Կ���ܵ�һ�����ݲ���MAC
   �������
vkIndex      :˽Կ������ָ����˽Կ�����ڽ�����������
lenOfRandom  :���������
random       :�����
zak          :���ڼ���MAC����Կ
dataByPK     :������Կ���ܵ�����
�������
macByZAK
 */
int UnionHsmCmd4G(char *vkIndex,int lenOfRandom,char *rando,char *zak,char *dataByPK,char *macByZAK);
// ������Կ
/*
   ����		"WP"
   ����ֵ��
   encryFlag       1N  	KEK������Կ���� 1��3DES����  2: SM4����
   keyType1	3H	��Կ����
   keyValue1	1A+32H  ��Կ���ģ�LMK�¼�����Կ����S��+32HΪSM4��Կ����X��+32HΪDES�1�7
   checkValue1     16H     У��ֵ
   keyType2	3H	KEK��Կ����	
   keyValue2	1A+32H  LMK�¼�����Կ����S��+32HΪSM4��Կ����X��+32HΪDES,
   ��KEKΪSM4��Կ����ʹ��SM4�㷨���������ת���ܵ���Կ��
   ��KEKΪ3DES��Կ����ʹ��3DES�㷨���������ת���ܵ���Կ��
   checkValue2     16H	KEK��ԿУ��ֵ;		
   ���ֵ:
   keyValue	32H	KEK�����µ���Կ��32H�����ġ�	
 */
int UnionHsmCmdWP(int encryFlag,TUnionDesKeyType keyType1,char *keyValue1,char *checkValue1,TUnionDesKeyType keyType2,char *keyValue2
		,char *checkValue2,char *keyValue);

//��SM4��Կ��SM2��Կ�¼���ת��ΪLMK�¼���(SM4��SM2)
// ��ݸũ����
/*
   ����		"WO"
   ����ֵ��
   vkIndex		2N	˽Կ������00������,"99":ʹ�������Կ
   lenOfVK		4N	������Կ����Ϊ99ʱ�д���˽Կ�ĳ���
   vkValue		nB	������Կ����Ϊ99ʱ�д���SM2��Կ����(ֱ������SM2��16������Կֵ)
   keyType		3H	��Կ���ͣ�������Կ���͵�ö��ֵ
   lenOfSM4	4N	��Կ���ĳ���	
   sm4BySM2	nB	��Կ����(SM2��Կ����),(ֱ������SM2��16������Կֵ)
   ���ֵ��
   keyValue	1A+32H	LMK���ܵ���Կ		
   checkValue	16H	��Կ��У��ֵ��
 */
int UnionHsmCmdWO(int vkIndex,int lenOfVK,char *vkValue,TUnionDesKeyType keyType,int lenOfSM4,char *sm4BySM2,char *keyValue,char *checkValue);

/*
   ��������:
   WE::  ������ɢ/������Կ
   �������:
   mode �㷨��־:
   "01" - SM4_CBC
   "02" - SM4_ECB
   masterKeyType   ������Կ����
   masterKey       ������Կ
   desKeyType      ����Կ����
   keyLengthFlag   ����ɢ����Կ����   1 = ˫������Կ
   data            ��ɢ����
   masterKey      	������Կ 
   masterKeyIndex  K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
   masterKeyCheckValue ������ԿУ��ֵ
   iv              ����
   �������:
   key     ����Կ����
   ����ֵ��
   �ɹ���>=0

 */
int UnionHsmCmdWE(char *mode, TUnionDesKeyType masterKeyType, char *masterKey, char* masterKeyCheckValue, int masterKeyIndex, TUnionDesKeyType desKeyType, char *keyLengthFlag,char *data, char *iv, char *key);

/*
   �������
   mode ����ģʽ��ʶ
   G=��ɢ��ԿSM4����
   H=��ɢ��ԿSM4����
   I=������ԿSM4����
   J=������ԿSM4����
   id ����ID
   01=ECB
   02=CBC
   03=CFB
   mkType ����Կ����
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkCheckValue ����ԿУ��ֵ
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   lenOfData ���ݳ���
   data ����
   �������
   criperDataLen ���ݳ���  
   criperData ����   
 */
int UnionHsmCmdWC(char *mode, char *id, char *mkType, char *mk, char *mkCheckValue, int mkIndex, int mkDvsNum, char *mkDvsData, char *proFactor, int lenOfData, char *data, int *criperDataLen, char *criperData);

/*
   �������ܣ�
   WD:: ���㼰У��MAC/TAC��ԭUB��
   �������
   mode ģʽ��ʶ 1=����MAC 2=У��MAC
   id ����ID
   4=SM4MAC��ʹ��2����������Կ����SM4��
   mkType ����Կ���� 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk ����Կ
   mkCheckValue ����ԿУ��ֵ 
   mkIndex ����Կ����
   mkDvsNum ��ɢ����
   mkDvsData ��ɢ����
   proFactor ��������
   macFillFlagMAC ��������ʶ
   1=ǿ�����0X80
   0=��ǿ�����0X80
   ivMac IV-MAC 
   lenOfData MAC�������ݳ���
   data MAC��������
   macFlagMAC ���ȱ�ʶ   
   1=4 BYTE
   2=8 BYTE
   �������
   checkMac
   mac MACֵ
   ����ֵ��
   �ɹ���>=0

 */

int UnionHsmCmdWD(char *mode, char *id, char *mkType,char *mk, char* mkCheckValue, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, char *checkMac, char *mac);

// ������ԿSM1��Կ
/* 
   �������
   keyType         ��Կ����
   �������
   keyByLMK        LMK��������Կ����ֵ
   checkValue      ��ԿУ��ֵ
 */
int UnionHsmCmdM0(TUnionDesKeyType keyType, char *keyByLMK, char *checkValue);

/*
   �������ܣ�
   SM1�㷨��ɢ����Կ�����������Կ���ù�����Կ��������Կ���㲢��֤MAC��(M9)
   ���������
mode: ģʽ��־
0-���������Կ����ɢ����Կ 
1-����MAC
2-У��MAC
mkType: ������Կ����ѡ��ָ����LMK�����������Կ�͹�����Կ 
001 �C ZPK ��LMK0607���ܣ�
008 �C ZAK (LMK2627����)
00A �C ZEK(LMK3031����)
mk: ��Կ����
mkIndex: ��Կ����, K+3λ������ʽ��ȡ���ܻ��ڱ�����Կ
mkDvsNum: ָ������Կ��ɢ�Ĵ�����0-3�Σ�
mkDvsData: n*8B ��ɢ��Ƭ��Կ�����ݣ�����n������ɢ����. ��ָ���ķ�ɢ����ƴ�ӷ�ɢ������ֵ��Ϊ�������ݣ������ܼ��㣬������16 �ֽڵĽ����Ϊ����Կ
proKeyFlag: ������Կ��ʶ,�Ƿ�Ա�����Կ���й��̼���,����Ҫ���������Կʱ,ʹ�ù�����Կ��������.
Y:���������Կ
N:�����������Կ
��ѡ��:��û�и���ʱȱʡΪN
proFactor: 8B ��������,�ڼ��������Կʱ������������8 �ֽ������,����ʱ���ܻ���ƴ��8 �ֽ�ȫ ��00����
ivMac: 32H MAC�����IV
checkMac: 8H ��У���MACֵ ��ģʽ��־Ϊ2ʱ���и���
macDataLen: 4N MAC���ݳ���
macData: 8H MAC����
macPrefixDataLen: 4N MACǰ׺������ݳ���
macPrefixData: nB MACǰ׺�������
���������
criperData: 32H ����Կ����,ָ����Կ�����¼��ܣ�ģʽ��־Ϊ0�и���
checkValue: 16H	����ԿУ��ֵ,��ɢ����ԿУ��ֵ��ģʽ��־Ϊ0�и���
proCriperData: 32H ������Կ����,������Կ��ʶΪ��Y��ʱ����ģʽ��־Ϊ0�и���  ָ����Կ�����¼���
proCheckValue: 16H ������ԿУ��ֵ,������Կ��ʶΪ��Y��ʱ����ģʽ��־Ϊ0�и���
SM1��LMK0405�¼���
3DES��ָ����Կ�����¼���
mac:MACֵ 8H	��ģʽ��־Ϊ1���и���
 */

int UnionHsmCmdM9(char *mode, char *mkType, char *mk, int mkIndex, int mkDvsNum, char *mkDvsData,
		char *proKeyFlag, char *proFactor,char *ivMac, char *checkMac, int macDataLen, char *macData,
		int macPrefixDataLen, char *macPrefixData, char *criperData, char *checkValue, char *proCriperData,
		char *proCheckValue, char *mac);

//����SM2˽Կ��TK������
/*�������
  SM2Index      ��Կ����
  lenOfSM2      �����Կ���ĳ���
  SM2           �����Կ����
  keyType               ��Կ����
  Key           ����Կ�����µĴ���(SM4)��Կ
  checkValue    ������ԿУ��ֵ
  �������
  lenOfVK               ��Կ�����ֽ���
  VK            TK�����µ�SM2˽Կ����

 */     
int UnionHsmCmdK9(int SM2Index,int lenOfSM2,char *SM2,TUnionDesKeyType keyType,char *Key,char *checkValue,int *lenOfVK,unsigned char *VK);
int UnionHsmCmdK9ForInput(char mode,int SM2Index,int lenOfSM2,char *SM2,int lenOfPK,char *pk,TUnionDesKeyType keyType,char *Key,char *checkValue,int *lenOfVK,unsigned char *VK);

/*
   ����:����һ��PINУ��ֵPVV
   ָ��:PV
   ���������
pvk:	1A+32H   ��LMK1415 ���ܣ���S��+3
pvkIndex: 1N	 ȡֵΪ1-F
tpk:	1A+32H	 ��LMK1415 ���ܣ���S��+32
pinBlock:  32H   ��TPK���ܵ�ANSI9.8PIN��
PAN:	   16H   16λ��������
�������:
pvv:	4N	���ɵ�PVV
 */
int UnionHsmCmdPV(char *pvk,char pvkIndex,char *tpk,char *pinBlock,char *pan,char *pvv);

/*
   ����: ����/��֤mac
   ָ��:W9
   �������:
mode:	ģʽ��1:����MAC��2:У��MAC
zakIndex: zak������
zakValue: zak��Կ
macCalcFlag: MAC���㷽ʽ,1:�������ĵ�MAC���㷽��	
2:˳���ļ�MAC�ļ��㷽��
msgType: ��Ϣ����,0:��Ϣ����Ϊ������
1:��Ϣ����Ϊ��չʮ������
lenOfMacData: macData���ݳ��ȣ�������8192
macData: ��������MAC������
oriMac:	��У���MACֵ,����ģʽΪ2ʱ���������
sizeofDesMac:desMac����󳤶�
�������:
desMac: MACֵ

 */
int UnionHsmCmdW9(int mode,char *zakIndex,char *zakValue,int macCalcFlag,int msgType,int lenOfMacData,char *macData,char *oriMac,char *desMac,int sizeofDesMac);

/*
   ����: PIN���ת����
   ָ��: W8
   �������:
oriAlgorith:	1N	 ԴPIN���㷨��1-DES/3DES, 2-SM4
desAlgorith:	1N	 Ŀ��PIN���㷨��1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	Դzpk�����Ż�����
desKeyLenFlag: 	1N	Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
desKeyIndex/desZpkValue: 1A+3H/16/32/48H	Ŀ��zpk�����Ż�����
oriFormat:	2N	ԴPIN���ʽ,��01��-��06�������PIN���ʽ
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
desFormat:	2N	Ŀ��PIN���ʽ,��01��-��06�������PIN��
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16��?		pinBlock:	16H/32H		��Կ���ܵ�PIN
oriAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
desAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���

sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	16H/32H		ת���ܺ��PIN����
 */
int UnionHsmCmdW8(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk);


//��������ר��W8,add by huangh 2016.04.15
/*
   ����: PIN���ת����
   ָ��: W8(��������ר��W8)
   �������:
oriAlgorith:	1N	 ԴPIN���㷨��1-DES/3DES, 2-SM4
desAlgorith:	1N	 Ŀ��PIN���㷨��1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	Դzpk�����Ż�����
desKeyLenFlag: 	1N	Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
deskeytype	3N	Ŀ����Կ����:Ŀ����Կ���ͱ�ʶΪYʱ������ڣ�֧��ZPK(001) / ZEK(00A)

desKeyIndex/desZpkValue: 1A+3H/16/32/48H	Ŀ��zpk�����Ż�����
oriFormat:	2N	ԴPIN���ʽ,��01��-��06�������PIN���ʽ
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
desFormat:	2N	Ŀ��PIN���ʽ,��01��-��06�������PIN��
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ք1�7
pinBlock:	16H/32H		��Կ���ܵ�PIN
oriAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
desAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���

sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	16H/32H		ת���ܺ��PIN����
 */

int UnionHsmCmdW8_Base(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *deskeytype,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk);

/*
   ���ܣ���PIN���ZPK��SM4���ܷ��뵽LMK�¼��ܡ�
   ָ�	WX
   ���������
mode: ģʽ?:PIN��LMKת��ZPK
1:PIN��ZPKת��LMK
zpkValue: zpk��Կֵ
oriPinBlock: PIN��	
format:   pin���ʽ
accNo:    �˺�,ȥ��У��λ������12λ
sizeofPinDesPinBlock: Ŀ��pin�����󳤶�
�������:
desPinBlock: LMK�ԣ�02-03���¼��ܵ�PIN/ZPK���ܵ�PIN
 */
int UnionHsmCmdWX(int mode,char *zpkValue,char *pinBlock,char *format,char *accNo,int lenOfAccNo,char *desPinBlock,int sizeofPinDesPinBlock);

/*
   �������ܣ�(����)
   WWָ�����һ������PIN/����һ���Ѽ��ܵ�PIN
   ���������
   mode��0:����    1:����
   oriPIN��Դpinֵ
accNo: �˺�
lenOfAccNo���ͻ��ʺų���
sizeofDesPIN:desPIN����󳤶�
sizeofReferenceNum:referenceNum����󳤶�,������ʱ����Ч
���������
desPIN��Ŀ��pinֵ
referenceNum: ������ʱ������Ч 
 */
int UnionHsmCmdWW(int mode,char *oriPIN,char *accNo,int lenOfAccNo,char *desPIN,int sizeofDesPIN,char *referenceNum,int sizeofReferenceNum);

/*
   �������ܣ�(����)
   CLָ�����VISA��У��ֵCVV
   ���������
   cardValidDate��VISA������Ч��
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   cvv�����ɵ�VISA����CVV
 */
int UnionHsmCmdCL(char *cardValidDate,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,int sizeofCVV);

/*
   �������ܣ�(����)
   WVָ��������һ��PIN
   ���������
   pinLength��Ҫ��������ɵ�PIN���ĵĳ���
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinBlockByLMK�����������PIN�����ģ���LMK����
sizeofPinBlock: pinBlockByLMK�ĳ���
 */
int UnionHsmCmdWV(int pinLength,char *accNo,int lenOfAccNo,char *pinBlockByLMK,int sizeofPinBlock);

//����SM2˽Կ
/*�������
  index         ��Կ����
  isOutPutPK    �Ƿ���Ҫ������Կ, 0�����޸��򣺱�ʾ����Ҫ������Կ, 1����ʾ��Ҫ������Կ
  �������
  lenOfVK               ��Կ�����ֽ���
  VK            ��Կ����
  PK            ��Կֵ
 */
int UnionHsmCmdK8(int index,char *isOutPutPK,int *lenOfVK,unsigned char *VK,unsigned char *PK);

// ȡ��RSA��˽��Կ<TW>
int UnionHsmCmdTW(char *mode,char *vkIndex,char *pkEncoding,char *vkByMK,int *vkLen,char *pk,int *pkLen);

// �ѻ�PIN����
int UnionHsmCmdVA(char *mode, char *id, int mkIndex, char *mk, char *iv, char *pan, char *bh, char *atc, char *plainData, char *P2, int *lenOfCiperData, char *ciperData);

/*
   ���ܣ�ʹ�ö������ɵ���Կ����AnsiX9.9MACָ��
   ���������
   keyValue	32H	������Կ
   random1		16H	�����A
   random2		16H	�����B
   lenOfData	3N	MAC���ݳ���
   data		 	��չ��MAC����
   ���������
   mac		16H	MAC
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */
int UnionHsmCmdG3(char *keyValue,char *random1,char *random2,int lenOfData,char *data,char *mac);

/*
   ����:��PIN����ת��ΪMD5����
   ���������
zpk:              ZPK
srcPinBlock:      PIN����
accNo:            ���AnxiX9.8��ʽ���ʺ�
lenOfAccNo:       �˺ų���
fillMode	  ���ģʽ��0������䣬1��PINǰ��䣬2��PIN�����
fillLen		  ��䳤��
fillData	  �������
���������
dstPinBlock: 	  PIN����
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
���㷽��
1.      ��ԴZPK����Ϊ���ģ�
2.      ����ZPK���ġ�pinblock���˺Ž��ܵõ�PIN���ģ�
3.      ��PIN���Ľ���MD5��
4.      ���PIN��MD5�����
 */
int UnionHsmCmdS4(char *zpk,char *srcPinBlock,char *accNo,int lenOfAccNo,int fillMode,int fillLen,char *fillData,int tailFillLen, char *tailFill, char *dstPinBlock);

/*
   ����:��PIN�����������㷨��Anxi X9.8�㷨֮�����ת��
   ���������
zpk1:              ZPK1
zpk2:              ZPk2
changeFlag:        ת����־(00�������㷨תAnsiX9.8��01��AnsiX9.8ת�����㷨)
srcPinBlock:           16λ����Կ1���ܵ�PIN����
accNo:                 ���AnxiX9.8��ʽ���ʺ�
lenOfAccNo:            �˺ų���
���������
dstPinBlock: 16λ����Կ2���ܵ�PIN����
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
 */
int UnionHsmCmdG2(char *zpk1,char *zpk2,int changeFlag,char *srcPinBlock,char *accNo,int lenOfAccNo,char *dstPinBlock);

/*
   ��������:
   PINBLOCK��ANSI9.8��ʽ�໥ת��
   �������:
zpk1: Դzpk
zpk2: Ŀ��zpk
srcPinBlock: ԴpinBlock
srcFormat: ԴpinBlock��ʽ    1: ANSI9.8��ʽ; 2: ����ũ��PINר�ø�ʽ
dstFormat: Ŀ��pinBlock��ʽ  1: ANSI9.8��ʽ; 2: ����ũ��PINר�ø�ʽ
accNo: �˺�
�������:
dstPinBlock: Ŀ��pinBlock
����ֵ:
�ɹ�>=0, ʧ��<0
 */
int UnionHsmCmdCD(char *zpk1,char *zpk2,char *srcPinBlock,int srcFormat,int dstFormat,char *accNo,char *dstPinBlock);

/*
   ���ܣ�
   ��PIN��X9.8תΪ�����㷨����
   ����:
   srcZPK ��ǰ����PIN���ZPK��LMK�ԣ�06-07���¼���
   dstZPK ��Ҫ����PIN���ZPK��LMK�ԣ�06-07���¼��ܡ�
   srcPin ԴZPK���ܵ��ַ���������
   srcPan �û���Ч���ʺ�
   dstPan �û���Ч���ʺ�
   ���:
   lenOfPin pin���ĳ���
   dstPinBlock ����PIN���� 
 */
int UnionHsmCmdN7(char* srcZPK, char* dstZPK, char *srcPinBlock, char* srcPan, char* dstPan, int* lenOfPin, char* dstPinBlock);

/*
   ����
   ����һ����Կ����LMK���������ͬʱ��ѡʹ��ZMK��������򱣴���ָ��������
   �������
   mode    ģʽ��0��������Կ��1��������Կ����ZMK�¼��ܣ�2��������Կ��������ָ��������
   keyType ��Կ����
   keyModel        ��Կ������0��������DES��Կ��1��˫����DES��Կ��2��������DES��Կ��3��SM4��Կ��4��SM1��Կ��5��AES��Կ
   zmk     LMK�¼��ܵ�ZMK����Կ����������ģʽΪ1ʱ�д���
   keyIndex        ��Կ������������Կ���λ�ã�����ģʽΪ2ʱ�д���
   �������
   keyByLMK        LMK�����µ���Կ
   keyByZMK        ZMK�����µ���Կ������ģʽΪ1ʱ�д���
   checkValue      ��ԿУ��ֵ
   ����ֵ
   >0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdA0(int mode,TUnionDesKeyType keyType,int keyModel,char *zmk,int keyIndex,char *keyByLMK,char *keyByZMK,char *checkValue);

/*
   ����: ��˽Կ���ܺ�ĵ�¼����ת��Ϊ������EDK����
   ���������
   fillMode	1N	��䷽ʽ
   vkIndex		2N	˽Կ����,��00������20�� �� ��99��Ϊ���ⲿ�����˽Կ
   lenOfPinByPK	4N	��Կ���ܵĵ�¼�����������ݳ���
   pinByPK		nB	��Կ���ܵĵ�¼������������
   keyLen		4N	edk��Կ����
   vk		nB	������Կ���ܵ�˽Կ	
   edk		16H/32H/48H	edk��Կ
   ���������
   pinByEDK	16H	edk���ܵĵ�¼��������
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */
int UnionHsmCmd4A(int fillMode,int vkIndex,int lenOfPinByPK,char *pinByPK,int keyLen,char *keyValue,char *edk,char *pinByEDK);


/*
   ����:�����ܺ�ĵ�¼����(AN9.8)ת��Ϊ������pinoffset
   ���������
   zpk		16H/32H/48H	zpk��Կֵ
   accNo		12N		�˺�
   pinBlock	16H		zpk���ܵ�pin��������
   edk		16H/32H/48H	edk��Կ
   ���������
   pinOffset	16H	edk���ܵĵ�¼��������
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmd4B(char *zpk,char *accNo,char *pinBlock,char *edk,char *pinOffset);

/*
   ����: ���ɹ�Կ���ܵ�PINת����ANSI9.8��׼
   ���������
   vkIndex		2N		������
   keyType		1N		��Կ����, 1��TPK  2��ZPK
   keyValue	16H/32H/48H	��Կֵ
   pinType		1N		pin����,0��ԭʼPIN ,1��ANSI9.8
   fillMode	1N		������䷽ʽ,"0"��������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м���, "1"��PKCS��䷽ʽ
   accNo		16N		�˺�
   pinLen		4N		��Կ���ܵ�pin����	
   pinByPK		B		��Կ���ܵ�pin����
   ���������
   pinBlock	16H		keyValue���ܵ�pin
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmd4C(int vkIndex,int keyType,char *keyValue,int pinType,int fillMode,char *accNo,int pinLen,char *pinByPK,char *pinBlock);

int UnionHsmCmdE0_GDSNX(int dataBlock, int CrypToFlag,int modeOfAlgorithm,
		char *ZEK, int inDataType,int outDataType,int padMode,char *padChar,int padCountFlag,char *IV,
		char *lenOfMSG,char *MSGBlock,char *datafmt,char *lenOfMSG1,char *MSGBlock1,char *IV1);

// add by lisq 20141021 ��ͨ��ר��ָ�
/*
   ����
   ����һ����Կ����LMK���������ͬʱ��ѡʹ��ZMK��������򱣴���ָ��������
   �������
   mode    ģʽ��0��������Կ��1��������Կ����ZMK�¼��ܣ�2��������Կ��������ָ��������
   keyType ��Կ����
   keyModel        ��Կ������0��������DES��Կ��1��˫����DES��Կ��2��������DES��Կ��3��SM4��Կ��4��SM1��Կ��5��AES��Կ
   zmk     LMK�¼��ܵ�ZMK����Կ����������ģʽΪ1ʱ�д���
   keyIndex        ��Կ������������Կ���λ�ã�����ģʽΪ2ʱ�д���
   �������
   keyByLMK        LMK�����µ���Կ
   keyByZMK        ZMK�����µ���Կ������ģʽΪ1ʱ�д���
   checkValue      ��ԿУ��ֵ
   ����ֵ
   >0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdA0(int mode,TUnionDesKeyType keyType,int keyModel,char *zmk,int keyIndex,char *keyByLMK,char *keyByZMK,char *checkValue);

// ����һ��DES/SM4��Կ A6
/*
   ����
   ��ZMK�¼��ܵ���Կת��ΪLMK�¼��ܣ���ѡ���浽ָ��������
   �������
   algFlag ģʽ��ʶ��Ĭ��ΪT��T��ZMK��������Կ���룻R������LMK�¼�����Կ��ָ������������Ϊ��ѡ��
   keyType ��Կ����
   zmk     LMK�¼��ܵ�ZMK������ģʽΪTʱ�д���
   keyByZmk        ��Կ����ģʽΪTʱ��ΪZMK���ܵ���Կ����ģʽΪRʱ��ΪLMK���ܵ���Կ��
   keyModel        ��Կ���͡�0��DES��Կ����0��SM4��Կ��
   keyIndex        ��Կ��������ģʽΪRʱ�д��򣬵�ģʽΪTʱ�����ѡ��
   �������
   keyByLmk        ��Կֵ��LMK���ܵ���Կ������ģʽΪTʱ�С�
   checkValue      ��ԿУ��ֵ
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdA6(char *algFlag, TUnionDesKeyType keyType,char *zmk,char *keyByZmk,int keyModel,int keyIndex,char *keyByLmk,char *checkValue);

// ���һ����ԿDES/SM4 A8
/*
   ����
   LMK���ܻ���������Կʹ��ZMK�������
   �������
   mode    ģʽ��0��������������Կ��LMK���ܣ�1��LMK���ܻ�������Կ����ΪZMK���ܡ�
   keyType ��Կ����
   zmk     LMK�¼��ܵ�ZMK����Կ����������ģʽΪ1ʱ�С�
   key     ��Ҫ�������Կ������ģʽΪ1ʱ��ΪZMK���ܵ���Կ����ģʽΪ0ʱ��Ϊ��Կ������K+3H��
   keyMode ��Կģʽ��0��DES��Կ��1��SM4��Կ��
   �������
   keyByZMK        ��Կֵ����ģʽΪ0ʱ��ΪLMK���ܣ���ģʽΪ1ʱ��ΪZMK���ܡ�
   checkValue      ��ԿУ��ֵ
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdA8(int mode,TUnionDesKeyType keyType,char *key,int keyMode,char *zmk,char *keyByZMK,char *checkValue);

// װ��һ��˽Կ EK
/*
   ����
   ��LMK���ܵ�RSA/SM2˽Կ���浽ָ��������
   �������
   algFlag �㷨��ʶ��1��SM2������:RSA��
   vkIndex ˽Կ��������Χ01��20��
   lenOfVK ˽Կ����
   vk      ˽Կ
   �������
   ��
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdEK(int algFlag,int vkIndex,int lenOfVK,unsigned char *vk);


// �����ǶԳ���Կ�� EI
/*
   ����
   �����ǶԳ���Կ
   �������
   algFlag �㷨��ʶ��0��RSA��1��SM2��
   type    ��Կ���͡�0��ǩ���ͼ��ܣ�1����Կ����2��ǩ���ͼ��ܣ���Կ����3����ԿЭ�̣����ҽ����㷨��ʶΪ1ʱ�С�
   length  ��Կ����
   pkEncoding      ��Կ���롣���㷨��ʶΪ0ʱ�С�01��DER��
   lenOfPKExponent ��Կָ���ĳ��ȡ����㷨��ʶΪ0ʱ�С�
   pkExponent      ��Կָ�������㷨��ʶΪ0ʱ�С�
   exportNullPK    �㹫Կ��ʶ
   index   ������01-20��������ڱ��������ɵ���Կ��99�������������ɵ���Կ��
   �������
   pk      ��Կ
   lenOfVK ˽Կ����
   vk      ˽Կ
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdEI(int algFlag,char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,int index,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

// ��˽Կ��ȡ��Կ EJ
/*
   ����
   ��˽Կ��ȡ��Կ
   �������
   algFlag �㷨��ʶ��0��RSA��1��SM2��
   vkIndex ˽Կ������01-20��99�����˽Կ��
   vkDataLen       ˽Կ���ݿ鳤�ȡ�����˽Կ����Ϊ99ʱ�С�
   vkData  ˽Կ���ݿ顣����˽Կ����Ϊ99ʱ�С�
   pkEncode        ��Կ���롣���㷨��ʶΪ0ʱ�С�01��DER��
   �������
   pk      ��Կ��
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdEJ(int algFlag,int vkIndex,int vkDataLen,char *vkData,char *pkEncode,char *pk,int sizeOfPK);

// �����ǶԳ�˽Կ EH
/*
   ����
   �����ǶԳ�˽Կ
   �������
   algFlag �㷨��ʶ��0��ECB��1��CBC��
   proKeyType      ������Կ���͡�0��KEK��1��MDK��
   proKey  ������Կ�����ڱ���RSA/SM2����Կ����Կ������
   proKeyModel     ������Կ������0��DES˫�������ܣ�1��SM4���ܡ�
   speNum  ��ɢ��������Χ0-5.
   speData ��ɢ����
   expAlgFlag      �����㷨��ʶ��0��RSA��1��SM2��
   vkIndex ˽Կ������01-20�����ܻ���˽Կ��99�����˽Կ��
   vkLen   ˽Կ����
   vkData  ˽Կ
   padFlag ˽Կ��������ʶ��0����ǿ����䣻1��ǿ�����80��
   iv      ��ʼ���������㷨��ʶΪ1ʱ�С�
   �������
   module  ģ��
   pkM     ��ԿģM
   pkE     ��Կָ��E
   vkD     ˽Կָ��D
   vkP     ˽Կ����P
   vkQ     ˽Կ����Q
   vkDP    ˽Կ����DP
   vkDQ    ˽Կ����DQ
   vkQINV  ˽Կ����QINV
   sm2X    SM2��ԿX
   sm2Y    SM2��ԿY
   sm2D    SM2˽Կ����D
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdEH(int algFlag,TUnionDesKeyType proKeyType,char *proKey,int proKeyModel,int speNum,char *speData,int expAlgFlag,int vkIndex,int vkLen,char *vkData,int padFlag,char *iv,int *module,char *pkM,char *pkE,char *vkE,char *vkD,char *vkP,char *vkQ,char *vkDP,char *vkDQ,char *vkQINV,char *sm2X,char *sm2Y,char *sm2D);

// ˽Կ�������� EP
/*
   ����
   ʹ��RSA/SM2˽Կ��������
   �������
   algFlag �㷨��ʶ��0��RSA��1��SM2��
   padFlag ����ʶ��0������䣻1����䣬PKCS#1 1.5�����㷨��ʶΪ0ʱ�С�
   dataLen ���ݿ����ĳ��ȡ�
   cipherData      ���ݿ����ġ�
   vkIndex ˽Կ������01-20�����ܻ���˽Կ��99�����˽Կ��
   vkDataLen       ˽Կ���ȡ�
   vkData  ˽Կ��
   �������
   plainData       �������ݡ�
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdEP(int algFlag,int padFlag,int dataLen,char *cipherData,int vkIndex,int vkDataLen,char *vkData,char *plainData,int sizeOfPlainData);

// ��Կ�������� ER
/*
   ����
   ʹ��RSA/SM2��Կ��������
   �������
   algFlag �㷨��ʶ��0��RSA��1��SM2��
   padFlag ����ʶ��0������䣻1����䣬PKCS#1 1.5�����㷨��ʶΪ0ʱ�С�
   dataLen ���ݳ��ȡ�
   plainData       ���ݡ�
   pkIndex ��Կ������01-20�����ܻ��ڹ�Կ��99�������Կ��
   pkDataLen       ��Կ���ȡ�
   pkData  ��Կ��
   �������
   cipherData      �������ݡ�
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdER(int algFlag,int padFlag,int dataLen,char *plainData,int pkIndex,int pkDataLen,char *pkData,char *cipherData,int sizeOfcipherData);

// ˽Կǩ������ EW
/*
   ����
   ʹ��RSA/SM2˽Կ�������ݵ�ǩ��
   �������
   hashFlag        HASH�㷨��ʶ��0��SHA-1��1��MD5��2��ISO 10118-2��3��NO HASH��4��SM3��
   signFlag        ǩ���㷨��ʶ��0��RSA��1��SM2��
   padFlag         ����ʶ��0������䣻1����䣬PKCS#1 1.5����ǩ���㷨��ʶΪ0ʱ�С�
   userIDLen       �û���ʶ���ȡ���Χ0000-0032����ǩ���㷨��ʶΪ1ʱ�С�
   userID          �û���ʶ����ǩ���㷨��ʶΪ1ʱ�С�
   dataLen         ǩ�����ݳ��ȡ�
   data            ǩ�����ݡ�
   vkIndex         ˽Կ������01-20�����ܻ���˽Կ��99�����˽Կ��
   vkDataLen       ˽Կ���ȡ�
   vkData          ˽Կ��
   �������
   sign            ǩ����
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdEW(int hashFlag,int signFlag,int padFlag,int userIDLen,char *userID,int dataLen,char *data,int vkIndex,int vkDataLen,char *vkData,char *sign,int sizeOfSing);


// ��Կ��ǩ���� EY
/* 
   ����
   ʹ��RSA/SM2��Կ��֤ǩ��
   �������
   hashFlag                HASH�㷨��ʶ��0��SHA-1��1��MD5��2��ISO 10118-2��3��NO HASH��4��SM3��
   signFlag                ǩ���㷨��ʶ��0��RSA��1��SM2��
   padFlag                 ����ʶ��0������䣻1����䣬PKCS#1 1.5����ǩ���㷨��ʶΪ0ʱ�С�
   userIDLen               �û���ʶ���ȡ���Χ0000-0032����ǩ���㷨��ʶΪ1ʱ�С�
   userID                  �û���ʶ����ǩ���㷨��ʶΪ1ʱ�С�
   signLen                 ǩ�����ȡ�
   sign                    ǩ����
   dataLen                 ǩ�����ݳ��ȡ�
   data                    ǩ�����ݡ�
   macLen                  MAC���ȡ�
   mac                     MAC��
   pkLen                   ��Կ���ȡ�
   pk                      ��Կ��
   macDataLen              ǩ�����ݳ��ȡ�
   macData                 ǩ�����ݡ�
   �������
   ��
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdEY(int hashFlag,int signFlag,int padFlag,int userIDLen,char *userID,int signLen, char *sign,int dataLen,char *data,int macLen, char *mac,int pkLen,char *pk,int macDataLen,char *macData);

// ��������ժҪ GM
/*
   ����
   ��������ժҪ
   �������
   hashFlag                HASH�㷨��ʶ��1��SHA-1��2��MD5��3��ISO 10118-2��4��SM3��
   dataLen                 ���ݳ��ȡ�
   data                    ���ݡ�
   userIDLen               �û���ʶ���ȡ���Χ0000-0032����ǩ��HSAH�㷨��ʶΪ3ʱ�С�
   userID                  �û���ʶ����ǩ��HSAH�㷨��ʶΪ3ʱ�С�
   pkLen                   ��Կ���ȡ�
   pk                      ��Կ��
   �������
   hash                    HASHֵ��
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdGM(int hashFlag,int dataLen,char *data,int userIDLen,char *userID,int pkLen,char *pk,int sizeofHash,char *hash);

// ���㼰У��MAC/TAC UB
/*
   ����
   ��ָ����Կ����ָ����������ɢ�õ�����Կ�������Կ��ΪMAC/TC������Կ���������ݼ���MAC/TC��
   �������
   algFlag         �㷨��ʶ��0��DES��1��SM4��
   mode            ģʽ��ʶ��0������MAC��1��У��MAC��
   id              ����ID��0��ʹ������Կ����3DES/SM4 ����MAC��1��ʹ�ù�����Կ����DES MAC��2��TAC��3��ʹ�ù�����Կ����3DES/SM4 ����MAC��
   mkType          ����Կ���͡�0��MK-AC��1��MK-SMI��2��MK-SMC��3��MK-DN��4��TAK��5��ZAK��
   mk              ����Կ��
   mkIndex         ����Կ������
   mkDvsNum        ����Կ��ɢ��������Χ0-5.
   mkDvsData       ����Կ��ɢ���ݡ�
   proFactor       �������ݡ���������IDΪ1��3ʱ�С�
   macFillFlag     MAC��������ʶ��0��ǿ�����0X80��1����ǿ�����0X80��
   ivMac           IV-MAC��
   lenOfData       MAC�������ݳ��ȡ�
   data            MAC�������ݡ�
   macFlag         MAC���ȡ�
   checkMac        ��У���MAC������ģʽ��ʶΪ1ʱ��
   �������
   mac             MAC������ģʽ��ʶΪ0ʱ�С�
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdUB(int algFlag, char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, int macLen, char *checkMac, char *mac);

// ���㼰У��MAC/TAC UB
/*
   ����
   ��ָ����Կ����ָ����������ɢ�õ�����Կ�������Կ��ΪMAC/TC������Կ���������ݼ���MAC/TC��
   �������
   algFlag         �㷨��ʶ��0��DES��1��SM4��
   mode            ģʽ��ʶ��0������MAC��1��У��MAC��
   id              ����ID��0��ʹ������Կ����3DES/SM4 ����MAC��1��ʹ�ù�����Կ����DES MAC��2��TAC��3��ʹ�ù�����Կ����3DES/SM4 ����MAC��
   mkType          ����Կ���͡�0��MK-AC��1��MK-SMI��2��MK-SMC��3��MK-DN��4��TAK��5��ZAK��
   mk              ����Կ��
   mkIndex         ����Կ������
   mkDvsNum        ����Կ��ɢ��������Χ0-5.
   mkDvsData       ����Կ��ɢ���ݡ�
   proFactor       �������ݡ���������IDΪ1��3ʱ�С�
   macFillFlag     MAC��������ʶ��0��ǿ�����0X80��1����ǿ�����0X80��
   ivMac           IV-MAC��
   lenOfData       MAC�������ݳ��ȡ�
   data            MAC�������ݡ�
   macFlag         MAC���ȡ�
   checkMac        ��У���MAC������ģʽ��ʶΪ1ʱ��
   �������
   mac             MAC������ģʽ��ʶΪ0ʱ�С�
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdUB(int algFlag, char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, int macLen, char *checkMac, char *mac);


// ARQC/TC/ACCУ�飬ARPC���� KW
/*
   ����
   ARQC/TC/ACC��У�飬ARPC�Ĳ�����ͬʱ��֤ARQC������ARPC
   �������
   mode    ģʽ��ʶ��0������֤ARQC��1����֤ARQC��EMV4.1��ʽһ����ARPC��2����EMV4.1��ʽһ����ARPC��3����֤ARQC��EMV4.1��ʽ������ARPC��4����EMV4.1��ʽ������ARPC��5������֤ARQC��VISA����6����֤ARQC��EMV4.1��ʽһ����ARPC��VISA����
   id      ��Կ��ɢ������
   mkIndex ����Կ������
   mk      ����Կ��
   iv      ��ʼ������
   dvsNum  ��ɢ������
   dvsData ��ɢ���ݡ�
   lenOfPan        �˺ų��ȡ�
   pan     �˺�
   bh      B/H����
   atc     ATC
   lenOfData       �������ݳ���
   data            ��������
   ARQC            ����֤��ARQC
   ARC     ARC
   csu     CSU
   lenOfData1      �������ݳ���
   data1   ��������
   �������
   ARPC    ARPC
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdKW(char *mode, char *id, int mkIndex, char *mk, char *iv,int dvsNum,char *dvsData,
		int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,
		char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,
		char *data1,char *ARPC);

// ������ȫͨ���Ự��Կ G1
/*
   ����
   ������ȫͨ���Ự��Կ
   �������
   algFlag         ��ɢ��ʶ��0���ȷ�ɢ����Կ���ɢ������Կ��1��ֻ��ɢ������Կ��
   keyType         ����Կ���͡�
   keyIndex        ����Կ������
   key             ����Կֵ��
   keyData         ��ɢ��Կ���ݡ�������ɢ��ʶΪ0ʱ�С�
   SN              SN��
   �������
   sencKey         ��ȫͨ����֤��Կ
   cmacKey         CMAC������Կ
   rmacKey         RMAC������Կ
   sdekKey         ���ݼ�����Կ
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdG1(int algFlag,TUnionDesKeyType keyType,int keyIndex,char *key,char *keyData,char *SN,char *sencKey,char *cmacKey,char *rmacKey,char *sdekKey);

// �Գ�ת�������� UY
/*
   ����
   �Գ�ת�������ݣ�Ǯ��/����ҵ��
   �������
   tranFlag        �㷨ת����ʶ��0��DESתSM4��1��SM4תDES��2��SM4תSM4��3��DESתDES��
   srcKeyType      Դ��Կ����
   srcKeyIndex     Դ��Կ����
   srcKey          Դ��Կ
   srcAlgFlag      Դ�㷨��ʶ��0��ECB��1��ECB_LP��2��ECP_P��3��CBC��4��CBC_LP��5��CBC_P��
   destKeyType     Ŀ����Կ����
   destKeyIndex    Ŀ����Կ����
   destKey         Ŀ����Կ
   destAlgFlag     Ŀ���㷨��ʶ��0��ECB��1��ECB_LP��2��ECP_P��3��CBC��4��CBC_LP��5��CBC_P��
   srcIv           Դ��ʼ��������Դ�㷨��ʶΪCBCʱ�С�
   destIv          Ŀ�ĳ�ʼ��������Ŀ���㷨��ʶΪCBCʱ�С�
   dataLen         ���ݳ���
   data            ����
   �������
   destData        Ŀ������
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdUY(int tranFlag,TUnionDesKeyType srcKeyType,int srcKeyIndex,char *srcKey,int srcAlgFlag,TUnionDesKeyType destKeyType,int destKeyIndex,char *destKey,int destAlgFlag,char *srcIv,char *destIv,int dataLen,char *data,int sizeofDestData,char *destData);


// ��ɢ��Կ���ݼӽ��ܼ��� U1
/*
   ����
   ��ָ����Կ����ָ����������ɢ�õ�����Կ�������Կ��ʹ�ø���Կ���������ݽ��м��ܻ���ܡ�
   �������
   algFlag         �㷨��ʶ��0��DES��1��SM4��
   mode            ����ģʽ��ʶ
   id              ����ID
   mkType          ����Կ����
   mk              ����Կ
   mkIndex         ����Կ����
   mkDvsNum        ��ɢ����
   mkDvsData       ��ɢ����
   proFactor       ��������
   iv              ��ʼ����
   lenOfData       ���ݳ���
   data            ����
   �������
   criperData      ���Ļ�����
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdU1(int algFlag, int mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, char *proFactor, char *iv, int lenOfData,
		char *data, int *criperDataLen, char *criperData);

// ��ȫ���ķ�ʽ������Կ U3
/*
   ����
   ��ɢ��Ƭ��Կ���԰�ȫ���ķ�ʽ����
   �������
   algFlag         �㷨��ʶ��0��DES��1��SM4��
   mode            ģʽ��ʶ��0�������ܣ�1�����ܲ�����MAC��
   id              ����ID
   mkType          ����Կ����
   mk              ����Կ
   mkIndex         ����Կ����
   mkDvsNum        ��ɢ����
   mkDvsData       ��ɢ����
   proKeyType      ������Կ����
   proKey          ������Կ
   proKeyIndex     ������Կ����
   proDvsNum       ������Կ��ɢ����
   proDvsData      ������Կ��ɢ����
   proFlag         ������Կ��ʶ
   proFactor       ��������
   iv              ��ʼ����
   encPadDataLen   �������ݳ���
   encPadData      ��������
   ivMac           IV-MAC
   macPadDataLen   MAC������ݳ���
   macPadData      MAC�������
   dataOffset      ƫ����
   �������
   criperDataLen   �������ݳ���
   criperData      ��������
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdU3(int algFlag, char *mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, int proKeyType, char *proKey, int proKeyIndex,
		int proDvsNum, char *proDvsData,int proFlag, char *proFactor, char *iv, int encPadDataLen,
		char *encPadData, char *ivMac, int macPadDataLen, char *macPadData, char *dataOffset, char *mac, int *criperDataLen, char *criperData);

// ����һ���Գ���Կ���Է�����ʽ��ӡ NE
/*
   ����
   ����һ���Գ���Կ���Է�����ʽ��ӡ
   �������
   keyType         ��Կ����
   keyLength       ��Կǿ��
   sm4Flag         SM4��Կ��ʶ����0��SM4��Կ��0��DES��Կ��
   fldNum          ��������
   fld             ����ֵ
   �������
   component       ��Կֵ
   ����ֵ
   >=0���ɹ�
   <0��ʧ��
 */
int UnionJTBHsmCmdNE(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int sm4Flag,int fldNum,char fld[][80],char *component);

// add by lisq 20141021 end

/*      
	����:   ��C-mac�㷨����MAC 
	���������
	zakKey          32H             lmk(008)���ܵ�ZAK
	IV              16H             ��ʼ����
	macData         NH              ����MAC����
	���������
	CMAC           16H             C-MAC       
	ICV             16H             ZAK��벿�ݣ�16λ����C-MAC DES��������
	����ֵ��        
	<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0������ִ�гɹ�
 */  
int UnionHsmCmdCF(char *zakKey, char *IV, char *macData, char *CMAC, char *ICV);
//add by zhouxw 20141115 end

// add by lisq 20141117 ��֦������
/*
   ����
   SM2��Կ���ܵ�PIN������SM2˽Կ��SCE���ܺ�����SM4�㷨�������
   �������
   mode    �㷨��ʶ��0��SM2����תΪSM4���ܣ�1��SM2����תΪ��֦��ר���㷨���ܡ�
   vkIndex ˽Կ������01-20��99�������Կ
   vkLen   ���˽Կ���ȡ�����vkIndexΪ99ʱ��
   vk      ���˽Կ������vkIndexΪ99ʱ��
   cipherTextLen   ���ĳ���
   cipherText      ����
   zpkValue        ZPK��Կֵ
   accNo   �˺�
   �������
   pinBlkByZpk     ZPK���ܵ�PIN����

   ����ֵ
   >=0���ɹ���<0��ʧ��
 */

int UnionHsmCmdKH(int mode, int vkIndex, int vkLen, char *vk, int cipherTextLen, char *cipherText, char *zpkValue, char *accNo, char *pinBlkByZpk, int sizeofPinBlkByZpk);

/*
   ����
   ˽�м����㷨����pin
   �������
   dataLen �������ݳ��ȣ���֧��6λ
   plainData       �������ݣ���֧��6λ
   sizeofCipherData        �����������������С
   �������
   cipherData      ��������
   ����ֵ  
   <0��ʧ��
   >=0���ɹ�
 */
int UnionHsmCmdWY(int dataLen, char *plainData, char *cipherData, int sizeofCipherData);

// add by lisq 20141117 end ��֦������

int RacalCmdHN( char *key1,char *key2,char *date, char *flag, int len ,char *cdate ,int *dstlen,char *dstdate);


int RacalCmdHM(char *key1, char *flag, char *date, int len ,char *macdate ,char *mac);

int RacalCmd50ForHR(char flag,char *edk,int lenOfData,char *indata,char *outdata,int *sizeOfOutData);

int RacalCmdTQ(int modeFlag, char *algFlag, char *keyType, int keyLen, char *keyValue, char *iv, int macDataLen, char *macData, int macLen, char *mac);

int RacalCmdHJ(char *vkIndex, int lenOfVK, char *valueOfVK, char *flag ,char *keyOfZPK ,char *lenOfPin,char *pinBlock,char *clientNo,char *pinOfHR,char *replayFactor);

int RacalCmdHK (char *index,char * keylen, char *key,char *flag,  char * datalen, char *data, char *num,char *genelen,char *gene,char *pin);

// modify by zhouxw 20160105 begin
//int RacalCmdTS( char *key1,char *key2,int blen,char *bdate,int alen,char *adate, char *dstkey);
int RacalCmdTS(char *mode, char *key1,char *key2,char *IV,int blen,char *bdate,int alen,char *adate, char *dstkey);
// modify by zhouxw 10160105 end

int RacalCmdHI(char *factorOfKey, char *keyOfZPK, char *pan, char *pin ,char *pinBlock);

int RacalCmdHB( char *cpOriZPK, char *cpDestZPK, char *AccNo, char *cpOriPIN,char *cpOutPIN);

int RacalCmdHE( char *zpkvalue, char* srcpinblock, int flag,char *account,char *dstpinbolck);

int RacalCmdHF( char *zpkvalue, int flag,char *account,char *gene,char *encrypteddata,char *dstpinbolck,char *nodeid);


/*
   ���ܣ���Կ���ܵ�PIN����תΪDES��Կ���ܣ������Ӽ��ܣ���
   �������:
   srcAlgorithmID  �ǶԳ���Կ�㷨��ʶ,0:�����㷨,  1:�����㷨
   vkIndex         ָ����˽Կ�����ڽ���PIN��������
   lenOfVK      ˽Կ����
   vkValue      ˽Կֵ
   flag         ��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
                ��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
   dstAlgorithmID ZPK��Կ�㷨��ʶ,0:�����㷨,  1:�����㷨
keyValue:       ���ڼ���PIN����Կ
pan             �û���Ч���ʺų���
lenOfPinByPK    ����Կ���ܵ�PIN�������ĳ���
pinByPk         ����Կ���ܵ�PIN��������
�������:
lenOfPinBlock   des��Կ���ܵ�pin����
pinBlock        des��Կ���ܵ�pin
idCodeLen       01-20
idCode          ID������
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
 */
int RacalCmdN8(int srcAlgorithmID,int vkIndex,int lenOfVK,char *vkValue, char flag, int dstAlgorithmID,char *keyValue, char *pan, int lenOfPinByPK,char *pinByPk,int *lenOfPinBlock, char *pinBlock, int *idCodeLen, char *idCode);

// add by lisq 20150120 ΢������

int UnionHsmCmdKJ(int mode, int flag, char *mkType, char *zek, char *mk, int divNum, char *divData, char *pk, int *keyByPkLen, char *keyByPk);
// add by lisq 20150120 end ΢������

int UnionHsmCmd8A(int mode, int vkIndex,int vkLen,char *vkValue,int  fillMode,int lenOfPinByPK, char *pinByPK, char *factorData,int keyLen, char *keyValue, char *randomData, int lenOfAccNo,char *accNo, char *dataByZEK,char *digest, char *pinBlock);

/*
   �������ܣ�
   8Dָ���IBM��ʽ����һ��PIN��Offset
   ���������
   minPINLength����СPIN����
   pinValidData���û��Զ�������
   decimalizationTable��ʮ����������ʮ��������ת����
   pinLength��LMK���ܵ�PIN���ĳ���
   pinBlockByLMK����LMK���ܵ�PIN����
   pvkLength��LMK���ܵ�PVK����
   pvk��LMK���ܵ�PVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   pinOffset��PIN Offset������룬�Ҳ�'F'

 */
int UnionHsmCmd8D(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset);

/*
   ��������:
   KKָ��,SM2��Կ���ܵ�pinת����zpk��DES��SM4������
   ���������
   keyIndex        SM2��Կ����
   vkLen           SM2�����Կ����
   vkValue         SM2�����Կ
   lenOfPinByPK    SM2��Կ���ܵ����ĳ���
   pinByPK         ����
   algorithmID     �㷨��ʶ,1��SM4��Կ, 2: 3DES��Կ
   keyType         ��Կ����,1��ZPK, 2��TPK 
   pinFormat       PIN��ʽ
   lenOfAccNo      �˺ų���        
   accNo           �˺�
   keyLen          ��Կ����
   keyValue        ZPK��TPK��Կ
   sizeofPinByKey  pinByKey����󳤶�
   ���������
   pinByKey        DES��SM4�¼��ܵ�PIN

 */

int unionHsmCmdKK(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *pinFormat, int lenOfAccNo,char *accNo,
		char *keyValue,char *specialAlg, char *pinByKey,int sizeofPinByKey);
/*
   ��������:
   KLָ��,SM2��Կ���ܵ���������ת����zpk��DES��SM4������
   ���������
   keyIndex        SM2��Կ����
   vkLen           SM2�����Կ����
   vkValue 	SM2�����Կ
   lenOfPinByPK    SM2��Կ���ܵ����ĳ���
   pinByPK  	����
   algorithmID     �㷨��ʶ,1��SM4��Կ, 2: 3DES��Կ
   keyType         ��Կ����, 0��ZMK, 1��ZPK, 2��ZEK 
   keyValue        ��Կֵ
   sizeofPinByKey	pinByKey��󳤶�
   ���������
   pinByKey        DES��SM4�¼��ܵ�PIN

 */

int unionHsmCmdKL(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *keyValue, char *pinByKey,int sizeofPinByKey);

int UnionHsmCmdKL(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *keyValue, int saltedOffset, int saltedLen, char *saltedData, char *pinByKey, int sizeofPinByKey, char *hash, int sizeOfHash);

/*(����)
  �������ܣ�
  8Eָ���IBM��ʽ����һ��PIN��Offset
  ���������
  minPINLength����СPIN����
  pinValidData���û��Զ�������
  decimalizationTable��ʮ����������ʮ��������ת����
  pinLength��LMK���ܵ�PIN���ĳ���
  pinBlockByLMK����LMK���ܵ�PIN����
  pvk��LMK���ܵ�PVK
  accNo���ͻ��ʺ�
  lenOfAccNo���ͻ��ʺų���
  ���������
  pinOffset��PIN Offset������룬�Ҳ�'F'

 */
int UnionHsmCmd8E(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset);

/*
   ��������: 9Bָ��        ��ZPK���ܵ�PIN����תΪ�������㷨����
   �������:
   algorithmID                     ��Կ�㷨��ʶ    03DES
   1SM4
   mode                            ����ģʽ  0�����Ĵ����㷨
   1�����Ŀ��㷨
   2�����������㷨
   accNo1                          �������  �˺���ȥ��У��λ������7λ
   zpk                             ��LMK�Լ����µ�zpk
   pinBlock                        ZPK���ܵ�PIN����
   format                          Pin��ʽ
   accNo2                          �˺�  �˺���ȥ��У��λ������12λ
   �������:
   Pin                             ˽�����㷨���ܵ�PIN����
 */
int UnionHsmCmd9B(int algorithmID, char *mode, char *accNo1, char *zpk, char *pinBlock, char *format, char *accNo2, char *Pin);

/*
   �������ܣ�9Cָ��        �������������㷨���ܵ�PIN����תΪZPK����
   ���������
   algorithmID                     ��Կ�㷨��ʶ    03DES
1 :SM4
zpk                             ��LMK�Լ����µ�zpk
pinBlock                        ZPK���ܵ�PIN����
accNo                           �˺�
���������
pinByZPK                        ZPK���ܵ�PIN����
 */
int UnionHsmCmd9C(int algorithmID, char *zpk, char *pinBlock, char *accNo, char *pinByZPK);

/*
   ��������:9Dָ��         ����&�����������MAC
   ���������
   mode            ģʽ    1������MAC
   2��У��MAC
   macDataLen      ��������MAC�����ݵĳ���
   macData         ��������MAC������
   mac1Len         ��У���MACֵ�ĳ���
   mac1            ��У���MACֵ
   �������
   mac2            ���ɵ�MAC
 */
int UnionHsmCmd9D(char *mode, char *macDataLen, char *macData, char *mac1, char *mac2);
/*
   ����: ����չ���ZPK���ܵ���������һ����Կ����(��Ϊ��չ����ʽ)
   ָ��: KM
   �������:
pinFlag:	1A		Y:ԴPIN��Ϊ��չ��ʽ��
N:δ��չԴPIN��
������Ĭ��ΪY
oriAlgorith:	1N		ԴPIN���㷨��1-DES/3DES, 2-SM4
desAlgorith:	1N		Ŀ��PIN���㷨��1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N		Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
oriKeyIndex/oriZpkValue:1A+3H/16/32/48H	Դzpk�����Ż�����
desKeyLenFlag: 	1N		Դ��Կ���ȱ�ʶ,1-������2-˫����3-����
desKeyIndex/desZpkValue:1A+3H/16/32/48H	Ŀ��zpk�����Ż�����
oriFormat:	2N		ԴPIN���ʽ,��01��-��06�������PIN���ʽ
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
desFormat:	2N		Ŀ��PIN���ʽ,��01��-��06�������PIN��
��ԴPIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16��
pinBlock:	32H/48H/64H	��Կ���ܵ�PIN
oriAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
desAccNo:	12/18N		12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���

sizeofPinByZpk:	pinByZpk����󳤶�.
�������:
pinByZpk:	32H/48H/64H		ת���ܺ��PIN����
 */
int UnionHsmCmdKM(char pinFlag,int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk);

int UnionHsmCmdSD (int srcAlgorithmID,int pkIndex,int lenOfVK,char *vkValue,char *fillMode,char *dataFlag,int lenOfData,char *data,int lenOfID, char *ID,int dstAlgorithmID,int lenofhashFlag,char *hashFlag,char *EDK,char *pinBlockByPK,char *lenOfPIN,char *pinBlockByEDK);

/*
   ����:ת��PIN��
   �������:
   rootKey��Ӧ������Կ
   keyType����Կ���ͣ�001-ZPK��008-ZAK��
   discreteNum����ɢ����
   discreteData1����ɢ����1
   discreteData2����ɢ����2
   discreteData3����ɢ����3
   resPinBlock��Դpinblock
   AccNo���ʺ�
   pinFmt��pin��ʽ
   zpk��Ŀ��zpk
   �������:
   disPinBlock��Ŀ��pinblock��
   errCode������������롣
   ����ֵ:
   <0: ����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0: ����ִ�гɹ�
 */
int UnionHsmCmdY5 (char *rootKey, char *keyType,int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char *resPinBlock, char *AccNo, char *pinFmt, char * zpk, char *disPinBlock);

/*
   ����:Mac���㣬����MACֵ��
   �������:
   rootKey��Ӧ������Կ
   keyType����Կ���ͣ�001-ZPK��008-ZAK��
   discreteNum����ɢ����
   discreteData1����ɢ����1
   discreteData2����ɢ����2
   discreteData3����ɢ����3
   msgBlockNum����Ϣ��� (0:��һ��,1:��һ��,2:�м��,3:����)
   msgType����Ϣ����(0����Ϣ����Ϊ������,1����Ϣ����Ϊ��չʮ������)
   iv��IV,Ĭ�ϳ�ʼֵ16H��0
   msgLength����Ϣ����
msg: ��Ϣ��
�������:
Mac��MAC
errCode������������롣
����ֵ:
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
>=0������ִ�гɹ�
 */
int UnionHsmCmdY7( char *rootKey, char * keyType,int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char msgBlockNum , char msgType, int  msgLength, char *msg, char *mac);

//��SM2��Կ��SCE����
/*�������
  pkIndex                       ��Կ����
  lenOfPK                       ��Կ���ĳ���
  PK                    ��Կ����
  lenOfData             ���ݳ���
  data                  ����
  �������
  lenOfCiphertext               ���ĳ���
  ciphertext            ����
 */
int UnionHsmCmdKE(int pkIndex,int lenOfPK,char *PK,int lenOfData,char *data,int *lenOfCiphertext,unsigned char *ciphertext);

//��SM2˽Կ��SM2����
/*�������
  vkIndex                       ��Կ����
  lenOfVK                       �����Կ����
  VK                    �����Կ
  lenOfCiphertext               ���ĳ���
  ciphertext            ����
  �������
  lenOfData             ���ݳ���
  data                  ����
 */     
int UnionHsmCmdKF(int vkIndex,int lenOfVK,char *VK,int lenOfCiphertext,unsigned char *ciphertext,int *lenOfData,char *data);

// ����SM2��Կ
/* �������
   index                ��Կ����
   lenOfPK      ��Կ�����ֽ���
   PK           ��Կ����
   �������
   ��
 */
int UnionHsmCmdE8(int index, int lenOfPK, char *PK);

//��ZPK��DES��SM4�����ܵ�pinת��SM2��Կ����
/* �������
   algorithmID  1A      �㷨��ʶ,1:SM4��Կ, 2:3DES��Կ
   SM2Index     2N      SM2����,"00"---"20"
   lenOfSM2	4N	SM2˽Կ����
   SM2Value	nB	SM2˽Կֵ
   keyType      1A      ��Կ����,1:ZPK, 2:TPK
   keyIndex     1A+3H   ��Կ���� 
   keyValue     1A+32H  ��Կֵ,3DES 1AΪX��SM4 1A ΪS
   format       2H      �μ�pin��ʽ˵��
   lenOfAccNo   nN      �˺ų��� 
   accNo        nN      �˺�
   pinBlock     16/32H  ����㷨��ʶ��3des��16H
   ����㷨��ʶ��SM4��32H
   sizeofPinByPK 4N     ����ֵ���ֵ

   �������
   pinByPK      nB      ��Կ���ܵ�����  
   ��
 */
int UnionHsmCmdKN(int algorithmID, int SM2Index,int lenOfSM2,char *SM2Value,int keyType,char *keyIndex,char *keyValue,char *format,int lenOfAccNo,char *accNo,char *pinBlock,char *pinByPK,int sizeofPinByPK);

int UnionHsmCmdVB(char* modeOfAlgorithm,char* szMethodID, char* szMKSMC, char* szPanNum, char* szAtc, char* szZPK, char* szPinCryptograph , char* szAccount, char* szLinkOffPinMod, char* szEncryptedData);


/*
   ����: �����ַ�PIN��SM4���ܵ�ANSI9.8��ʽ��pinblockת��ΪSM4�㷨������ʽ��PIN
   ָ��: LD
   �������:
oriZpkValue: 1A+32H     Դzpk����
oriZpkCheckValue:       16H     �����Կ��Ҫ������ԿУ��
desZpkValue: 1A+32H     Ŀ��zpk����
dstZpkCheckValue:       16H     �����Կ��Ҫ������ԿУ��
pinBlock:       32H     AnsiX9.8��ʽ��PIN����
lenOfOriAccNo:  N       Դ�˺ų���
oriAccNo:       12N     �û���Ч���ʺ�
lenOfDstAccNo:  N       Ŀ���˺ų���
desAccNo:       12N     �û���Ч���ʺ�
sizeofPinByZpk: pinByZpk����󳤶�.
�������:
pinLen          N               pin���ĳ���
pinByZpk:       64H             ת���ܺ��PIN����
 */
int UnionHsmCmdLD(char *oriZpkValue,char *oriZpkCheckValue,char *dstZpkValue,char *dstZpkCheckValue,char *pinBlock,int lenOfOriAccNo,char *oriAccNo,int lenOfDstAccNo,char *dstAccNo,int *pinLen,char *pinByZpk,int sizeofPinByZpk);

/*
   ����: PIN����ת��Ϊ���������㷨���ܻ�ת
   ָ��: LC
   �������:
   mode            2N      00��PIN��DES/3DES/SM4����תΪ�����������
   01��PIN�ɺ����������תΪSM4/DES/3DES����
   algorithmID     2N      1: des/3des
2: SM4
zpkIndex:       1A+3H   zpk��Կ������
zpk:            16H/1A+32H/1A+48H       zpk����
checkValue:     16H     ��ZPKΪ��S��+32H�ķ�ʽʱ�������
format:         2N      ��01��-��06�������PIN���ʽ
��PIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ��01�� ����9.8��ʽ���pin���F��16�ֽ�
pinBlock:       16H/32H/24H     PIN��Կ���ܵ�PIN������,�����ɼ���ԴPIN����㷨������
��ģʽΪ01�����㷨���ܵ�PIN����
lenOfAccNo:     N       Դ�˺ų���
accNo:          12/18N          12����01�����ͻ�ԭʼ�˺���12λ��ȥУ��λ��
18����ʽ04,������ʽ�޴���
sizeofPinByZpk: pinByZpk����󳤶�.
�������:
pinByZpk:       16H/32H         PIN��Կ���ܵ�PIN������,�����ɼ���PIN����㷨������ģʽΪ01������ڡ�
 */
int UnionHsmCmdLC(int mode,int algorithmID,char *zpkIndex,char *zpk,char *checkValue,int format,char *pinBlock,int lenOfAccNo,char *accNo,char *pinByZpk,int sizeofPinByZpk);

/*
   ����: ˽Կ���ܶԳ���Կ���ٽ��Գ���Կ���ܵĵ�¼����תΪmd5��SHA1������
   ָ��: HR
   �������:
   fillMode        1N      0: ������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油
1: PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
vkIndex         2N      ����ֵ"00-20"
lenOfVKValue    4N      ˽Կ����
vkValue         nB      ˽Կֵ
lenOfKeyByPK    4N      ��Կ���ܵ���Կֵ����
keyByPK         nB      ��Կ���ܵ���Կֵ
pinBlock        16H/32H ��¼��������
random		32H	�����
algorithmID     1N      1��md5, 2��SHA1
sizeofOutPinBlock       nN      pinByMD5����󳤶�
�������:
outPinBlock:    16H/32H
 */
int UnionHsmCmdHR(int fillMode,int vkIndex,int lenOfVKValue,char *vkValue,int lenOfKeyByPK,char *keyByPK,char *pinBlock,char *random,int algorithmID,char *outPinBlock,int sizeofOutPinBlock);
/*
   ����: ������ɢ��������Կ
   ָ��: US
   �������:
   mode		2N		��ɢģʽ	01: PBOC��ɢ
02: PBOC��ɢ������Կ���������
algorithmID	1A		��ɢ�㷨���� 	X��3DES�㷨	S: SM4�㷨
rootKeyType	3H		����Կ���� 	008,00A,000,001
rootKeyValue	1A+32H/3H	����Կ		�㷨��������ɢ�㷨��������һ��
discreteNum	1N		��ɢ����	1-3
discreteData	N*16H		��ɢ����
processFlag	1A		������Կ��ʶ	Y:�й�����Կ
N:�����ɹ�����Կ
processData	32H		���ɹ�����Կ����
lenOfData	2N		��ɢģʽΪ02ʱ����
data		32H		���ⲿ��0����,��ɢģʽΪ02ʱ����
discreteKeyType	3H		������ɢ����Կ����	008,00A,000,001

�������:
keyValue	32H		��Կ����
checkValue	16H		У��ֵ
lenOfOutData	2N		���ݳ���
outData				��Կ��������������ֵ
 */
int UnionHsmCmdUS(int mode,char algorithmID,TUnionDesKeyType rootKeyType,char *rootKeyValue,int discreteNum,char *discreteData,char *processFlag,char *processData,int lenOfData,char *data,TUnionDesKeyType discreteKeyType,char *keyValue,char *checkValue,int *lenOfOutData,char *outData);

/*
   ����:��PIN��X9.8תΪ�����㷨����
   ���������
   zpk		16H/32H/48H	zpk��Կֵ
   accNo		12N		�˺�
   pinBlock	16H		zpk���ܵ�pin��������
   algFlag  1A Ŀ��edk��־
   edk		16H/32H/48H	edk��Կ
   ���������
   pin	16H	���ص�pin��
   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */

int UnionHsmCmdNI(char *zpk,int algFlag, char *edk,char *pinBlock,char *accNo,char *pin);
/*
   ����:��ZPK���ܵ�PIN������������
   ���������
   algorithmID	1N		�㷨��ʶ,1:des/3des, 2:SM4
   keyIndex	1A+3H		��Կ������
   zpk		16H/1A+32H/1A+48H	zpk��Կֵ
   checkValue	16H		��ZPKΪ����ʱ���������
   format	2N		PIN���ʽ,��PIN���㷨ΪSM4������ָʾ�ĸ�ʽֻ��Ϊ"01"
   pinBlock	16H		zpk���ܵ�pin��������
   accNo	12N/18N		�˺�
   rule		1N		0:6λ����Ϊ��ͬ������
1:ʹ���������/�ݼ�1��˳���������
2:�������0��1����������
���������
����ֵ��
<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
=0:	0:��ʾ��������
=1:	1:��ʾ������

 */

int UnionHsmCmdZY(int algorithmID,char *keyIndex,char *zpk,char *checkValue,char *format,char *pinBlock,char *accNo,int rule);
/*
   ���ܣ����ɹ�Կ���ܵ�PINת����ANSI9.8��׼

   ������  ����    ����    ˵����
   �������        2       A       ֵ"HD"
   �㷨��ʶ        1       N       0
   ˽Կ����        2       N       ָ����˽Կ�����ڽ���PIN��������
   ��䷽ʽ        1       N       0       1
   ԴPIN��ʽ       2       N       07����ũ����ר��
   Ŀ��PIN��ʽ     2       N       01 ANSI9.8
   ��Կ����        3       N       001 ZPK
   ZPKֵ           16/32+A H       ���ڼ���PIN
   ԭ�˺�          12      N       ��ԴPIN��ʽΪ01ʱ����
   Ŀ���˺�        12      N       ��Ŀ��PIN��ʽΪ01ʱ����
   PIN���ĳ���     4       N
   PIN����         n       B       ����Կ���ܵ�PIN��������


   �����  ����    ����    ˵����
   ��Ӧ����        2       A       ֵ"HE"
   �������        2       H
   PIN����         16      H       ���ص�PIN����--ANSI9.8
 */
int UnionHsmCmdHD(char *AlgorithmMode,char *vkIndex,char *DataPadFlag,char *FirstPinCiperFormat,char *SecondPinCiperFormat,
		char *zpk, char *conEsscFldAccNo1, char *conEsscFldAccNo2, int lenOfPinByPK,char *pinByPK,int lenOfVK,char *vk, char *pinByZPK);

/**
  add by leipp 20151111

  ���ܣ���������pin(�Ĵ�ũ��)

  ������   	��������    ˵����
  �������        2A      	ֵ"SP"
  mode		1N      	ģʽ��־,1:���ܴ���PIN
5:���ܿ�PIN
pin		6H		pin����,6λ����
sizeOfEncPin	nN		�������ĳ���

�����  	��������	˵��
��Ӧ����        2A       	ֵ"SQ"
encPin		16H

 **/
int UnionHsmCmdSP(int mode, char *pin, char *encPin, int sizeOfEncPin);

/**
  add by leipp 20151111

  ����:����׼PinBlockתΪ�����㷨���ܵ�Pin

  ������   	��������    ˵����
  �������        2A      	ֵ"SN"
  mode		1N      	ģʽ��־,1:���ܴ���PIN
5:���ܿ�PIN
keyValue	16H/32H/	ZPK��Կֵ
1A+32H/1A+48H
pinType		2A		01��ANSIx9.8��ʽ
accout		12H		PIN�˺�
pinBlock	16H		PIN����,ZPK�¼��ܵ�PIN���ģ�����pin��ʽ��6λ����
sizeOfEncPin	nN		�������ĳ���

�����  	��������	˵��
��Ӧ����        2A       	ֵ"SO"
encPin		16H

 **/

int UnionHsmCmdSN(int mode, char *keyValue, char *pinType, char *accout, char *pinBlock, char *encPin, int sizeOfEncPin);

/*
   add by leipp 20151111
   ����:	�����Ĵ�ũ��˽���㷨��Կ
   ������	��Կ��3���ɷ���ɣ�ÿ���ɷݲ���8���ֽ�ʱ����0.
   �ڼ��ܻ��ڿ���ר�ŵĴ洢���洢�����ٴ�������

   ������   	��������    ˵����
   �������        2A      	ֵ"SJ"
   key1		16H		LMK0607�¼���
   key2		16H		LMK0607�¼���
   key3		16H		LMK0607�¼���

   �����  	��������	˵��
   ��Ӧ����        2A       	ֵ"SK"

 */
int UnionHsmCmdSJ(char *key1, char *key2, char *key3);

/*
   �������ܣ�
   XSָ���֤VISA����CVV
   ���������
   cvv������֤��VISA����CVV
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   errCode���������
   ����ֵ��
   <0������ִ��ʧ��
   0����֤�ɹ�
 */

int UnionHsmCmdXS(char *cvv,char *cardValidDate, int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode);

/*
   �������ܣ�
   SHָ�����VISA��У��ֵCVV
   ���������
   cardValidDate��VISA������Ч��
   cvkLength��CVK�ĳ���
   cvk��CVK
   accNo���ͻ��ʺ�
   lenOfAccNo���ͻ��ʺų���
   ���������
   cvv�����ɵ�VISA����CVV
   ����ֵ��
   <0������ִ��ʧ�ܣ���cvv
   >0���ɹ�������cvv�ĳ���
 */

int UnionHsmCmdSH(char *cardValidDate,int cvkLength,char *cvk, char *accNo,int lenOfAccNo,char *serviceCode,char *cvv);

/*
   �������ж���ָ��

   �������ܣ�
   ��Կ����PINת��DES���ܣ�֧���ַ��������㷨��

   ���������

   algorithmID	  		�㷨��ʾ		1N	  	1��RSA	   2��SM2
   vkIndex			˽Կ����    		2N   		"00"��"20"  	"99"��ʹ�������Կ
   lenOfVK   			���˽Կ����		4N		������Կ����Ϊ99ʱ�д�����һ���򳤶�		
   vk				���˽Կ
   fillMode			�ù�Կ����ʱ��		1N 	  	���㷨��ʾΪ��RSAʱ�д���		
   ���õ���䷽ʽ					
   ZEKValue			zekֵ		
   format			��λ��ʽ		1N   		1�����0x00��    2�����0x80��
   encrypMode			����ģʽ		1N   		1:ECB	 2:CBC
   dataPrefixLen		�������ǰ׺����	2N		����ǣ�00����û��ǰ׺�������
   dataPrefix			�������ǰ׺		3H		��������ݳ���ǰ׺׺����00ʱ�д���
   dataSuffixLen		������ݺ�׺����	2N		����ǣ�00����û�к�׺�������
   dataSuffix			������ݺ�׺		3H		��������ݺ�׺���Ȳ���00ʱ�д���
   lenOfPinByPK		���ĳ���			4N
   PinByPK			��Կ��������   		nB   		����Կ���ܵ�PIN��������SM2�㷨Ϊc1+c3+c2

   ���������
   PinByZek			PIN����			nH		zek���ܵ����ݳ���


 */
int UnionHsmCmd6A(int algorithmID,int algorithmID_ZEK,int vkIndex,int lenOfVK,char *vk,int fillMode,char *specialAlg,char *ZEKValue,int format,int encrypMode,int lenOfIV,char *IV,int dataPrefixLen,char *dataPrefix,int dataSuffixLen,char *dataSuffix,int lenOfPinByPK,char *PinByPK,char *PinByZek,char *complexityOfPin);

/*
   �ൺ���ж���ָ��

   ����: 
   ������SM2�㷨��Կ����ת����ΪSM4�㷨���ܡ�

   ���������
   vkIndex ˽Կ����        2N
   lenOfvk ˽Կ����        4N
   vk              ˽Կ����        nB
   keyType �Գ���Կ����    3H
   keyValue        �Գ���Կ        1A+32H
   lenOfdata       ���ݳ���        4N
   data            ������  nB

   ���������
   cipherData      �Գ���Կ���ܺ������    n*2

   ����ֵ��
   <0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
   >=0������ִ�гɹ�
 */     

int UnionHsmCmdPR(int vkIndex, int lenOfvk, char *vk,TUnionDesKeyType keyType,char *keyValue,int lenOfdata,char *data,char *cipherData);
/*��lmk������Կת��Ϊpk����
  ���������	keyType ��Կ����
  keyValue LMK��Կ��������
  checkValue У��ֵ
  pkIndex  ��Կ����
  lenOfPK  ��Կ����
  pk ��Կֵ
  �������
  lenOfKeyByPK PK���ܵ���Կֵ����
  keyByPK  PK���ܵ���Կֵ	
  pkHash   ��Կ����HASH
  ����ֵ��        PK���ܵ���Կֵ����       */
int UnionHsmCmdWN(char *keyType, char* keyValue, char* checkValue,int pkIndex,int lenOfPK,char *pk,int* lenOfKeyByPK,char* keyByPK,char* pkHash);
/*
   ��ZPK���ܻ���PINתΪ����ר���㷨�õ�PIN�������
   ���������
   zpkKeyValue	16H��1A+32H��1A+48H	ԴZPK	 	��ǰ����PIN���ZPK��LMK�ԣ�06-07���¼���
   pinBlock	16H			ԴPIN��		ԴZPK�¼��ܵ�ԴPIN��
   pinFormat	2N			ԴPIN���ʽ
   accout		12N			�˺�		�˺���ȥ��У��λ������12λ
   resPinFormat	2N			Ŀ��PIN���㷨	01������ũ����ר���㷨	02��̫��ũ����ר���㷨	03���Ͼ�����ר���㷨
   parameter	8A/12A	�1�7		���д������	8A̫������ 12A�Ͼ�����
   ���������
   resPinBlock	16H	Ŀ��PIN��
 */
int UnionHsmCmdS5(char* zpkKeyValue, char* pinBlock, char* pinFormat,char* accout,char* resPinFormat,char* parameter,char* resPinBlock);
/*
   ���Ͼ�����ר���㷨��PIN����תΪX9.8��ʽ��PIN�������
   ���������
   parameter       12A                     ���д������
   resPinFormat    2N                      Ŀ��PIN���㷨     01��ANSI9.8
   zpkKeyValue     16H��1A+32H��1A+48H     Ŀ��ZPK           LMK�ԣ�06-07���¼���
   accout          12N                     �˺�              �˺���ȥ��У��λ������12λ
   pinBlock        16H                     Pin������         �Ͼ�����ר���㷨����
   ���������
   resPinBlock     16H                     Ŀ��PIN��
 */
int UnionHsmCmdSR(char* parameter, char* resPinFormat, char* zpkKeyValue, char* accout, char* pinBlock, char* resPinBlock);

// add by leipp 20151228
/*
   ��¼�����ʼ������SM2��Կ���ܵ����ģ�ת����SM3/ZPK�����㷨�������
   ���������
   mode		1A			1-	����PIN������ӣ�SM3ժҪ���
   2-	���PIN������ӣ�SM3ժҪ���
   3-	SM2��Կ����תΪZPK��SM4�㷨���������
   vkIndex		2N			00-20��99��ʾSM2���
   vkLen		4N			������Կ����Ϊ99ʱ�д���
   vkValue     	nB			������Կ����Ϊ99ʱ�д���SM2��Կ����
   lenOfPinByPK	4N			���ĳ���
   pinByPK		nB			SM2��Կ���ܵ����ġ���C1+C3+C2��
   factorData	8H			���ط����ӣ����ⲿ���룬˽Կ�������ĺ󣬻���������ģ��ɴ������ӱȶ�
   keyValue	1A+32H			PIN���������Կ/ZPK, PIN���������PIN���Ľ����ϳ�PINBLOCK����������ɼ��ܻ��������32λ16��������
   ��ģʽΪ1��2��Կ����ΪZEK��SM4��Կ����
   ��ģʽΪ3ΪZPK(SM4��Կ)��
   randomData	32H			PIN�����������
   lenOfAccNo	2N			�˺ų���
   accNo		nN			�˺�

   ���������
   pinByZEK        32H                     ��ģʽΪ1����ZEK���ܵ�pin����������ġ�
   digest		64H			SM3�㷨ժҪ����ģʽ��־Ϊ1��2�д���
   pinBlock	32H			��ģʽΪ3��Zpk���ܵ�PINBLOCK���ģ�ANSI 9.8��ʽ����
 */

int UnionHsmCmd8B(int mode, int vkIndex,int vkLen,char *vkValue,int lenOfPinByPK, char *pinByPK, char *factorData, char *keyValue, char *randomData, int lenOfAccNo,char *accNo, char *pinByZEK,char *digest, char *pinBlock);

/*
   ƽ������	PHָ��

   �������ܣ�
   ��SM2��Կ���ܻ�������ת��Ϊר���㷨����

   ���������
   ID			�㷨ID		1A			   1-  ��SM2��Կ���ܵ�����תΪ��ҵ������¼����ר���㷨����
   2-  ��SM2��Կ���ܵ�����תΪ����������¼����ר���㷨����
   3-  ��SM2��Կ���ܵ�����תΪMD5�㷨����
   4-  ��SM2��Կ���ܵ�����תΪZPK����
   vkIndex		˽Կ����	 	2N			   00-20��99��ʾSM2���
   lenOfVK		˽Կ����		4N			   ������Կ����Ϊ99ʱ�д�����һ���򳤶�
   vk			˽Կ����		nB			   ������Կ����Ϊ99ʱ�д���SM2��Կ����
   lenOfPinByPK	���ĳ���		4N			
   PinByPK		����			nB			   SM2��Կ���ܵ����ġ���C1+C3+C2��
   ZPKValue		zpk			1A+16 H /32 H /48 H		��ID=1��4ʱ�и���DES�㷨ZPK
   lenOfaccNO	�ʺų��Ȼ��߸�����Ϣ����	2N			��ID=1ʱ�и���4~20
   accNO		�ʺţ�PAN�����߸�����Ϣ		N			��ID=1ʱ�и����û����ʺŻ��߸�����Ϣ����Ϊȫ"0"������򲻲������PIN������
   ��ID=4ʱ�и����û���Ч�˺�12λ

   ���������
   referFactor	�ط�����	
   PinBlock		pin����					��ID=1ʱ����ҵ������¼����ר���㷨�������� 32H
   ��ID=2ʱ������������¼����ר���㷨�������� 28B
   ��ID=3ʱ��MD5�㷨�������� 				 32H
   ��ID=4ʱ��ZPK���ܵ����ģ�ZPK���ܣ�DES�㷨�������ģ�ANSI 9.8��ʽ 16H
 */

int UnionHsmCmdPH(int ID,int vkIndex,int lenOfVK,char *vk,int lenOfPinByPK,char *PinByPK,char *ZPKValue,int lenOfaccNO,char *accNO,char *referFactor,char *PinBlock);	
	
int UnionHsmCmdWL(char *keytype,char *zmkvalue,char *zmkcheckvalue,char *zmkData,char *outkey,char *outcheckvalue);

int UnionHsmCmd98(char *zmkKey, int keyFlag, char *bankID, char *PSAMID, char *random, char *keyValue);

/*
   ��ZPK���ܵ�pinBlockת����MD5��SHA-1��SM3
   ���������
   algorithmID          1N                      PIN���㷨
                                                1�� DES/3DES
                                                2�� SM4
   keyType              3H                      ��Կ����
                                                001�� ZPK
                                                002�� TPK
   keyValue             1A+3H/16H/32H/48H       ��Կֵ
   checkValue           16H                     У��ֵ����ΪSM4�㷨ʱ�и���
   format               2N                      PIN���ʽ
   pinBlock             16H/32H                 ��Կ���ܵ�PIN��
   accNo                N                       �˺�
   hashID               2N                      ��ϣ�㷨��ʶ
   dataOffset           4N                      ����ƫ��
   length               4N                      ���γ���
   data                 nH                      ��������
   ���������
   hash                 nH                      ��ϣ
 */
int UnionHsmCmdZW(int algorithmID, char *keyType, char *keyValue, char *checkValue, char *format, char *pinBlock, char *accNo, int lenOfAccNo, char *hashID, int dataOffset, int length, char *data, char *hash, int sizeOfHash);

// add begin by zhouxw 20160419
int UnionHsmCmdW1(char *algoriAthmID, char *vkIndex, TUnionDesKeyType dstKeyType, char *keyValue, int dataFillMode, int cipherDataLen, char *cipherData, int lenOfUserName, char *userName, char *pinBlock, int sizeOfPinBlock);

int UnionHsmCmdW3(char *srcZPK, TUnionDesKeyType dstKeyType, char *dstKeyValue, int maxPinLen, char *srcPinBlock, char *format, char *accNo, int lenOfUserName, char *userName, char *dstPinBlock);

int UnionHsmCmdH6(char *srcZPK, int lenOfPin, char *pin, int lenOfPan, char *pan, int lenOfPinBlock, char *pinBlock);

int UnionHsmCmdY8(char *srcZPK, int lenOfPin, char *pin, int lenOfPan, char *pan, int lenOfPlainPin, char *plainPin);
// add end by zhouxw 20160419
int UnionHsmCmdS9(char *keyIndex,int vkLen ,char *vkValue,int lenOfPinByPK,char *pinByPK,int dataFillMode,int DataFillHeadLen,char *DataFillHead,int DataFillTailLen,char *DataFillTail,char *hash, int sizeofHash);

//�ű����ݼ������EMV 4.1/PBOC��
/*�������      
  mode                  ģʽ
  id                    ����ID
  mkIndex                       MK-SMC����
  mk                    MK-SMC
  checkMk                       MK-SMCУ��ֵ
  pan                   PAN/PAN���к�
  atc                   ATC
  iv                    ģʽ��־λ0ʱ�д���
  lenOfPlainData                �������ݳ���
  plainData             ��������
  �������
  lenOfCipherData               �������ݳ���
  cipherData            ��������
 */     
int UnionHsmCmdWB(char *mode,char *id,int mkIndex,char *mk,char *checkMk,char *pan,char *atc,char *iv,int lenOfPlainData,char *plainData,int *lenOfCipherData,char *cipherData);
#endif
