#ifndef UNION_RSA_H
#define UNION_RSA_H

int UnionVKEnc(unsigned char *plaintxt,int plaintxtLen,char *vk,int fillFlag,unsigned char *encTxt);
int UnionPKDec(unsigned char *crypk,int crypklen,char *pk,int fillFlag,unsigned char *plaintxt);
int UnionPKEnc(unsigned char *plaintxt,int plaintxtlen,char *pk,int fillFlag,unsigned char *crypktxt);
int UnionVKDec(unsigned char *encTxt,int encTxtLen,char *vk,int fillFlag,unsigned char *plaintxt);

//ASN.1 DER���룬�����ֶ����ͽṹ����ǩ�ָ��������outbuf��
int ASN1DER_Decode(char *encode,char *outbuf);

// ����RSA��Կ��
/*
���������
        bits�� ��Կǿ�ȣ�512/1024/1152/1408/1984/2048��
	pkExponent: ��Կָ��,  65537,3
        sizeofPK����Կ�Ļ����С
        sizeofVK: ˽Կ�Ļ����С
���������
        pk:     ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
        vk��    ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���أ�
        >=0 �ɹ�
        <0 ʧ�� 
*/
int UnionGenRSAByBitsAndPKExponent(int bits,int pkExponent,char *pk,int sizeofPK,char *vk,int sizeofVK);

// ����RSA��Կ��,ָ��Ϊ65537
/*
���������
	bits�� ��Կǿ�ȣ�512/1024/2048��
	sizeofPK����Կ�Ļ����С
	sizeofVK: ˽Կ�Ļ����С
���������
	pk:	��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
	vk��    ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���أ�
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionGenRSABySS(int bits,char *pk,int sizeofPK,char *vk,int sizeofVK);


// ��PKCS1����䷽ʽ������˽Կ����
/*
���������
	plaintxt�� ����������
	plaintxtLen�����ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	encTxt�����ܵ���������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKEncByPKCS1(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt);


// ��˽Կ���ܣ�����䷽ʽ
/*
���������
	plaintxt�� ����������
	plaintxtLen�����ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	encTxt�����ܵ���������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKEncNoPADDING(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt);


// ��PKCS1����䷽ʽ�����й�Կ����
/*
���������
	crypk�� ��������������
	crypklen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKDecByPKCS1(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt);


// ���й�Կ����,����䷽ʽ
/*
���������
	crypk�� ��������������
	crypklen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKDecNoPADDING(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt);


// ��PKCS1����䷽ʽ�����й�Կ����
/*
���������
	plaintxt�� ���Ķ���������
	plaintxtlen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	crypktxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKEncByPKCS1(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt);

// ���й�Կ����,����䷽ʽ
/*
���������
	plaintxt�� ���Ķ���������
	plaintxtlen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	crypktxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKEncNoPADDING(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt);

// ��PKCS1����䷽ʽ������˽Կ����
/*
���������
	encTxt�� ����������
	encTxtLen��  ���ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt�����ܺ����������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKDecByPKCS1(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt);

// ��˽Կ���ܣ�����䷽ʽ
/*
���������
	encTxt�� ����������
	encTxtLen��  ���ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt�����ܺ����������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKDecNoPADDING(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt);


// ��Կ PKCS#1 ASN.1 DER ���룻
int UnionPKCSASN1DER_Decode(char *encode,char *pubkey,char *Evalue);

// ���㹫Կ(�ɼ��ַ��������ŵ�BCD��)�������DER��ʽ. pubkey--ģ��Evalue--ָ��
int UnionPKCSASN1DER_Encode(char *pubkey,char *Evalue,char *encode);

int UnionPKCSCerASN1DER_Decode(char *encode,char *outpubkey,char *Evalue);

#endif


