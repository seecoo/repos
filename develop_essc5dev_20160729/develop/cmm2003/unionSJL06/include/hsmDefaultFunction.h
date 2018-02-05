//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _hsmDefaultFunction_
#define _hsmDefaultFunction_

#include "unionHsm.h"

/*
����
	�ж�ָ��IP�ļ��ܻ��Ƿ�����
�������
	ipAddr		���ܻ�IP
�������
	hsmGrpID	���
����ֵ
	1		����
	<=0		�쳣
*/
int UnionIsHsmNormal(char *ipAddr,char *hsmGrpID);

/*                                                           
����                                                         
	��ȡһ����������еĿ��������                       
�������                                                     
	hsmGrpID	��������                           
�������                                                     
	hsmGrp		������ṹ��                         
����ֵ                                                       
	>=0 �ɹ�                                             
	<0 ʧ��                                              
*/                                                           
int UnionFindHsmOfSpecHsmGrp(char *hsmGrpID,TUnionHsm hsmGrp);

/*
����
	��ȡһ����������е����п��������
�������
	hsmGrpID	��������
	maxNum		���ܷ��ص���������̨��
�������
	hsmRecGrp	���������ɵ�����
����ֵ
	>=0		ʵ�ʶ�ȡ�������̨��
	<0		������
*/
int UnionFindAllWorkingHsmOfSpecHsmGrp(char *hsmGrpID, TUnionHsm hsmRecGrp[], int maxNum);

/*
����
	��ȡ��������ڵĴ�ӡ�����
�������
	hsmGrpID	�������ID
�������
	hsmRec		������ṹ��
����ֵ
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionFindDefaultPrinterHsmOfHsmGroup(char *hsmGrpID,PUnionHsm hsmRec);

// ʹ��ָ�����ܻ����ڵ����������ӡһ����Կ,
/*
����
	ʹ��ָ�����ܻ����ڵ����������ӡһ����Կ
�������
	hsmGrpID		�������ID
	keyType			��Կ����
	keyLen			��Կ����
	printFormat		��Կ�Ĵ�ӡ��ʽ
	checkValueFormat	��ԿУ��ֵ�Ĵ�ӡ��ʽ
	numOfComponent		����������
	keyName			��Կ����
�������
	value			��Կֵ
	checkValue		��ԿУ��ֵ
����ֵ
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionPrintDesKeyUsingSpecHsmGroup(char *hsmGrpID, int keyType, int keyLen, char *printFormat, char *checkValueFormat, int numOfComponent, char *keyName, char *value, char *checkValue);

#endif


