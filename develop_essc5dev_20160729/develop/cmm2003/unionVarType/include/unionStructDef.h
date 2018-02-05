//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionStructDef_
#define _unionStructDef_

#include "unionVarDef.h"

#define conStructDefTagStructName		"structName"
#define conStructDefTagTypeDefName		"typeDefName"
#define conStructDefTagRemark		"remark"
#define conStructDeclareLineTag		"typeDefName="

// �ṹ��������
typedef struct
{
	char			structName[48+1];	// �ṹ������,struct���������
	char			typeDefName[48+1];	// �ṹ������,typedef������
	char			remark[128+1];		// ˵��
} TUnionStructDeclareDef;
typedef TUnionStructDeclareDef	*PUnionStructDeclareDef;

#ifdef _maxNumOfVarPerStructIs512_
#define conMaxNumOfVarPerStruct	512
#else
#define conMaxNumOfVarPerStruct	256
#endif
// ����ṹ�Ķ���
typedef struct
{
	TUnionStructDeclareDef	declareDef;				// �ṹ��������
	int			fldNum;					// �ṹ��������
	TUnionVarDef		fldGrp[conMaxNumOfVarPerStruct];	// ��Ķ���
} TUnionStructDef;
typedef TUnionStructDef		*PUnionStructDef;

/* ��һ���ַ����ж�ȡ�ṹ�����Ķ���
�������
	str		�������崮
	lenOfStr	�������崮�ĳ���
�������
	pdeclareDef	��������������
����ֵ��
	>=0 	������������Ŀ
	<0	�������	
	
*/
int UnionReadStructDeclareDefFromStr(char *str,int lenOfStr,PUnionStructDeclareDef pdeclareDef);

/* ��ָ���ṹ������ͷ��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintStructHeaderDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp);

/* ��ָ���ṹ������β��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintStructTailDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp);

/* ��ָ���ṹ�Ķ����ӡ���ļ���
�������
	pstructDef	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintStructDefToFp(PUnionStructDef pstructDef,FILE *fp);

/* ��ָ���ļ��ж���Ľṹ��ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintStructDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж���Ľṹ��ӡ����Ļ��
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputStructDefInFile(char *fileName);

/* ��ָ�����ƵĽṹ����������ļ���
�������
	nameOfType	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecStructDefToFp(char *nameOfType,FILE *fp);
	
/* ��ָ���ļ��ж���Ľṹ��ӡ����Ļ��
�������
	nameOfType	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecStructDef(char *nameOfType);

/* ��ָ���ṹ�Ķ����Զ����ʽ��ӡ���ļ���
�������
	pdef	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintStructDefToFpInDefFormat(PUnionStructDef pdef,FILE *fp);

#endif
