#ifndef _UnionComplexDBAPIOnUnionEnv_H_
#define _UnionComplexDBAPIOnUnionEnv_H_

#ifdef _WIN32_
#include "unionComplexDBCommon.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBObjectFileName.h"
#else

#include "unionComplexDBObjectDef.h"	// added by wolfgang wang,2009/5/29
/*
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
*/

/*
���ܣ�����һ�������ʵ��
���������
	idOfObject������
	record����ʽ1��"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertObjectRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ���ɾ��һ�������ʵ��
���������
	idOfObject������ID
	primaryKey��"fld1=��ֵ|"
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ��޸�һ�������ʵ��
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
int UnionUpdateUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ��ֲ�ѯһ�������ʵ��
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
int UnionSelectUniqueObjectRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord);


/*
���ܣ�����Ψһֵ��ѯһ�������ʵ��
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
int UnionSelectUniqueObjectRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord);

/*
���ܣ�����ɾ��һ�������ʵ��
���������
	idOfObject������ID
	condition��ɾ����������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	��
����ֵ��
	>0���ɹ������ر�ɾ����ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchDeleteObjectRecord(TUnionIDOfObject idOfObject,char *condition);

/*
���ܣ������޸�һ�������ʵ��
���������
	idOfObject������ID
	condition���޸���������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ|"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	>=0���ɹ������ر��޸ĵ�ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchUpdateObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord);

/*
���ܣ�������ѯһ�������ʵ��
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	fileName���洢�˲�ѯ������ʵ����ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ���ļ�¼����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchSelectObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

/*
���ܣ�ƴװһ���ؼ���������
���������
	idOfObject��	Ҫ���Ķ���ID	
	primaryKeyValueList	�ؼ�ֵ�ִ�,������ֵ��.�ֿ�
	lenOfValueList		�ؼ�ֵ�ִ�����
	sizeOfBuf	�ؼ����嵥�Ĵ�С
���������
	recStr		�ؼ���������
����ֵ��
	0��	�ؼ����������ĳ���
	<0��	ʧ�ܣ����ش�����
*/
int UnionFormObjectPrimarKeyCondition(TUnionIDOfObject idOfObject,char *primaryKeyValueList,int lenOfValueList,char *recStr,int sizeOfBuf);

/*
���ܣ�
	��һ����¼��ƴװ�ؼ���
���������
	idOfObject��		����ID
	record��		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��		record�ĳ���
	sizeOfPrimaryKey	�ؼ�ֵ�������С
���������
	primaryKey		�ؼ�ֵ������.�ֿ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormPrimaryKeyStrOfSpecObjectRec(TUnionIDOfObject idOfObject,char *record,int lenOfRecord,char *primaryKey,int sizeOfPrimaryKey);

/*
���ܣ���ö����ʵ����Ŀ
���������
	idOfObject������ID
���������
	��
����ֵ��
	>=0��ʵ����
	<0��ʧ�ܣ�������
*/
int UnionGetObjectRecordNumber(TUnionIDOfObject idOfObject);

int UnionReadObjectDef(TUnionIDOfObject idOfObject,PUnionObject pobject);

int UnionExistsObjectDef(TUnionIDOfObject idOfObject);

int UnionCreateObject(TUnionIDOfObject idOfObject);

int UnionDropObject(TUnionIDOfObject idOfObject);

/*
���ܣ�
	ִ��һ����Ĵ���������
���������
	resName		��������
	operationID	������ʶ
	isBeforeOperation	�ǲ���֮ǰִ�л���֮��ִ�У�1 ��ʾ����֮ǰ
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr);

/*
���ܣ�
	ִ��һ��������ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecOnObject(char *resName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

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

int UnionBatchSelectUnionObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

#endif

#endif

