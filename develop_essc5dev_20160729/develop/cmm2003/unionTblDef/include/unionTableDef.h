//	zhang yong ding
//	2012/12/07
//	Version 1.0

#ifndef _UnionTableDef_
#define _UnionTableDef_

#define conMDLNameOfTableDefTBL		"UnionTableDefMDL"
#define conMaxNumOfTableFieldDef	64
#define conMaxNumOfTableDef		128
#define conMaxFieldNumOfKeyGroup	10

typedef struct
{
	char				fieldName[40+1];			// �ֶ���
	char				fieldChnName[40+1];			// �ֶ�������
	int				fieldType;				// �ֶ�����
	float				fieldSize;				// �ֶδ�С
	char				defaultValue[128+1];			// ȱʡֵ
	int				isPrimaryKey;				// 1���ǣ�0����
	int				isNull;					// 1���ǣ�0����
} TUnionTableFieldDef;
typedef TUnionTableFieldDef		*PUnionTableFieldDef;

typedef struct
{
	char				tableName[40+1];			// ����
	char				tableChnName[40+1];			// ��������
	char				tableType[20+1];			// ������
	int				methodOfCached;				// ���淽��,0����׼��1�����Ի�
	int				fieldNum;				// �ֶ���Ŀ
	TUnionTableFieldDef		fieldDef[conMaxNumOfTableFieldDef];	// �ֶζ���
} TUnionTableDef;
typedef TUnionTableDef			*PUnionTableDef;

typedef struct
{
	int				tableNum;				// ʵ������
	TUnionTableDef			rec[conMaxNumOfTableDef];		// ���ֵ
} TUnionTableDefTBL;
typedef TUnionTableDefTBL		*PUnionTableDefTBL;

typedef struct
{
	char				tableName[40+1];			// ����
	int				dbIndex;				// ���ݿ�����
}TUnionDBIndex;
typedef TUnionDBIndex			*PUnionDBIndex;

typedef struct
{
	int				tableNum;				// ʵ������
	TUnionDBIndex			rec[conMaxNumOfTableDef];		// ���ֵ
} TUnionDBIndexTBL;
typedef TUnionDBIndexTBL		*PUnionDBIndexTBL;

int UnionGetFileDirOfTableDef(char *fileName);

int UnionGetFileNameOfTableDef(char *tableName,char *fileName);

int UnionGetTableNameFromFileName(char *fileName,char *tableName);

int UnionIsTableDefTBLConnected();

int UnionGetMaxNumOfTableDef();

int UnionConnectTableDefTBL();

int UnionDisconnectTableDefTBL();

int UnionRemoveTableDefTBL();

int UnionReloadTableDefTBL();

int UnionPrintTableDefTBLToFile(FILE *fp);

int UnionPrintTableDefToFile(PUnionTableDef pgunionTableDef,FILE *fp);

int UnionPrintTableFieldDefToFile(PUnionTableFieldDef pgunionTableFieldDef,FILE *fp);

int UnionPrintAllTableDefOfTableNameToFile(char *tableName,FILE *fp);

PUnionTableDef UnionFindTableDef(char *tableName);

PUnionTableDefTBL UnionGetTableDefTBL();

int UnionPrintRecOfTableDefToFile(char *tableName,FILE *fp);

int UnionTransferDBFieldNameToRealFieldName(char *tableName,char *dbFieldName,char *realFileName);

// ��ȡ���б��ֶ��嵥
int UnionReadTableFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList);

// ��ȡ������ʵ���ֶ��嵥
int UnionReadTableRealFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList);

// ��ȡ����������ֶ��嵥
int UnionReadTableVirtualFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList);

// �ж��Ƿ��������ֶ�
int UnionIsVirtualFieldOfTableName(char *tableName,char *fieldName);

// �ж��Ƿ�������
int UnionIsPrimaryKeyFieldOfTableName(char *tableName,char *fieldName);

// �ж��ֶ��Ƿ����
int UnionExistFieldOfTableName(char *tableName,char *fieldName);

// ���ݱ�����ȡ����ʱ��
int UnionReadMethodOfCachedOfTableName(char *tableName);

// ���ɽ������
int UnionGetCreateSQLByTableName(char *tableName,char *dbType,char *buf,int sizeOfBuf);

// �������ݿ�ű�
int UnionGetSQLScript(char *tableName,char *database, char *flag, char *buf,int sizeOfBuf);

int UnionReadTablePrimaryKeyGrpFromImage(char *tableName, char primaryKeyGrp[][64]);

/* 
���ܣ�	����ֶ��Ƿ�Ϊ��
������	tableName[in]		����
	fieldName[in]		�ֶ���
����ֵ��1			��
	0			��
*/
int UnionCheckFieldNameIsTrue(char *tableName,char *fieldName);

/* 
���ܣ�	ת�����ֶΣ����˲���ʾ���ֶ�
������	useFlag[in]		��;��1����ʾ��2���˵�����
	tableList[in]		����
	fieldList[in|out]	�ֶ���
����ֵ��1			��
	0			��
*/
int UnionTransformTableFileList(int useFlag,char *tableList,char *fieldList);

// ���ݱ�����ȡ���ݿ�����λ��
int UnionGetDBIndexByTableName(char *tableName);

#endif
