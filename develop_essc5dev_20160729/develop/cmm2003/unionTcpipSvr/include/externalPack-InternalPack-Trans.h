// author	Wolfgang Wang
// date		2010-10-25

#ifndef _externalPack_InternalPack_Trans_
#define _externalPack_InternalPack_Trans_

/*
����
	���ⲿ������ת��Ϊ�ڲ�������
�������
	exteranlReqPack		�ⲿ������
	lenOfExternalReqPack	�ⲿ�����ĳ���
	sizeOfInteranlReqPack	�ڲ������Ļ����С
�������
	interanlReqPack		�ڲ�������
����ֵ
	>=0			�ڲ������ĳ���
	<0			�������
*/
int UnionTransExternalReqPackIntoInternalReqPack(unsigned char *exteranlReqPack,int lenOfExternalReqPack,
		unsigned char *interanlReqPack,int sizeOfInteranlReqPack);

/*
����
	���ڲ���Ӧ����ת��Ϊ�ⲿ��Ӧ����
�������
	interanlResPack		�ڲ���Ӧ����
	lenOfInternalResPack	�ڲ���Ӧ���ĳ���
	sizeOfExteranlResPack	�ⲿ��Ӧ���Ļ����С
�������
	exteranlResPack		�ⲿ��Ӧ����
����ֵ
	>=0			�ⲿ��Ӧ���ĳ���
	<0			�������
*/
int UnionTransInternalResPackIntoExternalResPack(unsigned char *interanlResPack,int lenOfInternalResPack,unsigned char *exteranlResPack,
		int sizeOfExteranlResPack);

#endif
