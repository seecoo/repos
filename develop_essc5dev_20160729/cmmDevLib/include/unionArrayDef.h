//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionArrayDef_
#define _unionArrayDef_

#define conArrayDefTagVarName		"name"
#define conArrayDefTagNameOfType	"nameOfType"
#define conArrayDefTagDimisionNum	"dimisionNum"
#define conArrayDefTagSizeOfDimision	"sizeOfDimision"
#define conArrayDefTagRemark		"remark"


#ifndef conMaxNumOfArrayDimision
#define conMaxNumOfArrayDimision	4
#endif

// ��������Ķ���
typedef struct
{
	char		name[64+1];		// �������͵�����
	char		nameOfType[64+1];	// ���������
	int		dimisionNum;		// �����ά�ȣ�0��ʾ��������
	char		sizeOfDimision[conMaxNumOfArrayDimision][64+1];	// ÿһά�ȵĴ�С
	char		remark[80+1];
} TUnionArrayDef;
typedef TUnionArrayDef	*PUnionArrayDef;

/* ��ָ������Ķ����ӡ���ļ���
�������
	pdef	���鶨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintArrayDefToFp(PUnionArrayDef pdef,FILE *fp);

/* ��ָ���ļ��ж���������ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintArrayDefInFileToFp(char *fileName,FILE *fp);
	
/* ��ָ���ļ��ж���������ӡ����Ļ��
�������
	pdef	���鶨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputArrayDefInFile(char *fileName);

/* ��ָ�����Ƶ����鶨��������ļ���
�������
	nameOfType	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecArrayDefToFp(char *nameOfType,FILE *fp);
	
/* ��ָ���ļ��ж���������ӡ����Ļ��
�������
	nameOfType	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecArrayDef(char *nameOfType);

#endif
