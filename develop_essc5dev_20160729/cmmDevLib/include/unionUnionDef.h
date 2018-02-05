//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionUnionDef_
#define _unionUnionDef_

#include "unionVarDef.h"

#define UnionDefTagUnionName		"unionName"
#define UnionDefTagTypeDefName		"typeDefName"
#define UnionDefTagRemark		"remark"
#define conUnionDeclareLineTag		"typeDefName="

// �ṹ��������
typedef struct
{
	char			unionName[48+1];	// �ṹ������,struct���������
	char			typeDefName[48+1];	// �ṹ������,typedef������
	char			remark[128+1];		// ˵��
} TUnionUnionDeclareDef;
typedef TUnionUnionDeclareDef	*PUnionUnionDeclareDef;

#ifdef _maxNumOfVarPerUnionIs256_
#define conMaxNumOfVarPerUnion	256
#else
#define conMaxNumOfVarPerUnion	128
#endif
// ����ṹ�Ķ���
typedef struct
{
	TUnionUnionDeclareDef	declareDef;				// �ṹ��������
	int			fldNum;					// �ṹ��������
	TUnionVarDef		fldGrp[conMaxNumOfVarPerUnion];	// ��Ķ���
} TUnionUnionDef;
typedef TUnionUnionDef		*PUnionUnionDef;

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
int UnionReadUnionDeclareDefFromStr(char *str,int lenOfStr,PUnionUnionDeclareDef pdeclareDef);

/* ��ָ���ṹ������ͷ��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintUnionHeaderDeclareDefToFp(PUnionUnionDeclareDef pdef,FILE *fp);

/* ��ָ���ṹ������β��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintUnionTailDeclareDefToFp(PUnionUnionDeclareDef pdef,FILE *fp);

/* ��ָ���ṹ�Ķ����ӡ���ļ���
�������
	pstructDef	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintUnionDefToFp(PUnionUnionDef pstructDef,FILE *fp);

/* ��ָ���ļ��ж���Ľṹ��ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintUnionDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж���Ľṹ��ӡ����Ļ��
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputUnionDefInFile(char *fileName);

/* ��ָ�����ƵĽṹ����������ļ���
�������
	nameOfType	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecUnionDefToFp(char *nameOfType,FILE *fp);
	
/* ��ָ���ļ��ж���Ľṹ��ӡ����Ļ��
�������
	nameOfType	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecUnionDef(char *nameOfType);

#endif
