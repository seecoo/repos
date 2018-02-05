//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionPointerDef_
#define _unionPointerDef_

#define conPointerDefTagVarName		"name"
#define conPointerDefTagNameOfType	"nameOfType"
#define conPointerDefTagRemark		"remark"

// ����ָ��Ķ���
typedef struct
{
	char		name[64+1];		// ָ�����͵�����
	char		nameOfType[64+1];	// ָ�������
	char		remark[80+1];
} TUnionPointerDef;
typedef TUnionPointerDef	*PUnionPointerDef;

/* ��ȱʡ�ļ���ָ�����Ƶ�ָ��Ķ���
�������
	nameOfType	ָ�����͵�����
�������
	typeNameOfPointer	ָ�����͵�����
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
*/
int UnionReadTypeOfPointerDefFromDefaultDefFile(char *nameOfType,char *typeNameOfPointer);

/* ��ָ��ָ��Ķ����ӡ���ļ���
�������
	pdef	ָ�붨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintPointerDefToFp(PUnionPointerDef pdef,FILE *fp);

/* ��ָ��ָ��Ķ����ӡ�������ļ���
�������
	pdef	ָ�붨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintPointerDefToFpInDefFormat(PUnionPointerDef pdef,FILE *fp);

/* ��ָ���ļ��ж����ָ���ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintPointerDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж����ָ���ӡ����Ļ��
�������
	pdef	ָ�붨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputPointerDefInFile(char *fileName);

/* ��ָ�����Ƶ�ָ�����Ͷ���������ļ���
�������
	nameOfType	ָ����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecPointerDefToFp(char *nameOfType,FILE *fp);
	
/* ��ָ���ļ��ж����ָ�����ʹ�ӡ����Ļ��
�������
	nameOfType	ָ����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecPointerDef(char *nameOfType);

#endif
