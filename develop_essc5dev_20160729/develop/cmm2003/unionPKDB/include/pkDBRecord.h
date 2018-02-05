#ifndef _PKDBRecord_H_
#define _PKDBRecord_H_

#ifdef _WIN32_
#include "unionComplexDBCommon.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBObjectFileName.h"
#else

// ����������
typedef char		TUnionObjectFldName[40+1];
// ��������
typedef char		TUnionObjectName[40+1];
// �������ȱʡֵ
typedef char		TUnionObjectFldDefaultValue[48+1];
typedef char		TUnionIDOfObject[32+1];

typedef int                     bool;

// ��������
typedef struct
{
	TUnionObjectFldName		name;		// ������
	int				type;		// ������
	int				size;		// ��С
	//bool				isUnique;	// �Ƿ�Ψһֵ
	bool				nullPermitted;	// �Ƿ������ֵ��1�������ֵ��0���������ֵ
	TUnionObjectFldDefaultValue	defaultValue;	// ȱʡֵ����
} TUnionObjectFldDef;
typedef TUnionObjectFldDef		*PUnionObjectFldDef;

// ���鶨��
#define conMaxFldNumPerFldGrp		8	// һ��Ψһֵ������е���������Ŀ
typedef struct
{
	//int				maxValueLen;				// ����ֵ����󳤶�
	//TUnionObjectName		fldGrpName;				// ��������
	unsigned int			fldNum;					// �����е�����Ŀ
	TUnionObjectFldName		fldNameGrp[conMaxFldNumPerFldGrp];	// �����е�������
} TUnionObjectFldGrp;
typedef TUnionObjectFldGrp		*PUnionObjectFldGrp;

// �ⲿ�ؼ��ֶ���
typedef struct
{
	TUnionObjectName		objectName;		// �ⲿ��������
	TUnionObjectFldGrp		localFldGrp;		// ��������
	TUnionObjectFldGrp		foreignFldGrp;		// �ⲿ����
} TUnionDefOfRelatedObject;
typedef TUnionDefOfRelatedObject	*PUnionDefOfRelatedObject;

// ����һ������
#define conMaxUniqueFldGrpNumPerObject	8			// ����һ����������е�����Ψһֵ��
#define conMaxParentFldGrpNumPerObject	8			// ����һ����������е��������������������ʹ��refrence��foreign key�����ָ����������ļ�ֵ������
#define conMaxFldNumPerObject		64			// ����һ����������е���������Ŀ
typedef struct
{
	TUnionObjectName		name;						// ��������
	unsigned int			fldNum;						// ������Ŀ
	TUnionObjectFldDef		fldDefGrp[conMaxFldNumPerObject];		// ����
	TUnionObjectFldGrp		primaryKey;					// �ؼ��ֶ���
	unsigned int			uniqueFldGrpNum;				// Ψһֵ�������
	TUnionObjectFldGrp		uniqueFldGrp[conMaxUniqueFldGrpNumPerObject];	// Ψһֵ��������
	unsigned int			foreignFldGrpNum;				// �ⲿ�ؼ�������
	TUnionDefOfRelatedObject	foreignFldGrp[conMaxParentFldGrpNumPerObject];	// �ⲿ�ؼ��ֶ���
} TUnionObject;
typedef TUnionObject			*PUnionObject;

// ����ʹ��һ��������ⲿ�����嵥
#define conMaxChildFldGrpNumPerObject	32				// ����һ����������е�������ö�����������������ʹ��refrence��foreign keyָ��ı�����ļ�ֵ������
typedef struct
{
	int				childNum;					// ��ֵ���������
	TUnionDefOfRelatedObject	childObjectGrp[conMaxChildFldGrpNumPerObject];	// ���ñ��������������ؼ��ֶ���
} TUnionChildObjectGrp;
typedef TUnionChildObjectGrp		PUnionChildObjectGrp;

#endif // _WIN32_

/*
���ܣ�����һ����Կ
���������
	idOfObject������
	record����Կ����ʽ��"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertPKDBRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ���ɾ��һ����Կ
���������
	idOfObject������ID
	primaryKey���ؼ��֣�"fld1=��ֵ|"
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteUniquePKDBRecord(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ��޸�һ����Կ
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionUpdateUniquePKDBRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ��ֲ�ѯһ����Կ
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"fld1=��ֵ|fld2=��ֵ|��|fldN=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniquePKDBRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord);

/*
���ܣ�����Ψһֵ��ѯһ����Կ
���������
	idOfObject������ID
	uniqueKey������ʵ����Ψһֵ����ʽΪ"Ψһֵ��1=��ֵ|Ψһֵ��2=��ֵ|��Ψһֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniquePKDBRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord);

/*
���ܣ�����ɾ����Կ
���������
	idOfObject������ID
	condition��ɾ����������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	��
����ֵ��
	>0���ɹ������ر�ɾ������Կ��Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchDeletePKDBRecord(TUnionIDOfObject idOfObject,char *condition);

/*
���ܣ������޸���Կ
���������
	idOfObject������ID
	condition���޸���������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	record��Ҫ�޸ĵ���Կ��ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ|"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	>=0���ɹ������ر��޸ĵ���Կ��Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchUpdatePKDBRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord);

/*
���ܣ�������ѯ��Կ
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	fileName���洢�˲�ѯ��������Կ��ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ������Կ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchSelectPKDBRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

#endif // _PKDBRecord_H_

