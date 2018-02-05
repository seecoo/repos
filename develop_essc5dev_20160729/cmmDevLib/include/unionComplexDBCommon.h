//	Author: ChenJiaMei
//	Date: 2008-8-1

#ifndef _unionComplexDBCommon_
#define _unionComplexDBCommon_

#include "unionComplexDBObjectDef.h"

#define SeparableSignOfFieldNameGroup		':'		// Mary add, 2008-9-9
#define SeparableSignOfFieldValueGroup		'.'		// Mary add, 2008-9-9
#define conSeparableSignOfFieldNameInFldList	','		// ���������ӣ�2009/4/21
#define conSeparableSignOfLineInMultiRow	'^'		// ���������ӣ�2009/5/2

// �������ֵ����
typedef enum
{
	conObjectFldType_String = 1,	// �ַ���
	conObjectFldType_Int = 2,	// ����
	conObjectFldType_Double = 3,	// �ж���С��λ��˫������
	conObjectFldType_Bit = 4,	// ������
	conObjectFldType_Bool = 5,	// BOOL��
	conObjectFldType_Long = 6,	// ������
} TUnionObjectFldValueType;
typedef TUnionObjectFldValueType	*PUnionObjectFldValueType;

// ���ܣ���һ��������ת��Ϊ������������
/*
���������
	type		������
���������
	fldTypeName	����������
����ֵ��
	�ɹ���>=0
	ʧ�ܣ�<0��������
*/
int UnionGetFldTypeName(TUnionObjectFldValueType type,char *fldTypeName);

/*
���ܣ�ȥ���ַ������ұߵĶ����ַ�
���������
	Str���ַ���
	lenOfStr���ַ����ĳ���
	ch��Ҫȥ���Ķ����ַ�
���������
	Str��ȥ�������ַ�����ַ���
����ֵ��
	>=0���ɹ����������ַ����ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFilterRightChar(char *Str,int lenOfStr,char ch);

/*
���ܣ�ȥ���ַ�������ߵĶ����ַ�
���������
	Str���ַ���
	lenOfStr���ַ����ĳ���
	ch��Ҫȥ���Ķ����ַ�
���������
	Str��ȥ�������ַ�����ַ���
����ֵ��
	>=0���ɹ����������ַ����ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFilterLeftChar(char *Str,int lenOfStr,char ch);

/*
���ܣ���"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"���ָ�ʽ���У���ֳ�������
���������
	fieldNameStr����ʽ������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
	lenOfStr����ʽ���ĳ���
���������
	fldGrp��������ṹ
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormFldGrpFromRecordStr(char *recordStr,int lenOfStr,PUnionObjectFldGrp fldGrp);

/*
���ܣ���"����1.����2.����3��"���ָ�ʽ���У���ֳ�������
���������
	fieldNameStr����ʽ������ʽΪ"����1.����2.����3��"
	lenOfStr����ʽ���ĳ���
���������
	fldGrp��������ṹ
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormFldGrpFromFieldNameStr(char *fieldNameStr,int lenOfStr,PUnionObjectFldGrp fldGrp);

/*
���ܣ���������ṹƴ�ɸ�ʽ��"����1.����2.����3��"
���������
	fldGrp��������ṹ
���������
	fieldNameStr����ʽ������ʽΪ"����1.����2.����3��"
����ֵ��
	>=0���ɹ������ظ�ʽ���ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormFieldNameStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *fieldNameStr);

/*
���ܣ���������ṹ�ͼ�¼ֵƴ�ɸ�ʽ��"��ֵ1.��ֵ2.��ֵ3��"
���������
	fldGrp��������ṹ
	record����¼������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
	lenOfRecord����¼���ĳ���
���������
	fieldValueStr����ʽ������ʽΪ"��ֵ1.��ֵ2.��ֵ3��"
����ֵ��
	>=0���ɹ������ظ�ʽ��fieldValueStr�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormFieldValueStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *record,int lenOfRecord,char *fieldValueStr);

/*
���ܣ���������ṹ�ͼ�¼ֵƴ�ɸ�ʽ��"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
���������
	fldGrp��������ṹ
	record����¼������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
	lenOfRecord����¼���ĳ���
	sizeOfRecStr��fieldRecStr�Ĵ洢�ռ��С
���������
	fieldRecStr����ʽ������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
����ֵ��
	>=0���ɹ������ظ�ʽ��fieldRecStr�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormFieldRecordStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *record,int lenOfRecord,char *fieldRecStr,int sizeOfRecStr);

/*
���ܣ���"��1=��ֵ|��2=��ֵ|��"���ָ�ʽ��ƴ��"��ֵ1.��ֵ2.��ֵ3��"�ĸ�ʽ��
���������
	commonStr�����ʽ������ʽΪ"��1=��ֵ|��2=��ֵ|��"
	lenOfStr�����ʽ���ĳ���
���������
	fieldValueStr����ֵ��ʽ������ʽΪ"��ֵ1.��ֵ2.��ֵ3��"
����ֵ��
	>=0���ɹ���������ֵ��ʽ���ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormValueStrFromCommonStr(char *commonStr,int lenOfStr,char *fieldValueStr);

/*
���ܣ��ж���ĸ�ֵ�Ƿ�Ϸ�
���������
	fieldDef������
	fieldValue����ֵ
	lenOfValue����ֵ�ĳ���
���������
	��
����ֵ��
	1���Ϸ�
	0�����Ϸ�
	<0��ʧ�ܣ����ش�����
*/
int UnionIsValidObjectFieldValue(TUnionObjectFldDef fieldDef,char *fieldValue,int lenOfValue);

/*
���ܣ���ʵ���Ĵ洢λ�õõ�ʵ���Ĺؼ���
���������
	storePosition��ʵ���Ĵ洢λ��
	lenOfPosition��storePosition�ĳ���
���������
	primaryKey��ʵ���Ĺؼ���
����ֵ��
	>=0���ɹ������عؼ��ֵĳ���
	<0��ʧ�ܣ����ش�����
*/
int UnionGetRecordPrimaryKeyFromStorePosition(char *storePosition,int lenOfPosition,char *primaryKey);
#endif
