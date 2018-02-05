//	Author: Wolfgang Wang
//	Date: 2008/3/6

#ifndef _unionComplexDBObjectDef_
#define _unionComplexDBObjectDef_

// ����������
typedef char		TUnionObjectFldName[40+1];
// ��������
typedef char		TUnionObjectName[40+1];
// �������ȱʡֵ
typedef char		TUnionObjectFldDefaultValue[48+1];
typedef char		TUnionIDOfObject[32+1];

#ifndef _boolDefinedAlready_
#define _boolDefinedAlready_
#ifndef _WIN32_
typedef int                     bool;
#endif
#endif

// ��������
typedef struct
{
	TUnionObjectFldName		name;		// ������
	int				type;		// ������
	int				size;		// ��С
	//bool				isUnique;	// �Ƿ�Ψһֵ
	bool				nullPermitted;	// �Ƿ������ֵ��1�������ֵ��0���������ֵ
	TUnionObjectFldDefaultValue	defaultValue;	// ȱʡֵ����
	char				remark[128+1];	// ˵��
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

#define ObjectDefFlagOfObjectName		"objectName:"
#define ObjectDefFlagLenOfObjectName		11
#define ObjectDefFlagOfFldDefGrp		"fldDefGrp:"
#define ObjectDefFlagLenOfFldDefGrp		10
#define ObjectDefFlagOfPrimaryKey		"primaryKey:"
#define ObjectDefFlagLenOfPrimaryKey		11
#define ObjectDefFlagOfUniqueFldGrp		"uniqueFldGrp:"
#define ObjectDefFlagLenOfUniqueFldGrp		13
#define ObjectDefFlagOfForeignFldGrp		"foreignFldGrp:"
#define ObjectDefFlagLenOfForeignFldGrp		14

void UnionInitObjectDef(PUnionObject pdef);

/*
function:
        �ж��Ƿ��Ƕ����е��ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               �ǹؼ��ֶ�
        0               ���ǹؼ��ֶ�
*/
//int UnionIsFldOfObject(PUnionTableDef pobject, char *fldName);

/*
���ܣ�
	���������������ת��Ϊʵ�����ݿ��е�����
���������
	pobject��		ָ��������ָ��
	defFldNameGrp		����ʱ���õ�����
���������
	realFldNameGrpInDB	ʵ�����ݿ��е�����	
����ֵ��
	>=0��	����ת��������������
	<0��	ʧ�ܣ����ش�����
*/
//int UnionConvertObjectFldNameInNameGrp(PUnionTableDef pobject,char defFldNameGrp[][128+1],int numOfFld,char realFldNameGrpInDB[][128+1]);

/*
���ܣ�
	���������������ת��Ϊʵ�����ݿ��е�����
���������
	idOfObject		��������
	defFldNameGrp		����ʱ���õ�����
���������
	realFldNameGrpInDB	ʵ�����ݿ��е�����	
����ֵ��
	>=0��	����ת��������������
	<0��	ʧ�ܣ����ش�����
*/
int UnionConvertObjectFldNameOfSpecObjectInNameGrp(TUnionIDOfObject idOfObject,char defFldNameGrp[][128],int numOfFld,char realFldNameGrpInDB[][128]);

/*
���ܣ��Ӷ������ļ�����һ��������Ĺؼ����嵥
���������
	idOfObject��Ҫ���Ķ���ID	
	sizeOfBuf	�ؼ����嵥�Ĵ�С
���������
	primaryKeyList	�ؼ����嵥��������֮����,�ֿ�
����ֵ��
	0���ؼ��ִ��ĳ���
	<0��ʧ�ܣ����ش�����
*/
int UnionReadObjectPrimarKeyList(TUnionIDOfObject idOfObject,char *primaryKeyList,int sizeOfBuf);

/*
����	
	������ݿ������󳤶�
�������
	fldSize		�򳤶�
�������
	��
����ֵ
	��ĺϷ�����
*/
int UnionGetValidFldSizeOfDatabase(int fldSize);

// ��������
/*
���ܣ����һ������Ķ����Ƿ����
���������
	idOfObject������ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectDef(TUnionIDOfObject idOfObject);

/*
���ܣ��ж�һ������Ķ����Ƿ�Ϸ�
���������
	objectDef��������
���������
	��
����ֵ��
	1���Ϸ�
	0�����Ϸ�
	<0��ʧ�ܣ�������
*/
int UnionIsValidObjectDef(TUnionObject objectDef);

/*
���ܣ��������壬д�뵽�������ļ���
���������
	pobject��ָ��������ָ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
//int UnionStoreObjectDef(PUnionTableDef pobject);

/*
���ܣ��������壬д�뵽�������ļ���
���������
	pobject��ָ��������ָ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
//int UnionStoreObjectDefAnyway(PUnionTableDef pobject);

/*
���ܣ��������壬д�뵽ָ���ļ���
���������
	pobject��	ָ��������ָ��
	fileName	�ļ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionPrintObjectDefToSpecFile(PUnionObject pobject,char *fileName);

/*
���ܣ��Ӷ������ļ�����һ��������
���������
	idOfObject��Ҫ���Ķ���ID
���������
	pobject��ָ��������ָ��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionReadObjectDef(TUnionIDOfObject idOfObject,PUnionObject pobject);

/*
���ܣ��Ӷ������ļ�����һ��������
���������
	idOfObject��Ҫ���Ķ���ID
	withLock	�Ƿ�Ӷ�����1,������0,������
���������
	pobject��ָ��������ָ��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionReadObjectDefWithLock(TUnionIDOfObject idOfObject,PUnionObject pobject,int withLock);

/*
����	
	��һ������д�ɴ�
�������
	flag		����ı�ʶ
	pfldGrp		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreateFldGrpIntoSQLToFp(char *flag,PUnionObjectFldGrp pfldGrp,FILE *fp);

/*
����	
	��һ�����崴��SQL���
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreateSQLFromObjectDefToFp(PUnionObject prec,FILE *fp);

/*
���ܣ�
	���һ����������������
���������
	pobject��	ָ��������ָ��
	fldName		������
���������
	fldTypeName	���c��������
����ֵ��
	>=0��	����
	<0��	ʧ�ܣ����ش�����
*/
//int UnionGetTypeOfSpecFldFromObjectDef(PUnionTableDef pobject,char *fldName,char *fldTypeName);

/*
���ܣ�
	ת��һ�������������
���������
	pobject��	ָ��������ָ��
	oriFldName	������
���������
	defName		����ʱ���õ�����
����ֵ��
	>=0��	����
	<0��	ʧ�ܣ����ش�����
*/
//int UnionConvertObjectFldName(PUnionTableDef pobject,char *oriFldName,char *defName);

/*
���ܣ�
	ת��һ�������������
���������
	objectName	��������
	oriFldName	������
���������
	defName		����ʱ���õ�����
����ֵ��
	>=0��	����
	<0��	ʧ�ܣ����ش�����
*/
int UnionConvertObjectFldNameOfSpecObject(char *objectName,char *oriFldName,char *defName);

/*
����	
	����һ�ű�Ĺؼ���
�������
	tblName	����
�������
	primaryKey	�����Ĺؼ��ִ���2���ؼ���֮����,�ָ�
����ֵ
	>=0	�ɹ�,�ؼ��ִ��ĳ���
	<0	������
*/
int UnionReadPrimaryKeyOfSpecObject(char *tblName,char *primaryKey);

int UnionConvertTBLFldTypeIntoName(int type,char *typeName);

#endif
