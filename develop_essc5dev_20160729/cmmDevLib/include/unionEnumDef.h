//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionEnumDef_
#define _unionEnumDef_

#include "unionVarDef.h"

#define conEnumDefTagTypeDefName	"enumName"
#define conEnumDefTagRemark		"remark"
#define conEnumDeclareLineTag		"enumName="

#include "unionConstDef.h"

// ö����������
typedef struct
{
	char			name[48+1];		// ö�ٵ�����,typedef������
	char			remark[128+1];		// ˵��
} TUnionEnumDeclareDef;
typedef TUnionEnumDeclareDef	*PUnionEnumDeclareDef;

#ifdef _maxNumOfVarPerEnumIs512_
#define conMaxNumOfVarPerEnum	512
#else
#define conMaxNumOfVarPerEnum	256
#endif
// ����ö�ٵĶ���
typedef struct
{
	TUnionEnumDeclareDef	declareDef;			// ö����������
	int			constNum;				// ö�ٵ�������
	TUnionConstDef		constGrp[conMaxNumOfVarPerEnum];	// ��Ķ���
} TUnionEnumDef;
typedef TUnionEnumDef		*PUnionEnumDef;

/* ��һ���ַ����ж�ȡö�������Ķ���
�������
	str		�������崮
	lenOfStr	�������崮�ĳ���
�������
	pdeclareDef	��������������
����ֵ��
	>=0 	������������Ŀ
	<0	�������	
	
*/
int UnionReadEnumDeclareDefFromStr(char *str,int lenOfStr,PUnionEnumDeclareDef pdeclareDef);

/* ��ָ��ö�ٵ�����ͷ��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintEnumHeaderDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp);

/* ��ָ��ö�ٵ�����β��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintEnumTailDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp);

/* ��ָ��ö�ٵĶ����ӡ���ļ���
�������
	pdef	ö�ٶ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintEnumDefToFp(PUnionEnumDef pdef,FILE *fp);

/* ��ָ���ļ��ж����ö�ٴ�ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintEnumDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж����ö�ٴ�ӡ����Ļ��
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputEnumDefInFile(char *fileName);

/* ��ָ�����Ƶ�ö�ٶ���������ļ���
�������
	nameOfType	ö������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecEnumDefToFp(char *nameOfType,FILE *fp);
	
/* ��ָ���ļ��ж����ö�ٴ�ӡ����Ļ��
�������
	nameOfType	ö������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecEnumDef(char *nameOfType);

#endif
