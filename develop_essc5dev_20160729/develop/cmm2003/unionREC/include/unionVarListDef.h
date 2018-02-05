// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionVarListDef_
#define _unionVarListDef_

#define conMaxNumOfVarPerList	128

typedef struct
{
	char	*varName;
	int	len;
	char	*varValue;
} TUnionVarValue;
typedef TUnionVarValue	*PUnionVarValue;

typedef struct
{
	char		varListName[40+1];
	int		varNum;
	PUnionVarValue	varList[conMaxNumOfVarPerList];
} TUnionVarList;
typedef TUnionVarList	*PUnionVarList;

/*
����
	��һ�������б�д����־��
���������
	pvarList	�����б�
	title		����
�������
	��
����ֵ
	��
*/
void UnionLogVarList(PUnionVarList pvarList,char *title);

/*
����
	��ʼ��һ������
���������
	varName		��������
	len		����ֵ
	varValue	����ֵ
�������
	��
����ֵ
	�ɹ�	����ָ��
	ʧ��	NULL
*/
PUnionVarValue UnionInitOneVar(char *varName,int len,char *varValue);

/*
����
	�ͷ�һ������
���������
	ptr		����ָ��
�������
	��
����ֵ
	��
*/
void UnionFreeOneVar(PUnionVarValue ptr);

/*
����
	�Ǽ�һ������
���������
	pvarList	�����б�
	varName		��������
	len		����ֵ
	varValue	����ֵ
�������
	��
����ֵ
	>=0		��������
	<0		�������
*/
int UnionAddOneVarToVarList(PUnionVarList pvarList,char *varName,int len,char *varValue);

/*
����
	�ƻ�һ�������б�
���������
	pvarList	�����б�
�������
	��
����ֵ
	��
*/
void UnionFreeVarList(PUnionVarList pvarList);

/*
����
	����һ�������б�
���������
	varListName	�б�����
�������
	��
����ֵ
	�ɹ�	�����б�ָ��
	ʧ��	NULL
*/
PUnionVarList UnionCreateVarList(char *varListName);

/*
����
	��ȡһ������
���������
	pvarList	�����б�
	varName		��������
	sizeOfBuf	���������С
�������
	varValue	����ֵ
����ֵ
	>=0		��������
	<0		�������
*/
int UnionReadOneVarByVarName(PUnionVarList pvarList,char *varName,char *varValue,int sizeOfBuf);

#endif
