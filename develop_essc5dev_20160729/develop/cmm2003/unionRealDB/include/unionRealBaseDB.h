#ifndef _realBaseDB_
#define _realBaseDB_

#define DB_NUM	10

#ifdef _realBaseDB_2_x_

typedef enum
{
	// 查询数据库
	conDBSvrCmdSelect = 10,
	// 查询数据总数
	conDBSvrCmdSelectCounts = 11,
	// 获取序列号
	conDBSvrCmdGetSequence = 12,
	// 查询树
	conDBSvrCmdSelectTree = 13,
	// 执行SQL语句
	conDBSvrCmdExecSql = 21,
	// 删除树
	conDBSvrCmdDeleteTree = 22,
}TUnionDBSvrCmd;

#define defLenOfCommand		8

#define defDBSvrCmd_Separate	';'

void UnionSetIsUseHighCached();

void UnionSetIsNotUseHighCached();

/*
功能：	查询数据库，获取记录
输入参数：
	sql		sql语句
	currentPage	当前页，1~n
	numOfPerPage	每页的数量
输出参数：
	prealDBRecord	记录结构体
返回值：
	小于0	错误代码
	>=0	记录数
*/
int UnionSelectRealDBRecord(char *sql,int currentPage,int numOfPerPage);

typedef int (*select_callback)(void*, int, char**, char**);

int UnionSelectRealDBRecord2(char *sql,int currentPage,int numOfPerPage,select_callback callback,void *pArg);

// 根据序列读取自增长序号
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
功能：	执行一条sql语句
输入参数：
	sql		sql语句
输出参数：
	无
返回值：
	小于0	错误代码
	>=0	正确
*/
int UnionExecRealDBSql(char *sql);
int UnionExecRealDBSql2(int syncFlag,char *sql);

int UnionDecryptDatabasePassword(char *ciperPassword,char *plainPassword);


// 设置数据库索引，为了支持连接多个数据库
void UnionSetDatabaseIndex(int index);

int UnionGetDatabaseIndex();

int UnionIsFatalDBErrCode(int dbErrCode);

#endif
