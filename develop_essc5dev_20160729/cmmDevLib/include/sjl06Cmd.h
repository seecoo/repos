// Author:	Wolfgang Wang
// Date:	2003/10/09

#include "sjl06.h"
#include "unionDesKey.h"

#ifndef _SJL06Cmd_
#define _SJL06Cmd_

#ifndef _Use_SJL06Server_
#ifdef _SJL06CmdForJK_IC_RSA_

/*
2007/4/11,������������
50ָ��
���ܣ���EDK��Կ�ӽ������ݣ�����ǽ���״̬�����������Ȩ�²��ܴ������򱨴�

��  ��  ��  Ϣ  ��  ʽ

������		����	����	˵����
��Ϣͷ		m	A	
�������	2	A	ֵ"50"
Flag		1	N	0������
			1������
EDK		16 or 32
		1A+32 or 1A+48	
			H	LMK24-25����
DATA_length	4	N	���������ֽ�����8�ı�������Χ��0008-4096
DATA		n*2	H	�����ܻ���ܵ����ݣ���BCD���ʾ��

��  ��  ��  Ϣ  ��  ʽ

�����	����	����	˵����
��Ϣͷ		M	A	
��Ӧ����	2	A	ֵ"51"
�������	2	N	
DATA		n*2	H	������

*/
int RacalCmd50(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *edk,int lenOfData,char *indata,char *outdata,int sizeOfOutData,char *errCode);

// 20051206������������
// ��ZMK���ܵ���Կת��ΪLMK���ܵ�ZAK/ZEK
int RacalCmdFK(int hsmSckHDL,PUnionSJL06 pSJL06,char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue,char *errCode);
/*
��  ��  ��  Ϣ  ��  ʽ
������	����	����	˵����
�������		2A	ֵΪ"TI"��
ԴKEY��־	1	N	1��TPK           2��ZPK
ԴKEY	16
1A+32
1A+48	H	��KEY��־Ϊ1ʱΪTPK��LMK�ԣ�14-15���¼��ܣ�
��KEY��־Ϊ2ʱΪZPK��LMK�ԣ�06-07���¼��ܡ�
Ŀ��KEY��־	1	N	1��TPK           2��ZPK
Ŀ��KEY	16
1A+32
1A+48	H	��KEY��־Ϊ1ʱΪTPK��LMK�ԣ�14-15���¼��ܣ�
��KEY��־Ϊ2ʱΪZPK��LMK�ԣ�06-07���¼��ܡ�
ԴPIN��	16	H	ԴZPK�¼��ܵ�ԴPIN�顣
ԴPIN��ʽ	1	N	1��ANSI9.8��ʽ    2��IBM��ʽ
ԴPAN	16	H	�û����ʺţ����õ�����12λ; ��ԴPIN��ʽΪ1ʱ�д���
Ŀ��PIN��ʽ	1	N	1��ANSI9.8��ʽ    2��IBM��ʽ
Ŀ��PAN	16	H	�û����ʺţ����õ�����12λ; ��ԴPIN��ʽΪ1ʱ�д���

��  ��  ��  Ϣ  ��  ʽ
�����	����	����	˵����
��Ӧ����	2	A	ֵ"TJ"
�������	2	H	
PIN����	16	H	TPK��ZPK�¼���
*/
int RacalCmdTI(int hsmSckHDL,PUnionSJL06 pSJL06,char zpk1type,int zpk1Length,char *zpk1,char zpk2type,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo1,int lenOfAccNo1,char *accNo2,int lenOfAccNo2,
		char *pinFormat2,char *pinBlockByZPK2,char *errCode);

// ��ZMK���ܵ�ZPKת��ΪLMK����
int RacalCmdFA(int hsmSckHDL,PUnionSJL06 pSJL06,char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue,char *errCode);
// RSAָ��
int SJL06Cmd3E(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk,char *PK,char *keyByBMK,char *checkValue,char *keyByPK,char *errCode);

int SJL06Cmd34(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,int lenOfVK,char *pk,int sizeOfPK,char *errCode);
int SJL06Cmd35(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int lenOfVKByMK,char *errCode);
int SJL06Cmd36(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd37(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int dataLen,char *data,char *signature,int sizeOfSignature,char *errCode);
int SJL06Cmd38(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk,char *errCode);
int SJL06Cmd36(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd40(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmkForPVK,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK,char *errCode);
int SJL06Cmd41(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode);
int SJL06Cmd42(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText,char *errCode);
int SJL06Cmd3C(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,int lenOfData,char *data,char *hash,char *errCode);

// Mary add begin, 2008-9-23
/*
�������ܣ�
	33ָ�˽Կ����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	flag���ù�Կ����ʱ�����õ���䷽ʽ��
		'0'��������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
		'1'��PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
	vkIndex��˽Կ������"00"��"20"
	cipherDataLen���������ݵ��ֽ���
	cipherData�����ڽ��ܵ���������
	sizeOfPlainData��plainData���ڵĴ洢�ռ��С
���������
	plainData�����ܵõ�����������
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���plainData
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������plainData�ĳ���
*/
int SJL06Cmd33(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int cipherDataLen,char *cipherData,char *plainData,int sizeOfPlainData,char *errCode);
// Mary add end, 2008-9-23

// �㷢MPר��ָ��
int SJL06Cmd71(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// ���ؽ��ܺ�����ݵĳ���
int SJL06Cmd72(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		int pinOffset1,int pinLen1,int pinOffset2,int pinLen2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);
// ���ؼ��ܺ����ݵĳ���
int SJL06Cmd74(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd75(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *mac,char *errCode);
int SJL06Cmd76(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int macDataLen,char *macData,
		int dataLen,char *data,char *errCode);
int SJL06Cmd77(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// ���ؼ��ܺ����ݵĳ���
int SJL06Cmd7C(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,
		char *version2,char *group2,char *index2,
		char *lsdata,char *gcdata,
		int macDataHeadLen,char *macDataHead,
		int dataLen,char *data,
		int offset,
		char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd7D(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *lsData,
		char *errCode);
		
// ���ؽ��ܺ�����ݵĳ���
int SJL06Cmd73(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		char seprator,int pinFld1,int pinFld2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);

// ͨ��IC��ָ��
int SJL06CmdE2(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);

int SJL06CmdE0(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdE4(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *errCode);
int SJL06CmdE6(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD0(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD2(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD4(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *errCode);
int SJL06CmdD6(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06Cmd10(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version ,char *group,char *index,char *lsData,char *errCode);
int SJL06Cmd12(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *gcData,char *errCode);
int SJL06Cmd14(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *tmkIndex,char *txKey,char *errCode);
int SJL06Cmd20(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd22(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd24(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd26(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *keyType,char *macMode,char *tmkIndex,char *txKey,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06CmdC0(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *tmkIndex,char *inputData,char *outputData,char *errCode);
int SJL06CmdC2(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *inputData,char *outputData,char *errCode);
int SJL06Cmd50(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd52(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd54(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength1,char *macData1,char *mac1,
	       int dataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd56(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd58(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd70(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *enFlag,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *inputData,char *mac,char *outputData,char *errCode);
int SJL06CmdC4(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *enFlag,char *tmkIndex,char *txKey,
	       int dataLength,char *inputData,char *outputData,char *errCode);


// ͨ�ô�����ָ��

#ifdef _Suport3Des_
int SJL06Cmd1A(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd13(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd31(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen1,TUnionDesKeyLength keyLen2,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#else
int SJL06Cmd1A(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd13(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd31(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(int hsmSckHDL,PUnionSJL06 pSJL06,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#endif
int SJL06Cmd15(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue1,char *keyValue2,char *errCode);


int SJL06Cmd1B(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd1C(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue1,char *variant,char *keyValue2,char *errCode);
int SJL06Cmd21(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd32(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd62(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *clearPin,char *pan,char *enPin,char *errCode);
int SJL06Cmd63(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd64(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd65(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pik,char *enPin1,char *pan,char *tmkIndex,char *enPin2,char *errCode);
int SJL06Cmd67(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan,char *enPin2,char *errCode);
int SJL06Cmd69(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan1,char *pan2,char *enPin2,char *errCode);
int SJL06Cmd84(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *mak,char *enPin1,char *pan,char *addInfo,int macDataLength,char *macData,char *enPin2,char *mac,char *errCode);
int SJL06Cmd85(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *mak1,char *bmkIndex2,char *mak2,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd86(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *mak1,char *bmkIndex2,char *pik2,char *mak2,char *enPin1,char *pan,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *enPin2,char *mac2,char *errCode);
int SJL06Cmd01(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode);
int SJL06Cmd03(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *errCode);
int SJL06Cmd04(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *errCode);
int SJL06Cmd90(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd91(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd92(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd93(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd7A(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,int dataLength,char *flag,char *data,char *enData,char *errCode);

#endif // for _SJL06CmdForJK_IC_RSA_

#ifdef _RacalCmdForNewRacal_
#include "3DesRacalCmd.h"
#include "3DesRacalSyntaxRules.h"

/*
�ߡ�	ת��DES��Կ��������Կ���ܵ���Կ����
���ڷַ���Կ��

������	����	����	˵����
�������	2	A	ֵ"3B"
��Կ����	32	H	������Կ���ܵ�DES��Կ
����Կ	n	B	


�����	����	����	˵����
��Ӧ����	2	A	"3C"
�������	2	H	
���ֵ	16	H	DES��Կ����64bits��0
��Կ����	4	N	DES��Կ���ĵ��ֽ���
��Կ����	n	B	�ù�Կ���ܵ�DES��Կ
*/
int SJL06Cmd3B(int hsmSckHDL,PUnionSJL06 pSJL06,char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue,char *errCode);

/*
����	ת��DES��Կ���ӹ�Կ���ܵ�����Կ����
���ڽ�����Կ��

������	����	����	˵����
�������	2	A	ֵ"3A"
˽Կ����	2	N	"00"��"20"����������ڵ�˽Կ
��Կ����	4	N	DES��Կ���ĵ��ֽ���
��Կ����	n	B	�ù�Կ���ܵ�DES��Կ


�����	����	����	˵����
��Ӧ����	2	A	"3B"
�������	2	H	
DES��Կ	32	H	������Կ���ܵ�DES��Կ
���ֵ	16	H	DES��Կ����64bits��0
*/
int SJL06Cmd3A(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue,char *errCode);

int SJL06Cmd3E(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk,char *pk,char *keyByBMK,char *checkValue,char *keyByPK,char *errCode);
int SJL06Cmd3C(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,int lenOfData,char *data,char *hash,char *errCode);
int SJL06Cmd38(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk,char *errCode);

int SJL06Cmd37(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int dataLen,char *data,char *signature,int sizeOfSignature,char *errCode);
int SJL06Cmd34(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,int lenOfVK,char *pk,int sizeOfPK,char *errCode);
int SJL06Cmd40(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK,char *errCode);
int SJL06Cmd41(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode);
int SJL06Cmd42(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText,char *errCode);

// ��������MAC
int RacalCmdMU(int hsmSckHDL,PUnionSJL06 pSJL06,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);
int RacalCmdA0(int hsmSckHDL,PUnionSJL06 pSJL06,
	int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,
	char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue,char *errCode);

// ��һ��ZMK���ܵ���Կת��ΪLMK����
int RacalCmdA6(int hsmSckHDL,PUnionSJL06 pSJL06,
	TUnionDesKeyType keyType,char *zmk,char *keyByZmk,
	char *keyByLmk,char *checkValue,char *errCode);

int RacalCmd0A(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode);
int RacalCmdRA(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode);
int RacalCmdPA(int hsmSckHDL,PUnionSJL06 pSJL06,char *format,char *errCode);
int RacalCmdA2(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode);

// ת��PIN
int RacalCmdCC(int hsmSckHDL,PUnionSJL06 pSJL06,int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2,char *errCode);

// ����MAC
int RacalCmdMS(int hsmSckHDL,PUnionSJL06 pSJL06,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);

// Mary add begin, 2004-3-11
#define gPVKI				"0"
//#define gServiceCode			"000"
//#define gIBMDecimalizationTable	"0123456789012345"
//#define gIBMDefaultPinOffset		"FFFFFFFFFFFFFFFF"


/*
�������ܣ�
	DGָ���PVK����PIN��PVV(PIN Verification Value)��
	���õļ��ܱ�׼ΪVisa Method
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinLength��LMK���ܵ�PIN���ĳ���
	pin��LMK���ܵ�PIN����
	pvkLength��LMK���ܵ�PVK�Գ���
	pvk��LMK���ܵ�PVK��
	lenOfAccNo���ͻ��ʺų���
	accNo���ͻ��ʺ�
���������
	pvv��������PVV
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���PVV
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������PVV�ĳ���
*/
int RacalCmdDG(int hsmSckHDL,PUnionSJL06 pSJL06,int pinLength,char *pin,int pvkLength,char *pvk,\
		int lenOfAccNo,char *accNo,char *pvv,char *errCode);

/*
�������ܣ�
	CAָ���һ��TPK���ܵ�PINת��Ϊ��ZPK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByZPK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByZPK�ĳ���
*/
int RacalCmdCA(int hsmSckHDL,PUnionSJL06 pSJL06,int tpkLength,char *tpk,int zpkLength,\
		char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK,char *errCode);

/*
�������ܣ�
	JEָ���һ��ZPK���ܵ�PINת��Ϊ��LMK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinFormat��PIN��ʽ
	pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdJE(int hsmSckHDL,PUnionSJL06 pSJL06,int zpkLength,char *zpk,\
		char *pinFormat,char *pinBlockByZPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	JCָ���һ��TPK���ܵ�PINת��Ϊ��LMK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	tpkLength��LMK���ܵ�TPK����
	tpk��LMK���ܵ�TPK
	pinFormat��PIN��ʽ
	pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdJC(int hsmSckHDL,PUnionSJL06 pSJL06,int tpkLength,char *tpk,\
		char *pinFormat,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	JGָ���һ��LMK���ܵ�PINת��Ϊ��ZPK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinFormat��PIN��ʽ
	pinLength��LMK���ܵ�PIN���ĳ���
	pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByZPK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByZPK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByZPK�ĳ���
*/
int RacalCmdJG(int hsmSckHDL,PUnionSJL06 pSJL06,int zpkLength,char *zpk,\
		char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,\
		int lenOfAccNo,char *pinBlockByZPK,char *errCode);

/*
�������ܣ�
	JAָ��������һ��PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinLength��Ҫ��������ɵ�PIN���ĵĳ���
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK�����������PIN�����ģ���LMK����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdJA(int hsmSckHDL,PUnionSJL06 pSJL06,int pinLength,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	EEָ���IBM��ʽ����һ��PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdEE(int hsmSckHDL,PUnionSJL06 pSJL06,int minPINLength,char *pinValidData,\
		char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,\
		char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	DEָ���IBM��ʽ����һ��PIN��Offset
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinOffset
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinOffset�ĳ���
*/
int RacalCmdDE(int hsmSckHDL,PUnionSJL06 pSJL06,int minPINLength,char *pinValidData,\
		char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,\
		char *pvk,char *accNo,int lenOfAccNo,char *pinOffset,char *errCode);

/*
�������ܣ�
	BAָ�ʹ�ñ�������Կ����һ��PIN����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinCryptogramLen�����ܺ��PIN���ĵĳ���
	pinTextLength��PIN���ĵĳ���
	pinText��PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdBA(int hsmSckHDL,PUnionSJL06 pSJL06,int pinCryptogramLen,int pinTextLength,\
		char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	NGָ�ʹ�ñ�������Կ����һ��PIN����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinCryptogramLen��PIN���ĵĳ���
	pinCryptogram��PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	referenceNumber����LMK18-19�����ʺŵõ���ƫ��ֵ
	pinText��PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinText
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinText�ĳ���
*/
int RacalCmdNG(int hsmSckHDL,PUnionSJL06 pSJL06,int pinCryptogramLen,char *pinCryptogram,\
		char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText,char *errCode);

/*
�������ܣ�
	DAָ���IBM��ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdDA(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,\
		char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	EAָ���IBM��ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdEA(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,\
		char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	DCָ���VISA��ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdDC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,\
		char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	ECָ���VISA��ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdEC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,\
		char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	BCָ��ñȽϷ�ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByTPK��TPK���ܵ�PIN����
	tpkLength��LMK���ܵ�TPK
	tpk��LMK���ܵ�TPK
	pinByLMKLength������PIN���ĳ���
	pinByLMK������PIN���ģ���LMK02-03����
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdBC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,\
		int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	BEָ��ñȽϷ�ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByZPK��ZPK���ܵ�PIN����
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinByLMKLength������PIN���ĳ���
	pinByLMK������PIN���ģ���LMK02-03����
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdBE(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,\
		int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	CWָ�����VISA��У��ֵCVV 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	cardValidDate��VISA������Ч��
	cvkLength��CVK�ĳ���
	cvk��CVK
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	cvv�����ɵ�VISA����CVV
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���cvv
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������cvv�ĳ���
*/
int RacalCmdCW(int hsmSckHDL,PUnionSJL06 pSJL06,char *cardValidDate,int cvkLength,char *cvk,\
		char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,char *errCode);

/*
�������ܣ�
	CYָ���֤VISA����CVV 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdCY(int hsmSckHDL,PUnionSJL06 pSJL06,char *cvv,char *cardValidDate,\
		int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *errCode);

/*
�������ܣ�
	EWָ���˽Կǩ�� 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	indexOfVK��˽Կ������
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	vkLength��LMK���ܵ�˽Կ����
	vk��LMK���ܵ�˽Կ
���������
	signature�����ɵ�ǩ��
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���signature
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������signature�ĳ���
*/
//int RacalCmdEW(int hsmSckHDL,PUnionSJL06 pSJL06,char *indexOfVK,int signDataLength,
//		char *signData,int vkLength,char *vk,char *signature,char *errCode);

/*
�������ܣ�
	EYָ��ù�Կ��֤ǩ��
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	macOfPK����Կ��MACֵ
	signatureLength������֤��ǩ���ĳ���
	signature������֤��ǩ��
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	publicKeyLength����Կ�ĳ���
	publicKey����Կ
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
//int RacalCmdEY(int hsmSckHDL,PUnionSJL06 pSJL06,char *macOfPK,int signatureLength,
//		char *signature,int signDataLength,char *signData,int publicKeyLength,
//		char *publicKey,char *errCode);
// Mary add end, 2004-3-11

// Mary add begin, 2004-3-24
/*
�������ܣ�
	BUָ�����һ����Կ��У��ֵ
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	key��LMK���ܵ���Կ����
���������
	checkValue�����ɵ���ԿУ��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������checkValue�ĳ���
*/
int RacalCmdBU(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode);

/*
�������ܣ�
	A4ָ��ü�����Կ�����ĳɷֺϳ�һ����Կ��������У��ֵ
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	partKeyNum����Կ�ɷֵ�����
	partKey����Ÿ�����Կ�ɷֵ����黺�壬ΪLMK���ܵ���Կ����
���������
	keyByLMK���ϳɵ���Կ���ģ���LMK����
	checkValue���ϳɵ���Կ��У��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���keyByLMK��checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������keyByLMK��checkValue���ܳ���
*/
int RacalCmdA4(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode);
// Mary add end, 2004-3-24

/*
�������ܣ�
	A5ָ�������Կ���ĵļ����ɷݺϳ����յ���Կ��ͬʱ����Ӧ��LMK��Կ�Լ���
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	partKeyNum����Կ�ɷֵ�����
	partKey����Ÿ�����Կ�ɷֵ����黺��
���������
	keyByLMK���ϳɵ���Կ���ģ���LMK����
	checkValue���ϳɵ���Կ��У��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���keyByLMK��checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������keyByLMK��checkValue���ܳ���
*/
int RacalCmdA5(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode);

int RacalCmdA8(int hsmSckHDL,PUnionSJL06 pSJL06,
	TUnionDesKeyType keyType,char *key,char *zmk,
	char *keyByZMK,char *checkValue,char *errCode);
#endif // for _RacalCmdForNewRacal_

#endif // ifndef _Use_SJL06Server_


#ifdef _Use_SJL06Server_

#include "unionSJL06API.h"

#ifdef _SJL06CmdForJK_IC_RSA_

// ��ZMK���ܵ�ZPKת��ΪLMK����
int RacalCmdFA(PUnionSJL06Server psjl06Server,char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue,char *errCode);
// RSAָ��
int SJL06Cmd3E(PUnionSJL06Server psjl06Server,char *bmk,char *PK,char *keyByBMK,char *checkValue,char *keyByPK,char *errCode);
int SJL06Cmd34(PUnionSJL06Server psjl06Server,char *vkIndex,int lenOfVK,char *pk,int sizeOfPK,char *errCode);
int SJL06Cmd35(PUnionSJL06Server psjl06Server,char *vkIndex,char *vkByMK,int lenOfVKByMK,char *errCode);
int SJL06Cmd36(PUnionSJL06Server psjl06Server,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd37(PUnionSJL06Server psjl06Server,char flag,char *vkIndex,int dataLen,char *data,char *signature,int sizeOfSignature,char *errCode);
int SJL06Cmd38(PUnionSJL06Server psjl06Server,char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk,char *errCode);
int SJL06Cmd36(PUnionSJL06Server psjl06Server,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd40(PUnionSJL06Server psjl06Server,char *vkIndex,char *bmkForPVK,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK,char *errCode);
int SJL06Cmd41(PUnionSJL06Server psjl06Server,char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode);
int SJL06Cmd42(PUnionSJL06Server psjl06Server,char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText,char *errCode);
int SJL06Cmd3C(PUnionSJL06Server psjl06Server,char flag,int lenOfData,char *data,char *hash,char *errCode);

// �㷢MPר��ָ��
int SJL06Cmd71(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// ���ؽ��ܺ�����ݵĳ���
int SJL06Cmd72(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		int pinOffset1,int pinLen1,int pinOffset2,int pinLen2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);
// ���ؼ��ܺ����ݵĳ���
int SJL06Cmd74(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd75(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *mac,char *errCode);
int SJL06Cmd76(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int macDataLen,char *macData,
		int dataLen,char *data,char *errCode);
int SJL06Cmd77(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// ���ؼ��ܺ����ݵĳ���
int SJL06Cmd7C(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,
		char *version2,char *group2,char *index2,
		char *lsdata,char *gcdata,
		int macDataHeadLen,char *macDataHead,
		int dataLen,char *data,
		int offset,
		char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd7D(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *lsData,
		char *errCode);
		
// ���ؽ��ܺ�����ݵĳ���
int SJL06Cmd73(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		char seprator,int pinFld1,int pinFld2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);

// ͨ��IC��ָ��
int SJL06CmdE2(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);

int SJL06CmdE0(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdE4(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *errCode);
int SJL06CmdE6(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD0(PUnionSJL06Server psjl06Server,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD2(PUnionSJL06Server psjl06Server,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD4(PUnionSJL06Server psjl06Server,char *tmkIndex,char *errCode);
int SJL06CmdD6(PUnionSJL06Server psjl06Server,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06Cmd10(PUnionSJL06Server psjl06Server,char *desType,char *version ,char *group,char *index,char *lsData,char *errCode);
int SJL06Cmd12(PUnionSJL06Server psjl06Server,char *desType,char *gcData,char *errCode);
int SJL06Cmd14(PUnionSJL06Server psjl06Server,char *desType,char *tmkIndex,char *txKey,char *errCode);
int SJL06Cmd20(PUnionSJL06Server psjl06Server,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd22(PUnionSJL06Server psjl06Server,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd24(PUnionSJL06Server psjl06Server,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd26(PUnionSJL06Server psjl06Server,char *desType,char *keyType,char *macMode,char *tmkIndex,char *txKey,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06CmdC0(PUnionSJL06Server psjl06Server,char *desType,char *tmkIndex,char *inputData,char *outputData,char *errCode);
int SJL06CmdC2(PUnionSJL06Server psjl06Server,char *desType,char *inputData,char *outputData,char *errCode);
int SJL06Cmd50(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd52(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd54(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength1,char *macData1,char *mac1,
	       int dataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd56(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd58(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd70(PUnionSJL06Server psjl06Server,char *desType,char *enFlag,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *inputData,char *mac,char *outputData,char *errCode);
int SJL06CmdC4(PUnionSJL06Server psjl06Server,char *desType,char *enFlag,char *tmkIndex,char *txKey,
	       int dataLength,char *inputData,char *outputData,char *errCode);


// ͨ�ô�����ָ��
int SJL06Cmd31(PUnionSJL06Server psjl06Server,char *bmkIndex,char *bmkValue,char *errCode);

#ifdef _Suport3Des_
int SJL06Cmd1A(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd1B(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd13(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen1,TUnionDesKeyLength keyLen2,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#else
int SJL06Cmd1A(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(PUnionSJL06Server psjl06Server,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd1B(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd13(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(PUnionSJL06Server psjl06Server,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(PUnionSJL06Server psjl06Server,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(PUnionSJL06Server psjl06Server,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(PUnionSJL06Server psjl06Server,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(PUnionSJL06Server psjl06Server,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#endif

int SJL06Cmd15(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue1,char *keyValue2,char *errCode);


int SJL06Cmd1C(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue1,char *variant,char *keyValue2,char *errCode);
int SJL06Cmd21(PUnionSJL06Server psjl06Server,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd32(PUnionSJL06Server psjl06Server,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd62(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *clearPin,char *pan,char *enPin,char *errCode);
int SJL06Cmd63(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd64(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd65(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pik,char *enPin1,char *pan,char *tmkIndex,char *enPin2,char *errCode);
int SJL06Cmd67(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan,char *enPin2,char *errCode);
int SJL06Cmd69(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan1,char *pan2,char *enPin2,char *errCode);
int SJL06Cmd84(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *mak,char *enPin1,char *pan,char *addInfo,int macDataLength,char *macData,char *enPin2,char *mac,char *errCode);
int SJL06Cmd85(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *mak1,char *bmkIndex2,char *mak2,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd86(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *pik1,char *mak1,char *bmkIndex2,char *pik2,char *mak2,char *enPin1,char *pan,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *enPin2,char *mac2,char *errCode);
int SJL06Cmd01(PUnionSJL06Server psjl06Server,char *errCode);
int SJL06Cmd03(PUnionSJL06Server psjl06Server,char *bmkIndex,char *errCode);
int SJL06Cmd04(PUnionSJL06Server psjl06Server,char *tmkIndex,char *errCode);
int SJL06Cmd90(PUnionSJL06Server psjl06Server,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd91(PUnionSJL06Server psjl06Server,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd92(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd93(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd7A(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,int dataLength,char *flag,char *data,char *enData,char *errCode);

#endif // for _SJL06CmdForJK_IC_RSA_

#ifdef _RacalCmdForNewRacal_
#include "3DesRacalCmd.h"
#include "3DesRacalSyntaxRules.h"

// ��һ��ZMK���ܵ���Կת��ΪLMK����
int RacalCmdA6(PUnionSJL06Server psjl06Server,
	TUnionDesKeyType keyType,char *zmk,char *keyByZmk,
	char *keyByLmk,char *checkValue,char *errCode);

int RacalCmdA0(PUnionSJL06Server psjl06Server,
	int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,
	char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue,char *errCode);

int RacalCmd0A(PUnionSJL06Server psjl06Server,char *errCode);
int RacalCmdRA(PUnionSJL06Server psjl06Server,char *errCode);
int RacalCmdPA(PUnionSJL06Server psjl06Server,char *format,char *errCode);
int RacalCmdA2(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode);

// ת��PIN
int RacalCmdCC(PUnionSJL06Server psjl06Server,int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2,char *errCode);

int RacalCmdTI(PUnionSJL06Server psjl06Server,char zpk1type,int zpk1Length,char *zpk1,char zpk2type,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo1,int lenOfAccNo1,char *accNo2,int lenOfAccNo2,char *pinFormat2,char *pinBlockByZPK2,char *errCode);

// ����MAC
int RacalCmdMS(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);

// ����MAC
/*int RacalCmdMU(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);
*/

// Mary add begin, 2004-3-11
#define gPVKI				"0"
#define gServiceCode			"000"
//#define gIBMDecimalizationTable	"0123456789012345"
//#define gIBMDefaultPinOffset		"FFFFFFFFFFFFFFFF"


/*
�������ܣ�
	DGָ���PVK����PIN��PVV(PIN Verification Value)��
	���õļ��ܱ�׼ΪVisa Method
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinLength��LMK���ܵ�PIN���ĳ���
	pin��LMK���ܵ�PIN����
	pvkLength��LMK���ܵ�PVK�Գ���
	pvk��LMK���ܵ�PVK��
	lenOfAccNo���ͻ��ʺų���
	accNo���ͻ��ʺ�
���������
	pvv��������PVV
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���PVV
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������PVV�ĳ���
*/
int RacalCmdDG(PUnionSJL06Server psjl06Server,int pinLength,char *pin,int pvkLength,char *pvk,\
		int lenOfAccNo,char *accNo,char *pvv,char *errCode);

/*
�������ܣ�
	CAָ���һ��TPK���ܵ�PINת��Ϊ��ZPK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByZPK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByZPK�ĳ���
*/
int RacalCmdCA(PUnionSJL06Server psjl06Server,int tpkLength,char *tpk,int zpkLength,\
		char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK,char *errCode);

/*
�������ܣ�
	JEָ���һ��ZPK���ܵ�PINת��Ϊ��LMK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinFormat��PIN��ʽ
	pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdJE(PUnionSJL06Server psjl06Server,int zpkLength,char *zpk,\
		char *pinFormat,char *pinBlockByZPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	JCָ���һ��TPK���ܵ�PINת��Ϊ��LMK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	tpkLength��LMK���ܵ�TPK����
	tpk��LMK���ܵ�TPK
	pinFormat��PIN��ʽ
	pinBlockByTPK��ת��ǰ��TPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdJC(PUnionSJL06Server psjl06Server,int tpkLength,char *tpk,\
		char *pinFormat,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	JGָ���һ��LMK���ܵ�PINת��Ϊ��ZPK����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinFormat��PIN��ʽ
	pinLength��LMK���ܵ�PIN���ĳ���
	pinBlockByZPK��ת��ǰ��ZPK���ܵ�PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByZPK��ת������LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByZPK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByZPK�ĳ���
*/
int RacalCmdJG(PUnionSJL06Server psjl06Server,int zpkLength,char *zpk,\
		char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,\
		int lenOfAccNo,char *pinBlockByZPK,char *errCode);

/*
�������ܣ�
	JAָ��������һ��PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinLength��Ҫ��������ɵ�PIN���ĵĳ���
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK�����������PIN�����ģ���LMK����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdJA(PUnionSJL06Server psjl06Server,int pinLength,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	EEָ���IBM��ʽ����һ��PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdEE(PUnionSJL06Server psjl06Server,int minPINLength,char *pinValidData,\
		char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,\
		char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	DEָ���IBM��ʽ����һ��PIN��Offset
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinOffset
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinOffset�ĳ���
*/
int RacalCmdDE(PUnionSJL06Server psjl06Server,int minPINLength,char *pinValidData,\
		char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,\
		char *pvk,char *accNo,int lenOfAccNo,char *pinOffset,char *errCode);

/*
�������ܣ�
	BAָ�ʹ�ñ�������Կ����һ��PIN����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinCryptogramLen�����ܺ��PIN���ĵĳ���
	pinTextLength��PIN���ĵĳ���
	pinText��PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	pinBlockByLMK��LMK���ܵ�PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinBlockByLMK
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinBlockByLMK�ĳ���
*/
int RacalCmdBA(PUnionSJL06Server psjl06Server,int pinCryptogramLen,int pinTextLength,\
		char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
�������ܣ�
	NGָ�ʹ�ñ�������Կ����һ��PIN����
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinCryptogramLen��PIN���ĵĳ���
	pinCryptogram��PIN����
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	referenceNumber����LMK18-19�����ʺŵõ���ƫ��ֵ
	pinText��PIN����
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���pinText
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������pinText�ĳ���
*/
int RacalCmdNG(PUnionSJL06Server psjl06Server,int pinCryptogramLen,char *pinCryptogram,\
		char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText,char *errCode);

/*
�������ܣ�
	DAָ���IBM��ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdDA(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,\
		char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	EAָ���IBM��ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdEA(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,\
		char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	DCָ���VISA��ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdDC(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,\
		char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	ECָ���VISA��ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdEC(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,\
		char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	BCָ��ñȽϷ�ʽ��֤�ն˵�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByTPK��TPK���ܵ�PIN����
	tpkLength��LMK���ܵ�TPK
	tpk��LMK���ܵ�TPK
	pinByLMKLength������PIN���ĳ���
	pinByLMK������PIN���ģ���LMK02-03����
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdBC(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,\
		int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	BEָ��ñȽϷ�ʽ��֤�������ĵ�PIN
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	pinBlockByZPK��ZPK���ܵ�PIN����
	zpkLength��LMK���ܵ�ZPK����
	zpk��LMK���ܵ�ZPK
	pinByLMKLength������PIN���ĳ���
	pinByLMK������PIN���ģ���LMK02-03����
	pinFormat��PIN��ʽ
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdBE(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,\
		int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
�������ܣ�
	CWָ�����VISA��У��ֵCVV 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	cardValidDate��VISA������Ч��
	cvkLength��CVK�ĳ���
	cvk��CVK
	accNo���ͻ��ʺ�
	lenOfAccNo���ͻ��ʺų���
���������
	cvv�����ɵ�VISA����CVV
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���cvv
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������cvv�ĳ���
*/
int RacalCmdCW(PUnionSJL06Server psjl06Server,char *cardValidDate,int cvkLength,char *cvk,\
		char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,char *errCode);

/*
�������ܣ�
	CYָ���֤VISA����CVV 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
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
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
int RacalCmdCY(PUnionSJL06Server psjl06Server,char *cvv,char *cardValidDate,\
		int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *errCode);

/*
�������ܣ�
	EWָ���˽Կǩ�� 
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	indexOfVK��˽Կ������
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	vkLength��LMK���ܵ�˽Կ����
	vk��LMK���ܵ�˽Կ
���������
	signature�����ɵ�ǩ��
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���signature
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������signature�ĳ���
*/
//int RacalCmdEW(PUnionSJL06Server psjl06Server,char *indexOfVK,int signDataLength,
//		char *signData,int vkLength,char *vk,char *signature,char *errCode);

/*
�������ܣ�
	EYָ��ù�Կ��֤ǩ��
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	macOfPK����Կ��MACֵ
	signatureLength������֤��ǩ���ĳ���
	signature������֤��ǩ��
	signDataLength����ǩ�����ݵĳ���
	signData����ǩ��������
	publicKeyLength����Կ�ĳ���
	publicKey����Կ
���������
	errCode���������
����ֵ��
	<0������ִ��ʧ��
	0����errCodeΪ'00'����֤�ɹ���������ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
*/
//int RacalCmdEY(PUnionSJL06Server psjl06Server,char *macOfPK,int signatureLength,
//		char *signature,int signDataLength,char *signData,int publicKeyLength,
//		char *publicKey,char *errCode);
// Mary add end, 2004-3-11

// Mary add begin, 2004-3-24
/*
�������ܣ�
	BUָ�����һ����Կ��У��ֵ
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	key��LMK���ܵ���Կ����
���������
	checkValue�����ɵ���ԿУ��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������checkValue�ĳ���
*/
int RacalCmdBU(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode);

/*
�������ܣ�
	A4ָ��ü�����Կ�����ĳɷֺϳ�һ����Կ��������У��ֵ
���������
	hsmSckHDL������ܻ�������SOCKET������
	pSJL06�����ܻ����ԣ�����IP��ַ��
	keyType����Կ������
	keyLength����Կ�ĳ���
	partKeyNum����Կ�ɷֵ�����
	partKey����Ÿ�����Կ�ɷֵ����黺�壬ΪLMK���ܵ���Կ����
���������
	keyByLMK���ϳɵ���Կ���ģ���LMK����
	checkValue���ϳɵ���Կ��У��ֵ
	errCode���������
����ֵ��
	<0������ִ��ʧ�ܣ���keyByLMK��checkValue
	0�����ܻ�ִ��ָ��ʧ�ܣ�errCode�а������ܻ��Ĵ������
	>0���ɹ�������keyByLMK��checkValue���ܳ���
*/
int RacalCmdA4(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode);
// Mary add end, 2004-3-24

int RacalCmdDF(PUnionSJL06Server psjl06Server,char *zpk,char *pvk,int maxPinLen,char *decimalizationTable,
		char *pinValidationData,char *pinBlock1,char *accNo1,char *accNo2,
		char *pinBlock2,char *pinOffset,char *errCode);

int RacalCmdA8(PUnionSJL06Server psjl06Server,
	TUnionDesKeyType keyType,char *key,char *zmk,
	char *keyByZMK,char *checkValue,char *errCode);

// 20051206������������
int RacalCmdFK(PUnionSJL06Server psjl06Server,char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue,char *errCode);

/*
����	ת��DES��Կ���ӹ�Կ���ܵ�����Կ����
���ڽ�����Կ��

������	����	����	˵����
�������	2	A	ֵ"3A"
˽Կ����	2	N	"00"��"20"����������ڵ�˽Կ
��Կ����	4	N	DES��Կ���ĵ��ֽ���
��Կ����	n	B	�ù�Կ���ܵ�DES��Կ


�����	����	����	˵����
��Ӧ����	2	A	"3B"
�������	2	H	
DES��Կ	32	H	������Կ���ܵ�DES��Կ
���ֵ	16	H	DES��Կ����64bits��0
*/
int SJL06Cmd3A(PUnionSJL06Server psjl06Server,int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue,char *errCode);

/*
�ߡ�	ת��DES��Կ��������Կ���ܵ���Կ����
���ڷַ���Կ��

������	����	����	˵����
�������	2	A	ֵ"3B"
��Կ����	32	H	������Կ���ܵ�DES��Կ
����Կ	n	B	


�����	����	����	˵����
��Ӧ����	2	A	"3C"
�������	2	H	
���ֵ	16	H	DES��Կ����64bits��0
��Կ����	4	N	DES��Կ���ĵ��ֽ���
��Կ����	n	B	�ù�Կ���ܵ�DES��Կ
*/
int SJL06Cmd3B(PUnionSJL06Server psjl06Server,char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue,char *errCode);

// ����ZAK/ZEK
int RacalCmdFI(PUnionSJL06Server psjl06Server,char keyFlag,char *zmk,int variant,char keyLenFlag,
	char *keyByZmk,char *keyByLmk,char *checkValue,char *errCode);

// ��������MAC
/*int RacalCmdMU(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);
*/


//lixiang add begin,2009-01-19
/*
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	pvkIndex��	ָ����˽Կ�����ڽ���PIN��������
	fillMode��	��0����������ݳ���С����Կ���ȣ�����ʱ��������ǰ�油0x00����ʹ���ݳ��ȵ�����Կ�ĳ��ȣ�Ȼ���ٽ��м��ܣ�
			��1����PKCS��䷽ʽ��һ�������ʹ�ô˷�ʽ��
	ZPK��		���ڼ���PIN����Կ��
	accNo��		�û����ʺ�
	lenOfPinBlock:	��Կ���ܵ�PIN���ĵĳ���
	pinBlock��	����Կ���ܵ�PIN��������
�������	
	lenOfPin��	��Կ����
	pinBlock1��	����ZPK�����µ�PIN����
	pinBlock2��	����ZPK���ܵ�PIN���ģ�����ZPK���ܻ�ԭ��ʵPIN��
	lenOfUniqueID��	01-20
	UniqueID��	���صľ�ASCII��չ��ID������
	errCode��	�����������
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�
*/
int RacalCmdH2 (PUnionSJL06Server psjl06Server, int pvkIndex, char fillMode, char *ZPK, char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,  \
		char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID, char *errCode);

/*
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	OriKeyType��	ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
	OriKey��	ԴKEY�����ڽ���PIN����Կ��
	lenOfAcc��	�ʺų��ȣ�4~20��
	accNo��		�û����ʺš�
	DesKeyType��	������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
	DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
	pinBlock��	����Կ���ܵ�PIN�������ġ�
�������
	lenOfPinBlock1�����볤�ȡ�
	pinBlock1��	����ZPK2/PVK�����µ�PIN���ġ�
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�
*/
int RacalCmdH5 (int hsmSckHDL,PUnionSJL06 pSJL06,char *OriKey, char *lenOfAcc, 	char *AccNo,char DesKeyType, \
			char *DesKey, char *pinBlock, char *lenOfPinBlock1, char *pinBlock1, char *errCode);
		 
/*
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	ZPK��		���ڽ���PIN����Կ��
	lenOfAcc��	�ʺų��ȣ�4~20��
	accNo��		�û����ʺš�
	pinBlockByZPK��	��ZPK���ܵ�PIN�������ġ�
	PVK��		PVK��
	pinBlockByPVK��	��PVK���ܵ�PIN�������ġ�
�������
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�
*/

/*
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	OriKeyType��	ԴKEY���ͣ������� - ZPK �������� - PVK��Ŀ��KEY���Ͳ���Ϊ0��
	OriKey��	ԴKEY�����ڽ���PIN����Կ��
	lenOfAcc��	�ʺų��ȣ�4~20��
	accNo��		�û����ʺš�
	DesKeyType��	������ - ZPK��ֻ��ԴKEY����Ϊ0ʱ��Ŀ��KEY���Ϳ�Ϊ0���������� - PVK
	DesKey��	Ŀ��KEY�����ڼ���PIN����Կ��
	pinBlock��	����Կ���ܵ�PIN�������ġ�
�������
	lenOfPinBlock1�����볤�ȡ�
	pinBlock1��	����ZPK2/PVK�����µ�PIN���ġ�
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�
*/
int RacalCmdH7 (int hsmSckHDL,PUnionSJL06 pSJL06, char *OriKey, char *lenOfAcc,char *AccNo, char *DesKey, \
		char *pinBlock, char *lenOfPinBlock1,char *pinBlock1, char *errCode);


int RacalCmdH9 (int hsmSckHDL,PUnionSJL06 pSJL06, char *ZPK, char *lenOfAcc, char *AccNo, \
		char *pinBlockByZPK, char *PVK, char *pinBlockByPVK, char *errCode);
		
/*
����	
	��ָ��Ӧ�õ�PK���ܵ�PINת��ΪZPK���ܡ�
�������
	hsmSckHDL��	����ܻ�������SOCKET������
	pSJL06��	���ܻ����ԣ�����IP��ַ��
	ZPK��		���ڼ���PIN����Կ��
	accNo��		�û����ʺš�
	pinBlockByZPK��	��ZPK���ܵ�PIN�������ġ�
	dataOfZAK��	����ZAK��Կ������֮һ��
�������	
	lenOfPin��	���볤��
	ZAK��		����LMK26-27�ԶԼ����µ�ZAK��Կ��
	errCode��	����������롣
����ֵ	
	<0��		����ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>=0��		����ִ�гɹ�
*/
int RacalCmdH4 (int hsmSckHDL, PUnionSJL06 pSJL06, char *ZPK, char *AccNo, char *pinBlockByZPK, \
	char *dataOfZAK, char *lenOfPin, char *ZAK, char *errCode);

/*
����
	��PinOffsetת��Ϊר���㷨��FINSE�㷨�����ܵ�����
��  ��  ��  Ϣ  ��  ʽ

��  ��  ��	��  ��	��  ��	��      ��
��   ��	2	A	S1
PVK	16H��1A+32H��1A+48H	H	LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset��
PinOffset	12	N	Offset��ֵ�����������ķ�ʽ���ұ�����ַ���F����
��鳤��	2	N	��С��PIN���ȡ�
�˺�	12	N	�˺���ȥ��У��λ������12λ��
ʮ����ת����	16	N	��ʮ������ת��Ϊʮ���Ƶ�ת����
PINУ������	12	A	�û�����ġ�����ʮ�������ַ����ַ���N�������ݣ�����ָʾHSM�����˺����5λ��λ�á�


��  ��  ��  Ϣ  ��  ʽ
��  ��  ��	��  ��	��  ��	��      ��
Ӧ����	2	A	S2
�������	2	H	00�� ��ȷ      ������ʧ��
���	6	H	PIN����
*/
int RacalCmdS1(int hsmSckHDL, PUnionSJL06 pSJL06, char *PVK, char *pinOffSet,int minPinLen, \
	char *accNO,char *decimalizationTable,char *pinValidationData, char *pinBlock, char *errCode);

/*
���ܣ�
	��PinBlockת��Ϊר���㷨��FINSE�㷨�����ܵ�����
��  ��  ��  Ϣ  ��  ʽ

��  ��  ��	��  ��	��  ��	��      ��
��   ��	2	A	S2
ZPK	16H��1A+32H��1A+48H	H	LMK�ԣ�14-15���¼��ܵ�PVK�����ڽ���offset��
��鳤��	2	N	��С��PIN���ȡ�
�˺�	12	N	�˺���ȥ��У��λ������12λ��
ԴPIN��	16H	H	ԴPIN��


��  ��  ��  Ϣ  ��  ʽ
��  ��  ��	��  ��	��  ��	��      ��
Ӧ����	2	A	S 3
�������	2	H	00�� ��ȷ      ������ʧ��
���	6	H	PIN����
*/
int RacalCmdS2(int hsmSckHDL, PUnionSJL06 pSJL06, char *ZPk, int minPinLen,char *accNo, \
	char *OriPinBlock, char *pinBlock,char *errCode);
	
/*
Field	Length&Type	Details
COMMAND MESSAGE
Message header	mA	(Subsequently returned to the Host unchanged).
Command code	2A	Value E0.
Message block number	1N	0: The only block.
1: The first block.
2: A middle block.
3: The last block.
Crypto flag	1N	0: DES encryption, 1: DES decryption
Algorithm:
Mode of operation	1N	1: ECB �C Electronic Code Book
2: CBC �C Cipher Block Chaining
3: CFB �C Cipher Feed Back
4: OFB �C Output Feed Back
5: PCBC
Key flag	1N	0: ZEK �C Zone Encryption Key
ZEK	16H/1A+32H/1A+48H	ZEK encrypted under LMK pair 30-31.

Input data format	1N	0: Binary, 1: expanded Hex
Onput data format	1N	0: Binary, 1: expanded Hex
Pad  mode	1N	Present only when message block number is 0, or 3.
0: If data lengths are exact multiples of eight bytes, not padding; otherwise padding characters defined by the following field (Pad character)until data lengths are exact multiples of eight bytes.
1: If  data lengths are exact multiples of eight bytes ,Padding another eight bytes defined  by the following field( (Pad character)�� otherwise  padding characters defined by the following field(Pad character),until data lengths are exact multiples of eight bytes.
Pad character	4H
	Present only when message block number is 0, or 3�� .e.g.
          Pad mode|Pad character|Pad count flag
ANSI X9.19  :  0         0000           0
ANSI X9.23  :  1         0000           1
PBOC MAC  :  1         8000           0 
etc.
Pad count flag	1N	Present only when message block number is 0, or 3. 
0��Last byte is not padding count 
1��Last byte is padding count,and the count is within the range of X��01�� to X��08��
IV	16H	Initialization value, present only when Algorithm:Mode of operation is 2, 3,4or 5.
Message length	3H	Actual Message length in bytes.
Binary: nB, 
Expanded Hex : n/2 n must be even.
Message block	nB	The clear/cipher text message block.
RESPONSE MESSAGE

Message header	mA  	Returned to the Host unchanged.
Response code	2A	Value E1.
Error code   	2N	00 : No Errors.
05: Invalid message block number
10 : ZEK/TEK parity error
12 : No keys loaded in user storage.
13 : LMK error. Report to supervisor.
15 : Error in input data.
21 : Invalid user storage index.
26: Invalid key scheme
80: Output error
Onput data format	1N	0: Binary, 1: expanded Hex
Message length	3H	The encrypted/decrypted message length in bytes.
Binary: nB, expanded Hex: n/2 n must be even.
Message block	nB	The encrypted/decrypted text message block.
Next Initial Vector	16H	The next initial vector. Present only in mode of operation: CBC, OFB, CFB, PCBC and Message block number is 1 or 2.
*/
int RacalCmdE0(int hsmSckHDL, PUnionSJL06 pSJL06, char *dataBlock, char *CrypToFlag,char *modeOfAlgorithm, \
	char *ZEK, char *inDataType,char *outDataType,char *padMode,char *padChar,char *padCountFlag,char *IV, \
	char *lenOfMSG,char *MSGBlock,char *datafmt,char *lenOfMSG1,char *MSGBlock1,char *IV1,char *errCode);
	
/*
������ɢMAK����MAC

��  ��  ��  Ϣ  ��  ʽ

������	����	����	˵����
�������	2	A	ֵ��G1��
ZAK	16H/1A+32H/1A+48H	N	��LMK26-27�Լ���
��ɢ����	16	H	���ڼ�����ɢ��Կ������
MAC���ݳ���	3H	H	���ڼ���MAC������
MAC����	nB	B	MAC����
							��    ��    ��    Ϣ
�����	����	����	˵����
��Ӧ����	2	A	ֵ��G2��
�������	2	H	
MAC	16	H	���ص�MACֵ
�������̣�
1.	��ZAK��Կ����ɢ���ݲ���IC����׼��ɢ�㷨������ɢ�����ZAKΪ64bit������ֱ�Ӷ���ɢ���ݽ���DES���ܼ��ɡ�
2.	����ɢ��Կ��MAC���ݲ���Ansi9.19��ZAK˫�������ϣ���9.9��ZAK���������㷨����MAC��
*/
int RacalCmdG1(int hsmSckHDL, PUnionSJL06 pSJL06, char *ZAK, char *scatData,char *lenOfmac,char *macData, char *mac,char *errCode);


//lixiang add end,2009-01-19

//add by changzx 2009-3-3
/*���������ָ��*/
int RacalCmdY3(int hsmSckHDL,PUnionSJL06 pSJL06,int min,int max,int Cnt,int isRepeat,char *RandNo,char *errCode);

/*
ָ��ܣ�����PINBLOCK��ANSI9.8��ʽ�໥ת��
��  ��  ��  Ϣ  ��  ʽ
������	����	������		˵����
��Ϣͷ	mA	
�������	2A	Value �� Y1 ��
ԭZPK	16H or
1A+32H or 1A+48H
	��LMK�ԣ�06-07���¼���
Ŀ��ZPK	16H or
1A+32H or 1A+48H
	��LMK�ԣ�06-07���¼���
ԴPIN��	16H or 32H	ԴZPK�¼��ܵ�ԴPIN�顣
ԴPINBLOCK��ʽ	1N	1��ANSI9.8��ʽ
2������PINר�ø�ʽ��ר���㷨
Ŀ��PINBLOCK��ʽ	1N	1��ANSI9.8��ʽ
2������PINר�ø�ʽ��ר���㷨
�˺�	12N	�û����ʺţ����õ�����12λ����������λ;
��  ��  ��  Ϣ  ��  ʽ
��Ϣͷ	mA	
��Ӧ����	2A	Value �� Y2 ��
�������	2N	
PINBLOCK	16H��32H	��ת����ANSI9.8��ʽ��PINBLOCK��16H����ת��������PIN��ʽ����32H��
ע��1������PINBLOCKΪ�����ʺż��ܡ�
2������ANSI9.8��ʽת������ר�ø�ʽʱ����Ϊ�����ʺż��ܡ�
*/
int RacalCmdY1(int hsmSckHDL,PUnionSJL06 pSJL06,char *OriZPK,char *DesZPK,char *OriPinBlock,char *accNo,char *pinBlock,char *errCode);




#endif // for _RacalCmdForNewRacal_

#endif // ifdef _Use_SJL06Server_

#endif

