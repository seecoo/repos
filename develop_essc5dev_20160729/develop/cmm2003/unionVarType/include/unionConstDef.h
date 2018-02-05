// wolfang wang
// 2008/10/3

#ifndef _unionConstDef_
#define _unionConstDef_

#define conConstDefTagValue		"value"
#define conConstDefTagName		"name"
#define conConstDefTagType		"type"
#define conConstDefTagRemark		"remark"

typedef struct
{
	char			name[64+1];		// ��������
	char			value[128+1];		// ֵ
	int			type;			// ����
	char			remark[128+1];		// ˵��
} TUnionConstDef;
typedef TUnionConstDef	*PUnionConstDef;

/*
����	
	��ȱʡ�����ļ���ȡ����������ֵ
�������
	constName	��������
�������
	constValue	����ֵ
����ֵ
	>=0		����ֵ
	<0		�������
*/
int UnionCalculateIntConstValue(char *constName);

/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadConstDefFromStr(char *str,int lenOfStr,PUnionConstDef pdef);

/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintConstDefToFp(PUnionConstDef pdef,FILE *fp);

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintConstDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputConstDefInFile(char *fileName);

/* ��ȱʡ�����ļ���ָ�����Ƶĳ����Ķ���
�������
	nameOfConst	��������
�������
	pdef	�����ĳ�������
����ֵ��
	>=0 	�����ĳ����Ĵ�С
	<0	�������	
	
*/
int UnionReadConstDefFromDefaultDefFile(char *nameOfConst,PUnionConstDef pdef);

/* ��ָ�����Ƶĳ�������������ļ���
�������
	nameOfConst	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecConstDefToFp(char *nameOfConst,FILE *fp);
	
/* ��ָ���ļ��ж���ĳ�����ӡ����Ļ��
�������
	nameOfConst	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecConstDef(char *nameOfConst);

#endif
