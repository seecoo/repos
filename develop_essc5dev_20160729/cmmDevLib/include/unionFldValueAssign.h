//---------------------------------------------------------------------------

#ifndef unionFldValueAssignH
#define unionFldValueAssignH
//---------------------------------------------------------------------------

#define conFldValueSetMethodTagAutoSetBySoftware        "auto"       // ϵͳ�Զ���ֵ
#define conFldValueSetMethodTagUserInput                "null"       // �û���������
#define conFldValueSetMethodDefaultSet                  "default::"  // ȱʡֵ
#define conFldValueSetMethodRefrenceOtherTBLFld         "query::"    // ������������
#define conFldValueSetMethodEnumGrp                     "enum::"     // ö��ֵ

#include "unionFldGrp.h"

#define 	conMaxNumOfFldPerTBL		64

// ����һ����ĸ�ֵ������
typedef struct
{
	int		fldNum;
	TUnionFldName	fldName[conMaxNumOfFldPerTBL];			// ����
	TUnionFldName	fldAlais[conMaxNumOfFldPerTBL];			// �����
	char		fldValue[conMaxNumOfFldPerTBL][128+1];		// ��ֵ����
} TUnionFldValueAssignMethodGrp;
typedef TUnionFldValueAssignMethodGrp	*PUnionFldValueAssignMethodGrp;

/*
����	���ļ��ж�ȡ��ֵ��������
�������
	fileName	�ļ���
�������
	prec		��ֵ��������
����ֵ
	>=0		��ֵ��������Ŀ
	<0		�������
*/
int UnionReadFldValueAssignMethodGrpDef(char *fileName,PUnionFldValueAssignMethodGrp prec);

/*
����	��ָ�����ָ����ĸ�ֵ����
�������
	tblName         �����
        isFldAlaisUsed  ���������������������  1�����������0��������
        fldTag          �����������
        sizeOfBuf       �����С
�������
	method		�����ĸ�ֵ����
����ֵ
	>=0		��ֵ�����ĳ���
	<0		�������
*/
int UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL(char *tblName,int isFldAlaisUsed,char *fldTag,char *method,int sizeOfBuf);


//---------------------------------------------------------------------------
/*
����	����Ԥ����Ĳ�ѯ���崮��ñ���ֵ
�������
	conditionStr            ��ѯ���崮
        lenOfConditionStr       ��ѯ���崮����
        maxValueNum             ���ɶ���ֵ��Ŀ
�������
	varGrp		������ֵ
����ֵ
	>=0		������ֵ��Ŀ
	<0		�������
*/
int UnionGetAvailableValueGrpByQueryCondition(char *conditionStr,int lenOfConditionStr,char varGrp[][128+1],int maxValueNum);

//---------------------------------------------------------------------------
/*
����	���ĳ���ĳ���ȡֵ��
�������
	tblName		����
        fldAlais        �����
        maxValueNum     ���ɶ�����ֵ��Ŀ
�������
	varGrp		������ֵ
����ֵ
	>=0		������ֵ��Ŀ
	<0		�������
*/
int UnionGetAvailableValueGrp(char *tblName,char *fldAlais,char varGrp[][128+1],int maxValueNum);

//---------------------------------------------------------------------------
/*
����	������ֵ��������ֵת��Ϊ��ʾ��ʽ
�������
	method          ��ֵ����
        value           ԭֵ
        lenOfValue      ԭֵ����
        sizeOfBuf       ���ջ����С
�������
	displayValue    ��ʾֵ
����ֵ
	>=0		��ʾֵ����
	<0		�������
*/
int UnionConvertFldValueIntoDisplayFormat(char *method,char *value,int lenOfValue,char *displayValue,int sizeOfBuf);

//---------------------------------------------------------------------------
/*
����	������ֵ��������ֵ����ʾ��ʽת��Ϊʵ��ֵ
�������
	method          ��ֵ����
	displayValue    ��ʾֵ
        lenOfDisplayValue      ԭֵ����
        sizeOfBuf       ���ջ����С
�������
        value           ԭֵ
����ֵ
	>=0		��ʾֵ����
	<0		�������
*/
int UnionConvertFldValueFromDisplayFormat(char *method,char *displayValue,int lenOfDisplayValue,char *value,int sizeOfBuf);

#endif
