// wolfang wang
// 2008/10/3

#ifndef _unionVariableDef_
#define _unionVariableDef_

#define conVariableDefTagDefaultValue	"defaultValue"

#include "unionVarDef.h"

// ����ȫ�ֱ���
typedef struct
{
	TUnionVarDef		varDef;
	char			defaultValue[128+1];	// ȱʡֵ
} TUnionVariableDef;
typedef TUnionVariableDef	*PUnionVariableDef;

/*
����	
	��һ������ڴ��ж�ȡһ��ȫ�ֱ������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		ȫ�ֱ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVariableDefFromStr(char *str,int lenOfStr,PUnionVariableDef pdef);

/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVariableDefToFp(PUnionVariableDef pdef,FILE *fp);

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVariableDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputVariableDefInFile(char *fileName);

/* ��ָ�����Ƶ�ȫ�ֱ�������������ļ���
�������
	nameOfVariable	ȫ�ֱ�������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecVariableDefToFp(char *nameOfVariable,FILE *fp);
	
/* ��ָ���ļ��ж����ȫ�ֱ�����ӡ����Ļ��
�������
	nameOfVariable	ȫ�ֱ�������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecVariableDef(char *nameOfVariable);

#endif
