#ifndef _UNION_VK_FORM_
#define _UNION_VK_FORM_

#define DerByteType  2      //�������ַ���
#define	DerOIDType  6		//OID����
#define	DerOctStringType  4    //�ַ���
#define	DerNullType  5			//��ֵ
#define algIdString  "2A864886F70D010101"

/*
���ܣ�ȡDER��ʽ��ָʾ�ĳ���,��֮���������ڵ�ַ��λ��
���룺
	derByte��der��ʽ���ַ���
	derByteLen��derByte�ĳ���
�����
	���ִ��ĵ�һ������ֵ���ڵ�ַ��λ��
���أ�
	>= �ɹ�����ʾDER��ʽ��ָʾ�ĳ���
	< 0. ʧ��
*/
int UnionFetchLenOfDerVal(unsigned char *derByte,int derByteLen,int *locationOfVal);

/*
���ܣ�ȡDER��ʽ��ָʾ�����Գ���,�����Ե�ֵ
���룺
	derByte��der��ʽ���ַ���
	derByteLen��derByte�ĳ���
	derType��	���Ե�����
�����
	pval��ָ������ֵ��ַ��ָ��
	valLen������ֵ�ĳ���
���أ�
	>=0 �ɹ�����ʾ��һ�����Ե�λ��
	<0 ʧ��
*/
int UnionGetNextValOfDer(unsigned char *derByte,int derByteLen,int derType,unsigned char **pval,int *valLen);

/*
���ܣ�����DER��ʽ��˽Կ(��չ��ascHex)��ȡ��DER��ʽ�Ĺ�Կ(��չ��ascHex)
���룺
	derVkAscHex��DER��ʽ��˽Կ(��չ��ascHex)
�����
	derPkAscHex��DER��ʽ�Ĺ�Կ(��չ��ascHex)
���أ�
	>=0 �ɹ���
	<0 ʧ��
*/
int UnionGetPKFromVK(char *derVkAscHex,char *derPkAscHex);


/*
���ܣ�����DER��ʽ��˽Կ(��չ��ascHex)��ȡ��������(��չ��ascHex)
���룺
	derVkAscHex��DER��ʽ��˽Կ(��չ��ascHex)
�����
	moduluesAscHex����Կģ(��չ��ascHex)
	publicExponentAscHex: ��Կָ��(��չ��ascHex)
	privateExponentAscHex: ˽Կ��ָ��(��չ��ascHex)
	primePAscHex:	˽Կ��P(��չ��ascHex)
	primeQAscHex: ˽Կ��Q(��չ��ascHex)
	dmp1AscHex: ˽Կ��dmp1(��չ��ascHex)
	dmq1AscHex: ˽Կ��dmq1(��չ��ascHex)
	coefAscHex: ˽Կ��coef(��չ��ascHex)
���أ�
	>=0 �ɹ���
	<0 ʧ��
*/
int UnionUnPackFromVK(char *derVkAscHex,char *modulues, char *publicExponent, char *privateExponent,char *primeP,char *primeQ,char *dmp1,char *dmq1,char *coef);


#endif
