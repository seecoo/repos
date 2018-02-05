//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionSimpleTypeDef_
#define _unionSimpleTypeDef_

#include "unionSimpleTypeDef.h"

#define conSimpleTypeDefTagVarName	"name"
#define conSimpleTypeDefTagNameOfType	"nameOfType"
#define conSimpleTypeDefTagRemark	"remark"

// ��������͵Ķ���
typedef struct
{
	char		name[64+1];		// ���������͵�����
	char		nameOfType[64+1];	// �����͵�����
	char		remark[80+1];
} TUnionSimpleTypeDef;
typedef TUnionSimpleTypeDef	*PUnionSimpleTypeDef;

/*
����	
	���ָ�������Ͷ�Ӧ��c���Ե�����
�������
	nameOfType	��������
�������
	��
����ֵ
	�������͵��ڲ���ʶ
*/
int UnionGetFinalTypeTagOfSpecNameOfSimpleType(char *nameOfType);

/*
����	
	���ָ�������Ͷ�Ӧ����������
�������
	oriNameOfType	��������
�������
	finalNameOfType	������������
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionGetFinalTypeNameOfSpecNameOfSimpleType(char *oriNameOfType,char *finalNameOfType);

/* ��ָ�������͵Ķ����ӡ���ļ���
�������
	pdef	�����Ͷ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSimpleTypeDefToFp(PUnionSimpleTypeDef pdef,FILE *fp);

/* ��ָ���ļ��ж���ļ����ʹ�ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSimpleTypeDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж���ļ����ʹ�ӡ����Ļ��
�������
	pdef	�����Ͷ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSimpleTypeDefInFile(char *fileName);

/* ��ָ�����Ƶļ����Ͷ���������ļ���
�������
	nameOfType	����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecSimpleTypeDefToFp(char *nameOfType,FILE *fp);

/* ��ָ�������͵Ķ����Զ����ʽ��ӡ���ļ���
�������
	pdef	�����Ͷ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSimpleTypeDefToFpInDefFormat(PUnionSimpleTypeDef pdef,FILE *fp);
	
/* ��ָ���ļ��ж���ļ����ʹ�ӡ����Ļ��
�������
	nameOfType	����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecSimpleTypeDef(char *nameOfType);

#endif
