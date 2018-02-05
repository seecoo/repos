//	Author: ChenJiaMei
//	Date: 2008-8-1

#ifndef _unionComplexDBObjectSql_
#define _unionComplexDBObjectSql_

#include "unionComplexDBObjectDef.h"

/*
���ܣ�
	���һ�������ȱʡ���ͳ���
���������
	type	����
���������
	��
����ֵ��
	����
*/
int UnionGetDefaultSizeOfTBLFieldType(int type);

/*
���ܣ�
	���һ����������ռ�ڴ��С
���������
	��
���������
	��
����ֵ��
	���һ�����嶨����ռ�ڴ�Ĵ�С
*/
int UnionGetSizeOfObjectDef();

// ���ܣ���ָ�����ж�һ������Ķ���
/*
���������
	str		�ַ���
	lenOfStr	�ַ�������
���������
	pfldGrp		���������鶨�壬����Ԥ�ȷ���ռ�
����ֵ��
	�ɹ������ض������ַ�����ռ���ַ���
	ʧ�ܣ����ش�����
*/
int UnionGetObjectFldGrpDefFromStr(char *str,int lenOfStr,PUnionObjectFldGrp pfldGrp);

// ���ܣ���ָ���ַ����ж�������
/*
���������
	fldStr		���崮
	lenOfFldStr	���崮����
���������
	pfldDef		���������壬����Ԥ�ȷ���ռ�
����ֵ��
	�ɹ���0
	ʧ�ܣ����ش�����
*/
int UnionGetObjectFldDefFromStr(char *fldStr,int lenOfFldStr,PUnionObjectFldDef pfldDef);

/*
���ܣ��Ӷ��󴴽��ļ����һ������Ķ���
���������
	idOfObject������ID
���������
	pobject�������Ķ����壬����Ԥ�ȷ���ռ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObjectDefFromCreateSqlFile(TUnionIDOfObject idOfObject,PUnionObject pobject);

// �ж��Ƿ�Ϸ����������
/*
���������
	varName		�������
���������
	��
����ֵ��
	1	�Ϸ�������
	0	�Ƿ�������
*/
int UnionIsValidObjectFldName(char *varName);

// ���ܣ���������е����Ƿ��ڶ����ж�����
/*
���������
	pfldGrp		Ҫ��������
	pobject		�ڸö����м��
���������
	��
����ֵ��
	�ɹ���>= 0
	ʧ�ܣ�<0��������
*/
int UnionCheckFldGrpInObject(PUnionObjectFldGrp pfldGrp,PUnionObject pobject);

/*
���ܣ��ж�һ����ֵ�Ƿ�����һ����ֵ������
���������
	pfldGrpChild��Ҫ��������
	pfldGrpParent���ڸ������м��
���������
	��
����ֵ��
	1����
	0������
	<0��ʧ�ܣ����ش�����
*/
int UnionIsChildFldGrpOfFldGrp(PUnionObjectFldGrp pfldGrpChild,PUnionObjectFldGrp pfldGrpParent);

/*
���ܣ��ж����������Ƿ��ظ�
���������
	fldGrp1������1
	fldGrp2������2
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ����ش�����
*/
int UnionFldGrpIsRepeat(PUnionObjectFldGrp fldGrp1,PUnionObjectFldGrp fldGrp2);

/*
���ܣ��ж����������Ƿ���ȫһ�£�����������������˳���ж�
���������
	fldGrp1������1
	fldGrp2������2
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ����ش�����
*/
int UnionFldGrpIsCompleteRepeat(PUnionObjectFldGrp fldGrp1,PUnionObjectFldGrp fldGrp2);

/*
���ܣ��ж������Ƿ�Ϸ�
���������
	fldDef������
���������
	��
����ֵ��
	1���Ϸ�
	0�����Ϸ�
	<0��ʧ�ܣ����ش�����
*/
int UnionIsValidObjectFieldDef(TUnionObjectFldDef fldDef);

// Mary add begin, 2008-9-5
/*
���ܣ���һ��������Ĵ�д����󴴽��ļ�
���������
	idOfObject������ID
	objSqlStr�������崮
	objSqlStrLen�������崮�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObjectSqlFileByString(TUnionIDOfObject idOfObject,char *objSqlStr,int objSqlStrLen);
// Mary add end, 2008-9-5

/*
���ܣ��������壬д�뵽�����ļ���
���������
	pobject��ָ��������ָ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateComplexDBTBLCreateSqlFile(PUnionObject pobject);

#endif
