#ifndef _AsymmetricKeyDBJnl_H_
#define _AsymmetricKeyDBJnl_H_

// 定义对称密钥库操作
typedef enum 
{
	conAsymmetricKeyDBOperationInsert,
	conAsymmetricKeyDBOperationDelete,
	conAsymmetricKeyDBOperationUpdate
} TUnionAsymmetricKeyDBOperation;

/* 
功能：	登记非对称密钥更新日志
参数：	operation[in]		操作标识
	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionInsertAsymmetricKeyDBJnl(TUnionAsymmetricKeyDBOperation operation,PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	获取登记非对称密钥更新日志的SQL语句
参数：	operation[in]		操作标识
	pasymmetricKeyDB[in]	密钥容器信息
	sql[out]		sql缓冲区
	sizeOfSql		sql缓冲区大小
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGetSqlForAsymmetricKeyDBJnl(TUnionAsymmetricKeyDBOperation operation,PUnionAsymmetricKeyDB pasymmetricKeyDB,char *sql,int sizeOfSql);

#endif

