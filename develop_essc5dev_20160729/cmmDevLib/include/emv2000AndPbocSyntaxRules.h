//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _Emv2000AndPbocSyntaxRules_
#define _Emv2000AndPbocSyntaxRules_

// PBOC mode='0'��ʽ����PAN
int UnionForm16BytesDisperseDataOfV41A(int lenOfData, char *data, char *disperseData);

/*
����
	���ݹ淶�������γ�16�ֽڵ���ɢ����
�������
	method����ɢ������ɷ�����0��EMV2000 V4.1ѡ��A��1��EMV2000 V4.1ѡ��B��2��PBOC2.0
	lenOfData�����ݳ���
	data������
�������
	disperseData��16�ֽڵ���ɢ����
����ֵ
	<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>0������ִ�гɹ�������disperseData�ĳ���
*/
int UnionForm16BytesDisperseData(int iMode, int lenOfData, char *data, char *disperseData);

int UnionPBOCEMVFormPlainDataBlock(int lenOfData,unsigned char *data,unsigned char *dataBlock);

int UnionPBOCEMVFormPlainDataBlockForSM4(int lenOfData,unsigned char *data,unsigned char *dataBlock);

#endif
