//	Wolfgang Wang
//	2008/11/15

// ��������ĸ�ֵ����

#ifndef _unionVarAssignment_
#define _unionVarAssignment_

#define conVarAssignmentMethodValue		"value"
#define conVarAssignmentMethodFunVar		"fun"
#define conVarAssignmentMethodGolobalVar	"var"
#define conVarAssignmentMethodInput		"input"

#define conVarAssignmentMethodFldVarValue	"value"
#define conVarAssignmentMethodFldVarName	"var"
#define conVarAssignmentMethodFldVarLen		"len"
#define conVarAssignmentMethodFldVarIsBinary	"isBinary"
#define conVarAssignmentMethodFldVarFunIndex	"index"

typedef struct
{
	char		varName[128+1];		// ��������
	char		method[128+1];		// ������ֵ����
	char		value[4096+1];		// ������ֵ
	int		isVarName;		// ������ֵ�ǲ�����һ������������
	char		lenTag[128+1];		// ����ֵ�ĳ���
	int		isBinary;		// ������ֵ�ǲ��Ƕ�����,1��,0��
} TUnionVarAssignmentDef;
typedef TUnionVarAssignmentDef	*PUnionVarAssignmentDef;

/*
����	�ж��Ƿ��ǺϷ��ı�����ֵ����
�������
	method		����
�������
        ��
����ֵ
	1		�Ϸ�
	0		���Ϸ�
*/
int UnionIsValidVarAssignmentMethod(char *method);

/*
����	��ָ�������ĸ�ֵ����
�������
	str         	��
        lenOfStr  	������
        sizeOfBuf	���ո�ֵ������ֵ�Ļ���Ĵ�С
�������
        varName		��������
	method		�����ĸ�ֵ����
	value		��ֵ����ʹ�õ�ֵ
����ֵ
	>=0		��ֵ�����ĳ���
	<0		�������
*/
int UnionReadVarValueAssignMethod(char *str,int lenOfStr,char *varName,char *method,char *value,int sizeOfBuf);

/*
����	
	��һ�����ж�ȡ�����ĸ�ֵ����
�������
	str		��
	lenOfStr	������
�������
	pdef		��ֵ����
����ֵ
	>=0		�ɹ�,ֵ�ĳ���
	<0		�������
*/
int UnionReadVarAssignmentDefFromStr(char *str,int lenOfStr,PUnionVarAssignmentDef pdef);

/*
����	
	��һ����������д�뵽ָ���ļ���
�������
	pdef		��ֵ����
	fp		ָ���ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarAssignmentDefToFile(PUnionVarAssignmentDef pdef,FILE *fp);

#endif
