//	Author: ������
//	Date: 2008-11-29

// ������ѧ�����
#ifndef _unionCalcuOperator_
#define _unionCalcuOperator_

#define conCalcuOperatorTagNot			"NOT"
#define conCalcuOperatorTagNotOfCProgram	"!"
#define conCalcuOperatorTagAnd			"AND"
#define conCalcuOperatorTagAndOfCProgram	"&&"
#define conCalcuOperatorTagOr			"OR"
#define conCalcuOperatorTagOrOfCProgram		"||"

typedef enum
{
	conCalcuOperatorNot = 1,
	conCalcuOperatorAnd = 2,
	conCalcuOperatorOr = 3,
} TUnionCalcuOperator;

/*��
����
	��һ���������ת��Ϊ�ڲ���ʶ
�������
	operatorTag	�����
�������
	��
����ֵ
	>=0		�ڲ���ʾ�������
	<0		�������
*/
int UnionConvertCalcuOperatorStrTagIntoIntTag(char *operatorTag);

/*
����	��һ����ϵ����ת��Ϊ��ϵ����
�������
	compType	��ϵ����
�������
	compTag		��ϵ����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionConvertSpecRelationCompTypeIntoDBRelationCompTag(int compType,char *compTag);

/*
����	��һ����ϵ����ת��Ϊ��ֵ����
�������
	compType	��ϵ����
�������
	compTag		��ϵ����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionConvertSpecRelationCompTypeIntoDBValueAssignTag(int compType,char *compTag);

#endif


