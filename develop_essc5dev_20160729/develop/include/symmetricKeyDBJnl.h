#ifndef _SymmetricKeyDBJnl_H_
#define _SymmetricKeyDBJnl_H_

// 定义对称密钥库操作
typedef enum 
{
	conSymmetricKeyDBOperationInsert,
	conSymmetricKeyDBOperationDelete,
	conSymmetricKeyDBOperationUpdate
} TUnionSymmetricKeyDBOperation;

/* 
功能：	登记对称密钥更新日志
参数：	operation[in]		操作标识
	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionInsertSymmetricKeyDBJnl(TUnionSymmetricKeyDBOperation operation,PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	获取登记对称密钥更新日志的SQL语句
参数：	operation[in]		操作标识
	psymmetricKeyDB[in]	密钥容器信息
	sql[out]		sql缓冲区
	sizeOfSql		sql缓冲区大小
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGetSqlForSymmetricKeyDBJnl(TUnionSymmetricKeyDBOperation operation,PUnionSymmetricKeyDB psymmetricKeyDB,char *sql,int sizeOfSql);

#endif

