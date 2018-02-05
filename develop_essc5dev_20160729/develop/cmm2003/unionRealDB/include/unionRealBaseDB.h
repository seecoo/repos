#ifndef _realBaseDB_
#define _realBaseDB_

#define DB_NUM	10

#ifdef _realBaseDB_2_x_

typedef enum
{
	// ��ѯ���ݿ�
	conDBSvrCmdSelect = 10,
	// ��ѯ��������
	conDBSvrCmdSelectCounts = 11,
	// ��ȡ���к�
	conDBSvrCmdGetSequence = 12,
	// ��ѯ��
	conDBSvrCmdSelectTree = 13,
	// ִ��SQL���
	conDBSvrCmdExecSql = 21,
	// ɾ����
	conDBSvrCmdDeleteTree = 22,
}TUnionDBSvrCmd;

#define defLenOfCommand		8

#define defDBSvrCmd_Separate	';'

void UnionSetIsUseHighCached();

void UnionSetIsNotUseHighCached();

/*
���ܣ�	��ѯ���ݿ⣬��ȡ��¼
���������
	sql		sql���
	currentPage	��ǰҳ��1~n
	numOfPerPage	ÿҳ������
���������
	prealDBRecord	��¼�ṹ��
����ֵ��
	С��0	�������
	>=0	��¼��
*/
int UnionSelectRealDBRecord(char *sql,int currentPage,int numOfPerPage);

typedef int (*select_callback)(void*, int, char**, char**);

int UnionSelectRealDBRecord2(char *sql,int currentPage,int numOfPerPage,select_callback callback,void *pArg);

// �������ж�ȡ���������
int UnionGetAddSelfNumBySequence(char *sequenceName);

int UnionSelectRealDBRecordCounts(char *tableName,char *joinCondition,char *condition);

int UnionSelectRealDBTree(int returnSql,char *tableName,char *fieldName,char *parentFieldName,int fieldType,char *rootValue,char *buf,int sizeOfBuf);

int UnionDeleteRealDBTree(char *tableName,char *fieldName,char *parentFieldName,int fieldType,char *rootValue);

int UnionSetIdleDBTime(int seconds);

void UnionCloseIdleDBAlrm();

#else

int UnionSelectRealDBRecord(char *tableName,char *field,char *condition,char *record,int sizeOfRecord,char *fileName);

int UnionInsertRealDBRecord(char *tableName,char *record,int lenOfRecord);

int UnionDeleteRealDBRecord(char *tableName,char *condition);

int UnionUpdateRealDBRecord(char *tableName,char *field,char *condition);

int UnionOpenRealDBSql(char *sql, char *record, int sizeOfRecord);

int UnionSelectDBRecordByRealSQL(char *tableName, char *sql, char *record, int sizeOfRecord, char *fileName);

int UnionSelectRealDBRecordCounts(char *tableName,char *condition);

#endif

/*
���ܣ�	ִ��һ��sql���
���������
	sql		sql���
���������
	��
����ֵ��
	С��0	�������
	>=0	��ȷ
*/
int UnionExecRealDBSql(char *sql);
int UnionExecRealDBSql2(int syncFlag,char *sql);

int UnionDecryptDatabasePassword(char *ciperPassword,char *plainPassword);


// �������ݿ�������Ϊ��֧�����Ӷ�����ݿ�
void UnionSetDatabaseIndex(int index);

int UnionGetDatabaseIndex();

int UnionIsFatalDBErrCode(int dbErrCode);

#endif
